#
# Build script for Integration Tests
#


#-------------------------------------------------------------------------------
# User-defined part start
#

# BIN_LIB is the destination library for the objects.
BIN_LIB=ILEVATOR

# Library which contains RUTESTCASE service program
RU_LIB=*LIBL

# This library contains the ILEvator service program.
ILEVATOR_LIB=$(BIN_LIB)

OUTPUT=*NONE

DEFINE=RPG_HAS_OVERLOAD

#
# User-defined part end
#-------------------------------------------------------------------------------


OBJECTS = ITGET ITPOST ITDELETE ITEXEC ITHEAD ITMULTI ITOPTIONS ITPUT ITPATCH ITREQHDLR
SRVPGMS = $(RU_LIB)/RUTESTCASE $(ILEVATOR_LIB)/ILEVATOR

# system and application include folder
INCLUDE='/QIBM/include' '../headers/' '../ext/headers'

# RPG compiler parameter
RCFLAGS=INCDIR('.' '../headers' '../ext/headers') DBGVIEW(*source) OUTPUT($(OUTPUT)) STGMDL(*SNGLVL) DEFINE($(DEFINE))

# Bind options
BINDFLAGS=STGMDL(*SNGLVL) BNDSRVPGM($(SRVPGMS)) EXPORT(*ALL) OPTION(*DUPPROC)


all: clean compile ileastic

compile: $(OBJECTS)

ileastic: .PHONY
	$(MAKE) -C ileastic/ $*

start_ileastic: .PHONY
	$(MAKE) -C ileastic/ start

ITGET:
	system -i "CHGATR OBJ('itget.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITGET) SRCSTMF('itget.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITGET MODULE(($(BIN_LIB)/ITGET)) $(BINDFLAGS)"
	
ITPOST:
	system -i "CHGATR OBJ('itpost.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITPOST) SRCSTMF('itpost.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITPOST MODULE(($(BIN_LIB)/ITPOST)) $(BINDFLAGS)"

ITDELETE:
	system -i "CHGATR OBJ('itdelete.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITDELETE) SRCSTMF('itdelete.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITDELETE MODULE(($(BIN_LIB)/ITDELETE)) $(BINDFLAGS)"

ITHEAD:
	system -i "CHGATR OBJ('ithead.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITHEAD) SRCSTMF('ithead.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITHEAD MODULE(($(BIN_LIB)/ITHEAD)) $(BINDFLAGS)"

ITMULTI:
	system -i "CHGATR OBJ('itmulti.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITMULTI) SRCSTMF('itmulti.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITMULTI MODULE(($(BIN_LIB)/ITMULTI)) $(BINDFLAGS)"

ITOPTIONS:
	system -i "CHGATR OBJ('itoptions.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITOPTIONS) SRCSTMF('itoptions.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITOPTIONS MODULE(($(BIN_LIB)/ITOPTIONS)) $(BINDFLAGS)"

ITPUT:
	system -i "CHGATR OBJ('itput.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITPUT) SRCSTMF('itput.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITPUT MODULE(($(BIN_LIB)/ITPUT)) $(BINDFLAGS)"

ITPATCH:
	system -i "CHGATR OBJ('itpatch.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITPATCH) SRCSTMF('itpatch.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITPATCH MODULE(($(BIN_LIB)/ITPATCH)) $(BINDFLAGS)"

ITREQHDLR:
	system -i "CHGATR OBJ('itreqhdlr.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITREQHDLR) SRCSTMF('itreqhdlr.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITREQHDLR MODULE(($(BIN_LIB)/ITREQHDLR)) $(BINDFLAGS)"

ITEXEC:
	system -i "CHGATR OBJ('itexec.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITEXEC) SRCSTMF('itexec.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITEXEC MODULE(($(BIN_LIB)/ITEXEC)) $(BINDFLAGS)"

ITTUNNEL:
	system -i "CHGATR OBJ('ittunnel.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITTUNNEL) SRCSTMF('ittunnel.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITTUNNEL MODULE(($(BIN_LIB)/ITTUNNEL)) $(BINDFLAGS)"

clean:
	-system -i "DLTMOD $(BIN_LIB)/ITGET"
	-system -i "DLTMOD $(BIN_LIB)/ITPOST"
	-system -i "DLTMOD $(BIN_LIB)/ITDELETE"
	-system -i "DLTMOD $(BIN_LIB)/ITEXEC"
	-system -i "DLTMOD $(BIN_LIB)/ITHEAD"
	-system -i "DLTMOD $(BIN_LIB)/ITMULTI"
	-system -i "DLTMOD $(BIN_LIB)/ITOPTIONS"
	-system -i "DLTMOD $(BIN_LIB)/ITPUT"
	-system -i "DLTMOD $(BIN_LIB)/ITPATCH"
	-system -i "DLTMOD $(BIN_LIB)/ITREQHDLR"
	-system -i "DLTMOD $(BIN_LIB)/ITTUNNEL"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITGET"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITPOST"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITDELETE"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITEXEC"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITHEAD"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITMULTI"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITOPTIONS"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITPUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITPATCH"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITREQHDLR"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITTUNNEL"
	$(MAKE) -C ileastic/ clean

.PHONY:
