/**
 * Test program for new TemperaturePool class based on I2cBus
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

#include <predef.h>
#include <startnet.h>
#include <autoupdate.h>

#ifdef ENABLE_UNIT_TESTING
# include "tests/unit_tests.h"
#endif
#include "global_definitions/constants.h"
#include "global_definitions/variables.h"
#include "web_server/http_handler.h"
#include "drivers/i2c_bus.h"
#include "system/temperature_pool.h"

extern "C"
{
void UserMain(void *pd);
}

const std::string app_name = "test_temperature_pool_" + constants::app_version;
const char *AppName = app_name.c_str();

void UserMain(void *pd)
{
    std::cout << "\nApplication: " << AppName << "\n" << std::endl;

    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();

#ifndef ENABLE_UNIT_TESTING
    std::unique_ptr<I2cBus> i2c_bus;
    try
    {
		// Use I2C module 1
        i2c_bus = std::make_unique<I2cBus>(1);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Failed to create i2c_bus : " << e.what() << ". Terminating" << std::endl;
        return;
    }

    g_vars::temp_pool = new TemperaturePool(*i2c_bus);
    g_vars::temp_pool->registerSensor(constants::tsensornames::power_unit, 0x4A);
    g_vars::temp_pool->registerSensor(constants::tsensornames::modem, 0x4E);
    g_vars::temp_pool->registerSensor(constants::tsensornames::cross_board, 0x49);
    g_vars::temp_pool->registerSensor(constants::tsensornames::upconverter, 0x4D);

    auto printTemperature = [&] (const std::string &name) -> std::string {
        std::ostringstream oss;
        oss << name << " sensor: ";

        types::Temperature t = g_vars::temp_pool->temperature(name);
        if (t.is_valid)
            oss << std::fixed << std::setprecision(2) << t.value << " C";
        else
            oss << "no connection";

        return oss.str();
    };

    StartHTTP();
    registerGet();
    registerPost();

    while (1)
    {
        g_vars::temp_pool->pollSensors();

        std::cout << printTemperature(constants::tsensornames::power_unit) << "\n";
        std::cout << printTemperature(constants::tsensornames::modem) << "\n";
        std::cout << printTemperature(constants::tsensornames::cross_board) << "\n";
        std::cout << printTemperature(constants::tsensornames::upconverter) << "\n";
        std::cout << std::endl;

        OSTimeDly(TICKS_PER_SECOND);
    }
#else
    std::cout << "\nStarting tests...\n" << std::endl;

    test_main();
    while (1)
        OSTimeDly(TICKS_PER_SECOND);
#endif // ENABLE_UNIT_TESTING
}
