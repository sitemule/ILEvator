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


OBJECTS = ITGET ITPOST
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
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITGET) SRCSTMF('itget.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITGET MODULE(($(BIN_LIB)/ITGET)) $(BINDFLAGS)"
	
ITPOST:
	system -i "CRTRPGMOD MODULE($(BIN_LIB)/ITPOST) SRCSTMF('itpost.rpgmod') $(RCFLAGS)"
	system -i "CRTSRVPGM $(BIN_LIB)/ITPOST MODULE(($(BIN_LIB)/ITPOST)) $(BINDFLAGS)"

clean:
	-system -i "DLTMOD $(BIN_LIB)/ITGET"
	-system -i "DLTMOD $(BIN_LIB)/ITPOST"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITGET"
	-system -i "DLTSRVPGM $(BIN_LIB)/ITPOST"
	$(MAKE) -C ileastic/ clean

.PHONY:
