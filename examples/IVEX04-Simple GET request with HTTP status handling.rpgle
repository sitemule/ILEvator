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

dcl-ds programStatus psds qualified;
  procedure char(10) ;             // Module or main procedure name
  status zoned(5) ;                // Status code
  previousStatus zoned(5) ;        // Previous status
  sourceLineNumber char(8) ;       // Source line number
  routine char(8) ;                // Name of the RPG routine
  parms zoned(3) ;                 // Number of parms passed to program
  exceptionType char(3) ;          // Exception type
  exceptionNumber zoned(4) ;       // Exception number
  exception char(7) samepos(ExceptionType) ;
end-ds;


dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE:4) ccsid(1208);
    
    monitor;
        string = iv_get('http://localhost:8983/select?' + iv_encode_query('q' : 'my search term'));

        iv_joblog(string);
    on-error;
        select;
	        when (programStatus.exceptionNumber = 400);
	            // the data passed to the web service was probably not ok
	            // or not what the web service was expecting
             
	        when (programStatus.exceptionNumber = 401);
	            // authentication data was missing or invalid
	        
	        when (programStatus.exceptionNumber = 403);
	            // authentication data was ok but not enough permissions
             
	        when (programStatus.exceptionNumber = 404);
                // the requested data is not available
                
            other;
                // here any not expected HTTP status code will be handled
                
        endsl;
    endmon;
end-proc;
