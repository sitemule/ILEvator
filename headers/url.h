#ifndef ILEVATOR_URL_H
#define ILEVATOR_URL_H

#include "ostypes.h"
#include "varchar.h"

typedef _Packed struct _URL {
    ULONG protocolLength;
    UCHAR protocol[12];
    ULONG usernameLength;
    UCHAR username[100];
    ULONG passwordLength;
    UCHAR password[100];
    ULONG hostLength;
    UCHAR host[1000];
    USHORT port;     
    ULONG pathLength;
    UCHAR path[65530];
    ULONG queryLength;
    UCHAR query[65530];
    ULONG proxyLength;
    UCHAR proxy[1000];
    LGL ipv6;
} URL, *PURL;

URL iv_url_parse(LVARCHAR url);

LVARCHAR iv_url_toString(PURL url);

#endif