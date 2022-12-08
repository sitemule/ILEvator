**FREE
///
// Simple get a resource at google
///
    
ctl-opt copyright('Sitemule.com  (C), 2022');
ctl-opt decEdit('0,') datEdit(*YMD.) ;
ctl-opt debug(*yes) bndDir('ILEVATOR');
ctl-opt main(main);


/include ./headers/ILEvator.rpgle


// -----------------------------------------------------------------------------
// Program Entry Point
// -----------------------------------------------------------------------------     
dcl-proc main;

    dcl-s pHttp pointer; 
    dcl-s outbuf varchar(4:65000) ccsid(1208); 

    pHttp = iv_newClient(); 
    iv_setResponseBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_BUF_VARCHAR4 : IV_XLATE_UTF8);
    iv_do (pHttp : 'GET' : 'http://google.com' : 30000); 
    if iv_getStatus(pHttp) <> 200; 
        // text = iv_getMessage (pHttp);
        // iv_log (pHttp : 'My request failed ' + text);
    endif;


    // Yet top be implemented
    //pHttp = iv_newClient(); 
    //iv_setURL    (pHttp : 'http://google.com'); 
    //iv_setMethod (pHttp : 'GET'); 
    //iv_setTimOut (pHttp : 30000); 
    //iv_setUser   (pHttp : 'john'); 
    //iv_setPassword  (pHttp : 'mypassword'); 
    //iv_setResponseBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_LONGVARCHAR : IV_TO_UTF_8);
    //iv_setResponseFile (pHttp : '/tmp/mydata.txt' : iv_getResponseCcsid(pHttp);
    //iv_setLogFile (pHttp : '/tmp/ILEvator/log.txt');
    //iv_do (pHttp); 
    //if iv_getStatus(pHttp) <> 200; 
    //    text = iv_getMessage (pHttp);
    //    iv_log (pHttp : 'My request failed ' + text);
    //endif;

on-exit:  
    iv_delete(pHttp); 
end-proc;

