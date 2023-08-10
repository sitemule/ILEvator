#ifndef ILEVATOR_REQUEST_H
#define ILEVATOR_REQUEST_H

#include "ostypes.h"
#include "varchar.h"

PVOID iv_request_new(VARCHAR12 method, PUCHAR host, LONG port, PUCHAR path, PUCHAR query);
#pragma map(iv_request_new, "iv_request_new_unpackedUrl")
#pragma descriptor (void iv_request_new(void))

void iv_request_addHeaders(PVOID request, PVOID headers);
void iv_request_dispose(PVOID request);
LVARPUCHAR iv_request_toString(PVOID request);
void iv_request_setTextBody(PVOID request, LVARCHAR body, ULONG ccsid);

#endif