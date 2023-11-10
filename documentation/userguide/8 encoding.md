---
title : Encoding
---

Encoding plays an important role in the world wide web as we no longer are only on our own server 
but are communicating with different servers which may be using the same or a different encoding 
as we are.

## MIME Types

[MIME](https://en.wikipedia.org/wiki/MIME) stands for _Multipurpose Internet Mail Extensions_ and 
was originally designed and developed for email communication (mainly SMTP) but has also founds 
its way into other communication protocols like HTTP.

In the context of HTTP the MIME type indicates what the format and structure of the message body
is. But in case of vendor or personal specific MIME types it can also give you more detailed
information about the message body.

Note: The terms media type and MIME type are (mostly) interchangeable in the context of HTTP.


### Examples

`text/plain` : This is just text and by default the characters set used is US-ASCII.

`application/xml` : This is XML and the encoding is specified in the XML itself.

`application/json` : This is JSON. JSON by default is encoded in UTF-8.

`application/vnd.rpgnextgen.blog` : The content is a blog entry from RPG Next Gen. But we have no idea what the format is like or the encoding.

`application/prs.rpgnextgen.blog+json` : This is better as we now know what is passed as the message body and in what format.

`application/prs.rpgnextgen.blog+json+v1` : This defines the content of the message pretty well. It is a blog entry from RPG Next Gen in the JSON format and the structure of the JSON is in version 1.

Depending on the content and the MIME type it might be necessary to state the encoding of the content.

`text/plain; charset=UTF-8` : In this case the text is not in US-ASCII but in UTF-8 and may contain symbols not available in US-ASCII.


## Content Type

If data is sent to the server (f. e. on POST, PUT, PATCH requests) the HTTP header `Content-Type`
should be set accordingly. HTTP headers can be set for all requests made by the HTTP client by 
using `iv_addHeader` and `iv_addHeaders`. If a HTTP header should only be used in a single request
then a list of headers can be passed as an additional parameter.

The porcelain API provides a separate parameter for passing the `Content-Type` HTTP header value.

The `Content-Type` HTTP header only _declares_ what the content is expected to be. It doesn't not 
acutally converts any data to the corresponding MIME type or changes the data to the declared 
encoding/charset.

It acts like a label on a packages. It states what to expect inside the package but the real 
content may differ from the label.


## Message Body Data Translation

When using `iv_execute` the data is provided by passing the variable to `iv_setRequestDataBuffer`
and specifying the CCSID on the last parameter. The data will automatically be converted to the
CCSID which corresponds to the `Content-Type` used on the request. If no CCSID could be mapped 
to the used `Content-Type` the data will be converted to UTF-8.

The porcelain API only accepts data in UTF-8.

When receiving data the data is put into the variable provided by calling `iv_setResponseDataBuffer`.
Currently there is no extra translation of the data. The data from the message response is put into 
the data buffer as is.
