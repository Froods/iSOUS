#pragma once

/**
 * @class WindowMotor
 * @brief Class to control window motor
 * 
 * Saves the state of the window in a private attribute. Used to open and close the window
 */
class WindowMotor {

private:

    /**
     * @brief Defining motor pins
     * we are using the digital pins 22 (Blue), 24 (Pink), 26 (Yellow), 28 (Orange)
     */
    int motor_Pins[4] = {22,24,26,28};

  
    /**
     * @brief 
     * stages to make the motor spin with the 4 coils
     * The O,Y,P,B stands for Orange, Yellow, Pink & Blue and is a refenrence to the cables on the ULN2003A
     */
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

    /**
     * @brief Bool to save the state of the window. Used to avoid "open" or "close" twice in a row
     */
    bool isOpen = false;

public:

    /**
     * @brief Constructor for WindowMotor. Inits motorpins and Init isOpen to "false".
     */
    WindowMotor();

    /**
     * @brief init all 4 windowpins as output pins.
     */
    void initWindowMotorPins();

    /**
     * @brief Moves motor 1 stage, takes direction as parameter.
     * @param direction Bool. 0 = anti-clockwise direction, 1 = clockwise direction
     */
    void moveMotor1Stage (bool direction);

    /**
     * @brief Open the window by taking 4096 steps in a clockwise rotation (plus-sequence)
     */
    void openWindow();

    /**
     * @brief Close the window by taking 4096 steps in a anti-clockwise (minus-sequence) 
     */
    void closeWindow();

    /**
     * @brief Used only in "modultest" to test functionality of the code.
     */
    void test_Of_Motor();

};