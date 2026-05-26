#pragma once
#include <Arduino.h>
#include "UserSettings.h"
#include "CO2Setting.h"

/**
 * @brief Håndterer UART kommunikation mellem Arduino og GUI (TKinter).
 *
 * Klassen modtager 4-byte kommandopakker fra GUI'en via Serial,
 * parser dem og udfører den tilsvarende handling på UserSettings.
 * Svar sendes som 6-byte pakker tilbage til GUI'en.
 *
 * Kommandopakke format (GUI -> Arduino): [CMD, PAR1, PAR2, STOPBYTE]
 * Svarpakke format (Arduino -> GUI):     [TYPE_ID, DATA1, DATA2, DATA3, DATA4, STOPBYTE]
 */
class UARTinterface {
public:
    /**
     * @brief Konstruerer en UARTinterface instans og initialiserer Serial.
     * @param baudrate Baudrate for Serial kommunikation (f.eks. 9600).
     * @param settings Reference til det delte UserSettings objekt.
     */
	UARTinterface(uint32_t baudrate, UserSettings& settings);

    /**
     * @brief Initialiserer Serial med den givne baudrate.
     * @param baudrate Baudrate for Serial kommunikation.
     */
	void init(uint32_t baudrate);

    /**
     * @brief Checker om der er data tilgængeligt og læser det ind i bufferen.
     *
     * Skal kaldes i hver iteration af main-løkken for at sikre
     * at indkommende kommandoer behandles løbende.
     */
	void update();

private:

	// Attributter
	char buffer_[64];       ///< Ringbuffer til indkommende bytes
	int bufferHead_ = 0;    ///< Antal bytes i bufferen
	bool auto_ = false;     ///< Intern auto-tilstand (ubrugt i nuværende implementation)

    // Kommando ID'er (GUI -> Arduino)
	static const char CMD_SET_DESIRED_VALUES = 0x01;    ///< Sæt ønsket temperatur og CO2-niveau
	static const char CMD_GET_ROOM_TEMP = 0x02;         ///< Hent indendørs temperatur
	static const char CMD_GET_ROOM_CO2 = 0x03;          ///< Hent CO2-niveau
	static const char CMD_GET_OUTSIDE_TEMP = 0x04;      ///< Hent udendørs temperatur
	static const char CMD_GET_LIGHT = 0x05;             ///< Hent lysintensitet
	static const char CMD_SET_WINDOW_STATE = 0x06;      ///< Sæt vinduestilstand manuelt
	static const char CMD_SET_CURTAIN_STATE = 0x07;     ///< Sæt gardintilstand manuelt
	static const char TOGGLE_MANUAL = 0X08;             ///< Skift mellem manuel og automatisk tilstand
	static const char CMD_GET_WINDOW_STATE = 0x09;      ///< Hent aktuel vinduestilstand
	static const char CMD_GET_CURTAIN_STATE = 0x0A;     ///< Hent aktuel gardintilstand

    // Svar TYPE_ID'er (Arduino -> GUI)
	static const char ROOM_TEMP_ID = 0x01;      ///< Type ID for indendørs temperatur svar
	static const char ROOM_CO2_ID = 0x02;       ///< Type ID for CO2-niveau svar
	static const char OUTSIDE_TEMP_ID = 0x03;   ///< Type ID for udendørs temperatur svar
	static const char LIGHT_ID = 0x04;          ///< Type ID for lysintensitet svar (4 bytes payload)
	static const char WINDOW_STATE = 0x05;      ///< Type ID for vinduestilstand svar
	static const char CURTAIN_STATE = 0x06;     ///< Type ID for gardintilstand svar

	const char STOPBYTE = 0xFF;             ///< Stopbyte der afslutter alle pakker
	const char PARAMETER_OMITTED = 0x00;    ///< Bruges når et parameter ikke er relevant
	const int EXPECTED_BYTES = 4;           ///< Forventet antal bytes per kommandopakke
	UserSettings& settings_;                ///< Reference til det delte UserSettings objekt

	// Metoder
    /**
     * @brief Parser den første kommando i bufferen og udfører den tilsvarende handling.
     *
     * Læser CMD, PAR1 og PAR2 fra bufferen og behandler kommandoen
     * via et switch-statement. Sender svar tilbage til GUI hvis relevant.
     */
	void parseCommand();

    /**
     * @brief Sender en 6-byte svarpakke til GUI via Serial.
     * @param command Pointer til char array med 6 bytes der skal sendes.
     */
	void sendResponse(char* command);

    /**
     * @brief Checker om der er bytes tilgængelige på Serial.
     * @return true hvis der er data klar til læsning.
     */
	bool dataAvailable();

    /**
     * @brief Læser 4 bytes fra Serial ind i bufferen og parser kommandoen.
     */
	void readIntoBuffer();

    /**
     * @brief Skifter auto-tilstand (intern, ubrugt i nuværende implementation).
     */
	void toggleAuto();

    /**
     * @brief Tilføjer 4 bytes til bufferen.
     * @param bytes Pointer til de 4 bytes der skal tilføjes.
     */
	void pushToBuffer(char* bytes);

    /**
     * @brief Fjerner og returnerer de første 4 bytes fra bufferen.
     *
     * Shifter de resterende bytes ned så bufferen forbliver konsistent.
     * @param outArr Pointer til array hvor de 4 bytes skrives til.
     */
	void removeFromBuffer(char* outArr);
};