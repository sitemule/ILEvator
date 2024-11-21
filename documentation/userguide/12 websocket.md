---
title : WebSocket
---

> In a nutshell, WebSocket is a technology that enables bidirectional, full-duplex communication
> between client and server over a persistent, single-socket connection. The intent is to provide 
> what is essentially an as-close-to-raw-as-possible TCP communication layer to web application 
> developers while adding a few abstractions to eliminate certain friction that would otherwise 
> exist concerning the way the web works. A WebSocket connection starts as an HTTP request/response
> handshake; beyond this handshake, WebSocket and HTTP are fundamentally different.
>    --- websocket.org

ILEvator supports the WebSocket protocol as a first class citizen. Procedures, variables and 
constants with the prefix `iv_ws` are part of the WebSocket support. 

A WebSocket connection is initiated by doing a "handshake" which is a normal HTTP request with some
corresponding HTTP headers to perform an "Upgrade". From then on the WebSocket protocol is in effect.

WebSocket is fundamentally different from HTTP. For a compact comparison of these two protocols
visit [WebSocket.org](https://websocket.org/guides/road-to-websockets/#comparing-websockets-and-http).
This statement from WebSocket.org sums it up quite nicely:

> The intent is to provide what is essentially an as-close-to-raw-as-possible TCP communication layer ...

## Usage

First you need to create a new HTTP client instance and make a connect (Upgrade request) to the 
websocket server.

```
dcl-s client pointer; 

client = iv_newHttpClient();

if (iv_ws_connect(client : 'ws://localhost:35801/echo'));
    // got a connection to the web socket server -> feel free ;-)
endif;
```

Some websocket servers are very picky and may deny a connection. ILEvator will output the reason
to the job log when the debug mode has been enabled.

After you got a connection to the websocket server you can send and receive messages. Remember
that WebSocket is a bi-directional protocol. You don't have to make a request to get data from
the server. The server can send data to the client without the client making the first move
(server push).

If you are finished you need to close the connection by calling `iv_ws_disconnect`. This will
close the socket connection and you can reuse the client instance.

Note: The socket connection will be closed automatically when releasing all resources on calling 
      `iv_free`.

## Imperative Style

ILEvator lets you be in full control when it comes to communicating with the websocket server.
You decide when you want to receive messages and when you send messages.

```
dcl-s client pointer; 
dcl-ds response likeds(iv_ws_message_t) inz;
dcl-s msg char(50);

client = iv_newHttpClient(); 

if (iv_ws_connect(client : 'ws://localhost:35801/echo'));
    iv_ws_sendText(client : 'Hello World!');
    response = iv_ws_receiveText(client);
    msg = %char(response.payload);
    dsply msg;
else;
    dsply 'Connection failed.';
endif;

on-exit;
    iv_free(client);
```

This style is best used when you (mainly) want to send data to the websocket server.


## Reactive Style

When your main goal is to react on messages sent from a websocket server you can more easily
achieve this by using a more reactive style. You just provide a procedure which gets called
every time the websocket server sends messages to your client.

ILEvator supports this with the procedures `iv_ws_onMessage`and `iv_ws_endMessageReceiving`.

Your procedure needs to implement the following procedure interface:

```
dcl-pi *n;
    client pointer value;
    userData pointer value;
    message likeds(iv_ws_message_t);
end-pi;
```

_client_ : This is your HTTP client instance. If you want to respond to the received message
           you will need this pointer.  
_userData_ : userData points to data you provided on the `iv_ws_onMessage` call. By passing 
             this to your procedure you don't need to have some static variables for data your 
             procedure needs to access.  
_message_ : This is the message received from the websocket server.

Note: [Control frames](https://www.rfc-editor.org/rfc/rfc6455.html#section-5.5) (messages) will 
      be processed by ILEvator and will not be passed to your callback procedure.

This example will count the number of text messages sent from the server till the server disconnects.

```
dcl-proc main;
    dcl-s client pointer; 
    dcl-s errorMessage varchar(256);
    dcl-s userData int(10);
    
    client = iv_newHttpClient(); 
    
    iv_ws_connect(client : 'ws://localhost:35801/ws'); 
    
    iv_ws_onMessage(client : %paddr(callback) : %addr(userData));
    dsply %trimr('Message count: ' + %char(userData));

    on-exit;
        iv_free(client);
end-proc;

dcl-proc callback;
    dcl-pi *n;
        client pointer value;
        userData pointer value;
        message likeds(iv_ws_message_t);
    end-pi;
    
    dcl-s counter int(10) based(userData);
    dcl-s msg char(50);

    if (message.opcode = IV_WS_OPCODE_DATA_TEXT);
        counter += 1;

        msg = %char(message.payload);
        dsply msg;
    endif;
end-proc;
```

## Tests

The ILEvator project contains an integration test server which also integrates a web socket server,
Eclipse [Tyrus](https://eclipse-ee4j.github.io/tyrus/). This test server can be run on IBM i, 
see [Readme.md](https://github.com/sitemule/ILEvator/blob/main/integrationtests/helidon/README.md).

The test suite [ITWEBSOCK](https://github.com/sitemule/ILEvator/blob/main/integrationtests/itwebsock.rpgmod) 
contains all the test cases which can be easily run with [RPGUnit](https://github.com/Tools-400/irpgunit)
by a Jenkins instance on IBM i.


## Use Cases

The WebSocket protocol is mostly used in the web environment to communicate streaming data between
the browser and the server. But there are some cases where you want to stream data from a backend 
process to the server or receive streaming data from the server.

Message streaming server which support the WebSocket protocol are
- [Apache Kafka](https://kafka.apache.org)
- [Apache Pulsar](https://pulsar.apache.org)

Another use case is where ports in an environment are restricted to HTTP(S) but an application specific
protocol (not HTTP) over sockets is needed.
