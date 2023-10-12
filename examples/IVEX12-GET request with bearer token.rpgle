**FREE

///
// ILEvator Example : GET request with bearer token
//
// This example does a GET request. The HTTP header Authorization is added 
// by using the ILEvator authentication provider for bearer token, iv_bearer_new.
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
    
    iv_setAuthProvider(httpClient : iv_bearer_new('my_token'));
    
    iv_execute (httpClient : 'GET' : 'http://google.com');
    if iv_getStatus(httpClient) <> IV_HTTP_OK;
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(buffer);
    endif;

    on-exit;
        iv_free(httpClient);
end-proc;
