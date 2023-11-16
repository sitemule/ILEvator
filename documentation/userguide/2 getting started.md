---
title : Getting Started
---

```
**FREE

ctl-opt dftactgrp(*no) actgrp(*caller) bnddir('ILEVATOR') main(main);

/include 'ilevator.rpgle'

dcl-proc main;
    dcl-s string varchar(IV_BUFFER_SIZE) ccsid(1208);
    
    string = iv_get('http://localhost');

    iv_joblog(string);
end-proc;

```

This is probably the simplest use case you can implement with ILEvator. The content from the 
index page of the web server on localhost (your IBM i server) is fetched with a GET request and 
stored in the variable `string` which is output to the joblog. There is actually nothing to 
explain here as everything is clearly visible in the code.

Depending on how you have installed ILEvator you need to adjust the compile command and/or the 
code.

If you have installed ILEvator manually by compiling the source you will have a binding directory
`ILEVATOR` in your target library where the ILEvator service program has been placed. You just
need to add the target library to your library list and are good to go.

```
CRTBNDRPG SAMPLE INCDIR('/my/ilevator/source/directory/headers')
```

If you have installed ILEvator with iPKG you will find a binding directory `IPKG` in the library
you installed the ILEvator package in. The ILEvator service program is automatically registered
in that binding directory. So you will have to replace the value of the `bnddir` control option 
with `IPKG`. Now you just need to add the library you used with iPKG to your library list and are
good to go.

```
CRTBNDRPG SAMPLE INCDIR('/my/ipkg/location/for/copybooks/ilevator')
```

## Hassle-free compile

If you take the control/compile options out of the source code it won't matter anymore how you
installed your software packages or if you are using binding directories or not. Change the
control options line to just:

```
ctl-opt main(main);
```

For every pro there is also a con: Now you have to do two build steps to create the program.

```
CRTRPGMOD SAMPLE INCDIR('/my/directory/for/copybooks/ilevator')
CRTPGM SAMPLE DFTACTGRP(*NO) ACTGRP(*CALLER) BNDDIR('MY_BND_LIB/BND_DIR')
```

Even if you restructure your includes or binding directories or switch from/to binding the
service programs directly you don't have to change the source code anymore.
