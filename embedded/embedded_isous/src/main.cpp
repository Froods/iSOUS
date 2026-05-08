#include <Arduino.h>
#include "curtain_driver.h"
#include "window_driver.h"


int main(){
  init();
  Serial.begin(9600);
  WindowMotor window;
  CurtainMotor curtain;

  Serial.println("Initialization done");

  while (1){
    window.test_Of_Motor();
    _delay_ms(500);
    curtain.test_of_curtain();
    _delay_ms(500);

    Serial.println("loop done");
  }
}