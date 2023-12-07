---
title : HTTPS
---

To be able to make HTTP requests over SSL/TLS (also known as HTTPS) you first need to set the 
keystore with the necessary certificates by calling `iv_setCertificate`. If the keystore needs 
a password it also needs to be passed to the procedure.

The keystore can also be registered by using environment variables.

- ILEVATOR_CERTIFICATE : path to the keystore file
- ILEVATOR_CERTIFICATE_PASSWORD : password

The ILEvator project comes with a keystore file `ilevator.kdb`.


## TLS Version

By default ILEvator and the target server will try to negotiate the newest/highest TLS version
supported by both parties. ILEvator allows to disabled specific TLS versions. This can either be
made by a procedure call or by setting an environment variable.

> Environment variables take precedence over settings made by procedure calls.

### Procedure Call

Calling `iv_configureTlsVersion` will enable or disable the TLS version for negotiation with the
server. A constant is available for each TLS version like `IV_TLS_13` for TLS version 1.3.

```
iv_configureTlsVersion(httpClient : IV_TLS_13 : *off);
```

This setting will only affect the HTTP client instance you passed as the first parameter. Multiple
client instances can be created side by side with different TLS version settings.


### Environment Variable

Each TLS version can also be enabled/disabled with the setting of an environment variable. 

- TLS 1.0 : `ILEVATOR_TLS_10`
- TLS 1.1 : `ILEVATOR_TLS_11`
- TLS 1.2 : `ILEVATOR_TLS_12`
- TLS 1.3 : `ILEVATOR_TLS_13`

Setting the value of the environment variable to `0` disables the TLS version and `1` enables the TLS version.

Disable TLS version 1.1:

```
ADDENVVAR ILEVATOR_TLS_11 '0'
```

Enable TLS version 1.3:

```
ADDENVVAR ILEVATOR_TLS_13 '1'
```

Setting the TLS version via environment variables affects all HTTP client instances in the job.


## Keystore File

ILEvator uses the IBM GSKit on the IBM i server for making a secure connection to the target server.
A keystore file in the GSKit format is needed with the certificates corresponding to the target
server. To create the keystore file (suffix .kdb) the command `gsk8cmd` from the IBM GSKit can be used.

