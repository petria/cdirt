#!/usr/bin/env python3
"""Memory and timer regressions in a disposable sanitizer/audit image only."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time

from docker_smoke import IMAGE, available_port, docker_logs, register_player, wait_server
from docker_verb_audit import Control


def main():
    report = Path('/tmp/cdirt-memory-audit/timer.json')
    report.parent.mkdir(parents=True, exist_ok=True)
    folder = Path(tempfile.mkdtemp(prefix='cdirt-memory-timer-'))
    folder.chmod(0o777)
    container = f'cdirt-memory-{os.getpid()}'
    port = available_port()
    players = []
    checks = []
    image = subprocess.check_output(['docker', 'image', 'inspect', '--format', '{{.Id}}', IMAGE], text=True).strip()
    subprocess.run(['docker','run','-d','--name',container,'-p',f'127.0.0.1:{port}:6715',
                    '-v',f'{folder}:/audit','-e','CDIRT_AUDIT_SOCKET=/audit/control.sock',
                    '-e','ASAN_OPTIONS=detect_leaks=0',IMAGE],check=True,capture_output=True)
    try:
        time.sleep(2)
        first = wait_server('127.0.0.1',port,container)
        actor = register_player('127.0.0.1',port,'Rydis',first)
        witness = register_player('127.0.0.1',port,'Observer')
        players.extend([actor,witness])
        control = Control(folder/'control.sock')
        control.request('timers','manual')
        control.request('seed',7)
        for name in ('Rydis','Observer'):
            control.request('fixture','player',name,'room','sherwood4@sherwood')
            control.request('fixture','player',name,'sflag:Color',0)
            control.request('fixture','player',name,'sflag:NewStyle',1)
            control.request('fixture','player',name,'strength',1000)
        output = actor.command('prompt ' + '%n'*30)
        assert 'Rydis'*30 in output, output
        checks.append('full 60-byte name prompt expands to 150 bytes')
        control.request('fixture','player','Rydis','level',90000)
        out = actor.command('setin ' + '%n'*39)
        assert 'wizard' not in out.lower() and 'wrong format' not in out.lower(), out
        out = actor.command('setout ' + '%n'*39)
        assert 'wizard' not in out.lower() and 'wrong format' not in out.lower(), out
        for player in players: player.drain()
        move = actor.command('east')
        out = witness.drain()
        assert 'Rydis'*39 in out, {'move':move,'witness':out}
        checks.append('78-byte travel template expands to 312 marked bytes')
        actor.command('setout')
        actor.command('setout')  # Repeated reset must not double free.
        checks.append('repeated travel-message reset')
        control.request('fixture','player','Rydis','room','sherwood4@sherwood')
        control.request('fixture','player','Rydis','level',10)
        actor.command('prompt ' + '%h'*30)
        zombie = control.request('player','The Zombie')['id']
        rid = control.request('player','Rydis')['id']
        control.request('fixture','player','The Zombie','room','sherwood4@sherwood')
        control.request('fixture','player','The Zombie','strength',1000)
        control.request('fixture','player','The Zombie','fighting',rid)
        control.request('fixture','player','Rydis','fighting',zombie)
        control.request('mark'); control.request('tick',1)
        assert control.request('player','Rydis')['connected']
        actor.command('look')
        checks.append('timer combat rebuilds repeated health prompt')
        for name in ('Rydis','The Zombie'):
            control.request('fixture','player',name,'fighting',-1)
        control.request('fixture','player','The Zombie','room','church2@church')
        control.request('fixture','player','Rydis','duration:lit',2)
        control.request('fixture','player','Rydis','duration:lit',0)
        assert control.request('player','Rydis')['durations']==2
        control.request('mark'); control.request('tick',1)
        receipt = control.request('receipt')
        assert 'Your lit spell expires.\n\r' in receipt['output'].get('Rydis',''), receipt
        assert 'Your lit spell expires.' not in receipt['output'].get('Observer',''), receipt
        assert control.request('player','Rydis')['durations']==1
        control.request('tick',2)
        assert control.request('player','Rydis')['durations']==0
        control.request('fixture','player','Rydis','duration:lit',20)
        control.request('fixture','player','Rydis','duration:wipe',0)
        control.request('fixture','player','Rydis','duration:wipe',0)
        assert control.request('player','Rydis')['durations']==0
        checks.append('spell head expiry, surviving duration, and repeated wipe')
        subprocess.run(['docker','exec','--user','0',container,'sh','-c',
                        'mkdir -p /www && chown mud:mud /www'],check=True,capture_output=True)
        control.request('fixture','player','Rydis','level',90000)
        control.request('fixture','player','Rydis','title','x'*255+'%s')
        actor.command('who')
        actor.command('prompt C: ')
        control.request('tick',180)
        html = subprocess.check_output(['docker','exec',container,'cat','/www/players.html'],text=True)
        assert 'x'*255+'Rydis' in html, html
        assert control.request('player','Rydis')['connected']
        checks.append('180 timer ticks including HTML, weather and regeneration')
        log = docker_logs(container)
        assert 'ERROR: AddressSanitizer' not in log and 'runtime error:' not in log, log
        report.write_text(json.dumps({'passed':True,'image':image,'checks':checks,'log':log},indent=2)+'\n')
        print(f'Memory/timer audit passed: {len(checks)} checks')
    except Exception as exc:
        report.write_text(json.dumps({'passed':False,'image':image,'checks':checks,
                                     'error':str(exc),'log':docker_logs(container)},indent=2)+'\n')
        raise
    finally:
        for player in players: player.close()
        subprocess.run(['docker','rm','-f',container],capture_output=True)
        shutil.rmtree(folder)


if __name__=='__main__': main()
