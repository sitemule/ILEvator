# ILEvator Integration Tests

For executing the integration tests located in the folder _integrationtests_ you
need to previously install either [iRPGUnit][iru] or [RPGUnit][ru].

You can compile the unit tests with the make tool.

    make

By default the unit tests are placed in the ILEVATOR library. You can change
that by passing your custom library to the BIN_LIB parameter like this:

    make BIN_LIB=ILEVATORIT

The integration tests need the ILEVATOR service program. By default it is 
expected in the library ILEVATOR. You can change the by passing that to the 
parameter ILEVATOR_LIB like this:

    make BIN_LIB=ILEVATORIT ILEVATOR_LIB=MY_LVTR

The service programs of the unit testing framework are assumed to be on the 
library list. You can specify a different library by passing it to the _make_ 
command.

    make BIN_LIB=ILEVATORIT ILEVATOR_LIB=MY_LVTR RU_LIB=MY_RPGUNIT

To execute a unit test just call the RUCALLTST command.

    RUCALLTST BASICAUTUT


## ILEastic Backend

The integration tests are executed with the backed located in this project. One
of those backends is ILEastic. The backend can be build with the _make_ command

    make ileastic

It is assumed that the ILEASTIC service program is in the library ILEASTIC. You 
can specify a different library by passing it as a parameter:

    make ILEASTIC_LIB=MY_ILEA ileastic

The location of the copy books of ILEastic also need to be specified by passing
it as a parameter:

    make ILEASTIC_LIB=MY_ILEA ILEASTIC_INCDIR=/home/me/include/ileastic ileastic


## Building All

By either passing no target or by passing the target `all` the backend and the
integration tests will be built.


## Starting the Backend

Each backend has a `start_<backend>` target. To start the ILEastic backed pass
the target `start_ileastic`.

    make start_ileastic


[iru]: https://irpgunit.sourceforge.net
[ru]: https://rpgunit.sourceforge.net

