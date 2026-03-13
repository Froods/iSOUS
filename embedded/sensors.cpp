#pragma once
#include "sensors.h"

//initializer CO2Sensoren med I2C objektet. Addresse er hardcoded
CO2Sensor::CO2Sensor(I2C &i2c)
  :i2c_(i2c), address_(0x61); //addresse fra datablad

CO2Sensor::read()
{
  this->readRegister(0x0038) //usikker på om det er den rigtige address

}

