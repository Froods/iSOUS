#pragma once
#include "i2c.h"
#include <stdint.h>



//command bits
#define TSL2561_COMMAND_BIT (0x80) ///< Must be 1
#define TSL2561_CLEAR_BIT \
  (0x40) ///< Clears any pending interrupt (write 1 to clear)
#define TSL2561_WORD_BIT (0x20)  ///< 1 = read/write word (rather than byte)
#define TSL2561_BLOCK_BIT (0x10) ///< 1 = using block read/write

#define TSL2561_CONTROL_POWERON (0x03) ///< Control register setting to turn on
#define TSL2561_CONTROL_POWEROFF \ (0x00) ///< Control register setting to turn off

/** TSL2561 I2C Registers */
enum {
  TSL2561_REGISTER_CONTROL = 0x00,          // Control/power register
  TSL2561_REGISTER_TIMING = 0x01,           // Set integration time register
  TSL2561_REGISTER_THRESHHOLDL_LOW = 0x02,  // Interrupt low threshold low-byte
  TSL2561_REGISTER_THRESHHOLDL_HIGH = 0x03, // Interrupt low threshold high-byte
  TSL2561_REGISTER_THRESHHOLDH_LOW = 0x04,  // Interrupt high threshold low-byte
  TSL2561_REGISTER_THRESHHOLDH_HIGH =
      0x05,                           // Interrupt high threshold high-byte
  TSL2561_REGISTER_INTERRUPT = 0x06,  // Interrupt settings
  TSL2561_REGISTER_CRC = 0x08,        // Factory use only
  TSL2561_REGISTER_ID = 0x0A,         // TSL2561 identification setting
  TSL2561_REGISTER_CHAN0_LOW = 0x0C,  // Light data channel 0, low byte
  TSL2561_REGISTER_CHAN0_HIGH = 0x0D, // Light data channel 0, high byte
  TSL2561_REGISTER_CHAN1_LOW = 0x0E,  // Light data channel 1, low byte
  TSL2561_REGISTER_CHAN1_HIGH = 0x0F  // Light data channel 1, high byte
};


/**
 * @brief Driver for the TSL2561 ambient light sensor.
 *
 * Oversigt over registre (Fra TSL2561 datablad):
 * - 0x00 Control register (power on/off)
 * - 0x01 Timing register (integration time + gain)
 * - 0x0A ID register (chip identification, returnerer 0x50 for T-package)
 * - 0x0C/0x0D Channel 0 (broadband, synligt + IR lys)
 * - 0x0E/0x0F Channel 1 (IR-only)
 *
 * Alle register-adgange skal OR'es med command-bit (0x80).
 * Word-reads (16-bit) skal også OR'es med word-bit (0x20).
 */


/**
 * @brief Status koder returneret af TSL2561 metoder.
 */
enum TSL2561_Status {
  TSL2561_OK,            ///< Operation lykkedes
  TSL2561_ID_ERROR,      ///< Chip ID register matcher ikke forventet værdi (0x50)
  TSL2561_BUS_ERROR      ///< I2C transaktion fejlede
};


/**
 * @brief Gain (forstærknings) indstillinger for TSL2561.
 *
 * Højere gain (16x) giver bedre følsomhed i svagt lys, men risikerer
 * mætning ved stærkt lys.
 */
enum TSL2561_Gain {
  TSL2561_GAIN_1X  = 0x00,  ///< Ingen forstærkning (godt til kraftigt lys)
  TSL2561_GAIN_16X = 0x10   ///< 16x forstærkning (godt til svagt lys)
};


/**
 * @brief Integrationstid for TSL2561.
 *
 * Længere integrationstid giver mere præcise målinger, men tager længere tid
 * at gennemføre. Påvirker også scaling-konstanter i lux-beregningen.
 */
enum TSL2561_IntegrationTime {
  TSL2561_INT_13MS  = 0x00,  ///< 13.7 ms integration
  TSL2561_INT_101MS = 0x01,  ///< 101 ms integration
  TSL2561_INT_402MS = 0x02   ///< 402 ms integration (bedst præcision)
};


/**
 * @brief Klasse til at læse data fra TSL2561 lys-sensoren via I2C.
 *
 * Sensoren har to ADC-kanaler:
 *  - Channel 0 (broadband): synligt lys + infrarødt
 *  - Channel 1 (IR): kun infrarødt
 *
 * Lux beregnes ud fra forholdet mellem de to kanaler vha. en stykkevis
 * lineær approksimation (Se TSL2561 datablad).
 */
class TSL2561 {
public:
  /**
   * @brief Konstruerer en TSL2561 instans.
   *
   * @param i2c     Reference til I2C bus objektet.
   * @param address 7-bit I2C adresse for sensoren. Default 0x39 (ADDR pin floating).
   *                Brug 0x29 hvis ADDR er pulled low, 0x49 hvis pulled high.
   */
  TSL2561(I2C &i2c, uint8_t address = 0x39);


  /**
   * @brief Initialiserer sensoren.
   *
   * Verificerer kommunikation ved at læse chip-ID registret, og konfigurerer
   * sensoren med default gain og integration time.
   *
   * @return TSL2561_OK hvis init lykkedes,
   *         TSL2561_ID_ERROR hvis chip-ID ikke matcher,
   *         TSL2561_BUS_ERROR hvis I2C-kommunikation fejlede.
   */
  TSL2561_Status begin();


  /**
   * @brief Læser begge ADC kanaler fra sensoren og beregner lux.
   *
   * Sensoren tændes, der ventes på at integrationen er færdig, og begge
   * kanaler læses. Lux beregnes og gemmes i interne attributter, hvorefter
   * sensoren slukkes igen for at spare strøm.
   *
   * Værdierne kan efterfølgende hentes med getLux(), getBroadband() og getIR().
   *
   * @return TSL2561_OK ved succes, TSL2561_BUS_ERROR ved I2C-fejl.
   */
  TSL2561_Status readData();


  /**
   * @brief Henter den senest beregnede lux-værdi.
   *
   * @return Lys-intensitet i lux. Returnerer 0 hvis sensoren er mættet
   *         eller readData() endnu ikke er kaldt.
   */
  uint32_t getLux();


  /**
   * @brief Henter den seneste råværdi fra broadband kanalen (synligt + IR).
   *
   * @return 16-bit ADC værdi fra channel 0.
   */
  uint16_t getBroadband();


  /**
   * @brief Henter den seneste råværdi fra IR-kanalen.
   *
   * @return 16-bit ADC værdi fra channel 1.
   */
  uint16_t getIR();


private:
  I2C &I2C_BUS;             ///< Reference til I2C bus
  const uint8_t Adresse;    ///< 7-bit I2C adresse for sensoren

  TSL2561_Gain gain;                       ///< Aktuel gain indstilling
  TSL2561_IntegrationTime integrationTime; ///< Aktuel integrationstid

  uint16_t broadband;  ///< Senest læste broadband ADC-værdi
  uint16_t ir;         ///< Senest læste IR ADC-værdi
  uint32_t lux;        ///< Senest beregnede lux-værdi


  /**
   * @brief Skriver en 8-bit værdi til et register på sensoren.
   *
   * Command-bit (0x80) tilføjes automatisk til register-adressen.
   *
   * @param reg   Register adresse (uden command-bit).
   * @param value 8-bit værdi der skal skrives.
   */
  void writeRegister(uint8_t reg, uint8_t value);


  /**
   * @brief Læser en 8-bit værdi fra et register på sensoren.
   *
   * Command-bit (0x80) tilføjes automatisk til register-adressen.
   *
   * @param reg Register adresse (uden command-bit).
   * @return    Læst 8-bit værdi.
   */
  uint8_t readRegister8(uint8_t reg);


  /**
   * @brief Læser en 16-bit værdi (to bytes, little-endian) fra et register.
   *
   * Command-bit (0x80) og word-bit (0x20) tilføjes automatisk for at
   * fortælle sensoren at vi vil læse 2 bytes ad gangen.
   *
   * @param reg Register adresse (uden command/word-bits).
   * @return    Læst 16-bit værdi.
   */
  uint16_t readRegister16(uint8_t reg);


  /**
   * @brief Tænder sensoren (skriver 0x03 til control-registret).
   */
  void enable();


  /**
   * @brief Slukker sensoren for at spare strøm (skriver 0x00 til control-registret).
   */
  void disable();


  /**
   * @brief Beregner lux ud fra de to ADC kanaler.
   *
   * Implementering taget fra TSL2561 datablad og Adafruit's referencedriver.
   * Bruger en stykkevis lineær approksimation baseret på forholdet mellem
   * IR og broadband kanalerne, samt gain og integrationstid.
   *
   * @param broadband ADC-værdi fra channel 0 (synligt + IR).
   * @param ir        ADC-værdi fra channel 1 (kun IR).
   * @return          Beregnet lys-intensitet i lux. Returnerer 65536 ved mætning.
   */
  uint32_t calculateLux(uint16_t broadband, uint16_t ir);
};