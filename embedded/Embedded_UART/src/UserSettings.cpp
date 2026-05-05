#include "UserSettings.h"
#include <Arduino.h>

/**
 * @brief constructor for usersettings
 * @param setting The CO2 setting
 * @param temp The target temperature
 * 
 * Usersettings class is used to store the user settings for the CO2 setting and target temperature.
 * It has a constructor to initialize the settings, getters to retrieve the current settings, and setters to update the settings.
 * 
 */
UserSettings::UserSettings(CO2Setting setting, int temp)
    : CO2Setting_(setting), targetTemp_(temp)
{}

/**
 * @brief getter for CO2Setting
 * @return the current CO2 setting
 * 
 * This function returns the current CO2 setting stored in the UserSettings class. 
 * It is a const function, meaning it does not modify any member variables of the class.
 */
CO2Setting UserSettings::getCO2Setting() const {
    return CO2Setting_;
}

/**
 * @brief getter for targetTemp
 * @return the current target temperature
 * 
 * This function returns the current target temperature stored in the UserSettings class.
 * 
 */
int UserSettings::getTargetTemp() const {
    return targetTemp_;
}

int UserSettings::getRoomTemp() const {
    return roomTemp_;
}

int UserSettings::getOutTemp() const {
    return outTemp_;
}

CO2Setting UserSettings::getActualCO2() const {
    return actualCO2_;
}

uint32_t UserSettings::getLight() const {
	return light_;
}

bool UserSettings::isManual() const {
	return manual_;
}

bool UserSettings::getWindowTargetState() const {
	return windowToBeOpen_;
}

bool UserSettings::getCurtainTargetState() const {
	return curtainToBeOpen_;
}

/**
 * @brief setter for CO2Setting
 * @param setting The new CO2 setting to be set
 * @return void
 * 
 * This function sets the CO2 setting for the UserSettings class.
 * It takes a CO2Setting parameter and assigns it to the internal CO2Setting_ variable.
 * 
 */
void UserSettings::setCO2Setting(int co2) {
    if (co2 < 750) {
		CO2Setting_ = CO2Setting::Minimum_CO2;
	} else if (co2 < 1000) {
		CO2Setting_ = CO2Setting::Normalt_indeklima;
	} else {
		CO2Setting_ = CO2Setting::Høj;
	}
}

/**
 * @brief setter for targetTemp
 * @param temp The new target temperature to be set
 * @return void
 * 
 * This function sets the target temperature for the UserSettings class.
 * It takes a float parameter and assigns it to the internal targetTemp_ variable.
 * 
 */
void UserSettings::setTargetTemp(int temp) {
    targetTemp_ = temp;
}


void UserSettings::setRoomTemp(int temp) {
	roomTemp_ = temp;
}

void UserSettings::setOutTemp(int temp) {
	outTemp_ = temp;
}

void UserSettings::setActualCO2(int co2) {
	if (!manual_) {
		if (co2 < 750) {
			actualCO2_ = CO2Setting::Minimum_CO2;
		} else if (co2 < 1000) {
			actualCO2_ = CO2Setting::Normalt_indeklima;
		} else {
			actualCO2_ = CO2Setting::Høj;
		}
	}
}

void UserSettings::setLight(uint32_t light) {
	light_ = light;
}

void UserSettings::enableManual() {
	manual_ = true;
}

void UserSettings::disableManual() {
	manual_ = false;
}

void UserSettings::setWindowTargetState(bool state) {
	windowToBeOpen_ = state;
}

void UserSettings::setCurtainTargetState(bool state) {
	curtainToBeOpen_ = state;
}