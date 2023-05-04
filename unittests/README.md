# ILEvator Unit Tests

For executing the unit tests located in the folder _unittests_ you need to 
previously install either [iRPGUnit][iru] or [RPGUnit][ru].

You can compile the unit tests with the make tool.

    make

The unit tests need access to the ASSERT copy book from the iRPGUNIT or RPGUNIT.
You need to pass this as a parameter to the make command:

    make RUINCDIR=/usr/local/include/irpgunit

By default the unit tests are placed in the ILEVATOR library. You can change
that by passing your custom library to the BIN_LIB parameter like this:

    make BIN_LIB=ILEVATORUT

The unit tests need the ILEVATOR modules. By default they are expected in the
library ILEVATOR. You can change the by passing that to the parameter
ILEVATOR_LIB like this:

    make BIN_LIB=ILEVATORUT ILEVATOR_LIB=MY_LVTR

Note: It is assumed that the ASSERT service program of the unit testing
      framework is in the library list.

[iru]: https://irpgunit.sourceforge.net
[ru]: https://rpgunit.sourceforge.net

