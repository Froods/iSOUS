#ifndef USERSETTINGS_H
#define USERSETTINGS_H
#include <Arduino.h>

/**
 * @brief CO2Setting enum to represent different CO2 settings
 * 
 * This enum class represents the different CO2 settings that can be used in the UserSettings class.
 * The options include Ureguleret, Høj, Normalt_indeklima, and Minimum_CO2.
 * 
 */
enum class CO2Setting {
    Ureguleret,
    Høj,
    Normalt_indeklima,
    Minimum_CO2,
};

/**
 * @brief UserSettings class to store user settings for CO2 and target temperature
 * 
 * This class is used to store the user settings for the CO2 setting and target temperature.
 * It has a constructor to initialize the settings, getters to retrieve the current settings,
 * and setters to update the settings.
 * 
 */
class UserSettings {

private:

	bool windowToBeOpen_;
    CO2Setting CO2Setting_;
    int targetTemp_;

	CO2Setting actualCO2_;
	int roomTemp_ = 0;
	int outTemp_ = 0;
	uint32_t light_;

	bool curtainToBeOpen_;
	bool manual_ = false;


public:
    // Constructor
    UserSettings(CO2Setting setting, int temp);

    // Getters
	// For ISOUScontroller
    CO2Setting getCO2Setting() const;
    int getTargetTemp() const;
	bool isManual() const;
	bool getWindowTargetState() const;
	bool getCurtainTargetState() const;

	// For UARTinterface
	int getRoomTemp() const;
	int getOutTemp() const;
	CO2Setting getActualCO2() const;
	uint32_t getLight() const;

    // Setters
	// For ISOUScontroller
	void setRoomTemp(int temp);
	void setOutTemp(int temp);
	void setActualCO2(int co2);
	void setLight(uint32_t light);
	void setWindowTargetState(bool state);
	void setCurtainTargetState(bool state);

	// For UARTinterface
    void setCO2Setting(int co2);
    void setTargetTemp(int temp);
	void enableManual();
	void disableManual();


};

#endif