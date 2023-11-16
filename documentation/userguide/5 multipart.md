---
title : Multipart
---

## What is a HTTP multipart message?

A normal HTTP message has exactly one part ... the message body. Multipart refers to messages 
which have multiple parts. These parts reside all in the main message body. Each part consists 
of a header section and a body section. The parts are separated by a boundary.

The encoding of the content of each part can either be text, binary or base64 encoded. Base64
encoded parts are often split into lines of 80 characters (including new line characters).
For usage with the HTTP protocol this is not really necessary. But it helps when you need to 
inspect a multipart message content.

Multipart messages are often used by file uploads but not exclusively.

The most used MIME type for multipart messages when used with the HTTP protocol is 
`multipart/form-data`. Though there are also other MIME types for multipart messages.


## Building Multipart Messages

The procedures with the namespace (prefix) `iv_multipart` can be used to easily build multipart 
messages. Sending multipart messages is done with the following steps:

1. Create a new multipart "object"
2. Add parts to the multipart object
3. Finalize the multipart object (no more parts can be added after this step)
4. Registering the multipart object as a request handler at the HTTP client instance
5. Executing the HTTP request
6. Freeing the memory allocated by the multipart object

The code for these steps can look like this:

```
dcl-s multipart pointer;
    
multipart = iv_multipart_new(IV_MULTIPART_MEDIA_TYPE);                // step 1
iv_multipart_addPartFromString(multipart : 'user' : 'Mihael');        // step 2
iv_multipart_addPartFromFile(multipart : 'avatar' : 'avatar.jpg');    // step 2
iv_multipart_finalize(multipart);                                     // step 3

httpClient = iv_newHttpClient();
iv_setRequestHandler(httpClient : multipart);                         // step 4

iv_execute(httpClient : 'POST' : 'http://localhost:35801/upload');    // step 5
...

on-exit;
    iv_multipart_free(multipart);                                     // step 6
```

Each multipart part has a name associated with it. In our case it is `user` and `avatar`.
When a file is added as a part some extra attributes are also set on the HTTP header
`Content-Disposition`, like filename. By default the filename of the original file is set
on the `Content-Disposition` header. But a different filename can be passed to the procedure,
see API documenation for `iv_multipart_addPartFromFile`.

Note: The multipart message is assembled in a stream file in `/tmp`. This can be viewed
and analyzed for debugging purposes. `iv_multipart_free` deletes the file.

