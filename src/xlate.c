// CMD:CRTCMOD 
/* ------------------------------------------------------------- */
/* Program . . . : xlate                                         */
/* Date  . . . . : 24.04.2008                                    */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : X-alation using iconv                         */
/*                                                               */
/* By     Date       PTF     Description                         */
/* NL     24.04.2008         New program                         */
/* ------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <iconv.h>
#include <QTQICONV.h>

#include "ostypes.h"
#include "varchar.h"
#include "xlate.h"

/* ------------------------------------------------------------- */
iconv_t xlateOpen (int fromCCSID, int toCCSID)
{
   QtqCode_T to;
   QtqCode_T from;

   memset(&from , 0, sizeof(from));
   from.CCSID = fromCCSID;
   from.cnv_alternative = 0 ;
   from.subs_alternative = 0 ;
   from.shift_alternative = 0;
   from.length_option = 0;
   from.mx_error_option = 0;

   memset(&to , 0, sizeof(to));
   to.CCSID = toCCSID;
   to.cnv_alternative = 0;
   to.subs_alternative = 0;
   to.shift_alternative = 0;
   to.length_option = 0;
   to.mx_error_option = 0;

   // Get descriptor
   return QtqIconvOpen( &to, &from);
}
/* ------------------------------------------------------------- */
ULONG xlateBuf(PUCHAR outBuf, PUCHAR inBuf , ULONG length, int fromCCSID, int toCCSID)
{
   iconv_t cd;
   ULONG outLen;

   if ( length ==0 ) return 0;

   if (fromCCSID == toCCSID) {
      memcpy(outBuf, inBuf , length);
      return length;
   }

   cd = xlateOpen  (fromCCSID, toCCSID);
   if  (cd.return_value == -1) return -1;

   outLen = xlateBufCd(cd, outBuf, inBuf , length);
   iconv_close (cd);

   return (outLen);  // Number of bytes converted
}

/* ------------------------------------------------------------- */
ULONG xlateBufCd(iconv_t cd, PUCHAR outBuf, PUCHAR inBuf , ULONG length)
{
   PUCHAR pOutBuf;
   PUCHAR pInBuf;
   int i;
   size_t outLen, inBytesleft, outBytesleft;
   size_t before, rc;

   if (length ==0 ) return 0;

   before = outBytesleft = length * 4; // Max size of UTF8 expand to 4 times bytes
   inBytesleft  = length;

   pOutBuf = outBuf;
   pInBuf  = inBuf;

   // Do Conversion
   rc = iconv (cd, &pInBuf, &inBytesleft, &pOutBuf, &outBytesleft);
   if (rc == -1) return (-1);

   outLen  = before - outBytesleft;
   return (outLen);  // Number of bytes converted
}
/* ------------------------------------------------------------- */
void xlateVc  (PVARCHAR out, PVARCHAR in ,  int fromCCSID, int toCCSID)
{
   out->Length = xlateBuf(out->String , in->String , in->Length , fromCCSID, toCCSID);
}
/* ------------------------------------------------------------- */
void xlateLvc  (PLVARCHAR out, PLVARCHAR in ,  int fromCCSID, int toCCSID)
{
   out->Length = xlateBuf(out->String , in->String , in->Length , fromCCSID, toCCSID);
}
/* ------------------------------------------------------------- */
PUCHAR xlateStr (PUCHAR out, PUCHAR in, int fromCCSID, int toCCSID)
{
   int length = xlateBuf(out, in , strlen(in)  , fromCCSID, toCCSID);
   out[length] = 0;
   return out;
}
