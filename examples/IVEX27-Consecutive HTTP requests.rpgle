**FREE

///
// ILEvator Example : Execute multiple consecutive HTTP requests
//
// This example executes multiple GET requests using the same socket connection.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'

dcl-s names varchar(50) dim(3) ctdata;

dcl-proc main;
    dcl-s httpClient pointer;
    dcl-s buffer varchar(65000:4) ccsid(1208);
    dcl-s errorMessage varchar(256); 
    dcl-s i int(10);
    dcl-s baseUrl varchar(100) inz('http://localhost:35801');
    
    httpClient = iv_newHttpClient();
    if (not iv_connect(httpClient : baseUrl));
        dsply 'Could not connect to server';
        return;
    endif;
    
    for i = 1 to %elem(names);
        // Note: You need to set the response buffer before each call.
        iv_setResponseDataBuffer (httpClient : %addr(buffer) : %size(buffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
        
        iv_http_request(httpClient : 'GET' : baseUrl + '/api/hellox?q=' + names(i));
        
        if iv_getStatus(httpClient) <> IV_HTTP_OK;
            iv_joblog('My request failed. ' + iv_getErrorMessage(httpClient));
        else;
            iv_joblog(buffer);
        endif;
    endfor;
    
    on-exit;
        iv_free(httpClient);
end-proc;


**CTDATA names
Niels
Jens
Mihael
