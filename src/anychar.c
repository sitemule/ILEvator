/* --------------------------------------------------------------------------- */
void anyCharSet (
    PANYCHAR pAt,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
)
{
    pAt->type  = bufferType;
    pAt->xlate = bufferXlate;
    pAt->length = 0;
    pAt->type = bufferType;
    
    switch (pAt->type) {
        case  IV_BYTES    :  {
            pAt->size = bufferSize ; 
            pAt->data = pBuffer;
            break;

        }
        case  IV_VARCHAR2 : {
            pAt->size = bufferSize + 2; 
            pAt->data = pBuffer + 2;
            break;
        }
        case  IV_VARCHAR4 : {
            pAt->size = bufferSize +4; 
            pAt->data = pBuffer + 4;
            break;
        }
    }    
}
/* --------------------------------------------------------------------------- */
void anyCharAppend (
    PANYCHAR pAt,
    PUCHAR   pBuf,
    LONG     length
)
{
    LONG bytesLeft , bytesToCopy;
    if (pAt->size == 0 ) return;

    bytesLeft = pAt->size - pAt->length; 
    if (length > bytesLeft) length = bytesLeft;
    memcpy ( pAt->data + pAt->length , pBuf , length);
    pAt->length += length;

}

/* --------------------------------------------------------------------------- */
void anyCharFinalize (
    PANYCHAR pAt
)
{
    if (pAt->size == 0 ) return;

    switch (pAt->type) {
        case  IV_VARCHAR2 : {
            * (PUSHORT) (pAt->data - 2) = pAt->length;
            break;
        }
        case  IV_VARCHAR4 : {
            * (PUSHORT) (pAt->data - 4) = pAt->length;
            break;
        }
    }    

}
