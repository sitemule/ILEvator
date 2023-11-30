---
title : HTTP Tunneling
---

> Tunneling transmits private network data and protocol information through public network by
> encapsulating the data. HTTP tunneling is using a protocol of higher level (HTTP) to transport 
> a lower level protocol (TCP).
> --- [MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Proxy_servers_and_tunneling#http_tunneling)

ILEvator supports HTTP tunneling. The HTTP client instance just needs to have the proxy URL set.
Any further calls with this HTTP client instance will be made through the proxy.

```
// configure HTTP client instance to use proxy
iv_setProxyTunnel(httpClient : 'http://my_proxy:3128');
```

The porcelain API cannot be used when using HTTP tunneling as the HTTP client instance needs to
be configured and that cannot be done with the procelain API.

For executing HTTP requests always use `iv_execute`.

```
dcl-proc main;

    dcl-s httpClient pointer;
    dcl-s buffer varchar(65000:4) ccsid(1208);
    
    httpClient = iv_newHttpClient();
    iv_setProxyTunnel (httpClient : 'http://my_proxy:3128');

    iv_setResponseDataBuffer(
        httpClient : 
        %addr(buffer) : 
        %size(buffer) : 
        IV_VARCHAR4 : 
        IV_CCSID_UTF8
    );
    
    iv_execute (httpClient : 'GET' : 'http://target_resource'); 

    if (iv_getStatus(httpClient) <> IV_HTTP_OK); 
        iv_joblog('Invalid status: ' + %char(iv_getStatus(httpClient)));
    endif;

    iv_joblog(%char(buffer));

    on-exit;
        iv_free(httpClient);
        
end-proc;
```
