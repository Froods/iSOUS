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
  Serial.begin(9600);
  _delay_ms(200);
  I2C i2c{};
  i2c.init_I2C(100000);
  Serial.println("I2C initialized @ 100KHz");


  UserSettings s(CO2Setting::Normalt_indeklima, 20);
  ISOUSController ISOUS(i2c, s);
  Serial.println("Isous constructed");

  ISOUS.init();
  _delay_ms(2500);
  Serial.println("Initialization done");
  Serial.println("entering loop");

  while (1) {
  ISOUS.update();

   Serial.print("CO2="); Serial.print(s.getActualCO2() == CO2Setting::Hoej ? "HIGH" : "ok");
   Serial.print(" T_in="); Serial.print(s.getRoomTemp());
   Serial.print(" T_out="); Serial.print(s.getOutTemp());
   Serial.print(" Lux="); Serial.println(s.getLight());

   _delay_ms(2000);
  }
}