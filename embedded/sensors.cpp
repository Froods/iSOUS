#pragma once
#include "sensors.h"

/**
*@brief Initializer CO2Sensor objektet, med sin hardcodede addresse
*
*@param i2c Det tidligere lavede I2C objekt, som alle sensorerne bruger
*
*/
CO2Sensor::CO2Sensor(I2C &i2c)
  :i2c_(i2c), address_(0x61);


/**
*@brief Når kaldt returneres sensormålingerne
*
* Data bliver modtaget i følgende rækkefølge:
* Byte1: CO2 concentration MMSB
* Byte2: CO2 concentration MLSB
* Byte3: CRC
* Byte4: CO2 concentration LMSB
* Byte5: CO2 concentration LLSB
* Byte6: CRC
* Byte7: Temperature MMSB
* Byte8: Temperature MLSB
* Byte9: CRC
* Byte10: Temperature LMSB
* Byte11: Temperature LLSB
* Byte12: CRC
* Byte13: Humidity MMSB
* Byte14: Humidity MLSB
* Byte15: CRC
* Byte16: Humidity LMSB
* Byte17: Humidity LLSB
* Byte18: CRC
*
*/
CO2Sensor::read()
{
  this->readRegister(0x0300) //usikker på om det er den rigtige address

}


/**
*@brief Initializer CO2Sensor objektet, med sin hardcodede addresse
*
*@param i2c Det tidligere lavede I2C objekt, som alle sensorerne bruger
*
*/
Temperatursensor::Temperatursensor(I2C &i2c)
  :i2c_(i2c), address_(0x61); //addresse fra datablad


/**
*@brief Når kaldt returneres sensormålingerne
*
* Data bliver modtaget i følgende rækkefølge:
* Byte1: CO2 concentration MMSB
* Byte2: CO2 concentration MLSB
* Byte3: CRC
* Byte4: CO2 concentration LMSB
* Byte5: CO2 concentration LLSB
* Byte6: CRC
* Byte7: Temperature MMSB
* Byte8: Temperature MLSB
* Byte9: CRC
* Byte10: Temperature LMSB
* Byte11: Temperature LLSB
* Byte12: CRC
* Byte13: Humidity MMSB
*
* Byte14: Humidity MLSB
* Byte15: CRC
* Byte16: Humidity LMSB
* Byte17: Humidity LLSB
* Byte18: CRC
*
*/
Temperatursensor::read()
{
  this->readRegister(0x0300) //Addresse samme som CO2, da samme sensor.

}

/**
*@brief Initializer CO2Sensor objektet, med sin hardcodede addresse
*
*@param i2c Det tidligere lavede I2C objekt, som alle sensorerne bruger
*
*/

LightSensor::LightSensor(I2C &i2c)
  :i2c_(i2c), address_(0x39);


/**
*@brief Returnerer lysmålingerne når kaldt
*
*
*/
LightSensor::read(){

}

WindowMotor::open();
