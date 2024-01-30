**FREE

///
// ILEvator Example : using proxy 
///

// ADDENVVAR ENVVAR(ILEVATOR_TRACE_STMF) VALUE('/tmp/comtrace.txt')
// ADDENVVAR ENVVAR(ILEVATOR_DEBUG) VALUE('1')
// addlible ilevator 
// strdbg ivex18
// call  ivex18

ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');

/define RPG_HAS_OVERLOAD
/include 'ilevator.rpgle'

dcl-proc main;

    dcl-s httpClient pointer;
    dcl-s reqBuffer  varchar(65000:4) ;
    dcl-s rspBuffer  varchar(65000:4) ;
    
    reqBuffer = 'ABC∆ÿ≈';

    httpClient = iv_newHttpClient();
    // iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');
    iv_setCertificate(httpClient : '/home/jmc/mitm.kdb' : 'john' );
    

    iv_setProxyTunnel (httpClient : 'http://fwdprx.workmule.dk:3128');

    iv_setRequestDataBuffer(
        httpClient : 
        %addr(reqBuffer) : 
        %size(reqBuffer) : 
        IV_VARCHAR4 : 
        IV_CCSID_JOB
    );


    iv_setResponseDataBuffer(
        httpClient : 
        %addr(rspBuffer) : 
        %size(rspBuffer) : 
        IV_VARCHAR4 : 
        IV_CCSID_JOB
    );

    iv_execute (httpClient : 'POST' : 'https://google.com'); 

    if iv_getStatus(httpClient) <> IV_HTTP_OK ; 
        iv_joblog('Invalid status: ' + %char(iv_getStatus(httpClient)));
    endif;

    // iv_joblog(%char(reqBuffer));
    return;

    
    on-exit;
        iv_free(httpClient);
end-proc;
