#include "temperature_pool.h"

#include <vector>

#include "global_definitions/constants.h"
#include "drivers/i2c_bus.h"

/**
 * @brief TemperaturePool class obtains and holds temperature values from registered sensors
 */
TemperaturePool::TemperaturePool(I2cBus &i2c_bus) :
    i2c_bus_(i2c_bus)
{
}

/**
 * @brief Add sensor to the temperature pool
 *
 * @param[in] name Name of sensor
 * @param[in] addr Address of sensor
 * @pre Sensor with a name `name` is not in the pool
 * @post Sensor with a name `name` and address `addr` is added to the pool
 * @return true if sensor was added to the pool, false otherwise
 */
bool TemperaturePool::registerSensor(const std::string &name, std::uint8_t addr)
{
    return temperature_sensors_.emplace(name, TemperatureSensorData {addr}).second;
}

/**
 * @brief Remove sensor from the temperature pool
 *
 * @param[in] name Name of sensor
 * @post There is no sensor with a name `name` in the pool
 * @return true if sensor was removed from the pool, false otherwise
 */
bool TemperaturePool::unregisterSensor(const std::string &name)
{
    return (temperature_sensors_.erase(name) == 1) ? true : false;
}

/**
 * @brief Read temperature values from all sensors in the pool
 *
 * If I2C bus failed to obtain temperature value, invalid `Temperature` will
 * be stored for this sensor
 */
void TemperaturePool::pollSensors()
{
    for (auto &ts: temperature_sensors_)
    {
        try
        {
            std::vector<std::uint8_t> buf = i2c_bus_.readBuf(ts.second.address, 2);

            // Combine 10-bit value from buf
            int adc_val = ((buf[0] & 0x7F) << 2) + (buf[1] >> 6);

            // If sign bit is 1 (negative temp)
            if (buf[0] & 0x80)
                adc_val -= 512;

            ts.second.temperature = types::Temperature(static_cast<float>(adc_val) / 4.0f);
        }
        catch (const std::runtime_error &e)
        {
            ts.second.temperature = types::Temperature();

			// TODO: send error message to system log
        }
    }
}

/**
 * @brief Get last known temperature value for sensor with name `name`
 *
 * @param[in] name Name of sensor
 * @pre Sensor with name `name` is in the pool
 * @post Temperature value for requested sensor is returned
 * @return Temperature value
 */
types::Temperature TemperaturePool::temperature(const std::string &name) const
{
    auto ts = temperature_sensors_.find(name);
    if (ts != temperature_sensors_.end())
        return ts->second.temperature;

    return types::Temperature();
}
