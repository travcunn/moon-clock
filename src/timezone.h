/**
 * @file timezone.h
 * @brief Header file for timezone decoding functionality
 *
 * Provides functionality to determine the timezone offset based on
 * geographical coordinates.
 */

#ifndef DECODETIMEZONE_H
#define DECODETIMEZONE_H

/**
 * @brief Determines the timezone offset based on geographical coordinates
 *
 * This function uses the provided latitude and longitude to calculate
 * the appropriate timezone offset in hours from UTC.
 *
 * @param LAT Latitude in decimal degrees (positive for North, negative for South)
 * @param LNG Longitude in decimal degrees (positive for East, negative for West)
 * @return float Timezone offset in hours from UTC
 */
float decodeTimezone(double LAT, double LNG);

#endif // DECODETIMEZONE_H
