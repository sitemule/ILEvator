#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <decimal.h>
#include <errno.h>

#include "ostypes.h"
#include "anychar.h"
#include "simpleList.h"
#include "ilevator.h"
#include "chunked.h"

/* -------------------------------------------------------------------------- */
// Nore - this starts with the buffer already received 
// and continues with its own buffer
/* -------------------------------------------------------------------------- */
API_STATUS iv_chunked_receive(PILEVATOR pIv)
{
    enum {
        GET_LEN,
        SKIP_TO_EOL,
        GET_DATA
    } Mode = GET_LEN;

    int    chunkedlen, Len;
    UCHAR  rcvbuf [4096];
    BOOL   DigitFound = FALSE;
    PUCHAR pInBuf   = pIv->contentData;
    PUCHAR newEnd   = pIv->bufferEnd;

    pIv->contentLength =0;
    chunkedlen = 0;

    for(;;) {

        // Get the next block - this is asynchronious from the chrunks
        if (pInBuf == newEnd) {
            Len = sockets_receive (pIv->pSockets, rcvbuf, sizeof(rcvbuf), pIv->timeOut); 
 
            if  (Len <= 0) {  // Data is complete in outbuffer, so "disconnect" is ok (len ==0)
                return API_OK;
            }
            pInBuf = rcvbuf;
            newEnd = pInBuf + Len;
            pInBuf[Len] = '\0';
        }

        // Get the lenght from: <CR><LF>12345<CR><LF>   where the first <CR><LF> is optional
        switch (Mode) {
        case GET_LEN:
            #pragma convert(1252)
            if (*pInBuf >= '0' && *pInBuf <= '9') {
                chunkedlen = (16 * chunkedlen) + *pInBuf - '0';
                DigitFound = TRUE;
            } else if (*pInBuf >= 'a' && *pInBuf <= 'f') {
                chunkedlen = (16 * chunkedlen) + *pInBuf - 'a' + 10;
                DigitFound = TRUE;
            } else if (*pInBuf >= 'A' && *pInBuf <= 'F') {
                chunkedlen = (16 * chunkedlen) + *pInBuf - 'A' + 10;
                DigitFound = TRUE;
            } else {
                pInBuf--; // Redo the same char in SKIP_TO_EOL mode
                Mode = SKIP_TO_EOL;
            }
            #pragma convert(0)
            break;

        case SKIP_TO_EOL:
            if (*pInBuf == 0x0A) {    // Skip CR  and wait for LF ...
                if (DigitFound){
                    if  (chunkedlen == 0) {
                        return API_OK; // End-of-file
                    }
                    Mode = GET_DATA;
                } else {
                    Mode = GET_LEN;
                }
            }
            break;

        // Get the data - as the above length describes
        // TODO !! Optimize for block data - not byte-by-byte
        case GET_DATA:
            if (chunkedlen > 0) {
                if (pIv->responseDataFile) {
                    fputc (*pInBuf , pIv->responseDataFile);
                } 
                iv_anychar_append ( &pIv->responseDataBuffer ,pInBuf , 1);

                pIv->contentLength++;
                chunkedlen --;
            }
            if (chunkedlen == 0) { // can not be the "else" since we will skip one char
                DigitFound = FALSE;
                Mode = GET_LEN;
            }
            break;
        }
        pInBuf++;
    }
}
