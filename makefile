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

## OS version dependent defines
OS_VERSION=$(shell uname -v).$(shell uname -r)
ifeq ($(shell expr $(OS_VERSION) \>= 7.3), 1)
DEFINE=RPG_HAS_OVERLOAD
TGTCCSID=TGTCCSID($(TARGET_CCSID))
endif


INCLUDE='headers/' 'ext/headers' '/QIBM/include'

# C compile flags
CCFLAGS=OUTPUT($(OUTPUT)) OPTION(*NOSHOWINC *STDLOGMSG) OPTIMIZE(10) ENUM(*INT) TERASPACE(*YES) STGMDL(*INHERIT) SYSIFCOPT(*IFSIO) INCDIR($(INCLUDE)) DBGVIEW($(DBGVIEW)) DEFINE($(DEFINE)) TGTCCSID($(TARGET_CCSID)) TGTRLS($(TARGET_RLS))
# RPG compile flags
RCFLAGS=OUTPUT($(OUTPUT)) OPTION(*NOUNREF *SRCSTMT) STGMDL(*INHERIT) INCDIR('headers' 'ext/headers') DBGVIEW(*LIST) TGTRLS($(TARGET_RLS)) DEFINE($(DEFINE)) $(TGTCCSID)
RCFLAGS2=OUTPUT(*PRINT) OPTION(*NOUNREF *SRCSTMT) STGMDL(*INHERIT) INCDIR('headers' 'ext/headers') DBGVIEW(*LIST) TGTRLS($(TARGET_RLS)) DEFINE($(DEFINE)) $(TGTCCSID)

# For current compile:
CCFLAGS2=OUTPUT(*print) OPTION(*NOSHOWINC *STDLOGMSG) OPTIMIZE(10) ENUM(*INT) TERASPACE(*YES) STGMDL(*INHERIT) SYSIFCOPT(*IFSIO) INCDIR($(INCLUDE)) DBGVIEW($(DBGVIEW)) DEFINE($(DEFINE)) TGTCCSID($(TARGET_CCSID)) TGTRLS($(TARGET_RLS))

# remove all default suffix rules
.SUFFIXES:

MODULES=$(BIN_LIB)/ANYCHAR $(BIN_LIB)/API $(BIN_LIB)/BASE64 $(BIN_LIB)/BASICAUTH $(BIN_LIB)/BEARER $(BIN_LIB)/CHUNKED $(BIN_LIB)/DEBUG $(BIN_LIB)/ENCODE $(BIN_LIB)/FORM $(BIN_LIB)/HTTPCLIENT $(BIN_LIB)/INIT $(BIN_LIB)/MESSAGE $(BIN_LIB)/MIME $(BIN_LIB)/REQUEST $(BIN_LIB)/SIMPLELIST $(BIN_LIB)/SOCKETS $(BIN_LIB)/STREAM $(BIN_LIB)/STRUTIL $(BIN_LIB)/TERASPACE $(BIN_LIB)/URL $(BIN_LIB)/VARCHAR  $(BIN_LIB)/XLATE


# Dependency list

all:  $(BIN_LIB).lib ext modules ilevator.srvpgm hdr ilevator.bnd modules.bnd

ext: .PHONY
	$(MAKE) -C ext/ $*

modules: anychar.c api.rpgmod basicauth.rpgmod bearer.rpgmod chunked.c debug.rpgmod \
         encode.rpgmod form.rpgmod httpclient.c init.cpp mime.rpgmod request.rpgmod \
         sockets.c url.rpgmod

#-----------------------------------------------------------

%.lib:
	-system -q "CRTLIB $* TYPE(*TEST)"

%.bnd: 
	-system -q "DLTBNDDIR BNDDIR($(BIN_LIB)/$*)"
	system -q "CRTBNDDIR BNDDIR($(BIN_LIB)/$*)"

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

ilevator.bnd:
	-system -q "DLTBNDDIR BNDDIR($(BIN_LIB)/ILEVATOR)"
	system -q "CRTBNDDIR BNDDIR($(BIN_LIB)/ILEVATOR)"
	system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/ILEVATOR) OBJ(($(BIN_LIB)/ILEVATOR *SRVPGM *IMMED))"

modules.bnd:
	-system -q "DLTBNDDIR BNDDIR($(BIN_LIB)/MODULES)"
	system -q "CRTBNDDIR BNDDIR($(BIN_LIB)/MODULES)"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/ANYCHAR *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/API *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/BASE64 *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/BASICAUTH *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/BEARER *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/CHUNKED *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/DEBUG *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/ENCODE *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/FORM *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/HTTPCLIENT *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/MIME *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/REQUEST *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/SOCKETS *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STREAM *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STRUTIL *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/SIMPLELIST *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/TERASPACE *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/URL *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/VARCHAR *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/XLATE *MODULE))"

hdr:
	-system -q "CRTSRCPF FILE($(BIN_LIB)/QRPGLEREF) RCDLEN(200)"
	-system -q "CRTSRCPF FILE($(BIN_LIB)/H) RCDLEN(200)"
  
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.rpgle') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/QRPGLEREF.FILE/ILEVATOR.MBR') MBROPT(*REPLACE)"
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.h') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/H.FILE/ILEVATOR.MBR') MBROPT(*REPLACE)"

all:
	@echo Build success!

clean:
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ANYCHAR) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/API) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/BASICAUTH) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/BEARER) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/CHUNKED) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/DEBUG) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ENCODE) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/FORM) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/HTTPCLIENT) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/INIT) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/MIME) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/REQUEST) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/SOCKETS) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/URL) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ILEVATOR) OBJTYPE(*SRVPGM)"

purge: clean
	-system -q "DLTOBJ OBJ($(BIN_LIB)/H) OBJTYPE(*FILE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/QRPGLEREF) OBJTYPE(*FILE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/QSRVSRC) OBJTYPE(*FILE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ILEVATOR) OBJTYPE(*BNDDIR)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/MODULES) OBJTYPE(*BNDDIR)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/EVFEVENT)  OBJTYPE(*file)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/RELEASE)   OBJTYPE(*file)"
	$(MAKE) -C ext/ clean
	$(MAKE) -C integrationtests/ clean
	$(MAKE) -C unittests/ clean

unittests: .PHONY
	$(MAKE) -C unittests/ $*

itests: .PHONY
	$(MAKE) -C integrationtests/ $*
	
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

ifeq "$(suffix $(SRC))" ".rpgmod"
current:
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/$(basename $(notdir $(SRC)))) SRCSTMF('$(SRC)') $(RCFLAGS2) "
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
