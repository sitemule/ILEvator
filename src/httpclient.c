/* ------------------------------------------------------------- */
/* Program . . . : ILEvator - http client                        */
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
#include <decimal.h>
#include <errno.h>
#include <locale.h>

#include "anychar.h"
#include "base64.h"
#include "debug.h"
#include "httpclient.h"
#include "ilevator.h"
#include "message.h"
#include "mime.h"
#include "ostypes.h"
#include "parms.h"
#include "request.h"
#include "streamer.h"
#include "strutil.h"
#include "simpleList.h"
#include "teraspace.h"
#include "url.h"
#include "varchar.h"
#include "xlate.h"

static UCHAR EOL [] = {CR, LF , 0};

void streamRequest(PILEVATOR pIv, PVOID request);
LONG socketStreamWriter(PSTREAM pStream, PUCHAR buf, ULONG len);


/* --------------------------------------------------------------------------- *\
   wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
LONG urlEncodeBlanks  (PUCHAR outBuf , PUCHAR inBuf)
{
    PUCHAR start = outBuf;
    UCHAR c;
    LONG len = strutil_strTrimLen(inBuf);

    while (len--) {
        c = *inBuf;
        if (c == ' ') {
            outBuf += sprintf(outBuf , "%%20");
        } else {
            *(outBuf++) = c;
        }
        inBuf++;
    }
    *outBuf = '\0';
    return (outBuf - start);
}
/* -------------------------------------------------------------------------- */
PUCHAR findEOL(PUCHAR p)
{
    for (;;) {
        switch (*p) {
            case 0x0d:
            case 0x0a:
            case 0x00:
            return(p);
        }
        p ++;
   }
}

/* --------------------------------------------------------------------------- */
int getCcsid(PUCHAR mimeType)
{
    UCHAR valueXlated[100];
    xlate_translateString(valueXlated, mimeType, 1252, 0);
     
    VARCHAR contentType;
    str2vc(&contentType, valueXlated);
    int ccsid = iv_mime_getCcsid(&contentType);

    return ccsid;
}

/* -------------------------------------------------------------------------- */
BOOL lookForHeader(PUCHAR Buf, LONG totlen, PUCHAR * contentData)
{
    LONG i;
    PUCHAR p;

    for (i=0;i<totlen;i++) {
        p = Buf + i;

        if (p[0] == 0x0d && p[1] == 0x0a
        &&  p[2] == 0x0d && p[3] == 0x0a) {
            *contentData= p + 4;
            *p = 0;
            return TRUE; //found
        } else if ((p[0] == 0x0d && p[1] == 0x0d)
        ||         (p[0] == 0x0a && p[1] == 0x0a)) {
            *contentData= p + 2;
            *p = 0;
            return TRUE; //found
        }
   }
   return FALSE; //not found
}
/* -------------------------------------------------------------------------- */
#pragma convert(1252)
VOID parseHttpParm(PILEVATOR pIv, PUCHAR Parm , PUCHAR Value)
{
    if (strutil_beginsWithAscii (Parm , "Content-Length"))  {
        pIv->contentLength = strutil_a2i(Value);
        pIv->responseHeaderHasContentLength = ON;
    }
    
    if (strutil_beginsWithAscii (Parm , "Transfer-Encoding"))  {
        pIv->responseIsChunked = strstr (Value, "chunked") > NULL ? ON:OFF; //!! TODO striastr 
    }
    
    if (strutil_beginsWithAscii (Parm , "location"))  {
        xlate_translateString(pIv->location , strutil_firstnonblankAscii(Value) , 1252 , 0);
    }
    
    // Unpack: "Content-Type: text/html; charset=windows-1252"
    if (strutil_beginsWithAscii (Parm , "Content-Type"))  {
        pIv->ccsid = getCcsid(Value);
    }
}
#pragma convert(0)

/* -------------------------------------------------------------------------- */
// All constans is in ascii windows-1252
#pragma convert(1252)
SHORT parseResponse(PILEVATOR pIv, PUCHAR buf, PUCHAR contentData)
{
    PUCHAR start, end, p, s1, s2;

    // Only let the out file survive
    // TODO - reset input!!  

    pIv->contentData = contentData;
    pIv->headLen = pIv->contentData - buf;

    // Retrive the HTTP responce
    start = strstr(buf , "HTTP");
    if (start == NULL ) {
        return SOCK_ERROR; /* No response */
    }

    // Find the status code
    s1 = strchr(start ,' ');
    if (s1) {
        s1++;
        s2 = strchr(s1 ,' ');
        if (s2) {
            UCHAR temp [26];
            strncpy(temp , s1  , s2-s1);
            pIv->status = strutil_a2i(temp);
        }
    }

    pIv->rawResponse = start;
    end = findEOL(start);

    while(*end) {
        start = end;
        // set up next loop
        while (*start < ' ') {
            start ++;
        }

        // Do interpretation
        end = findEOL(start);
        p  = strchr(start ,':');
        if (p && end) {
            UCHAR  parmName[256];
            UCHAR  parmValue[65535];
            strutil_substr(parmName , start , p - start);
            strutil_substr(parmValue, p +1  , end - p - 1 );
            parseHttpParm(pIv , parmName, parmValue);
        }
    }
    return SOCK_OK ; // OK
}
#pragma convert(0)
/* -------------------------------------------------------------------------- *\
   returns @ in current CCSID
\* -------------------------------------------------------------------------- */
UCHAR masterspace () 
{
    UCHAR  masterspace;

    #pragma convert(1252)
    xlate_translateBuffer(&masterspace , "@" , 1  , 1252, 0);
    #pragma convert(0)
    return masterspace;
}
/* -------------------------------------------------------------------------- *\
   Parse: http://www.google.com:1234/any.asp&parm1=value1
   Or   : <www.extraprivacy.com:8080>http://www.google.com:1234/any.asp&parm1=value1
   With User id and password:
   http://userid:password@www.google.com:1234/any.asp&parm1=value1

\* -------------------------------------------------------------------------- */
void parseUrl (PILEVATOR pIv, PUCHAR url)
{
    PUCHAR temp;
    URL l_url;
    VARCHAR s;
    
    str2vc(&s, url);
    l_url = iv_url_parse(s);
    
    pIv->sockets->asSSL = ( strutil_beginsWith(l_url.protocol.String , "https")) ? SECURE_HANDSHAKE_IMEDIATE: PLAIN_SOCKET;

    strutil_substr(pIv->server , l_url.host.String, l_url.host.Length);
    pIv->port = l_url.port;
    // sprintf(pIv->host,  "%s:%d" , pIv->server,  pIv->port);
    sprintf(pIv->host,  "%s" , pIv->server);
    
    if(l_url.path.Length == 0) {
        strcpy(pIv->resource , "/");
    }
    else {
        strutil_substr(pIv->resource , l_url.path.String, l_url.path.Length);
    }
    
    if (l_url.query.Length > 0) {
        strcat(pIv->resource , "?");
        strutil_substr(pIv->resource + strlen(pIv->resource) , l_url.query.String, l_url.query.Length);
    }
    
    strutil_substr(pIv->user , l_url.username.String, l_url.username.Length);
    strutil_substr(pIv->password, l_url.password.String, l_url.password.Length);
}
/* --------------------------------------------------------------------------- */
API_STATUS sendRequest (PILEVATOR pIv) 
{
    PVOID request;
    VARCHAR12 method;
    LVARPUCHAR requestString;
    LONG rc;

    str2vc(&method, pIv->method);

    request = iv_request_new(
        method, 
        &pIv->host[0],
        pIv->port,
        &pIv->resource[0],
        ""
    );
    iv_request_addHeaders(request, pIv->headerList);
    iv_request_addHeaders(request, pIv->requestHeaderList);
    
    if (pIv->requestDataBuffer.length > 0) {
        if (pIv->requestDataBuffer.type == IV_ANYCHAR_BYTES)
            iv_request_setBinaryBody(request, pIv->requestDataBuffer.data, pIv->requestDataBuffer.length);
        else
            iv_request_setTextBodyBytes(request, pIv->requestDataBuffer.data, pIv->requestDataBuffer.length, pIv->requestDataBuffer.xlate);
    }
    
    if (pIv->authProvider)
        pIv->authProvider->processRequest(pIv->authProvider, request);
    
    if (pIv->requestHandler)
        pIv->requestHandler->processRequest(pIv->requestHandler, request);
    
    if (iv_request_needsStreaming(&request) == ON) {
        // TODO handle errors
        streamRequest(pIv, request);
        return API_OK;
    }
    else {
        requestString = iv_request_toString(&request);
        iv_request_dispose(request);
    
        rc = sockets_send (pIv->sockets, requestString.String, requestString.Length); 
        teraspace_free((PVOID) requestString.String);
        
        return (rc == requestString.Length ? API_OK : API_ERROR); 
    }
}

static void streamRequest(PILEVATOR pIv, PVOID request) 
{
    // create stream
    PSTREAM socketOutputStream = stream_new(65535); // 64k , TODO how big should we make the buffer
    // set stream write function 
    socketOutputStream->writer = socketStreamWriter;
    // set ILEvator instance
    socketOutputStream->handle = pIv;
  
    iv_request_toStream(&request, &socketOutputStream);
    
    stream_delete (socketOutputStream);
}

static LONG socketStreamWriter(PSTREAM outputStream, PUCHAR buffer, ULONG length)
{
    PILEVATOR pIv = outputStream->handle;
    int rc = sockets_send(pIv->sockets, buffer, length);
    return rc;
}

/* --------------------------------------------------------------------------- */
API_STATUS receiveHeader(PILEVATOR pIv)
{

    BOOL  headFound = FALSE;
    BOOL  headParsed = FALSE;
    LONG  len, curContLen = 0;

    pIv->bufferEnd =  pIv->buffer;
    pIv->bufferTotalLength = 0;
   
    for (;;) { // repeat until a header is received

        LONG bufferRemain = pIv->bufferSize - pIv->bufferTotalLength;
         // Protocol error
        if  (bufferRemain < 0) {  
            sockets_close(pIv->sockets);
            message_info("Buffer overrun ");
            return API_ERROR;
        }

        // Append to the current buffer
        len = sockets_receive (pIv->sockets, pIv->bufferEnd, bufferRemain , pIv->timeOut * 1000);
        
        // timeout
        if  (len == SOCK_TIMEOUT) {  
            return ( pIv->bufferTotalLength > 0 ? API_OK: API_ERROR);
        }

        // Protocol error
        if  (len == SOCK_ERROR ) {  
            return API_ERROR;
        }

        // "Connection close" - end of transmission and no "Content-Length" provided, then we are done
        if (len == 0 && pIv->responseHeaderHasContentLength == OFF) {
            return API_OK;
        }

        pIv->bufferTotalLength += len;
        pIv->bufferEnd  += len;
        *pIv->bufferEnd  = '\0';

        // Header not found yet - Search for it
        if (! headFound ) {
            headFound = lookForHeader(pIv->buffer, pIv->bufferTotalLength, &pIv->contentData);
            if (headFound) {
                int err = parseResponse(pIv , pIv->buffer, pIv->contentData);
                if (err) {
                    sockets_close(pIv->sockets);
                    message_info("Invalid response header: %d" , err);
                    return API_ERROR;
                }

                if (pIv->status == 301     // Temporary moved
                ||  pIv->status == 302     // Permanent moved
                ||  pIv->status == 307) {  // Temporary moved

                    parseUrl (
                        pIv,
                        pIv->location // New input !!
                    ); 

                    // MEGA HACK !! The parsurl is called twice ( WHY WHY WHY !!!) and brigs back the original url 
                    strcpy (pIv->url , pIv->location);

                    // talking to the final end point OR negotiating with the proxy -> close socket
                    if (pIv->proxyTunnel == NULL) sockets_close(pIv->sockets);
                    
                    iv_debug("redirected to %s", pIv->location);
                    return API_RETRY;
                }
                
                if (pIv->status == 100) {  // Continue is was a proxy
                    iv_debug("-- Proxy continue received --");
                    return API_RETRY; // Found but start over again - it was a proxy
                }

                pIv->contentLengthCalculated = pIv->bufferTotalLength - pIv->headLen;
                iv_anychar_append ( &pIv->responseHeaderBuffer , pIv->buffer , pIv->headLen);

                return API_OK; // Got what we came for + perhaps more 

            }
        }
    }
}
/* --------------------------------------------------------------------------- */
API_STATUS receiveData ( PILEVATOR pIv)
{
    UCHAR buffer [65535];
    LONG  len;

    // TODO !! Move this together with the file write; 
    iv_anychar_append ( &pIv->responseDataBuffer ,pIv->contentData , pIv->contentLengthCalculated);

    // Received data for "non chunked" !!! TODO Move to reveice data --- this is not the place
    if (pIv->responseDataFile && pIv->contentLength) {
        fwrite (pIv->contentData , 1, pIv->contentLengthCalculated , pIv->responseDataFile);
    }

    // Was everything in the first socket I/O? 
    if (pIv->responseHeaderHasContentLength == ON 
    &&  pIv->contentLength == pIv->contentLengthCalculated) {
        return API_OK;
    }

    for (;;) { // repeat until all data is complete
        len = sockets_receive (pIv->sockets, buffer , sizeof(buffer) , pIv->timeOut * 1000);
        
        // timeout
        if  (len == SOCK_TIMEOUT) {  
            return API_ERROR;
        }

        // Protocol error
        if  (len == SOCK_ERROR) {  
            return API_ERROR;
        }

        // "Connection close" - end of transmission and no "Content-Length" provided, the we are done
        if (len == 0 && pIv->responseHeaderHasContentLength == false) {
            return API_OK;
        }

        iv_anychar_append ( &pIv->responseDataBuffer, buffer, len);

        if (pIv->responseDataFile) {
            fwrite (buffer  , 1, len , pIv->responseDataFile);
        }
    }
}

/* --------------------------------------------------------------------------- *\
    Setup the local top be POSIX i.e. the charset in ccsid(37) 
\* --------------------------------------------------------------------------- */
void initialize(void)
{
    static BOOL initialized;

    if (initialized) return;
    initialized = true;

    setlocale(LC_CTYPE , "POSIX"); 
}


