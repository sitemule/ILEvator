**FREE

///
// ILEvator Example : Simple GET request
//
// This example does a simple GET request to the local web server on the default
// port 80.
// 
// If no web server is running on port 80 an escape message will be thrown after
// reaching the default timeout of 30 seconds.
//
// By default the client will retry the request two times on an unsuccessful
// attempt.
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    
    string = iv_get('http://google.com');

    iv_joblog(string);
end-proc;
