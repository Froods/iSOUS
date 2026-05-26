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

/**
 * @brief Central controller for iSOUS systemet.
 *
 * ISOUSController er systemets hjernecentrum og koordinerer alle sensorer
 * og aktuatorer. Klassen læser sensordata, evaluerer beslutningslogik og
 * styrer gardin- og vinduesmotor baseret på brugerens indstillinger fra
 * UserSettings.
 *
 * I automatisk tilstand evalueres vindue og gardin ud fra temperatur,
 * CO2-niveau og lysintensitet med ±1°C hysterese. I manuel tilstand
 * følges brugerens direkte kommandoer fra UserSettings.
 *
 * Opdateringsrækkefølge i update():
 * 1. syncSensorData()     — læser alle sensorer og opdaterer UserSettings
 * 2. evaluateWindow()     — beslutter vinduestilstand (auto tilstand)
 * 3. evaluateCurtain()    — beslutter gardintilstand (auto tilstand)
 * 4. applyManualTargets() — udfører brugerens manuelle kommandoer (manuel tilstand)
 */
class ISOUSController {
private:
    SCD30 SCD30_;               ///< Indendørs CO2 og temperatur sensor
    LM75 LM75_;                 ///< Udendørs temperatur sensor
    TSL2561 TSL2561_;           ///< Udendørs lyssensor
    UserSettings& settings_;    ///< Delt reference til brugerindstillinger og sensordata
    WindowMotor window_;        ///< Driver til vinduesmotor (28BYJ-48)
    CurtainMotor curtain_;      ///< Driver til gardinmotor (MG996R)

	int n = 11;

    /**
     * @brief Evaluerer og opdaterer vinduestilstanden i automatisk tilstand.
     *
     * CO2 har højeste prioritet og åbner vinduet uanset temperatur hvis
     * grænseværdien overskrides. Ellers åbnes vinduet hvis udluftning
     * kan hjælpe med at nå måltemperaturen. Vinduet lukkes hvis
     * udetemperaturen ikke favoriserer åbning.
     * Implementerer ±1°C hysterese omkring måltemperaturen.
     */
    void evaluateWindow();

    /**
     * @brief Evaluerer og opdaterer gardintilstanden i automatisk tilstand.
     *
     * Gardinet rulles for hvis det er for varmt og solen skinner, for at
     * blokere solindstråling. Gardinet rulles fra hvis det er for koldt
     * og solen skinner, for at udnytte passiv solvarme. Ved for koldt
     * uden sol rulles gardinet for for at isolere.
     * Lysgrænse for "solen skinner" er sat til 1000 lux.
     * Implementerer ±1°C hysterese omkring måltemperaturen.
     */
    void evaluateCurtain();

    /**
     * @brief Læser alle sensorer og opdaterer UserSettings med de nye værdier.
     *
     * Læserækkefølge: TSL2561 → LM75 → SCD30.
     * Opdaterer kun UserSettings hvis sensoren returnerer OK status.
     */
    void syncSensorData();

    /**
     * @brief Udfører brugerens manuelle kommandoer fra UserSettings.
     *
     * Kaldes i stedet for evaluateWindow og evaluateCurtain når
     * systemet er i manuel tilstand. Åbner eller lukker vindue og
     * gardin baseret på UserSettings target states.
     */
    void applyManualTargets();

public:
    /**
     * @brief Konstruerer en ISOUSController instans.
     * @param i2c Reference til den delte I2C bus.
     * @param s Reference til det delte UserSettings objekt.
     */
    ISOUSController(I2C &i2c, UserSettings &s);

    /**
     * @brief Initialiserer alle sensorer.
     *
     * Starter SCD30 i continuous measurement mode og venter 2500ms
     * på at sensoren producerer sin første måling. Initialiserer
     * derefter TSL2561 og verificerer chip-ID.
     */
    void init();

    /**
     * @brief Hovedopdateringsfunktion der skal kaldes i hver iteration af main-løkken.
     *
     * Synkroniserer sensordata og evaluerer herefter enten automatisk
     * styring eller udfører manuelle kommandoer afhængigt af tilstanden
     * i UserSettings.
     */
    void update();
};

#endif