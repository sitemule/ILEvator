**FREE

///
// ILEvator Example : HTTPS GET request
//
// This example does a GET request to Google.
// 
// The certificate store (key store) needs to be set before making any HTTP requests.
// No password needs to be specified for the certificate store if the permissions 
// *PUBLIC *USE are set.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s httpClient pointer; 
    dcl-s outbuffer varchar(65000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    
    iv_setCertificate(httpClient : '/path/to/ilevator/ilevator.kdb' : 'ilevator');

    iv_execute(httpClient : 'GET' : 'https://google.com'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
end-proc;
