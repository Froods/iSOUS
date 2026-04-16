#include "../include/curtain_driver.h"
#include <Arduino.h>

//Defining motor PWM pin:
// we are using the PWM pin 2 (PE4, OC3B, INT4)
//init pin:
//Making motor pin become output:
void init_Curtain_Motor_pin() {
    pinMode(2, OUTPUT);
}

