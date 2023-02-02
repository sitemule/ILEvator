#ifndef ILEVATOR_HTTPCLIENT_H
#define ILEVATOR_HTTPCLIENT_H
#define CR 0x0D
#define LF 0x25

#include "ostypes.h"
#include "ilevator.h"

LONG urlEncodeBlanks  (PUCHAR outBuf , PUCHAR inBuf);
void putWsTrace(PILEVATOR pIv, PUCHAR ctlStr, ...);
void xsetmsg(PILEVATOR pIv , PUCHAR msgid , PUCHAR Ctlstr, ...);
PUCHAR findEOL(PUCHAR p);
int getCcsid(PUCHAR base);
BOOL isNewLineAscii(UCHAR c);
BOOL lookForHeader(PUCHAR Buf, LONG totlen, PUCHAR * contentData);
VOID parseHttpParm(PILEVATOR pIv, PUCHAR Parm , PUCHAR Value);
SHORT parseResponse(PILEVATOR pIv, PUCHAR buf, PUCHAR contentData);
LONG addRealmLogin (PUCHAR pReq, PUCHAR user , PUCHAR password);
UCHAR masterspace (void); 
void initialize(void);
API_STATUS sendHeader (PILEVATOR pIv) ;
API_STATUS receiveHeader ( PILEVATOR pIv);
API_STATUS receiveData ( PILEVATOR pIv);

void parseUrl (PILEVATOR pIv, PUCHAR url);

#endif