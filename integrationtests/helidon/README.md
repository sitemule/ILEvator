This is an integration backed for ILEvator using the microservice framework
Helidon. Helidon is written in Java and thus needs Java for building and running.

## Requirements

- Java 17
- Apache Maven (for building)


## Build

The project is built with Maven.

```
mvn -Djava.net.preferIPv4Stack=true clean package
```


## Run

The integration backend is started with a simple Java command. Java 11 is needed.
The Java version can be checked on the shell with the command `java -version`.
Java 11 is available on IBM i via yum or as a licensed product.

The application is started with the command:

```
java -Djava.net.preferIPv4Stack=true -cp ./:target/ilevator-helidon-integration.jar:target/libs/* io.helidon.microprofile.cdi.Main

```

### Multipart Endpoint

The Multipart endpoint for file upload will store the file in the `/tmp` folder. 
The uploaded files will not be automatically deleted.

The upload folder can be configure by specifying a system property on the call:

```
java -Djava.net.preferIPv4Stack=true -Dilevator.upload.dir=/my/custom/upload/dir  -cp ./:target/ilevator-helidon-integration.jar:target/libs/* io.helidon.microprofile.cdi.Main
```

It can also be specified by using an environment variable:

```
export ILEVATOR_UPLOAD_DIR=/my/custom/upload/dir

java -Djava.net.preferIPv4Stack=true -cp ./:target/ilevator-helidon-integration.jar:target/libs/* io.helidon.microprofile.cdi.Main
```
