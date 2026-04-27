#include "i2c.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <Arduino.h>


  void I2C::recoverbus(){
    // 1. Disable TWI hardware to take manual control of pins
    TWCR = 0; 
    pinMode(20, INPUT_PULLUP); // SDA (Mega Pin 20)
    pinMode(21, OUTPUT);       // SCL (Mega Pin 21)

    // 2. If SDA is held low, clock SCL up to 9 times
    // This "clocks out" any half-finished bytes in the slave
    for (uint8_t i = 0; i < 9; i++) {
        if (digitalRead(20) == HIGH) break; // Slave released SDA!
        digitalWrite(21, LOW);
        _delay_us(5);
        digitalWrite(21, HIGH);
        _delay_us(5);
    }

    // 3. Send a manual STOP condition
    pinMode(20, OUTPUT);
    digitalWrite(20, LOW);
    _delay_us(5);
    digitalWrite(21, HIGH);
    _delay_us(5);
    digitalWrite(20, HIGH); // SDA Low -> High while SCL is High = STOP
    
    _delay_ms(10);
    // 4. Re-enable TWI
  }

void I2C::init_I2C(uint32_t clockFreq) {
    recoverbus(); 

    //Hardware TWI Initialization
    TWSR = 0x00; // No prescaler
    // Calculation: SCL freq = F_CPU / (16 + 2(TWBR) * Prescaler)
    TWBR = ((F_CPU / clockFreq) - 16) / 2; 
    TWCR = (1 << TWEN); // Enable TWI
}
I2C_Status I2C::start() {
    
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    uint32_t timeout = 0;
    while (!(TWCR & (1 << TWINT))) {
        timeout++;
        if(timeout > 50000) return I2C_BUS_ERROR; // Don't hang forever!
    }

    uint8_t status = TWSR & 0xF8;
    if(status == 0x08 || status == 0x10) {
        return I2C_OK;
    } else {
        return I2C_START_ERROR;
    }
}

I2C_Status I2C::write(uint8_t data) {
  TWDR = data; //Data som bliver sendt
  TWCR = (1 << TWINT) | (1 << TWEN); //starter transmissionen
  while (!(TWCR & (1 << TWINT))); //Venter indtil byte er sendt 

  uint8_t status = TWSR & 0xF8; //læser status register

    // IF THIS PRINTS 0x00, YOU HAVE A HARDWARE/NOISE ISSUE
    // IF THIS PRINTS 0x38, THE MASTER THINKS ANOTHER MASTER IS TALKING

  if (status == 0x00) {
        // A bus error occurred. We MUST reset the TWI hardware.
        TWCR = 0; 
        TWCR = (1 << TWEN);
        return I2C_BUS_ERROR; 
    }
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
void I2C::stop() {
    // 1. Initiate Hardware Stop
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

    // 2. Wait for TWSTO bit to clear
    uint16_t timeout = 0;
    while((TWCR & (1 << TWSTO)) && (timeout < 1000)) {
        timeout++;
    }

    // 3. Mega-Specific Bus Recovery
    // Check if SDA (Port D, Bit 1) is still being held LOW by the slave
    if (!(PIND & (1 << PD1))) { 
        // Force-disable the TWI hardware to release the pins
        TWCR = 0; 
        pinMode(21, OUTPUT); // SCL
        
        // Manual Toggle: Give the slave the "missing" clock pulses to make it let go
        for (uint8_t i = 0; i < 9; i++) {
            digitalWrite(21, HIGH);
            _delay_us(5);
            digitalWrite(21, LOW);
            _delay_us(5);
        }
        
        // Send a manual STOP condition
        pinMode(20, OUTPUT); // SDA
        digitalWrite(20, LOW);
        _delay_us(5);
        digitalWrite(21, HIGH);
        _delay_us(5);
        digitalWrite(20, HIGH);
        
        // Re-enable TWI hardware
        TWCR = (1 << TWEN);
    }
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
