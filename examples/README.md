# ILEvator Examples

The example programs show how the HTTP client can be used.


## Compiling

The `ILEVATOR` binding directory must be on the library list. The easiest way to
achieve this is by adding the ILEVATOR library to the library list.

The location of the copy book `ilevator.rpgle` is not specified in the example
code.

```
/include 'ilevator.rpgle'
```

So the location of the copy book must be specified on the `INCDIR` parameter of
`CRTBNDRPG` command.

```
CRTBNDRPG PGM(IVEX01) SRCSTMF('/path/to/ilevator/examples/IVEX01-Simple GET request.rpgle') INCDIR('/path/to/ilevator/headers')
```
