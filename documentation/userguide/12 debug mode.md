---
title : Debug Mode
---

ILEvator supports writing debug messages into the job log. These messages are of type `*INFO`.
To activate debug messages the environment variable `ILEVATOR_DEBUG` needs to be set to `1` which
maps to `*ON`. By default this is turned off.

```
ADDENVVAR ILEVATOR_DEBUG '1'
```

The environment variable needs to be set before the first call of any procedure from the service
program `ILEVATOR` (more precisely before activating the service program). Once set it cannot be
unset. If you need to set/unset this on a regular basis then you can run it in a named activation
group. If you want to unset the debug mode just reclaim the activation group so that the service
program will be loaded again on the next call and evaluates the environment variable for the debug 
mode again.


## Communication Trace

The communication between the client and the server can be traced separatedly from the debug 
information in a stream file. Tracing is enabled by calling the procedure `iv_setCommTrace` and 
passing the stream file for the trace as a parameter or by setting the environment variable 
`ILEVATOR_TRACE_STMF` to the stream file for the trace. This needs to be set before the first 
HTTP request with the created client.

```
ADDENVVAR ILEVATOR_TRACE_STMF '/home/my_user/trace_file.txt'
```

The file will be created if it does not exist (with the CCSID Latin 1 , 5348 = Windows, Latin 1 
with Euro, see [CCSID values defined on IBM i](https://www.ibm.com/docs/en/i/7.5?topic=information-ccsid-values-defined-i)). The trace data will be appended to the configured 
trace file.