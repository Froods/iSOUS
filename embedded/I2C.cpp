#include "i2c.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>



void I2C::init_I2C(uint32_t clockFreq) {
  TWSR = 0x00; //No prescaler
  TWBR = ((F_CPU / clockFreq) - 16) / 2; //sætter frekvensen
}


void I2C::start() {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //Sender start condition
  while (!(TWCR & (1 << TWINT))) //venter indtil start er sendt.
    {}
}

void I2C::write(uint8_t data) {
  TWDR = data; //Data som bliver sendt
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) //Venter indtil byte er sendt 
    {}
}

//Sender stop condition
void I2C::stop() 
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while (TWCR & (1 << TWSTO)) // vent indtil stop er gennemført
    {}
} 

//Læser data fra I2C
uint8_t I2C::read(uint8_t isLast) 
{
  // if not last byte to read
  if (isLast == 0) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); //sender ack
  } else {
    TWCR = (1 << TWINT) | (1 << TWEN); //sender ikke ack
  }
  while ((TWCR & (1 << TWINT)) == 0) 
    {}
  return TWDR; //returnerer data i regiteret.
}
