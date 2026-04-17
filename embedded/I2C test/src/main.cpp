#include <Arduino.h>
#include "I2C.h"
#include "Sensors.h"
extern "C" {
  #include "uart.h"
}
#include <util/delay.h>

I2C I2CBUS;
SCD30 tempsensor(I2CBUS);

void setup() {

  I2CBUS.init_I2C();
  _delay_ms(2000);
}

void loop() {
  tempsensor.readData();
}