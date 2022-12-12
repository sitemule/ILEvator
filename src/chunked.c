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
BOOL receiveChunked(PILEVATOR pIv)
{
	enum {
        GET_LEN,
        SKIP_TO_EOL,
        GET_DATA
	} Mode = GET_LEN;

	int    chunkedlen, Len;
    ULONG maxLen = 1234;  // TODO get that from ANYCHAR in responseDataBuffer
	UCHAR  rcvbuf [4096];
	BOOL   DigitFound = FALSE;
	PUCHAR pInBuf   = pIv->ContentData;
	PUCHAR pOutBuf  = pIv->ContentData;

	pIv->ContentLength =0;
	chunkedlen = 0;

	for(;;) {

		// Get the next block - this is asynchronious from the chrunks
		if (pInBuf == pIv->pCurEnd) {
		    Len = sockets_receive (pIv->pSockets, rcvbuf, sizeof(rcvbuf), 3); // TODO !! Inter block timout !! 
 
			if  (Len <= 0) {  // Data is complete in outbuffer, so "disconnect" is ok (len ==0)
				 return TRUE;
			}
			pInBuf = rcvbuf;
			pIv->pCurEnd = pInBuf + Len;
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
						return; // End-of-file
					}
					Mode = GET_DATA;
				} else {
					Mode = GET_LEN;
				}
			}
			break;

		// Get the data - as the abowe length describes
		case GET_DATA:
			if (chunkedlen > 0) {
				if (pIv->responseDataFile) {
				  fputc (*pInBuf , pIv->responseDataFile);
				} 
                if (pIv->ContentLength < maxLen) {
				  *pOutBuf = *pInBuf;
				  pOutBuf++;
				}
				pIv->ContentLength++;
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
