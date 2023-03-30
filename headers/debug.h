#ifndef ILEVATOR_DEBUG_H
#define ILEVATOR_DEBUG_H

#include "ostypes.h"

void iv_debug(PUCHAR message, LONG replaceValueCount, ...);
#pragma map(iv_debug , "iv_debug_c")

#endif