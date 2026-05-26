#ifndef USERSETTINGS_H
#define USERSETTINGS_H
#include <Arduino.h>
#include "CO2Setting.h"

/**
 * @brief Centralt datadelingsobjekt mellem ISOUSController og UARTinterface.
 *
 * Klassen indeholder både brugerens ønskede indstillinger (måltemperatur og
 * CO2-niveau) samt de seneste sensoraflæsninger. Den fungerer som et passivt
 * dataobjekt uden egen logik, og deles som reference mellem ISOUSController
 * og UARTinterface, så begge klasser altid arbejder på den samme tilstand.
 */
class UserSettings {

private:

	bool windowToBeOpen_ = false;       ///< Ønsket vinduestilstand (true = åben)
    CO2Setting CO2Setting_;             ///< Brugerens ønskede CO2-niveau
    int targetTemp_;                    ///< Brugerens ønskede måltemperatur i °C

	CO2Setting actualCO2_;              ///< Senest målte CO2-niveau (konverteret til enum)
	int roomTemp_ = 0;                  ///< Senest målte indendørstemperatur i °C
	int outTemp_ = 0;                   ///< Senest målte udendørstemperatur i °C
	uint32_t light_;                    ///< Senest målte lysintensitet i lux

	bool curtainToBeOpen_ = true;       ///< Ønsket gardintilstand (true = åben)
	bool manual_ = false;               ///< true = manuel tilstand, false = automatisk


public:
    /**
     * @brief Konstruerer et UserSettings objekt med standardindstillinger.
     * @param setting Brugerens ønskede CO2-niveau som CO2Setting enum.
     * @param temp Brugerens ønskede måltemperatur i °C.
     */
    UserSettings(CO2Setting setting, int temp);

    // Getters
	// For ISOUScontroller
    /**
     * @brief Returnerer brugerens ønskede CO2-niveau.
     * @return CO2Setting enum værdi.
     */
    CO2Setting getCO2Setting() const;

    /**
     * @brief Returnerer brugerens ønskede måltemperatur.
     * @return Måltemperatur i °C.
     */
    int getTargetTemp() const;

    /**
     * @brief Returnerer om systemet er i manuel tilstand.
     * @return true hvis manuel, false hvis automatisk.
     */
	bool isManual() const;

    /**
     * @brief Returnerer den ønskede vinduestilstand.
     * @return true hvis vinduet skal være åbent, false hvis lukket.
     */
	bool getWindowTargetState() const;

    /**
     * @brief Returnerer den ønskede gardintilstand.
     * @return true hvis gardinet skal være åbent, false hvis lukket.
     */
	bool getCurtainTargetState() const;

	// For UARTinterface
    /**
     * @brief Returnerer senest målte indendørstemperatur.
     * @return Indendørstemperatur i °C.
     */
	int getRoomTemp() const;

    /**
     * @brief Returnerer senest målte udendørstemperatur.
     * @return Udendørstemperatur i °C.
     */
	int getOutTemp() const;

    /**
     * @brief Returnerer senest målte CO2-niveau som enum.
     * @return CO2Setting enum værdi baseret på seneste måling.
     */
	CO2Setting getActualCO2() const;

    /**
     * @brief Returnerer senest målte lysintensitet.
     * @return Lysintensitet i lux som uint32_t.
     */
	uint32_t getLight() const;

    // Setters
	// For ISOUScontroller
    /**
     * @brief Opdaterer senest målte indendørstemperatur.
     * @param temp Ny indendørstemperatur i °C.
     */
	void setRoomTemp(int temp);

    /**
     * @brief Opdaterer senest målte udendørstemperatur.
     * @param temp Ny udendørstemperatur i °C.
     */
	void setOutTemp(int temp);

    /**
     * @brief Konverterer rå CO2-ppm værdi til CO2Setting enum og gemmer den.
     *
     * Opdateres kun når systemet ikke er i manuel tilstand.
     * Grænseværdier: under 750 ppm = Minimum_CO2, under 1000 ppm = Normalt_indeklima, ellers Hoej.
     * @param co2 Rå CO2-koncentration i ppm.
     */
	void setActualCO2(int co2);

    /**
     * @brief Opdaterer senest målte lysintensitet.
     * @param light Lysintensitet i lux.
     */
	void setLight(uint32_t light);

    /**
     * @brief Sætter den ønskede vinduestilstand.
     * @param state true = vindue skal åbnes, false = vindue skal lukkes.
     */
	void setWindowTargetState(bool state);

    /**
     * @brief Sætter den ønskede gardintilstand.
     * @param state true = gardin skal åbnes, false = gardin skal lukkes.
     */
	void setCurtainTargetState(bool state);

	// For UARTinterface
    /**
     * @brief Sætter brugerens ønskede CO2-niveau fra en heltalsværdi modtaget via UART.
     *
     * Mapping: 0 = Ureguleret, 1 = Minimum_CO2, 2 = Normalt_indeklima, 3 = Hoej.
     * @param co2 Heltal modtaget fra GUI via UART.
     */
    void setCO2Setting(int co2);

    /**
     * @brief Sætter brugerens ønskede måltemperatur.
     * @param temp Ny måltemperatur i °C.
     */
    void setTargetTemp(int temp);

    /**
     * @brief Aktiverer manuel tilstand.
     *
     * Når manuel tilstand er aktiv, ignoreres automatisk regulering
     * og systemet følger brugerens direkte kommandoer.
     */
	void enableManual();

    /**
     * @brief Deaktiverer manuel tilstand og genaktiverer automatisk regulering.
     */
	void disableManual();
};

#endif