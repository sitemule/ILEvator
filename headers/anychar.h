typedef enum {
    IV_BYTES = 0,
    IV_VARCHAR2 = 1,
    IV_VARCHAR4 = 2
} IVBUFTYPE , *PIVBUFTYPE;

typedef enum {
    IV_XLATE_NO     = 0,
    IV_XLATE_UTF8   = 1,
    IV_XLATE_EBCDIC = 2
} IVXLATE , *PIVXLATE;

typedef struct _ANYCHAR {
    PUCHAR    data;
    ULONG     size;
    ULONG     length;
    IVBUFTYPE type;
    IVXLATE   xlate;
} ANYCHAR , *PANYCHAR;


/* --------------------------------------------------------------------------- */
void anyCharSet (
    PANYCHAR pAt,
    PUCHAR pBuffer,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
);
void anyCharAppend (
    PANYCHAR pAt,
    PUCHAR   pBuf,
    LONG     length
);
void anyCharFinalize (
    PANYCHAR pAt
);

