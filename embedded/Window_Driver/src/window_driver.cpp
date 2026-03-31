#include "window_driver.h"
#include <Arduino.h>

//Defining motor pins
//we are using pins 28 (O), 26 (Y), 24 (P), 22 (B)

int motor_Pins[4] = {28,26,24,22};

//init pins:
//Making motor pins become output:
void init_Motor_pins() {
    for(int i = 0; i < 4; i++) {
        pinMode(motor_Pins[i], OUTPUT);
    }
}

//stages to make the motor spin with the 4 coils
//the motor spins when a coil i grounded
const bool stages_O_Y_P_B[8][4] = {
//step 1:
{false, true, true, true},
//step 2:
{false, false, true, true},
//step 3:
{true, false, true, true},
//step 4:
{true, false, false, true},
//step 5:
{true, true, false, true},
//step 6:
{true, true, false, false},
//step 7:
{true, true, true, false},
//step 8:
{false, true, true, false},
};