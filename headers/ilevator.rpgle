**FREE

/if defined (ILEVATOR)
/eof
/endif

/define ILEVATOR

///
// ILEvator - HTTP client for ILE on IBM i
//
// It is a HTTP client API for the ILE environment on
// IBM i for implementing web/microservices alike applications.
// <p>
// ILEvator is a service program that provides a simple, blazing fast
// programmable HTTP client for your application so you can easily plug your RPG
// code into a service infrastructure or make simple web applications without
// the need of any third party web client products.
//
// @author Niels Liisberg
// @date 27.08.2018
// @project ILEvator
// @link https://github.com/sitemule/ILEvator Project page
// @version 1.1.3
//
// @info Source for HTTP status documentation is the Mozilla developer website
//       at https://developer.mozilla.org/en-US/docs/Web/HTTP/Status .
///


///
// Template for UTF8 varchars.
///
dcl-s IV_LONGUTF8VARCHAR varchar(524284:4) ccsid(*utf8) template;



///
// Protocol plain HTTP 
///
dcl-c IV_HTTP       0;

///
// Protocol HTTPS (Secure HTTP: Certificate and certificate password required)
///
dcl-c IV_HTTPS      1;


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
// HTTP request
//
// This data structure contains the values of the incoming
// HTTP request. The values can be retrieve by using the
// iv_getVarcharValue procedure or by using one of the
// iv_getRequest... procedures.
///
dcl-ds iv_request qualified template;
    config           pointer;
    method           likeds(iv_varchar);
    url              likeds(iv_varchar);
    resource         likeds(iv_varchar);
    queryString      likeds(iv_varchar);
    protocol         likeds(iv_varchar);
    headers          likeds(iv_varchar);
    content          likeds(iv_varchar);
    contentType      varchar(256);
    contentLength    uns(20);
    completeHeader   likeds(iv_varchar);
    headerList       pointer;
    parameterList    pointer;
    resourceSegments pointer;
    threadLocal      pointer;
    routing          pointer;
    routeId          varchar(256);
end-ds;


///
// create a new client 
//
// Returns pointer to the client
//
// @return pointer to the client
///
dcl-pr iv_newClient pointer  extproc(*CWIDEN:'iv_newClient');
end-pr;



///
// buffer types
///
dcl-c IV_BYTES 0;
dcl-c IV_VARCHAR2 1;
dcl-c IV_VARCHAR4 2;

///
// Conversion - named values; more exists
///
dcl-c IV_XLATE_EBCDIC 0;
dcl-c IV_XLATE_UTF8   1208;
dcl-c IV_XLATE_NO     65535;

///
// Set the buffer where the result is placed
//
//
// @param pClient pointer to the http client 
// @param pointer to buffer
// @param size    size of buffer ( bytes in total)
// @param buffer  type  0=Byte buffer, 1=varchar2, 2=varchar4 
// @param xlate   Any ccsid, 0=Current job, 65535=no xlate, 1208=UTF-8
///
dcl-pr iv_setResponseBuffer  extproc(*CWIDEN:'iv_setResponseBuffer');
    pClient       pointer value;
    pBuffer       pointer value;
    bufferSize    int(10);
    bufferType    int(5);
    bufferXlate   uint(5);
end-pr;

///
// run the request 
// @return *ON if ok 
///
dcl-pr iv_do ind   extproc(*CWIDEN:'iv_do');
    pClient     pointer value;
    method      pointer options(*string:*nopass) value;
    url         pointer options(*string:*nopass) value;
    timeOut     int(10) options(*nopass) value;
end-pr;

///
// returns the http status code 
//
//
// @param  pClient pointer to the http client 
// @return pointer to the client
///
dcl-pr iv_getStatus int(5) extproc(*CWIDEN:'iv_getStatus');
    pClient       pointer value;
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
dcl-pr iv_decodeBase64 varchar(524284:4) ccsid(*utf8) extproc(*CWIDEN : 'iv_decodeBase64') rtnparm;
  string varchar(524284:4) ccsid(*utf8) options(*varsize) const;
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
dcl-pr iv_encodeBase64 varchar(524284:4) ccsid(*utf8) extproc(*CWIDEN : 'iv_encodeBase64') rtnparm;
  string varchar(524284:4) ccsid(*utf8) options(*varsize) const;
end-pr;

///
// Add message to job log
//
// Convenience function: put message in joblog.
// Works like printf but with strings only like
//
//    iv_joblog('This is %s a test' : 'Super');
//
// @param format string
// @param Parms : list of strings
//
///
dcl-pr iv_joblog extproc(*CWIDEN : 'iv_joblog') ;
  formatString  pointer  options(*string)  value;
  string0       pointer  options(*string:*nopass) value;
  string1       pointer  options(*string:*nopass) value;
  string2       pointer  options(*string:*nopass) value;
  string3       pointer  options(*string:*nopass) value;
  string4       pointer  options(*string:*nopass) value;
  string5       pointer  options(*string:*nopass) value;
  string6       pointer  options(*string:*nopass) value;
  string7       pointer  options(*string:*nopass) value;
  string8       pointer  options(*string:*nopass) value;
  string9       pointer  options(*string:*nopass) value;
end-pr;

