#include "i2csensor.h"
#include <cstdint>

uint8_t I2CSensor::readRegister(uint8_t reg) {
  i2c_.start();
  i2c_.write(address_ << 1); // addresse, og write bit da LSB er 0 = write
  i2c_.write(reg);           // register som skal læses
  i2c_.start();
  i2c_.write((address_ << 1) | 1);
  uint8_t result = i2c_.read(true); // true = is last byte, send NACK
  i2c_.stop();
  return result;
}

void I2CSensor::writeRegister(uint8_t reg, uint8_t data) {
  i2c_.start();
  i2c_.write(address << 1);
  i2c_.write(reg);
  i2c_.write(data);
  i2c_.stop();
}
