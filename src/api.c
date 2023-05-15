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
#include "sockets.h"
#include "anychar.h"
#include "sockets.h"
#include "ilevator.h"
#include "chunked.h"
#include "base64.h"
#include "teraspace.h"
#include "varchar.h"
#include "strutil.h"
#include "simpleList.h"
#include "message.h"
#include "parms.h"
#include "httpclient.h"
#include "debug.h"

static UCHAR EOL [] = {CR, LF , 0};
#define BUFFER_SIZE 1048576
#define DEFAULT_TIMEOUT 30000
#define NO_RETRIES 0

/* --------------------------------------------------------------------------- */
PILEVATOR iv_newHttpClient(void)
{
    // Get mem and set to zero
    PILEVATOR pIv = teraspace_calloc(sizeof(ILEVATOR));
    pIv->buffer = teraspace_alloc(BUFFER_SIZE);
    pIv->bufferSize = BUFFER_SIZE;
    pIv->sockets =  sockets_new();
    pIv->headerList = sList_new ();
    return (pIv);
}
/* --------------------------------------------------------------------------- */
void iv_free ( PILEVATOR pIv)
{
    if (pIv == NULL) return;
    sockets_free (pIv->sockets);
    sList_free (pIv->headerList);
    teraspace_free (&pIv->authProvider);
    teraspace_free (&pIv->buffer);
    teraspace_free (&pIv);
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
    iv_anychar_set ( 
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
    iv_anychar_set ( 
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
	unlink(strutil_righttrim(fileName)); // Just to reset the CCSID which will not change if file exists
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

    if (0 != access(certificateFile, R_OK)) {
        message_info("Certificate error: %s File: %s:", 
            strerror(errno),
            pIv->sockets->certificateFile
        );
        return OFF;
    }
    
    sockets_setSSL(
        pIv->sockets, 
        SECURE_HANDSHAKE_IMEDIATE,
        certificateFile,
        certificatePassword
    );
    
    return ON;
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
    SHORT try;
    
    pIv->method = method; 
    pIv->url = url; 
    pIv->timeOut = (parms >= 4) ? timeOut : DEFAULT_TIMEOUT;
    pIv->retries = (parms >= 5) ? retries : NO_RETRIES;
    
    parseUrl(pIv, url); 

    for (try = 0; try <= pIv->retries ; try++) {

        BOOL ok = sockets_connect(
            pIv->sockets, 
            pIv->server ,
            atoi (pIv->port) ,
            pIv->timeOut
        ); 
        apiStatus = ( ok? API_OK:API_ERROR);
        if (apiStatus == API_ERROR) break; 

        apiStatus = sendRequest (pIv);
        if (apiStatus == API_ERROR) break; 

        apiStatus = receiveHeader (pIv);
        if (apiStatus == API_ERROR) break; 
        if (apiStatus == API_RETRY) continue; 

        iv_debug("HTTP response status: %s", "unknown"); // TODO set response status code

        // Dont try to get data if it was a HEAD request - it is only the header
        // or status 204 => no content
        if (strutil_beginsWith(pIv->method , "HEAD")  ||  pIv->status == 204 )  {
            // No Content, dont read any longer
            apiStatus = API_OK;
        }
        else if (pIv->responseIsChunked == ON ) {
            apiStatus = iv_chunked_receive(pIv);
        }
        else {
            apiStatus = receiveData (pIv);
        }

        if (apiStatus == API_OK   ) break; 
        if (apiStatus == API_ERROR) break; 
        if (apiStatus == API_RETRY) continue; 

    }

    iv_anychar_finalize (&pIv->responseHeaderBuffer);
    iv_anychar_finalize (&pIv->responseDataBuffer);

    if (pIv->responseDataFile) {
        fclose(pIv->responseDataFile);
    }

    iv_free(pIv);

    return apiStatus == API_OK ? ON : OFF;

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
/* --------------------------------------------------------------------------- */
void iv_setAuthProvider(PILEVATOR pIv, PAUTH_PROVIDER authProvider) 
{
    if (pIv->authProvider) teraspace_free(&pIv->authProvider);
    
    pIv->authProvider = authProvider;
}

void iv_get(PLVARCHAR returnBuffer, VARCHAR url, VARCHAR acceptMimeType, PSLIST headers) 
{
    int parms = _NPMPARMLISTADDR()->OpDescList->NbrOfParms;
    PILEVATOR pIv = iv_newHttpClient();
    
    UCHAR l_url[32767];
    memcpy(&l_url, &url.String, url.Length);
    l_url[url.Length] = '\0';
    
    iv_setResponseDataBuffer(
        pIv, 
        (PVOID) returnBuffer, 
        BUFFER_SIZE,
        IV_VARCHAR4,
        IV_CCSID_UTF8
    );
    
    if (parms >= 3) {
      // TODO handle acceptMimeType
    }
    
    if (parms >= 4) {
      // TODO handle headers
    }
    
    LGL rc = iv_execute (pIv, "GET", &l_url[0], DEFAULT_TIMEOUT, NO_RETRIES);
    // TODO what to do with the result?
    
    iv_free(pIv); // TODO catch any error and free the http client instance
}


