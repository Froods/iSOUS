#pragma once
#include "I2C.h"
#include <stdint.h>

// Command bit — skal OR'es på alle register-adresser
#define TSL2561_COMMAND_BIT (0x80)
#define TSL2561_WORD_BIT    (0x20)   // 1 = læs/skriv 16-bit word

// Registre
#define TSL2561_REG_CONTROL    0x00
#define TSL2561_REG_TIMING     0x01
#define TSL2561_REG_ID         0x0A
#define TSL2561_REG_CHAN0_LOW  0x0C
#define TSL2561_REG_CHAN1_LOW  0x0E

// Hardcoded konfiguration: gain 1x, integrationstid 402 ms
// (Bedst dynamisk rækkevidde til udendørs brug — undgår mætning i direkte sol)
#define TSL2561_TIMING_CONFIG  0x02   // gain=1x (bit 4=0), int=402ms (bits 1:0=10)

// Lux-beregning konstanter (fra TSL2561 datablad, T-package)
// Pre-skaleret til gain=1x og integrationstid=402ms
#define TSL2561_LUX_LUXSCALE   14
#define TSL2561_LUX_RATIOSCALE 9
#define TSL2561_LUX_K1T 0x0040
#define TSL2561_LUX_B1T 0x01f2
#define TSL2561_LUX_M1T 0x01be
#define TSL2561_LUX_K2T 0x0080
#define TSL2561_LUX_B2T 0x0214
#define TSL2561_LUX_M2T 0x02d1
#define TSL2561_LUX_K3T 0x00c0
#define TSL2561_LUX_B3T 0x023f
#define TSL2561_LUX_M3T 0x037b
#define TSL2561_LUX_K4T 0x0100
#define TSL2561_LUX_B4T 0x0270
#define TSL2561_LUX_M4T 0x03fe
#define TSL2561_LUX_K5T 0x0138
#define TSL2561_LUX_B5T 0x016f
#define TSL2561_LUX_M5T 0x01fc
#define TSL2561_LUX_K6T 0x019a
#define TSL2561_LUX_B6T 0x00d2
#define TSL2561_LUX_M6T 0x00fb
#define TSL2561_LUX_K7T 0x029a
#define TSL2561_LUX_B7T 0x0018
#define TSL2561_LUX_M7T 0x0012
#define TSL2561_LUX_K8T 0x029a
#define TSL2561_LUX_B8T 0x0000
#define TSL2561_LUX_M8T 0x0000


/**
 * @enum TSL2561_Status
 * @brief Returværdi for TSL2561 metoder.
 */
enum TSL2561_Status {
  TSL2561_OK,            ///< Operation lykkedes
  TSL2561_ID_ERROR,      ///< Chip ID register matcher ikke 0x5_ (forkert sensor)
  TSL2561_BUS_ERROR      ///< I2C transaktion fejlede
};


/**
 * @class TSL2561
 * @brief Driver til TSL2561 ambient light sensor via I2C.
 *
 * Hardkoded konfiguration: I2C-adresse 0x29, gain 1x, integrationstid 402 ms.
 * Disse valg er passende for udendørs brug, hvor lysniveau svinger fra natlys
 * til direkte sol uden risiko for mætning.
 *
 * Lux beregnes ud fra forholdet mellem broadband- (synligt + IR) og IR-kanal
 * vha. en stykkevis lineær approksimation fra TSL2561 datablad.
 */
class TSL2561 {
public:
  /**
   * @brief Konstruerer en TSL2561 instans.
   * @param i2c Reference til I2C bus objektet.
   */
  TSL2561(I2C &i2c, uint8_t addr = 0x29);

  /**
   * @brief Verificerer sensor og sætter timing/gain.
   *
   * Læser chip-ID-registret og tjekker at det matcher TSL2561 (øverste 4 bits = 0x5).
   * Skriver derefter timing-konfigurationen (gain 1x, integration 402 ms).
   *
   * @return TSL2561_OK ved succes, TSL2561_ID_ERROR ved chip-mismatch,
   *         TSL2561_BUS_ERROR ved I2C-fejl.
   */
  TSL2561_Status begin();

  /**
   * @brief Tænder sensor, læser begge kanaler, beregner lux, slukker sensor.
   * @return TSL2561_OK ved succes, TSL2561_BUS_ERROR ved I2C-fejl.
   */
  TSL2561_Status readData();

  /**
   * @brief Henter seneste beregnede lux-værdi fra readData().
   * @return Lys-intensitet i lux.
   */
  uint32_t getLux() const { return lux; }

private:
  I2C &I2C_BUS;
  uint32_t lux;
  uint8_t Adresse;
};
