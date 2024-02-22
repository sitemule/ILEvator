---
title : Headers
---

ILEvator sets the minimal HTTP headers needed for the request. But you can add additional custom
headers as needed. Headers can also be set multiple times. Generally the last one wins (though
some headers are only allowed once like `Host`, `Content-Length`, `Upgrade` or `Authorization`).

## Porcelain API

When using the porcelain API like `iv_get` or `iv_post` you can add additional headers by passing
a list of headers as a parameter to to procedure call.

```
dcl-s headers pointer;

headers = iv_buildList(
    'accept-language' : 'dk,de;q=0.5' :
    'authentication' : 'Bearer my_token'
);

string = iv_get('http://localhost' : IV_MEDIA_TYPE_JSON : headers);
```

The headers list is built dynamically and as such allocates memory dynamically. This memory must
be freed manually by calling `iv_freeList`.

```
iv_freeList(headers);
```


## Low Level API

The low level API supports setting HTTP headers global for that HTTP client instance. Those
HTTP headers will be added to each HTTP request made with this client instance, see 
`iv_addHeader` and `iv_addHeaders`.

```
dcl-proc main;
    dcl-s httpClient pointer;
    dcl-s buffer varchar(65000:4) ccsid(1208);
    dcl-s headers pointer;
    
    headers = iv_buildList(
        'accept-language' : 'dk,de;q=0.5' :
        'authentication'  : 'Bearer my_token'
    );
    
    httpClient = iv_newHttpClient();
    iv_addHeaders(httpClient : headers);
    iv_setResponseDataBuffer(httpClient : %addr(buffer) : %size(buffer) : IV_VARCHAR4 : IV_CCSID_UTF8);

    iv_execute(httpClient : 'GET' : 'http://localhost');
    if (iv_getStatus(httpClient) = IV_HTTP_OK);
        // everything ok
    else;
        // not ok
    endif;

    on-exit;
        iv_free(httpClient);
        iv_freeList(headers);
end-proc;
```

Headers can also be passed to the `iv_execute` procedure call. These headers will be appended
after the headers previously set on the HTTP client instance.

```
dcl-proc main;
    dcl-s httpClient pointer;
    dcl-s buffer varchar(65000:4) ccsid(1208);
    dcl-s headers pointer;
    
    headers = iv_buildList(
        'accept-language' : 'dk,de;q=0.5' :
        'authentication'  : 'Bearer my_token'
    );
    
    httpClient = iv_newHttpClient();
    iv_setResponseDataBuffer(httpClient : %addr(buffer) : %size(buffer) : IV_VARCHAR4 : IV_CCSID_UTF8);

    iv_execute(httpClient : 'GET' : 'http://localhost' : headers);
    if (iv_getStatus(httpClient) = IV_HTTP_OK);
        // everything ok
    else;
        // not ok
    endif;

    on-exit;
        iv_free(httpClient);
        iv_freeList(headers);
end-proc;
```

Note: The headers list is built dynamically and as such allocates memory dynamically. This memory 
must be freed manually by calling `iv_freeList`.


## Dynamic Header List

The list can be created with `iv_buildList`. You can create an empty list or fill the list with
initial values. The list is not immutable. You can add values to the list by calling
`iv_addHeaderToList`.

```
    dcl-s headers pointer;
    
    headers = iv_buildList();
    iv_addHeaderToList(headers : 'accept-language' : 'dk,de;q=0.5');
    iv_addHeaderToList(headers : 'authentication' : 'Bearer my_token');

    string = iv_get('http://localhost' : IV_MEDIA_TYPE_JSON : headers);

    iv_freeList(headers);
end-proc;
```


## Request Handler

You can also hook yourself into the flow of a request and modify the request, f. e. add a header.
This can be done with a request handler. The handler is implemented as a data structure with a 
subfield `processRequest` which contains the pointer to the function which will be called by 
ILEvator to intercept and process the request.

```
dcl-ds requestHandler_t qualified template;
    processRequest pointer(*proc);
end-ds;
```

The process procedure has a predefined procedure interface which needs to be implemented.

```
dcl-pi *n;
    handler pointer value;
    request pointer value;
end-pi;
```

So a procedure to add a header could look like this:

```
dcl-proc addCustomHeader export;
    dcl-pi *n;
        handler pointer value;
        request pointer value;
    end-pi;

    dcl-ds requestHandler likeds(requestHandler_t) based(handler);

    iv_request_addHeader(request : 'X-Custom-Header' : 'Value 123');
end-proc;
```

Inside the process procedure you can use the request API of ILEvator to modify the request before
it is sent to the server.

Last but not least you need to set the pointer to your process procedure in the handler data
structure.

```
dcl-ds requestHandler likeds(requestHandler_t) inz;

requestHandler.processRequest = %paddr(addCustomHeader);
```

And before you are making your first call you need to register the request handler at the HTTP
client instance.

```
iv_setRequestHandler(httpClient : %addr(requestHandler));
```

Now on each request `addCustomHeader` will be called and each request will have the HTTP header
`X-Custom-Header`.


## Non-ASCII Characters

Historically characters from the ISO-8859-1 character set were also allowed in HTTP header values
but that is a thing of the past. Servers, frameworks and software libraries will expect only ASCII
characters in HTTP header keys and values. So as a general rule only ASCII characters should be used.

In case non-ASCII characters need to be passed as a header value the sender and the receiver will
have to encode/decode them manually. Base64 and MIME encoding can be used for these cases. ILEvator
provides procedures for both, `iv_encode_mime` and `iv_encode_base64` / `iv_encode_base64_buffer`.
