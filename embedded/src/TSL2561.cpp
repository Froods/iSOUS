#include "TSL2561.h"
#include <util/delay.h>


TSL2561::TSL2561(I2C &i2c, uint8_t addr) : I2C_BUS(i2c), Adresse(addr), lux(0) {}


TSL2561_Status TSL2561::begin()
{
    // Læs chip-ID for at verificere at sensoren svarer korrekt
    if (I2C_BUS.start() != I2C_OK)                                   return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(this->Adresse << 1) != I2C_OK)                  return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_REG_ID) != I2C_OK) return TSL2561_BUS_ERROR;
    if (I2C_BUS.start() != I2C_OK)                                   return TSL2561_BUS_ERROR;
    if (I2C_BUS.write((this->Adresse << 1) | 1) != I2C_OK)            return TSL2561_BUS_ERROR;
    uint8_t chip_id = I2C_BUS.read(1);
    I2C_BUS.stop();

    // Bits 7:4 = PARTNO (skal være 0x5 for TSL2561)
    if ((chip_id & 0xF0) != 0x50) return TSL2561_ID_ERROR;

    // Skriv timing-konfiguration (gain 1x + integration 402 ms)
    if (I2C_BUS.start() != I2C_OK)                                          return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(this->Adresse << 1) != I2C_OK)                         return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_REG_TIMING) != I2C_OK)  return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_TIMING_CONFIG) != I2C_OK)                     return TSL2561_BUS_ERROR;
    I2C_BUS.stop();

    return TSL2561_OK;
}


TSL2561_Status TSL2561::readData()
{
    // Tænd sensor (skriv 0x03 til control-registret)
    if (I2C_BUS.start() != I2C_OK)                                           return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(this->Adresse << 1) != I2C_OK)                          return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_REG_CONTROL) != I2C_OK)  return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(0x03) != I2C_OK)                                       return TSL2561_BUS_ERROR;
    I2C_BUS.stop();

    // Vent på ADC-konvertering (402 ms integrationstid + lidt margin)
    _delay_ms(420);

    // Læs kanal 0 (broadband: synligt + IR) som 16-bit word
    uint16_t broadband;
    if (I2C_BUS.start() != I2C_OK)                                                         return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(this->Adresse << 1) != I2C_OK)                                        return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REG_CHAN0_LOW) != I2C_OK) return TSL2561_BUS_ERROR;
    if (I2C_BUS.start() != I2C_OK)                                                         return TSL2561_BUS_ERROR;
    if (I2C_BUS.write((this->Adresse << 1) | 1) != I2C_OK)                                  return TSL2561_BUS_ERROR;
    uint8_t bb_lsb = I2C_BUS.read(0);
    uint8_t bb_msb = I2C_BUS.read(1);
    I2C_BUS.stop();
    broadband = ((uint16_t)bb_msb << 8) | bb_lsb;

    // Læs kanal 1 (IR-only) som 16-bit word
    uint16_t ir;
    if (I2C_BUS.start() != I2C_OK)                                                         return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(this->Adresse << 1) != I2C_OK)                                        return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REG_CHAN1_LOW) != I2C_OK) return TSL2561_BUS_ERROR;
    if (I2C_BUS.start() != I2C_OK)                                                         return TSL2561_BUS_ERROR;
    if (I2C_BUS.write((this->Adresse << 1) | 1) != I2C_OK)                                  return TSL2561_BUS_ERROR;
    uint8_t ir_lsb = I2C_BUS.read(0);
    uint8_t ir_msb = I2C_BUS.read(1);
    I2C_BUS.stop();
    ir = ((uint16_t)ir_msb << 8) | ir_lsb;

    // Sluk sensor (skriv 0x00 til control-registret) for at spare strøm
    if (I2C_BUS.start() != I2C_OK)                                           return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(this->Adresse << 1) != I2C_OK)                          return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_REG_CONTROL) != I2C_OK)  return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(0x00) != I2C_OK)                                       return TSL2561_BUS_ERROR;
    I2C_BUS.stop();

    // --- Lux-beregning ---
    // Pre-skaleret for gain=1x og integration=402ms: chScale = (1<<10)<<4 = 16384
    // (bb * 16384) >> 10 = bb * 16
    unsigned long ch0 = (unsigned long)broadband * 16;
    unsigned long ch1 = (unsigned long)ir * 16;

    unsigned long ratio = 0;
    if (ch0 != 0) {
        ratio = ((ch1 << (TSL2561_LUX_RATIOSCALE + 1)) / ch0 + 1) >> 1;
    }

    unsigned int b, m;
    if      (ratio <= TSL2561_LUX_K1T) { b = TSL2561_LUX_B1T; m = TSL2561_LUX_M1T; }
    else if (ratio <= TSL2561_LUX_K2T) { b = TSL2561_LUX_B2T; m = TSL2561_LUX_M2T; }
    else if (ratio <= TSL2561_LUX_K3T) { b = TSL2561_LUX_B3T; m = TSL2561_LUX_M3T; }
    else if (ratio <= TSL2561_LUX_K4T) { b = TSL2561_LUX_B4T; m = TSL2561_LUX_M4T; }
    else if (ratio <= TSL2561_LUX_K5T) { b = TSL2561_LUX_B5T; m = TSL2561_LUX_M5T; }
    else if (ratio <= TSL2561_LUX_K6T) { b = TSL2561_LUX_B6T; m = TSL2561_LUX_M6T; }
    else if (ratio <= TSL2561_LUX_K7T) { b = TSL2561_LUX_B7T; m = TSL2561_LUX_M7T; }
    else                                { b = TSL2561_LUX_B8T; m = TSL2561_LUX_M8T; }

    unsigned long temp = 0;
    if ((ch0 * b) > (ch1 * m)) {
        temp = (ch0 * b) - (ch1 * m);
    }
    temp += (1UL << (TSL2561_LUX_LUXSCALE - 1));
    lux = temp >> TSL2561_LUX_LUXSCALE;

    return TSL2561_OK;
}
