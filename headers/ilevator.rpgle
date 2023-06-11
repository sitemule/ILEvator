**FREE

/if defined (ILEVATOR)
/eof
/endif

/define ILEVATOR

///
// ILEvator - HTTP client for ILE on IBM i
//
// ILEvator is a HTTP client API for the ILE environment on
// IBM i for implementing web/microservices alike applications.
// <p>
// ILEvator is a service program that provides a simple, blazing fast
// programmable HTTP client for your application so you can easily plug your RPG
// code into a service infrastructure or make simple web applications without
// the need of any third party web client products.
// <p>
// The convenient procedures like <em>iv_get</em> will throw an escape message
// when the server responds with anything above and including HTTP status 400.
// The escape message id will contain the HTTP status code like ILV0404 for the
// HTTP status code 404. The message id can easily be retrieved from the program
// status data structure.
// <p>
// If the convenience procedures doesn't fit with the use case just use the more
// generic <code>iv_execute</code> procedure.
//
// @author Niels Liisberg
// @date 15.12.2022
// @project ILEvator
// @link https://github.com/sitemule/ILEvator Project page
// @version 1.0.0
//
// @info Source for HTTP status documentation is the Mozilla developer website
//       at https://developer.mozilla.org/en-US/docs/Web/HTTP/Status .
///


///
// Template for UTF8 varchars. default to 2M 
///
dcl-s IV_LONGUTF8VARCHAR varchar(2097152:4) ccsid(*utf8) template;

dcl-c IV_STATUS_OK 0;
dcl-c IV_STATUS_RETRY 1;
dcl-c IV_STATUS_ERROR 2;

///
// Maximum HTTP method size
///
dcl-c IV_METHOD_SIZE 10;
///
// Maximum URL size
///
dcl-c IV_URL_SIZE 32766;
///
// Default buffer size for input and output 
///
dcl-c IV_BUFFER_SIZE 1048576;
///
// Maximum HTTP header name/key size
///
dcl-c IV_HEADER_NAME_SIZE 1024;
///
// Maximum HTTP header value size
///
dcl-c IV_HEADER_VALUE_SIZE 16384;

///
// Protocol plain HTTP 
///
dcl-c IV_HTTP 0;

///
// Protocol HTTPS (Secure HTTP: Certificate and certificate password required)
///
dcl-c IV_HTTPS 1;


///
// The request has succeeded. The meaning of the success depends on the HTTP method: 
// <ul>
//   <li>GET: The resource has been fetched and is transmitted in the message body.</li>
//   <li>HEAD: The entity headers are in the message body.</li>
//   <li>PUT or POST: The resource describing the result of the action is transmitted in the message body.</li>
//   <li>TRACE: The message body contains the request message as received by the server.</li>
// </ul>
///
dcl-c IV_HTTP_OK 200;
///
// The request has succeeded and a new resource has been created as a result. 
// This is typically the response sent after POST requests, or some PUT requests.
///
dcl-c IV_HTTP_CREATED 201;
///
// The request has been received but not yet acted upon. It is noncommittal, 
// since there is no way in HTTP to later send an asynchronous response indicating 
// the outcome of the request. It is intended for cases where another process or 
// server handles the request, or for batch processing.
///
dcl-c IV_HTTP_ACCEPTED 202;
///
// This response code means the returned meta-information is not exactly the same 
// as is available from the origin server, but is collected from a local or a 
// third-party copy. This is mostly used for mirrors or backups of another resource. 
// Except for that specific case, the "200 OK" response is preferred to this status.
///
dcl-c IV_HTTP_NON_AUTHORITIVE_INFO 203;
///
// There is no content to send for this request, but the headers may be useful. 
// The user-agent may update its cached headers for this resource with the new ones.
///
dcl-c IV_HTTP_NO_CONTENT 204;
///
// Tells the user-agent to reset the document which sent this request.
///
dcl-c IV_HTTP_RESET_CONTENT 205;
///
// This response code is used when the Range header is sent from the client to 
// request only part of a resource.
///
dcl-c IV_HTTP_PARTIAL_CONTENT 206;
///
// Conveys information about multiple resources, for situations where multiple 
// status codes might be appropriate.
///
dcl-c IV_HTTP_MULTI_STATUS 207;
///
// Used inside a &lt;dav:propstat&gt; response element to avoid repeatedly enumerating 
// the internal members of multiple bindings to the same collection.
///
dcl-c IV_HTTP_ALREADY_REPORTED 208;
///
// The server has fulfilled a GET request for the resource, and the response is 
// a representation of the result of one or more instance-manipulations applied 
// to the current instance.
///
dcl-c IV_HTTP_IM_USED 226;
///
// The server could not understand the request due to invalid syntax.
///
dcl-c IV_HTTP_BAD_REQUEST 400;
///
// Although the HTTP standard specifies "unauthorized", semantically this response 
// means "unauthenticated". That is, the client must authenticate itself to get 
// the requested response.
///
dcl-c IV_HTTP_UNAUTHORIZED 401;
///
// This response code is reserved for future use. The initial aim for creating 
// this code was using it for digital payment systems, however this status code 
// is used very rarely and no standard convention exists.
///
dcl-c IV_HTTP_PAYMENT_REQUIRED 402;
///
// The client does not have access rights to the content; that is, it is 
// unauthorized, so the server is refusing to give the requested resource. 
// Unlike 401, the client's identity is known to the server.
///
dcl-c IV_HTTP_FORBIDDEN 403;
///
// The server can not find the requested resource. In the browser, this means 
// the URL is not recognized. In an API, this can also mean that the endpoint is 
// valid but the resource itself does not exist. Servers may also send this 
// response instead of 403 to hide the existence of a resource from an 
// unauthorized client. This response code is probably the most famous one due 
// to its frequent occurrence on the web.
///
dcl-c IV_HTTP_NOT_FOUND 404;
///
// The request method is known by the server but has been disabled and cannot be 
// used. For example, an API may forbid DELETE-ing a resource. The two mandatory 
// methods, GET and HEAD, must never be disabled and should not return this error 
// code.
///
dcl-c IV_HTTP_METHOD_NOT_ALLOWED 405;
///
// This response is sent when the web server, after performing server-driven 
// content negotiation, doesn't find any content that conforms to the criteria 
// given by the user agent.
///
dcl-c IV_HTTP_NOT_ACCEPTABLE 406;
///
// This is similar to 401 but authentication is needed to be done by a proxy.
///
dcl-c IV_HTTP_PROXY_AUTH_REQUIRED 407;
///
// This response is sent on an idle connection by some servers, even without any 
// previous request by the client. It means that the server would like to shut 
// down this unused connection. This response is used much more since some 
// browsers, like Chrome, Firefox 27+, or IE9, use HTTP pre-connection mechanisms 
// to speed up surfing. Also note that some servers merely shut down the 
// connection without sending this message.
///
dcl-c IV_HTTP_REQUEST_TIMEOUT 408;
///
// This response is sent when a request conflicts with the current state of the server.
///
dcl-c IV_HTTP_CONFLICT 409;
///
// This response is sent when the requested content has been permanently deleted 
// from server, with no forwarding address. Clients are expected to remove their 
// caches and links to the resource. The HTTP specification intends this status 
// code to be used for "limited-time, promotional services". APIs should not feel 
// compelled to indicate resources that have been deleted with this status code.
///
dcl-c IV_HTTP_GONE 410;
///
// Server rejected the request because the Content-Length header field is not 
// defined and the server requires it.
///
dcl-c IV_HTTP_LENGTH_REQUIRED 411;
///
// The client has indicated preconditions in its headers which the server does not meet.
///
dcl-c IV_HTTP_PRECONDITION_FAILED 412;
///
// Request entity is larger than limits defined by server; the server might 
// close the connection or return an Retry-After header field.
///
dcl-c IV_HTTP_PAYLOAD_TOO_LARGE 413;
///
// The URI requested by the client is longer than the server is willing to interpret.
///
dcl-c IV_HTTP_REQUEST_URI_TOO_LONG 414;
///
// The media format of the requested data is not supported by the server, so the 
// server is rejecting the request.
///
dcl-c IV_HTTP_UNSUPPORTED_MEDIA_TYPE 415;
///
// The range specified by the Range header field in the request can't be 
// fulfilled; it's possible that the range is outside the size of the target 
// URI's data.
///
dcl-c IV_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE 416;
///
// This response code means the expectation indicated by the Expect request 
// header field can't be met by the server.
///
dcl-c IV_HTTP_EXPECTATION_FAILED 417;
///
// The server refuses the attempt to brew coffee with a teapot.
///
dcl-c IV_HTTP_TEAPOT 418;
///
// The request was directed at a server that is not able to produce a response. 
// This can be sent by a server that is not configured to produce responses for 
// the combination of scheme and authority that are included in the request URI.
///
dcl-c IV_HTTP_MISDIRECTED_REQUEST 421;
///
// The request was well-formed but was unable to be followed due to semantic errors.
///
dcl-c IV_HTTP_UNPROCESSABLE_ENTITY 422;
///
// The resource that is being accessed is locked.
///
dcl-c IV_HTTP_LOCKED 423;
///
// The request failed due to failure of a previous request.
///
dcl-c IV_HTTP_FAILED_DEPENDENCY 424;
///
// Indicates that the server is unwilling to risk processing a request that 
// might be replayed.
///
dcl-c IV_HTTP_TOO_EARLY 425;
///
// The server refuses to perform the request using the current protocol but 
// might be willing to do so after the client upgrades to a different protocol. 
// The server sends an Upgrade header in a 426 response to indicate the required 
// protocol(s).
///
dcl-c IV_HTTP_UPGRADE_REQUIRED 426;
///
// The origin server requires the request to be conditional. This response is 
// intended to prevent the 'lost update' problem, where a client GETs a 
// resource's state, modifies it, and PUTs it back to the server, when meanwhile 
// a third party has modified the state on the server, leading to a conflict.
///
dcl-c IV_HTTP_PRECONDITION_REQUIRED 428;
///
// The user has sent too many requests in a given amount of time ("rate limiting").
///
dcl-c IV_HTTP_TOO_MANY_REQUESTS 429;
///
// The server is unwilling to process the request because its header fields are 
// too large. The request may be resubmitted after reducing the size of the 
// request header fields.
///
dcl-c IV_HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE 431;
///
// The user-agent requested a resource that cannot legally be provided, such as 
// a web page censored by a government.
///
dcl-c IV_HTTP_UNAVAILABLE_FOR_LEGAL_REASONS 451;
///
// The server has encountered a situation it doesn't know how to handle.
///
dcl-c IV_HTTP_INTERNAL_SERVER_ERROR 500;
///
// The request method is not supported by the server and cannot be handled. 
// The only methods that servers are required to support (and therefore that 
// must not return this code) are GET and HEAD.
///
dcl-c IV_HTTP_NOT_IMPLEMENTED 501;
///
// This error response means that the server, while working as a gateway to get 
// a response needed to handle the request, got an invalid response.
///
dcl-c IV_HTTP_BAD_GATEWAY 502;
///
// The server is not ready to handle the request. Common causes are a server 
// that is down for maintenance or that is overloaded. Note that together with 
// this response, a user-friendly page explaining the problem should be sent. 
// This responses should be used for temporary conditions and the Retry-After: 
// HTTP header should, if possible, contain the estimated time before the 
// recovery of the service. The webmaster must also take care about the 
// caching-related headers that are sent along with this response, as these 
// temporary condition responses should usually not be cached.
///
dcl-c IV_HTTP_SERVICE_UNAVAILABLE 503;
///
// This error response is given when the server is acting as a gateway and 
// cannot get a response in time.
///
dcl-c IV_HTTP_GATEWAY_TIMEOUT 504;
///
// The HTTP version used in the request is not supported by the server.
///
dcl-c IV_HTTP_VERSION_NOT_SUPPORTED 505;
///
// The server has an internal configuration error: the chosen variant resource 
// is configured to engage in transparent content negotiation itself, and is 
// therefore not a proper end point in the negotiation process.
///
dcl-c IV_HTTP_VARIANT_ALSO_NEGOTIATES 506;
///
// The method could not be performed on the resource because the server is 
// unable to store the representation needed to successfully complete the request.
///
dcl-c IV_HTTP_INSUFFICIENT_STORAGE 507;
///
// The server detected an infinite loop while processing the request.
///
dcl-c IV_HTTP_LOOP_DETECTED 508;
///
// Further extensions to the request are required for the server to fulfil it.
///
dcl-c IV_HTTP_NOT_EXTENDED 510;
///
// The 511 status code indicates that the client needs to authenticate to gain 
// network access.
///
dcl-c IV_HTTP_NETWORK_AUTH_REQUIRED 511;


///
// Any media type.
///
dcl-c IV_MEDIA_TYPE_ALL '*/*';
///
// Default format for JSON data.
///
dcl-c IV_MEDIA_TYPE_JSON 'application/json';
///
// Default format for XML data.
///
dcl-c IV_MEDIA_TYPE_XML 'application/xml';


///
// Create a new HTTP client 
//
// Returns a pointer to the new HTTP client instance.
//
// @return Pointer to the HTTP client
//
// @info The memory allocated by this client instance must be freed by 
//       calling the procedure <code>iv_free</code>. 
//
///
dcl-pr iv_newHttpClient pointer extproc(*dclcase) end-pr;

///
// Free client memory
//
// Disconnect and cleanup memory of the passed client instance.
//
// @param Pointer to the HTTP client 
//
///
dcl-pr iv_free extproc(*dclcase);
    client pointer value;
end-pr;

///
// CCSID of the current job
///
dcl-c IV_CCSID_JOB 0;
///
// CCSID UTF-8
///
dcl-c IV_CCSID_UTF8 1208;
///
// CCSID Windows-1252 (Latin 1)
///
dcl-c IV_CCSID_WIN1252 1252;
///
// CCSID special value indicating hex data and it will not be converted.
///
dcl-c IV_CCSID_BINARY 65535;


///
// Set request buffer
//
// Sets the buffer of the data which will be sent as the message body of the 
// HTTP request.
//
// @param Pointer to the HTTP client 
// @param Pointer to buffer
// @param Size of buffer (bytes in total)
// @param Buffer type (0=Byte buffer, 1=varchar2, 2=varchar4)
// @param CCSID (0=Current job, 65535=no xlate, 1208=UTF-8)
///
dcl-pr iv_setRequestDataBuffer extproc(*dclcase);
    client        pointer value;
    buffer        pointer value;
    bufferSize    int(10) value;
    bufferType    int(5) value;
    bufferCcsid   int(10) value;
end-pr;

///
// Set response header buffer
//
// Sets the buffer where the response header is received.
//
// @param Pointer to the HTTP client 
// @param Pointer to buffer
// @param Size of buffer ( bytes in total)
// @param Buffer type (0=Byte buffer, 1=varchar2, 2=varchar4)
// @param CCSID (0=Current job, 65535=no xlate, 1208=UTF-8)
///
dcl-pr iv_setResponseHeaderBuffer extproc(*dclcase);
    client        pointer value;
    buffer        pointer value;
    bufferSize    int(10) value;
    bufferType    int(5) value;
    bufferCcsid   int(10) value;
end-pr;

///
// Set response data buffer
//
// Sets the buffer where the response data (message body) is received.
//
// @param Pointer to the HTTP client 
// @param Pointer to buffer
// @param Size of buffer ( bytes in total)
// @param Buffer type (0=Byte buffer, 1=varchar2, 2=varchar4)
// @param CCSID (0=Current job, 65535=no xlate, 1208=UTF-8)
///
dcl-pr iv_setResponseDataBuffer extproc(*dclcase);
    pClient       pointer value;
    pBuffer       pointer value;
    bufferSize    int(10) value;
    bufferType    int(5) value;
    bufferCcsid   int(10) value;
end-pr;

///
// Set response output file
//
// Sets the output file for the response.
//
// @param Pointer to the HTTP client 
// @param IFS file name
// @param CCSID of the file (default: 1252)
///
dcl-pr iv_setResponseFile extproc(*dclcase);
    client        pointer value;
    fileName      pointer options(*string) value;
    fileCcsid     int(10)  options(*nopass) value;
end-pr;

///
// Execute HTTP request
//
// Executes the HTTP request with the passed client.
//
// @param Pointer to the HTTP client 
// @param HTTP method
// @param URL
// @return <code>*ON</code> if ok
///
dcl-pr iv_execute ind extproc(*dclcase);
    client  pointer value;
    method  pointer options(*string) value;
    url     pointer options(*string) value;
end-pr;

///
// Set SSL keystore
//
// Sets the SSL keystore used when making HTTPS requests.
//
// @param Pointer to the HTTP client 
// @param IFS path to the keystore file
// @param Keystore password (default: empty password)
// @return <code>*ON</code> if ok 
///
dcl-pr iv_setCertificate ind extproc(*dclcase);
    client              pointer value;
    certificateFile     pointer options(*string) value;
    certificatePassword pointer options(*string:*nopass) value;
end-pr;

///
// Get HTTP status
//
// Returns the HTTP status code of the last request done with this HTTP client
// instance.
//
// @param Pointer to the HTTP client 
// @return HTTP status code
///
dcl-pr iv_getStatus int(5) extproc(*dclcase);
    client pointer value;
end-pr;

///
// Get error message
//
// Returns the error message from the last HTTP request.
//
// @param  Pointer to the HTTP client 
// @return Error message from the HTTP request
///
dcl-pr iv_getErrorMessage varchar(256) extproc(*dclcase);
    client pointer value;
end-pr;

///
// Set authentication provider
//
// Sets the passed authentication provider on the HTTP client.
//
// @param Pointer to the HTTP Client
// @param Authentication provider
///
dcl-pr iv_setAuthProvider extproc(*dclcase);
  client pointer value;
  authProvider pointer value;
end-pr;

///
// Base64 decode value
//
// Decodes a base64 encoded string.
//
// @param Encoded string
// @return Decoded string
//
// @info The character encoding for the original value is expected to be UTF-8.
///
dcl-pr iv_decodeBase64 varchar(2097152:4) ccsid(*utf8) extproc(*CWIDEN : 'iv_decodeBase64') rtnparm;
  string varchar(2097152:4) ccsid(*utf8) options(*varsize) const;
end-pr;

///
// Base64 encoding value
//
// Encodes a string into base64 .
//
// @param string to encode
// @return Encoded string
//
// @info The character encoding for the original value is expected to be UTF-8.
///
dcl-pr iv_encodeBase64 varchar(2097152:4) ccsid(*utf8) extproc(*CWIDEN : 'iv_encodeBase64') rtnparm;
  string varchar(2097152:4) ccsid(*utf8) options(*varsize) const;
end-pr;

///
// Add message to job log
//
// Convenience function: put message in joblog.
// Works like printf but with strings only like
// <br/><br/>
// <code>iv_joblog('This is %s a test' : 'super');</code>
//
// @param message string
///
dcl-pr iv_joblog extproc(*CWIDEN : 'iv_joblog') ;
  message varchar(512) options(*varsize)  const ;
end-pr;

///
// Set Comm Trace
//
// Enables the communication trace. The trace data will be written to the passed
// file.
// 
// @param Pointer to the HTTP client
// @param IFS path to the trace file
///
dcl-pr iv_setCommTrace extproc(*dclcase) ;
    client    pointer value;
    traceFile pointer options(*string) value;
end-pr;
    
///
// Convert varchar value
//
// Converts the value of a (small) varchar variable to another CCSID.
//
// @param Input and output string
// @param From CCSID
// @param To CCSID
// @return Converted value
///
dcl-pr iv_xlateVc varchar(32768:2) ccsid(65535) extproc(*CWIDEN : 'iv_xlateVc') rtnparm;
  inoutString varchar(32768:2) ccsid(65535) options(*varsize) const;
  fromCCSID int(10) value;
  toCCSID   int(10) value;
end-pr;

///
// Convert long varchar value
//
// Converts the value of a (long) varchar variable to another CCSID.
//
// @param Input and output string
// @param From CCSID
// @param To CCSID
// @return Converted value
///
dcl-pr iv_xlateLvc varchar(2097152:4) ccsid(65535) extproc(*CWIDEN : 'iv_xlateLvc') rtnparm;
    inoutString varchar(2097152:4) ccsid(65535) options(*varsize) const;
    fromCCSID int(10) value;
    toCCSID   int(10) value;
end-pr;

///
// Execute GET HTTP request
//
// Executes a GET HTTP request to the passed URL.
//
// @param URL
// @param MIME type for the <code>Accept</code> HTTP header
// @param Pointer to a simple list with additional HTTP headers
// @return Response message body
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_get varchar(IV_BUFFER_SIZE:4) ccsid(1208) extproc(*dclcase) rtnparm;
    url varchar(IV_URL_SIZE:2) value;
    acceptMimeType varchar(IV_HEADER_VALUE_SIZE:2) ccsid(1208) value options(*nopass);
    headers pointer value options(*nopass);
end-pr;

///
// Execute DELETE HTTP request
//
// Executes a DELETE HTTP request to the passed URL.
//
// @param URL
// @param Pointer to a simple list with additional HTTP headers
// @return Response message body
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_delete varchar(IV_BUFFER_SIZE:4) ccsid(1208) extproc(*dclcase) rtnparm;
    url varchar(IV_URL_SIZE:2) value;
    headers pointer value options(*nopass);
end-pr;

///
// Execute HEAD HTTP request
//
// Executes a HEAD HTTP request to the passed URL.
//
// @param URL
// @param Pointer to a simple list with additional HTTP headers
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_head extproc(*dclcase);
    url varchar(IV_URL_SIZE:2) value;
    headers pointer value options(*nopass);
end-pr;

///
// Execute OPTIONS HTTP request
//
// Executes a OPTIONS HTTP request to the passed URL.
//
// @param URL
// @param MIME type for the <code>Accept</code> HTTP header
// @param Pointer to a simple list with additional HTTP headers
// @return Response message body
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_options varchar(IV_BUFFER_SIZE:4) ccsid(1208) extproc(*dclcase) rtnparm;
    url varchar(IV_URL_SIZE:2) value;
    acceptMimeType varchar(IV_HEADER_VALUE_SIZE:2) ccsid(1208) value options(*nopass);
    headers pointer value options(*nopass);
end-pr;

///
// Execute PATCH HTTP request
//
// Executes a PATCH HTTP request to the passed URL.
//
// @param URL
// @param Request message body
// @param MIME type for the <code>Accept</code> HTTP header
// @param Pointer to a simple list with additional HTTP headers
// @return Response message body
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_patch varchar(IV_BUFFER_SIZE:4) ccsid(1208) extproc(*dclcase) rtnparm;
    url varchar(IV_URL_SIZE:2) value;
    messageBody pointer value options(*string);
    acceptMimeType varchar(IV_HEADER_VALUE_SIZE:2) ccsid(1208) value options(*nopass);
    headers pointer value options(*nopass);
end-pr;

///
// Execute POST HTTP request
//
// Executes a POST HTTP request to the passed URL.
//
// @param URL
// @param Request message body
// @param MIME type for the <code>Accept</code> HTTP header
// @param Pointer to a simple list with additional HTTP headers
// @return Response message body
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_post varchar(IV_BUFFER_SIZE:4) ccsid(1208) extproc(*dclcase) rtnparm;
    url varchar(IV_URL_SIZE:2) value;
    messageBody pointer value options(*string);
    acceptMimeType varchar(IV_HEADER_VALUE_SIZE:2) ccsid(1208) value options(*nopass);
    headers pointer value options(*nopass);
end-pr;

///
// Execute PUT HTTP request
//
// Executes a PUT HTTP request to the passed URL.
//
// @param URL
// @param Request message body
// @param MIME type for the <code>Accept</code> HTTP header
// @param Pointer to a simple list with additional HTTP headers
// @return Response message body
// 
// @throws Escape message on an unsuccessful request with the HTTP status encoded 
//         like ILV0404 for a 404 HTTP status.
///
dcl-pr iv_put varchar(IV_BUFFER_SIZE:4) ccsid(1208) extproc(*dclcase) rtnparm;
    url varchar(IV_URL_SIZE:2) value;
    messageBody pointer value options(*string);
    acceptMimeType varchar(IV_HEADER_VALUE_SIZE:2) ccsid(1208) value options(*nopass);
    headers pointer value options(*nopass);
end-pr;

///
// Added HTTP header
//
// Adds the passed HTTP header (key/value) to the HTTP client instance. This 
// HTTP header will be added to each request done with this client instance.
// <p>
// Duplicate HTTP header keys are allowed in an HTTP message. Generally the last
// one "wins".
//
// @param Pointer to the HTTP client
// @param HTTP header name (key)
// @param HTTP header value
///
dcl-pr iv_addHeader extproc(*dclcase);
    client pointer value;
    headerName varchar(IV_HEADER_NAME_SIZE:2) ccsid(*utf8) value;
    headerValue varchar(IV_HEADER_VALUE_SIZE:2) ccsid(*utf8) value;
end-pr;

///
// Add list of HTTP headers
//
// Adds a list of HTTP headers (key/value pairs) to the HTTP client instance. 
// These HTTP headers will be added to each request done with this client instance.
// <p>
// Duplicate HTTP header keys are allowed in an HTTP message. Generally the last
// one "wins".
//
// @param Pointer to the HTTP client
// @param Simple list of HTTP headers
///
dcl-pr iv_addHeaders extproc(*dclcase);
    client pointer value;
    headers pointer value;
end-pr;

///
// Set timeout
//
// Sets the timeout for the client instance.
//
// @param Pointer to the HTTP client
// @param Timeout in seconds
///
dcl-pr iv_setTimeout extproc(*dclcase);
    client pointer value;
    timeout int(5) value;
end-pr;

///
// Set Retries
//
// Sets the number of retries the client does on an unsuccessful request.
//
// @param Pointer to the HTTP client
// @param Number of retries
///
dcl-pr iv_setRetries extproc(*dclcase);
    client pointer value;
    retries int(5) value;
end-pr;
