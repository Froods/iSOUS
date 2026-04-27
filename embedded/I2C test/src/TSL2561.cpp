#include "TSL2561.h"


TSL2561::TSL2561(I2C &i2c, uint8_t address = 0x39)  
    :   I2C_BUS(i2c), Adresse(address), 
        gain(TSL2561_GAIN_1X), integrationTime(TSL2561_INT_402MS)
        broadband(0), ir(0), lux(0){}


void TSL2561::writeRegister(uint8_t reg, uint8_t value)
{
    I2C_BUS.start();
    I2C_BUS.write((Adresse << 1)); //address + write
    I2C_BUS.write(reg);
    I2C_BUS.write(value);
    I2C_BUS.stop(); 
}

uint8_t TSL2561::readRegister8(uint8_t reg)
{
    //Sender anmodning om data først (write)
    I2C_BUS.start();
    I2C_BUS.write((Adresse << 1)); //adresse + write
    I2C_BUS.write(reg);
    I2C_BUS.stop();

    //Read request
    I2C_BUS.start();
    I2C_BUS.write(Adresse << 1 | 1); //addresse + read
    uint8_t received_byte = I2C_BUS.read(1); //læser data ind i byte, og NACK'er (stop)
    I2C_BUS.stop();
    return received_byte;
}

uint16_t  TSL2561::readRegister16(uint8_t reg)
{
    //Sender anmodning om data først (write)
    I2C_BUS.start();
    I2C_BUS.write((Adresse << 1)); //adresse + write
    I2C_BUS.write(reg);
    I2C_BUS.stop();

    //Read request
    I2C_BUS.start();
    I2C_BUS.write(Adresse << 1 | 1); //addresse + read
    uint8_t received_lsb = I2C_BUS.read(0); //LSB = første read, ACK
    uint8_t received_msb = I2C_BUS.read(1); //MSB = andet read, NACK (stop)
    I2C_BUS.stop();

    //shifter MSB 8 op, og masker med LSB
    uint16_t received_data = (((uint16_t)received_msb << 8) | received_lsb);
    return received_data;
}


void TSL2561::enable()
{
    writeRegister(TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);
}


void TSL2561::disable()
{
    writeRegister(TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWEROFF);
}
