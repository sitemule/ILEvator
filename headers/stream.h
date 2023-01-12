#ifndef ILEVATOR_STREAM_H
#define ILEVATOR_STREAM_H

#include "ostypes.h"
#include "varchar.h"

typedef struct _STREAM_BUFFER {
    PVOID data;
    ULONG length;
} STREAM_BUFFER, *PSTREAM_BUFFER;

LGL iv_stream_hasMore(PVOID stream);

STREAM_BUFFER iv_stream_read(PVOID pointer);

LGL iv_stream_write(PVOID stream, PVOID data, ULONG length);

void iv_stream_setBlockSize(PVOID stream, ULONG blockSize);

void iv_stream_finalize(PVOID stream);

#endif