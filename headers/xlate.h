#ifndef XLATE_H
#define XLATE_H
#include <iconv.h>
#include <QTQICONV.h>
iconv_t xlateOpen (int fromCCSID, int toCCSID);
ULONG xlateBuf(PUCHAR outBuf, PUCHAR inBuf , ULONG Len, int fromCCSID, int toCCSID);
ULONG xlateBufCd(iconv_t cd, PUCHAR outBuf, PUCHAR inBuf , ULONG length);
void  xlateVc  (PVARCHAR out, PVARCHAR in ,  int fromCCSID, int toCCSID);
void  xlateLvc  (PLVARCHAR out, PLVARCHAR in ,  int fromCCSID, int toCCSID);
PUCHAR xlateStr (PUCHAR out, PUCHAR in, int fromCCSID, int toCCSID);
#endif