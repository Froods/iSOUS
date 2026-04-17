#include "../include/curtain_driver.h"
#include <Arduino.h>

#include <avr/io.h>
#include <util/delay.h>

//Defining motor PWM pin:
// we are using the PWM pin 2 (PE4, OC3B, INT4)
//init pin:
//Making motor pin become output:
void init_Curtain_Motor_pin() {
    pinMode(2, OUTPUT);
}


void setup_pwm() {
    // 1. Sæt Pin 2 (PE4) som OUTPUT
    // DDRE er Data Direction Register for Port E
    DDRE |= (1 << PE4);

    // 2. Konfigurer Timer 3
    // COM3B1: Sæt til 'Non-inverting mode' for kanal B
    // WGM31 & WGM30: En del af Mode 14 (Fast PWM, TOP = ICR3)
    TCCR3A = (1 << COM3B1) | (1 << WGM31);

    // WGM33 & WGM32: Resten af Mode 14
    // CS31: Sæt prescaler til 8
    TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS31);

    // 3. Sæt TOP værdien for at ramme 50 Hz (20ms periode)
    ICR3 = 39999;

    // 4. Startposition: 90 grader (1.5ms puls)
    OCR3B = 3000;
}

void set_servo_angle(float pulse_width_us) {
    // Konverter mikrosekunder til register-værdi
    // Da 20.000 us = 40.000 i registeret, skal vi blot gange med 2
    if (pulse_width_us < 500) pulse_width_us = 500;   // Sikkerhed: min 0.5ms
    if (pulse_width_us > 2500) pulse_width_us = 2500; // Sikkerhed: max 2.5ms
    
    OCR3B = pulse_width_us * 2;
}
