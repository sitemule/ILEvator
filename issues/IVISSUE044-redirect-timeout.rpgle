**FREE
///
// Test issue #44
///
    
ctl-opt copyright('Sitemule.com  (C), 2022-2023');
ctl-opt decEdit('0,') datEdit(*YMD.) ;
ctl-opt debug(*yes) bndDir('ILEVATOR':'QC2LE');
ctl-opt main(main);

/include ./headers/ILEvator.rpgle
/include ./headers/clib.rpginc

// -----------------------------------------------------------------------------
//  Redirects might timeout?
// -----------------------------------------------------------------------------     
dcl-proc main;

    dcl-s pHttp  pointer; 
    dcl-s outbuf varchar(65000:4);

    system ('ADDENVVAR ENVVAR(ILEVATOR_TRACE_STMF) VALUE(''/tmp/issue044.txt'') REPLACE(*YES)');
    system ('ADDENVVAR ENVVAR(ILEVATOR_DEBUG) VALUE(''1'') REPLACE(*YES)');

    pHttp = iv_newHttpClient(); 
    
    iv_setResponseDataBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_VARCHAR4 : IV_CCSID_JOB);
    iv_execute (pHttp : 'GET' : 'http://api.nemhandel.dk/nemhandel-api/search/networkLookup?receiverId=0184%3A15965398'); 
    if iv_getStatus(pHttp) <> IV_HTTP_OK ; 
        iv_joblog('Failed');
    endif;

    return;

on-exit;
    iv_free(pHttp); 
end-proc;