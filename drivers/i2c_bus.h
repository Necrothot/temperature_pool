#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <vector>
#include <set>

#include <ucos.h>

class I2cBus
{
public:
    I2cBus(int module_num);
    ~I2cBus();

    void sendBuf(uint8_t addr, std::vector<uint8_t> &buf);
    std::vector<uint8_t> readBuf(uint8_t addr, int num);

    void scan();

private:
    I2cBus(const I2cBus &) = delete;
    I2cBus & operator = (const I2cBus &) = delete;

    // List of registered I2C modules
    static std::set<int> s_registered_modules_;

    // I2C module number
    int module_num_;

    // Semaphore for controlling I2C bus access
    OS_SEM i2c_sem_;

};

#endif // I2C_BUS_H
