#ifndef ILEVATOR_STREAM_MEMORY_H
#define ILEVATOR_STREAM_MEMORY_H

#include "ostypes.h"
#include "stream.h"

PVOID iv_stream_memory_new(ULONG length, PVOID data);

LGL iv_stream_memory_hasMore(PVOID stream);

STREAM_BUFFER iv_stream_memory_read(PVOID stream);

LGL iv_stream_memory_write(PVOID stream, PVOID data, ULONG length);

void iv_stream_memory_finalize(PVOID stream);

#endif