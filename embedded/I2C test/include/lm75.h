#pragma once
#include "i2c.h"
#include <stdint.h>

// LM75 register map
#define LM75_REG_TEMP   0x00   // Temperatur (read-only, 16-bit)
#define LM75_REG_CONFIG 0x01   // Configuration (8-bit)
#define LM75_REG_THYST  0x02   // Hysterese threshold (16-bit)
#define LM75_REG_TOS    0x03   // Over-temp shutdown threshold (16-bit)

/**
 * @brief Status koder returneret af LM75 metoder.
 */
enum LM75_Status {
    LM75_OK,
    LM75_BUS_ERROR
};

/**
 * @brief Driver for LM75 I2C temperatur sensor.
 *
 * LM75 har op til 8 mulige adresser (0x48-0x4F), styret af A0/A1/A2 pins.
 * Default base-adresse er 0x48 (alle adresse-pins til GND).
 *
 * Forventer at I2C-bussen er initialiseret (init_I2C) FØR readData() kaldes.
 */
class LM75 {
public:
    /**
     * @param i2c     Reference til I2C bus objektet.
     * @param address 7-bit I2C adresse. Default 0x48 (A0=A1=A2=GND).
     *                Kan være 0x48-0x4F afhængigt af adresse-pins.
     */
    LM75(I2C &i2c, uint8_t address = 0x4D);

    /**
     * @brief Læser temperatur-registret og opdaterer interne attributter.
     * @return LM75_OK ved succes, LM75_BUS_ERROR ved I2C-fejl.
     */
    LM75_Status readData();

    /**
     * @brief Henter senest læste temperatur som rå 9-bit værdi.
     *        Hver enhed = 0.5°C. F.eks. 50 = 25.0°C.
     */
    int16_t getTempRaw() const { return tempRaw; }

    /**
     * @brief Henter senest læste temperatur i grader Celsius.
     */
    float getTempC() const { return tempRaw * 0.5f; }

private:
    I2C &I2C_BUS;
    const uint8_t Adresse;
    int16_t tempRaw;   // signeret, 0.5°C per enhed
};