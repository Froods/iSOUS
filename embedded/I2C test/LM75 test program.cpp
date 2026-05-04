#include <Arduino.h>
#include <util/delay.h>
#include "i2c.h"
#include "LM75.h"

I2C i2c;
LM75 temp(i2c, 0x4D);   // adresse fundet via scanner

static void banner(const char *s) {
    Serial.println();
    Serial.print(F("----- ")); Serial.print(s); Serial.println(F(" -----"));
}

void setup() {
    Serial.begin(9600);
    _delay_ms(200);
    Serial.println(F("\n=== LM75 bring-up ==="));

    // -----------------------------------------------------------------
    // Stage 1: I2C bus init
    // -----------------------------------------------------------------
    banner("Stage 1: I2C init");
    i2c.init_I2C(100000);
    Serial.println(F("I2C bus initialized at 100kHz"));

    // -----------------------------------------------------------------
    // Stage 2: Address probe
    // Bekræfter at LM75 ACKer på 0x4D før vi prøver at læse registre.
    // -----------------------------------------------------------------
    banner("Stage 2: I2C presence at 0x4D");
    I2C_Status s = i2c.start();
    if (s != I2C_OK) {
        Serial.print(F("FAIL: start() returned ")); Serial.println(s);
        while (1) {}
    }
    s = i2c.write(0x4D << 1);   // SLA+W
    i2c.stop();
    if (s == I2C_OK) {
        Serial.println(F("PASS: device ACKed at 0x4D"));
    } else {
        Serial.print(F("FAIL: no ACK, status = ")); Serial.println(s);
        while (1) {}
    }

    // -----------------------------------------------------------------
    // Stage 3: Første reading
    // Værdien skal være "rumtemperatur" (typisk 18-28°C). Hvis den
    // er præcis 0, eller helt urealistisk (f.eks. -64 eller 127),
    // så går læsningen galt et sted.
    // -----------------------------------------------------------------
    banner("Stage 3: First read");
    if (temp.readData() != LM75_OK) {
        Serial.println(F("FAIL: readData() bus error"));
        while (1) {}
    }
    Serial.print(F("Raw (0.5°C units) = ")); Serial.println(temp.getTempRaw());
    Serial.print(F("Temp = ")); Serial.print(temp.getTempC()); Serial.println(F(" °C"));

    float t = temp.getTempC();
    if (t > 10.0f && t < 40.0f) {
        Serial.println(F("PASS: temperatur ser fornuftig ud for et rum"));
    } else if (t == 0.0f) {
        Serial.println(F("WARN: præcis 0°C — sandsynligvis læsefejl, ikke rigtig måling"));
    } else {
        Serial.println(F("WARN: temperatur uden for forventet rum-interval"));
    }

    // -----------------------------------------------------------------
    // Stage 4: Finger test
    // Læg en finger på sensoren — temperaturen skal stige målbart
    // inden for 10-15 sekunder. Dette beviser at vi læser live data
    // og ikke bare et statisk register.
    // -----------------------------------------------------------------
    banner("Stage 4: Finger test");
    temp.readData();
    float t_before = temp.getTempC();
    Serial.print(F("Før: ")); Serial.print(t_before); Serial.println(F(" °C"));

    Serial.println(F("Læg en finger på sensoren i 15 sekunder..."));
    for (int i = 15; i > 0; i--) {
        Serial.print(i); Serial.print(F("... "));
        _delay_ms(1000);
    }
    Serial.println();

    temp.readData();
    float t_after = temp.getTempC();
    Serial.print(F("Efter: ")); Serial.print(t_after); Serial.println(F(" °C"));
    Serial.print(F("Delta: ")); Serial.print(t_after - t_before); Serial.println(F(" °C"));

    if (t_after - t_before > 0.5f) {
        Serial.println(F("PASS: sensoren reagerer på varme"));
    } else {
        Serial.println(F("FAIL: ingen målbar stigning"));
    }

    Serial.println(F("\n=== Bring-up klar — kører kontinuerligt ==="));
}

void loop() {
    if (temp.readData() == LM75_OK) {
        Serial.print(F("Temp = "));
        Serial.print(temp.getTempC());
        Serial.println(F(" °C"));
    } else {
        Serial.println(F("read error"));
    }
    _delay_ms(1000);
}