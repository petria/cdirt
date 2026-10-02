# Utilities makefile

all:	$(BIN)newid $(BIN)pfdel $(BIN)dump \
	$(BIN)dns-serv $(BIN)expire

$(BIN)newid: $(HDIR)/config.h
	@$(ECHO) -n 'Making Newid Utility...'
	@$(CC) -I$(HDIR) -I$(XDIR) -o $(BIN)newid newid.c
	@$(ECHO) 'done'

$(BIN)pfdel: $(HDIR)/config.h
	@$(ECHO) -n 'Making Pfdel Utility...'
	@$(CC) -I$(HDIR) -I$(XDIR) -o $(BIN)pfdel pfdel.c
	@$(ECHO) 'done'

$(BIN)dump: $(HDIR)/kernel.h $(HDIR)/macros.h $(FDIR)/sflags.h $(FDIR)/pflags.h $(HDIR)/config.h $(FDIR)/pflagnames.h
	@$(ECHO) -n 'Making Dump Utility...'
	@$(CC) -I$(HDIR) -I$(XDIR) -I$(FDIR) -o $(BIN)dump dump.c
	@$(ECHO) 'done'

$(BIN)dns-serv: dns-serv.h
	@$(ECHO) -n 'Making DNS Server...'
	@$(CC) -I$(HDIR) -I$(XDIR) -o $(BIN)dns-serv  dns-serv.c
	@$(ECHO) 'done'

$(BIN)expire: $(HDIR)/config.h
	@$(ECHO) -n 'Making Expire Utility...'
	@$(CC) -I$(HDIR) -I$(XDIR) -o $(BIN)expire expire.c
	@$(ECHO) 'done'
