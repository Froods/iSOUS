#pragma once
#include "sensors.h"


CO2Sensor::CO2Sensor(I2C &i2c)
  :i2c_(i2c), address_(0x61);


CO2Sensor::read()
{
  this->readRegister(0x0300) //usikker på om det er den rigtige address

}



Temperatursensor::Temperatursensor(I2C &i2c)
  :i2c_(i2c), address_(0x61); //addresse fra datablad



Temperatursensor::read()
{
  this->readRegister(0x0300) //Addresse samme som CO2, da samme sensor.

}



LightSensor::LightSensor(I2C &i2c)
  :i2c_(i2c), address_(0x39);



LightSensor::read(){

}

WindowMotor::open();
