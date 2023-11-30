This is an integration backed for ILEvator which starts a proxy server on port 35801
and an echo service on port 35802.

The proxy is only bound to the local interface (localhost) and is not available from
the outside.


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
java -Djava.net.preferIPv4Stack=true -cp target/proxy.echo-1.0.0.jar:target/libs/* ilevator.proxy.echo.ProxyEchoApplication

```
