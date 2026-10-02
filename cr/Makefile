# C-dirt Makefile

CR   = cr.a 

CROBJS = fight.o edit.o mail.o muserscom.o \
	hostscom.o uaf.o tiecom.o untiecom.o actions.o\
	throwcom.o digcom.o configure.o bprintf.o \
	weather.o vote.o pig.o splotch.o idlookup.o noswear.o mudfd.o \
        store.o score.o client.o level.o bong.o dns-lookup.o flags.o \
	update.o

.c.o:
	@$(ECHO) 'Compiling $*.c...'
	@$(CC) -c $(CFLAGS) $(DFLAGS) $*.c

all: $(CR)

$(CR): $(CROBJS)
	ar rcs $(CR) $(CROBJS)


# Do not delete this line, the makedep program requires it. #
# Last Updated: Fri Jul 30 00:36:34 EST 1999                #
#############################################################

# Dependencies for the MUD source code
#
actions.o: actions.c /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/sendsys.h ../cr_inc/bprintf.h ../include/objects.h \
 ../include/mobiles.h ../cr_inc/actions.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/sflags.h ../include/objsys.h \
 ../include/zones.h ../include/parse.h ../include/rooms.h \
 ../cr_inc/fight.h ../specials/wavecom.h
bong.o: bong.c ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/sendsys.h \
 ../cr_inc/bprintf.h ../include/mobile.h /usr/include/bits/time.h \
 /usr/include/stdlib.h /usr/include/alloca.h ../include/mud.h
bprintf.o: bprintf.c \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/sys/file.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/fcntl.h /usr/include/bits/fcntl.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/stdlib.h /usr/include/alloca.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../cr_inc/sflags.h \
 ../include/mobile.h /usr/include/bits/time.h ../include/mud.h \
 ../include/rooms.h ../cr_inc/bprintf.h
client.o: client.c /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/sys/time.h \
 /usr/include/bits/time.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
 /usr/include/netdb.h /usr/include/rpc/netdb.h \
 /usr/include/sys/socket.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/crypt.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/mobile.h \
 ../cr_inc/sflags.h ../include/sendsys.h ../include/mud.h \
 ../include/parse.h ../cr_inc/bprintf.h ../include/commands.h \
 ../include/verbs.h ../include/timing.h ../include/client.h \
 ../include/client_int.h
configure.o: configure.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../include/kernel.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/commands.h \
 ../include/wizard.h ../include/mud.h ../include/uaf.h \
 ../cr_inc/mail.h /usr/include/bits/time.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h ../include/timing.h ../cr_inc/sflags.h \
 ../include/sendsys.h ../include/parse.h ../include/mobile.h \
 /usr/include/sys/file.h /usr/include/fcntl.h \
 /usr/include/bits/fcntl.h ../cr_inc/bprintf.h ../include/change.h \
 ../cr_inc/edit.h ../include/frob.h ../include/verbs.h
digcom.o: digcom.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 ../include/kernel.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/bprintf.h ../include/parse.h \
 ../include/uaf.h ../include/clone.h ../include/acct.h \
 ../include/mud.h
dns-lookup.o: dns-lookup.c /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/sys/sysmacros.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h /usr/include/sys/socket.h \
 /usr/include/bits/socket.h /usr/include/bits/sockaddr.h \
 /usr/include/asm/socket.h /usr/include/asm/sockios.h \
 /usr/include/netinet/in.h /usr/include/stdint.h \
 /usr/include/bits/in.h /usr/include/bits/byteswap.h \
 /usr/include/arpa/inet.h /usr/include/netdb.h \
 /usr/include/rpc/netdb.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/ctype.h /usr/include/string.h \
 ../include/MACHINE.H ../include/config.h ../include/levels.h \
 ../include/exits.h ../include/types.h ../include/utils.h \
 ../flags/flags.h ../flags/sflags.h ../flags/nflags.h \
 ../flags/eflags.h ../flags/pflags.h ../flags/aflags.h \
 ../flags/oflags.h ../flags/lflags.h ../flags/mflags.h \
 ../include/quests.h ../flags/cflags.h ../include/log.h \
 ../include/mudtypes.h ../include/xrweather.h ../cr_inc/store.h \
 ../include/mudmacros.h ../include/macros.h ../include/extern.h \
 ../include/files.h
edit.o: edit.c /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../cr_inc/edit.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/sys/file.h \
 /usr/include/fcntl.h /usr/include/bits/fcntl.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../cr_inc/mail.h \
 /usr/include/bits/time.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h ../include/timing.h ../cr_inc/sflags.h \
 ../include/sendsys.h ../include/parse.h ../include/uaf.h \
 ../include/mobile.h ../cr_inc/bprintf.h ../include/mud.h \
 ../include/change.h
expire.o: expire.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/dirent.h /usr/include/bits/dirent.h \
 /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h \
 /usr/include/linux/limits.h /usr/include/time.h \
 /usr/include/bits/time.h /usr/include/sys/stat.h \
 /usr/include/bits/stat.h ../include/config.h
fight.o: fight.c ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/sflags.h ../include/sendsys.h \
 /usr/include/stdlib.h /usr/include/alloca.h ../include/mud.h \
 ../include/group.h ../cr_inc/bprintf.h ../include/parse.h \
 ../include/commands.h ../include/rooms.h ../include/uaf.h \
 ../include/mobiles.h ../include/objects.h ../include/locations.h \
 ../include/verbs.h ../cr_inc/oflags.h ../cr_inc/fight.h \
 ../cr_inc/fightmsg.h ../include/move.h ../include/objsys.h \
 ../include/clone.h ../include/spell.h ../specials/breakitem.h \
 ../specials/hitchance.h ../specials/damage.h ../specials/mobdeath.h \
 ../specials/wearcom.h ../specials/killmob.h ../specials/killplr.h
flags.o: flags.c ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../cr_inc/oflags.h \
 ../cr_inc/sflags.h ../include/verbs.h ../include/objsys.h \
 ../include/parse.h ../flags/flagindex.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/bprintf.h ../include/rooms.h \
 ../include/uaf.h
hostscom.o: hostscom.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/bits/time.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 ../cr_inc/bprintf.h ../include/parse.h ../include/uaf.h \
 ../include/clone.h ../include/mud.h ../include/timing.h
idlookup.o: idlookup.c ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../cr_inc/idlookup.h \
 /usr/include/errno.h /usr/include/bits/errno.h \
 /usr/include/linux/errno.h /usr/include/asm/errno.h \
 /usr/include/sys/time.h /usr/include/bits/time.h \
 /usr/include/sys/socket.h /usr/include/arpa/inet.h \
 /usr/include/netdb.h /usr/include/rpc/netdb.h /usr/include/fcntl.h \
 /usr/include/bits/fcntl.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h ../cr_inc/bprintf.h ../include/mud.h \
 /usr/include/stdlib.h /usr/include/alloca.h
level.o: level.c ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/sendsys.h \
 ../include/wizlist.h ../cr_inc/bprintf.h ../cr_inc/level.h \
 ../include/mobile.h /usr/include/bits/time.h ../include/levelnames.h
mail.o: mail.c /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../cr_inc/edit.h \
 /usr/include/sys/file.h /usr/include/fcntl.h \
 /usr/include/bits/fcntl.h ../cr_inc/mail.h /usr/include/bits/time.h \
 /usr/include/unistd.h /usr/include/bits/posix_opt.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 ../include/timing.h ../cr_inc/sflags.h ../include/sendsys.h \
 ../include/parse.h ../include/uaf.h ../include/mobile.h \
 ../cr_inc/bprintf.h ../include/mud.h ../include/change.h \
 /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h \
 /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h \
 /usr/include/bits/ioctl-types.h /usr/include/sys/ttydefaults.h \
 /usr/include/errno.h /usr/include/bits/errno.h \
 /usr/include/linux/errno.h /usr/include/asm/errno.h
mudfd.o: mudfd.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/sys/sysmacros.h /usr/include/fcntl.h \
 /usr/include/bits/fcntl.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h /usr/include/malloc.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/mud.h
muserscom.o: muserscom.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/bits/time.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 ../cr_inc/bprintf.h ../include/parse.h ../include/uaf.h \
 ../include/clone.h ../include/mud.h ../include/coders.h \
 ../include/timing.h
noswear.o: noswear.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/string.h /usr/include/ctype.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../cr_inc/sflags.h \
 ../cr_inc/bprintf.h
pig.o: pig.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/string.h ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/ctype.h \
 ../include/MACHINE.H ../include/config.h ../include/levels.h \
 ../include/exits.h ../include/types.h ../include/utils.h \
 ../flags/flags.h ../flags/sflags.h ../flags/nflags.h \
 ../flags/eflags.h ../flags/pflags.h ../flags/aflags.h \
 ../flags/oflags.h ../flags/lflags.h ../flags/mflags.h \
 ../include/quests.h ../flags/cflags.h ../include/log.h \
 ../include/mudtypes.h ../include/xrweather.h ../cr_inc/store.h \
 ../include/mudmacros.h ../include/macros.h ../include/extern.h \
 ../include/files.h ../cr_inc/sflags.h ../include/sendsys.h \
 ../include/mobile.h /usr/include/bits/time.h ../cr_inc/bprintf.h \
 ../cr_inc/pig.h
score.o: score.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 ../include/kernel.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/bprintf.h ../include/parse.h \
 ../include/uaf.h ../include/clone.h ../include/mud.h \
 ../cr_inc/xrscore.h ../cr_inc/smess.h ../cr_inc/fight.h
splotch.o: splotch.c /usr/include/stdio.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/string.h /usr/include/ctype.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h
store.o: store.c ../include/kernel.h /usr/include/sys/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/objsys.h ../cr_inc/bprintf.h \
 ../include/parse.h ../include/clone.h
throwcom.o: throwcom.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 ../include/kernel.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/bprintf.h ../include/parse.h \
 ../include/uaf.h ../include/clone.h ../include/acct.h \
 ../include/mud.h
tiecom.o: tiecom.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 ../include/kernel.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/bprintf.h ../include/parse.h \
 ../include/uaf.h ../include/clone.h ../include/acct.h \
 ../include/mud.h
uaf.o: uaf.c /usr/include/sys/file.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/fcntl.h /usr/include/bits/fcntl.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/unistd.h /usr/include/bits/posix_opt.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/bits/time.h \
 /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/errno.h \
 /usr/include/bits/errno.h /usr/include/linux/errno.h \
 /usr/include/asm/errno.h ../flags/pflags.h ../include/kernel.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/aflags.h \
 ../flags/oflags.h ../flags/lflags.h ../flags/mflags.h \
 ../include/quests.h ../flags/cflags.h ../include/log.h \
 ../include/mudtypes.h ../include/xrweather.h ../cr_inc/store.h \
 ../include/mudmacros.h ../include/macros.h ../include/extern.h \
 ../include/files.h ../cr_inc/sflags.h ../include/clone.h \
 ../include/uaf.h ../include/mobile.h ../include/exit.h \
 ../include/rooms.h ../include/sendsys.h ../include/mud.h \
 ../cr_inc/bprintf.h ../include/wizlist.h ../include/parse.h \
 ../include/locations.h /usr/include/stdlib.h /usr/include/alloca.h \
 ../include/objsys.h ../cr_inc/fight.h
untiecom.o: untiecom.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 ../include/kernel.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/locations.h \
 ../include/objects.h ../include/mobiles.h ../cr_inc/sflags.h \
 ../cr_inc/oflags.h ../include/sendsys.h ../include/commands.h \
 ../include/rooms.h ../include/objsys.h ../include/mobile.h \
 /usr/include/bits/time.h ../cr_inc/bprintf.h ../include/parse.h \
 ../include/uaf.h ../include/clone.h ../include/acct.h \
 ../include/mud.h
update.o: update.c /usr/include/unistd.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/types.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 /usr/include/signal.h /usr/include/bits/sigset.h \
 /usr/include/bits/signum.h /usr/include/time.h \
 /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h \
 /usr/include/bits/sigcontext.h /usr/include/asm/sigcontext.h \
 /usr/include/bits/sigstack.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/zones.h \
 ../cr_inc/update.h ../include/sendsys.h ../include/mud.h \
 ../cr_inc/bprintf.h ../include/spell.h ../include/exit.h \
 ../include/rooms.h ../include/clone.h ../include/mobile.h \
 /usr/include/bits/time.h ../include/client.h ../cr_inc/sflags.h \
 ../include/verbs.h ../include/uaf.h ../include/objsys.h \
 ../include/god.h
vote.o: vote.c ../cr_inc/vote.h /usr/include/stdio.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h /usr/include/malloc.h \
 ../include/parse.h ../cr_inc/bprintf.h ../include/mobile.h \
 /usr/include/bits/time.h ../include/mud.h
weather.o: weather.c /usr/include/stdlib.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h \
 /usr/include/sys/types.h /usr/include/bits/types.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/bits/time.h /usr/include/math.h \
 /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h \
 /usr/include/bits/mathcalls.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/float.h \
 ../include/kernel.h /usr/include/sys/param.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/linux/param.h \
 /usr/include/asm/param.h /usr/include/netinet/in.h \
 /usr/include/stdint.h /usr/include/bits/socket.h \
 /usr/include/bits/sockaddr.h /usr/include/asm/socket.h \
 /usr/include/asm/sockios.h /usr/include/bits/in.h \
 /usr/include/bits/byteswap.h /usr/include/stdio.h \
 /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stdarg.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/include/bits/stdio_lim.h /usr/include/ctype.h \
 /usr/include/string.h ../include/MACHINE.H ../include/config.h \
 ../include/levels.h ../include/exits.h ../include/types.h \
 ../include/utils.h ../flags/flags.h ../flags/sflags.h \
 ../flags/nflags.h ../flags/eflags.h ../flags/pflags.h \
 ../flags/aflags.h ../flags/oflags.h ../flags/lflags.h \
 ../flags/mflags.h ../include/quests.h ../flags/cflags.h \
 ../include/log.h ../include/mudtypes.h ../include/xrweather.h \
 ../cr_inc/store.h ../include/mudmacros.h ../include/macros.h \
 ../include/extern.h ../include/files.h ../include/sendsys.h \
 ../include/climate.h ../cr_inc/sflags.h ../cr_inc/bprintf.h \
 ../include/parse.h ../include/rooms.h ../include/xrweathermsgs.h
