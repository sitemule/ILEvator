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

    # Set ADDENVVAR ILEVATOR_DEBUG '1' before running, to view transaction.
    iv_configureTlsVersion(httpClient : IV_SSL_3  : *off);
    iv_configureTlsVersion(httpClient : IV_TLS_10 : *off);
    iv_configureTlsVersion(httpClient : IV_TLS_11 : *off);
    iv_configureTlsVersion(httpClient : IV_TLS_12 : *on);
    iv_configureTlsVersion(httpClient : IV_TLS_13 : *on);

    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    
    iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');

    iv_execute(httpClient : 'GET' : 'https://google.com'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
    on-exit;
        iv_free(httpClient);
end-proc;
