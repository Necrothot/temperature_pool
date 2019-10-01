#ifndef TEMPERATURE_POOL_H
#define TEMPERATURE_POOL_H

#include <string>
#include <map>

#include "global_definitions/types.h"

class I2cBus;

class TemperaturePool
{
public:
    explicit TemperaturePool(I2cBus &i2c_bus);

    bool registerSensor(const std::string &name, std::uint8_t addr);
    bool unregisterSensor(const std::string &name);

    void pollSensors();

    types::Temperature temperature(const std::string &name) const;

private:
    struct TemperatureSensorData
    {
        // Address on I2C bus
        std::uint8_t address;

        // Last known value from sensor
        types::Temperature temperature;
    };

    // first - User-friendly sensor name (usually indicates sensor's location)
    std::map<std::string, TemperatureSensorData> temperature_sensors_;

    // Reference to I2cBus that will be used for communication
    I2cBus &i2c_bus_;

};

#endif // TEMPERATURE_POOL_H
