#include <stdio.h>
#include "stream.h"
#include "streammem.h"


//
// This demo shows how to read from and write to a memory stream.
//


void writeToStream(PVOID stream, PVOID streamData);
void readFromStream(PVOID stream);


int main() {
    PVOID stream;
    PVOID nullPointer;
    PVOID streamData = "Hello World";
    ULONG blockSize = 4;
    
    // create a new stream "object"
    stream = iv_stream_memory_new(100, nullPointer);
    
    // setting the block size of the buffer 
    // which is used when reading from the stream
    iv_stream_setBlockSize(&stream, blockSize);
    
    writeToStream(stream, streamData);
    
    readFromStream(stream);
    
    // free all memory allocated by the stream "object"
    iv_stream_finalize(&stream);
    
    return 0;
}


void writeToStream(PVOID stream, PVOID streamData) {
    printf("Writing to stream ...\n");
    
    LGL rc = iv_stream_write(&stream, &streamData, strlen(streamData));
    if (OFF == rc) {
      printf("Not successfully written all data to the stream\n");
    }
}


void readFromStream(PVOID stream) {
    STREAM_BUFFER buffer;
    
    printf("Reading from stream ...\n");
    
    while (iv_stream_hasMore(&stream) == ON) {
        buffer = iv_stream_read(&stream);
        printf("got %u bytes\n", buffer.length);
        printf("data: %s\n", buffer.data);
    }
}
