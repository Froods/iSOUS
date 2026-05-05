#include <Arduino.h>
#include "I2C.h"
#include "Sensors.h"
#include <util/delay.h>



I2C I2CBUS;
SCD30 scd30(I2CBUS);

void setup() {
  Serial.begin(9600);

  Serial.println(F("=== SCD30 Test Program ==="));

  I2CBUS.init_I2C(50000);
  delay(3000);
  Serial.println(F("I2C bus initialized."));

  scd30.begin();
  delay(2000);
  Serial.println(F("SCD30 measurement started."));
  delay(1000);
}

void loop() {
  Serial.println("---Reading SCD30---");
  
  SCD30_Status status = scd30.readData();

  switch (status){
    case SCD30_OK: 
      float co2 = scd30.getCO2();
      float temp = scd30.getTemperature();

      Serial.print(F("CO2:         "));
      Serial.print(co2, 2);
      Serial.println(F(" ppm"));

      Serial.print(F("Temperature: "));
      Serial.print(temp, 2);
      Serial.println(F(" C"));
      break;
    
    case SCD30_CRC_ERROR:
      Serial.println(F("ERROR: CRC mismatch on received data."));
      break;

    case SCD30_TIMEOUT:
      Serial.println(F("ERROR: Timeout waiting for data-ready."));
      break;

    default:
      Serial.println(F("ERROR: Unknown status."));
      break;
  }
  Serial.println();
  delay(2000);  // SCD30's default measurement interval
}