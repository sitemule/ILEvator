**FREE

///
// ILEvator Example : Simple GET request with encoded query parameters
//
// This example does a simple GET request to the local web server on the default
// port 80. 
//
// A query parameter is added to the URL. The key and value of the query parameter
// are encoded if needed. In this example the key is a simple "q" and does not need
// to be encoded. The value of the query parameter has some spaces. Those spaces
// will be percent encoded as "%20".
///


ctl-opt dftactgrp(*no) actgrp(*new) debug(*yes) bnddir('ILEVATOR') main(main);
ctl-opt copyright('Sitemule.com  (C), 2022-2023');


/include 'ilevator.rpgle'


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    
    string = iv_get('http://localhost:80?' + iv_encode_query('q' : 'spaces should be encoded'));

    iv_joblog(string);
end-proc;
