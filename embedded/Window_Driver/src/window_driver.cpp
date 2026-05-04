#include "window_driver.h"
#include <Arduino.h>

//constructor
WindowMotor::WindowMotor() {
    initWindowMotorPins();
    this->isOpen = false;
}

//init pins:
//Making motor pins become output:
void WindowMotor::initWindowMotorPins() {
    for(int i = 0; i < 4; i++) {
        pinMode(this->motor_Pins[i], OUTPUT);
    }
}

//A function to move the motor
//takes a bool as a parameter for which direction. 
//True = clockwise
void WindowMotor::moveMotor1Stage (bool direction) {
    //This int keeps track of which stage the motor is in according to stages_O_Y_P_B
    //it is made static to only be allorcated once (the first time the function is called)
    //afterwards it keeps its assigned value even though the function is called a lot of times
    static int current_Stage = 0;

    //if direction is clockwise (true) then the sequence is from 1 - 8 which means:
    //going from stage 1 - 8 causes the motor to go clockwise
    //going from stage 8 - 1 causes the motor to go anti-clockwise
  if (direction) {
    current_Stage++;
  } else {
    current_Stage--;
  }
  
  //We only have stages from 1-8 (0-7) and therefore we want to keep the current_Stage within that limit
  //This way we avoid calling the stage_O_Y_P_B with a number, that does not match an index in the array
  if (current_Stage > 7) current_Stage = 0;
  if (current_Stage < 0) current_Stage = 7;

  // Lastly the four motor pins is set after the array in stage_O_Y_P_B
  for (int i = 0; i < 4; i++) {
    digitalWrite(this->motor_Pins[i], this->stages_O_Y_P_B[current_Stage][i]);
  }
}

void WindowMotor::openWindow() {
if (this->isOpen == false) 
  {
  //to move the motor 1 full rotation in one direction we need to move the motor some stages:
  //To move the motor 1 step (5.625 degrees), we need to go through 64 stages.
  //And then to move the motor 360 degrees, we need to go through 64 steps.
  //therefore to drive a full direction, we need to make 64x64 = 4096 stages
  // Drive 4096 steps in the clockwise direction
  for (int i = 0; i < 4096; i++) {
    moveMotor1Stage(true); //clockwise
    _delay_ms(5); // the pause between each stage. Essentially the speed. Must not be lower than 2 ms
    }

  this->isOpen = true;
  } 
  else 
  {
  //Does nothing because window is already open
  }
}

void WindowMotor::closeWindow() {
if (this->isOpen == true) 
  {
  //to move the motor 1 full rotation in one direction we need to move the motor some stages:
  //To move the motor 1 step (5.625 degrees), we need to go through 64 stages.
  //And then to move the motor 360 degrees, we need to go through 64 steps.
  //therefore to drive a full direction, we need to make 64x64 = 4096 stages
  // Drive 4096 steps in anti-clockwise direction
  for (int i = 0; i < 4096; i++) {
    moveMotor1Stage(false); //anti-clockwise
    _delay_ms(5); // the pause between each stage. Essentially the speed. Must not be lower than 2 ms
    }

  this->isOpen = false;
  } 
  else 
  {
  //Does nothing because window is already closed
  }
}

void WindowMotor::test_Of_Motor() {

  openWindow();

  openWindow();
  
  _delay_ms(1000);

  closeWindow();

  closeWindow();
  
  _delay_ms(1000);
}