#include <stdio.h>
#include <string.h>

#include "ostypes.h"
#include "anychar.h"

/* --------------------------------------------------------------------------- */
void iv_anychar_set (
    PANYCHAR pAt,
    PUCHAR pBuffer,
    ULONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
)
{
    pAt->type  = bufferType;
    pAt->xlate = bufferXlate;
    pAt->length = 0;
    pAt->type = bufferType;
    
    switch (pAt->type) {
        case  IV_ANYCHAR_BYTES    :  {
            pAt->size = bufferSize ; 
            pAt->data = pBuffer;
            break;

        }
        case  IV_ANYCHAR_VARCHAR2 : {
            pAt->size = bufferSize - 2; 
            pAt->data = pBuffer + 2;
            break;
        }
        case  IV_ANYCHAR_VARCHAR4 : {
            pAt->size = bufferSize - 4; 
            pAt->data = pBuffer + 4;
            break;
        }
    }    
}
/* --------------------------------------------------------------------------- */
void iv_anychar_append ( PANYCHAR pAt, PUCHAR pBuf, ULONG length )
{
    ULONG bytesLeft;
    if (pAt->size == 0 ) return;

    bytesLeft = pAt->size - pAt->length; 
    if (length > bytesLeft) length = bytesLeft;
    memcpy ( pAt->data + pAt->length , pBuf , length);
    pAt->length += length;
}

/* --------------------------------------------------------------------------- */
void iv_anychar_finalize ( PANYCHAR pAt )
{
    if (pAt->size == 0 ) return;

    switch (pAt->type) {
        case  IV_ANYCHAR_VARCHAR2 : {
            * (PUSHORT) (pAt->data - 2) = pAt->length;
            break;
        }
        case  IV_ANYCHAR_VARCHAR4 : {
            * (PULONG) (pAt->data - 4) = pAt->length;
            break;
        }
    }    

}
/* --------------------------------------------------------------------------- */
ULONG iv_anychar_get_length ( PANYCHAR pAt )
{
    if (pAt->size == 0 ) return 0;
    return pAt->length;

}
