---
title: Installation
---

ILEvator can be installed in various ways with more or less effort depending on the choice.

## From Source

The prerequisites for building ILEvator from source are at least IBM i OS release 7.3, access to 
the IFS of your IBM i server, ILE C and RPG compiler and `gmake`.

`gmake` can be installed via [yum](https://ibmi-oss-docs.readthedocs.io/en/latest/yum/README.html#installing-additional-software).

If all prerequisites are met you can either download or clone the 
[Git repository](https://github.com/sitemule/ILEvator) to your IBM i server.

You start building the project just be executing `gmake`. By default the library `ILEVATOR` will
be created and all objects will be placed in that library. 


### Build Options

You can change the target library by passing the parameter `BIN_LIB` to the `gmake` command.

```
gmake BIN_LIB=MY_IV_LIB
```

By default the project will be built for the current release of the system. You can specify the 
target release version by passing the parameter `TARGET_RLS` to the `gmake` command.

```
gmake TARGET_RLS=V7R3M0
```

No compile listing will be output to the console during the build. To get a compile listing you
need to pass the parameter `OUTPUT` to the `gmake` command.

```
gmake OUTPUT=*PRINT
```

If the module and service program needs to be rebuilt you can pass the target `ext` (for building
all external dependencies) and `compile` to the `gmake` command.

```
gmake ext compile
```

Single modules can be compiled by passing the module file name to the `gmake` command.

```
gmake request.rpgmod
```

For just building the service program execute the target `ilevator.srvpgm`.


## iPKG

But you don't need to build the service program by yourself. There is already a prepackaged
version of ILEvator available at the iPKG RPM repository at https://repo.rpgnextgen.com.

You just need to download the [iPKG client] and follow the instructions at https://repo.rpgnextgen.com.

First create the library to you want to install ILEvator into. Just execute the `install` command for
installing ILEvator.

```
ipkg install ilevator
```

This will install ILEvator in the default library `IPKG`. To specify another library just add the
parameter `IPKGLIB`.

```
ipkg install ilevator ipkglib(MY_IV_LIB)
```

The copybooks can also be installed with iPKG.

```
ipkg install 'ilevator-devel'
```

The copybooks are packaged as stream files. By default the install command will try to install
them in `/usr/local/include` in the IFS. You can change the target location by passing the location
as a parameter.

```
ipkg install 'ilevator-devel' loc('/home/mihael/include')
```
