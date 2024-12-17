**FREE

///
// ILEvator Example : Talk to openAI 

// Additional output for debugging purposes can be written to the job log by 
// enabling the debug mode of ILEvator by setting the environment variable
// ILEVATOR_DEBUG to 1. This has to be done before the service program ILEVATOR
// is loaded. The debug mode is only evaluated once. If the service program is
// already started you cannot change the debug mode anymore.
//
// To enable debug mode just set the environment variable:
//
//     ADDENVVAR ILEVATOR_DEBUG 1
//
///

ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2024');

/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s i int(5);

    for i= 1 to 100;
        do_request();
    endfor;

end-proc;

dcl-proc do_request;

    dcl-s httpClient pointer; 
    dcl-s errorMessage varchar(256); 
    dcl-s token varchar(4096);
    dcl-s inbuffer varchar(4096:2) ccsid(1208); 
    dcl-s outbuffer varchar(4096:2) ccsid(1208); 

    token = 'PutInYourToken';
    
    inBuffer = '{"model":"gpt-4o","temperature":0.0,"messages":[{"role":"user","content":'
             + '"Oversæt: Collection til dansk og returner kun oversættelsen."}]}';

    httpClient = iv_newHttpClient(); 
    iv_setRequestDataBuffer (httpClient : %addr(inbuffer)  : %size(inbuffer) : IV_VARCHAR2 :IV_CCSID_UTF8 );
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR2 :IV_CCSID_UTF8 );
    iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');
    
    iv_setCommTrace(httpClient : '/tmp/ivex30-commtrace.txt');
    iv_addHeader ( httpClient: 'authorization' : 'Bearer ' + token);
    iv_addheader ( httpClient: 'content-type'  : 'application/json;charset=utf-8');
    
    iv_execute(httpClient : 'POST' : 'https://api.openai.com/v1/chat/completions'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog('reult:' + outbuffer);
    endif;

on-exit;
    iv_free(httpClient);
end-proc;
