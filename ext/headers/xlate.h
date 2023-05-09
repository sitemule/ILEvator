#ifndef XLATE_H
#define XLATE_H

#include <iconv.h>
#include <QTQICONV.h>

#include "ostypes.h"
#include "varchar.h"

iconv_t xlate_open ( LONG fromCCSID,  LONG toCCSID);
ULONG xlate_translateBuffer(PUCHAR outBuf, PUCHAR inBuf , ULONG Len,  LONG fromCCSID,  LONG toCCSID);
ULONG xlate_translateDescriptor(iconv_t cd, PUCHAR outBuf, PUCHAR inBuf , ULONG length);
void  xlate_translateVarchar (PVARCHAR out, PVARCHAR in ,   LONG fromCCSID,  LONG toCCSID);
void  xlate_translateLongVarchar (PLVARCHAR out, PLVARCHAR in ,   LONG fromCCSID,  LONG toCCSID);
PUCHAR xlate_translateString (PUCHAR out, PUCHAR in,  LONG fromCCSID,  LONG toCCSID);

#endif