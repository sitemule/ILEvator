**FREE

///
// ILEvator Example : Set automatic retries
//
// This example does a GET request. Not every web service is very stable and 
// some may fail sporadically or may need some time to spin up. So to 
// compensate for the web service to need to always readily available the 
// client needs to retry some times in case of an unsuccessful request.
// iv_setRetries sets the number of retries for the passed HTTP client instance.
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
    
    iv_setRetries(httpClient : 5);
    
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
