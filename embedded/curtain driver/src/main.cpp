#include <Arduino.h>
#include "../include/curtain_driver.h"

int main () {

    init_Curtain_Motor_pin();
    
    setup_pwm();

    while (1) {
        // Eksempel: Drej fra 0 til 180 grader
        set_servo_angle(500);  // 0 grader
        _delay_ms(1000);
        
        set_servo_angle(1500); // 90 grader
        _delay_ms(1000);
        
        set_servo_angle(2500); // 180 grader
        _delay_ms(1000);

}
    return 0;
}