**FREE

///
// ILEvator Example : Simple GET request with additional headers
//
// This example does a simple GET request to the local web server on the default
// port 80. A list of additional HTTP headers will be built and passed to the 
// call.
//
// The list of headers will be deleted at the end of the program, freeing up any
// allocated memory.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    dcl-s headers pointer;
    
    headers = iv_buildList(
        'accept-language' : 'dk,de;q=0.5' :
        'authentication' : 'Bearer my_token'
    );
    
    string = iv_get('http://localhost:80' : IV_MEDIA_TYPE_JSON : headers);

    iv_joblog(string);
    
    on-exit;
        iv_freeList(headers);
end-proc;
