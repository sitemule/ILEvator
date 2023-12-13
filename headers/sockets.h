/* ------------------------------------------------------------- */
/* Program . . . : SOCKETS                                       */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : SSL/ socket wrapper                           */
/*                                                               */
/* By     Date       Task    Description                         */
/* NL     15.05.2005 0000000 New program                         */
/* NL     25.02.2007     510 Ignore namespace for WS parameters  */
/* ------------------------------------------------------------- */

#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <gskssl.h>
#include "ostypes.h"

typedef enum {
   PLAIN_SOCKET = 0,
   SECURE_HANDSHAKE_IMEDIATE = 1,
   SECURE_HANDSHAKE_LATER = 2
} USESSL , *PUSESSL;

typedef struct _SOCKETS_TLS_CONFIG {
    LONG version;
    LONG enabled;
} SOCKETS_TLS_CONFIG;

typedef struct _SOCKETS {
    SOCKET socket;
    BOOL   isInitialized;
    BOOL   hasSocket;
    USESSL asSSL;
    LONG   timeOut; // Milisec 
    FILE * trace;
    UCHAR  tracefilename   [128];
    UCHAR  certificateFile [128];
    UCHAR  keyringPassword [32];
    gsk_handle my_env_handle;         // secure environment handle
    gsk_handle my_session_handle;     // secure session handle
    validationCallBack valCallBack;
    int rcvTotalLen;
    BOOL   isSecure;
    struct _SOCKETS_TLS_CONFIG tls[4];
} SOCKETS, *PSOCKETS;


// Prototypes
PSOCKETS sockets_new(void);
void  sockets_free (PSOCKETS ps);
void  sockets_setKeystore(PSOCKETS ps,USESSL asSSL, PUCHAR certificateFile , PUCHAR keyringPassword);
void  sockets_setTrace(PSOCKETS ps, PUCHAR traceFileName);
void  sockets_putTrace(PSOCKETS ps, PUCHAR Ctlstr, ...);

BOOL  sockets_connect(PSOCKETS ps, PUCHAR ServerIP, LONG ServerPort, LONG TimeOut);
BOOL  sockets_set_secure (PSOCKETS ps);
BOOL  sockets_handshakeSSL (PSOCKETS ps);
LONG  sockets_send (PSOCKETS ps, PUCHAR Buf, LONG Len);
LONG  sockets_sendXlate (PSOCKETS ps, PUCHAR Buf, LONG Len);
LONG  sockets_sendCssXlate      (PSOCKETS ps, int fromCcsId, int toCcsId, PUCHAR Buf, LONG Len);
LONG  sockets_receive (PSOCKETS ps, PUCHAR Buf, LONG Len, LONG TimeOut);
LONG  sockets_receiveXlate (PSOCKETS ps, PUCHAR Buf, LONG Len, LONG TimeOut);
LONG  sockets_printf (PSOCKETS ps, PUCHAR Ctlstr , ...);
LONG  sockets_printfXlate (PSOCKETS ps, PUCHAR Ctlstr , ...);
LONG  sockets_printfCcsXlate   (PSOCKETS ps, int fromCcsId, int toCcsId, PUCHAR Ctlstr , ...);
void  sockets_close(PSOCKETS ps);
void  sockets_setTls(PSOCKETS ps, LONG tlsVersion, LONG status);

#define  SOCK_INVALID -1

#define  SOCK_OK 0
#define  SOCK_ERROR -1
#define  SOCK_TIMEOUT -2

#endif
