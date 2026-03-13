#pragma once
#include "i2c.h"
#include "i2csensor.h"
#include <stdint.h>

class CO2Sensor : public I2CSensor {
public:
  CO2Sensor(I2C &i2c);
  uint16_t read(); // returns ppm
};

class TemperatureSensor : public I2CSensor {
public:
  TemperatureSensor(I2C &i2c);
  float read(); // returns celcius
};

class LightSensor : public I2CSensor {
public:
  LightSensor(I2C &i2c);
  uint16_t read(); // returns lux
};
