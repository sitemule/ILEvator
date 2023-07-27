**FREE

///
// ILEvator Example : Write response data to stream file
//
// This example does a GET request and writes the response data to the stream file
// /tmp/ivex08-response.txt.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s httpClient pointer; 
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseFile(httpClient : '/tmp/ivex08-response.txt');
    
    iv_setCertificate(httpClient : '/path/to/ilevator/ilevator.kdb' : 'ilevator');

    iv_execute(httpClient : 'GET' : 'https://google.com'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        dsply 'Look at /tmp/ivex08-response.txt';
    endif;
end-proc;
