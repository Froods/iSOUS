#pragma once    

/**
 * @class CurtainMotor
 * @brief Class to control curtain motor
 * 
 * Saves the state of the curtain in a private attribute. Used to roll in og roll out the curtain
 */
class CurtainMotor {

private:

    /**
     * @brief Bool to save the state of the curtain. Used to avoid double roll in
     */
    bool isOut = false;

public:

    /**
     * @brief Contructor for curtainmotor. Inits both timer 2 and timer 3. Init isOut to false.
     */
    CurtainMotor();

    /**
     * @brief init timer 3 to use for PWM signal for servo motor. Motor stands still
     */
    void initCurtainMotorPWM();

    /**
     * @brief roll out curtain by turning motor clockwise for 8,53 sec
     */
    void rollOutCurtain();

    /**
     * @brief roll in curtain by turning motor anti-clockwise for 8,53 sec
     */
    void rollInCurtain();

    /**
     * @brief Test of roll out/in functions and if statements
     */
    void test_of_curtain();

    /**
     * @brief Init timer 2 to use for precision delay.
     */
    void initTimer2Timer();

    /**
     * @brief Function that returns the current state of the curtain.
     */
	bool getIsOut();

};


