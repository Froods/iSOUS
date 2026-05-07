#include "ISOUScontroller.h"

ISOUSController::ISOUSController(I2C &i2c, UserSettings &s)
    : SCD30_(i2c),
      LM75_(i2c),
      TSL2561_(i2c),
      settings_(s),
      window(),
      curtain()
{}


void ISOUSController::syncSensorData(){ 
    //Lav en ny måling for hver sensor
    if (SCD30_.readData() == SCD30_OK) {
        settings_.setRoomTemp(SCD30_.getTemperature());
        settings_.setActualCO2(SCD30_.getCO2());
    }
    if(LM75_.readData() == LM75_OK){
        settings_.setOutTemp(LM75_.getTempC());
    }
    if(TSL2561_.readData() == TSL2561_OK){
        settings_.setLight(TSL2561_.getLux());
    }

}

void ISOUSController::evaluateWindow() {
	int actualInTemp = static_cast<int>(SCD30_.getTemperature());
	int actualOutTemp = static_cast<int>(LM75_.getTempC());
	int actualCO2 = static_cast<int>(SCD30_.getCO2());
	int actualLight = static_cast<int>(TSL2561_.getLux());

	int targetTemp = settings_.getTargetTemp();
	CO2Setting targetCO2 = settings_.getCO2Setting();

	
}
