#include "LM75.h"

LM75::LM75(I2C &i2c, uint8_t address)
    : I2C_BUS(i2c), Adresse(address), tempRaw(0)
{}

LM75_Status LM75::readData() {
    I2C_Status s;

    // Fase 1: Pege på temperatur-registret (0x00).
    // Strengt taget er det default ved power-on, men vi sætter det
    // eksplicit så driveren er robust selvom andre registre er læst.
    if ((s = I2C_BUS.start())             != I2C_OK) return LM75_BUS_ERROR;
    if ((s = I2C_BUS.write(Adresse << 1)) != I2C_OK) return LM75_BUS_ERROR;  // SLA+W
    if ((s = I2C_BUS.write(LM75_REG_TEMP)) != I2C_OK) return LM75_BUS_ERROR;

    // Fase 2: Repeated start + read 2 bytes (MSB først, så LSB).
    if ((s = I2C_BUS.start())                  != I2C_OK) return LM75_BUS_ERROR;
    if ((s = I2C_BUS.write((Adresse << 1) | 1)) != I2C_OK) return LM75_BUS_ERROR; // SLA+R

    uint8_t msb = I2C_BUS.read(0);   // ACK, mere kommer
    uint8_t lsb = I2C_BUS.read(1);   // NACK, sidste byte
    I2C_BUS.stop();

    // LM75 leverer 9-bit signeret temperatur i de øverste 9 bits af
    // de 16 modtagne bits. Højre-shift med 7 giver os rå 0.5°C-enheder.
    // Vi caster til int16_t FØR shift for at bevare fortegn ved minus-temperaturer.
    int16_t raw = (int16_t)(((uint16_t)msb << 8) | lsb);
    tempRaw = raw >> 7;

    return LM75_OK;
}