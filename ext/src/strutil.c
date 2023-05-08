/* SYSIFCOPT(*IFSIO) TERASPACE(*YES *TSIFC) STGMDL(*INHERIT) */
/* ------------------------------------------------------------- */
/* Date  . . . . : 14.12.2005                                    */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : Base utilies                                  */
/*                                                               */
/* By     Date       PTF     Description                         */
/* NL     09.03.2005         New program                         */
/* ------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <decimal.h>
#include <mih/milckcom.h>     /* Lock types           */
#include <mih/locksl.h>       /* LOCKSL instruction   */
#include <mih/unlocksl.h>     /* UNLOCKSL instruction */
#include <mih/setspfp.h>
#include <mih/setsppfp.h>

#include "ostypes.h"
#include "teraspace.h"
#include "strutil.h"

// ------------------------------------------------------------- 
// does a begins with string sequence of b
// -------------------------------------------------------------
BOOL strutil_beginsWith(PUCHAR a, PUCHAR b)
{
    return memicmp(a, b, strlen(b)) == 0;
}
// ------------------------------------------------------------- 
// Q&D ASCII version of: does a begins with string sequence of b
// -------------------------------------------------------------
BOOL strutil_beginsWithAscii(PUCHAR a, PUCHAR b)
{
    LONG i;
    LONG l = strlen(b);
    for (i=0; i < l; i ++) {
        if ((a[i] & 0xdf) != (b[i] & 0xdf)) {
            return FALSE;
        }
    }
    return TRUE;
}
/* ------------------------------------------------------------- *\
   copy a string and returns number of bytes copied
\* ------------------------------------------------------------- */
LONG strutil_cpystr  (PUCHAR out , PUCHAR in)
{
   int l = strlen(in);
   memcpy (out , in , l+1);
   return (l);
}
/* ------------------------------------------------------------- *\
   copy memroy and returns number of bytes copied
\* ------------------------------------------------------------- */
LONG strutil_cpymem (PUCHAR out , PUCHAR in, LONG len)
{
   memcpy (out , in , len);
   return (len);
}
/* ------------------------------------------------------------- *\
   strIcmp  is stricmp in ccsid 277
\* ------------------------------------------------------------- */
SHORT strutil_strIcmp (PUCHAR s1, PUCHAR s2)
{
    SHORT c =0;
    do {
      c = strutil_toUpper(*(s1++)) - strutil_toUpper(*(s2++));
    } while (c == 0 && *s1 && *s2);

    return c;
}
/* ------------------------------------------------------------- *\
   memIcmp  is memicmp in ccsid 277
\* ------------------------------------------------------------- */
SHORT strutil_memIcmp (PUCHAR s1, PUCHAR s2 , LONG len)
{
    SHORT c =0;
    while (len-- > 0 && c==0) {
      c = strutil_toUpper(*(s1++)) - strutil_toUpper(*(s2++));
    }
    return c;
}
/* ------------------------------------------------------------- *\
   memmem 
\* ------------------------------------------------------------- */
PUCHAR strutil_memmem  (PUCHAR heystack , ULONG haystackLen, 
                       PUCHAR needle , ULONG needleLen)
{
    PUCHAR p = heystack;
    PUCHAR end = heystack + haystackLen;
    while (p < end) {
        if (*p == *needle
        &&   0 == memcmp ( p , needle , needleLen)) {
            return p;
        }
        p++;
    }
    return NULL;
}
/* ------------------------------------------------------------- *\
   toUpper and toLower in ccsid 277
\* ------------------------------------------------------------- */
UCHAR strutil_toUpper(UCHAR c)
{
   switch(c) {
      case 'æ' : return 'Æ';
      case 'ø' : return 'Ø';
      case 'å' : return 'Å';
      default  : return toupper(c);
   }
}
UCHAR strutil_toLower(UCHAR c)
{
   switch(c) {
      case 'Æ' : return 'æ';
      case 'Ø' : return 'ø';
      case 'Å' : return 'å';
      default  : return tolower(c);
   }
}
/* ------------------------------------------------------------- */
UCHAR strutil_toupperascii (UCHAR c)
{
#pragma convert(1252)
   if (c >= 'a' && c <= 'z') return c - ( 'a' - 'A');
   return c;
#pragma convert(0)
}
/* ------------------------------------------------------------- *\
\* ------------------------------------------------------------- */
UCHAR strutil_tolowerascii (UCHAR c)
{
#pragma convert(1252)
   if (c >= 'A' && c <= 'Z') return c + ( 'a' - 'A');
   return c;
#pragma convert(0)
}
/* ------------------------------------------------------------- *\
   stristr is strstr that ignores the case
   is trturns the pointer to "key" with in base
\* ------------------------------------------------------------- */
PUCHAR strutil_stristr(PUCHAR base, PUCHAR key )
{
   UCHAR k = strutil_toUpper(key[0]) ;
   SHORT keylen = strlen (key);

   while (*base) {
     if  (strutil_toUpper(*base) == k) {
        if (strutil_memIcmp (base , key , keylen) == 0) {  /* Found !! */
           return base;
        }
     }
     base ++;
   }
   return NULL;
}
/* ------------------------------------------------------------- *\
   strchrreplace returns a string, where chars are replaced one by one if byte match
\* ------------------------------------------------------------- */
PUCHAR strutil_strchrreplace(PUCHAR out , PUCHAR in , PUCHAR from , PUCHAR to )
{
   PUCHAR pf, pt, res = out;

   for (;*in; in++, out++) {
     *out = *in;
     for   (pf=from, pt=to ;*pf;pf++, pt++)  {
       if (*in == *pf) {
         *out = *pt;
         break;
       }
     }
   }
   *out = '\0';
   return res;
}
/* ------------------------------------------------------------- *\
   memstrreplace returns new lengt of the buffer where replaced with a string
\* ------------------------------------------------------------- */
LONG  strutil_bufferReplace(PUCHAR buf , LONG len , PUCHAR from , PUCHAR to )
{
   PUCHAR in , out = buf, inbuf, end;
   int lFrom = strlen(from), lTo = strlen(to);

   in = inbuf = teraspace_alloc(len);
   memcpy(in, buf, len);
   end = in + len;

   while (in<end) {
     if (*in == *from
     &&  memcmp (in , from  , lFrom) ==0) {
       memcpy(out , to , lTo);
       out += lTo;
       in += lFrom;
     } else {
       *(out++) = *(in++);
     }
   }
   teraspace_free(&inbuf);
   return out - buf;
}
/* ------------------------------------------------------------- *\
   "memstr" returns a pointer to the first occurrence of a substring within another string.

   base: points to the string to be scanned.
   key: points to the (sub)string to scan for. This string should end in the usual '\0'.
   len:  is the length of "base".
   returns:
   points to the first occurrence of the substring in the given string. If the substring is not found, this will be a nu
   ll pointer.
\* ------------------------------------------------------------- */
PUCHAR strutil_memstr(PUCHAR base, PUCHAR key, LONG len )
{
   UCHAR k = key[0] ;
   SHORT keylen = strlen (key);

   while (len>0) {
     if  (*base == k) {
        if (memcmp (base , key , keylen) == 0) {  /* Found !! */
           return base;
        }
     }
     base ++;
     len --;
   }
   return NULL;
}
PUCHAR strutil_memIstr(PUCHAR base, PUCHAR key, LONG len )
{
   UCHAR k = strutil_toUpper(key[0]) ;
   SHORT keylen = strlen (key);

   while (len>0) {
     if  (strutil_toUpper(*base) == k) {
        if (strutil_memIcmp (base , key , keylen) == 0) {  /* Found !! */
           return base;
        }
     }
     base ++;
     len --;
   }
   return NULL;
}
SHORT strutil_memicmpascii (PUCHAR m1 , PUCHAR m2 , LONG len)
{
   while (len) {
     UCHAR c1 = strutil_toupperascii(*m1);
     UCHAR c2 = strutil_toupperascii(*m2);
     if (c1 > c2) return 1;
     if (c1 < c2) return -1;
     len --;
     m1++;m2++;
   }
   return 0;
}
PUCHAR strutil_memistrascii(PUCHAR base, PUCHAR key, LONG len )
{
   SHORT keylen = strlen (key);
   UCHAR k = strutil_toupperascii(key[0]);

   while (len>0) {
     if  (strutil_toupperascii(*base) == k) {
       if (strutil_memicmpascii (base , key , keylen) == 0) {  /* Found !! */
          return base;
       }
     }
     base ++;
     len --;
   }
   return NULL;
}
/* ------------------------------------------------------------- *\
   firstnonblank returns pointer to the string > ' '
\* ------------------------------------------------------------- */
PUCHAR strutil_firstnonblank(PUCHAR in)
{
// Find first non blank
   for (;;){
      if (*in == '\0') return (in);
      if (*in > ' ')   return (in);
      in ++;
   }
}
/* ------------------------------------------------------------- *\
   firstnonblank returns pointer to the string > ' '
\* ------------------------------------------------------------- */
#pragma convert 1252
PUCHAR strutil_firstnonblanka(PUCHAR in)
{
// Find first non blank
   for (;;){
      if (*in == '\0') return (in);
      if (*in > ' ')   return (in);
      in ++;
   }
}
#pragma convert 0
/* ------------------------------------------------------------- *\
   lastnonblank returns pointer to the last char > ' '
\* ------------------------------------------------------------- */
PUCHAR strutil_lastnonblank(PUCHAR in)
{
   LONG   len;
   PUCHAR end = in + strlen(in);

// Find last  non blank
   while (end > in) {
      if (*end  > ' ') return (end);
      end--;
   }
   return in;
}
/* ------------------------------------------------------------- *\
   righttrim - just set string termination after the last non blank
\* ------------------------------------------------------------- */
PUCHAR strutil_righttrim(PUCHAR in)
{
  PUCHAR p = strutil_lastnonblank(in);
  * (p+1) = '\0';
  return in;
}
/* ------------------------------------------------------------- *\
   trim both
\* ------------------------------------------------------------- */
PUCHAR strutil_trim(PUCHAR in)
{
  PUCHAR out, end, begin, ret;
  BOOL   docopy = false;
  ret = begin = end = out = in;
  while (*in) {
     if (*in > ' ') {
        docopy = true;
     }
     if (docopy) {
       *(out++) = *in;
       if (*in > ' ') end = out;  // Store that address just after the nonblank char
     }
     in++;
  }
  *end = '\0';
  return ret;
}
/* ------------------------------------------------------------- *\
   righttrimlen - start from length
\* ------------------------------------------------------------- */
PUCHAR strutil_righttrimlen(PUCHAR in , LONG size)
{
  PUCHAR p = in + size -1 ;
  for   (;p >= in && * p <= ' ' ; p--);
  * (p+1) = '\0';
  return in;
}
/* ------------------------------------------------------------- *\
   righttrimlen - start from length
\* ------------------------------------------------------------- */
LONG strutil_lenrighttrimlen(PUCHAR in , LONG size)
{
  PUCHAR p = in + size -1 ;
  if (size <= 0) return 0;
  for   (;p >= in && * p <= ' ' ; p--, size --);
  return size;
}
/* ------------------------------------------------------------- *\
   lastnonblank returns pointer to the last char > ' '
\* ------------------------------------------------------------- */
PUCHAR strutil_lastnonblankfrom(PUCHAR in, LONG from)
{
   PUCHAR end = in;
   PUCHAR p = in;
   while  (from --) {
      if (*p  > ' ') {
        end = p;
      } else if (*p == '\0') {
        break;
      }
      p++;
   }
   return end;
}
/* ------------------------------------------------------------- *\
   strtrimcpy copys and remows blanks before and after
\* ------------------------------------------------------------- */
PUCHAR strutil_strtrimncpy(PUCHAR out , PUCHAR in , LONG maxlen)
{
   PUCHAR end = out;
   PUCHAR ret = out;
   BOOL   findfirst = TRUE;

   for  (; maxlen > 0 ; maxlen --) {
      if (findfirst ) {
         if (*in > ' ') {
            findfirst = FALSE;
         }
      }
      if (! findfirst) {
        *out = *in;
        if (*out > ' ') {
          end = out + 1; // Where the zero termination will be
        }
        out++;
      }
      in++;
   }
   *(end) = '\0';
   return ret;
}
/* ------------------------------------------------------------- *\
   strtrimcpy copys and remows blanks before and after
\* ------------------------------------------------------------- */
PUCHAR strutil_strtrimcpy(PUCHAR out , PUCHAR in)
{
   PUCHAR end = out;
   PUCHAR ret = out;
   BOOL   findfirst = TRUE;
   int maxlen = strlen(in);

   for  (; maxlen > 0 ; maxlen --) {
      if (findfirst ) {
         if (*in > ' ') {
            findfirst = FALSE;
         }
      }
      if (! findfirst) {
        *out = *in;
        if (*out > ' ') {
          end = out + 1; // Where the zero termination will be
        }
        out++;
      }
      in++;
   }
   *(end) = '\0';
   return ret;
}
/* ------------------------------------------------------------- *\
   substr  copys and from and up to len
\* ------------------------------------------------------------- */
PUCHAR strutil_substr(PUCHAR out , PUCHAR in , LONG len)
{
   if (len < 0) len =0;

   if (in == NULL) {
      out[0] = '\0';
      return (out);
   }

   if (out == NULL) {
     system("DSPJOB OUTPUT(*PRINT) OPTION(*PGMSTK)");
   }

   memcpy(out, in , len);
   out[len] = '\0';
   return (out);
}
/* ------------------------------------------------------------- *\
   copys a subword from at list seperated by the delimiter list
   wordindex start at 0
\* ------------------------------------------------------------- */
PUCHAR strutil_subword (PUCHAR out , PUCHAR in , LONG ix, PUCHAR delimiters)
{
   PUCHAR res = out;
   PUCHAR pi  = in;
   LONG cx  =0;
   PUCHAR p;
   LONG i;

   *out = '\0';
   if (*in == '\0' ) return res; // Not found

   // Find the first occurens
   for (; *in > '\0' && cx <ix; in++) {
      p = strchr(delimiters , *in);
      if (p) cx ++;
   }
   if (cx != ix) return res; // Not found

   // Find the first occurens
   while(*in > '\0') {
      p = strchr(delimiters , *in);
      if (p) break;
      *(out++) = *(in++);
   }
   *(out) = '\0';
   return res;
}
/* ------------------------------------------------------------- */
LONG strutil_subwords (PUCHAR in , PUCHAR  delimiters)
{
   LONG res =1;
   PUCHAR p;

   if (!*in) return 0;

   for (; *in ; in++) {
      p = strchr(delimiters , *in);
      if (p) res++;
   }
   return res;
}
/* ------------------------------------------------------------- */
/* -----------------------------------------------------------------
   Copy a C-string to fixed char according to its length
   padding it right with blanks
   ----------------------------------------------------------------- */
PUCHAR strutil_padncpy(PUCHAR dst, PUCHAR src, SHORT dstlen)
{
   PUCHAR ret = dst;
   SHORT i;
   for (i=0;i< dstlen ; i++) {
      if (*src) {
         *dst = *src;
         src++;
      } else {
         *dst = ' ';
      }
      dst++;
   }
   return ret;
}
/* -----------------------------------------------------------------
   ----------------------------------------------------------------- */
PUCHAR strutil_pad(PUCHAR s , LONG l)
{
  BOOL   dopad = FALSE;
  PUCHAR r = s;
  LONG   i;
  for (i = 0; i< l; i++,s++) {
    if (*s  < ' ') dopad = TRUE;
    if (dopad) *s = ' ';

  }
  return r;
}
/* -----------------------------------------------------------------
   ----------------------------------------------------------------- */
PUCHAR strutil_righttrimcpy(PUCHAR dst, PUCHAR src)
{
   PUCHAR end = dst;
   PUCHAR ret = dst;
   SHORT i;
   while (*src) {
      *dst = *src;
      if (*dst > ' ') {
        end = dst;
      }
      dst ++;
      src ++;
   }
   *(end+1) = '\0';
   return ret;
}
/* -----------------------------------------------------------------
   ----------------------------------------------------------------- */
PUCHAR strutil_righttrimncpy(PUCHAR dst, PUCHAR src, LONG len)
{
   PUCHAR end = dst;
   PUCHAR ret = dst;
   SHORT i;
   while (*src && len-- > 0) {
      *dst = *src;
      if (*dst > ' ') {
        end = dst;
      }
      dst ++;
      src ++;
   }
   *(end+1) = '\0';
   return ret;
}
/* -----------------------------------------------------------------
   ----------------------------------------------------------------- */
PUCHAR strutil_UpperString(PUCHAR str)
{
   PUCHAR r = str;
   if (str == NULL) return NULL;
   while (*str) {
      *str = strutil_toUpper(*str);
      str++;
   }
   return r;
}
/* -----------------------------------------------------------------
   ----------------------------------------------------------------- */
PUCHAR strutil_str2upper (PUCHAR out, PUCHAR in )
{
   PUCHAR r = out;
   if (in) {
     while (*in ) {
        *(r++)   = strutil_toUpper(*(in++));
     }
   }
   *r= '\0';
   return out;
}
/* -----------------------------------------------------------------
   ----------------------------------------------------------------- */
PUCHAR strutil_str2lower (PUCHAR out, PUCHAR in )
{
   PUCHAR r = out;
   if (in) {
     while (*in ) {
        *(r++)   = strutil_toLower(*(in++));
     }
   }
   *r= '\0';
   return out;
}
/* --------------------------------------------------------------------------- */
UCHAR strutil_hexchar2int (UCHAR c)
{
   if (c >= '0' && c <= '9') {
     return (c - '0');
   }
   if (c >= 'A' && c <= 'F') {
     return (c - 'A' + 10);
   }
   if (c >= 'a' && c <= 'f') {
     return (c - 'a' + 10);
   }
}
/* --------------------------------------------------------------------------- */
PUCHAR strutil_binMem2Hex (PUCHAR out , PUCHAR in , LONG len)
{
   PUCHAR  res = out;
   PUCHAR h = "0123456789ABCDEF";
   UCHAR c;
   SHORT i;

   for (i=0; i < len  ; i++) {
     c = in[i] ;
     *(res++) = h[c / 16];
     *(res++) = h[c % 16];
   }
    *(res++) = '\0';   // Can be a string
   return out;
}
/* --------------------------------------------------------------------------- */
PUCHAR strutil_hex2BinMem (PUCHAR out , PUCHAR in , LONG len)
{
   PUCHAR  res = out;

   while (len-- > 0) {
     *(out++) = (16 * strutil_hexchar2int(*(in++)) + strutil_hexchar2int(*(in++)));
   }
   return res;
}
/* --------------------------------------------------------------------------- */
ULONG strutil_hexstr2int (PUCHAR s)
{
   LONG res = 0;
   while (*s) {
      res = (res * 256) + (16 * strutil_hexchar2int(*(s++)) + strutil_hexchar2int(*(s++)));
   }
   return res;
}
/* ------------------------------------------------------------- */
FIXEDDEC strutil_str2dec(PUCHAR str , UCHAR decPoint)
{
   PUCHAR p;
   FIXEDDEC        Res   = 0D;
   decimal(17,16)  Temp  = 0D;
   decimal(17)     Decs  = 1D;
   BOOL  DecFound = FALSE;
   UCHAR c = '0';
   PUCHAR firstDigit = NULL;
   PUCHAR lastDigit  = NULL;
   int   Dec=0;
   int   Prec=0;

   for (p=str; (c = *p) > '\0' ; p ++) {
      if (c >= '0' && c <= '9' ) {
         if (!firstDigit) firstDigit = p;
         lastDigit = p;
         if (DecFound) {
           if (++Prec <= 15) {
              Decs  *= 10D;
              Temp = (c - '0');
              Temp /= Decs;
              Res += Temp;
           }
         } else {
           if (Dec < 15) {
             Res = Res * 10D + (c - '0');
             if (Res > 0D) Dec++;
           }
         }
      } else if (c == decPoint) {
         DecFound = TRUE;
      }
   }
   if ((firstDigit > str && *(firstDigit-1) == '-')
   ||  (lastDigit        && *(lastDigit+1)  == '-')) {
      Res = - Res;
   }
   return (Res );
}
/* ------------------------------------------------------------- */
LONG strutil_packedMem2Int(PUCHAR buf, SHORT bytes)
{
    SHORT i;
    LONG  res  = 0;
    for(i=0;i<bytes-1; i++,buf++) {
       res = 10 * res + (*buf >> 4);
       res = 10 * res + (*buf & 0x0f);
    }
    res = 10 * res + (*buf >> 4);
    if ((*buf & 0x0F) != 0x0F) {
      res = - res;
    }
    return res;
}
/* ------------------------------------------------------------- */
PUCHAR strutil_stripLeadingZeros(PUCHAR out, PUCHAR s)
{
   PUCHAR p = s;
   BOOL   Neg = FALSE;

   if (*p == '-') {
     p++;
     Neg = TRUE;
   }

   // NULL returns 0. Also it could be a single '-' from the abowe
   if (*p == '\0') {
      strcpy(out , "0");
      return out;
   }

   while (*p == '0') {
     p ++;
   }
   if (p > s && ! isdigit(*p)) p --;

   if (Neg) {
     p--;
     *p = '-';
   }
   strcpy(out , p);
   return (out);
}
/* ------------------------------------------------------------- */
PUCHAR strutil_formatPacked(PUCHAR out , PUCHAR in , SHORT len , SHORT prec, UCHAR decPoint)
{
   UCHAR  temp [64];
   PUCHAR pOut = temp;
   SHORT  ByteLen = len / 2 + 1;  /* Bytes required */
   SHORT  HighNibbel = len % 2;
   SHORT  j;

   // Negative when not xF in last lo nibble */
   if ((in[ByteLen-1] & 0x0F)  != 0x0F) {
      *pOut++ = '-';
   }
   for (j=0;j<len;j++) {
      if (j == (len - prec)) {
         *pOut++ = decPoint;
      }
      if (HighNibbel) {
          *pOut++ = '0' + (*in >> 4);
          HighNibbel = FALSE;
      } else {
          *pOut++ = '0' + (*in++ & 0x0f);
          HighNibbel = TRUE;
      }
   }
   *pOut = '\0';
   return(strutil_stripLeadingZeros(out, temp));
}
PUCHAR strutil_formatZoned(PUCHAR out , PUCHAR in , SHORT len , SHORT prec, UCHAR decPoint)
{
   UCHAR  temp [64];
   PUCHAR pOut = temp;
   SHORT  ByteLen = len ;  // Bytes required
   SHORT  j;

   // Negative when not xF in last lo nibble
   if ((in[ByteLen-1] & 0xF0)  != 0xF0) {
      *pOut++ = '-';
   }
   for (j=0;j<len;j++) {
      if (j == (len - prec)) {
         *pOut++ = decPoint;
      }
      *pOut++ = '0' + (*in++ & 0x0f);
   }
   *pOut = '\0';
   return(strutil_stripLeadingZeros(out , temp));
}
// -------------------------------------------------------------
/*
PUCHAR strutil_strDup(PUCHAR s)
{
    PUCHAR p;
    LONG len = strlen(s);
    p = teraspace_alloc (len);
    return p;
}
*/
/* ------------------------------------------------------------- */
PUCHAR strutil_strlastchr(PUCHAR str , UCHAR c)
{
     PUCHAR p, found = NULL;
     while ( p = strchr(str , c )) {
        found = p;
        str = p +1;
     }
     return found;
}
PUCHAR strutil_blob2str(PBLOB pb)
{
    pb->String[pb->Length] = '\0';
    return  pb->String;
}
/* ---------------------------------------------------------------------------------------- */
LONG strutil_strTrimLen(PUCHAR str)
{
    PUCHAR end = str;
    LONG l=0,len=0;
    while (*end) {
       l++;
       if (*end > ' ') {
         len = l ;
       }
       end++;
    }
    return len;
}
/* --------------------------------------------------------------------------- *\
   ASCII - atoi
\* --------------------------------------------------------------------------- */
#pragma convert(1252)
LONG strutil_a2i (PUCHAR s)
{
    int i;
    int ret=0;
    for (i=0;i<15;i++) {
        if (s[i] == ' ') {
        } else if (s[i] >= '0' && s[i] <= '9') {
            ret = ret * 10 + (s[i] - '0');
        } else {
            break;
        }
   }
   return ret;
}
#pragma convert(0)

static void swap(PUCHAR x, PUCHAR y) {
    char t = *x; *x = *y; *y = t;
}
 
static PUCHAR reverse(PUCHAR buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
PUCHAR strutil_itoa(int value, PUCHAR buffer, int base)
{
    // max base = 32
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    int n = abs(value);
 
    int i = 0;
    while (n) {
        int r = n % base;
 
        if (r >= 10) {
            // chars A-Z , 65 = A in ASCII , in EBCDIC : A=193 , J=209 , S=226
            if (r <= 18)
              buffer[i++] = 193 + (r - 10);
            else if (r <= 27)
              buffer[i++] = 209 + (r - 10 - 9);
            else
              buffer[i++] = 226 + (r - 10 - 18);
        }
        else {
            // numbers 0 - 9 , 0=48 in ASCII , 0=240 in EBCDIC
            buffer[i++] = 240 + r; 
        }
 
        n = n / base;
    }
 
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    if (value < 0) buffer[i++] = '-';
 
    buffer[i] = '\0';
 
    return reverse(buffer, 0, i - 1);
}
