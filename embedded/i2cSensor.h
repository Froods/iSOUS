#pragma once
#include "i2c.h"
#include <stdint.h>

class I2CSensor {
public:
  I2CSensor(I2C &i2c, uint8_t address) : i2c_(i2c), address_(address) {};
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t data);

protected:
  I2C &i2c_;
  uint8_t address_;
}
