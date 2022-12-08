[![Open in Visual Studio Code](https://open.vscode.dev/badges/open-in-vscode.svg)](https://open.vscode.dev/sitemule/ILEvator)
# ILEvator
It is a http client for the ILE environment on IBM i. 

This project is premature - don't waste time cloning it yet - but stay tuned.




### Installation

Installation of ILEVATOR should be done with `git` and `gmake` (GNU Make), which are available via `yum` - you can read more about [yum here](https://bitbucket.org/ibmi/opensource/src/master/docs/yum/).

ILEVATOR is a two step process. ILEVATOR requires you to build from source, but this step has been totally automated for you. To install ILEVATOR, you need to use the pase environment (with `ssh` for example) and with a couple of seconds you can have the project built. No need to download save files, upload them or restore them.

```
ssh my_ibm_i
mkdir /prj
cd /prj
git -c http.sslVerify=false clone https://github.com/sitemule/ILEvator.git
cd ILEvator
gmake
```

This will create:

* The `ILEVATOR` library
* `ILEVATOR/ILEVATOR` service program.
* `ILEVATOR/QRPGLEREF.ILEVATOR` for the XML ILEVATOR API prototypes.
* `ILEVATOR/ILEVATOR` binding directory, with the `ILEVATOR` object on it.


### Build the distribution.

When you have made the project in library ILEVATOR, you can create the release as a savefile
```
ssh my_ibm_i
cd /prj/ILEvator
gmake clean release
```