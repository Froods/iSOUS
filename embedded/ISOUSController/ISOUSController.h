#pragma once

class SCD30;
class TempSensorOutside;
class LightSensor;
class UserSettings;
class WindowMotor;
class CurtainMotor;

/**
 * @brief Controller class for coordinating sensors, user settings, window and curtain.
 *
 * ISOUSController is the high level class from the UML diagram. It receives references
 * to the existing subsystem objects and uses update() to evaluate actuator behavior.
 */
class ISOUSController {

private:
    SCD30& scd30_;
    TempSensorOutside& tempOutside_;
    LightSensor& light_;
    UserSettings& settings_;
    WindowMotor& window_;
    CurtainMotor& curtain_;

    void evaluateWindow();
    void evaluateCurtain();

public:
    ISOUSController(SCD30& scd30,
                    TempSensorOutside& tempOutside,
                    LightSensor& light,
                    UserSettings& settings,
                    WindowMotor& window,
                    CurtainMotor& curtain);

    void update();
};
