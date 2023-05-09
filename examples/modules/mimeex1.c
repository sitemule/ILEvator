#include <stdio.h>
#include "mime.h"
#include "teraspace.h"


//
// This demo shows how to get the corresponding CCSID from a MIME type
//


int main() {
  int cp;
  ULONG ccsid;
  PVARCHAR mimeType;
  mimeType = teraspace_calloc(sizeof(VARCHAR));
  
  str2vc(mimeType, "text/plain");
  ccsid = iv_mime_getCcsid(mimeType);
  printf("MIME Type: text/plain\n");
  printf("CCSID: %lu\n", ccsid);  // prints 367
  
  str2vc(mimeType, "text/plain;charset=windows-1252");
  ccsid = iv_mime_getCcsid(mimeType);
  printf("MIME Type: text/plain;charset=windows-1252\n");
  printf("CCSID: %lu\n", ccsid);  // prints 1252
  
  str2vc(mimeType, "text/plain;charset=unicode");
  ccsid = iv_mime_getCcsid(mimeType);
  printf("MIME Type: text/plain;charset=unicode\n");
  printf("CCSID: %lu\n", ccsid);  // prints 1200

  str2vc(mimeType, "application/json");
  ccsid = iv_mime_getCcsid(mimeType);
  printf("MIME Type: application/json\n");
  printf("CCSID: %lu\n", ccsid);  // prints 1208
  
  teraspace_free(&mimeType);
  
  return 0;
}