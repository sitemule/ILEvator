**FREE
///
// ILEvator Example : POST request with IFS stream file
//
// To get the trace:
// ADDENVVAR ENVVAR(ILEVATOR_TRACE_STMF) VALUE('/tmp/comtrace.txt')
// ADDENVVAR ENVVAR(ILEVATOR_DEBUG) VALUE('1')
///
ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');

/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s httpClient pointer; 
    dcl-s outbuffer varchar(65000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256); 
    

    httpClient = iv_newHttpClient();
    iv_setRequestFile (httpClient : '/prj/ILEvator/examples/README.md');
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    
    // post it binary
    iv_addHeader(httpClient : 'Content-Type' : 'application/octet-stream');
    
    iv_execute(httpClient : 'POST' : 'http://localhost:44025'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog(%char(iv_getStatus(httpClient)) + ' My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
    on-exit;
        iv_free(httpClient);
end-proc;
