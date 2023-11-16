#ifndef ILEVATOR_URL_H
#define ILEVATOR_URL_H

#include "ostypes.h"
#include "varchar.h"

typedef _Packed struct _URL {
    VARCHAR12 protocol;
    VARCHAR128 username;
    VARCHAR128 password;
    VARCHAR1024 host;
    USHORT port;     
    VARCHAR path;
    VARCHAR query;
    VARCHAR1024 proxy;
    LGL ipv6;
} URL, *PURL;

URL iv_url_parse(VARCHAR url);

VARCHAR iv_url_toString(PURL url);

#endif