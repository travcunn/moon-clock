/**
 * @file moonphase.h
 * @brief Header file for the moon phase calculation library
 *
 * Provides functionality to calculate various properties of the moon
 * including phase, age, illumination fraction, distance, and position
 * in the zodiac.
 */

#ifndef MoonPhase_h
#define MoonPhase_h

#include <time.h>

// Constants for moon calculations
#define MOON_SYNODIC_PERIOD 29.530588853   // Period of moon cycle in days.
#define MOON_SYNODIC_OFFSET 2451550.26     // Reference cycle offset in days.
#define MOON_DISTANCE_PERIOD 27.55454988   // Period of distance oscillation
#define MOON_DISTANCE_OFFSET 2451562.2     // Reference distance offset in days
#define MOON_LATITUDE_PERIOD 27.212220817  // Latitude oscillation period in days
#define MOON_LATITUDE_OFFSET 2451565.2     // Reference latitude offset in days
#define MOON_LONGITUDE_PERIOD 27.321582241 // Longitude oscillation period in days
#define MOON_LONGITUDE_OFFSET 2451555.8    // Reference longitude offset in days

/**
 * @class _MoonPhase
 * @brief Class for calculating and storing moon phase and position information
 *
 * This class provides methods to calculate various properties of the moon
 * based on a given time, including its phase, age, illumination fraction,
 * distance from Earth, position in the sky, and zodiac constellation.
 */
class _MoonPhase
{
public:
    double jDate;           ///< Julian date
    double phase;           ///< Moon phase (0-1, 0=new, 0.5=full)
    double age;             ///< Age in days of current lunar cycle
    double fraction;        ///< Fraction of illuminated disk (0-1)
    double distance;        ///< Moon distance in earth radii
    double latitude;        ///< Moon ecliptic latitude in degrees
    double longitude;       ///< Moon ecliptic longitude in degrees
    const char *phaseName;  ///< Text name of phase (New, Full, etc.)
    const char *zodiacName; ///< Zodiac constellation name

    /**
     * @brief Constructor for _MoonPhase class
     *
     * Initializes all member variables to default values.
     */
    _MoonPhase();

    /**
     * @brief Calculate moon properties for the specified time
     *
     * @param t Unix timestamp (seconds since Jan 1, 1970)
     */
    void calculate(time_t t);

private:
    /**
     * @brief Convert Unix time to Julian date
     *
     * @param t Unix timestamp (seconds since Jan 1, 1970)
     * @return double Julian date
     */
    double _julianDate(time_t t);
};
#endif
