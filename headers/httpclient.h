#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#define CR 0x0D
#define LF 0x25

LONG urlEncodeBlanks  (PUCHAR outBuf , PUCHAR inBuf);
void putWsTrace(PILEVATOR pIv, PUCHAR ctlStr, ...);
void xsetmsg(PILEVATOR pIv , PUCHAR msgid , PUCHAR Ctlstr, ...);
PUCHAR findEOL(PUCHAR p);
int charset2ccsid(PUCHAR Charset);
int getCcsid(PUCHAR base , PUCHAR charsetStr, BOOL isASCII);
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

void parseUrl (
    PILEVATOR pIv,
    PUCHAR url, 
    PUCHAR server , 
    PUCHAR port , 
    PUCHAR resource, 
    PUCHAR host, 
    PUCHAR user, 
    PUCHAR password);

#endif