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

/* --------------------------------------------------------------------------- *\
   wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
LONG urlEncodeBlanks  (PUCHAR outBuf , PUCHAR inBuf)
{
    PUCHAR start = outBuf;
    UCHAR c;
    LONG len = strTrimLen(inBuf);

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
    xlateBuf (temp2 , temp , len+1, 0 , 1252); // Incl the null termination
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
/* -------------------------------------------------------------------------- */
int charset2ccsid(PUCHAR charset)
{

    int ccsid;
    if (charset == NULL
    ||  beginsWith(charset, "*NONE")
    ||  beginsWith(charset, "0")) {
        ccsid     = 0;
    } else if (beginsWith(charset, "*UTF8")
    ||         beginsWith(charset, "UTF-8")) {
        ccsid     = 1208;
    } else if (beginsWith(charset, "*ISO8859")
    ||         beginsWith(charset, "ISO8859")
    ||         beginsWith(charset, "ISO-8859")) {
        ccsid   = 819;
    } else if (beginsWith(charset , "*WIN1252")
    ||         beginsWith(charset , "WINDOWS-1252")) {
        ccsid   = 1252;
    } else {
        ccsid   = 1252;
    }
    return ccsid;
}

/* --------------------------------------------------------------------------- */
int getCcsid(PUCHAR base , PUCHAR charsetStr, BOOL isASCII)
{
    PUCHAR charset  = stristr(base, charsetStr);

    if (charset) {
        charset += strlen(charsetStr);  // Length of "charset=" without zeroterm
        if (isASCII) {
            UCHAR tempcharSet[32];
            xlateStr (tempcharSet , charset  , 1252 , 0);
            return charset2ccsid(tempcharSet);
        } else {
            return charset2ccsid(charset);
        }
    }
    return 0;
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
    if (beginsWithAscii (Parm , "Content-Length"))  {
        pIv->contentLength = a2i(Value);
        pIv->responseHeaderHasContentLength = true;
    }
    if (beginsWithAscii (Parm , "Transfer-Encoding"))  {
        pIv->responseIsChunked = strstr (Value, "chunked") > NULL; //!! TODO striastr 
    }
    if (beginsWithAscii (Parm , "location"))  {
        xlateStr(pIv->location , Value , 1252 , 0);
    }
    // Unpack: "Content-Type: text/html; charset=windows-1252"

    if (beginsWithAscii (Parm , "Content-Type"))  {
        pIv->Ccsid = getCcsid(Value, "charset=", /*IsASCII=*/ TRUE);
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
            pIv->status = a2i(temp);
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
            substr(parmName , start , p - start);
            substr(parmValue, p +1  , end - p - 1 );
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
   pw1Len  = xlateBuf(pw1 , userAndPasseord , strlen(userAndPasseord) , 0 , 1252);
   pw2Len = iv_base64EncodeBuf(pw2 , pw1, pw1Len);
   pw2[pw2Len] = '\0';
   pw3Len = xlateBuf(pw3 , pw2 , strlen(pw2) , 1252, 0);
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
    xlateBuf(&masterspace , "@" , 1  , 1252, 0);
    #pragma convert(0)
    return masterspace;
}
/* -------------------------------------------------------------------------- *\
   Parse: http://www.google.com:1234/any.asp&parm1=value1
   Or   : <www.extraprivacy.com:8080>http://www.google.com:1234/any.asp&parm1=value1
   With User id and password:
   http://userid:password@www.google.com:1234/any.asp&parm1=value1

\* -------------------------------------------------------------------------- */
void parseUrl (
    PILEVATOR pIv,
    PUCHAR url, 
    PUCHAR server , 
    PUCHAR port , 
    PUCHAR resource, 
    PUCHAR host, 
    PUCHAR user, 
    PUCHAR password)
{
    PUCHAR pProtocol;
    PUCHAR pServer;
    PUCHAR pPort;
    PUCHAR pResource;
    PUCHAR pProxy = NULL;
    PUCHAR pProxyEnd = NULL;
    PUCHAR pTemp;
    PUCHAR pUrlParm;

    pIv->pSockets->asSSL =  PLAIN_SOCKET;
    pIv->useProxy = FALSE;
    strcpy (user, "");
    strcpy (password, "");

    // Skip blanks
    for (; *url == ' '; url++);

    // No remote server was given ... just use loopback
    /* 
    if (url[0] == '/') {
        int PortNum = pSvr->SVPORT;
        sprintf(Port, "%d" , PortNum);
        strcpy(Server, "127.0.0.1");
        UrlEncodeBlanks (Resource , url );
        sprintf(Host , "%s:%d" , Server , PortNum);
        AsHttps = pSvr->SVPROT == 1;
        return;
    }
    */ 

    // Skip the parm for now, so they dont interrupt the parsing
    pUrlParm  = strchr (url, '?');
    if (pUrlParm) {
        *pUrlParm = '\0';
    }

    // Detect if a proxy parameter is given
    if (url[0] == '<') {
        pProxyEnd = strstr(url , ">");  // Skip until > is found
        if (pProxyEnd == NULL) return; // TODO Log invalid proxy syntax missing > after proxy name
        pProxy = url + 1;
        url = pProxyEnd + 1; // Skip the proxy from the url
        for (; *url == ' '; url++); // Skip the leading blanks in url
    }

    pProtocol = strstr(url , "://");
    if (pProtocol) {
        pIv->pSockets->asSSL = ( beginsWith(url , "https")) ? SECURE_HANDSHAKE_IMEDIATE: PLAIN_SOCKET;
        pServer = pProtocol + 3;
    } else {
        pServer = url;
    }
    

    pTemp = strchr(pServer , masterspace());
    if (pTemp) {
        UCHAR userPassword [256];
        PUCHAR pPassword;
        substr(userPassword , pServer , pTemp - pServer);
        pPassword  = strchr(userPassword  , ':');
        if (pPassword) {
            substr(user, userPassword ,  pPassword - userPassword );
            strcpy (password, pPassword+1);
        }
        pServer = pTemp +1;
    }

    pPort      = strstr(pServer , ":");
    pResource  = strstr(pServer , "/");

    // Now put the parms back
    if (pUrlParm) {
        *pUrlParm = '?';
    }

    if (pResource) {
        urlEncodeBlanks (resource , pResource);
    } else {
        strcpy(resource , "/");
    }

    // Pick up the port
    if (pPort) {
        if (pResource) {
            substr (port, pPort + 1, pResource - pPort -1);
        } else {
            strcpy (port, pPort + 1);
        }
    } else {
        if (pIv->pSockets->asSSL == PLAIN_SOCKET) {
            strcpy (port, "80");
        } else {
            strcpy (port , "443");
        }
    }

    // Pick up the server
    if (pPort) {
        substr ( server , pServer , pPort - pServer);
    } else if (pResource) {
        substr ( server , pServer , pResource - pServer);
    } else if (pUrlParm) {
        substr ( server , pServer , pUrlParm - pServer);
    } else {
        strrighttrimcpy(server , pServer);
    }

    // Pick up host (server + port)
    if (pPort) {
        sprintf(host , "%s:%s" , server , port);
    } else {
        strcpy (host , server);
    }

    // pick up the non standart proxy was given between a < and a >
    if (pProxy) {
        UCHAR  proxy [256];
        pIv->useProxy = TRUE;
        pIv->pSockets->asSSL = PLAIN_SOCKET;
        substr(proxy, pProxy , pProxyEnd - pProxy);
        pPort  = strstr(proxy , ":");
        if (pPort) {
            strcpy ( port , pPort + 1);
            substr ( server , proxy , pPort - proxy);
        } else {
            strcpy ( port   , "8080");
            strcpy ( server , proxy);
        }
        urlEncodeBlanks (resource , pProxyEnd  + 1);
    }
}
/* --------------------------------------------------------------------------- */
API_STATUS sendHeader (PILEVATOR pIv) 
{

    UCHAR buffer [65535];
    PUCHAR pReq;
    LONG len, rc;

    pReq = buffer;
    pReq += sprintf(pReq, "%s %s HTTP/1.1%s", pIv->method , pIv->useProxy ? pIv->url : pIv->resource , EOL);

    if (*pIv->user > ' ') {
        pReq += addRealmLogin (pReq, pIv->user , pIv->password);
    }
    if (pIv->requestLength > 0) {
        pReq += sprintf(pReq, "Content-Length: %d%s" , pIv->requestLength , EOL);
    }
    pReq += sprintf(pReq, "User-Agent: ILEvator%s", EOL);
    pReq += sprintf(pReq, "Host: %s%s", pIv->host, EOL);
    //pReq += sprintf(pReq, "Connection: Close%s", EOL);
    pReq += sprintf(pReq, "Connection: keep-alive%s", EOL);

    // Todo !! Headers;
    SLISTITERATOR iterator = sList_setIterator(pIv-> headerList);
    while (sList_foreach(&iterator) == ON) {
        PSLISTNODE node = iterator.this;
        pReq += sprintf(pReq, "%s%s", iterator.this->payloadData , EOL);
    }


    len = pReq - buffer;
    len = xlateBuf(buffer , buffer , len , 0 , 1252); // simple SBCS conversion !! Perhaps UTF-8


    if (isNewLineAscii( * (pReq -1))) {
        pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
    } else {
        pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
        pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
    }
    len = pReq - buffer;

    rc = sockets_send (pIv->pSockets, buffer, len); 

    return (rc == len ? API_OK : API_ERROR); 

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
            iv_joblog ("Buffer overrun ");
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
                    iv_joblog ("Invalid response header: %d" , err);
                    return API_ERROR;
                }

                if (pIv->status == 301     // Temporary moved
                ||  pIv->status == 302) {  // Permanent moved
                    parseUrl (
                        pIv,
                        pIv->location, // New input !!
                        pIv->server ,
                        pIv->port ,
                        pIv->resource ,
                        pIv->host ,
                        pIv->user ,
                        pIv->password
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
                anyCharAppend ( &pIv->responseHeaderBuffer , pIv->buffer , pIv->headLen);

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
    anyCharAppend ( &pIv->responseDataBuffer ,pIv->contentData , pIv->contentLengthCalculated);

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

        anyCharAppend ( &pIv->responseDataBuffer, buffer, len);

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
