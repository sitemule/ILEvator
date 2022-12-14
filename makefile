
#-----------------------------------------------------------
# User-defined part start
#

# NOTE - UTF is not allowed for ILE source (yet) - so convert to WIN-1252

# BIN_LIB is the destination library for the service program.
# the rpg modules and the binder source file are also created in BIN_LIB.
# binder source file and rpg module can be remove with the clean step (make clean)
BIN_LIB=ILEVATOR
DBGVIEW=*ALL
TARGET_CCSID=*JOB
TARGET_RLS=*PRV


# Do not touch below
INCLUDE='/QIBM/include' 'headers/'  

CCFLAGS=OPTIMIZE(10) ENUM(*INT) TERASPACE(*YES) STGMDL(*INHERIT) SYSIFCOPT(*IFSIO) INCDIR($(INCLUDE)) DBGVIEW($(DBGVIEW)) DEFINE($(DEFINE)) TGTCCSID($(TARGET_CCSID)) TGTRLS($(TARGET_RLS))

# For current compile:
CCFLAGS2=OPTION(*STDLOGMSG) OUTPUT(*print) $(CCFLAGS)

#
# User-defined part end
#-----------------------------------------------------------

# Dependency list

all:  $(BIN_LIB).lib ilevator.srvpgm  hdr

ilevator.srvpgm: api.c  chunked.c xlate.c base64.c sockets.c anychar.c teramem.c simpleList.c varchar.c strUtil.c sndpgmmsg.c ilevator.bnddir
ilevator.bnddir: ilevator.entry ilevator.srvpgm

#-----------------------------------------------------------

%.lib:
	-system -q "CRTLIB $* TYPE(*TEST)"


%.bnddir: 
	-system -q "DLTBNDDIR BNDDIR($(BIN_LIB)/$*)"
	-system -q "CRTBNDDIR BNDDIR($(BIN_LIB)/$*)"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/$*) OBJ($(patsubst %.entry,(*LIBL/% *SRVPGM *IMMED),$^))"

%.entry:
	# Basically do nothing..
	@echo "Adding binding entry $*"

%.c:
	system -q "CHGATR OBJ('src/$*.c') ATR(*CCSID) VALUE(1252)"
	system "CRTCMOD MODULE($(BIN_LIB)/$(notdir $*)) SRCSTMF('src/$*.c') $(CCFLAGS)"

%.cpp:
	system -q "CHGATR OBJ('src/$*.cpp') ATR(*CCSID) VALUE(1252)"
	system "CRTCPPMOD MODULE($(BIN_LIB)/$(notdir $*)) SRCSTMF('src/$*.cpp') $(CCFLAGS)"

%.clle:
	system -q "CHGATR OBJ('src/$*.clle') ATR(*CCSID) VALUE(1252)"
	-system -q "CRTSRCPF FILE($(BIN_LIB)/QCLLESRC) RCDLEN(200)"
	system "CPYFRMSTMF FROMSTMF('src/$*.clle') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QCLLESRC.file/$(notdir $*).mbr') MBROPT(*ADD)"
	system "CRTCLMOD MODULE($(BIN_LIB)/$(notdir $*)) SRCFILE($(BIN_LIB)/QCLLESRC) DBGVIEW($(DBGVIEW)) TGTRLS($(TARGET_RLS))"

%.srvpgm:

	-system -q "CRTSRCPF FILE($(BIN_LIB)/QSRVSRC) RCDLEN(200)"
	system "CPYFRMSTMF FROMSTMF('headers/$*.bnddir') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSRVSRC.file/$*.mbr') MBROPT(*replace)"
	
	# You may be wondering what this ugly string is. It's a list of objects created from the dep list that end with .c or .clle.
	$(eval modules := $(patsubst %,$(BIN_LIB)/%,$(basename $(filter %.c %.cpp %.clle,$(notdir $^)))))
	
	system -q -kpieb "CRTSRVPGM SRVPGM($(BIN_LIB)/$*) MODULE($(modules)) SRCFILE($(BIN_LIB)/QSRVSRC) ACTGRP(QILE) ALWLIBUPD(*YES) DETAIL(*BASIC) TGTRLS($(TARGET_RLS))"


hdr:

	-system -q "CRTSRCPF FILE($(BIN_LIB)/QRPGLEREF) RCDLEN(200)"
	-system -q "CRTSRCPF FILE($(BIN_LIB)/H) RCDLEN(200)"
  
	system "CPYFRMSTMF FROMSTMF('headers/ILEVATOR.rpgle') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QRPGLEREF.file/ILEVATOR.mbr') MBROPT(*REPLACE)"
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.h') TOMBR('/QSYS.lib/$(BIN_LIB).lib/H.file/ilevator.mbr') MBROPT(*REPLACE)"

all:
	@echo Build success!

clean:
	-system -q "DLTOBJ OBJ($(BIN_LIB)/*ALL) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/EVFEVENT)  OBJTYPE(*file)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/RELEASE)   OBJTYPE(*file)"

	
release: clean
	@echo " -- Creating ilevator release. --"
	@echo " -- Creating save file. --"
	system "CRTSAVF FILE($(BIN_LIB)/RELEASE)"
	system "SAVLIB LIB($(BIN_LIB)) DEV(*SAVF) SAVF($(BIN_LIB)/RELEASE) DTACPR(*HIGH) OMITOBJ((RELEASE *FILE))"
	-mkdir -p release
	-rm ./release/release.savf
	system "CPYTOSTMF FROMMBR('/QSYS.lib/$(BIN_LIB).lib/RELEASE.FILE') TOSTMF('./release/release.savf') STMFOPT(*REPLACE) STMFCCSID(1252) CVTDTA(*NONE)"
	@echo " -- Cleaning up... --"
	system "DLTOBJ OBJ($(BIN_LIB)/RELEASE) OBJTYPE(*FILE)"
	@echo " -- Release created! --"
	@echo ""
	@echo "To install the release, run:"
	@echo "  > CRTLIB $(BIN_LIB)"
	@echo "  > CPYFRMSTMF FROMSTMF('./release/release.savf') TOMBR('/QSYS.lib/$(BIN_LIB).lib/RELEASE.FILE') MBROPT(*REPLACE) CVTDTA(*NONE)"
	@echo "  > RSTLIB SAVLIB($(BIN_LIB)) DEV(*SAVF) SAVF($(BIN_LIB)/RELEASE)"
	@echo ""

# For vsCode / single file then i.e.: gmake current sqlio.c  
ifeq "$(suffix $(SRC))" ".c"
current:
	system -i "CRTCMOD MODULE($(BIN_LIB)/$(basename $(notdir $(SRC)))) SRCSTMF('$(SRC)') $(CCFLAGS2) "
	system -i "UPDSRVPGM SRVPGM($(BIN_LIB)/ilevator) MODULE($(BIN_LIB)/*ALL)"  
endif

ifeq "$(suffix $(SRC))" ".cpp"
current:
	system "CRTCPPMOD MODULE($(BIN_LIB)/$(basename $(notdir $(SRC)))) SRCSTMF('$(SRC)') $(CCFLAGS2) "
	system "UPDSRVPGM SRVPGM($(BIN_LIB)/ilevator) MODULE($(BIN_LIB)/*ALL)"  
endif


# For vsCode / single file then i.e.: gmake current sqlio.c  
example: 
	system -i "CRTBNDRPG PGM($(BIN_LIB)/$(SRC)) SRCSTMF('examples/$(SRC).rpgle') DBGVIEW(*ALL)" > error.txt

test: 
	system -i "CRTBNDRPG PGM($(BIN_LIB)/$(SRC)) SRCSTMF('test/$(SRC).rpgle') DBGVIEW(*ALL)" > error.txt