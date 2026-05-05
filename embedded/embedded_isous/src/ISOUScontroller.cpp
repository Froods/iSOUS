#include "ISOUScontroller.h"

ISOUSController::ISOUSController(I2C &i2c, UserSettings &s)
    : SCD30_(i2c),
      LM75_(i2c),
      TSL2561_(i2c),
      settings(s),
      window(),
      curtain()
{}


void ISOUScontroller::syncSensorData(){ 
    //Lav en ny måling for hver sensor
    if (SCD30_.readData() == SCD30_OK) {
        settings.setRoomTemp(SCD30_.getTemperature());
        settings.setActualCo2(SCD30_.getCO2());
    }
    if(LM75_.readdata() == LM75_OK){
        settings.setOutTemp(LM75_.getTempC());
    }
    if(TSL2561_.readData() == TSL2561_OK){
        settings.setLight(TSL2561_.getLux());
    }

}