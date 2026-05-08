#ifndef ISOUSCONTROLLER_H
#define ISOUSCONTROLLER_H

#include "CO2Sensor.h"
#include "TempSensorInside.h"
#include "TempSensorOutside.h"
#include "LightSensor.h"
#include "UserSettings.h"
#include "WindowMotor.h"
#include "CurtainMotor.h"

class ISOUSController {
private:
    CO2Sensor co2;
    TempSensorInside temp_inside;
    TempSensorOutside temp_outside;
    LightSensor light;
    UserSettings& settings;   // shared ifølge UML
    WindowMotor window;
    CurtainMotor curtain;

    void evaluateWindow();
    void evaluateCurtain();

public:
    ISOUSController(CO2Sensor c,
                    TempSensorInside ti,
                    TempSensorOutside to,
                    LightSensor l,
                    UserSettings& s,
                    WindowMotor w,
                    CurtainMotor cu);

    void update();
};

#endif