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
#include <fcntl.h>
#include <locale.h>


/* in qsysinc library */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ssl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>


#include "ostypes.h"
#include "teramem.h"
#include "varchar.h"
#include "sysdef.h"
#include "strUtil.h"
#include "streamer.h"
#include "simpleList.h"
#include "sndpgmmsg.h"
#include "parms.h"
#include "e2aa2e.h"

/* --------------------------------------------------------------------------- */
PILEVATOR iv_newClient()
{
    // Get mem and set to zero
    PILEVATOR pIv = memCalloc(sizeof(ILEVATOR));
    pIv -> pSockets = memCalloc(sizeof(SOCKETS));
    return pIv;

}
/* --------------------------------------------------------------------------- */
void iv_delete ( PILEVATOR pIv)
{
    sockets_free (pIv -> pSockets);
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
static BOOL sendHeader (PILEVATOR pIv) 
{

    UCHAR buffer [65535];
    PUCHAR pReq;
    LONG len, rc;

    pReq = buffer;
    pReq += sprintf(pReq, "%s %s HTTP/1.1%s", pIv->method , pIv->useProxy ? pIv->url : pIv->resource , EOL);

    if (*pIv->user > ' ') {
        pReq += addRealmLogin (pReq, pIv->user , pIv->password);
    }
    if (pIv->reqLength > 0) {
        pReq += sprintf(pReq, "Content-Length: %d%s" , pIv->reqLength , EOL);
    }
    pReq += sprintf(pReq, "User-Agent: ILEvator%s", EOL);
    pReq += sprintf(pReq, "Host: %s%s", pIv->host, EOL);
    //pReq += sprintf(pReq, "Connection: Close%s", EOL);
    pReq += sprintf(pReq, "Connection: keep-alive%s", EOL);

    // Todo !! Headers;
    len = pReq - buffer;
    e2a(buffer , buffer , len);

    if (IsNewLineAscii( * (pReq -1))) {
        pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
    } else {
        pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
        pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
    }

    rc = sockets_send (pIv->pSockets, buffer, len); 

    return (rc == len); 

}

/* --------------------------------------------------------------------------- */
static BOOL receiveHeader ( PILEVATOR pIv)
{
    UCHAR buffer [65535];
    PUCHAR pBuffer = buffer , contentData; 
    BOOL  headFound = FALSE;
    BOOL  headParsed = FALSE;
    LONG totlen, len, bufferLength , rcvTotalLen = 0;

    pIv->retry = FALSE; 


    for (;;) { // repeat until a header is received
        len = sockets_receive (pIv->pSockets, pBuffer, bufferLength - totlen, pIv->timeOut);
        
        // timeout
        if  (len == -2) {  
            return ( rcvTotalLen > 0 );
        }

        // Protocol error
        if  (len < 0) {  
            pIv->retry = TRUE; 
            return TRUE;
        }

        // "Connection close" - end of transmission and no "Content-Length" provided, the we are done
        if (len == 0 && pIv->headerHasContentLength == false) {
            return TRUE;
        }

        rcvTotalLen += Len;
        pBuf += Len;
        *pBuf = '\0';

        // Header not found yet - Search for it
        if (! headFound ) {
            headFound = LookForHeader(buffer, rcvTotalLen, &contentData);
            if (headFound) {

                err = ParseResponse(pIv , buffer, contentData);
                if (err) {
                    xsetmsg("CMN2000" ,  "Invalid response header: %d" , err);
                    return FALSE;
                }

                curContLen = rcvTotalLen - pIv->headerHeadLen;
                pIv->headerPCurEnd = pBuf;

                anyCharAppend ( &pIv->responseHeaderBuffer ,buffer , pIv->headerHeadLen);
                anyCharAppend ( &pIv->responseDataBuffer ,contentData , curContLen);


                if (pIv->headerStatus == 301     // Temporary moved
                ||  pIv->headerStatus == 302) {  // Permanent moved
                    parseUrl (
                        pIv->headerLocation,
                        pIv->server ,
                        pIv->port ,
                        pIv->resource ,
                        pIv->host ,
                        pIv->user ,
                        pIv->password
                    ); 
                    putWsTrace( "\r\n redirected to %s\r\n",  pIv->headerLocation );
                    pIv->retry = TRUE; 
                    return TRUE;
                }

                if (pIv->headerStatus == 100) {  // Continue is was a proxy
                    putWsTrace( "\r\n-- Proxy continue received --\r\n");
                    headFound = FALSE;
                    pBuf = Buffer;
                    rcvTotalLen = 0;
                    continue;   // Found but start over again - it was a proxy
                }


                if (pIv->headerChunked) {
                    break;  // !! Found
                }

                // For now continue but return er error
                //   CleanUp();
                //   if (pOptions  != pOptionsStr  ) jx_Close(&pOptions); // Was parser here  - need to free-up
                //   return ON;


                // Dont try to get data if it was a HEAD request - it is only the header
                // or status 204 => no content
                if (BeginsWith(pReqMethod, "HEAD")
                ||  pIv->headerStatus == 204 )     {  // No Content, dont read any longer
                    return TRUE;
                }

                // Received data for "non chunked" !!! TODO Move to reveice data --- this is not the place
                if (pIv->responseDataFile && pIv->headercontentData) {
                    fwrite (pIv->headercontentData , 1, curContLen , outfile);
                    while  ( Len > 0 && (curContLen < pIv->headerContentLength || pIv->headerHasContentLength == false)) {
                        len = SockReceive ( Buffer, sizeof(Buffer), pIv->timeOut);
                        fwrite (Buffer , 1, len , pIv->responseDataFile);
                        curContLen += len;
                    }
                    return TRUE;
                }
            }
        }
        curContLen = rcvTotalLen - pIv->headerHeadLen;
        if (pIv->headerHasContentLength == true
        &&  curContLen >= pIv->headerContentLength) {
            break;  // !! Found
        }
    }

/* --------------------------------------------------------------------------- */
static BOOL receiveData ( PILEVATOR pIv)
{
    UCHAR buffer [65535];
    LONG  len;

    for (;;) { // repeat until a header is received
        len = sockets_receive (pIv->pSockets, buffer , sizeof(buffer , pIv->timeOut);
        
        // timeout
        if  (len == -2) {  
            return FALSE;
        }

        // Protocol error
        if  (len < 0) {  
            return FALSE;
        }

        // "Connection close" - end of transmission and no "Content-Length" provided, the we are done
        if (len == 0 && pIv->headerHasContentLength == false) {
            return TRUE;
        }

        anyCharAppend ( &pIv->responseDataBuffer, buffer, len);

        if (pIv->responseDataFile) {
            fwrite (buffer  , 1, len , pIv->responseDataFil);
        }
    }
}

/*  !!!!!!!!!! MOve this else where ... 

   // When no "contents-length" is given
   if (pIv->headerHasContentLength == false) {
      if (respType == R_ILOB) {
         pRspIlobDta->DataLength = curContLen;
      } else  if (respType == R_VARCHAR) {
         rspPayload->Length = curContLen;
      }
   } else {
      // We have a content length: Check it !
      if (curContLen < pIv->headerContentLength) {
         xsetmsg("CMN2000",  "Invalid length %d receieved compared to header %d", curContLen, pIv->headerContentLength);
         return false;
      }
   }
) 
   if (pIv->headerChunked) {
      pIv->headerOut = outfile; // If any
      GetChunked(&HttpHeader , maxResponseSize);
      if (respType == R_ILOB) {
         pRspIlobDta->DataLength = pIv->headerContentLength;
      } else  if (respType == R_VARCHAR) {
         rspPayload->Length = pIv->headerContentLength;
      }
   }


    exit_with_data:
    CleanUp();
    if (outfile) fclose(outfile);
    if (pOptions  != pOptionsStr) jx_Close(&pOptions); // Was parser here  - need to free-up
    if (pIv->headerStatus >= 200 && pIv->headerStatus < 300) {  // Must be OK
        return (OFF); // OK;
    } else {
        xsetmsg("CMN2000" ,  "Invalid response status: %d" , pIv->headerStatus);
        return (ON);  // error;
    }

    exit_error:
    CleanUp();
    if (outfile) fclose(outfile);
    if (pOptions  != pOptionsStr) jx_Close(&pOptions); // Was parser here  - need to free-up
    return (ON);  // error;
    }
*/



/* --------------------------------------------------------------------------- */
LGL iv_do (
    PILEVATOR pIv,
    PUCHAR method,
    PUCHAR url,
    ULONG  timeOut
)
{

    PNPMPARMLISTADDRP pParms = _NPMPARMLISTADDR();
    BOOL ok = false; 
    
    pIv->method = method; 
    pIv->url = url; 
    pIv->timeOut = timeOut; // TODO for now

    parseUrl (
        url,
        pIv->server ,
        pIv->port ,
        pIv->resource ,
        pIv->host ,
        pIv->user ,
        pIv->password
    ); 

    for (retry = 0; retry < maxRetry ; retry++) {

        ok = sockets_connect(
            pIv->pSockets, 
            pIv->server ,
            pIv->port ,
            pIv->timeOut
        ); 
        if (!ok) break; 

        ok = sendHeader (pIv);
        if (!ok) break; 

        ok = receiveHeader (pIv);
        if (!ok) break; 
        if (piv->retry) continue; 

        if (pIv->headerChunked) {
            ok = receiveChunked(pIv);
        }
        else {
            ok = receiveData (pIv);
        }

        if (!ok) break; 
        if (piv->retry) continue; 

        break;

    }

    iv_delete ( pIv);

    return (ok?ON:OFF);

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
    sList_push(pIv->headerList, len, &header, OFF);
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
