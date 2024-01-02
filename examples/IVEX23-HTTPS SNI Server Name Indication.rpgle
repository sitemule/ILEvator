**FREE

///
// ILEvator Example : a client code that requestion SNI (Server Name Indication) 
//
// Under the covers, we configure TLS to negotiate SNI silently. 
// So this is transparent for you as a user. 
// 
///
ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2024');

/include 'ilevator.rpgle'

dcl-proc main;

    dcl-s httpClient pointer; 
    dcl-s outbuffer varchar(65000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);

    // Use system default certificate authority - you need at least *USE access to this file: 
    iv_setCertificate(httpClient : '/QIBM/USERDATA/ICSS/CERT/CERTAUTH/DEFAULT.KDB' );

    // SNI - This site requires SNI: Server Name Indication     
    iv_execute(httpClient : 'GET' : 'https://api.nemhandel.dk/v3-nhr-api/search/find?cvr=15965398'); 

    // Monitor for errors:
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
on-exit;
    iv_free(httpClient);

end-proc;
