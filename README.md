# temperature_pool

This project is made for testing `TemperaturePool` class on Netburner MOD54415 platform. `TemperaturePool` simply reads temperature values from sensors on I2C bus.

To compile it use IDE NBEclipse 2.8.7 or higher. There are 2 build configurations:

1. `_release` - builds example program that sets up a web server, registers some sensors that must be presented on the board and periodically polls them sending the values via HTTP and serial port
2. `_release_tests` - builds test program with some tests for `I2cBus` and `TemperaturePool` classes. Must also be executed on the board with mentioned sensors