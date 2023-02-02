#include <stdio.h>
#include "ostypes.h"
#include "varchar.h"
#include "url.h"


//
// This demo shows how to parse an URL and access the different parts of the URL
//


int main() {
  URL url;
  LVARCHAR s;
  LVARCHAR output;
      
  str2lvc (&s , "http://me");
      
  url = iv_url_parse(s);
  printf("Protocol size: %d\n", url.protocolLength);
  printf("Host size: %d\n", url.hostLength);
  printf("Host: %.12s\n", url.host);
  
  output = iv_url_toString(&url);
  printf("URL: %s\n", output.String);
  
  return 0;
}
