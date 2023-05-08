**FREE
///
// Simple get a resource at google
///
    
ctl-opt copyright('Sitemule.com  (C), 2022-2023');
ctl-opt decEdit('0,') datEdit(*YMD.) ;
ctl-opt debug(*yes) bndDir('ILEVATOR');
ctl-opt main(main);


/include ./headers/ILEvator.rpgle


// -----------------------------------------------------------------------------
// Program Entry Point
// -----------------------------------------------------------------------------     
dcl-proc main;

    // example1();
    // example2();
    // example3();
    example4();
    
end-proc;
// -----------------------------------------------------------------------------
// Simple get 
// -----------------------------------------------------------------------------     
dcl-proc example1;

    dcl-s pHttp pointer; 
    dcl-s outbuf varchar(65000:4) ccsid(1208); 

    pHttp = iv_newHttpClient(); 
    iv_setResponseDataBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_VARCHAR4 : IV_CCSID_UTF8);
    iv_execute (pHttp : 'GET' : 'http://google.com' : 3000); 
    if iv_getStatus(pHttp) <> IV_HTTP_OK; 
        // text = iv_getMessage (pHttp);
        // iv_joblog ('My request failed ' + text);
    else;
        iv_joblog (%subst ( iv_xlateLvc(outbuf: 1252: 0): 1: 256));
    endif;

    return; // Remember to use return - otherwise the on-exit will not be called

on-exit;
    iv_delete(pHttp); 
end-proc;
// -----------------------------------------------------------------------------
// Simple get using https. Cert full defined
// -----------------------------------------------------------------------------     
dcl-proc example2;

    dcl-s pHttp  pointer; 
    dcl-s outbuf varchar(65000:4) ccsid(1208); 

    pHttp = iv_newHttpClient(); 
    iv_setResponseDataBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_VARCHAR4 : IV_CCSID_UTF8);
    iv_setCertificate (pHttp : '/prj/ILEvator/ilevator.kdb':'ilevator');
    iv_execute (pHttp : 'GET' : 'https://google.com' : 3000); 
    if iv_getStatus(pHttp) <> IV_HTTP_OK ; 
        // text = iv_getMessage (pHttp);
        // iv_joblog ('My request failed ' + text);
    else;
        iv_joblog (%subst ( iv_xlateLvc(outbuf: 1252: 0): 1: 256));
    endif;

    return; // Remember to use return - otherwise the on-exit will not be called

on-exit;
    iv_delete(pHttp); 
end-proc;
// -----------------------------------------------------------------------------
// Simple get using https, usages to the cerfiles - 
// no password required, if *PUBLIC *USE 
// -----------------------------------------------------------------------------     
dcl-proc example3;

    dcl-s pHttp  pointer; 
    dcl-s outbuf varchar(65000:4) ccsid(1208); 

    pHttp = iv_newHttpClient(); 
    iv_setResponseDataBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_VARCHAR4 : IV_CCSID_UTF8);
    iv_setCertificate (pHttp : '/prj/ILEvator/ilevator.kdb');
    iv_execute (pHttp : 'GET' : 'https://google.com' : 3000); 
    if iv_getStatus(pHttp) <> IV_HTTP_OK ; 
        // text = iv_getMessage (pHttp);
        // iv_joblog ('My request failed ' + text);
    else;
        iv_joblog (%subst ( iv_xlateLvc(outbuf: 1252: 0): 1: 256));
    endif;

    return; // Remember to use return - otherwise the on-exit will not be called

on-exit;
    iv_delete(pHttp); 
end-proc;
// -----------------------------------------------------------------------------
// 'get' using http. Write output data to response file
//  ccsid default to the resource on the net
// -----------------------------------------------------------------------------     
dcl-proc example4;

    dcl-s pHttp  pointer; 

    pHttp = iv_newHttpClient(); 
    iv_setResponseFile (pHttp : '/tmp/flowrate.txt' );
    iv_execute (pHttp : 'GET' : 'http://www.floatrates.com/daily/dkk.json' : 3000); 
    if iv_getStatus(pHttp) <> IV_HTTP_OK ; 
        // text = iv_getMessage (pHttp);
        // iv_joblog ('My request failed ' + text);
    endif;

    return; // Remember to use return - otherwise the on-exit will not be called

on-exit;
    iv_delete(pHttp); 
end-proc;
