**FREE

///
// ILEvator Example : POST request with different charset
//
// This example does a POST request with data encoded with a different charset.
// Some Umlaute are sent as Latin 1 (CCSID 819).
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/define RPG_HAS_OVERLOAD
/include 'ilevator.rpgle'
/include 'form.rpginc'
/include 'varchar.rpginc'


dcl-proc main;
    dcl-s httpClient pointer; 
    dcl-s data varchar(1000:4) ccsid(819);
    dcl-s outbuffer varchar(65000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256); 
    
    data = 'This message contains some Umlaute like äöü and in upper case ÄÖÜ.';

    httpClient = iv_newHttpClient();
    iv_setRequestDataBuffer(httpClient : %addr(data) : %size(data) : IV_VARCHAR4 : 819);
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    
    // we should tell the server in what CCSID/charset our data is sent
    iv_addHeader(httpClient : 'Content-Type' : 'text/plain;charset=iso-8859-1');
    
    iv_execute(httpClient : 'POST' : 'http://localhost'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
    on-exit;
        iv_free(httpClient);
end-proc;
