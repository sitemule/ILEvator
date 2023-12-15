#
# Build script for Unit Tests
#


#-------------------------------------------------------------------------------
# User-defined part start
#

# BIN_LIB is the destination library for the unit tests.
BIN_LIB=ILEVATOR

# Library which contains RUTESTCASE service program
RU_LIB=*LIBL

# This library contains the ILEASTIC modules.
ILEVATOR_LIB=$(BIN_LIB)

OUTPUT=*PRINT

##DEFINE=RPG_HAS_OVERLOAD

#
# User-defined part end
#-------------------------------------------------------------------------------


OBJECTS = BASE64UT SMPLLSTUT URLPARSEUT REQUESTUT STRUTILUT BASICAUTUT BEARERUT ENCODEUT FORMUT MULTIPARUT
SRVPGMS = ($(RU_LIB)/RUTESTCASE) (QICU/QXICUUC40 *DEFER)

# system and application include folder
INCLUDE='/QIBM/include' '../headers/' '../ext/headers'

# CCFLAGS = C compiler parameter
CCFLAGS=OUTPUT($(OUTPUT)) OPTIMIZE(10) TGTCCSID(37) TGTRLS($(TARGET_RLS)) ENUM(*INT) TERASPACE(*YES) STGMDL(*INHERIT) SYSIFCOPT(*IFSIO) INCDIR($(INCLUDE)) DBGVIEW(*ALL)

# RPG compiler parameter
RCFLAGS=INCDIR('.' '../headers' '../ext/headers') DBGVIEW(*source) OUTPUT($(OUTPUT)) STGMDL(*SNGLVL) DEFINE($(DEFINE))

# Bind options
BINDFLAGS=STGMDL(*SNGLVL) BNDSRVPGM($(SRVPGMS)) BNDDIR($(ILEVATOR_LIB)/MODULES) EXPORT(*ALL) OPTION(*DUPPROC) DETAIL(*FULL)


.SUFFIXES: .c

.c:
	system -i "CHGATR OBJ('$<') ATR(*CCSID) VALUE(1208)"
	system -ik "CRTCMOD MODULE($(BIN_LIB)/$@) SRCSTMF('$<') $(CCFLAGS)"


all: clean compile

compile: $(OBJECTS)

REQUESTUT:
	system -i "CHGATR OBJ('requestut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/REQUESTUT) SRCSTMF('requestut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/REQUESTUT MODULE(($(BIN_LIB)/REQUESTUT)) $(BINDFLAGS)"

STRUTILUT:
	system -i "CHGATR OBJ('strutilut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/STRUTILUT) SRCSTMF('strutilut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/STRUTILUT MODULE(($(BIN_LIB)/STRUTILUT)) $(BINDFLAGS)"

URLPARSEUT:
	system -i "CHGATR OBJ('urlparseut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/URLPARSEUT) SRCSTMF('urlparseut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/URLPARSEUT MODULE(($(BIN_LIB)/URLPARSEUT)) $(BINDFLAGS)"

BASE64UT:
	system -i "CHGATR OBJ('base64ut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -ik "CRTRPGMOD MODULE($(BIN_LIB)/BASE64UT) SRCSTMF('base64ut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/BASE64UT MODULE(($(BIN_LIB)/BASE64UT)) $(BINDFLAGS)"

BASICAUTUT:
	system -i "CHGATR OBJ('basicautut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -ik "CRTRPGMOD MODULE($(BIN_LIB)/BASICAUTUT) SRCSTMF('basicautut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/BASICAUTUT MODULE(($(BIN_LIB)/BASICAUTUT)) $(BINDFLAGS)"

BEARERUT:
	system -i "CHGATR OBJ('bearerut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -ik "CRTRPGMOD MODULE($(BIN_LIB)/BEARERUT) SRCSTMF('bearerut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/BEARERUT MODULE(($(BIN_LIB)/BEARERUT)) $(BINDFLAGS)"

SMPLLSTUT:
	system -i "CHGATR OBJ('smpllstut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/SMPLLSTUT) SRCSTMF('smpllstut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/SMPLLSTUT MODULE(($(BIN_LIB)/SMPLLSTUT)) $(BINDFLAGS)"

ENCODEUT:
	system -i "CHGATR OBJ('encodeut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -ik "CRTRPGMOD MODULE($(BIN_LIB)/ENCODEUT) SRCSTMF('encodeut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ENCODEUT MODULE(($(BIN_LIB)/ENCODEUT)) $(BINDFLAGS)"

FORMUT:
	system -i "CHGATR OBJ('formut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -ik "CRTRPGMOD MODULE($(BIN_LIB)/FORMUT) SRCSTMF('formut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/FORMUT MODULE(($(BIN_LIB)/FORMUT)) $(BINDFLAGS)"

MULTIPARUT:
	system -i "CHGATR OBJ('multiparut.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -ik "CRTRPGMOD MODULE($(BIN_LIB)/MULTIPARUT) SRCSTMF('multiparut.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/MULTIPARUT MODULE(($(BIN_LIB)/MULTIPARUT)) $(BINDFLAGS)"

clean:
	-system -i "DLTMOD $(BIN_LIB)/REQUESTUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/REQUESTUT"
	-system -i "DLTMOD $(BIN_LIB)/STRUTILUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/STRUTILUT"
	-system -i "DLTMOD $(BIN_LIB)/BASE64UT"
	-system -i "DLTSRVPGM $(BIN_LIB)/BASE64UT"
	-system -i "DLTMOD $(BIN_LIB)/SMPLLSTUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/SMPLLSTUT"
	-system -i "DLTMOD $(BIN_LIB)/URLPARSEUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/URLPARSEUT"
	-system -i "DLTMOD $(BIN_LIB)/BEARERUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/BEARERUT"
	-system -i "DLTMOD $(BIN_LIB)/ENCODEUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/ENCODEUT"
	-system -i "DLTMOD $(BIN_LIB)/BASICAUTUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/BASICAUTUT"
	-system -i "DLTMOD $(BIN_LIB)/FORMUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/FORMUT"
	-system -i "DLTMOD $(BIN_LIB)/MULTIPARUT"
	-system -i "DLTSRVPGM $(BIN_LIB)/MULTIPARUT"
