#ifndef ISOUSCONTROLLER_H
#define ISOUSCONTROLLER_H

#include "SCD30.h"
#include "lm75.h"
#include "TSL2561.h"
#include "UserSettings.h"
#include "window_driver.h"
#include "curtain_driver.h"
#include "I2C.h"
#include "CO2Setting.h"

class ISOUSController {
private:
    SCD30 SCD30_;
    LM75 LM75_;
    TSL2561 TSL2561_;
    UserSettings& settings_;   // shared ifølge UML
    WindowMotor window;
    CurtainMotor curtain;

	int n = 11;

    void evaluateWindow();
    void evaluateCurtain();
    void syncSensorData();

public:
    ISOUSController(I2C &i2c, UserSettings &s);

    void update();
};



#endif