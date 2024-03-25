**FREE

///
// ILEvator Example : HTTP GET a JSON via a local ILEastic server on MY_IBM_I
//
///

ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2024');

/include 'ilevator.rpgle'

dcl-proc main;
    dcl-s httpClient pointer; 
    dcl-s outbuffer varchar(700000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    
    // iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');

    iv_execute(httpClient : 'GET' : 'http://MY_IBM_I:44012/big.json'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
on-exit;
    iv_free(httpClient);
end-proc;
