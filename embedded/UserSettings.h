#ifndef USERSETTINGS_H
#define USERSETTINGS_H


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
    CO2Setting CO2Setting_;
    float targetTemp_;

public:
    // Constructor
    UserSettings(CO2Setting setting, float temp);

    // Getters
    CO2Setting getCO2Setting() const;
    float getTargetTemp() const;

    // Setters
    void setCO2Setting(CO2Setting setting);
    void setTargetTemp(float temp);


};

#endif