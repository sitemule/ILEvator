

/* SYSIFCOPT(*IFSIO) TERASPACE(*YES *TSIFC) STGMDL(*INHERIT) */
/* ------------------------------------------------------------- */
/* Date  . . . . : 14.12.2005                                    */
/* Design  . . . : Niels Liisberg                                */
/* Function  . . : File utilies                                  */
/*                                                               */
/* By     Date       PTF     Description                         */
/* NL     14.12.2005         New program                         */
/* ------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Qp0lstdi.h>

#include "ostypes.h"

int fileSetCcsid ( PUCHAR fileName , LONG ccsid) 
{

    typedef struct _ATTR  {                  
        Qp0l_Attr_Header_t type;             
        ULONG  value;                        
    } ATTR, *PATTR;                          
                                         
    typedef struct _PATH  {                  
        Qlg_Path_Name_T  type;               
        char             name[256] ;         
    } PATH, *PPATH;                          
                                         
    int rc;                                  
    PATH path;                               
    ATTR attr;                               
                                            
    memset(&path  , 0 , sizeof(path));       
    strcpy(path.name , fileName);            
    path.type.CCSID = 37;                    
    memcpy(path.type.Country_ID,"US",2 );    
    memcpy(path.type.Language_ID,"ENU", 3);  
    path.type.Path_Type = 0;                      
    path.type.Path_Length = strlen(path.name);    
    path.type.Path_Name_Delimiter[0] = '/';       
                                                
    memset(&attr  , 0 , sizeof(attr));            
    attr.type.Attr_ID = QP0L_ATTR_CCSID;          
    attr.type.Attr_Size = sizeof(attr.value);     
    attr.value = ccsid;                           
                                                
    rc=Qp0lSetAttr(                               
        (Qlg_Path_Name_T  *) &path,                
        (PUCHAR) &attr,                            
        sizeof(attr),                              
        QP0L_FOLLOW_SYMLNK                         
    );                                            
    return rc;                                    
}
