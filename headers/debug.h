#ifndef ILEVATOR_DEBUG_H
#define ILEVATOR_DEBUG_H

#include "ostypes.h"

void iv_debug(PUCHAR message, ...);
#pragma map (iv_debug , "iv_debug")
#pragma descriptor (void iv_debug(void))

#endif