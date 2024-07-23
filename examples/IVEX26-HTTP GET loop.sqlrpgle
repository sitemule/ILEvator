**FREE

///
// ILEvator Example : HTTPS loop over 1000 GET's to measure performance
//
// In SQL do:
//     create or replace table ilevator.trace ( 
//         now timestamp,
//         text varchar(32000)
//     );
// 
///

ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2024');
exec sql set option commit=*NONE;

/include 'ilevator.rpgle'
dcl-s outbuffer varchar(32000:2) ccsid(1208); 


dcl-proc main;
    dcl-s i int(5);

    for i= 1 to 1000;
        do_request();
    endfor;

end-proc;

dcl-proc do_request;

    dcl-s httpClient pointer; 
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR2 :IV_CCSID_UTF8 );
    iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');
    iv_setBlockingSockets (httpClient : *ON);
    
    iv_setCommTrace(httpClient : '/tmp/ivex26-commtrace.txt');

    
    iv_execute(httpClient : 'GET' : 'https://system-method.com'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        exec sql insert into ilevator.trace (now, text) values (
            now(),
            substring(:outbuffer , 1 , 32000)
        );
    endif;
    
on-exit;
    iv_free(httpClient);
end-proc;
