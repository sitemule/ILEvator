#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ostypes.h"
#include "varchar.h"
#include "message.h"


void iv_joblog(PVARCHAR message)
{
   APIERR apierr = APIERR_INIT;
   char msgkey [10];
   long stackcount=1;
   QMHSNDPM ("CPF9898", MESSAGE_QCPFMSG , message->String , message->Length , MESSAGE_INFO , 
             "iv_joblog" , stackcount, msgkey , &apierr, 9, "*NONE     *NONE     ", -1);
   if (apierr.avail) {
      printf ("Api error: %7s - %s" ,apierr.msgid, apierr.msgdta);
   }
}

