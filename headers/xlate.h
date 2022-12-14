#ifndef XLATE_H
#define XLATE_H
#include <iconv.h>
#include <QTQICONV.h>
iconv_t xlateOpen ( LONG fromCCSID,  LONG toCCSID);
ULONG xlateBuf(PUCHAR outBuf, PUCHAR inBuf , ULONG Len,  LONG fromCCSID,  LONG toCCSID);
ULONG xlateBufCd(iconv_t cd, PUCHAR outBuf, PUCHAR inBuf , ULONG length);
void  xlateVc  (PVARCHAR out, PVARCHAR in ,   LONG fromCCSID,  LONG toCCSID);
void  xlateLvc  (PLVARCHAR out, PLVARCHAR in ,   LONG fromCCSID,  LONG toCCSID);
PUCHAR xlateStr (PUCHAR out, PUCHAR in,  LONG fromCCSID,  LONG toCCSID);
#endif