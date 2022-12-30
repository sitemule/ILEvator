#ifndef ILEVATOR_ANYCHAR_H
#define ILEVATOR_ANYCHAR_H

#include "ostypes.h"

typedef enum {
    IV_ANYCHAR_BYTES = 0,
    IV_ANYCHAR_VARCHAR2 = 1,
    IV_ANYCHAR_VARCHAR4 = 2
} IV_ANYCHAR_BUFTYPE , *PIV_ANYCHAR_BUFTYPE;

typedef enum {
    IV_ANYCHAR_XLATE_NO     = 0,
    IV_ANYCHAR_XLATE_UTF8   = 1,
    IV_ANYCHAR_XLATE_EBCDIC = 2
} IV_ANYCHAR_XLATE , *PIV_ANYCHAR_XLATE;

typedef struct _ANYCHAR {
    PUCHAR data;
    ULONG size;
    ULONG length;
    IV_ANYCHAR_BUFTYPE type;
    IV_ANYCHAR_XLATE xlate;
} ANYCHAR , *PANYCHAR;


/* --------------------------------------------------------------------------- */
void iv_anychar_set (
    PANYCHAR pAt,
    PUCHAR pBuffer,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
);
void iv_anychar_append ( PANYCHAR pAt, PUCHAR pBuf, LONG length );
void iv_anychar_finalize ( PANYCHAR pAt );

#endif
