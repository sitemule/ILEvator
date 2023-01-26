/* ------------------------------------------------------------- */
/* Program . . . : ILEVATOR                                      */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : web client SSL/ socket wrapper                */
/*                                                               */
/* By     Date       Task    Description                         */
/* NL     15.05.2005 0000000 New program                         */
/* NL     25.02.2007     510 Ignore namespace for WS parameters  */
/* ------------------------------------------------------------- */
#ifndef ILEVATOR_H
#define ILEVATOR_H

#include "anychar.h"
#include "ostypes.h"
#include "simpleList.h"
#include "sockets.h"

#define BUFFER_SIZE 1048576

typedef enum _API_STATUS {
    API_OK, 
    API_RETRY,
    API_ERROR
} API_STATUS , *PAPI_STATUS;


typedef struct _ILEVATOR {
    PSOCKETS  pSockets;
    PUCHAR    method; 
    PUCHAR    url; 
    SHORT     timeOut;
    SHORT     retries;
    BOOL      useProxy;
    BOOL      responseHeaderHasContentLength;
    BOOL      responseIsChunked;
    PSLIST    headerList;

    ANYCHAR   requestHeaderBuffer; 
    FILE *    requestHeaderFile;
    ANYCHAR   requestDataBuffer; 
    FILE *    requestDataFile;
    LONG      requestLength;

    ANYCHAR   responseHeaderBuffer; 
    FILE *    responseHeaderFile;
    ANYCHAR   responseDataBuffer; 
    FILE *    responseDataFile;
    LONG      responseLength;

    // TODO - refactor
    LONG      headLen;
    PUCHAR    contentData;
    LONG      contentLength;
    LONG      contentLengthCalculated;
    
    PUCHAR    ResponseString;
    LONG      Ccsid; 
    LONG      status;

    PUCHAR    pResBuffer; 
    ULONG     resBufferSize;
    IV_ANYCHAR_BUFTYPE resBufferType;
    IV_ANYCHAR_XLATE   resBufferXlate;
    FILE *    resFile;
    FILE *    wstrace;

    UCHAR     server    [512];
    UCHAR     port      [7];
    UCHAR     resource  [32766];
    UCHAR     host      [512];
    UCHAR     user      [256];
    UCHAR     password  [256];
    UCHAR     location  [256];
    UCHAR     message   [256];

    PUCHAR    buffer; 
    LONG      bufferSize;  
    LONG      bufferTotalLength;   
    PUCHAR    bufferEnd;

} ILEVATOR, *PILEVATOR;


// Prototypes
//void iv_newHttpClient(PILEVATOR * ppIv);
PILEVATOR iv_newHttpClient(void);
void  iv_delete(PILEVATOR ps);
void iv_setResponseBuffer (
    PILEVATOR pIv,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
);  

LGL iv_execute (
    PILEVATOR pIv,
    PUCHAR method,
    PUCHAR url,
    ULONG  timeOut,
    ULONG  retries
);


#endif
