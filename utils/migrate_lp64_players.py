#!/usr/bin/env python3
"""Recover player records written by the old little-endian LP64 long* saver.

Run with the MUD stopped. Dry run is the default. This is a migration of a
known legacy format, not a relaxation of the daemon's typed record validation.
"""
import argparse
import os
from pathlib import Path
import shutil
import tempfile


INT_FIELDS = {
    b"PLines", b"CarryCap", b"Strength", b"Damage", b"Armor",
    b"Visibility", b"Level", b"Wimpy", b"Magic", b"Channel",
    b"Killed", b"Died", b"Coins", b"Class",
}


def recover(data):
    lines = []
    changes = []
    for line in data.splitlines(keepends=True):
        token = line[:11].strip()
        if token in INT_FIELDS or token == b"Score":
            number = int(line[11:].strip())
            if not -(1 << 63) <= number < (1 << 63):
                raise ValueError("numeric value outside legacy signed long range")
            corrected = number & 0xffffffff
            if token in INT_FIELDS and corrected >= (1 << 31):
                corrected -= 1 << 32
            if corrected != number:
                ending = b"\r\n" if line.endswith(b"\r\n") else b"\n" if line.endswith(b"\n") else b""
                lines.append(line[:11] + str(corrected).encode("ascii") + ending)
                changes.append((token.decode("ascii"), number, corrected))
                continue
        lines.append(line)
    return b"".join(lines), changes


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("players", type=Path, help="players directory containing A/Name ... Z/Name")
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--legacy-lp64", action="store_true", help="confirm these records came from the old little-endian 64-bit saver")
    parser.add_argument("--backup-dir", type=Path)
    args = parser.parse_args()
    if args.apply and (not args.legacy_lp64 or args.backup_dir is None):
        parser.error("--apply requires --legacy-lp64 and a fresh --backup-dir")
    if not args.players.is_dir():
        parser.error("players directory does not exist")

    # Validate all candidates before changing anything. Never print other fields,
    # which include password hashes and user-defined strings.
    pending = []
    for initial in "ABCDEFGHIJKLMNOPQRSTUVWXYZ":
        for path in sorted((args.players / initial).glob("*")):
            if path.is_symlink() or not path.is_file():
                continue
            original = path.read_bytes()
            corrected, changes = recover(original)
            if changes:
                pending.append((path, original, corrected, changes))
    if args.apply:
        args.backup_dir.mkdir(parents=True, mode=0o700, exist_ok=False)
        for path, original, corrected, changes in pending:
            backup = args.backup_dir / path.relative_to(args.players)
            backup.parent.mkdir(mode=0o700, exist_ok=True)
            shutil.copy2(path, backup)

    for path, original, corrected, changes in pending:
        print(path.name + ": " + "; ".join(f"{key} {old} -> {new}" for key, old, new in changes))
        if args.apply:
            if path.read_bytes() != original:
                raise RuntimeError("record changed during migration; stop the MUD first")
            stat = path.stat()
            fd, temporary = tempfile.mkstemp(prefix=".lp64-", dir=path.parent)
            try:
                with os.fdopen(fd, "wb") as output:
                    output.write(corrected)
                    output.flush()
                    os.fchmod(output.fileno(), stat.st_mode & 0o7777)
                    if os.geteuid() == 0:
                        os.fchown(output.fileno(), stat.st_uid, stat.st_gid)
                    os.fsync(output.fileno())
                os.replace(temporary, path)
            finally:
                if os.path.exists(temporary):
                    os.unlink(temporary)
    print(f"{'Migrated' if args.apply else 'Would migrate'} {len(pending)} records")


if __name__ == "__main__":
    main()
