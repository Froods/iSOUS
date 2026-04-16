#include "window_driver.h"
#include <Arduino.h>

//Defining motor pins
// we are using the digital pins 22 (Blue), 24 (Pink), 26 (Yellow), 28 (Orange)
int motor_Pins[4] = {22,24,26,28};

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
//stage 1:
{false, true, true, true},
//stage 2:
{false, false, true, true},
//stage 3:
{true, false, true, true},
//stage 4:
{true, false, false, true},
//stage 5:
{true, true, false, true},
//stage 6:
{true, true, false, false},
//stage 7:
{true, true, true, false},
//stage 8:
{false, true, true, false},
};

//A function to move the motor
//takes a bool as a parameter for which direction. 
//True = forward
void move_Motor_1_stage (bool direction) {
    //This int keeps track of which stage the motor is in according to stages_O_Y_P_B
    //it is made static to only be allorcated once (the first time the function is called)
    //afterwards it keeps its assigned value even though the function is called a lot of times
    static int current_Stage = 0;

    // if direction is forward (true) then we want to go to the next stage to go forward
    //going from stage 1 - 8 causes the motor to go forward
    //going from stage 8 -1 causes the motor to go backwards
  if (direction) {
    current_Stage++;
  } else {
    current_Stage--;
  }
  
  //We only have stages from 1-8 (0-7) and therefore we want to keep the current_Stage within that limit
  //This way we avoid calling the stage_O_Y_P_B with a number, that does not match an array
  if (current_Stage > 7) current_Stage = 0;
  if (current_Stage < 0) current_Stage = 7;

    // Lastly the four motor pins is set after the array in stage_O_Y_P_B
  for (int i = 0; i < 4; i++) {
    digitalWrite(motor_Pins[i], stages_O_Y_P_B[current_Stage][i]);
  }
}

void test_Of_Motor() {
  //to move the motor 1 full rotation in one direction we need to move the motor some stages:
  //To move the motor 1 step (5 degrees), we need to go through 64 stages.
  //And then to move the motor 360 degrees, we need to go through 64 steps.
  //therefore to drive a full direction, we need to make 64x64 = 4096 stages
  // Drive 4096 steps in one direction
  for (int i = 0; i < 4096; i++) {
    move_Motor_1_stage(true);
    delay(2); // the pause between each stage. Should NOT be less than 1-2 ms
  }
  
  delay(1000); // wait one second
  
  // Drive the motor the other direction
  for (int i = 0; i < 4096; i++) {
    move_Motor_1_stage(false);
    delay(2); //the pause between each stage. Should NOT be less than 1-2 ms
  }
  
  delay(1000);
}

