---
title : HTTPS
---

To be able to make HTTP requests over SSL/TLS (also known as HTTPS) you first need to set the 
keystore with the necessary certificates by calling `iv_setCertificate`. If the keystore needs 
a password it also needs to be passed to the procedure.

The keystore can also be registered by using environment variables.

- ILEVATOR_CERTIFICATE : path to the keystore file
- ILEVATOR_CERTIFICATE_PASSWORD : password

The ILEvator project comes with a keystore file `ilevatgor.kdb`.


## Create Keystore

TODO 


## Self Signed Certificate

TODO

