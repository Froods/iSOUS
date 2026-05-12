#include <Arduino.h>
#include "curtain_driver.h"
#include "window_driver.h"
#include "lm75.h"
#include "I2C.h"
#include "SCD30.h"
#include "TSL2561.h"
#include "UARTinterface.h"
#include "UserSettings.h"
#include "ISOUScontroller.h"


int main(){
  init();
  _delay_ms(200);
  I2C i2c{};
  i2c.init_I2C(100000);

  UserSettings s(CO2Setting::Normalt_indeklima, 20);
  UARTinterface uart(9600, s);
  ISOUSController ISOUS(i2c, s);

  ISOUS.init();

  while (1) {
    uart.update();
    ISOUS.update();
  }
}