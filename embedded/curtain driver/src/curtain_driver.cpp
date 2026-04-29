#include "../include/curtain_driver.h"
#include <Arduino.h>

#include <avr/io.h>
#include <util/delay.h>


CurtainMotor::CurtainMotor(){
    initCurtainMotorPWM();
    initTimer2Timer();
    this->isOut = false;
}


void CurtainMotor::initCurtainMotorPWM() {

    //reset all registrers after init() is run:
    TCCR3A = 0;
    TCCR3B = 0;
    OCR3B = 0;
    ICR3 = 0;

    //Defining motor PWM pin:
    //we are using the PWM pin 2 (PE4, OC3B, INT4) as OUTPUT
    //Making motor pin become output:
    DDRE |= (1 << PE4);

    //set mode 14 (Fast PWM, TOP = ICR3)
    TCCR3A |= (1 << WGM31);
    TCCR3B |= (1 << WGM32);
    TCCR3B |= (1 << WGM33); 

    //use COM3B1 to make timer B run in 'inverting mode'
    TCCR3A |= (1 << COM3B0);
    TCCR3A |= (1 << COM3B1);

    //use CS31 to make prescaler 8:
    TCCR3B |= (1 << CS31);

    //Make the top value 39999 to get 50 hz (20 ms period)
    ICR3 = 39999;

    //init start position by making the duty cycle 1,5 ms. 
    //this makes the motor stand stil
    OCR3B = 36999;
}

void CurtainMotor::initTimer2Timer() {

    //reset all registrers after init() is run:
    //Which also puts timer 0 in normal mode
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    TIFR2 = 0;

    //16000000 / 1024 = 15625 hz
    //15625 / 100 = 156.25 steps = 0,01 ms

    //set prescaler to 1024
    TCCR2B |= (1 << CS20);
    TCCR2B |= (1 << CS21);
    TCCR2B |= (1 << CS22);
} 

void CurtainMotor::delayForCutain() {
    //reset timer overflow flag
    TIFR2 |= (1<<0); 

    //make timer only take 156 steps
    TCNT2 = 255-156; 

    //int to keep track of overflows
    int numberOfOverflows = 0; 

    //stay here indtil 849 overflows has passed 
    while (numberOfOverflows != 849) { 

        //wait for the timer to reach an overflow (sets up flag)
        while ((TIFR2 & (1<<0)) == 0) {} 

        //reset timer overflow flag
        TIFR2 |= (1<<0);

        //make timer only take 156 steps
        TCNT2 = 255-156;

        //increments numberofoverflow when overflow happen
        numberOfOverflows++; 
    }

}

void CurtainMotor::rollOutCurtain() {
    //check if isOut is false to avoid rollOut twice in a row
    if (this->isOut == false) {

        //sets dutycycle to 2 ms to go full speed to clockwise
        OCR3B = 37999;

        //executes delay function to wait 8,53 sec
        delayForCutain();

        //sets dutycycle to 1,5 ms to make the motor stop
        OCR3B = 36999;

        //sets isOut to true after curtain is rolled out
        this->isOut = true;   
    } else {
        //Do nothing
    }
}

void CurtainMotor::rollInCurtain() {
    //check if isOut is true to avoid rollIn twice in a row
    if (this->isOut == true) {

        //sets dutycycle to 1 ms to go full speed to anti-clockwise
        OCR3B = 35999;

        //executes delay function to wait 8,53 sec
        delayForCutain();

        //sets dutycycle to 1,5 ms to make the motor stop
        OCR3B = 36999;
        
        //sets isOut to false after curtain is rolled in
        this->isOut = false;   
    } else {
        //Do nothing
    }
}

void CurtainMotor::test_of_curtain() {
    rollOutCurtain();
    rollOutCurtain();
    _delay_ms(2000);
    rollInCurtain();
    rollInCurtain();
    _delay_ms(2000);
}