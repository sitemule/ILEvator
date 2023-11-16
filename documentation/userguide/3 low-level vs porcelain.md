---
title : Low-Level API vs Porcelain API
---

ILEvator supports two kind of APIs: Low-Level API and Porcelain API. Depending on the features
you need you will choose one over the other.

## Low-Level API

With the low-level API you can execute every HTTP request supported by ILEvator. You have the
following workflow when using the low-level API.

1. Create a new HTTP client instance with `iv_newHttpClient`
2. Configure the HTTP client instance
3. Execute the HTTP request with `iv_execute`
4. Free allocated resources with `iv_free`

## Porcelain API

This API is rather high-level but is not as flexible as the low-level API. The API is modelled
after the most used and obvious use cases. But some features cannot be used when using the 
porcelain API.

All the HTTP methods supported by ILEvator have a porcelain API like `iv_get`, `iv_options`, 
`iv_delete`, etc.

You have no access to the actual HTTP client instance. If you need to configure the HTTP client
you have to use the low-level API.

### HTTP Status Code

There is also no direct querying of the returned HTTP status code. Every status code less than
400 is considered successful. When using the porcelain API every returned HTTP status code greater
than or equal 400 sends an escape message to the caller. The escape message has the message id
starting with `ILV` plus the HTTP status code. The HTTP status code 404 - Not Found will trigger
an escape message with the message id `ILV0404`. If a HTTP status cannot be mapped to message id
the escape message will have the message id `ILV0999`.
