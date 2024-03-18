#
# Build examples for ILEvator
#
# This works for:
#  gmake              compiles all demo programs
#  gmake helloworld   only compiles this single file
#

#-------------------------------------------------------------------------------
# User-defined part start
#

# BIN_LIB is the destination library for the demo programs.
# NOTE: LIBLIST can be overwritten from the commandline, but defaults to the BIN_LIB
BIN_LIB=ILEVATOR
LIBLIST="$(BIN_LIB) NOXDB SMSDB"
OUTPUT=*NONE

# The shell we use
SHELL=/QOpenSys/usr/bin/qsh

#
# add this after CRTBNDRPG for build in vsCode:
# | grep '*RNF' | grep -v '*RNF7031' | sed  "s!*!$<: &!"
# NOTE that OUTPUT are overridden by the make in the vsCode task.json 
# to compile a single file and get erros to the editor
# User-defined part end
#-------------------------------------------------------------------------------
RPG_OPT=OPTION(*EVENTF *NOXREF *NOUNREF *NOSHOWCPY *NOEXPDDS)  OUTPUT($(OUTPUT)) DBGVIEW(*ALL) INCDIR('./..' '/prj/ILEvator/headers')
SQL_OPT=RPGPPOPT(*NONE) TGTRLS(V7R3M0)
CCSID=277


all: compile 
.ONESHELL:
compile:
	./compile.sh -f "$(SRC)"  -b $(BIN_LIB) -l $(LIBLIST) -c $(CCSID) -o "$(RPG_OPT)" -s "$(SQL_OPT)" 
.PHONY: compile
