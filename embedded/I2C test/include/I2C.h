#pragma once
#include <stdint.h>

/**
 * @class I2C
 * @brief Low-level I2C (TWI) driver til ATmega2560.
 *
 * Abstraherer start, stop, write og read operationer
 * på TWI-bussen.
 */
class I2C {
public:
   /**
   * @brief Initialiserer I2C bussen.
   * @param clockFreq Clock-frekvens i Hz (f.eks. 100000 for 100kHz).
   */
  void init_I2C(uint32_t clockFreq = 100000);
  
  /**
  * @brief Sender start condition
  */
  void start();

  /**
  * @brief Sender stop condition
  */
  void stop();

  /**
  * @brief Skriver en byte på bussen
  * @param data Byte som skal sendes
  */
  void write(uint8_t data);

  /**
  * @brief Læser en byte fra bussen
  * @param isLast 1 = NACK (sidste byte), 0 = ACK (flere bytes følger).
  * @return Den læste byte
  */
  uint8_t read(uint8_t isLast);
};
