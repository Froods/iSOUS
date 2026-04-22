#include <Arduino.h>
#include "window_driver.h"

int main () {

init(); //init timers on arduino

initWindowMotorPins(); //init motor pins

while (1) //test the motor for ever
{
  test_Of_Motor();
}


  return 0;
}