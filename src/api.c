/* ------------------------------------------------------------- */
/* Program . . . : ILEvator - main interface                     */
/* Date  . . . . : 06.12.2022                                    */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : Main Socket server                            */
/*                                                               */
/* By     Date       PTF     Description                         */
/* NL     06.12.2022         New program                         */
/* ------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <decimal.h>
#include <errno.h>
#include <locale.h>


#include "ostypes.h"
#include "anychar.h"
#include "simpleList.h"
#include "ilevator.h"
#include "chunked.h"
#include "xlate.h"
#include "base64.h"


#include "teramem.h"
#include "varchar.h"
#include "strutil.h"
#include "streamer.h"
#include "simpleList.h"
#include "sndpgmmsg.h"
#include "parms.h"
#include "httpclient.h"

static UCHAR EOL [] = {CR, LF , 0};

/* --------------------------------------------------------------------------- */
PILEVATOR iv_newHttpClient(void)
{
    // Get mem and set to zero
    PILEVATOR pIv = memCalloc(sizeof(ILEVATOR));
    pIv->buffer = memAlloc(BUFFER_SIZE);
    pIv->bufferSize = BUFFER_SIZE;
    pIv->pSockets =  sockets_new();
    pIv->headerList = sList_new ();
    return (pIv);
}
/*
void  xxx_iv_newHttpClient(PILEVATOR *ppIv)
{
    // Get mem and set to zero
    PILEVATOR pIv = memCalloc(sizeof(ILEVATOR));
    pIv->buffer = memAlloc(BUFFER_SIZE);
    pIv->bufferSize = BUFFER_SIZE;
    pIv->pSockets =  sockets_new();
    pIv->headerList = sList_new ();
    ppIv = pIv;

}
*/ 
/* --------------------------------------------------------------------------- */
void iv_delete ( PILEVATOR pIv)
{
    if (pIv == NULL) return;
    sockets_free (pIv->pSockets);
    sList_free (pIv->headerList);
    memFree (&pIv->buffer);
    memFree (&pIv);
}
/* --------------------------------------------------------------------------- */
void iv_setRequestHeaderBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
)
{
    anyCharSet ( 
        &pIv->requestHeaderBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferXlate
    );
}
/* --------------------------------------------------------------------------- */
void iv_setRequestDataBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
)
{
    anyCharSet ( 
        &pIv->requestDataBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferXlate
    );
}
/* --------------------------------------------------------------------------- */
void iv_setResponseHeaderBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
)
{
    anyCharSet ( 
        &pIv->responseHeaderBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferXlate
    );
}
/* --------------------------------------------------------------------------- */
void iv_setResponseDataBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    SHORT bufferXlate
)
{
    anyCharSet ( 
        &pIv->responseDataBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferXlate
    );
}
/* --------------------------------------------------------------------------- */
LGL iv_setCertificate  (
    PILEVATOR pIv,
    PUCHAR certificateFile,
    PUCHAR certificatePassword
)
{
    int res;
    strcpy  (pIv->pSockets->certificateFile, certificateFile);
    strcpy  (pIv->pSockets->keyringPassword, certificatePassword);

    res = access ( pIv->pSockets->certificateFile ,  R_OK); 
    if (res == 0) {
        return ON;
    } else {
        iv_joblog( "Certificate error: %s File: %s:", 
            strerror(errno),
            pIv->pSockets->certificateFile
        );
        return OFF;
    }
}
/* --------------------------------------------------------------------------- */
LGL iv_execute (
    PILEVATOR pIv,
    PUCHAR method,
    PUCHAR url,
    ULONG  timeOut
)
{

    PNPMPARMLISTADDRP pParms = _NPMPARMLISTADDR();
    API_STATUS apiStatus = API_RETRY; 
    SHORT retry;
    
    pIv->method = method; 
    pIv->url = url; 
    pIv->timeOut = timeOut; 
    pIv->retryMax = 3;  // TOOD for now  

    parseUrl (
        pIv,
        url,
        pIv->server ,
        pIv->port ,
        pIv->resource ,
        pIv->host ,
        pIv->user ,
        pIv->password
    ); 

    for (retry = 0; retry < pIv->retryMax ; retry++) {

        BOOL ok = sockets_connect(
            pIv->pSockets, 
            pIv->server ,
            atoi (pIv->port) ,
            pIv->timeOut
        ); 
        apiStatus = ( ok? API_OK:API_ERROR);
        if (apiStatus == API_ERROR) break; 


        apiStatus = sendHeader (pIv);
        if (apiStatus == API_ERROR) break; 

        apiStatus = receiveHeader (pIv);
        if (apiStatus == API_ERROR) break; 
        if (apiStatus == API_RETRY) continue; 

        // Dont try to get data if it was a HEAD request - it is only the header
        // or status 204 => no content
        if (beginsWith(pIv->method , "HEAD")
        ||  pIv->status == 204 )     {  // No Content, dont read any longer
            apiStatus = API_OK;
        }
        else if (pIv->responseIsChunked) {
            apiStatus = receiveChunked(pIv);
        }
        else {
            apiStatus = receiveData (pIv);
        }

        if (apiStatus == API_OK   ) break; 
        if (apiStatus == API_ERROR) break; 
        if (apiStatus == API_RETRY) continue; 

    }

    anyCharFinalize (&pIv->requestHeaderBuffer);
    anyCharFinalize (&pIv->requestDataBuffer);
    anyCharFinalize (&pIv->responseHeaderBuffer);
    anyCharFinalize (&pIv->responseDataBuffer);

    iv_delete ( pIv);

    return (apiStatus == API_OK ? ON:OFF);

}

/* --------------------------------------------------------------------------- */
void iv_addHeader ( PILEVATOR pIv, PVARCHAR headerName, PVARCHAR headerValue)
{
    UCHAR header[65535];
    PUCHAR p = header;
    LONG len;

    p += cpymem (p, headerName->String, headerName->Length);
    p += cpystr (p , ": ");
    p += cpymem (p, headerValue->String, headerValue->Length);

    *p = '\0';
    p++;

    len = p - header;
    sList_push(pIv->headerList, len, header, OFF);
}

/* --------------------------------------------------------------------------- */
PUCHAR iv_getFileExtension  (PVARCHAR256 extension, PVARCHAR fileName)
{
    PUCHAR f = vc2str(fileName);
    PUCHAR temp, ext = f;

    for(;;) {
        temp = strchr ( ext  , '.');
        if (temp == NULL) break;
        ext = temp +1;
    }
    if (ext == f) {
        str2vc(extension , "");
        return;
    }
    str2vc(extension , ext);
    return extension->String;
}
/* --------------------------------------------------------------------------- */
SHORT iv_getStatus ( PILEVATOR pIv)
{
   return pIv->status; 
}
