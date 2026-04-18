#include "i2c.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <Arduino.h>



void I2C::init_I2C(uint32_t clockFreq = 100000) {
  TWSR = 0x00; //No prescaler
  TWBR = ((F_CPU / clockFreq) - 16) / 2; //sætter frekvensen
}


I2C_Status I2C::start() {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //Sender start condition
  while (!(TWCR & (1 << TWINT))); //venter indtil start er sendt.

  uint8_t status = TWSR & 0xF8; //læser status register
  //0x00 = Bus fejl (hardware problem, SDA/SCL stuck)
  //0x38 = Arbitration lost (anden master har overtaget bussen)
  if(status == 0x00 || status == 0x38) {
    stop(); //frigør bussen, ukendt fejl
    return I2C_BUS_ERROR;
  }


  //0x08 = START sent successfully
  //0x10 Repeated START transmitted successfully
  if(status != 0x08 && status != 0x10) {
    stop(); // Frigører bussen
    return I2C_START_ERROR;
  }
  return I2C_OK;
  }

I2C_Status I2C::write(uint8_t data) {
  TWDR = data; //Data som bliver sendt
  TWCR = (1 << TWINT) | (1 << TWEN); //starter transmissionen
  while (!(TWCR & (1 << TWINT))); //Venter indtil byte er sendt 

  uint8_t status = TWSR & 0xF8; //læser status register

  //0x20 = SLA+W sendt, NACK modtaget (slave svarede ikke på adresse+write)
  //0x48 = SLA+R sendt, NACK modtaget (slave svarede ikke på adresse+read)
  if(status == 0x20 || status == 0x48) {
    stop(); //frigør bussen
    return I2C_ADDR_NACK;
  }

  //0x30 = Data byte sendt, NACK modtaget (slave afviste data)
  if (status == 0x30) {
    stop(); //frigør bussen
    return I2C_DATA_NACK;
  }

  // 0x18 = address+write ACKed
  // 0x28 = data byte ACKed
  // 0x40 = address+read ACKed
  if (status != 0x18 && status != 0x28 && status != 0x40) {
    stop(); //frigør bussen, ukendt fejl
    return I2C_BUS_ERROR;
  }
  return I2C_OK;
}

//Sender stop condition
void I2C::stop() 
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); //sender stop condition
  _delay_us(100); //giv hardware tid til at sende stop
  TWCR = 0; //sluk TWI hardware helt
}

//Læser data fra I2C
uint8_t I2C::read(uint8_t isLast) 
{
  uint32_t timeout = 0;
  // if not last byte to read
  if (isLast == 0) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); //sender ack
  } else {
    TWCR = (1 << TWINT) | (1 << TWEN); //sender ikke ack
  }
  while ((TWCR & (1 << TWINT)) == 0) 
    {
      timeout++;
      if(timeout > 500000){
        return 0;
      }
    }
  return TWDR; //returnerer data i regiteret.
}
