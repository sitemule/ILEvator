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