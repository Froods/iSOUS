#include "ISOUScontroller.h"

ISOUSController::ISOUSController(I2C &i2c, UserSettings &s)
    : SCD30_(i2c),
      LM75_(i2c),
      TSL2561_(i2c),
      settings_(s),
      window_(),
      curtain_()
{}


void ISOUSController::init() {
    SCD30_.begin();
    _delay_ms(2500);                          // let SCD30 produce first sample
    TSL2561_Status ts = TSL2561_.begin();
}

void ISOUSController::update() {
    if (!mockMode_) syncSensorData();   // springes over i test
    if (settings_.isManual()) applyManualTargets();
    else { evaluateWindow(); evaluateCurtain(); }
}

void ISOUSController::syncSensorData() {
     if (SCD30_.readData() == SCD30_OK) {
    settings_.setRoomTemp(static_cast<int>(SCD30_.getTemperature()));
    settings_.setActualCO2(static_cast<int>(SCD30_.getCO2()));
    mockCO2ppm_ = static_cast<int>(SCD30_.getCO2());   // <-- ny
}
}

void ISOUSController::evaluateWindow() {
    int actualInTemp  = settings_.getRoomTemp();
    int actualOutTemp = settings_.getOutTemp();
    int actualCO2 = mockCO2ppm_;
    

    int targetTemp        = settings_.getTargetTemp();
    CO2Setting targetCO2  = settings_.getCO2Setting();

    // ---- 1) Map CO2 setting to a ppm threshold (jf. krav 1.9) ----
    int  co2Threshold = 0;
    bool co2Regulated = true;
    switch (targetCO2) {
        case CO2Setting::Minimum_CO2:        co2Threshold = 750;  break;  // < 750 ppm
        case CO2Setting::Normalt_indeklima:  co2Threshold = 1000; break;  // < 1000 ppm
        case CO2Setting::Hoej:                co2Threshold = 1500; break;  // < 1500 ppm
        case CO2Setting::Ureguleret:         co2Regulated = false; break; // vinduesstyring fra
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
        if (!window_.getIsOpen()){
        window_.openWindow();
        settings_.setWindowTargetState(true);
        }
        return;
    }

    if (tooHot && canCoolByVenting) {
        if (!window_.getIsOpen()){
        window_.openWindow();
        settings_.setWindowTargetState(true);
        }
    }
    else if (tooCold && canHeatByVenting) {
        if (!window_.getIsOpen()){
        window_.openWindow();
        settings_.setWindowTargetState(true);
        }
    }
    else if (inBand) {
        // Inden for hysterese-bånd – lad vinduet stå som det står
    }
    else {
        // Ude favoriserer ikke åbning (fx for koldt ude, for varmt inde)
        if (window_.getIsOpen()){
        window_.closeWindow();
        settings_.setWindowTargetState(false);
        }
    }
}

void ISOUSController::evaluateCurtain() {
    int actualInTemp = settings_.getRoomTemp();
    int actualLight  = static_cast<int>(settings_.getLight());
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
            if (!curtain_.getIsOut()){ 
            curtain_.rollOutCurtain();
            settings_.setCurtainTargetState(false);
            }
    }
    else if (tooCold && sunShining) {
            // Luk solens varme ind
            if (curtain_.getIsOut()){
            curtain_.rollInCurtain();
            settings_.setCurtainTargetState(true);
            }
        }
    else if (tooCold && !sunShining) {
        // Isoler – scenarie 6 i datavariationslisten
        if (!curtain_.getIsOut()){
        curtain_.rollOutCurtain();
        settings_.setCurtainTargetState(false);
        }
    }
    else if (inBand) {
        // Hysterese: lad gardinet stå
    }
    // tooHot && !sunShining: ingen sol at blokere – ingen handling
    }




void ISOUSController::applyManualTargets() {
    bool wantWindowOpen = settings_.getWindowTargetState();
    if (wantWindowOpen && !window_.getIsOpen())  window_.openWindow();
    if (!wantWindowOpen && window_.getIsOpen())  window_.closeWindow();

    bool wantCurtainOpen = settings_.getCurtainTargetState();
    if (!wantCurtainOpen && !curtain_.getIsOut())  curtain_.rollOutCurtain();
    if (wantCurtainOpen && curtain_.getIsOut())  curtain_.rollInCurtain();
}

