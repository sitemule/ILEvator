**FREE
///
// Simple get a resource at google
///
    
ctl-opt copyright('Sitemule.com  (C), 2022-2023');
ctl-opt decEdit('0,') datEdit(*YMD.) ;
ctl-opt debug(*yes) bndDir('ILEVATOR':'JSONXML');
ctl-opt main(main);


/include ./headers/ILEvator.rpgle
/copy noxdb/QRPGLEREF,JSONParser

// -----------------------------------------------------------------------------
// Program Entry Point
// -----------------------------------------------------------------------------     
dcl-proc main;

    example4();
    
end-proc;
// -----------------------------------------------------------------------------
// 'get' using http. Write output data to response file
//  ccsid default to the resource on the net
// -----------------------------------------------------------------------------     
dcl-proc example4;

    dcl-s pHttp  pointer; 
    dcl-s pflow  pointer; 
    dcl-s eur    packed(11:4);
    dcl-s outbuf varchar(65000:4);

    pHttp = iv_newHttpClient(); 
    // iv_setResponseFile (pHttp : '/tmp/flowrate.json' );
    
    iv_setResponseDataBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_VARCHAR4 : IV_CCSID_JOB);
    iv_execute (pHttp : 'GET' : 'http://www.floatrates.com/daily/dkk.json' : 3000); 
    if iv_getStatus(pHttp) = IV_HTTP_OK ; 
        pflow = json_parsestring(outbuf); 
        eur = json_getnum(pflow:'eur.inverseRate');
        json_joblog('eur' + %char(eur));
    endif;

    return; // Remember to use return - otherwise the on-exit will not be called

on-exit;
    iv_free(pHttp); 
    json_delete(pflow);
end-proc;
