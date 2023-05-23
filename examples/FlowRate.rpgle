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
    iv_free(pHttp); 
end-proc;
