---
title : Consecutive Requests
---

If you execute a HTTP request via the porcelain API or by calling `iv_execute` a socket to the web
server is opened. The socket connection will automatically be closed after the request. In case you 
want to execute multiple requests to the same web server this will waste time and resources.

## Unclosed Socket

With ILEvator you also have the option to open a socket connection and leave it open for later 
requests by calling `iv_connect`.

```
if (iv_connect(client : url));
    executeRequests();
else;
    // could not connect to server
    ...
endif;
```

After having successfully opened a socket connection to the web server you can send HTTP requests
by calling `iv_http_request`. This will not close the socket in contrast when using the other 
procedures like `iv_execute`.

A call to `iv_disconnect` will close the socket connection. If you don't need the HTTP client
instance any more you can skip calling `iv_disconnect` and just call `iv_free`. Any open socket 
connection of the passed HTTP client instance will be closed before freeing any memory.

Note: On both calls (`iv_connect` and `iv_http_request`) a URL needs to be passed. `iv_connect`
      just needs the host and port part of the URL whereas `iv_http_request` needs the rest of
      the URL. You can safely just pass the same full URL to both procedures.