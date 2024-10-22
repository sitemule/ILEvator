#include <stdio.h>
#include "ostypes.h"
#include "varchar.h"
#include "request.h"
#include "strutil.h"
#include "teraspace.h"
#include "xlate.h"


int main() {
  VARCHAR12 method;
  VARCHAR1024 mimeType;
  VARCHAR url;
  LVARPUCHAR requestString;
  PVOID request;
  UCHAR s[100];
  ULONG length;
  
  str2vc(&method, "GET");
  method.Length = strlen(method.String);
  
  #pragma convert(1252)
  str2vc(&mimeType, "application/json");
  mimeType.Length = strlen(mimeType.String);
  
  str2vc(&url, "https://sitemule.com/index.html");
  url.Length = strlen(url.String);
  #pragma convert(0)
  
  request = iv_request_new_packedUrl(method, url, mimeType);
  requestString = iv_request_toString(&request);
  
  length = xlate_translateBuffer(&s[0], requestString.String, requestString.Length, 1208, 0);
  s[length] = '\0';
  
  printf("HTTP request: %s\n", s);
  
  iv_request_dispose(request);
  teraspace_free((PVOID) requestString.String);
  
  return 0;
}