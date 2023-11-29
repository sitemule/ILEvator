**FREE

///
// ILEvator Example : Multipart form data
//
// This example does a POST request with multipart form data as the message body.
// The first part of the multipart message is a text part. The second part is a 
// file.
//
// This example can be used as a template for uploading a file to a web service
// which accepts the media type multipart/form-data.
//
// When using the multipart module be aware that you have to free the memory of
// the multipart request handler by calling iv_multipart_free(multipart).
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');

/define RPG_HAS_OVERLOAD
/include 'ilevator.rpgle'

dcl-proc main;

    dcl-s httpClient pointer;
    dcl-s buffer varchar(65000:4) ccsid(1208);
    
    httpClient = iv_newHttpClient();
    iv_setCertificate(httpClient : '/prj/ilevator/ilevator.kdb' : 'ilevator');
    // iv_setCertificate(httpClient : '/tmp/proxy.pem' );
    

    iv_setProxyTunnel (httpClient : 'http://fwdprx.workmule.dk:3128');

    iv_setResponseDataBuffer(
        httpClient : 
        %addr(buffer) : 
        %size(buffer) : 
        IV_VARCHAR4 : 
        IV_CCSID_UTF8
    );
    
    iv_execute (httpClient : 'GET' : 'https://google.com'); 

    if iv_getStatus(httpClient) <> IV_HTTP_OK ; 
        iv_joblog('Invalid status: ' + %char(iv_getStatus(httpClient)));
    endif;

    iv_joblog(%char(buffer));
    return;

    
    on-exit;
        iv_free(httpClient);
end-proc;
