#include <Arduino.h>
#include "window_driver.h"

int main () {

init(); //init timers on arduino

WindowMotor w;

while (1) //test the motor
{
  w.test_Of_Motor();
}

  return 0;
}