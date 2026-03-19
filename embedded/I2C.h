#pragma once
#include <stdint.h>

class I2C {
public:
  void init_I2C(uint32_t clockFreq);
  void start();
  void stop();
  void write(unsigned char data);
  unsigned char read(unsigned char isLast);
  void i2c_stop();
};
