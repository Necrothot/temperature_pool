#include "i2c_bus.h"

#include <cstdio>
#include <iomanip>
#include <sstream>

#include <multichanneli2c.h>

#include "utils/lock_guard.h"

std::set<int> I2cBus::s_registered_modules_;

/**
 * @brief Class for communicating over Netburner I2C bus
 *
 * Throws exception if `module_num` is already taken by other I2cBus instance
 *
 * @exception std::runtime_error Requested module is already registered
 */
I2cBus::I2cBus(int module_num)
{
    auto i = s_registered_modules_.insert(module_num);
    if (!i.second)
        throw std::runtime_error("Module already in use");

    OSSemInit(&i2c_sem_, 1);

    module_num_ = module_num;
    MultiChannel_I2CInit(module_num_);
}

/**
 * Removes current module from registered modules list
 */
I2cBus::~I2cBus()
{
    s_registered_modules_.erase(module_num_);
}

/**
 * @brief Send data to slave
 *
 * Sends `buf` to slave with address `addr` on `module_num_` bus
 *
 * @param[in] addr Slave address
 * @param[in] buf Vector of bytes that will be sent
 * @pre `addr` is an address of valid slave device
 * @pre `buf` is not empty
 * @post `buf` is sent to device with address `addr`
 * @exception std::runtime_error Failed to write to slave
 */
void I2cBus::sendBuf(uint8_t addr, std::vector<uint8_t> &buf)
{
    SemLockGuard<> lockGuard(i2c_sem_);

    BYTE status = MultiChannel_I2CSendBuf(module_num_, addr, buf.data(), buf.size(), true);
    if (status > I2C_MASTER_OK)
        throw std::runtime_error("I2C write error");
}

/**
 * @brief Read data from slave
 *
 * Reads `num` bytes from slave with address `addr` on `module_num_` bus
 *
 * @param[in] addr Slave address
 * @param[in] num The number of bytes to read
 * @pre `addr` is an address of valid slave device
 * @pre `num` is a positive integer
 * @post Returned vector contains received bytes and its size is equal to `num`
 * @exception std::runtime_error Failed to read from slave
 * @return Vector of received bytes
 */
std::vector<uint8_t> I2cBus::readBuf(uint8_t addr, int num)
{
    std::vector<uint8_t> buf(num);

    SemLockGuard<> lockGuard(i2c_sem_);

    BYTE status = MultiChannel_I2CReadBuf(module_num_, addr, buf.data(), num, true);
    if (status > I2C_MASTER_OK)
        throw std::runtime_error("I2C read error");

    return buf;
}

/**
 * @brief Print all available devices on `module_num_`
 *
 * Iterates through range of slave addresses (0x01-0x80) on the bus
 * and sends 1 byte. If ACK is received from the slave device, then
 * this device address is printed out
 */
void I2cBus::scan()
{
    SemLockGuard<> lockGuard(i2c_sem_);

    for (uint32_t addr = 0x01u; addr < 0x80u; addr++)
    {
        BYTE status = MultiChannel_I2CStart(module_num_, addr, I2C_START_WRITE);
        if (status < I2C_TIMEOUT)
            printf("Found device at addr 0x%02X\n", static_cast<uint8_t>(addr));

        MultiChannel_I2CStop(module_num_);
    }
}
