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
iconv_t xlate_open ( LONG fromCCSID,  LONG toCCSID)
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
ULONG xlate_translateBuffer(PUCHAR outBuf, PUCHAR inBuf , ULONG length,  LONG fromCCSID,  LONG toCCSID)
{
   iconv_t cd;
   ULONG outLen;

   if ( length ==0 ) return 0;

   if (fromCCSID == toCCSID) {
      memcpy(outBuf, inBuf , length);
      return length;
   }

   cd = xlate_open(fromCCSID, toCCSID);
   if  (cd.return_value == -1) return -1;

   outLen = xlate_translateDescriptor(cd, outBuf, inBuf , length);
   iconv_close (cd);

   return (outLen);  // Number of bytes converted
}

/* ------------------------------------------------------------- */
ULONG xlate_translateDescriptor(iconv_t cd, PUCHAR outBuf, PUCHAR inBuf , ULONG length)
{
   PUCHAR pOutBuf;
   PUCHAR pInBuf;
   int i;
   size_t outLen, inBytesleft, outBytesleft;
   size_t before; 
   int rc;

   if (length == 0 ) return 0;

   before = outBytesleft = length * 4; // Max size of UTF8 expand to 4 times bytes
   inBytesleft  = length;

   pOutBuf = outBuf;
   pInBuf  = inBuf;

   // Do Conversion
   rc = iconv (cd, &pInBuf, &inBytesleft, &pOutBuf, &outBytesleft);
   if (rc == -1) return (0);

   outLen  = before - outBytesleft;
   return (outLen);  // Number of bytes converted
}
/* ------------------------------------------------------------- */
PUCHAR xlate_translateString (PUCHAR out, PUCHAR in,  LONG fromCCSID,  LONG toCCSID)
{
   int length = xlate_translateBuffer(out, in , strlen(in)  , fromCCSID, toCCSID);
   out[length] = '\0';
   return out;
}
/* ------------------------------------------------------------- */
// Export: 
/* ------------------------------------------------------------- */
void xlate_translateVarchar (PVARCHAR out, PVARCHAR in ,  LONG fromCCSID,  LONG toCCSID)
{
   out->Length = xlate_translateBuffer(out->String , in->String , in->Length , fromCCSID, toCCSID);
}
/* ------------------------------------------------------------- */
void xlate_translateLongVarchar (PLVARCHAR out, PLVARCHAR in ,   LONG fromCCSID,  LONG toCCSID)
{
   out->Length = xlate_translateBuffer(out->String , in->String , in->Length , fromCCSID, toCCSID);
}
