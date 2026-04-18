#include <Arduino.h>
#include "I2C.h"
#include "Sensors.h"
extern "C" {
  #include "uart.h"
}
#include <util/delay.h>



I2C I2CBUS;

void setup() {
  Serial.begin(9600);
  I2CBUS.init_I2C();
  delay(3000);
}

void loop() {
  I2C_Status result = I2CBUS.start();
  if (result != I2C_OK) {
    Serial.print("Start fejl: ");
    Serial.println(result);
    delay(1000);
    return;
  }

  result = I2CBUS.write(0x61 << 1); // SCD30 adresse + write
  if (result != I2C_OK) {
    Serial.print("Write fejl: ");
    Serial.println(result);
    delay(1000);
    return;
  }

  I2CBUS.stop();
  Serial.println("SCD30 ACK");
  delay(1000);
}