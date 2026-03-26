#pragma once
#include "i2c.h"
#include <stdint.h>


/**
 * @enum SCD30_Status
 * @brief Returværdi for SCD30::readData().
 *
 * Bruges til at indikere om en sensor-aflæsning lykkedes,
 * eller hvad der gik galt.
 */
enum SCD30_Status 
{
  SCD30_OK,
  SCD30_TIMEOUT,
  SCD30_CRC_ERROR
};

/**
  * @class SCD30
  * @brief Håndterer kommunikation med SCD30 CO2 sensoren
  * 
  * Klassen opdaterer sine attributter (CO2 og temperatur), når readData bliver kaldt.
  * For at få temperatur/CO2 målingerne, bruges 'getters'
  *
  */
class SCD30 {
public:
  /**
  * @brief constructor. Starter kontinuerlig måling i sensoren
  * @param I2C_BUS reference til I2C-bussen. Addresse er hardcoded, da vi har én sensor
  */
  SCD30(I2C &i2c);
  
  /**
  *@brief Opdaterer temperatur og CO2 med nyeste målinger
  */
  SCD30_Status readData();

  /**
  * @brief getter CO2
  * @return returnerer CO2 fra seneste readData();
  */
  float getCO2();

  /**
  * @brief getter Temperature
  * @return returnerer temperature fra seneste readData();
  */
  float getTemperature();

private:
  float CO2;
  float temperature;
  I2C &I2C_BUS;
  uint8_t Adresse = 0x61;
};




