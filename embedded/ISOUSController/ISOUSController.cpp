#include "ISOUSController.h"

ISOUSController::ISOUSController(SCD30& scd30,
                                 TempSensorOutside& tempOutside,
                                 LightSensor& light,
                                 UserSettings& settings,
                                 WindowMotor& window,
                                 CurtainMotor& curtain)
    : scd30_(scd30),
      tempOutside_(tempOutside),
      light_(light),
      settings_(settings),
      window_(window),
      curtain_(curtain)
{}

void ISOUSController::update() {
    evaluateWindow();
    evaluateCurtain();
}

void ISOUSController::evaluateWindow() {
    // Window logic is added when the sensor and motor interfaces are implemented.
}

void ISOUSController::evaluateCurtain() {
    // Curtain logic is added when the light sensor interface is implemented.
}
