#-----------------------------------------------------------
# User-defined part start
#

# NOTE - UTF is not allowed for ILE source (yet) - so convert to WIN-1252

# BIN_LIB is the destination library for the service program.
# the rpg modules and the binder source file are also created in BIN_LIB.
# binder source file and rpg module can be remove with the clean step (make clean)

BIN_LIB=ILEVATOR
TARGET_CCSID=*JOB
OUTPUT=*NONE
ILEASTIC_LIB=ILEASTIC
ILEASTIC_INCDIR=/usr/local/include/ileastic

#
# User-defined part end
#-----------------------------------------------------------


#
# Do not touch below
#

INCLUDE='/QIBM/include' '$(ILEASTIC_INCDIR)'

# RPG compile flags
RCFLAGS=OUTPUT($(OUTPUT)) OPTION(*NOUNREF *SRCSTMT) STGMDL(*INHERIT) INCDIR($(INCLUDE)) DBGVIEW(*LIST) TGTRLS(*CURRENT) TGTCCSID($(TARGET_CCSID))

# remove all default suffix rules
.SUFFIXES:

MODULES=$(BIN_LIB)/IVILEASTIC $(BIN_LIB)/IVILEAGET $(BIN_LIB)/IVILEADEL $(BIN_LIB)/IVILEAHEAD $(BIN_LIB)/IVILEAOPT $(BIN_LIB)/IVILEAPUT


# Dependency list

all:  modules ivileastic.pgm

modules: ivileastic.rpgmod ivileaget.rpgmod ivileadel.rpgmod ivileahead.rpgmod ivileaopt.rpgmod ivileaput.rpgmod

start:
	system "SBMJOB CMD(CALL PGM(IVILEASTIC)) JOB(IVILEASTIC) JOBQ(QUSRNOMAX) CURLIB($(BIN_LIB)) INLLIBL($(ILEASTIC_LIB)) ALWMLTTHD(*YES)"

clean:
	-system -i "DLTMOD $(BIN_LIB)/IVILEASTIC"
	-system -i "DLTPGM $(BIN_LIB)/IVILEASTIC"
	-system -i "DLTMOD $(BIN_LIB)/IVILEAGET"
	-system -i "DLTMOD $(BIN_LIB)/IVILEADEL"
	-system -i "DLTMOD $(BIN_LIB)/IVILEAHEAD"
	-system -i "DLTMOD $(BIN_LIB)/IVILEAOPT"
	-system -i "DLTMOD $(BIN_LIB)/IVILEAPUT"

#-----------------------------------------------------------

%.rpgmod: .PHONY
	system -q "CHGATR OBJ('$*.rpgmod') ATR(*CCSID) VALUE(1252)"
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/$*) SRCSTMF('$*.rpgmod') $(RCFLAGS)"

%.pgm:
	system -i "CRTPGM PGM($(BIN_LIB)/$*) MODULE($(MODULES)) ACTGRP(*CALLER) BNDSRVPGM($(ILEASTIC_LIB)/ILEASTIC)"

.PHONY:
