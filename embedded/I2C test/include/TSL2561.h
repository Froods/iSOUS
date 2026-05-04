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
#define TSL2561_CONTROL_POWEROFF  (0x00) ///< Control register setting to turn off

//skaleringer for lux afhænging af integrationstid (til beregning af lux)
#define TSL2561_LUX_LUXSCALE (14)          ///< Scale by 2^14
#define TSL2561_LUX_RATIOSCALE (9)         ///< Scale ratio by 2^9
#define TSL2561_LUX_CHSCALE (10)           ///< Scale channel values by 2^10
#define TSL2561_LUX_CHSCALE_TINT0 (0x7517) ///< 322/11 * 2^TSL2561_LUX_CHSCALE
#define TSL2561_LUX_CHSCALE_TINT1 (0x0FE7) ///< 322/81 * 2^TSL2561_LUX_CHSCALE


// T, FN and CL package values
#define TSL2561_LUX_K1T (0x0040) ///< 0.125 * 2^RATIO_SCALE
#define TSL2561_LUX_B1T (0x01f2) ///< 0.0304 * 2^LUX_SCALE
#define TSL2561_LUX_M1T (0x01be) ///< 0.0272 * 2^LUX_SCALE
#define TSL2561_LUX_K2T (0x0080) ///< 0.250 * 2^RATIO_SCALE
#define TSL2561_LUX_B2T (0x0214) ///< 0.0325 * 2^LUX_SCALE
#define TSL2561_LUX_M2T (0x02d1) ///< 0.0440 * 2^LUX_SCALE
#define TSL2561_LUX_K3T (0x00c0) ///< 0.375 * 2^RATIO_SCALE
#define TSL2561_LUX_B3T (0x023f) ///< 0.0351 * 2^LUX_SCALE
#define TSL2561_LUX_M3T (0x037b) ///< 0.0544 * 2^LUX_SCALE
#define TSL2561_LUX_K4T (0x0100) ///< 0.50 * 2^RATIO_SCALE
#define TSL2561_LUX_B4T (0x0270) ///< 0.0381 * 2^LUX_SCALE
#define TSL2561_LUX_M4T (0x03fe) ///< 0.0624 * 2^LUX_SCALE
#define TSL2561_LUX_K5T (0x0138) ///< 0.61 * 2^RATIO_SCALE
#define TSL2561_LUX_B5T (0x016f) ///< 0.0224 * 2^LUX_SCALE
#define TSL2561_LUX_M5T (0x01fc) ///< 0.0310 * 2^LUX_SCALE
#define TSL2561_LUX_K6T (0x019a) ///< 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6T (0x00d2) ///< 0.0128 * 2^LUX_SCALE
#define TSL2561_LUX_M6T (0x00fb) ///< 0.0153 * 2^LUX_SCALE
#define TSL2561_LUX_K7T (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7T (0x0018) ///< 0.00146 * 2^LUX_SCALE
#define TSL2561_LUX_M7T (0x0012) ///< 0.00112 * 2^LUX_SCALE
#define TSL2561_LUX_K8T (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8T (0x0000) ///< 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8T (0x0000) ///< 0.000 * 2^LUX_SCALE




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
  TSL2561(I2C &i2c, uint8_t address = 0x29);


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