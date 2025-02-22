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
#include "decimal.h"
#include "ostypes.h"
#include "simpleList.h"
#include "sockets.h"

#define BUFFER_SIZE 1048576

//
// buffer types
//
#define IV_BYTES 0
#define IV_VARCHAR2 1
#define IV_VARCHAR4 2

//
// Conversion - named values; more exists
//
#define IV_CCSID_JOB     0
#define IV_CCSID_UTF8    1208
#define IV_CCSID_WIN1252 1252
#define IV_CCSID_BINARY  65535

typedef enum _API_STATUS {
    API_OK, 
    API_RETRY,
    API_ERROR
} API_STATUS , *PAPI_STATUS;

typedef void (* iv_processRequest_t) (PVOID handler, PVOID request);

typedef struct _REQUEST_HANDLER {
    iv_processRequest_t processRequest;
} REQUEST_HANDLER, *PREQUEST_HANDLER;

typedef struct _ILEVATOR {
    PSOCKETS     sockets;
    PUCHAR       method; 
    PUCHAR       url; 
    decimal(9,3) timeOut;
    SHORT        retries;
    LGL          useProxy;
    LGL          responseHeaderHasContentLength;
    LGL          responseIsChunked;
    LGL          blockingSockets;
    PSLIST       headerList;
    PSLIST       requestHeaderList;
    PSLIST       responseHeaderList;
                
    ANYCHAR      requestDataBuffer; 
    PUCHAR       requestDataFile;
    ANYCHAR      responseHeaderBuffer; 
    ANYCHAR      responseDataBuffer; 
    FILE *       responseDataFile;

    PREQUEST_HANDLER authProvider;
    PREQUEST_HANDLER requestHandler;

    // TODO - refactor
    LONG         headLen;
    PUCHAR       contentData;
    LONG         contentLength;
    LONG         contentLengthCalculated;
                 
    PUCHAR       rawResponse;
    LONG         ccsid; 
    LONG         status;
                 
    UCHAR        server    [512];
    LONG         port      ;
    UCHAR        resource  [32766];
    UCHAR        host      [512];
    UCHAR        user      [256];
    UCHAR        password  [256];
    UCHAR        location  [256];
    UCHAR        message   [256];
                 
    PUCHAR       buffer; 
    LONG         bufferSize;  
    LONG         bufferTotalLength;   
    PUCHAR       bufferEnd;
    struct _ILEVATOR * proxyTunnel;
    LGL          connected;
    
    // web socket
    ULONG        wsFrameSize;
    LGL          wsKeepRunning;
} ILEVATOR, *PILEVATOR;


// Prototypes
PILEVATOR iv_newHttpClient(void);
void iv_delete(PILEVATOR ps);
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
    PUCHAR url
);
#pragma descriptor (void iv_execute(void))

void iv_get(PLVARCHAR returnBuffer, VARCHAR url, VARCHAR acceptMimeType, PSLIST headers);

#endif
