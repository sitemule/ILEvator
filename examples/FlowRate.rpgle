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

    example1();
    
end-proc;
// -----------------------------------------------------------------------------
//  Example #1 - Fetch currency exhange rate from www.floatrates.com
//  ----------------------------------------------------------------------------
//  1: Fetch from data (3000 bytes)
//  2: Fetch EUR 'InverseRate'
//  3: Write Result to JobLog - dspjob + F10 + F18
// -----------------------------------------------------------------------------     
dcl-proc example1;

    dcl-s pHttp  pointer; 
    dcl-s pflow  pointer; 
    dcl-s eur    packed(11:5);
    dcl-s outbuf varchar(65000:4);

    pHttp = iv_newHttpClient(); 
    
    iv_setResponseDataBuffer (pHttp : %addr(outbuf) : %size(outbuf) : IV_VARCHAR4 : IV_CCSID_JOB);
    iv_execute (pHttp : 'GET' : 'http://www.floatrates.com/daily/dkk.json' : 3000); 
    if iv_getStatus(pHttp) = IV_HTTP_OK ; 
        pflow = json_parsestring(outbuf); 
        eur = json_getnum(pflow:'eur.inverseRate');
        json_joblog('DKK to EUR: ' + %char(eur));
    endif;

on-exit;
    iv_free(pHttp); 
    json_delete(pflow);
end-proc;
