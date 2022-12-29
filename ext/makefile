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

#
# User-defined part end
#-----------------------------------------------------------


# Do not touch below
INCLUDE='/QIBM/include' 'headers/'  

# C compile flags
CCFLAGS=OUTPUT(*PRINT) OPTIMIZE(10) ENUM(*INT) TERASPACE(*YES) STGMDL(*INHERIT) SYSIFCOPT(*IFSIO) INCDIR($(INCLUDE)) DBGVIEW($(DBGVIEW)) DEFINE($(DEFINE)) TGTCCSID($(TARGET_CCSID)) TGTRLS($(TARGET_RLS))

# remove all default suffix rules
.SUFFIXES:


# Dependency list

all: compile

compile: xlate.c base64.c teraspace.c simplelist.c strutil.c varchar.c

%.c:
	system -q "CHGATR OBJ('src/$*.c') ATR(*CCSID) VALUE(1252)"
	system "CRTCMOD MODULE($(BIN_LIB)/$(notdir $*)) SRCSTMF('src/$*.c') $(CCFLAGS)"

clean:
	-system -q "DLTOBJ OBJ($(BIN_LIB)/BASE64) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/MESSAGE) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/SIMPLELIST) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/STRUTIL) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/TERASPACE) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/VARCHAR) OBJTYPE(*MODULE)"
	-system -q "DLTOBJ OBJ($(BIN_LIB)/XLATE) OBJTYPE(*MODULE)"
