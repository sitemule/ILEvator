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
#include "strUtil.h"
#include "streamer.h"
#include "simpleList.h"
#include "sndpgmmsg.h"
#include "parms.h"

#define CR 0x0D
#define LF 0x25
static UCHAR EOL [] = {CR, LF , 0};

//////// HELPERS - TODO - MOVE 
/* -------------------------------------------------------------------------- */
#define abeginsWith(a,b)  _abeginsWith(a,b,sizeof(b)-1)
BOOL _abeginsWith(PUCHAR a, PUCHAR b , int l)
{
   int i;
   for (i=0; i < l; i ++) {
     if ((a[i] & 0xdf) != (b[i] & 0xdf)) {
       return FALSE;
     }
   }
   return TRUE;
}
/* --------------------------------------------------------------------------- *\
   wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
static LONG UrlEncodeBlanks  (PUCHAR OutBuf , PUCHAR InBuf)
{
  PUCHAR Start = OutBuf;
  UCHAR c;
  LONG len = strTrimLen(InBuf);

  while (len--) {
     c = *InBuf;
     if (c == ' ') {
        OutBuf += sprintf(OutBuf , "%%20");
     } else {
        *(OutBuf++) = c;
     }
     InBuf++;
  }
  *OutBuf = '\0';
  return (OutBuf - Start);
}

/* --------------------------------------------------------------------------- *\
  wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
static void putWsTrace(PILEVATOR pIv, PUCHAR Ctlstr, ...)
{
   va_list arg_ptr;
   UCHAR   temp[4096];
   UCHAR   temp2[4096];
   LONG    len;
   SHORT   l,i;
   if (pIv->wstrace == NULL) return;

   va_start(arg_ptr, Ctlstr);
   len = vsprintf( temp , Ctlstr, arg_ptr);
   va_end(arg_ptr);
   xlateBuf (temp2 , temp , len+1, 0 , 1252); // Incl the null termination
   fputs (temp2 , pIv->wstrace);
}
/* --------------------------------------------------------------------------- *\
  wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
static void xsetmsg(PILEVATOR pIv , PUCHAR msgid , PUCHAR Ctlstr, ...)
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
static PUCHAR findEOL(PUCHAR p)
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
/* --------------------------------------------------------------------------- *\
   ASCII - atoi
\* --------------------------------------------------------------------------- */
#pragma convert(1252)
static LONG a2i (PUCHAR s)
{
   int i;
   int ret=0;
   for (i=0;i<15;i++) {
      if (s[i] == ' ') {
      } else if (s[i] >= '0' && s[i] <= '9') {
         ret = ret * 10 + (s[i] - '0');
      } else {
        break;
      }
   }
   return ret;
}
#pragma convert(0)
/* -------------------------------------------------------------------------- */
static int Charset2ccsid(PUCHAR Charset)
{
   int ccsid;
   if (Charset == NULL
   ||  beginsWith(Charset, "*NONE")
   ||  beginsWith(Charset, "0")) {
      ccsid     = 0;
   } else if (beginsWith(Charset, "*UTF8")
   ||         beginsWith(Charset, "UTF-8")) {
      ccsid     = 1208;
   } else if (beginsWith(Charset, "*ISO8859")
   ||         beginsWith(Charset, "ISO8859")
   ||         beginsWith(Charset, "ISO-8859")) {
      ccsid   = 819;
   } else if (beginsWith(Charset , "*WIN1252")
   ||         beginsWith(Charset , "WINDOWS-1252")) {
      ccsid   = 1252;
   } else {
      ccsid   = 1252;
   }
   return ccsid;
}

/* --------------------------------------------------------------------------- */
static int getCcsid(PUCHAR base , PUCHAR charsetStr, BOOL isASCII)
{
   PUCHAR charset  = stristr(base, charsetStr);

   if (charset) {
      charset += strlen(charsetStr);  // Length of "charset=" without zeroterm
      if (isASCII) {
         UCHAR tempCharSet[32];
         xlateStr (tempCharSet , charset  , 1252 , 0);
         return Charset2ccsid(tempCharSet);
      } else {
         return Charset2ccsid(charset);
      }
   }
   return 0;
}

/* --------------------------------------------------------------------------- */
static BOOL IsNewLineAscii(UCHAR c)
{
   if (c == 0x0d) return(TRUE);
   if (c == 0x0a) return(TRUE);
   return(FALSE);
}
/* -------------------------------------------------------------------------- */
static BOOL LookForHeader(PUCHAR Buf, LONG totlen, PUCHAR * ContentData)
{
   LONG i;
   PUCHAR p;

   for (i=0;i<totlen;i++) {
     p = Buf + i;

     if (p[0] == 0x0d && p[1] == 0x0a
     &&  p[2] == 0x0d && p[3] == 0x0a) {
       *ContentData= p + 4;
       *p = 0;
       return TRUE; //found
     } else if ((p[0] == 0x0d && p[1] == 0x0d)
     ||         (p[0] == 0x0a && p[1] == 0x0a)) {
       *ContentData= p + 2;
       *p = 0;
       return TRUE; //found
     }
   }
   return FALSE; //not found
}
/* -------------------------------------------------------------------------- */
#pragma convert(1252)
static VOID ParseHttpParm(PILEVATOR pIv, PUCHAR Parm , PUCHAR Value)
{
   if (abeginsWith (Parm , "Content-Length"))  {
      pIv->contentLength = a2i(Value);
      pIv->responseHeaderHasContentLength = true;
   }
   if (abeginsWith (Parm , "Transfer-Encoding"))  {
      pIv->responseIsChunked = strstr (Value, "chunked") > NULL; //!! TODO striastr 
   }
   if (abeginsWith (Parm , "location"))  {
      xlateStr(pIv->location , Value , 1252 , 0);
   }
// Unpack: "Content-Type: text/html; charset=windows-1252"

   if (abeginsWith (Parm , "Content-Type"))  {
      pIv->Ccsid = getCcsid(Value, "charset=", /*IsASCII=*/ TRUE);
   }
}
#pragma convert(0)

/* -------------------------------------------------------------------------- */
// All constans is in ascii windows-1252
#pragma convert(1252)
static SHORT ParseResponse(PILEVATOR pIv, PUCHAR buf, PUCHAR contentData)
{
   PUCHAR Http, start, end, p, s1, s2;

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
         ParseHttpParm(pIv , parmName, parmValue);
      }
   }
   return 0 ; // OK
}
#pragma convert(0)


/* --------------------------------------------------------------------------- *\
   Authentications is base64 encoded userid in ascii
\* --------------------------------------------------------------------------- */
static LONG addRealmLogin (PUCHAR pReq, PUCHAR user , PUCHAR password)
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
   Parse: http://www.google.com:1234/any.asp&parm1=value1
   Or   : <www.extraprivacy.com:8080>http://www.google.com:1234/any.asp&parm1=value1
   With User id and password:
   http://userid:password@www.google.com:1234/any.asp&parm1=value1

\* -------------------------------------------------------------------------- */
static void parseUrl (
   PILEVATOR pIv,
   PUCHAR url, 
   PUCHAR Server , 
   PUCHAR Port , 
   PUCHAR Resource, 
   PUCHAR Host, 
   PUCHAR User, 
   PUCHAR Password)
{
   PUCHAR pProtocol;
   PUCHAR pServer;
   PUCHAR pPort;
   PUCHAR pResource;
   PUCHAR pProxy = NULL;
   PUCHAR pProxyEnd = NULL;
   PUCHAR pTemp;
   PUCHAR pUrlParm;
   UCHAR  masterspace;

   pIv->AsHttps = FALSE;
   pIv->useProxy = FALSE;
   strcpy (User, "");
   strcpy (Password, "");

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
      pIv->AsHttps = beginsWith(url , "https");
      pServer = pProtocol + 3;
   } else {
      pServer = url;
   }
  
   // TODO !! move to funciton 
   #pragma convert(1252)
   xlateBuf(&masterspace , "@" , 1  , 1252, 0);
   #pragma convert(0)

   pTemp = strchr(pServer , masterspace);
   if (pTemp) {
      UCHAR userPassword [256];
      PUCHAR pPassword;
      substr(userPassword , pServer , pTemp - pServer);
      pPassword  = strchr(userPassword  , ':');
      if (pPassword) {
        substr(User, userPassword ,  pPassword - userPassword );
        strcpy (Password, pPassword+1);
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
     UrlEncodeBlanks (Resource , pResource);
   } else {
     strcpy(Resource , "/");
   }

   // Pick up the port
   if (pPort) {
      if (pResource) {
         substr (Port, pPort + 1, pResource - pPort -1);
      } else {
         strcpy (Port, pPort + 1);
      }
   } else {
      if (pIv->AsHttps) {
         strcpy (Port, "443");
      } else {
         strcpy (Port, "80");
      }
   }

   // Pick up the server
   if (pPort) {
      substr ( Server , pServer , pPort - pServer);
   } else if (pResource) {
      substr ( Server , pServer , pResource - pServer);
   } else if (pUrlParm) {
      substr ( Server , pServer , pUrlParm - pServer);
   } else {
      strrighttrimcpy(Server , pServer);
   }

   // Pick up host (server + port)
   if (pPort) {
      sprintf(Host , "%s:%s" , Server , Port);
   } else {
      strcpy (Host , Server);
   }

   // pick up the non standart proxy was given between a < and a >
   if (pProxy) {
     UCHAR  proxy [256];
     pIv->useProxy = TRUE;
     pIv->AsHttps  = FALSE;
     substr(proxy , pProxy , pProxyEnd - pProxy);
     pPort  = strstr(proxy , ":");
     if (pPort) {
       strcpy ( Port , pPort + 1);
       substr ( Server , proxy , pPort - proxy);
     } else {
       strcpy ( Port   , "8080");
       strcpy ( Server , proxy);
     }
     UrlEncodeBlanks (Resource , pProxyEnd  + 1);
   }
}
// ------------------------------ real exports ---------------------------------


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
static API_STATUS sendHeader (PILEVATOR pIv) 
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


    if (IsNewLineAscii( * (pReq -1))) {
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
static API_STATUS receiveHeader ( PILEVATOR pIv)
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
            headFound = LookForHeader(pIv->buffer, pIv->bufferTotalLength, &pIv->contentData);
            if (headFound) {
                int err = ParseResponse(pIv , pIv->buffer, pIv->contentData);
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
static API_STATUS receiveData ( PILEVATOR pIv)
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

    memcpy(p, headerName->String, headerName->Length);
    p += headerName->Length;

    p += sprintf(p , "%s", ": ");

    memcpy(p, headerValue->String, headerValue->Length);
    p += headerValue->Length;

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
/* --------------------------------------------------------------------------- *\
    Setup the local top be POSIX i.e. the charset in ccsid(37) 
\* --------------------------------------------------------------------------- */
static void initialize(void)
{
    static BOOL initialized;

    if (initialized) return;
    initialized = true;

    setlocale(LC_CTYPE , "POSIX"); 
}
