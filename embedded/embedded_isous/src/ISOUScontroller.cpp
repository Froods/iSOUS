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
    int actualInTemp  = static_cast<int>(SCD30_.getTemperature());
    int actualOutTemp = static_cast<int>(LM75_.getTempC());
    int actualCO2     = static_cast<int>(SCD30_.getCO2());

    int targetTemp        = settings_.getTargetTemp();
    CO2Setting targetCO2  = settings_.getCO2Setting();

    // ---- 1) Map CO2 setting to a ppm threshold (jf. krav 1.9) ----
    int  co2Threshold = 0;
    bool co2Regulated = true;
    switch (targetCO2) {
        case Minimum_CO2:        co2Threshold = 750;  break;  // < 750 ppm
        case Normalt_indeklima:  co2Threshold = 1000; break;  // < 1000 ppm
        case Hoj:                co2Threshold = 1500; break;  // < 1500 ppm
        case Ureguleret:         co2Regulated = false; break; // vinduesstyring fra
    }

    // ---- 2) Build decision flags (med ±1 °C hysterese, krav 1.8) ----
    bool tooHot   = actualInTemp > (targetTemp + 1);
    bool tooCold  = actualInTemp < (targetTemp - 1);
    bool inBand   = !tooHot && !tooCold;

    bool co2TooHigh        = co2Regulated && (actualCO2 > co2Threshold);
    bool canCoolByVenting  = actualOutTemp < actualInTemp; // ude koldere
    bool canHeatByVenting  = actualOutTemp > actualInTemp; // ude varmere

    // ---- 3) Beslut vinduestilstand ----
    // CO2 har højeste prioritet: skal udluftes uanset temperatur
    if (co2TooHigh) {
        if (!window_.isOpen()) window_.open();
        return;
    }

    if (tooHot && canCoolByVenting) {
        if (!window_.isOpen()) window_.open();
    }
    else if (tooCold && canHeatByVenting) {
        if (!window_.isOpen()) window_.open();
    }
    else if (inBand) {
        // Inden for hysterese-bånd – lad vinduet stå som det står
    }
    else {
        // Ude favoriserer ikke åbning (fx for koldt ude, for varmt inde)
        if (window_.isOpen()) window_.close();
    }
}

void ISOUSController::evaluateCurtain() {
    int actualInTemp = static_cast<int>(SCD30_.getTemperature());
    int actualLight  = static_cast<int>(TSL2561_.getLux());
    int targetTemp   = settings_.getTargetTemp();

    // Tærskel for "solen skinner" – juster efter test (krav siger ikke en specifik værdi)
    const int LIGHT_THRESHOLD = 1000; // lux
    bool sunShining = actualLight > LIGHT_THRESHOLD;

    // ±1 °C hysterese (krav 1.8 – primært møntet på gardinet)
    bool tooHot  = actualInTemp > (targetTemp + 1);
    bool tooCold = actualInTemp < (targetTemp - 1);
    bool inBand  = !tooHot && !tooCold;

    // NB: antagelse om motor-API:
    //   rollOut() = ruller gardin FOR (blokerer sol)
    //   rollIn()  = ruller gardin FRA (lader sol ind)
    //   isOut()   = true hvis gardinet er FOR

    if (tooHot && sunShining) {
        // Bloker sol for at køle ned
        if (!curtain_.isOut()) curtain_.rollOut();
    }
    else if (tooCold && sunShining) {
        // Luk solens varme ind
        if (curtain_.isOut()) curtain_.rollIn();
    }
    else if (tooCold && !sunShining) {
        // Isoler – scenarie 6 i datavariationslisten
        if (!curtain_.isOut()) curtain_.rollOut();
    }
    else if (inBand) {
        // Hysterese: lad gardinet stå
    }
    // tooHot && !sunShining: ingen sol at blokere – ingen handling
}