#ifndef strutil_H
#define strutil_H

BOOL   strutil_beginsWithAscii(PUCHAR a, PUCHAR b);
BOOL   strutil_beginsWith(PUCHAR a, PUCHAR b);
SHORT  strutil_strIcmp (PUCHAR s1, PUCHAR s2);
SHORT  strutil_memIcmp (PUCHAR s1, PUCHAR s2 , LONG len);
SHORT  strutil_memicmpascii(PUCHAR m1  , PUCHAR m2 , LONG len );
UCHAR  strutil_toLower(UCHAR c);
UCHAR  strutil_toUpper(UCHAR c);
UCHAR  strutil_toupperascii(UCHAR c);
UCHAR  strutil_tolowerascii(UCHAR c);
LONG   strutil_strTrimLen(PUCHAR str);
LONG   strutil_strtrim(PUCHAR str);
PUCHAR strutil_charReplace(PUCHAR out , PUCHAR in , PUCHAR from , PUCHAR to );
LONG   strutil_bufferReplace(PUCHAR buffer , LONG len , PUCHAR from , PUCHAR to );
PUCHAR strutil_formatPacked(PUCHAR out , PUCHAR in , SHORT len , SHORT prec, UCHAR decPoint);
PUCHAR strutil_formatZoned(PUCHAR out , PUCHAR in , SHORT len , SHORT prec, UCHAR decPoint);
PUCHAR strutil_stripLeadingZeros(PUCHAR out, PUCHAR input);
PUCHAR strutil_str2lower(PUCHAR out , PUCHAR in);
PUCHAR strutil_str2upper(PUCHAR out , PUCHAR in);
PUCHAR strutil_subword (PUCHAR out , PUCHAR in , LONG ix, PUCHAR delimiters);
LONG   strutil_subwords (PUCHAR in , PUCHAR  delimiters);
// LONG   subwords (PVARCHAR inputStr, PUCHAR  delimiters);
PUCHAR strutil_stristr(PUCHAR base, PUCHAR key );
PUCHAR strutil_strlastChar(PUCHAR base, UCHAR c );
PUCHAR strutil_memstr(PUCHAR base, PUCHAR key , LONG len);
PUCHAR strutil_memistr(PUCHAR base, PUCHAR key , LONG len);
PUCHAR strutil_strtrimncpy(PUCHAR out , PUCHAR in , LONG maxlen);
PUCHAR strutil_strtrimcpy(PUCHAR out , PUCHAR in);
PUCHAR strutil_righttrimcpy(PUCHAR dst, PUCHAR src);
PUCHAR strutil_righttrimncpy(PUCHAR dst, PUCHAR src, LONG len);

LONG strutil_cpystr (PUCHAR out , PUCHAR in);
LONG strutil_cpymem (PUCHAR out , PUCHAR in, LONG len);

PUCHAR strutil_trim(PUCHAR in);
PUCHAR strutil_firstnonblank(PUCHAR in);
PUCHAR strutil_lastnonblank(PUCHAR in);
PUCHAR strutil_righttrim(PUCHAR in);
PUCHAR strutil_righttrimlen(PUCHAR in , LONG size);
LONG   strutil_lenrighttrimlen(PUCHAR in , LONG size);
PUCHAR strutil_substr(PUCHAR out , PUCHAR in , LONG len);
PUCHAR strutil_padncpy(PUCHAR dst, PUCHAR src, SHORT dstlen);
PUCHAR strutil_pad(PUCHAR s , LONG l);
PUCHAR strutil_blob2str   (PBLOB blob);
ULONG strutil_hexstr2int (PUCHAR s);
UCHAR strutil_hexchar2int (UCHAR c);
PUCHAR strutil_binMem2Hex (PUCHAR out , PUCHAR in , LONG len);
PUCHAR strutil_hex2BinMem (PUCHAR out , PUCHAR in , LONG len);
FIXEDDEC strutil_str2dec(PUCHAR str, UCHAR decPoint);
LONG strutil_packedMem2Int(PUCHAR buf, SHORT bytes);
PUCHAR strutil_memmem  (PUCHAR heystack , ULONG haystackLen, PUCHAR needle, ULONG needleLen);
LONG strutil_a2i (PUCHAR s);

#endif
