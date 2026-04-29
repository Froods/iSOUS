#include <Arduino.h>
#include "../include/curtain_driver.h"

int main () {

    init(); //init timers on arduino

    initCurtainMotorPWM(); //init timer 3

    initTimer2Timer(); //init timer 0

    while (1)
    {
        test_of_curtain(); //test curtain
    }
    return 0;
}