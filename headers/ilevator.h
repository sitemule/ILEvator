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

#include "ostypes.h"
#include "sockets.h"



typedef struct _ILEVATOR {
    PSOCKETS  pSockets;
    PUCHAR    method; 
    PUCHAR    url; 
    SHORT     timeOut;
    BOOL      retry;
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
    LONG      HeadLen;
    PUCHAR    ContentData;
    LONG      ContentLength;
    PUCHAR    ResponseString;
    LONG      Ccsid; 
    LONG      status;
    BOOL      AsHttps;
    PUCHAR    headerPCurEnd;
    PUCHAR    pCurEnd;

    PUCHAR    pResBuffer; 
    ULONG     resBufferSize;
    IVBUFTYPE resBufferType;
    IVXLATE   resBufferXlate;
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

} ILEVATOR, *PILEVATOR;


// Prototypes
PILEVATOR il_newClient(void);
void  il_delete(PILEVATOR ps);
void iv_setResponseBuffer (
    PILEVATOR pIv,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
);  

LGL iv_do (
    PILEVATOR pIv,
    PUCHAR method,
    PUCHAR url,
    ULONG  timeOut
);


#endif
