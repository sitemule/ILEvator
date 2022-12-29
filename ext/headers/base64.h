#ifndef BASE64_H
#define BASE64_H
void base64_encode (PLVARCHAR output, PLVARCHAR input );
void base64_decode (PLVARCHAR output, PLVARCHAR input );
LONG base64_encodeBuffer (PUCHAR output, PUCHAR input , LONG inputLength );
LONG base64_decodeBuffer (PUCHAR output, PUCHAR input , LONG inputLength );
#endif
