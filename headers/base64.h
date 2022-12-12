#ifndef BASE64_H
#define BASE64_H
void iv_base64Encode (PLVARCHAR output, PLVARCHAR input );
void iv_base64Decode (PLVARCHAR output, PLVARCHAR input );
LONG iv_base64EncodeBuf (PUCHAR output, PUCHAR input , LONG inputLength );
LONG iv_base64DecodeBuf (PUCHAR output, PUCHAR input , LONG inputLength );
#endif
