#include "TSL2561.h"
#include <util/delay.h>


TSL2561::TSL2561(I2C &i2c, uint8_t address)  
    :   I2C_BUS(i2c), Adresse(address), 
        gain(TSL2561_GAIN_1X), integrationTime(TSL2561_INT_402MS),
        broadband(0), ir(0), lux(0) 
{}


TSL2561_Status TSL2561::readData(){
   //tænd for enheden ved at sætte kontrol bit til 0x03
    if (enable() != TSL2561_OK) return TSL2561_BUS_ERROR;

    // vent x ms for ADC konvertering (afhængtigt af ing time)
    switch (this->integrationTime)
    {
    case TSL2561_INT_13MS:
        _delay_ms(15);
        break;

    case TSL2561_INT_101MS:
        _delay_ms(105);
        break;

    case TSL2561_INT_402MS:

    default:
        _delay_ms(402);
        break;
    }

    //Læs de 2 bytes fra kanal 0
    if (readRegister16(TSL2561_REGISTER_CHAN0_LOW, this->broadband) != TSL2561_OK) {
        disable(); //forsøg at slukke sensoren inden fejl returneres
        return TSL2561_BUS_ERROR;
    }

    //læs de 2 bytes fra kanal 1
    if (readRegister16(TSL2561_REGISTER_CHAN1_LOW, this->ir) != TSL2561_OK) {
        disable(); //forsøg at slukke sensoren inden fejl returneres
        return TSL2561_BUS_ERROR;
    }

    this->lux = calculateLux(broadband, ir);

    //sluk for enheden
    return disable();
}

uint32_t TSL2561::getLux()       { return lux; }
uint16_t TSL2561::getBroadband() { return broadband; }
uint16_t TSL2561::getIR()        { return ir; }

TSL2561_Status TSL2561::begin()
{
    uint8_t chip_id;
    if (readRegister8(TSL2561_REGISTER_ID, chip_id) != TSL2561_OK) return TSL2561_BUS_ERROR;

    // Bits 7:4 = PARTNO (skal være 0x5 for TSL2561). Bits 3:0 = REVNO (ignoreres).
    if ((chip_id & 0xF0) != 0x50) return TSL2561_ID_ERROR;

    return writeRegister(TSL2561_REGISTER_TIMING, gain | integrationTime);
}

TSL2561_Status TSL2561::writeRegister(uint8_t reg, uint8_t value)
{
    if (I2C_BUS.start() != I2C_OK)                          return TSL2561_BUS_ERROR;
    if (I2C_BUS.write((Adresse << 1)) != I2C_OK)            return TSL2561_BUS_ERROR; //address + write
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | reg) != I2C_OK) return TSL2561_BUS_ERROR;
    if (I2C_BUS.write(value) != I2C_OK)                     return TSL2561_BUS_ERROR;
    I2C_BUS.stop();
    return TSL2561_OK;
}

TSL2561_Status TSL2561::readRegister8(uint8_t reg, uint8_t &out)
{
    //Sender anmodning om data først (write)
    if (I2C_BUS.start() != I2C_OK)                          return TSL2561_BUS_ERROR;
    if (I2C_BUS.write((Adresse << 1)) != I2C_OK)            return TSL2561_BUS_ERROR; //adresse + write
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | reg) != I2C_OK) return TSL2561_BUS_ERROR;
    //Read request (repeated start er ok for tsl2561)
    if (I2C_BUS.start() != I2C_OK) { I2C_BUS.stop(); return TSL2561_BUS_ERROR; }
    if (I2C_BUS.write(Adresse << 1 | 1) != I2C_OK) return TSL2561_BUS_ERROR; //addresse + read
    out = I2C_BUS.read(1); //læser data ind i byte, og NACK'er (stop)
    I2C_BUS.stop();
    return TSL2561_OK;
}

TSL2561_Status TSL2561::readRegister16(uint8_t reg, uint16_t &out)
{
    //Sender anmodning om data først (write)
    if (I2C_BUS.start() != I2C_OK)            return TSL2561_BUS_ERROR;
    if (I2C_BUS.write((Adresse << 1)) != I2C_OK) return TSL2561_BUS_ERROR; //adresse + write
    // WORD_BIT = auto-increment, læser low+high i ét hug
    if (I2C_BUS.write(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | reg) != I2C_OK) return TSL2561_BUS_ERROR;
    //Read request (repeated start er ok for tsl2561)
    if (I2C_BUS.start() != I2C_OK) { I2C_BUS.stop(); return TSL2561_BUS_ERROR; }
    if (I2C_BUS.write(Adresse << 1 | 1) != I2C_OK) return TSL2561_BUS_ERROR; //addresse + read
    uint8_t received_lsb = I2C_BUS.read(0); //LSB = første read, ACK
    uint8_t received_msb = I2C_BUS.read(1); //MSB = andet read, NACK (stop)
    I2C_BUS.stop();

    //shifter MSB 8 op, og masker med LSB
    out = (((uint16_t)received_msb << 8) | received_lsb);
    return TSL2561_OK;
}

TSL2561_Status TSL2561::enable()
{
    return writeRegister(TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);
}

TSL2561_Status TSL2561::disable()
{
    return writeRegister(TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWEROFF);
}

uint32_t TSL2561::calculateLux(uint16_t bb, uint16_t ir_val)
{
    // 1) Skalering for integrationstid
    unsigned long chScale;
    switch (integrationTime) {
        case TSL2561_INT_13MS:  chScale = TSL2561_LUX_CHSCALE_TINT0; break;
        case TSL2561_INT_101MS: chScale = TSL2561_LUX_CHSCALE_TINT1; break;
        default:                chScale = (1UL << TSL2561_LUX_CHSCALE); break;
    }
    // 2) Ved 1x gain: gang med 16 for at matche referencen (16x gain)
    if (gain == TSL2561_GAIN_1X) chScale <<= 4;

    unsigned long channel0 = ((unsigned long)bb     * chScale) >> TSL2561_LUX_CHSCALE;
    unsigned long channel1 = ((unsigned long)ir_val * chScale) >> TSL2561_LUX_CHSCALE;

    unsigned long ratio1 = 0;
    if (channel0 != 0) {
        ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE + 1)) / channel0;
    }
    unsigned long ratio = (ratio1 + 1) >> 1;

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
    if ((channel0 * b) > (channel1 * m)) {
        temp = (channel0 * b) - (channel1 * m);
    }
    temp += (1UL << (TSL2561_LUX_LUXSCALE - 1));
    return temp >> TSL2561_LUX_LUXSCALE;
}