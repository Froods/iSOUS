#include <Arduino.h>
#include <util/delay.h>
#include "i2c.h"
#include "TSL2561.h"

I2C i2c;
TSL2561 tsl(i2c);   // default 0x39 (ADDR pin floating)

static void banner(const char *s) {
    Serial.println();
    Serial.print(F("----- ")); Serial.print(s); Serial.println(F(" -----"));
}

void setup() {
    Serial.begin(9600);
    _delay_ms(200);
    Serial.println(F("\n=== TSL2561 bring-up ==="));

    // -----------------------------------------------------------------
    // Stage 1: I2C bus init
    // Hvis init_I2C selv hænger her, er det wiring/pull-ups (4.7k typisk
    // mellem SDA/SCL og 3.3V — TSL2561 er IKKE 5V tolerant!).
    // -----------------------------------------------------------------
    banner("Stage 1: I2C init");
    i2c.init_I2C(100000);   // 100 kHz til at starte med
    Serial.println(F("I2C bus initialized at 100kHz"));

    // -----------------------------------------------------------------
    // Stage 2: Address probe
    // Sender bare SLA+W og ser om vi får ACK. Confirmerer at sensoren
    // sidder på 0x39 før vi prøver at læse registre.
    // -----------------------------------------------------------------
    banner("Stage 2: I2C presence at 0x29");
    I2C_Status s = i2c.start();
    if (s != I2C_OK) {
        Serial.print(F("FAIL: start() returned ")); Serial.println(s);
        Serial.println(F("Tjek wiring og pull-up modstande."));
        while (1) {}
    }
    s = i2c.write(0x29 << 1);   // SLA+W
    i2c.stop();
    if (s == I2C_OK) {
        Serial.println(F("PASS: device ACKed at 0x29"));
    } else {
        Serial.print(F("FAIL: no ACK, status = ")); Serial.println(s);
        Serial.println(F("Tjek ADDR-pin (float=0x39, GND=0x29, VCC=0x49)."));
        while (1) {}
    }

    // -----------------------------------------------------------------
    // Stage 3: begin() — verificerer chip-ID og skriver timing register
    // Hvis dette fejler men stage 2 passerede, er det enten en anden
    // chip på 0x39, eller vores command-bit/register-læsning er forkert.
    // -----------------------------------------------------------------
    banner("Stage 3: begin()");
    TSL2561_Status t = tsl.begin();
    if (t == TSL2561_OK) {
        Serial.println(F("PASS: chip-ID matchede"));
    } else if (t == TSL2561_ID_ERROR) {
        Serial.println(F("FAIL: chip-ID matchede ikke (forkert chip eller læsefejl)"));
        while (1) {}
    } else {
        Serial.println(F("FAIL: bus error"));
        while (1) {}
    }

    // -----------------------------------------------------------------
    // Stage 4: Første reading
    // Skal give værdier > 0 medmindre du sidder i totalt mørke.
    // CH0 (broadband) skal altid være >= CH1 (IR).
    // -----------------------------------------------------------------
    banner("Stage 4: First read");
    if (tsl.readData() != TSL2561_OK) {
        Serial.println(F("FAIL: readData() bus error"));
        while (1) {}
    }
    Serial.print(F("CH0 = ")); Serial.println(tsl.getBroadband());
    Serial.print(F("CH1 = ")); Serial.println(tsl.getIR());
    Serial.print(F("lux = ")); Serial.println(tsl.getLux());

    if (tsl.getBroadband() == 0 && tsl.getIR() == 0) {
        Serial.println(F("WARN: begge kanaler er 0 — sensor måske ikke wakede op?"));
    }
    if (tsl.getIR() > tsl.getBroadband()) {
        Serial.println(F("WARN: CH1 > CH0 — det burde aldrig ske, måske byteorden forkert"));
    }

    // -----------------------------------------------------------------
    // Stage 5: Hånd-over-sensor test
    // Tæller ned, så du kan dække sensoren med hånden. Værdierne SKAL
    // falde markant. Hvis de ikke gør, læser vi ikke rigtige data.
    // -----------------------------------------------------------------
    banner("Stage 5: Cover test");
    Serial.println(F("Læg en hånd over sensoren nu..."));
    for (int i = 3; i > 0; i--) {
        Serial.print(i); Serial.print(F("... "));
        _delay_ms(1000);
    }
    Serial.println();
    tsl.readData();
    uint16_t bb_dark = tsl.getBroadband();
    Serial.print(F("Tildækket CH0 = ")); Serial.println(bb_dark);

    Serial.println(F("Fjern hånden..."));
    for (int i = 3; i > 0; i--) {
        Serial.print(i); Serial.print(F("... "));
        _delay_ms(1000);
    }
    Serial.println();
    tsl.readData();
    uint16_t bb_light = tsl.getBroadband();
    Serial.print(F("Fri CH0 = ")); Serial.println(bb_light);

    if (bb_light > bb_dark + 10) {
        Serial.println(F("PASS: sensoren reagerer på lys"));
    } else {
        Serial.println(F("FAIL: ingen forskel mellem mørkt og lyst"));
    }

    Serial.println(F("\n=== Bring-up klar — kører kontinuerligt ==="));
}

void loop() {
    if (tsl.readData() == TSL2561_OK) {
        Serial.print(F("CH0=")); Serial.print(tsl.getBroadband());
        Serial.print(F("  CH1=")); Serial.print(tsl.getIR());
        Serial.print(F("  lux=")); Serial.println(tsl.getLux());
    } else {
        Serial.println(F("read error"));
    }
    _delay_ms(500);
}