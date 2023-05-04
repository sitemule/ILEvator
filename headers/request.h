#ifndef ILEVATOR_REQUEST_H
#define ILEVATOR_REQUEST_H

#include "ostypes.h"
#include "varchar.h"

PVOID iv_request_new(VARCHAR12 method, VARCHAR url, VARCHAR1024 mimeType);
void iv_request_addHeaders(PVOID request, PVOID headers);
void iv_request_dispose(PVOID request);
LVARPUCHAR iv_request_toString(PVOID request);

#endif