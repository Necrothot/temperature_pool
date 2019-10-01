#include "tests/unit_tests.h"

#include <cmath>
#include <cstdio>
#include <map>
#include <memory>

#include "3rdparty/lest/lest.hpp"

#include "global_definitions/types.h"
#include "global_definitions/constants.h"
#include "drivers/i2c_bus.h"
#include "system/temperature_pool.h"


// Define RF module symbol
//#define GOTMIC
#define ORPHEUS


#define SCENARIO(sketch) lest_SCENARIO(specification, sketch)
#define CASE(name) lest_CASE(specification, name)
static lest::tests specification;


// =========================== I2cBus class ===================================
CASE("I2cBus: create multiple instances with identical module numbers")
{
    // Testing I2cBus ctor adding handled module to static set
    SETUP("I2cBus instance with module 1")
    {
        I2cBus i2c_bus(1);

        SECTION("Throws upon creating another instance with module 1")
        {
            EXPECT_THROWS_AS(I2cBus(1), std::runtime_error);
        }
    }

    // Testing I2cBus dtor removing handled module from static set
    SETUP("I2cBus instance with module 1 after module 1 already been in use")
    {
        {
            I2cBus i2c_bus(1);
        }
        EXPECT_NO_THROW(I2cBus(1));
    }
}


// ======================= TemperaturePool class ==============================
CASE("TemperaturePool: register/unregister sensors")
{
    SETUP("Empty temperature pool")
    {
        I2cBus i2c_bus(1);
        TemperaturePool temp_pool(i2c_bus);

        SECTION("Fails when unregistering nonexistent sensor")
        {
            EXPECT_NOT(temp_pool.unregisterSensor("Test"));
        }

        SECTION("Registering and unregistering \"Test\" sensor")
        {
            EXPECT(temp_pool.registerSensor("Test", 0x10));
            EXPECT(temp_pool.unregisterSensor("Test"));
        }

        SECTION("Fails when registering two sensors with equal names")
        {
            temp_pool.registerSensor("Test", 0x40);

            EXPECT_NOT(temp_pool.registerSensor("Test", 0x50));
        }
    }
}

SCENARIO("TemperaturePool: read from temperature sensors")
{
    GIVEN("Pool with Power Unit, Modem, Cross Board and Upconverter sensors")
    {
        I2cBus i2c_bus(1);
        TemperaturePool temp_pool(i2c_bus);

        temp_pool.registerSensor(constants::tsensornames::power_unit, 0x4A);
        temp_pool.registerSensor(constants::tsensornames::modem, 0x4E);
        temp_pool.registerSensor(constants::tsensornames::cross_board, 0x49);
        temp_pool.registerSensor(constants::tsensornames::upconverter, 0x4D);

        WHEN("Temperatures has been polled")
        {
            temp_pool.pollSensors();

            THEN("Valid temperatures are obtained")
            {
                types::Temperature power_unit_t = temp_pool.temperature(constants::tsensornames::power_unit);
                types::Temperature modem_t = temp_pool.temperature(constants::tsensornames::modem);
                types::Temperature cross_board_t = temp_pool.temperature(constants::tsensornames::cross_board);
                types::Temperature upconverter_t = temp_pool.temperature(constants::tsensornames::upconverter);

                EXPECT(power_unit_t.is_valid);
                EXPECT(modem_t.is_valid);
                EXPECT(cross_board_t.is_valid);

                // Upconverter sensor presented only in Gotmic configuration
#ifdef GOTMIC
                EXPECT(upconverter_t.is_valid);
#else
                EXPECT_NOT(upconverter_t.is_valid);
#endif
            }
        }
    }
}


int test_main()
{
    std::vector<std::string> su{"-v"};

    if (int failures = lest::run(specification, su, std::cout))
        return failures;

    printf("All tests passed\n");
    return EXIT_SUCCESS;
}
