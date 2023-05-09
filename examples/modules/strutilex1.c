#include <stdio.h>
#include "ostypes.h"
#include "strutil.h"


int main() {
  UCHAR buffer[256];
  
  strutil_itoa(358, buffer, 10);
  printf("Zahl: %s\n", buffer);
  
  return 0;
}
