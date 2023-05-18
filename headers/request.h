#ifndef ILEVATOR_REQUEST_H
#define ILEVATOR_REQUEST_H

#include "ostypes.h"
#include "varchar.h"

PVOID iv_request_new(VARCHAR12 method, PUCHAR host, LONG port, PUCHAR path, PUCHAR query, VARCHAR1024 mimeType);
#pragma map(iv_request_new, "iv_request_new_unpackedUrl")
void iv_request_addHeaders(PVOID request, PVOID headers);
void iv_request_dispose(PVOID request);
LVARPUCHAR iv_request_toString(PVOID request);

#endif