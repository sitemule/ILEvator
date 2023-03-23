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

TARGET_RLS=*CURRENT

OUTPUT=*NONE

#
# User-defined part end
#-----------------------------------------------------------


#
# Do not touch below
#

INCLUDE='headers/' 'ext/headers' '/QIBM/include'

# C compile flags
CCFLAGS=OUTPUT($(OUTPUT)) OPTION(*NOSHOWINC) OPTIMIZE(10) ENUM(*INT) TERASPACE(*YES) STGMDL(*INHERIT) SYSIFCOPT(*IFSIO) INCDIR($(INCLUDE)) DBGVIEW($(DBGVIEW)) DEFINE($(DEFINE)) TGTCCSID($(TARGET_CCSID)) TGTRLS($(TARGET_RLS))
# RPG compile flags
RCFLAGS=OUTPUT($(OUTPUT)) OPTION(*NOUNREF *SRCSTMT) STGMDL(*INHERIT) INCDIR('headers' 'ext/headers') DBGVIEW(*LIST) TGTRLS($(TARGET_RLS)) TGTCCSID($(TARGET_CCSID))

# For current compile:
CCFLAGS2=OPTION(*STDLOGMSG) OUTPUT(*print) $(CCFLAGS)

# remove all default suffix rules
.SUFFIXES:

MODULES=$(BIN_LIB)/ANYCHAR $(BIN_LIB)/API $(BIN_LIB)/BASE64 $(BIN_LIB)/BASICAUTH $(BIN_LIB)/CHUNKED $(BIN_LIB)/ENCODE $(BIN_LIB)/FORM $(BIN_LIB)/HTTPCLIENT $(BIN_LIB)/MESSAGE $(BIN_LIB)/MIME $(BIN_LIB)/REQUEST $(BIN_LIB)/SIMPLELIST $(BIN_LIB)/SOCKETS $(BIN_LIB)/STREAM $(BIN_LIB)/STREAMMEM $(BIN_LIB)/STRUTIL $(BIN_LIB)/TERASPACE $(BIN_LIB)/URL $(BIN_LIB)/VARCHAR  $(BIN_LIB)/XLATE


# Dependency list

all:  $(BIN_LIB).lib ext modules ilevator.srvpgm hdr ilevator.bnd modules.bnd

ext: .PHONY
	$(MAKE) -C ext/ $*

modules: api.c basicauth.rpgmod bearer.rpgmod encode.rpgmod form.rpgmod httpclient.c chunked.c sockets.c anychar.c mime.rpgmod request.rpgmod stream.rpgmod streammem.rpgmod url.rpgmod

#-----------------------------------------------------------

%.lib:
	-system -q "CRTLIB $* TYPE(*TEST)"

%.bnd: 
	-system -q "DLTBNDDIR BNDDIR($(BIN_LIB)/$*)"
	-system -q "CRTBNDDIR BNDDIR($(BIN_LIB)/$*)"

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

%.rpgmod:
	system -q "CHGATR OBJ('src/$*.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/$*) SRCSTMF('src/$*.rpgmod') $(RCFLAGS)"

%.srvpgm:
	-system -q "CRTSRCPF FILE($(BIN_LIB)/QSRVSRC) RCDLEN(200)"
	system "CPYFRMSTMF FROMSTMF('headers/$*.bnd') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSRVSRC.file/$*.mbr') MBROPT(*replace)"
	system -q -kpieb "CRTSRVPGM SRVPGM($(BIN_LIB)/$*) MODULE($(MODULES)) SRCFILE($(BIN_LIB)/QSRVSRC) ACTGRP(QILE) ALWLIBUPD(*YES) DETAIL(*BASIC) TGTRLS($(TARGET_RLS))"

# TODO does not work yet
ilevator.bnd: %.bnd
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/$*) OBJ((*LIBL/ILEVATOR *SRVPGM *IMMED))"

# TODO does not work yet
modules.bnd: %.bnd
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/BASE64 *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/TERASPACE *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STREAM *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STREAMMEM *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/MESSAGE *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/URL *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/SIMPLELIST *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/VARCHAR *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STRUTIL *MODULE))"

hdr:
	-system -q "CRTSRCPF FILE($(BIN_LIB)/QRPGLEREF) RCDLEN(200)"
	-system -q "CRTSRCPF FILE($(BIN_LIB)/H) RCDLEN(200)"
  
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.rpgle') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/QRPGLEREF.FILE/ILEVATOR.MBR') MBROPT(*REPLACE)"
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.h') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/H.FILE/ILEVATOR.MBR') MBROPT(*REPLACE)"

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
	
.PHONY:
