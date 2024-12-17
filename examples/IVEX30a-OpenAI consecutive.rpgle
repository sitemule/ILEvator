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

dcl-s nouns varchar(50) dim(25) ctdata;

dcl-proc main;
    dcl-s i int(5);
    dcl-s httpClient pointer;
    dcl-s errorMessage varchar(256); 
    dcl-s baseUrl varchar(100) inz('https://api.openai.com/v1/chat/completions');

    dcl-s token varchar(4096);
    dcl-s inbuffer varchar(4096:2) ccsid(1208); 
    dcl-s outbuffer varchar(4096:2) ccsid(1208); 

    token = 'YourTokenGoesHere';

    httpClient = iv_newHttpClient(); 
    iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');
    iv_setCommTrace(httpClient : '/tmp/ivex30a-commtrace.txt');
    iv_addHeader ( httpClient: 'authorization' : 'Bearer ' + token);
    iv_addheader ( httpClient: 'content-type'  : 'application/json;charset=utf-8');

    if (not iv_connect(httpClient : baseUrl));
        dsply 'Could not connect to server';
        return;
    endif;
    
    for i = 1 to %elem(nouns);
        // Note: You need to set the response buffer before each call.
        inBuffer = '{"model":"gpt-4o","temperature":0.0,"messages":[{"role":"user","content":'
               + '"Oversæt: ' + %trim(nouns(i)) + ' til dansk og returner kun oversættelsen."}]}';
        
        iv_setRequestDataBuffer (httpClient : %addr(inbuffer)  : %size(inbuffer)  : IV_VARCHAR2 :IV_CCSID_UTF8 );
        iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR2 :IV_CCSID_UTF8 );
        iv_http_request(httpClient : 'POST' : baseUrl );
        
        if iv_getStatus(httpClient) <> IV_HTTP_OK;
            iv_joblog('My request failed. ' + iv_getErrorMessage(httpClient));
        else;
            iv_joblog(outbuffer);
        endif;
    endfor;
    
    on-exit;
        iv_free(httpClient);
end-proc;


**CTDATA nouns
apple
ball
cat
dog
elephant
flower
garden
house
island
jacket
kite
lamp
mountain
notebook
ocean
pencil
queen
river
stone
tree
umbrella
village
window
xylophone
yacht