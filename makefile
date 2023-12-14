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
ACTGRP=QILE


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

MODULES=$(BIN_LIB)/ANYCHAR $(BIN_LIB)/API $(BIN_LIB)/BASE64 $(BIN_LIB)/BASICAUTH $(BIN_LIB)/BEARER $(BIN_LIB)/CHUNKED $(BIN_LIB)/DEBUG $(BIN_LIB)/ENCODE $(BIN_LIB)/FORM $(BIN_LIB)/HTTPCLIENT $(BIN_LIB)/INIT $(BIN_LIB)/JOBLOG $(BIN_LIB)/MESSAGE $(BIN_LIB)/MIME $(BIN_LIB)/MULTIPART $(BIN_LIB)/REQUEST $(BIN_LIB)/SIMPLELIST $(BIN_LIB)/SOCKETS $(BIN_LIB)/STREAM $(BIN_LIB)/STRUTIL $(BIN_LIB)/TERASPACE $(BIN_LIB)/URL $(BIN_LIB)/VARCHAR  $(BIN_LIB)/XLATE


# Dependency list

all:  $(BIN_LIB).lib ext compile hdr messagefile ilevator.bnd modules.bnd

ext: .PHONY
	$(MAKE) -C ext/ $*

modules: anychar.c api.rpgmod basicauth.rpgmod bearer.rpgmod chunked.c debug.rpgmod \
         encode.rpgmod form.rpgmod httpclient.c init.cpp joblog.c mime.rpgmod \
         multipart.rpgmod request.rpgmod sockets.c url.rpgmod

compile: setHeaderCcsid modules ilevator.srvpgm

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
	system -q -kpieb "CRTSRVPGM SRVPGM($(BIN_LIB)/$*) MODULE($(MODULES)) SRCFILE($(BIN_LIB)/QSRVSRC) ACTGRP($(ACTGRP)) ALWLIBUPD(*YES) BNDSRVPGM(QICU/QXICUUC40) DETAIL(*BASIC) TGTRLS($(TARGET_RLS))"

ilevator.bnd:
	-system -q "DLTBNDDIR BNDDIR($(BIN_LIB)/ILEVATOR)"
	system -q "CRTBNDDIR BNDDIR($(BIN_LIB)/ILEVATOR)"
	system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/ILEVATOR) OBJ((*LIBL/ILEVATOR *SRVPGM *IMMED))"

## The MODULES binddir is only used for unittest so unexposed features can be tested individually.
## However, made available in this general makefile.
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
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/JOBLOG *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/MESSAGE *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/MIME *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/MULTIPART *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/REQUEST *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/SOCKETS *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STREAM *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/STRUTIL *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/SIMPLELIST *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/TERASPACE *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/URL *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/VARCHAR *MODULE))"
	-system -q "ADDBNDDIRE BNDDIR($(BIN_LIB)/MODULES) OBJ(($(BIN_LIB)/XLATE *MODULE))"

setHeaderCcsid:
	setccsid 1252 headers/*

hdr:
	-system -q "CRTSRCPF FILE($(BIN_LIB)/QRPGLEREF) RCDLEN(200)"
	-system -q "CRTSRCPF FILE($(BIN_LIB)/H) RCDLEN(200)"	
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.rpgle') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/QRPGLEREF.FILE/ILEVATOR.MBR') MBROPT(*REPLACE)"
	system "CPYFRMSTMF FROMSTMF('headers/basicauth.rpginc') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/QRPGLEREF.FILE/BASICAUTH.MBR') MBROPT(*REPLACE)"
	system "CPYFRMSTMF FROMSTMF('headers/ilevator.h') TOMBR('/QSYS.LIB/$(BIN_LIB).LIB/H.FILE/ILEVATOR.MBR') MBROPT(*REPLACE)"

messagefile:
	-system "DLTMSGF MSGF($(BIN_LIB)/ILEVATOR)"
	system "CRTMSGF MSGF($(BIN_LIB)/ILEVATOR) TEXT('ILEvator : HTTP status messages')"
	system "QSYS/ADDMSGD MSGID(ILV0400) MSGF($(BIN_LIB)/ILEVATOR) MSG('400 - Bad request') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0401) MSGF($(BIN_LIB)/ILEVATOR) MSG('401 - Unauthorized') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0402) MSGF($(BIN_LIB)/ILEVATOR) MSG('402 - Payment required') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0403) MSGF($(BIN_LIB)/ILEVATOR) MSG('403 - Forbidden') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0404) MSGF($(BIN_LIB)/ILEVATOR) MSG('404 - Not found') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0405) MSGF($(BIN_LIB)/ILEVATOR) MSG('405 - Method not allowed') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0406) MSGF($(BIN_LIB)/ILEVATOR) MSG('406 - Not acceptable') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0407) MSGF($(BIN_LIB)/ILEVATOR) MSG('407 - Proxy authentication required') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0408) MSGF($(BIN_LIB)/ILEVATOR) MSG('408 - Request timeout') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0409) MSGF($(BIN_LIB)/ILEVATOR) MSG('409 - Conflict') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0410) MSGF($(BIN_LIB)/ILEVATOR) MSG('410 - Gone') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0411) MSGF($(BIN_LIB)/ILEVATOR) MSG('411 - Length required') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0412) MSGF($(BIN_LIB)/ILEVATOR) MSG('412 - Precondition failed') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0413) MSGF($(BIN_LIB)/ILEVATOR) MSG('413 - Payload too large') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0414) MSGF($(BIN_LIB)/ILEVATOR) MSG('414 - URI too long') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0415) MSGF($(BIN_LIB)/ILEVATOR) MSG('415 - Unsupported media type') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0416) MSGF($(BIN_LIB)/ILEVATOR) MSG('416 - Range not satisfiable') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0417) MSGF($(BIN_LIB)/ILEVATOR) MSG('417 - Expectation failed') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0418) MSGF($(BIN_LIB)/ILEVATOR) MSG('418 - I am a teapot') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0421) MSGF($(BIN_LIB)/ILEVATOR) MSG('421 - Misdirected request') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0422) MSGF($(BIN_LIB)/ILEVATOR) MSG('422 - Unprocessable entity') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0423) MSGF($(BIN_LIB)/ILEVATOR) MSG('423 - Locked') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0424) MSGF($(BIN_LIB)/ILEVATOR) MSG('424 - Failed dependency') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0425) MSGF($(BIN_LIB)/ILEVATOR) MSG('425 - Too early') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0426) MSGF($(BIN_LIB)/ILEVATOR) MSG('426 - Upgrade required') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0428) MSGF($(BIN_LIB)/ILEVATOR) MSG('428 - Precondition required') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0429) MSGF($(BIN_LIB)/ILEVATOR) MSG('429 - Too many requests') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0431) MSGF($(BIN_LIB)/ILEVATOR) MSG('431 - Request header fields too large') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0451) MSGF($(BIN_LIB)/ILEVATOR) MSG('451 - Unavailable for legal reasons') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0500) MSGF($(BIN_LIB)/ILEVATOR) MSG('500 - Internal server error') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0501) MSGF($(BIN_LIB)/ILEVATOR) MSG('501 - Not implemented') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0502) MSGF($(BIN_LIB)/ILEVATOR) MSG('502 - Bad gateway') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0503) MSGF($(BIN_LIB)/ILEVATOR) MSG('503 - Service unavailable') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0504) MSGF($(BIN_LIB)/ILEVATOR) MSG('504 - Gateway timeout') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0505) MSGF($(BIN_LIB)/ILEVATOR) MSG('505 - HTTP version not supported') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0506) MSGF($(BIN_LIB)/ILEVATOR) MSG('506 - Variant also negotiates') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0507) MSGF($(BIN_LIB)/ILEVATOR) MSG('507 - Insufficient storage') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0508) MSGF($(BIN_LIB)/ILEVATOR) MSG('508 - Loop detected') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0510) MSGF($(BIN_LIB)/ILEVATOR) MSG('510 - Not extended') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0511) MSGF($(BIN_LIB)/ILEVATOR) MSG('511 - Network authentication required') SEV(10)"
	system "QSYS/ADDMSGD MSGID(ILV0999) MSGF($(BIN_LIB)/ILEVATOR) MSG('Not mapped HTTP status code') SEV(10)"

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
	-system -q "DLTOBJ OBJ($(BIN_LIB)/JOBLOG) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/MIME) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/MULTIPART) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/REQUEST) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/SOCKETS) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/URL) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ILEVATOR) OBJTYPE(*SRVPGM)"

purge: clean
	-system -q "DLTOBJ OBJ($(BIN_LIB)/H) OBJTYPE(*FILE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/QRPGLEREF) OBJTYPE(*FILE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/QSRVSRC) OBJTYPE(*FILE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ILEVATOR) OBJTYPE(*BNDDIR)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/ILEVATOR) OBJTYPE(*MSGF)"
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
	system -i "CRTCMOD MODULE($(BIN_LIB)/$(basename $(notdir $(SRC)))) SRCSTMF('$(SRC)') $(CCFLAGS2) " > error.txt
	system -i "UPDSRVPGM SRVPGM($(BIN_LIB)/ilevator) MODULE($(BIN_LIB)/*ALL)"  
endif

ifeq "$(suffix $(SRC))" ".rpgmod"
current:
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/$(basename $(notdir $(SRC)))) SRCSTMF('$(SRC)') $(RCFLAGS2) " > error.txt
	system -i "UPDSRVPGM SRVPGM($(BIN_LIB)/ilevator) MODULE($(BIN_LIB)/*ALL)"  
endif

ifeq "$(suffix $(SRC))" ".cpp"
current:
	system "CRTCPPMOD MODULE($(BIN_LIB)/$(basename $(notdir $(SRC)))) SRCSTMF('$(SRC)') $(CCFLAGS2) " > error.txt
	system "UPDSRVPGM SRVPGM($(BIN_LIB)/ilevator) MODULE($(BIN_LIB)/*ALL)"  
endif


# For vsCode / single file then i.e.: gmake current sqlio.c  
example: 
	system -i "CRTBNDRPG PGM($(BIN_LIB)/$(SRC)) SRCSTMF('examples/$(SRC).rpgle') DBGVIEW(*ALL)" > error.txt

test: 
	system -i "CRTBNDRPG PGM($(BIN_LIB)/$(SRC)) SRCSTMF('test/$(SRC).rpgle') DBGVIEW(*ALL)" > error.txt
	
.PHONY:
