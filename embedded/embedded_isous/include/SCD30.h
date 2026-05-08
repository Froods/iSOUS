#pragma once
#include "I2C.h"
#include <stdint.h>

/**
 * @enum SCD30_Status
 * @brief Returværdi for SCD30 metoder.
 *
 * Bruges til at indikere om en sensor-aflæsning lykkedes,
 * eller hvad der gik galt.
 */
enum SCD30_Status
{
  SCD30_OK,         ///< Måling modtaget og CRC valideret
  SCD30_TIMEOUT,    ///< Sensor returnerede ikke data inden for timeout (~5 sek)
  SCD30_CRC_ERROR,  ///< CRC validering fejlede på modtagne data
  SCD30_BUS_ERROR   ///< I2C kommunikationsfejl
};

/**
 * @class SCD30
 * @brief Håndterer kommunikation med SCD30 CO2 sensoren.
 *
 * Klassen opdaterer sine interne attributter (CO2 og temperatur) når
 * readData() kaldes. Værdierne hentes efterfølgende via getters.
 */
class SCD30 {
public:
  /**
   * @brief Opretter en SCD30 instans.
   * @param i2c Reference til I2C-bussen. Adressen er hardcoded (0x61),
   *            da der kun understøttes én SCD30 per bus.
   */
  SCD30(I2C &i2c);

  /**
   * @brief Starter kontinuerlig måling på sensoren.
   *
   * Sender kommando 0x0010 med trykargument 0x0000 (ingen trykkomensation).
   * Returnerer tidligt uden fejlmelding hvis I2C-kommunikationen fejler.
   */
  void begin();

  /**
   * @brief Poller sensor for data-ready og læser CO2 + temperatur.
   *
   * Venter i op til ~5 sekunder (50 forsøg á 100 ms) på at sensoren
   * melder data klar. Validerer alle CRC-checksums før data gemmes.
   *
   * @return SCD30_OK ved succes, SCD30_TIMEOUT hvis sensoren ikke svarer,
   *         SCD30_CRC_ERROR ved checksum-fejl, SCD30_BUS_ERROR ved I2C-fejl.
   */
  SCD30_Status readData();

  /**
   * @brief Returnerer CO2-koncentration fra seneste readData().
   * @return CO2 i ppm som 32-bit float.
   */
  float getCO2();

  /**
   * @brief Returnerer temperatur fra seneste readData().
   * @return Temperatur i grader Celsius som 32-bit float.
   */
  float getTemperature();

private:
  float CO2 = 0.0f;
  float temperature = 0.0f;
  I2C &I2C_BUS;
  uint8_t Adresse = 0x61;
};




