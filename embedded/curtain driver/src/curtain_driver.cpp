#include "../include/curtain_driver.h"
#include <Arduino.h>

#include <avr/io.h>
#include <util/delay.h>

volatile bool isOut = false;

void initCurtainMotorPWM() {
    //Defining motor PWM pin:
    //we are using the PWM pin 2 (PE4, OC3B, INT4) as OUTPUT
    //Making motor pin become output:
    DDRE |= (1 << PE4);

    //set mode 14 (Fast PWM, TOP = ICR3)
    TCCR3A = (1 << WGM31);
    TCCR3B = (1 << WGM33);
    TCCR3B = (1 << WGM32); 

    //use COM3B1 to make timer B run in 'inverting mode'
    TCCR3A = (1 << COM3B0);
    TCCR3A = (1 << COM3B1);

    //use CS31 to make prescaler 8:
    TCCR3B = (1 << CS31);

    //Make the top value 39999 to get 50 hz (20 ms period)
    ICR3 = 39999;

    //init start position by making the duty cycle 1,5 ms. 
    //this makes the motor stand stil
    OCR3B = 36999;
}

void rollOutCurtain() {
    if (isOut == false) {
        OCR3B = 37999;
        //indsæt ordentligt delay med timere:
        _delay_ms(4000);
        isOut = true;
    } else {
        //Do nothing
    }
}

void roolInCurtain() {
    if (isOut == true) {
        OCR3B = 35999;
        //indsæt ordentligt delay med timere:
        _delay_ms(4000);
        isOut = false;
    } else {
        //Do nothing
    }
}

void test_of_curtain() {
    rollOutCurtain();
    _delay_ms(1000);
    roolInCurtain();
    _delay_ms(1000);
}