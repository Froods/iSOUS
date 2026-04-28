#include <Arduino.h>
#include "../include/curtain_driver.h"

int main () {

    initCurtainMotorPWM();

    while (1)
    {
        test_of_curtain();
    }
    return 0;
}