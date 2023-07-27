**FREE

///
// ILEvator Example : Simple POST request with non default content type header
//
// This example does a simple POST request to the local web server on the default
// port 80. Some text is sent as the message body to the web service.
// The content type of the data passed to the web service is specified.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    
    string = iv_post('http://localhost:80' : 'Hello ILEvator!' : 'application/vnd.sitemule.ilevator.example');

    iv_joblog(string);
end-proc;
