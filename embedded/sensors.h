#pragma once
#include "i2c.h"
#include <stdint.h>

class CO2Sensor : public I2CSensor {
public:
  CO2Sensor(I2C &i2c);
  uint16_t read(); // returns ppm
private:
  I2C &I2C_BUS; 
  uint8_t Adresse;
};

class TemperatureSensor : public I2CSensor {
public:
  TemperatureSensor(I2C &i2c);
  uint16_t read(); // returns celcius
private:
  I2C &I2C_BUS; 
  uint8_t Adresse;
};

class LightSensor :{
public:
  LightSensor(I2C &i2c);
  uint16_t read(); // returns lux
private:
  I2C &I2C_BUS; 
  uint8_t Adresse;
};
