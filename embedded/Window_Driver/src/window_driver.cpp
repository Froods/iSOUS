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
//the motor spins when a coil is grounded
//The O,Y,P,B stands for Orange, Yellow, Pink & Blue and is a refenrence to the cables on the ULN2003
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

//A function to move the motor
//takes a bool as a parameter for which direction. 
//True = forward
void move_Motor (bool direction) {
    //This int keeps track of which stage the motor is in according to stages_O_Y_P_B
    //it is made static to only be allorcated once (the first time the function is called)
    //afterwards it keeps its assigned value even though the function is called a lot of times
    static int current_Stage = 0;

    // if direction is forward (true) then we want to go to the next step to go forward
    //going from step 1 - 8 causes the motor to go forward
    //going from step 8 -1 causes the motor to go backwards
  if (direction) {
    current_Stage++;
  } else {
    current_Stage--;
  }
  
  //We only have steps from 1-8 (0-7) and therefore we want to keep the current_stage within that limit
  //This way we avoid calling the stage_O_Y_P_B with a number, that does not match an array
  if (current_Stage > 7) current_Stage = 0;
  if (current_Stage < 0) current_Stage = 7;

    // Lastly the four motor pins is set after the array in stage_O_Y_P_B
  for (int i = 0; i < 4; i++) {
    digitalWrite(motor_Pins[i], stages_O_Y_P_B[current_Stage][i]);
  }
}

