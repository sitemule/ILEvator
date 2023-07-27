**FREE

///
// ILEvator Example : Enable debug output
//
// This example does a simple GET request to the local web server on the default
// port 80.
// 
// Additional output for debugging purposes can be written to the job log by 
// enabling the debug mode of ILEvator by setting the environment variable
// ILEVATOR_DEBUG to 1. This has to be done before the service program ILEVATOR
// is loaded. The debug mode is only evaluated once. If the service program is
// already started you cannot change the debug mode anymore.
//
// To enable debug mode just set the environment variable:
//
//     ADDENVVAR ILEVATOR_DEBUG 1
//
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    
    string = iv_get('http://localhost:35801/api/hello');

    iv_joblog(string);
end-proc;
