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
#include "sockets.h"
#include "ilevator.h"
#include "chunked.h"
#include "xlate.h"
#include "base64.h"
#include "teraspace.h"
#include "varchar.h"
#include "strutil.h"
#include "simpleList.h"
#include "message.h"
#include "parms.h"
#include "httpclient.h"

static UCHAR EOL [] = {CR, LF , 0};

/* --------------------------------------------------------------------------- */
PILEVATOR iv_newHttpClient(void)
{
    // Get mem and set to zero
    PILEVATOR pIv = teraspace_calloc(sizeof(ILEVATOR));
    pIv->buffer = teraspace_alloc(BUFFER_SIZE);
    pIv->bufferSize = BUFFER_SIZE;
    pIv->pSockets =  sockets_new();
    pIv->headerList = sList_new ();
    return (pIv);
}
/* --------------------------------------------------------------------------- */
void iv_delete ( PILEVATOR pIv)
{
    if (pIv == NULL) return;
    sockets_free (pIv->pSockets);
    sList_free (pIv->headerList);
    teraspace_free (&pIv->buffer);
    teraspace_free (&pIv);
}
/* --------------------------------------------------------------------------- */
void iv_setRequestHeaderBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    LONG  bufferCcsid
)
{
    anyCharSet ( 
        &pIv->requestHeaderBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferCcsid
    );
}
/* --------------------------------------------------------------------------- */
void iv_setRequestDataBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    LONG  bufferCcsid
)
{
    anyCharSet ( 
        &pIv->requestDataBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferCcsid
    );
}
/* --------------------------------------------------------------------------- */
void iv_setResponseHeaderBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    LONG  bufferCcsid
)
{
    anyCharSet ( 
        &pIv->responseHeaderBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferCcsid
    );
}
/* --------------------------------------------------------------------------- */
void iv_setResponseDataBuffer (
    PILEVATOR pIv ,
    PUCHAR pBuf,
    LONG  bufferSize,
    SHORT bufferType,
    LONG  bufferCcsid
)
{
    anyCharSet ( 
        &pIv->responseDataBuffer,
        pBuf,
        bufferSize,
        bufferType,
        bufferCcsid
    );
}
/* --------------------------------------------------------------------------- */
void iv_setResponseFile (
    PILEVATOR pIv ,
    PUCHAR fileName,
    LONG   ccsid
)
{
    int parms = _NPMPARMLISTADDR()->OpDescList->NbrOfParms;
    UCHAR   mode[32];

    ccsid = (parms >=3) ? parms : 1252;   

    sprintf(mode , "wb,o_ccsid=%d", ccsid);
	unlink  ( strutil_righttrim(fileName)); // Just to reset the CCSID which will not change if file exists
	pIv->responseDataFile  = fopen ( strutil_righttrim(fileName) , mode );
	if (pIv->responseDataFile == NULL) {
        message_info("Response output open failed: %s" , strerror(errno));
	}
}
/* --------------------------------------------------------------------------- */
// sets and validates the usages of the certificate file 
/* --------------------------------------------------------------------------- */
LGL iv_setCertificate  (
    PILEVATOR pIv,
    PUCHAR certificateFile,
    PUCHAR certificatePassword
)
{
    int parms = _NPMPARMLISTADDR()->OpDescList->NbrOfParms;

    strcpy  (pIv->pSockets->certificateFile, certificateFile);
    strcpy  (pIv->pSockets->keyringPassword , (parms >=3) ? certificatePassword: "");

    if (0 == access ( pIv->pSockets->certificateFile ,  R_OK)) {
        return ON;
    } else {
        message_info( "Certificate error: %s File: %s:", 
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
    ULONG  timeOut,
    ULONG  retries   
)
{
    int parms = _NPMPARMLISTADDR()->OpDescList->NbrOfParms;
    API_STATUS apiStatus = API_RETRY; 
    SHORT retry;
    
    pIv->method = method; 
    pIv->url = url; 
    pIv->timeOut = (parms >= 4) ? timeOut : 30000; 
    pIv->retries = (parms >= 5) ? retries : 3;
    
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

    for (retry = 0; retry < pIv->retries ; retry++) {

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
        if (strutil_beginsWith(pIv->method , "HEAD")
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

    if (pIv->responseDataFile) {
        fclose(pIv->responseDataFile);
    }

    iv_delete ( pIv);

    return (apiStatus == API_OK ? ON:OFF);

}

/* --------------------------------------------------------------------------- */
void iv_addHeader ( PILEVATOR pIv, PVARCHAR headerName, PVARCHAR headerValue)
{
    UCHAR header[65535];
    PUCHAR p = header;
    LONG len;

    p += strutil_cpymem (p, headerName->String, headerName->Length);
    p += strutil_cpystr (p , ": ");
    p += strutil_cpymem (p, headerValue->String, headerValue->Length);

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
