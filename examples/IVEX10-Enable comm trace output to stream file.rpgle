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
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setCommTrace(httpClient : '/tmp/ivex10-commtrace.txt');
    
    iv_setCertificate(httpClient : '/path/to/ilevator/ilevator.kdb' : 'ilevator');

    iv_execute(httpClient : 'GET' : 'https://google.com'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        dsply 'Look at comm trace /tmp/ivex10-commtrace.txt';
    endif;
end-proc;
