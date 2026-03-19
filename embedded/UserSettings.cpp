#include "UserSettings.h"

/**
 * @brief constructor for usersettings
 * @param setting The CO2 setting
 * @param temp The target temperature
 * 
 * Usersettings class is used to store the user settings for the CO2 setting and target temperature.
 * It has a constructor to initialize the settings, getters to retrieve the current settings, and setters to update the settings.
 * 
 */
UserSettings::UserSettings(CO2Setting setting, float temp)
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
float UserSettings::getTargetTemp() const {
    return targetTemp_;
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
void UserSettings::setCO2Setting(CO2Setting setting) {
    CO2Setting_ = setting;
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
void UserSettings::setTargetTemp(float temp) {
    targetTemp_ = temp;
}
