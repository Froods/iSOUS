#pragma once
#include "sensors.h"
#include <arduino/delay.h>

/**
* @brief implementering af sensorklasserne (CO2Sensor, TemperatureSensor, LightSensor)
*
* Oversigt over kommandoer (Fra SCD30 datablad)
* - 0x0010 start kontinuerlig måling, med 16bit argument for trykkomensation (som vi ikke bruger)
* - 0x0202 Get data ready status (Returns 0x0000 if not ready, returns 0x0001 if ready)
* - 0x0300 Read measurement
*
*/


/**
* @brief Beregner CRC-8 checksum for SCD30
*
* static helper function.
* Se datasheet afsnit 1.1.3 for beskrivelse af CRC algoritme
* implementering snuppet fra Sensirion/arduino-core
* https://github.com/Sensirion/arduino-core/blob/main/src/SensirionCrc.cpp#L34
*
*  @param data pointer til data-bytes som skal checkes.
*  @param count Antal bytes.
*  @param init Initialværdi for CRC (0xFF for SCD30).
*  @param polynomial CRC polynomial (0x31 for SCD30).
*
*  @return beregnet CRC-8 checksum.
*/
static uint8_t generateCRCGeneric(const uint8_t* data, size_t count, uint8_t init = 0xFF,
                           uint8_t polynomial = 0x31) {
    uint8_t crc = init;

    /* calculates 8-Bit checksum with given polynomial */
    for (size_t current_byte = 0; current_byte < count; ++current_byte) {
        crc ^= (data[current_byte]);
        for (uint8_t crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ polynomial;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}




CO2Sensor::CO2Sensor(I2C &i2c) : I2C_BUS(i2c) 
{
  //start kontinuerlig måling uden trykkompensation (afsnit 1.3.1 i SCD30 datablad)
  //Kommando: 0x0010, argument 0x0000.
  uint8_t arg[] = {0x00, 0x00};
  uint8_t crc = generateCRCGeneric(arg,2);

  I2C_BUS.start(); //start bit
  I2C_BUS.write(Adresse << 1); //I2C adresse er 7-BIT, men I2C sender 8-bit. De øverste 7-Bits er adresse, og den nederste bit er read/write. Bit 0 = 0 (write).  
  I2C_BUS.write(0x00); //cmd 0x0010 MSB
  I2C_BUS.write(0x10); //cmd 0x0010 LSB
  I2C_BUS.write(arg[0]);
  I2C_BUS.write(arg[1]);
  I2C_BUS.write(crc);
  I2C_BUS.stop();

  _delay_ms(10); //anbefalet delay efter kommunikation (fra datasheet)

}


uint16_t CO2Sensor::read()
{
  // Vent på at data er klar. Spørger sensor om den har data til os. Den modtagne lsb fra sensoren er 1, hvis data er klar. Ellers 0
  
  uint8_t received_lsb = 0;
  while (received_lsb != 1) 
  {
    I2C_BUS.start(); //start bit
    I2C_BUS.write(Adresse << 1); //I2C Addresse, samt shift op (write cmd)
    I2C_BUS.write(0x02); //cmd 0x0202 MSB
    I2C_BUS.write(0x02); //cmd 0x0202 LSB
    I2C_BUS.stop();

    _delay_ms(10); //anbefalet delay efter kommunikation (fra datasheet)

    //Anden besked (ingen kommando). Read istedet for write
    I2C_BUS.start(); //start bit
    I2C_BUS.write((Adresse << 1) | 1); //I2C addresse. Bemærk: Masker med 1. Bit 0 = 1 (read)
    uint8_t received_msb = I2C_BUS.read(0); // ACK
    received_lsb = I2C_BUS.read(0); // ACK (Bemærk, hvis data er klar (lsb = 1) exitter while loop i bunden)
    uint8_t crc = I2C_BUS.read(1); //NACK (sidste byte)
    I2C_BUS.stop();

    //checksum
  uint8_t receivedData[] = {received_msb, received_lsb};
    if (crc != generateCRCGeneric(receivedData, 2)) 
    {
      received_lsb = 0; //CRC fejl, start forfra
    }

    if (received_lsb != 1) //Delay inden vi går videre til næste skridt 
    {
      _delay_ms(100);
    }
  }
  
    
  //Læs måledata fra sensor
  //0x0300 - anmod om data
  I2C_BUS.start(); //start bit
  I2C_BUS.write(Adresse << 1); //I2C Addresse, samt shift op (write cmd)
  I2C_BUS.write(0x03); //cmd 0x0300 MSB
  I2C_BUS.write(0x00); //cmd 0x0300 LSB
  I2C_BUS.stop();

  _delay_ms(10);

  //Læs data
  I2C_BUS.start(); //start bit
  I2C_BUS.write((Adresse << 1) | 1); //I2C addresse (read) 
  
  uint8_t co2_MSB1 = I2C_BUS.read(0);
  uint8_t co2_LSB1 = I2C_BUS.read(0);
  uint8_t co2_CRC1 = I2C_BUS.read(0);
  uint8_t co2_MSB2 = I2C_BUS.read(0);
  uint8_t co2_LSB2 = I2C_BUS.read(0);
  uint8_t co2_CRC2 = I2C_BUS.read(0);

  uint8_t temp_MSB1 = I2C_BUS.read(0);
  uint8_t temp_LSB1 = I2C_BUS.read(0);
  uint8_t temp_CRC1 = I2C_BUS.read(0);
  uint8_t temp_MSB2 = I2C_BUS.read(0);
  uint8_t temp_LSB2 = I2C_BUS.read(0);
  uint8_t temp_CRC2 = I2C_BUS.read(0);

  uint8_t hum_MSB1 = I2C_BUS.read(0);
  uint8_t hum_LSB1 = I2C_BUS.read(0);
  uint8_t hum_CRC1 = I2C_BUS.read(0);
  uint8_t hum_MSB2 = I2C_BUS.read(0);
  uint8_t hum_LSB2 = I2C_BUS.read(0);
  uint8_t hum_CRC2 = I2C_BUS.read(1); // NACK (byte 18, sidste i transmissionen)

  I2C_BUS.stop();

  //CRC checks
  uint8_t co2Check1[] = {co2_MSB1, co2_LSB1};
  uint8_t co2Check2[] = {co2_MSB2, co2_LSB2};
  if (co2_CRC1 != generateCRCGeneric(co2Check1, 2) ||
      co2_CRC2 != generateCRCGeneric(co2Check2, 2)) 
  {
    return 0;
  }
  
  uint8_t tempCheck1[] = {temp_MSB1, temp_LSB1};
  uint8_t tempCheck2[] = {temp_MSB2, temp_LSB2};
  if (temp_CRC1 != generateCRCGeneric(tempCheck1, 2) ||
      temp_CRC2 != generateCRCGeneric(tempCheck2, 2)) 
  {
    return 0;
  }

  uint8_t humCheck1[] = {hum_MSB1, hum_LSB1};
  uint8_t humCheck2[] = {hum_MSB2, hum_LSB2};
  if (hum_CRC1 != generateCRCGeneric(humCheck1, 2) ||
      hum_CRC2 != generateCRCGeneric(humCheck2, 2)) 
  {
    return 0;
  }
}


Temperatursensor::Temperatursensor(I2C &i2c)
  :i2c_(i2c), address_(0x61); //addresse fra datablad



Temperatursensor::read()
{
  this->readRegister(0x0300) //Addresse samme som CO2, da samme sensor.

}



LightSensor::LightSensor(I2C &i2c)
  :i2c_(i2c), address_(0x39);



LightSensor::read(){

}

WindowMotor::open();
