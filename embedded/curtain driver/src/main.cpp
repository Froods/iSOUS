#include <Arduino.h>
#include "../include/curtain_driver.h"

int main () {

    init(); //init timers on arduino

    CurtainMotor p; //make a curtainmotor class p

    while (1)
    {
        p.test_of_curtain(); //test curtain p
    }
    return 0;
}