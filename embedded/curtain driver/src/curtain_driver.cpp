#include "../include/curtain_driver.h"
#include <Arduino.h>

#include <avr/io.h>
#include <util/delay.h>

volatile bool isOut = false;

void initCurtainMotorPWM() {

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

void initTimer2Timer() {

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

void delayForCutain() {
    TIFR2 |= (1<<0); //reset timer overflow flag
    TCNT2 = 255-156; //make timer only take 156 steps
    int numberOfOverflows = 0; //int to keep track of overflows
    while (numberOfOverflows != 849) { //stay here indtil 849 overflows has passed
        while ((TIFR2 & (1<<0)) == 0) {} //wait for the timer to reach an overflow (sets up flag)
        TIFR2 |= (1<<0);//reset timer overflow flag
        TCNT2 = 255-156;//make timer only take 156 steps
        numberOfOverflows++; //increments numberofoverflow when overflow happen
    }
}

void rollOutCurtain() {
    if (isOut == false) {
        OCR3B = 37999;
        delayForCutain();
        OCR3B = 36999;
        isOut = true;   
    } else {
        //Do nothing
    }
}

void rollInCurtain() {
    if (isOut == true) {
        OCR3B = 35999;
        delayForCutain();
        OCR3B = 36999;
        isOut = false;
    } else {
        //Do nothing
    }
}

void test_of_curtain() {
    rollOutCurtain();
    rollOutCurtain();
    _delay_ms(2000);
    rollInCurtain();
    rollInCurtain();
    _delay_ms(2000);
}