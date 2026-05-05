#include <Arduino.h>
#include <util/delay.h>
#include "i2c.h"
#include "TSL2561.h"
#include "LM75.h"
#include "SCD30.h"   // SCD30

I2C i2c;
TSL2561 tsl(i2c);          // 0x39
LM75    lm(i2c, 0x4D);     // 0x4D
SCD30   scd(i2c);          // 0x61

static void banner(const char *s) {
    Serial.println();
    Serial.print(F("===== ")); Serial.print(s); Serial.println(F(" ====="));
}

// Helper til at probe en adresse uden at læse data
static bool probe(uint8_t addr) {
    if (i2c.start() != I2C_OK) { i2c.stop(); return false; }
    I2C_Status s = i2c.write(addr << 1);
    i2c.stop();
    return s == I2C_OK;
}

void setup() {
    Serial.begin(9600);
    _delay_ms(200);
    Serial.println(F("\n###### Multi-sensor bring-up ######"));

    // -----------------------------------------------------------------
    // Stage 1: Bus init + scan
    // Bekræfter at alle tre sensorer ACKer på deres adresser FØR vi
    // begynder at snakke driver-protokol med dem.
    // -----------------------------------------------------------------
    banner("Stage 1: Bus init + scan");
    i2c.init_I2C(100000);
    Serial.println(F("I2C @ 100kHz"));

    bool tsl_present = probe(0x29);
    bool lm_present  = probe(0x4D);
    bool scd_present = probe(0x61);

    Serial.print(F("  TSL2561 @ 0x29: ")); Serial.println(tsl_present ? F("OK") : F("MISSING"));
    Serial.print(F("  LM75    @ 0x4D: ")); Serial.println(lm_present  ? F("OK") : F("MISSING"));
    Serial.print(F("  SCD30   @ 0x61: ")); Serial.println(scd_present ? F("OK") : F("MISSING"));

    if (!tsl_present || !lm_present || !scd_present) {
        Serial.println(F("FAIL: en eller flere sensorer mangler. Stopper."));
        while (1) {}
    }

    // -----------------------------------------------------------------
    // Stage 2: Init af hver sensor
    // SCD30::begin() starter kontinuerlig måling — den behøver ~5s
    // før første sample er klar, så vi tager den først.
    // -----------------------------------------------------------------
    banner("Stage 2: Init");
    scd.begin();
    Serial.println(F("SCD30 begin() done"));

    if (tsl.begin() != TSL2561_OK) {
        Serial.println(F("FAIL: TSL2561 begin"));
        while (1) {}
    }
    Serial.println(F("TSL2561 begin() done"));

    Serial.println(F("LM75 har ingen begin() — klar til brug"));

    // -----------------------------------------------------------------
    // Stage 3: Individuelle læsninger
    // Læser hver sensor isoleret, så vi kan se at hver enkelt virker
    // før vi blander dem sammen i loop'et.
    // -----------------------------------------------------------------
    banner("Stage 3: Individual reads");

    if (tsl.readData() == TSL2561_OK) {
        Serial.print(F("  TSL2561 lux=")); Serial.print(tsl.getLux());
        Serial.print(F("  CH0="));         Serial.print(tsl.getBroadband());
        Serial.print(F("  CH1="));         Serial.println(tsl.getIR());
    } else {
        Serial.println(F("  TSL2561 read FAIL"));
    }

    if (lm.readData() == LM75_OK) {
        Serial.print(F("  LM75 temp="));   Serial.print(lm.getTempC());
        Serial.println(F(" °C"));
    } else {
        Serial.println(F("  LM75 read FAIL"));
    }

    // SCD30 kan tage op til 20s før første sample er klar efter begin().
    // readData() poller selv data-ready, så den hænger bare lidt.
    Serial.println(F("  SCD30 venter på første sample (kan tage op til 20s)..."));
    SCD30_Status ss = scd.readData();
    if (ss == SCD30_OK) {
        Serial.print(F("  SCD30 CO2=")); Serial.print(scd.getCO2());
        Serial.print(F(" ppm  temp=")); Serial.print(scd.getTemperature());
        Serial.println(F(" °C"));
    } else {
        Serial.print(F("  SCD30 read FAIL, status=")); Serial.println(ss);
    }

    // -----------------------------------------------------------------
    // Stage 4: Round-robin stress
    // Læser alle tre i tæt rækkefølge 5 gange. Hvis bussen er ustabil
    // eller en sensor efterlader bussen i en mærkelig tilstand, vil
    // det vise sig her som læsefejl der ikke optrådte i stage 3.
    // -----------------------------------------------------------------
    banner("Stage 4: Round-robin (5x)");
    for (int i = 0; i < 5; i++) {
        Serial.print(F("--- runde ")); Serial.print(i + 1); Serial.println(F(" ---"));

        TSL2561_Status t = tsl.readData();
        Serial.print(F("  TSL: "));
        if (t == TSL2561_OK) {
            Serial.print(F("lux=")); Serial.println(tsl.getLux());
        } else {
            Serial.println(F("FAIL"));
        }

        LM75_Status l = lm.readData();
        Serial.print(F("  LM75: "));
        if (l == LM75_OK) {
            Serial.print(lm.getTempC()); Serial.println(F(" °C"));
        } else {
            Serial.println(F("FAIL"));
        }

        SCD30_Status sc = scd.readData();
        Serial.print(F("  SCD30: "));
        if (sc == SCD30_OK) {
            Serial.print(F("CO2=")); Serial.print(scd.getCO2());
            Serial.print(F("  T=")); Serial.println(scd.getTemperature());
        } else {
            Serial.print(F("FAIL status=")); Serial.println(sc);
        }
    }

    // -----------------------------------------------------------------
    // Stage 5: Plausibility cross-check
    // SCD30 og LM75 måler begge temperatur. De vil aldrig være helt ens
    // (forskellige placeringer, selv-opvarmning osv.) men de skal være
    // i samme boldgade — typisk inden for et par grader.
    // -----------------------------------------------------------------
    banner("Stage 5: Temp cross-check");
    lm.readData();
    scd.readData();
    float t_lm  = lm.getTempC();
    float t_scd = scd.getTemperature();
    Serial.print(F("  LM75:  ")); Serial.print(t_lm);  Serial.println(F(" °C"));
    Serial.print(F("  SCD30: ")); Serial.print(t_scd); Serial.println(F(" °C"));
    Serial.print(F("  Delta: ")); Serial.print(t_lm - t_scd); Serial.println(F(" °C"));

    float diff = t_lm - t_scd;
    if (diff < 0) diff = -diff;
    if (diff < 5.0f) {
        Serial.println(F("  PASS: temperaturer er konsistente"));
    } else {
        Serial.println(F("  WARN: stor forskel — en af sensorerne er nok forkert"));
    }

    Serial.println(F("\n###### Bring-up klar — kører kontinuerligt ######"));
}

void loop() {
    Serial.println();
    Serial.print(F("Lux: "));
    if (tsl.readData() == TSL2561_OK) Serial.println(tsl.getLux());
    else                              Serial.println(F("err"));

    Serial.print(F("LM75: "));
    if (lm.readData() == LM75_OK) { Serial.print(lm.getTempC()); Serial.println(F(" °C")); }
    else                          { Serial.println(F("err")); }

    Serial.print(F("SCD30: "));
    if (scd.readData() == SCD30_OK) {
        Serial.print(F("CO2=")); Serial.print(scd.getCO2());
        Serial.print(F(" ppm  T=")); Serial.print(scd.getTemperature());
        Serial.println(F(" °C"));
    } else {
        Serial.println(F("err"));
    }

    _delay_ms(2000);
}