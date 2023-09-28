This is an integration backed for ILEvator using the microservice framework
Helidon. Helidon is written in Java and thus needs Java for building and running.

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
java -Djava.net.preferIPv4Stack=true -cp ./:target/helidon-1.0.0.jar:target/libs/* io.helidon.microprofile.cdi.Main

```

### Multipart Endpoint

The Multipart endpoint for file upload will store the file in the `/tmp` folder. 
The uploaded files will not be automatically deleted.
 