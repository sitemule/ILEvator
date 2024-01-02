**FREE

///
// ILEvator Example : Simple GET request with HTTP status handling
//
// This example does a simple GET request to a web service. This fictional web 
// service can return various HTTP status codes depending on the passed data.
// Possible HTTP status codes: 200, 400, 401, 403, 404.
//
// If iv_execute is used the procedure iv_getStatus can be used to get the 
// HTTP status of the request.
//
// If any of the convenient procedures like iv_get or iv_post is used an escape
// message is sent to the caller on an unsuccessful HTTP request. HTTP status 
// on an unsuccessful request can be retrieved by examinig the escape message. 
// The HTTP status code is encoded in the message exception id, f. e. ILV0404 
// for HTTP status code 404 or ILV0415 for HTTP status code 415.
// 
// HTTP status codes can only be examined on an unsuccessful HTTP request
// (meaning HTTP status code above and including 400).
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;

    dcl-s httpClient pointer; 
    dcl-s outbuffer varchar(65000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256); 

    httpClient = iv_newHttpClient(); 
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    
    // iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');

    iv_setCertificate(httpClient : '/QIBM/USERDATA/ICSS/CERT/CERTAUTH/DEFAULT.KDB' );
    
    iv_execute(httpClient : 'GET' : 'https://api.nemhandel.dk/v3-nhr-api/search/find?cvr=15965398'); 
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
on-exit;
    iv_free(httpClient);

end-proc;
