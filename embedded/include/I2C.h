#pragma once
#include <stdint.h>

/**
 * @enum I2C_Status
 * @brief Returværdi for I2C start/write metoder.
 *
 * Bruges til at indikere om der sker fejl på I2C bussen,
 * eller hvad der gik galt.
 */
enum I2C_Status {
  I2C_OK,           ///< Operation gennemført uden fejl
  I2C_START_ERROR,  ///< Start condition fejlede (ugyldig busstatus)
  I2C_ADDR_NACK,    ///< Adresse sendt, NACK modtaget — slave svarede ikke
  I2C_DATA_NACK,    ///< Data byte sendt, NACK modtaget — slave afviste data
  I2C_BUS_ERROR,    ///< Busfejl (TWI status 0x00 eller ukendt tilstand)
  I2C_TIMEOUT       ///< Hardware svarede ikke inden for timeout
};

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
   * @brief Forsøger at befri en fastlåst I2C bus.
   *
   * Deaktiverer TWI-hardwaren og klocker manuelt op til 9 gange på SCL
   * for at frigøre en slave der holder SDA lav. Afslutter med en manuel
   * stop condition og genaktiverer TWI.
   */
  void recoverbus();

  /**
   * @brief Sender start condition og venter på at TWI er klar.
   * @return I2C_OK ved succes, I2C_START_ERROR ved ugyldig busstatus,
   *         I2C_BUS_ERROR ved timeout.
   */
  I2C_Status start();

  /**
   * @brief Sender stop condition og frigør bussen.
   *
   * Venter på at TWSTO-bit cleares. Hvis SDA stadig holdes lav af en slave
   * efter stop, udføres manuel bus-recovery med SCL-pulsering.
   */
  void stop();

  /**
   * @brief Skriver en byte på bussen og venter på ACK/NACK.
   * @param data Byte som skal sendes.
   * @return I2C_OK ved succes, I2C_ADDR_NACK hvis slave ikke svarede på adresse,
   *         I2C_DATA_NACK hvis slave afviste data, I2C_BUS_ERROR ved busfejl,
   *         I2C_TIMEOUT hvis hardware ikke svarede inden for timeout.
   */
  I2C_Status write(uint8_t data);

  /**
   * @brief Læser en byte fra bussen.
   * @param isLast 1 = send NACK (sidste byte), 0 = send ACK (flere bytes følger).
   * @return Den læste byte, eller 0 ved timeout.
   */
  uint8_t read(uint8_t isLast);
};
