**FREE

///
// ILEvator Example : Enable comm trace output to stream file
//
// This example does a GET request and writes a communication trace to the stream file
// /tmp/ivex10-commtrace.txt.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s httpClient pointer;
    dcl-s buffer varchar(65000:4) ccsid(1208);
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseDataBuffer (httpClient : %addr(buffer) : %size(buffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    iv_setCertificate(httpClient : '/path/to/ilevator/ilevator.kdb' : 'ilevator');
    
    iv_setCommTrace(httpClient : '/tmp/ivex10-commtrace.txt');
    
    iv_execute(httpClient : 'GET' : 'https://google.com'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        dsply 'Look at comm trace /tmp/ivex10-commtrace.txt';
    endif;
    
    on-exit;
        iv_free(httpClient);
end-proc;
