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
    dcl-s data varchar(1000:4) ccsid(819);
    dcl-s outbuffer varchar(65000:4) ccsid(1208); 
    dcl-s errorMessage varchar(256);
    dcl-s multipart pointer;
    
    multipart = iv_multipart_new(IV_MULTIPART_MEDIA_TYPE);
    iv_multipart_addPartFromString(multipart : 'user' : 'Mihael');
    iv_multipart_addPartFromFile(multipart : 'avatar' : 'integrationtests/resources/avatar.jpg');
    iv_multipart_finalize(multipart);

    httpClient = iv_newHttpClient();
    iv_setResponseDataBuffer(httpClient : %addr(outbuffer) : %size(outbuffer) : IV_VARCHAR4 : IV_CCSID_UTF8);
    iv_setRequestHandler(httpClient : multipart);
    
    iv_execute(httpClient : 'POST' : 'http://localhost:35801/api/multipart/file');
    if (iv_getStatus(httpClient) <> IV_HTTP_OK);
        errorMessage = iv_getErrorMessage(httpClient);
        iv_joblog('My request failed. ' + errorMessage);
    else;
        iv_joblog(outbuffer);
    endif;
    
    on-exit;
        iv_free(httpClient);
        iv_multipart_free(multipart);
end-proc;
