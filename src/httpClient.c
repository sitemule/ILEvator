/* SYSIFCOPT(*IFSIO) TERASPACE(*YES *TSIFC)   STGMDL(*SNGLVL) */
/* ------------------------------------------------------------- */
/* Program . . . : httpClient                                    */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : HTTP URL client                               */
/*                                                               */
/* By     Date       Task    Description                         */
/* ------ ---------- ------- ----------------------------------- */
/* NL     15.05.2005 0000000 New Program                         */
/* NL     25.02.2007     510 Ignore namesPace for WS Parameters  */
/* NL     10.06.2020     000 Fix SSL Cleaup sequence             */
/* NL     08.02.2022     000 No blocking and connect timeout     */
/* NL     25.08.2022     000 Quick fix for err 502=WOULD_BLOCK   */
/* ------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <xxdtaa.h>
#include <gskssl.h>
#include <sys/time.h>
#include <sys/tyPes.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll>
#include <netinet/in.h>
#include <arPa/inet.h>
#include <netdb.h>
#include <unistd.h>
// #include <ssl.h>
#include <errno.h>
#include <qsyrgaP1.h>


/* own standart includes */
#include "ostyPes.h"
#include "varchar.h"
#include "utl100.h"
#include "MinMax.h"
#include "svr00.h"
#include "Parms.h"
#include "streamer.h"
#include "jsonxml.h"
#include "servercall.h"
#define  T4TRUE 't'
#define  F4FALSE 'f'

typedef struct _HTTP_HEADER {
   PUCHAR ResponseString;
   LONG   HeadLen;
   LONG   ContentLength;
   BOOL   hasContentLength;
   PUCHAR ContentData;
   SHORT  Status;
   BOOL   Chunked;
   PUCHAR pCurEnd;
   LONG   Ccsid;
   UCHAR  Location[256];
   FILE * out;
} HTTP_HEADER , * PHTTP_HEADER;

typedef  enum {
     R_NONE,
     R_ILOB,
     R_VARCHAR,
     R_JSON,
     R_FILE
} RTYPE;

static HTTP_HEADER HttpHeader;
static FILE * wstrace;
static PSVR00R pSvr;

#define CR 0x0D
#define LF 0x25

static SOCKET sd = -1;

static BOOL AsHttps;
static BOOL useProxy;
static UCHAR EOL [] = {CR, LF , 0};
static gsk_handle my_env_handle=NULL;    /* secure environment handle */
static gsk_handle my_session_handle=NULL;    /* secure session handle */
static validationCallBack valCallBack;
static int rcvTotalLen =0;

// Proto tyPese
BOOL  SocketConnect(PUCHAR ServerIP, LONG ServerPort, SHORT TimeOut, PJXNODE pNode);
BOOL  SockSend (PUCHAR Buf, LONG Len);
LONG  SockReceive ( PUCHAR Buf, LONG Len, SHORT TimeOut);
SHORT ParseResponse(PHTTP_HEADER pHttpHeader, PUCHAR Buf, PUCHAR Content);
VOID  ParseHttpParm(PHTTP_HEADER pHttpHeader, PUCHAR Parm , PUCHAR Value);
void  ParseUrl (PUCHAR url, PUCHAR Server , PUCHAR Port , PUCHAR Resource, PUCHAR Host, PUCHAR User, PUCHAR Password);

/* --------------------------------------------------------------------------- *\
   wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
LONG UrlEncodeBlanks  (PUCHAR OutBuf , PUCHAR InBuf)
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
static void putWsTrace(PUCHAR Ctlstr, ...)
{
   va_list arg_ptr;
   UCHAR   temp[4096];
   UCHAR   temp2[4096];
   LONG    len;
   SHORT   l,i;
   if (wstrace == NULL) return;

   va_start(arg_ptr, Ctlstr);
   len = vsprintf( temp , Ctlstr, arg_ptr);
   va_end(arg_ptr);
   e2a(temp2 , temp , len+1); // Incl the null termination
   fputs (temp2 , wstrace);
}
/* --------------------------------------------------------------------------- *\
  wrapper for the message and trace to  the message log
\* --------------------------------------------------------------------------- */
static void xsetmsg(PUCHAR msgid , PUCHAR Ctlstr, ...)
{
   va_list arg_ptr;
   UCHAR   temp[1024];
   LONG    len;
   SHORT   l,i;
   va_start(arg_ptr, Ctlstr);
   len = vsprintf( temp , Ctlstr, arg_ptr);
   va_end(arg_ptr);

   putWsTrace("%s" , temp);
   setmsg(msgid , "%s" , temp);
}
/* --------------------------------------------------------------------------- */
static BOOL IsNewLineAscii(UCHAR c)
{
   if (c == 0x0d) return(TRUE);
   if (c == 0x0a) return(TRUE);
   return(FALSE);
}
static BOOL IsNewLineEbcdic(UCHAR c)
{
   if (c == CR)   return(TRUE);
   if (c == LF)   return(TRUE);
   return(FALSE);
}
/* --------------------------------------------------------------------------- *\
   ASCII - atoi
\* --------------------------------------------------------------------------- */
#pragma convert(1252)
int a2i (PUCHAR s)
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
/* --------------------------------------------------------------------------- *\
   ASCII - atoi
\* --------------------------------------------------------------------------- */
#define aBeginsWith(a,b)  _aBeginsWith(a,b,sizeof(b)-1)
BOOL _aBeginsWith(PUCHAR a, PUCHAR b , int l)
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
   Open the trace file / default file if requestet on server
\* --------------------------------------------------------------------------- */
static void openWsTrace(PUCHAR tracefile)
{
   if (tracefile) {
      wstrace = fopen(tracefile ,"ab,codepage=1252");
   } else if (pSvr->SVTRAC) {
      PUCHAR FileName = vc2str(&pSvr->SVTPTH);
      if (* FileName == '\0') {
        FileName = "trace.txt";
      }
      wstrace = fopen(FileName ,"ab,codepage=1252");
   } else {
      wstrace = NULL;
   }
}
/* --------------------------------------------------------------------------- *\
   Clean up
\* --------------------------------------------------------------------------- */
static void CleanUp(void)
{
   LONG ok;

   if (wstrace) {
      putWsTrace( "\r\n-------------- End of HTTP request --------------\r\n");
      fclose(wstrace);
      wstrace = NULL;
   }

   // disable SSL support for the socket
   if (my_session_handle != NULL) {
      ok = gsk_secure_soc_close(&my_session_handle);
      my_session_handle = NULL;
   }

   // close the connection
   if (sd  != -1) {
      close(sd);
      sd = -1;
   }

   // disable the SSL environment
   //if (my_env_handle != NULL) {
   //   ok = gsk_environment_close(&my_env_handle);
   //   my_env_handle = NULL;
   //}
}
/* --------------------------------------------------------------------------- *\
   Authentications is base64 encoded userid in ascii
\* --------------------------------------------------------------------------- */
LONG addRealmLogin (PUCHAR pReq, PUCHAR User , PUCHAR Password)
{
   UCHAR UserAndPasseord [256];
   ULONG pw1Len, pw2Len, pw3Len, len;
   UCHAR pw1 [256];
   UCHAR pw2 [256];
   UCHAR pw3 [256];

   if (*User == '\0' ||  *Password == '\0') return 0;

   sprintf(UserAndPasseord , "%s:%s" , User , Password);
   pw1Len  = XlateBuf(pw1 , UserAndPasseord , strlen(UserAndPasseord) , 0 , 1252);
   Base64EncodeBuf(pw2  , &pw2Len , pw1, &pw1Len);
   pw2[pw2Len] = '\0';
   pw3Len = XlateBuf(pw3 , pw2 , strlen(pw2) , 1252, 0);
   pw3[pw3Len] = '\0';

   len = sprintf(pReq, "Authorization: Basic %s%s" , pw3, EOL);

   return len;
}
/* --------------------------------------------------------------------------- *\
   Returns the IP of the local host
\* --------------------------------------------------------------------------- */
static void LocalHost(PUCHAR host)
{
   UCHAR TempHost [32];
   QXXRTVDA (*(_DTAA_NAME_T*)  "SVCHOSTIP *LIBL     ", 1 , sizeof(TempHost), TempHost);
   strtrimncpy(host, TempHost , sizeof(TempHost));
}
/* --------------------------------------------------------------------------- */
static void SetSSLmsg(int rc, PUCHAR txt)
{
   xsetmsg("CMN2000", "%s: %d: %s, %s", txt, rc, gsk_strerror(rc), strerror(errno));
}
/* --------------------------------------------------------------------------- */

static int sslCallBack(PUCHAR certChain, int valStatus)
{
  putWsTrace( "\nCallBack: %s\n", gsk_strerror(valStatus));
  return GSK_OK;
}
// ----------------------------------------------------------------------------------------
static PUCHAR addKeyVal(PUCHAR pVarRecCount, PUCHAR pVarRec, LONG key  , LONG len , PUCHAR value)
{
  LONG totLen;
  PUCHAR pNext;

  (* (PLONG) pVarRecCount) ++;

  // Ajust for 4-byte allignment
  totLen = (sizeof(LONG) * 3) + len;
  totLen += totLen % sizeof(LONG);
  pNext = pVarRec + totLen;

  * ((PLONG) pVarRec) = totLen;
  pVarRec += sizeof(LONG);

  * ((PLONG) pVarRec) = key;
  pVarRec += sizeof(LONG);

  * ((PLONG) pVarRec) = len;
  pVarRec += sizeof(LONG);

  memcpy(pVarRec , value , len);

  return ( pNext);
}

// ----------------------------------------------------------------------------------------
static BOOL set_attr (gsk_handle hndl, int attr , int value, PUCHAR msg)
{
   int rc;
   errno=0;

   rc  = gsk_attribute_set_numeric_value(hndl, attr, value);
   if (rc != GSK_OK) {
      SetSSLmsg(rc, msg);
      CleanUp();
      return true; // true fails
   } else {
      return false;
   }
}
/* --------------------------------------------------------------------------- *\
\* --------------------------------------------------------------------------- */
static BOOL initssl  (PUCHAR ServerIP, LONG ServerPort, SHORT TimeOut, PJXNODE pOptions)
{
   PUCHAR keyringFileName;
   PUCHAR keyringPassword;
   LONG   rc;
   PUCHAR appId = "ICEBREAK_SECURE_CLIENT";
   int    appIdLen = strlen(appId);
   PJXNODE pCert;

   if ( my_env_handle) {
       usleep(100);  // Detach the process due to bug in SSL
       return true;
   }

   // open a gsk environment !! Global due to memry leak
   errno = 0;
   rc = gsk_environment_open(&my_env_handle);
   if (rc != GSK_OK) {
     SetSSLmsg(rc, "gsk_environment_open()");
     CleanUp();
     return FALSE;
   }

   // set the Application ID to use
   /*
   errno = 0;
   rc = gsk_attribute_set_buffer(my_env_handle,
                                 GSK_OS400_APPLICATION_ID,
                                 appId,
                                 appIdLen);
   if (rc != GSK_OK) {
     SetSSLmsg(rc, "set the Application ID");
     CleanUp();
     return FALSE;
   }
   */

   // set the validation callback
   valCallBack.validation_callback = sslCallBack;
   valCallBack.validateRequired    = GSK_NO_VALIDATION;
   valCallBack.certificateNeeded   = GSK_END_ENTITY_CERTIFICATE;

   errno = 0;
   rc = gsk_attribute_set_callback(my_env_handle,
                                   GSK_CERT_VALIDATION_CALLBACK,
                                   &valCallBack);
   if (rc != GSK_OK) {
     SetSSLmsg(rc, "set the validation callback");
     CleanUp();
     return FALSE;
   }


   // if we have a certificate tage wil will use that
   pCert = jx_GetNode ( pOptions, "/certificate");
   if (pCert) {

      // set the Keyring file path
      keyringFileName = jx_GetValuePtr  ( pCert , "file"     , NULL);   // Null string
      keyringPassword = jx_GetValuePtr  ( pCert , "password" , NULL); // Null pointer

   } else {

      // set the Keyring file path
      pSvr->SVCEPF.data[pSvr->SVCEPF.len] =0;
      if (BeginsWith(pSvr->SVCEPF.data , "*DFT") || pSvr->SVCEPF.data[0]<= ' ') {
        keyringFileName    = "/QIBM/USERDATA/ICSS/CERT/CERTAUTH/DEFAULT.KDB";
      } else {
        keyringFileName    = pSvr->SVCEPF.data;
      }

      // set Password to the keyring
      pSvr->SVCEPW.data[pSvr->SVCEPW.len] =0;
      if (pSvr->SVCEPW.data[0] >  ' ') {
         keyringPassword    = pSvr->SVCEPW.data;
      } else {
         keyringPassword    = NULL;
      }
   }

   if (keyringFileName && *keyringFileName > ' ') {
      errno = 0;
      rc = gsk_attribute_set_buffer(my_env_handle,
                                    GSK_KEYRING_FILE,
                                    keyringFileName,
                                    strlen(keyringFileName));
      if (rc != GSK_OK) {
        SetSSLmsg(rc, "set the Keyring file");
        CleanUp();
        return FALSE;
      }
   }

   // set Password to the keyring
   if (keyringPassword && *keyringPassword > ' ' ) {
      errno = 0;
      rc = gsk_attribute_set_buffer(my_env_handle,
                                    GSK_KEYRING_PW,
                                    keyringPassword,
                                    strlen(keyringPassword));
      if (rc != GSK_OK) {
        SetSSLmsg(rc, "set Password to the keyring");
        CleanUp();
        return FALSE;
      }
   }

   // SNI will only work if a DNS name is given, IP are not allowed.
   // Default to sni:false
   //if (*jx_GetValuePtr  ( pOptions  , "request/sni" , "false") == T4TRUE) {  // true
   if (strspn (ServerIP , "0123456789.") != strlen( ServerIP)) {
      rc = gsk_attribute_set_buffer(
         my_env_handle,
         GSK_SSL_EXTN_SERVERNAME_REQUEST,
         //GSK_SSL_EXTN_SERVERNAME_CRITICAL_REQUEST, << use this if the name has to match 100%
         ServerIP,
         strlen(ServerIP)
      );
      if (rc != GSK_OK) {
        SetSSLmsg(rc, "set SNI error");
        CleanUp();
        return FALSE;
      }
   }

   // If one fails - then return  !!
   if (set_attr (my_env_handle, GSK_HANDSHAKE_TIMEOUT , 30             , "Set GSK_HANDSHAKE_TIMEOUT  error")
   ||  set_attr (my_env_handle, GSK_OS400_READ_TIMEOUT, TimeOut * 1000L, "Set GSK_OS400_READ_TIMEOUT error")
   ||  set_attr (my_env_handle, GSK_V2_SESSION_TIMEOUT, 60             , "Set GSK_V2_SESSION_TIMEOUT error")
   ||  set_attr (my_env_handle, GSK_V3_SESSION_TIMEOUT, 60             , "Set GSK_V3_SESSION_TIMEOUT error")) {
      return FALSE;
   }

   // set this side as the client (this is the default)
   errno = 0;
   rc = gsk_attribute_set_enum(my_env_handle,
                               GSK_SESSION_TYPE,
                               GSK_CLIENT_SESSION);
   if (rc != GSK_OK) {
     SetSSLmsg(rc, "set this side as the client");
     CleanUp();
     return FALSE;
   }


   // set auth-passthru
   errno = 0;
   rc = gsk_attribute_set_enum(my_env_handle,
                               GSK_CLIENT_AUTH_TYPE,
                               GSK_CLIENT_AUTH_PASSTHRU);

   if (rc != GSK_OK) {
     SetSSLmsg(rc, "set auth-passthru");
     CleanUp();
     return FALSE;
   }

   // Initialize the secure environment
   rc = gsk_environment_init(my_env_handle);

   // Not registeret yet - do it
   if (rc == GSK_AS400_ERROR_NOT_REGISTERED) {
      UCHAR  varRec [512];
      PLONG  pVarRecCount = (PLONG) varRec;
      PUCHAR pVarRec = varRec + sizeof(LONG);
      APIRTN apiRtn;

      apiRtn.ApiSize = sizeof(APIRTN);

      memset(varRec , 0 , sizeof(varRec));
      pVarRec = addKeyVal(varRec, pVarRec, 2 , 50 , "IceBreak Secure Client                            ");
      pVarRec = addKeyVal(varRec, pVarRec, 8 , 1  , "2"); // Client type
      pVarRec = addKeyVal(varRec, pVarRec, 10, 1  , "0"); // Client authentication supported. 1=application support
      pVarRec = addKeyVal(varRec, pVarRec, 4 , 1  , "0"); // Limit CA certificates trusted

      QsyRegisterAppForCertUse (
            appId,
            &appIdLen,
            (Qsy_App_Controls_T *) varRec,
            &apiRtn);

      if (apiRtn.ApiAvail !=0) {
        SetSSLmsg(rc, "Register App For Cert Use");
        CleanUp();
        return FALSE;
      }

      // re-initialize the secure environment */
      errno = 0;
      rc = gsk_environment_init(my_env_handle);
      if (rc != GSK_OK) {
        SetSSLmsg(rc, "gsk_environment_init");
        CleanUp();
        return FALSE;
      }
   }
   return true;
}
/* --------------------------------------------------------------------------- *\
\* --------------------------------------------------------------------------- */
static BOOL SocketConnect(PUCHAR ServerIP, LONG ServerPort, SHORT TimeOut, PJXNODE pOptions)
{
   LONG   rc;
   int    on = 1;
   struct sockaddr_in serveraddr;
   struct hostent * hostp;
   struct sockaddr peeraddr;
   struct fd_set  fdset;
   struct timeval timeout;
   struct pollfd pfd;

   if (AsHttps) {
      initssl (ServerIP, ServerPort, TimeOut, pOptions);
   }

   // Get a socket descriptor
   sd = socket(AF_INET, SOCK_STREAM, 0);

   if (sd == INVALID_SOCKET)  {
      xsetmsg("CMN4000" ,  "%s" , strerror(errno));
      return FALSE;
   }

   // Connect to an address
   memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
   serveraddr.sin_family        = AF_INET;
   serveraddr.sin_port          = htons(ServerPort);

   // If a valid ip adress is given (only digitd and dots)
   if (strspn (ServerIP , "0123456789.") == strlen( ServerIP)) {
      serveraddr.sin_addr.s_addr   = inet_addr( ServerIP);
   } else {

     // get host address
     hostp = gethostbyname(ServerIP);
     if (hostp == (struct hostent *)NULL) {
        CleanUp();
        xsetmsg("CMN4001" ,  "%s" , strerror(errno));
        return FALSE;
     }
     memcpy(&serveraddr.sin_addr,  hostp->h_addr, sizeof(serveraddr.sin_addr));
   }

   // Non blocking socket
   fcntl(sd, F_SETFL, O_NONBLOCK);

   // Do non-blocked connect
   rc = connect(sd , (struct sockaddr *)&serveraddr , sizeof(serveraddr));
   // rc will be -1 for NON_bloking sockets
   if( (rc != 0) && (errno != EINPROGRESS) ) {
      xsetmsg("CMN4002" ,  "%-64.64s%s" , ServerIP, strerror(errno));
      CleanUp();
      return FALSE;
   }

   // Wait until connected or timout
   pfd.fd = sd;
   pfd.events = POLLOUT;
   rc = poll( &pfd, 1, TimeOut * 1000 );

   // Wait for up to xx seconds on
   // select() for data to be read.
   if (rc == -1 ) {
      int so_error;
      socklen_t len = sizeof(so_error);
      getsockopt(sd, SOL_SOCKET, SO_ERROR, (PUCHAR) &so_error, &len);
      xsetmsg("CMN4002" ,  "%-64.64s%s" , ServerIP, strerror(so_error));
      CleanUp();
      return FALSE;
   } else if (rc == 0) { // 0=Timeout
      int so_error;
      socklen_t len = sizeof(so_error);
      getsockopt(sd, SOL_SOCKET, SO_ERROR, (PUCHAR) &so_error, &len);
      xsetmsg("CMN4008" ,  "%-64.64s%s" , ServerIP, strerror(so_error));
      CleanUp();
      return FALSE;
   }

   if (AsHttps) {
     // open a secure session
     errno = 0;
     rc = gsk_secure_soc_open(my_env_handle, &my_session_handle);
     if (rc != GSK_OK) {
       SetSSLmsg(rc, "gsk_secure_soc_open");
       CleanUp();
       return FALSE;
     }

     if (set_attr (my_session_handle, GSK_FD , sd  , "Set GSK_FD associate socket with the secure session")) {
        return FALSE;
     }

     // initiate the SSL handshake
     errno = 0;
     rc = gsk_secure_soc_init(my_session_handle);
     if (rc != GSK_OK) {
       SetSSLmsg(rc, "initiate the SSL handshake");
       CleanUp();
       return FALSE;
     }
   }

   /*
   rc = getpeername (sd , &peeraddr , &peeraddrlen) ;
   if (rc < 0) {
      sndpgmmsg ("CPF9898" ,INFO , "get peer name failed: %s" , strerror(errno));
   }
   */
   return TRUE;
}
/* --------------------------------------------------------------------------- *\
   SockSend puts data to the socket port and test for errors
\* --------------------------------------------------------------------------- */
static BOOL SockSend (PUCHAR Buf, LONG Len)
{
   LONG rc;
   int error;
   int errlen = sizeof(error);
   int amtWritten = 0;

   // Nothing to send?
   if (Len == 0) return(TRUE);

   if (wstrace) {
     fwrite(Buf , 1 , Len , wstrace);
   }

// rc = send (sd, Buf , Len ,0);
// rc = SSL_Write(pSsl, Buf , Len);

   if (AsHttps) {
     amtWritten = 0;
     rc = gsk_secure_soc_write(my_session_handle, Buf, Len, &amtWritten);
     if (rc != GSK_OK || amtWritten != Len) {
       SetSSLmsg(rc, "gsk_secure_soc_write");
       CleanUp();
       return FALSE;
     }
   } else {
     errno = 0;
     rc = send (sd, Buf , Len ,0);
     if (rc != Len) {

       // Get the error number.
       rc = getsockopt(sd, SOL_SOCKET, SO_ERROR, (PUCHAR) &error, &errlen);
       if (rc == 0) {
          errno = error;
       }
       xsetmsg("CMN4006" ,  "Not able to send: %s" , strerror(errno));
       CleanUp();
       return (FALSE);
     }
   }

   return(TRUE);
}
/* --------------------------------------------------------------------------- *\
\* --------------------------------------------------------------------------- */
static LONG SockReceive ( PUCHAR Buf, LONG Len, SHORT TimeOut)
{
   int rc;
   int error;
   int errlen = sizeof(error);
   struct fd_set read_fd;
   struct timeval timeout;
   int amtRead = 0;

   // Set select timeout
   timeout.tv_sec  = TimeOut;
   timeout.tv_usec = 0;

   // Wait for up to xx seconds on
   // select() for data to be read.
   FD_ZERO(&read_fd);
   FD_SET(sd,&read_fd);

   rc = select(sd +1, &read_fd ,NULL,NULL,&timeout);
   if (rc < 0) {

   /* Get the error number. */
      rc = getsockopt(sd, SOL_SOCKET, SO_ERROR, (PUCHAR) &error, &errlen);
      if (rc == 0) {
         errno = error;
      }
      xsetmsg("CMN4005" ,  ": %s" , strerror(errno));
      CleanUp();
      return(-1);
   } else if (rc == 0) {
      xsetmsg("CMN4005" , "Empty data");
      CleanUp();
      return(-2);
   }
   // read() from client
   // rc = SSL_Read(pSsl, Buf , Len);

   // receive a message from the client using the secure session
   if (AsHttps) {
      int retry = 10;

      rc = gsk_secure_soc_read(my_session_handle, Buf, Len, &amtRead);

      // Quick fix for error 502
      while (rc == GSK_WOULD_BLOCK && retry) {
         usleep(1000);
         rc = gsk_secure_soc_read(my_session_handle, Buf, Len, &amtRead);
         retry --;
      }

      // Quick Fix for Apache Cyote
      if (rc != GSK_OK && ! HttpHeader.Chunked &&  HttpHeader.ContentLength ==  0 && rcvTotalLen > 0) {
         return 0;
      }
      else if (rc == GSK_OS400_ERROR_TIMED_OUT) {  // Timeout
         xsetmsg("CMN4007" ,  "Timeout");
         CleanUp();
         return -2;
      }
      else if (rc != GSK_OK ) {
         SetSSLmsg(rc, "gsk_secure_soc_read");
         CleanUp();
         return -1;
      }

   } else {
      rc = read(sd, Buf, Len );
      if (rc < 0) {  // error
         xsetmsg("CMN4005" ,  ": %s" , strerror(errno));
         CleanUp();
         return -1;

      } else if (rc == 0) {  // Timeout
         xsetmsg("CMN4007" ,  "Timeout");
         CleanUp();
         return -2;
      }
      amtRead = rc;
   }

   if (wstrace) {
      fwrite(Buf , 1 , amtRead , wstrace);
   }
   return (amtRead); /* The returned lenght */
}
#ifdef xxxxxxxxxxx
/* -------------------------------------------------------------------------- *\
   Encodes the string into URL format
\* -------------------------------------------------------------------------- */
static LONG Encode (PUCHAR Output, PUCHAR Input)
{
   UCHAR  c;
   PUCHAR pI, pO;
   LONG   OutputLen = 0;
   LONG   InputLen;
   PUCHAR Hex = "0123456789ABCDEF";
   PUCHAR ValidChars = "abcdefghijklmnopqrstuwvxyz"
                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                       "0123456789";

   pI       = Input;
   InputLen = strlen(Input);
   pO       = Output;

   // Find the trimed lenght
   while (InputLen  > 0 && Input[InputLen-1] <= ' ') {
      InputLen --;
   }
   while (InputLen--) {
      if (strchr(ValidChars , *pI)) {  // Look up for characters that dont need encoding
         *pO++ = *pI;
         OutputLen ++;
      } else {
         *pO++ = '%';
         e2a(&c, pI, 1);
         *pO++ = Hex[(c / 16)];
         *pO++ = Hex[(c % 16)];
         OutputLen += 3;
      }
      pI++;
   }
   *pO = '\0'; /*zero terminate the string */
   return(OutputLen);
}
#endif
/* -------------------------------------------------------------------------- */
static int getCcsid(PUCHAR base , PUCHAR charsetStr, BOOL isASCII)
{
   PUCHAR charset  = stristr(base, charsetStr);

   if (charset) {
      charset += strlen(charsetStr);  // Length of "charset=" without zeroterm
      if (isASCII) {
         UCHAR tempCharSet[32];
         a2e(tempCharSet , charset  , 32);
         return Charset2ccsid(tempCharSet);
      } else {
         return Charset2ccsid(charset);
      }
   }
   return 0;
}
/* -------------------------------------------------------------------------- */
#pragma convert(1252)
static VOID ParseHttpParm(PHTTP_HEADER pHttpHeader, PUCHAR Parm , PUCHAR Value)
{
   if (aBeginsWith (Parm , "Content-Length"))  {
      pHttpHeader->ContentLength = a2i(Value);
      pHttpHeader->hasContentLength = true;
   }
   if (aBeginsWith (Parm , "Transfer-Encoding"))  {
      pHttpHeader->Chunked = stristr (Value, "chunked") > NULL;
   }
   if (aBeginsWith (Parm , "location"))  {
      a2e(pHttpHeader->Location , Value, strlen(Value) + 1);
   }
// Unpack: "Content-Type: text/html; charset=windows-1252"

   if (aBeginsWith (Parm , "Content-Type"))  {
      pHttpHeader->Ccsid = getCcsid(Value, "charset=", /*IsASCII=*/ TRUE);
   }
}
#pragma convert(0)
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
/* -------------------------------------------------------------------------- */
// All constans is in ascii windows-1252
#pragma convert(1252)
static SHORT ParseResponse(PHTTP_HEADER pHttpHeader, PUCHAR Buf, PUCHAR ContentData)
{
   PUCHAR Http, start, end, p, s1, s2;

   // Only let the out file survive
   memset(pHttpHeader, 0, sizeof(*pHttpHeader));

   pHttpHeader->ContentData = ContentData;
   pHttpHeader->HeadLen = pHttpHeader->ContentData - Buf;

   // Retrive the HTTP responce
   start = strstr(Buf , "HTTP");
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
          pHttpHeader->Status = a2i(temp);
       }
   }

   pHttpHeader->ResponseString = start;
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
         UCHAR  ParmName[256];
         UCHAR  ParmValue[65535];
         substr(ParmName , start , p - start);
         substr(ParmValue, p +1  , end - p - 1 );
         ParseHttpParm(pHttpHeader , ParmName, ParmValue);
      }
   }
   return 0 ; // OK
}
#pragma convert(0)
/* -------------------------------------------------------------------------- *\
   Parse: http://www.google.com:1234/any.asp&parm1=value1
   Or   : <www.extraprivacy.com:8080>http://www.google.com:1234/any.asp&parm1=value1
   With User id and password:
   http://userid:password@www.google.com:1234/any.asp&parm1=value1

\* -------------------------------------------------------------------------- */
static void ParseUrl (
   PUCHAR url, PUCHAR Server , PUCHAR Port , PUCHAR Resource, PUCHAR Host, PUCHAR User, PUCHAR Password)
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

   AsHttps = FALSE;
   useProxy = FALSE;
   strcpy (User, "");
   strcpy (Password, "");

   // Skip blanks
   for (; *url == ' '; url++);

   // No remote server was given ... just use loopback
   if (url[0] == '/') {
      int PortNum = pSvr->SVPORT;
      sprintf(Port, "%d" , PortNum);
      strcpy(Server, "127.0.0.1");
      UrlEncodeBlanks (Resource , url );
      sprintf(Host , "%s:%d" , Server , PortNum);
      AsHttps = pSvr->SVPROT == 1;
      return;
   }

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
      AsHttps = BeginsWith(url , "https");
      pServer = pProtocol + 3;
   } else {
      pServer = url;
   }

   XlateBuf(&masterspace , "@" , 1  , 277, 0);

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
      if (AsHttps) {
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
     useProxy = TRUE;
     AsHttps  = FALSE;
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
/* -------------------------------------------------------------------------- */
static int Charset2ccsid(PUCHAR Charset)
{
   int ccsid;
   if (Charset == NULL
   ||  BeginsWith(Charset, "*NONE")
   ||  BeginsWith(Charset, "0")) {
      ccsid     = 0;
   } else if (BeginsWith(Charset, "*UTF8")
   ||         BeginsWith(Charset, "UTF-8")) {
      ccsid     = 1208;
   } else if (BeginsWith(Charset, "*ISO8859")
   ||         BeginsWith(Charset, "ISO8859")
   ||         BeginsWith(Charset, "ISO-8859")) {
      ccsid   = 819;
   } else if (BeginsWith(Charset , "*WIN1252")
   ||         BeginsWith(Charset , "WINDOWS-1252")) {
      ccsid   = 1252;
   } else {
      ccsid   = 1252;
   }
   return ccsid;
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
void GetChunked(PHTTP_HEADER pHh, ULONG maxLen)
{
   enum {
     GET_LEN,
     SKIP_TO_EOL,
     GET_DATA
   } Mode = GET_LEN;

   int    chunkedlen, Len;
   UCHAR  rcvbuf [4096];
   BOOL   DigitFound = FALSE;
   PUCHAR pInBuf   = pHh->ContentData;
   PUCHAR pOutBuf  = pHh->ContentData;

   pHh->ContentLength =0;
   chunkedlen = 0;

   for(;;) {

      // Get the next block - this is asynchronious from the chrunks
      if (pInBuf == pHh->pCurEnd) {
         Len = SockReceive ( rcvbuf, sizeof(rcvbuf) , 3);
         if  (Len <= 0) {  // Data is complete in outbuffer, so "disconnect" is ok (len ==0)
             return ;
         }
         pInBuf = rcvbuf;
         pHh->pCurEnd = pInBuf + Len;
         pInBuf[Len] = '\0';
      }

      // Get the lenght from: <CR><LF>12345<CR><LF>   where the first <CR><LF> is optional
      switch (Mode) {
      case GET_LEN:
         #pragma convert(1252)
         if (*pInBuf >= '0' && *pInBuf <= '9') {
            chunkedlen = (16 * chunkedlen) + *pInBuf - '0';
            DigitFound = TRUE;
         } else if (*pInBuf >= 'a' && *pInBuf <= 'f') {
            chunkedlen = (16 * chunkedlen) + *pInBuf - 'a' + 10;
            DigitFound = TRUE;
         } else if (*pInBuf >= 'A' && *pInBuf <= 'F') {
            chunkedlen = (16 * chunkedlen) + *pInBuf - 'A' + 10;
            DigitFound = TRUE;
         } else {
            pInBuf--; // Redo the same char in SKIP_TO_EOL mode
            Mode = SKIP_TO_EOL;
         }
         #pragma convert(0)
         break;

      case SKIP_TO_EOL:
         if (*pInBuf == 0x0A) {    // Skip CR  and wait for LF ...
            if (DigitFound){
               if  (chunkedlen == 0) {
                  return; // End-of-file
               }
               Mode = GET_DATA;
            } else {
               Mode = GET_LEN;
            }
         }
         break;

      // Get the data - as the abowe length describes
      case GET_DATA:
         if (chunkedlen > 0) {
            if (pHh->out) {
              fputc (*pInBuf , pHh->out);
            } else if (pHh->ContentLength < maxLen) {
              *pOutBuf = *pInBuf;
              pOutBuf++;
            }
            pHh->ContentLength++;
            chunkedlen --;
         }
         if (chunkedlen == 0) { // can not be the "else" since we will skip one char
            DigitFound = FALSE;
            Mode = GET_LEN;
         }
         break;
      }
      pInBuf++;
   }
}
/* -------------------------------------------------------------------------- */
LGL HttpRequest2 (
  PJXNODE  pOptionsStr,
  PUCHAR   reqUrl,
  PVARCHAR reqHeader,
  PVARCHAR reqPayload,
  PILOB    pReqIlob,
  PVARCHAR rspHeader,
  PVARCHAR rspPayload,
  PILOB    pRspIlob
){

   PNPMPARMLISTADDRP pParms = _NPMPARMLISTADDR();
   PJXNODE  pOptions;
   PILOBDTA pReqIlobDta = NULL;
   PILOBHDR pReqIlobHdr = NULL;
   PILOBDTA pRspIlobDta = NULL;
   PILOBHDR pRspIlobHdr = NULL;
   PUCHAR   pReqUrl;
   PUCHAR   pReqMethod;
   PUCHAR   pHead;
   PUCHAR   pPayload;
   PUCHAR   pTemp;
   PUCHAR   requestFile;
   PJXNODE  pNode;
   LONG     Len;
   SHORT    ReqTimeOut;
   PUCHAR   pReq;
   PUCHAR   pBuf;
   UCHAR    Server    [512];
   UCHAR    Port      [32];
   UCHAR    Resource  [32766];
   UCHAR    Buffer    [65535];
   UCHAR    asciiBuffer [65535];
   LONG     BufLen   = 99999999;
   ULONG    maxResponseSize = 16000000; // This default wil be overwritten by the ilob size
   UCHAR    Host      [512];
   UCHAR    User      [256];
   UCHAR    Password  [256];
   // UCHAR    Content_Type [256] = "";
   SHORT    err;
   LONG     CurContLen;
   LONG     reqLen;
   int      FromCcsid;
   int      ToCcsid ;
   BOOL     HeadFound = FALSE;
   BOOL     HeadParsed = FALSE;
   BOOL     useProxy   = FALSE;
   int i;
   RTYPE    reqType,respType;
   FILE *   inputfile = NULL;
   FILE *   outfile   = NULL;

   int retrcnt = 0;
   PUCHAR  ContentData;
   PHTTP  pHttp = GetPhttp();
   pSvr = pHttp->pServer;

   // Unpack options:
   pOptions = jx_ParseString( (PUCHAR)  pOptionsStr , "");

   if (jx_Error(pOptions) == ON) { ;
      xsetmsg("CMN2000" ,  "XHR options syntax error: %s" , jx_Message(pOptions));
      goto exit_error;
   }

   if (pParms->OpDescList->NbrOfParms >= 2 && reqUrl &&  *reqUrl > ' ') {
       pReqUrl = reqUrl;
   } else {
       pReqUrl = jx_GetValuePtr  ( pOptions,"/request/url" , "");
   }

   pReqMethod = jx_GetValuePtr  ( pOptions,"/request/method" , "GET");
   ReqTimeOut = atoi(jx_GetValuePtr  ( pOptions, "/request/timeout" , "60"));


Retry:
   openWsTrace(jx_GetValuePtr (pOptions,"/traceFile" , NULL));

   putWsTrace( "\r\n-------------- Sending HTTP request --------------\r\n");

   ParseUrl (pReqUrl, Server , Port , Resource, Host, User, Password);

   useProxy  =  (NULL != jx_GetNode ( pOptions, "/request/proxy" ));
   if (useProxy) {
      strcpy( Server , jx_GetValuePtr ( pOptions, "/request/proxy/server" , ""));
      strcpy( Port   , jx_GetValuePtr ( pOptions, "/request/proxy/port"   , "8080"));
      AsHttps  = FALSE;
   }

   if (!SocketConnect ( Server , atoi(Port), ReqTimeOut, pOptions )) {
      putWsTrace( "\r\n ------------- No Connection  --------------\r\n");
      goto exit_error;
   }
   if (pParms->OpDescList->NbrOfParms >= 8 &&  pRspIlob != NULL ) {
      pRspIlobDta = (PILOBDTA) (pRspIlob - sizeof(ILOBDTA));
      pRspIlobHdr = (PILOBHDR) (pRspIlob - pRspIlobDta->OffsetToStart);
   }

   pReq = Buffer;
   pReq += sprintf(pReq, "%s %s HTTP/1.1%s", pReqMethod , useProxy ? pReqUrl : Resource , EOL);

   // Setup request type:
   if ( (requestFile = jx_GetValuePtr ( pOptions, "/request/inputfile" , NULL)) != NULL) {
      reqType = R_FILE;
   } else if ((pTemp = jx_GetValuePtr ( pOptions, "/request/payload" , NULL)) != NULL) {
      reqType = R_JSON;
      pPayload = pTemp;
      reqLen = strlen(pPayload);


   } else if (pParms->OpDescList->NbrOfParms >= 5 && pReqIlob != NULL) {
      reqType = R_ILOB;
      pReqIlobDta = (PILOBDTA) (pReqIlob - sizeof(ILOBDTA));
      pReqIlobHdr = (PILOBHDR) (pReqIlob - pReqIlobDta->OffsetToStart);
      pHead = (PUCHAR) pReqIlobHdr + pReqIlobHdr->HeaderOffset;

      /*
      if (pReqIlobHdr->HeaderLength >= 0) {
         #pragma convert(1252)
         tRequestParmAscii(pHead , "Content-Type:" , Content_Type);
         #pragma convert(0)


      } */
      reqLen   = pReqIlobDta->DataLength;
      pPayload = pReqIlob;
   } else if (pParms->OpDescList->NbrOfParms >= 4 && reqPayload != NULL) {
      reqType = R_VARCHAR;
      reqLen   = reqPayload->Length;
      pPayload = reqPayload->String;
      // Backwards compatible whith firs implementation of httprequest:
      if (pPayload[0] == '/') {
         reqType = R_FILE;
         requestFile = pPayload;
      }
   } else {
      reqType  = R_NONE;
      reqLen   = 0;
      pPayload = NULL;
   }


   // open and get the length of the request file
   if (reqType == R_FILE) {
      inputfile = fopen(requestFile, "rb");
      if (inputfile == NULL) {
         xsetmsg("CMN2000" ,  "Input file %s error: %s " , requestFile , strerror(errno));
         goto exit_error;
      }

      fseek (inputfile, 0, SEEK_END);   // end of file
      reqLen = ftell(inputfile);
      fseek (inputfile, 0, SEEK_SET);   // top of file
      // If data to send
   } else if ((reqType == R_VARCHAR && reqLen > 0)
          ||   reqType == R_JSON) {
      PUCHAR inputContentType = jx_GetValuePtr ( pOptions, "/request/header/content-type" , "");
      int inputFromCCSID = atoi(jx_GetValuePtr ( pOptions, "/request/fromCCSID" , "0"));
      int inputToCCSID   = getCcsid(inputContentType, "charset=", /*IsASCII=*/ FALSE);
      int inputToCCSID2  = atoi(jx_GetValuePtr ( pOptions, "/request/toCCSID" , "-1"));

      // An override value is given by the "options"
      if (inputToCCSID2 != -1) {
         inputToCCSID = inputToCCSID2;
      }
      // Default to 1208 (UTF-8)
      if (inputToCCSID == 0) {
         inputToCCSID = 1208;  // Fix - was 1252 until 18.07.2022
      }
      reqLen = XlateBuf(asciiBuffer, pPayload  , reqLen  , inputFromCCSID , inputToCCSID);
   }

   // TODO !! take the content type from the ilob id is was not given by the JSON


   if (ON == jx_Has( pOptions, "/request/authenticate" )) {
      strcpy( User     , jx_GetValuePtr ( pOptions, "/request/authenticate/userid"   , ""));
      strcpy( Password , jx_GetValuePtr ( pOptions, "/request/authenticate/password" , ""));
   }
   pReq += addRealmLogin (pReq, User , Password);

   if (reqLen > 0) {
      pReq += sprintf(pReq, "Content-Length: %d%s" , reqLen , EOL);
   }

   pReq += sprintf(pReq, "User-Agent: IceBreak%s", EOL);
   pReq += sprintf(pReq, "Host: %s%s", Host, EOL);
   //pReq += sprintf(pReq, "Connection: Close%s", EOL);
   pReq += sprintf(pReq, "Connection: keep-alive%s", EOL);



   pNode = jx_GetNodeChild ( jx_GetNode (  pOptions, "/request/headers"));
   while(pNode) {
      pReq += sprintf(pReq, "%s: %s%s" , jx_GetNodeNamePtr(pNode),  jx_GetValuePtr (pNode, "", "") , EOL);
      pNode = jx_GetNodeNext(pNode);
   }

   // VARCHAR ( in ebcdic) Need better implementataion: Todo!! update the JSON object whith headers
   if (reqHeader) {
      PUCHAR rh = vc2str(reqHeader);
      if (*rh > ' ') {
         pReq += sprintf(pReq, "%s", rh);
         if (rh[strlen(rh)-1] >= ' ') {
            pReq += sprintf(pReq, "%s", EOL);  // Always a EOL
         }
      }
   }

   Len = pReq - Buffer;
   e2a(Buffer , Buffer , Len);

   // ILOB's Need better implementataion: Todo!! update the JSON object whith headers
   if (pReqIlob && pReqIlobHdr->HeaderLength >0) {
      memcpy(pReq , pHead , pReqIlobHdr->HeaderLength);
      pReq += pReqIlobHdr->HeaderLength;
      pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
   }

   if (IsNewLineAscii( * (pReq -1))) {
      pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
   } else {
      pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
      pReq += sprintf(pReq, "%c%c", 0x0d , 0x0a);
   }

   // Send headers
   Len = pReq - Buffer;
   if (FALSE == SockSend( Buffer , Len )) {
      goto exit_error;
   }

   // Handle payload
   if (reqType == R_FILE) {
      while ((Len = fread( Buffer  , 1 , sizeof(Buffer), inputfile)) > 0) {
         if (FALSE == SockSend( Buffer , Len )) {
            fclose(inputfile);
            goto exit_error;
         }
      }
      fclose(inputfile);
   }
   // If data to send
   else if ((reqType == R_VARCHAR && reqLen > 0)
        ||   reqType == R_JSON) {
      if (FALSE == SockSend( asciiBuffer, reqLen )) {
         goto exit_error;
      }
   }
   else if (reqType == R_ILOB) {
      if (FALSE == SockSend( pPayload, reqLen )) {
         goto exit_error;
      }
   }

   // Setup response type:
   if ((pTemp = jx_GetValuePtr ( pOptions, "/response/outputfile" , NULL)) != NULL) {
      UCHAR mode [32];
      respType = R_FILE;
      unlink (pTemp);
      sprintf(mode , "wb,o_ccsid=%d" ,  HttpHeader.Ccsid == 0 ? 1208 : HttpHeader.Ccsid);
      outfile =  fopen(pTemp , mode);
   } else  if (pParms->OpDescList->NbrOfParms >= 8 && pRspIlob != NULL) {
      respType = R_ILOB;
      maxResponseSize = pRspIlobHdr->DataSize;
   } else if (pParms->OpDescList->NbrOfParms >= 4 && reqPayload != NULL) {
      respType = R_VARCHAR;
      pPayload = reqPayload->String;
   } else if (pParms->OpDescList->NbrOfParms >= 4 && reqPayload != NULL) {
      respType = R_VARCHAR;
      pPayload = reqPayload->String;
   } else {
      respType = R_NONE;
      pPayload = NULL;
   }


   putWsTrace( "\r\n-------------- Receiving HTTP request Response --------------\r\n");

   // repeat until no more proxies
   memset(&HttpHeader, 0, sizeof(HttpHeader));
   HeadFound = FALSE;
   rcvTotalLen = 0;
   pBuf = Buffer;

   for (;;) { // repeat until a header is received
      Len = SockReceive ( pBuf, 65535 -1 ,  ReqTimeOut);
      if  (Len == -2) {  // timeout
        if (rcvTotalLen > 0) {
           break;
        } else {
           goto exit_error;
        }
      }
      if  (Len < 0) {  // Protocol error
         goto exit_error;
      }

      // "Connection close" - end of transmission and no "Content-Length" provided, the we are done
      if (Len == 0 && HttpHeader.hasContentLength == false) {
         break;
      }

      rcvTotalLen += Len;
      pBuf += Len;
      *pBuf = '\0';

      // Header not found yet - Search for it
      if (! HeadFound ) {
         HeadFound = LookForHeader(Buffer, rcvTotalLen, &ContentData);
         if (HeadFound) {

            err = ParseResponse(&HttpHeader , Buffer, ContentData);
            if (err) {
              xsetmsg("CMN2000" ,  "Invalid response header: %d" , err);
              goto exit_error;
            }

            CurContLen = rcvTotalLen - HttpHeader.HeadLen;
            HttpHeader.pCurEnd = pBuf;


            // Handle header payload:
            if (respType == R_ILOB) {
               pHead = (PUCHAR) pRspIlobHdr + pRspIlobHdr->HeaderOffset;
               pRspIlobHdr->HeaderLength = min(HttpHeader.HeadLen, pRspIlobHdr->HeaderSize);
               // memcpy ( pHead , pRspIlob , pRspIlobHdr->HeaderLength);
               memcpy ( pHead , Buffer , pRspIlobHdr->HeaderLength);

               memcpy ( pRspIlob , HttpHeader.ContentData , CurContLen);

               HttpHeader.ContentData = ContentData = pRspIlob;
               HttpHeader.pCurEnd = pBuf = pRspIlob + CurContLen ;

               pRspIlobDta->DataLength = HttpHeader.ContentLength;
               pRspIlobHdr->Ccsid = HttpHeader.Ccsid;


            } else if (respType == R_VARCHAR) {
               rspHeader->Length = min(HttpHeader.HeadLen, 32766);
               memcpy( rspHeader->String, Buffer , rspHeader->Length);
               rspPayload->Length = min(HttpHeader.ContentLength, 32766);
            }


            if (HttpHeader.Status == 301     // Temporary moved
            ||  HttpHeader.Status == 302) {  // Permanent moved
                if ( retrcnt++ == 3) goto exit_error; // retry is used for avoid recursive redirection
                pReqUrl = HttpHeader.Location; // redo it all over with the new location
                putWsTrace( "\r\n redirected to %s\r\n",  HttpHeader.Location );
                CleanUp();
                goto Retry;
              }



            if (HttpHeader.Status == 100) {  // Continue is was a proxy

               putWsTrace( "\r\n-- Proxy continue received --\r\n");
               HeadFound = FALSE;
               pBuf = Buffer;
               rcvTotalLen = 0;
               continue;   // Found but start over again - it was a proxy
            }


            if (HttpHeader.Chunked) {
               break;  // !! Found
            }

            // For now continue but return er error
            //   CleanUp();
            //   if (pOptions  != pOptionsStr  ) jx_Close(&pOptions); // Was parser here  - need to free-up
            //   return ON;


            // Dont try to get data if it was a HEAD request - it is only the header
            // or status 204 => no content
            if (BeginsWith(pReqMethod, "HEAD")
            ||  HttpHeader.Status == 204 )     {  // No Content, dont read any longer
                 goto exit_with_data;
            }

            // Received data for "non chunked"
            if (respType == R_FILE && HttpHeader.ContentData) {
               fwrite (HttpHeader.ContentData , 1, CurContLen , outfile);
               while  ( Len > 0 && (CurContLen < HttpHeader.ContentLength || HttpHeader.hasContentLength == false)) {
                 Len = SockReceive ( Buffer, sizeof(Buffer), ReqTimeOut);
                 fwrite (Buffer , 1, Len , outfile);
                 CurContLen += Len;
               }
               goto exit_with_data;
            }
         }
      }
      CurContLen = rcvTotalLen - HttpHeader.HeadLen;
      if (HttpHeader.hasContentLength == true
      &&  CurContLen >= HttpHeader.ContentLength) {
         break;  // !! Found
      }
   }

   // When no "contents-length" is given
   if (HttpHeader.hasContentLength == false) {
      if (respType == R_ILOB) {
         pRspIlobDta->DataLength = CurContLen;
      } else  if (respType == R_VARCHAR) {
         rspPayload->Length = CurContLen;
      }
   } else {
      // We have a content length: Check it !
      if (CurContLen < HttpHeader.ContentLength) {
         xsetmsg("CMN2000",  "Invalid length %d receieved compared to header %d", CurContLen, HttpHeader.ContentLength);
         goto exit_error;
      }
   }

   if (HttpHeader.Chunked) {
      HttpHeader.out = outfile; // If any
      GetChunked(&HttpHeader , maxResponseSize);
      if (respType == R_ILOB) {
         pRspIlobDta->DataLength = HttpHeader.ContentLength;
      } else  if (respType == R_VARCHAR) {
         rspPayload->Length = HttpHeader.ContentLength;
      }
   }


exit_with_data:
   CleanUp();
   if (outfile) fclose(outfile);
   if (pOptions  != pOptionsStr) jx_Close(&pOptions); // Was parser here  - need to free-up
   if (HttpHeader.Status >= 200 && HttpHeader.Status < 300) {  // Must be OK
      return (OFF); // OK;
   } else {
      xsetmsg("CMN2000" ,  "Invalid response status: %d" , HttpHeader.Status);
      return (ON);  // error;
   }

exit_error:
   CleanUp();
   if (outfile) fclose(outfile);
   if (pOptions  != pOptionsStr) jx_Close(&pOptions); // Was parser here  - need to free-up
   return (ON);  // error;
}
/* --------------------------------------------------------------------------
 * New wrapper for ILOB_HttpRequest
 * -------------------------------------------------------------------------- */
LGL ILOB_HttpRequest(PVARCHAR ReqUrl, PUSHORT ReqTimeOut, PVARCHAR ReqMethod, PILOB pReqIlob, PILOB pRspIlob)
{
  UCHAR    optionsStr[256];
  LGL      error;

  PJXDELIM pDel = jx_GetDelimiters();
   JXDELIM gem  = * pDel;
  PJXNODE  pOptions = jx_NewObject(NULL);

  jx_SetDelimiters((PJXDELIM )"/\\@[] .{}");
  jx_SetStrByName  (pOptions, "/request/method" , vc2str(ReqMethod));
  jx_SetIntByName  (pOptions, "/request/timeout", *ReqTimeOut) ;

  error = HttpRequest2 (
     (PJXNODE)   pOptions,
     (PUCHAR)    vc2str(ReqUrl),
     (PVARCHAR)  NULL, // reqHeader,
     (PVARCHAR)  NULL, // reqPayload,
     (PILOB)     pReqIlob,
     (PVARCHAR)  NULL, // rspHeader,
     (PVARCHAR)  NULL, // rspPayload,
     (PILOB)     pRspIlob
  );

  jx_SetDelimiters(&gem);
  jx_NodeDelete(pOptions);

  return error;
}
/* --------------------------------------------------------------------------
 * New wrapper for HttpRequest
 * -------------------------------------------------------------------------- */
/****** Not ready yet "!!
LGL HttpRequest(PVARCHAR ReqUrl, PUSHORT ReqTimeOut, PVARCHAR ReqMethod, PVARCHAR ReqData, PVARCHAR ReqContType ,
                PVARCHAR ReqHead, PUCHAR ReqXlate, PUCHAR RespXlate, PVARCHAR RespHead , PVARCHAR RespData)

  LGL error;
  PJXNODE pOptinons = jx_NewObject();

  jx_SetString (pOptinons , "request/method" , vc2str(ReqMethod));
  jx_SetInt    (pOptinons , "request/timeout" , *ReqTimeOut);
  jx_SetString (pOptinons , "request/headers/Content-Type" , vc2str(ReqContType));


  error = HttpRequest2 (
     (PJXNODE)   pOptinons,
     (PUCHAR)    vc2str(ReqUrl),
     (PVARCHAR)  pParms->OpDescList->NbrOfParms >= 6 ? ReqHead : NULL, // reqHeader,
     (PVARCHAR)  pParms->OpDescList->NbrOfParms >= 4 ? ReqData : NULL, // reqPayload,
     (PILOB)     NULL,
     (PVARCHAR)  pParms->OpDescList->NbrOfParms >= 9 ? RespHead : NULL, // rspHeader,
     (PVARCHAR)  pParms->OpDescList->NbrOfParms >=10 ? RespData : NULL, // rspPayload,
     (PILOB)     NULL
  );
  jx_Close(&pOptinons);
  return error;

*/
/* -------------------------------------------------------------------------- */
LGL HttpRequest(PVARCHAR ReqUrl, PUSHORT ReqTimeOut, PVARCHAR ReqMethod, PVARCHAR ReqData, PVARCHAR ReqContType ,
                PVARCHAR ReqHead, PUCHAR ReqXlate, PUCHAR RespXlate, PVARCHAR RespHead , PVARCHAR RespData)

{
   LONG   Len;
   UCHAR  Req[32766];
   PUCHAR pReq = Req;
   UCHAR  Buf[500000];
   LONG   BufLen = sizeof(Buf);
   PUCHAR pBuf = Buf;
   UCHAR  Server  [512];
   UCHAR  Port    [32];
   UCHAR  Resource[32766];
   UCHAR  Host [512];
   UCHAR  User     [256];
   UCHAR  Password [256];

   SHORT  err;
   LONG   CurContLen;
   int    FromCcsid;
   int    ToCcsid ;
   BOOL HeadFound = FALSE;
   BOOL HeadParsed = FALSE;
   int i, totlen =0;
   int retrcnt = 0;
   PUCHAR  ContentData;
   BOOL   save2stream = FALSE;
   FILE * outStream = NULL;
   PHTTP  pHttp = GetPhttp();
   pSvr = pHttp->pServer;

   if (RespData && RespData->String[0] == '/') {
     save2stream = TRUE;
     RespData->String[RespData->Length] = '\0';
     outStream = fopen( RespData->String , "wb");
     save2stream = (outStream != NULL);
   }

   openWsTrace(NULL);
   putWsTrace( "\r\n-------------- Sending HTTP request --------------\r\n");

   if (ReqData) {
     ToCcsid = Charset2ccsid(ReqXlate);
     FromCcsid = 0;
     if (ToCcsid > 0) {
        ReqData->Length = Xlate(ReqData->String , ReqData->Length, FromCcsid, ToCcsid);
     }
   }

   ParseUrl (vc2str(ReqUrl) , Server , Port , Resource, Host, User, Password);

Retry:
   if (retrcnt ++ == 3) {
      return (ON); // error
   }
   if (!SocketConnect ( Server , atoi(Port), *ReqTimeOut, NULL)) {
      putWsTrace( "\r\n ------------- No Connection  --------------\r\n");
      return (ON); // error
   }
   pReq = Req;
   pReq += sprintf(pReq, "%s %s HTTP/1.1%s", ReqMethod ? vc2str(ReqMethod):"GET", Resource , EOL);
   if (ReqContType && *vc2str(ReqContType) > ' '){
      pReq += sprintf(pReq, "Content-Type: %s%s", vc2str(ReqContType), EOL);
   } else if (ReqMethod && BeginsWith(vc2str(ReqMethod) , "POST")) {
      pReq += sprintf(pReq, "Content-Type: application/x-www-form-urlencoded%s", EOL);
   } else {
      pReq += sprintf(pReq, "Content-Type: text/html%s", EOL);
   }
   pReq += addRealmLogin (pReq, User , Password);
   pReq += sprintf(pReq, "Content-Length: %d%s" , ReqData ? ReqData->Length:0 , EOL);
   pReq += sprintf(pReq, "User-Agent: IceBreak%s", EOL);
   pReq += sprintf(pReq, "Host: %s%s", Host, EOL);
   pReq += sprintf(pReq, "Connection: Close%s", EOL);

// pReq += sprintf(pReq, "Authorization: Negotiate TlRMTVNTUAABAAAAB7IAgAkACQAnAAAABwAHACAAAABMQU5QQzMyV09SS0d
// pReq += sprintf(pReq, "Host: icebreak.org%s", EOL);

   if (ReqHead) {
      PUCHAR rh = vc2str(ReqHead);
      if (*rh > ' ') {
         pReq += sprintf(pReq, "%s", rh);
         if (rh[strlen(rh)-1] >= ' ') {
            pReq += sprintf(pReq, "%s", EOL);  // Always a EOL
         }
      }
   }
   pReq += sprintf(pReq, "%s", EOL);  // Always a extra EOL before contents
   Len = pReq - Req;
   e2a(Req , Req , Len);

   if (ReqData) {
      memcpy(pReq , ReqData->String  , ReqData->Length);
      Len += ReqData->Length;
   }

   if (FALSE == SockSend(  Req , Len  )) {
      goto exit_error;
   }



   putWsTrace( "\r\n-------------- Receiving HTTP request Response --------------\r\n");

   // repeat until no more proxies
   do {
      HeadFound = FALSE;
      HeadParsed = FALSE;
      pBuf = Buf;
      totlen = 0;
      for (;;) { // repeat until a header is received
         PUCHAR p;
         Len = SockReceive ( pBuf, BufLen - totlen, ReqTimeOut ? *ReqTimeOut: 15);
         if  (Len == -2) {  // timeout
           CleanUp();
           return ON;
         }
         if  (Len < 0) {  // Protocol error
           CleanUp();
           goto Retry;
         }
         totlen += Len;
         pBuf[Len] = 0;
         pBuf[Len+1] = 0;


         // Header not found yet - Search for it
         if (! HeadFound ) {
            for (i=0;i<totlen;i++) {
              p = Buf + i;

              if (p[0] == 0x0d && p[1] == 0x0a
              &&  p[2] == 0x0d && p[3] == 0x0a) {
                ContentData= p + 4;
                HeadFound = TRUE;
                *p = 0;
                break;
              } else if ((p[0] == 0x0d && p[1] == 0x0d)
              ||         (p[0] == 0x0a && p[1] == 0x0a)) {
                ContentData= p + 2;
                HeadFound = TRUE;
                *p = 0;
                break;
              }
            }
         }
         // Header found but not parsed yet - Parse it
         if (HeadFound) {
           if (! HeadParsed) {
              HeadParsed = TRUE;
              err = ParseResponse(&HttpHeader , Buf, ContentData);
              if (err) {
                CleanUp();
                return ON;
              }

              if (HttpHeader.Chunked) {
                HttpHeader.pCurEnd = pBuf + Len;
                break;  // !! Found
              }
              if (HttpHeader.Status == 100) {  // Continue is was a proxy
                break;  // !! Found
              }
              if (HttpHeader.Status == 301     // Temporary moved
              ||  HttpHeader.Status == 302) {  // Permanent moved
                // retrcnt--; !! retry is used for avoid recusrive redirection
                ParseUrl (HttpHeader.Location  , Server , Port , Resource, Host, User, Password);
                CleanUp();
                goto Retry;
              }
              // Dont try to get data if it was a HEAD request - it is only the header
              if (ReqMethod && BeginsWith(vc2str(ReqMethod) , "HEAD")) {
                if (RespHead) {
                  RespHead->Length = HttpHeader.HeadLen;
                  memcpy(RespHead->String , Buf, RespHead->Length);
                  RespHead->String[RespHead->Length] = 0;
                }
                CleanUp();
                return OFF; // No errors
              }
           }
           CurContLen = totlen - HttpHeader.HeadLen;

           if (save2stream) {
             if (HttpHeader.ContentData) {
               fwrite (HttpHeader.ContentData , 1, CurContLen , outStream);
               while  ( CurContLen < HttpHeader.ContentLength && Len > 0) {
                 Len = SockReceive ( Buf, sizeof(Buf), ReqTimeOut ? *ReqTimeOut: 15);
                 fwrite (Buf , 1, Len , outStream);
                 CurContLen += Len;
               }
               fclose(outStream);
               CleanUp();
               return OFF; // No errors
             }
           }

           if ( CurContLen >= HttpHeader.ContentLength) {
              break;  // !! Found
           }
         }
         pBuf+= Len;
      }
   } while (HttpHeader.Status == 100);  // Continue

   pBuf = Buf;
   Len = totlen;

   if (HttpHeader.Chunked) {
      HttpHeader.out = outStream;
      GetChunked(&HttpHeader, 32766);
      if (HttpHeader.out) fclose(HttpHeader.out);
   } else {
      if (HttpHeader.ContentLength == 0) {
         HttpHeader.ContentLength = CurContLen;
      }
   }

   // The header
   if (RespHead) {
      RespHead->Length = HttpHeader.HeadLen;
      memcpy(RespHead->String , Buf, RespHead->Length);
      RespHead->String[RespHead->Length] = 0;
   }

   // The "body"
   if (RespData) {
      int len ;
      len =  min(HttpHeader.ContentLength  , 32766);

      // Determin the conversion table type
      FromCcsid = Charset2ccsid(RespXlate);
      ToCcsid   = 0;

      RespData->Length = XlateBuf(RespData->String, HttpHeader.ContentData , len , FromCcsid, ToCcsid);
   } else {
      int len ;
      PUCHAR pBuf = pHttp->OutBuf + pHttp->OutBufLen;
      len =  pHttp->OutBufSize - pHttp->OutBufLen;
      len =  min(HttpHeader.ContentLength  , len);
      memcpy(pBuf , HttpHeader.ContentData , len);
      pHttp->OutBufLen += len;
   }

   CleanUp();

   // Chunked have already made validy test
   if (! HttpHeader.Chunked) {
      if (CurContLen < HttpHeader.ContentLength) {
        return(ON);  // Error !!
      }
   }

   return(OFF);

exit_error:
   CleanUp();
   return(ON); // Error
}