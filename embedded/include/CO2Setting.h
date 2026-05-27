#pragma once

/**
 * @brief Enum der repræsenterer de mulige CO2-niveauer i systemet.
 *
 * Bruges i UserSettings til at angive både brugerens ønskede CO2-grænse
 * og det senest målte CO2-niveau. ISOUSController mapper enum-værdien
 * til en konkret ppm-grænseværdi når vinduesstyringen evalueres.
 *
 * Mapping til ppm-grænseværdier (jf. ISOUSController::evaluateWindow):
 * - Minimum_CO2:       < 750 ppm
 * - Normalt_indeklima: < 1000 ppm
 * - Hoej:              < 1500 ppm
 * - Ureguleret:        Ingen grænse, vinduesstyring baseret på CO2 deaktiveret
 */
enum class CO2Setting {
    Ureguleret,         ///< Ingen CO2-regulering, vinduet styres udelukkende af temperatur
    Hoej,               ///< Høj CO2-tolerance, grænse ved 1500 ppm
    Normalt_indeklima,  ///< Normal CO2-grænse for godt indeklima, grænse ved 1000 ppm
    Minimum_CO2         ///< Lav CO2-tolerance for bedst mulig luftkvalitet, grænse ved 750 ppm
};