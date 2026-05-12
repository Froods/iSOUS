#include <Arduino.h>
#include "I2C.h"
#include "UserSettings.h"
#include "ISOUScontroller.h"
#include "CO2Setting.h"

static void printResult(int num, const UserSettings& s) {
    Serial.print(F("Scenarie "));
    Serial.print(num);
    Serial.print(F(" -> Vindue: "));
    Serial.print(s.getWindowTargetState() ? F("AABEN") : F("LUKKET"));
    Serial.print(F(" | Gardin: "));
    Serial.println(s.getCurtainTargetState() ? F("FRA") : F("FOR"));
}

static void runScenario(int num, ISOUSController& ctrl, UserSettings& s,
                        int tIn, int tOut, int co2, uint32_t lux) {
    Serial.print(F("\n--- Scenarie ")); Serial.print(num); Serial.println(F(" ---"));
    Serial.print(F("Tin="));  Serial.print(tIn);
    Serial.print(F("C  Tud=")); Serial.print(tOut);
    Serial.print(F("C  CO2=")); Serial.print(co2);
    Serial.print(F(" ppm  Lux=")); Serial.println(lux);

    // Fodr systemet med mock data
    s.setRoomTemp(tIn);
    s.setOutTemp(tOut);
    s.setLight(lux);
    ctrl.setMockCO2ppm(co2);

    // Lad controlleren beslutte
    ctrl.update();

    // Vent saa motorerne fysisk faar tid til at koere
    _delay_ms(3000);

    printResult(num, s);
}

int main() {
    init();
    Serial.begin(9600);
    _delay_ms(500);

    I2C i2c{};
    i2c.init_I2C(100000);

    // Praekondition fra accepttesten:
    //   onsket temperatur = 21 C
    //   onsket CO2 niveau = 1000 ppm  ->  Normalt_indeklima
    UserSettings s(CO2Setting::Normalt_indeklima, 21);
    ISOUSController ctrl(i2c, s);

    ctrl.setMockMode(true);     // VIGTIGT: skal sættes FOR init()
    ctrl.init();                // i mockMode er init et no-op

    Serial.println(F("=== UC2 Accepttest: Automatisk styring (mock data) ==="));
    Serial.println(F("Target: 21 C, 1000 ppm (Normalt_indeklima)"));

    // Scenarie 1: forventet -> Vindue LUKKET + Gardin FOR
    runScenario(1, ctrl, s, /*Tin*/15, /*Tud*/10, /*CO2*/1000, /*Lux*/500);

    // Scenarie 2: forventet -> Vindue AABEN + Gardin FRA
    runScenario(2, ctrl, s, /*Tin*/15, /*Tud*/25, /*CO2*/1500, /*Lux*/10000);

    Serial.println(F("\n=== Test slut ==="));

    while (1) { /* idle */ }
}