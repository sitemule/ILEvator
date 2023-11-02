---
title : Headers
---

ILEvator sets the minimal HTTP headers needed for the request. But you can add additional custom
headers as needed. Headers can also be set multiple times. Generally the last one wins (though
some headers are only allowed once like `Host`, `Content-Length`, `Upgrade` or `Authorization`).

## Porcelain API

When using the porcelain API like `iv_get` or `iv_post` you can add additional headers by passing
a list of headers as a parameter to to function call.

The list can be created with `iv_buildList`. You can create an empty list or fill the list with
initial values. The list is not immutable. You can later add values to the list by calling
`iv_addHeaderToList`.

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

The process function has a predefined procedure interface which needs to be implemented.

```
dcl-pi *n;
    handler pointer value;
    request pointer value;
end-pi;
```

So a function to add a header could look like this:

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

Inside the process function you can use the request API of ILEvator to modify the request before
it is sent to the server.

Last but not least you need to set the pointer to your process function in the handler data
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
