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
#include "httpclient.h"
#include "ilevator.h"
#include "message.h"
#include "mime.h"
#include "ostypes.h"
#include "parms.h"
#include "request.h"
#include "strutil.h"
#include "simpleList.h"
#include "teraspace.h"
#include "url.h"
#include "varchar.h"
#include "xlate.h"

static UCHAR EOL [] = {CR, LF , 0};

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

/* --------------------------------------------------------------------------- *\
  wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
void putWsTrace(PILEVATOR pIv, PUCHAR ctlStr, ...)
{
    va_list arg_ptr;
    UCHAR   temp[4096];
    UCHAR   temp2[4096];
    LONG    len;
    SHORT   l,i;
    if (pIv->wstrace == NULL) return;

    va_start(arg_ptr, ctlStr);
    len = vsprintf( temp , ctlStr, arg_ptr);
    va_end(arg_ptr);
    xlate_translateBuffer (temp2 , temp , len+1, 0 , 1252); // Incl the null termination
    fputs (temp2 , pIv->wstrace);
}
/* --------------------------------------------------------------------------- *\
  wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
void xsetmsg(PILEVATOR pIv , PUCHAR msgid , PUCHAR Ctlstr, ...)
{
    va_list arg_ptr;
    LONG    len;
    SHORT   l,i;
    va_start(arg_ptr, Ctlstr);
    len = vsprintf( pIv->message , Ctlstr, arg_ptr);
    va_end(arg_ptr);

    putWsTrace(pIv ,"%s" , pIv->message);
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
    int ccsid;
    
    UCHAR valueXlated[100];
    xlate_translateString(valueXlated, mimeType, 1252, 0);
    
    PVARCHAR contentType = teraspace_calloc(sizeof(VARCHAR));
    str2vc(contentType, valueXlated);
    ccsid = iv_mime_getCcsid(contentType);
    teraspace_free(&contentType);

    return ccsid;
}

/* --------------------------------------------------------------------------- */
BOOL isNewLineAscii(UCHAR c)
{
   if (c == 0x0d) return(TRUE);
   if (c == 0x0a) return(TRUE);
   return(FALSE);
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
        pIv->responseHeaderHasContentLength = true;
    }
    
    if (strutil_beginsWithAscii (Parm , "Transfer-Encoding"))  {
        pIv->responseIsChunked = strstr (Value, "chunked") > NULL; //!! TODO striastr 
    }
    
    if (strutil_beginsWithAscii (Parm , "location"))  {
        xlate_translateString(pIv->location , Value , 1252 , 0);
    }
    
    // Unpack: "Content-Type: text/html; charset=windows-1252"
    if (strutil_beginsWithAscii (Parm , "Content-Type"))  {
        pIv->Ccsid = getCcsid(Value);
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
        return -2; /* No response */
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

    pIv->ResponseString = start;
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
    return 0 ; // OK
}
#pragma convert(0)
/* --------------------------------------------------------------------------- *\
   Authentications is base64 encoded userid in ascii
\* --------------------------------------------------------------------------- */
LONG addRealmLogin (PUCHAR pReq, PUCHAR user , PUCHAR password)
{
   UCHAR userAndPasseord [256];
   ULONG pw1Len, pw2Len, pw3Len, len;
   UCHAR pw1 [256];
   UCHAR pw2 [256];
   UCHAR pw3 [256];

   if (*user == '\0' ||  *password == '\0') return 0;

   sprintf(userAndPasseord , "%s:%s" , user , password);
   pw1Len  = xlate_translateBuffer(pw1 , userAndPasseord , strlen(userAndPasseord) , 0 , 1252);
   pw2Len = base64_encodeBuffer(pw2 , pw1, pw1Len);
   pw2[pw2Len] = '\0';
   pw3Len = xlate_translateBuffer(pw3 , pw2 , strlen(pw2) , 1252, 0);
   pw3[pw3Len] = '\0';

   len = sprintf(pReq, "Authorization: Basic %s%s" , pw3, EOL);

   return len;
}
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
    LVARCHAR s;
    
    str2lvc(&s, url);
    l_url = iv_url_parse(s);
    
    pIv->pSockets->asSSL = PLAIN_SOCKET;
    pIv->useProxy = FALSE;
    strcpy(pIv->user, "");
    strcpy(pIv->password, "");

    if (l_url.protocol.Length > 0) {
        pIv->pSockets->asSSL = ( strutil_beginsWith(l_url.protocol.String , "https")) ? SECURE_HANDSHAKE_IMEDIATE: PLAIN_SOCKET;
    }

    strncat(pIv->server , l_url.host.String, l_url.host.Length);
    strutil_itoa(l_url.port, pIv->port, 10);
    strncat(pIv->host , l_url.host.String, l_url.host.Length);
    strcat(pIv->host , ":");
    strcat(pIv->host , pIv->port);
    
    if(l_url.path.Length == 0) {
        strcpy(pIv->resource , "/");
    }
    else {
        strncat(pIv->resource , l_url.path.String, l_url.path.Length);
    }
    
    if (l_url.query.Length > 0) {
        strcat(pIv->resource , "?");
        strncat(pIv->resource , l_url.query.String, l_url.query.Length);
    }
    
    strncat(pIv->user , l_url.username.String, l_url.username.Length);
    strncat(pIv->password, l_url.password.String, l_url.password.Length);
    
    // TODO urlEncodeBlanks (resource , pResource);
    // TODO URL encode path and query
}
/* --------------------------------------------------------------------------- */
API_STATUS sendRequest (PILEVATOR pIv) 
{
    PVOID request;
    VARCHAR12 method;
    VARCHAR1024 acceptMimeType; 
    VARCHAR url;
    LVARPUCHAR requestString;
    LONG rc;

    str2vc(&method, pIv->method);
    method.Length = strlen(method.String);

    xlate_translateBuffer(url.String , pIv->url , strlen(pIv->url) , 0, 1252);
    url.Length = strlen(pIv->url);
    // TODO check if translated length still fits
    
// TODO auth
//    if (*pIv->user > ' ') {
//        pReq += addRealmLogin (pReq, pIv->user , pIv->password);
//    }

    request = iv_request_new(method, url, acceptMimeType);
    requestString = iv_request_toString(&request);
    iv_request_dispose(request);
    
    rc = sockets_send (pIv->pSockets, requestString.String, requestString.Length); 
    teraspace_free((PVOID) requestString.String);

    return (rc == requestString.Length ? API_OK : API_ERROR); 

}

/* --------------------------------------------------------------------------- */
API_STATUS receiveHeader ( PILEVATOR pIv)
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
            sockets_close(pIv->pSockets);
            message_info("Buffer overrun ");
            return API_ERROR;
        }

        // Append to the current buffer
        len = sockets_receive (pIv->pSockets, pIv->bufferEnd, bufferRemain , pIv->timeOut);
        
        // timeout
        if  (len == -2) {  
            return ( pIv->bufferTotalLength > 0 ? API_OK: API_ERROR);
        }

        // Protocol error
        if  (len < 0) {  
            return API_ERROR;
        }

        // "Connection close" - end of transmission and no "Content-Length" provided, the we are done
        if (len == 0 && pIv->responseHeaderHasContentLength == false) {
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
                    sockets_close(pIv->pSockets);
                    message_info("Invalid response header: %d" , err);
                    return API_ERROR;
                }

                if (pIv->status == 301     // Temporary moved
                ||  pIv->status == 302) {  // Permanent moved
                    parseUrl (
                        pIv,
                        pIv->location // New input !!
                    ); 
                    sockets_close(pIv->pSockets);
                    putWsTrace( pIv, "\r\n redirected to %s\r\n",  pIv->location );
                    return API_RETRY;
                }
                
                if (pIv->status == 100) {  // Continue is was a proxy
                    putWsTrace( pIv, "\r\n-- Proxy continue received --\r\n");
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

    for (;;) { // repeat until a header is received
        len = sockets_receive (pIv->pSockets, buffer , sizeof(buffer) , pIv->timeOut);
        
        // timeout
        if  (len == -2) {  
            return API_ERROR;
        }

        // Protocol error
        if  (len < 0) {  
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
