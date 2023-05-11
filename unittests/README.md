# ILEvator Unit Tests

For executing the unit tests located in the folder _unittests_ you need to 
previously install either [iRPGUnit][iru] or [RPGUnit][ru].

You can compile the unit tests with the make tool.

    make

By default the unit tests are placed in the ILEVATOR library. You can change
that by passing your custom library to the BIN_LIB parameter like this:

    make BIN_LIB=ILEVATORUT

The unit tests need the ILEVATOR modules. By default they are expected in the
library ILEVATOR. You can change the by passing that to the parameter
ILEVATOR_LIB like this:

    make BIN_LIB=ILEVATORUT ILEVATOR_LIB=MY_LVTR

The service programs of the unit testing framework are assumed to be on the 
library list. You can specify a library by passing it to the _make_ command.

    make BIN_LIB=ILEVATORUT ILEVATOR_LIB=MY_LVTR RU_LIB=MY_RPGUNIT

To execute a unit test just call the RUCALLTST command.

    RUCALLTST BASICAUTUT

## Request Test Data

The test data for the request unit test are located in the request folder. It
is important that the line endings are CR + LF. If the test data is edited on a
Linux box or in PASE then the editor may change the line endings to LF only. The
line endings can be fixed with the program `unix2dos`.

    unix2dos request/test10-formData.http

Any additionally calls of unix2dos on an already converted file will yield the
same result. No harm is done in calling it twice or more times.

[iru]: https://irpgunit.sourceforge.net
[ru]: https://rpgunit.sourceforge.net

