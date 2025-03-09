/**
 * @file timezone.cpp
 * @brief Implementation of timezone decoding functionality
 *
 * This file contains the implementation of the decodeTimezone function
 * which determines the timezone offset based on geographical coordinates.
 * The implementation uses a data structure of timezone regions to efficiently
 * determine the appropriate timezone for a given latitude and longitude.
 */

#include <cmath> // For round() function

/**
 * @struct TimezoneRegion
 * @brief Structure representing a geographical region with a specific timezone offset
 *
 * Each TimezoneRegion defines a rectangular area on the globe with minimum and maximum
 * latitude and longitude boundaries, and the corresponding timezone offset in hours.
 */
struct TimezoneRegion
{
    double minLat; ///< Minimum latitude (southern boundary)
    double maxLat; ///< Maximum latitude (northern boundary)
    double minLng; ///< Minimum longitude (western boundary)
    double maxLng; ///< Maximum longitude (eastern boundary)
    float offset;  ///< Timezone offset in hours from UTC
};

/**
 * @brief Array of timezone regions with their boundaries and offsets
 *
 * This array defines the major timezone regions used for determining the
 * appropriate timezone offset for a given geographical location.
 * Each entry specifies a rectangular region and its corresponding UTC offset.
 */
const TimezoneRegion TIMEZONE_REGIONS[] = {
    // UTC+14 Regions (Line Islands, Kiribati)
    {0.0, 4.0, -160.5, -158.0, 14.0},
    {-6.0, 4.0, -158.0, -150.0, 14.0},
    {-12.0, -6.0, -155.0, -150.0, 14.0},

    // UTC+13 Regions (Samoa, Tonga, parts of New Zealand)
    {-6.0, -2.5, -179.0, -158.0, 13.0},
    {-10.0, -6.0, -179.0, -169.0, 13.0},
    {-15.0, -10.0, -174.0, -171.0, 13.0},
    {-26.0, -15.0, -178.5, -172.5, 13.0},
    {-26.0, -22.0, -179.99, -178.5, 13.0},

    // UTC+12:45 Regions (Chatham Islands)
    {-45.0, -42.0, -179.9, -172.5, 12.75},

    // UTC+12 Regions (New Zealand, Fiji, parts of Russia)
    {59.0, 90.0, 172.5, 180.0, 12.0},
    {70.0, 75.0, -179.9, -172.5, 12.0},
    {63.5, 70.0, -179.9, -168.0, 12.0},
    {54.0, 70.0, 162.0, 172.5, 12.0},
    {20.0, 48.0, 172.5, 180.0, 12.0},
    {9.0, 20.0, 160.0, 180.0, 12.0},
    {-3.0, 9.0, 165.0, 180.0, 12.0},
    {-8.0, -3.0, 172.5, 180.0, 12.0},
    {-15.0, -8.0, -179.9, -179.0, 12.0},
    {-15.0, -8.0, 172.5, 180.0, 12.0},
    {-42.0, -26.0, 172.5, 180.0, 12.0},
    {-42.0, -26.0, -179.9, -172.5, 12.0},
    {-53.0, -42.0, 165.0, 180.0, 12.0},
    {-60.0, -53.0, 172.5, 180.0, 12.0},

    // UTC+11 Regions (Eastern parts of Russia, Solomon Islands, Vanuatu)
    {70.0, 90.0, 157.5, 172.5, 11.0},
    {65.0, 72.38, 141.0, 150.0, 11.0},
    {70.0, 72.38, 150.0, 157.5, 11.0},
    {20.0, 51.0, 157.5, 172.5, 11.0},
    {9.0, 20.0, 157.5, 160.0, 11.0},
    {3.0, 9.0, 154.0, 165.0, 11.0},
    {-10.0, -3.0, 154.0, 172.5, 11.0},
    {-30.0, -10.0, 157.5, 172.5, 11.0},
    {-53.0, -32.0, 157.5, 165.0, 11.0},
    {-60.0, -53.0, 157.5, 172.5, 11.0},

    // UTC+10:30 Regions (Lord Howe Island)
    {-32.0, -30.0, 157.5, 165.0, 10.5},

    // UTC+10 Regions (Eastern Australia, Papua New Guinea, parts of Russia)
    {77.0, 90.0, 142.5, 157.5, 10.0},
    {73.0, 77.0, 153.0, 157.5, 10.0},
    {72.38, 73.0, 130.0, 157.5, 10.0},
    {65.0, 72.38, 130.0, 141.0, 10.0},
    {30.0, 42.8, 145.26, 157.5, 10.0},
    {24.5, 30.0, 150.0, 157.5, 10.0},
    {16.0, 30.0, 142.5, 150.0, 10.0},
    {-15.0, 16.0, 140.9, 154.0, 10.0},
    {-38.0, -15.0, 140.9, 157.5, 10.0},
    {-60.0, -38.0, 142.5, 157.5, 10.0},

    // UTC+9:30 Regions (Central Australia)
    {-15.0, -11.0, 127.5, 140.9, 9.5},
    {-33.0, -15.0, 129.0, 138.0, 9.5},
    {-33.0, -26.0, 138.0, 140.9, 9.5},
    {-38.0, -33.0, 127.5, 140.9, 9.5},

    // UTC+9 Regions (Japan, Korea, Eastern Indonesia, parts of Russia)
    {77.0, 90.0, 127.5, 142.5, 9.0},
    {73.0, 77.0, 115.5, 153.0, 9.0},
    {69.5, 73.0, 110.0, 130.0, 9.0},
    {65.0, 69.5, 105.5, 130.0, 9.0},
    {30.0, 42.8, 124.2, 145.26, 9.0},
    {23.0, 30.0, 123.0, 127.5, 9.0},
    {16.0, 30.0, 127.5, 142.5, 9.0},
    {7.5, 16.0, 127.5, 136.0, 9.0},
    {-11.0, 7.5, 127.5, 140.9, 9.0},
    {-11.0, 0.0, 127.5, 125.5, 9.0},

    // UTC+8:45 Regions (Western Australia - Eucla)
    {-33.0, -30.0, 126.0, 129.0, 8.75},

    // UTC+8 Regions (China, Western Australia, Philippines, Malaysia, Taiwan)
    {77.0, 90.0, 115.5, 127.5, 8.0},
    {58.6, 65.0, 105.5, 109.0, 8.0},
    {53.0, 58.3, 97.0, 105.5, 8.0},
    {42.8, 49.5, 98.4, 131.0, 8.0},
    {34.0, 41.4, 75.0, 82.5, 8.0},
    {31.3, 45.0, 82.5, 97.0, 8.0},
    {22.26, 30.0, 98.7, 123.0, 8.0},
    {15.0, 22.26, 108.0, 123.0, 8.0},
    {0.0, 23.0, 123.0, 127.5, 8.0},
    {1.2, 15.0, 112.5, 123.0, 8.0},
    {0.0, 1.2, 112.5, 123.0, 8.0},
    {-9.0, 0.0, 114.6, 125.5, 8.0},
    {-30.0, -11.0, 112.5, 127.5, 8.0},
    {-33.0, -30.0, 112.5, 126.0, 8.0},
    {-60.0, -33.0, 112.5, 127.5, 8.0},

    // UTC+7 Regions (Western Indonesia, Thailand, Vietnam, parts of Russia)
    {81.6, 90.0, 97.5, 112.5, 7.0},
    {79.0, 81.6, 75.0, 112.5, 7.0},
    {77.0, 79.0, 78.2, 112.5, 7.0},
    {71.4, 77.0, 78.2, 110.0, 7.0},
    {65.0, 71.4, 82.5, 105.5, 7.0},
    {9.92, 20.0, 98.7, 108.0, 7.0},
    {9.92, 15.0, 108.0, 112.5, 7.0},
    {6.67, 9.92, 97.5, 112.5, 7.0},
    {0.0, 6.67, 95.0, 99.0, 7.0},
    {0.0, 1.2, 101.0, 112.5, 7.0},
    {-9.0, 1.2, 112.5, 114.6, 7.0},
    {-60.0, 0.0, 97.5, 112.5, 7.0},

    // UTC+6:30 Regions (Myanmar/Burma, Cocos Islands)
    {25.54, 28.8, 97.0, 98.7, 6.5},
    {20.0, 25.54, 92.5, 98.7, 6.5},
    {15.0, 20.0, 87.4, 92.5, 6.5},
    {9.92, 20.0, 97.5, 98.7, 6.5},
    {-15.0, -11.5, 96.5, 97.5, 6.5},

    // UTC+6 Regions (Bangladesh, Bhutan, parts of Russia, Kazakhstan)
    {81.6, 90.0, 82.5, 97.5, 6.0},
    {54.51, 58.3, 69.64, 75.8, 6.0},
    {50.0, 54.51, 60.6, 78.2, 6.0},
    {41.1, 50.0, 67.5, 82.0, 6.0},
    {26.37, 29.6, 88.4, 92.5, 6.0},
    {20.0, 25.54, 88.4, 92.5, 6.0},
    {15.0, 20.0, 87.4, 92.5, 6.0},
    {6.67, 15.0, 82.5, 90.0, 6.0},
    {0.0, 6.67, 82.5, 95.0, 6.0},
    {-60.0, -15.0, 82.5, 97.5, 6.0},

    // UTC+5:45 Regions (Nepal)
    {26.37, 31.3, 80.0, 88.4, 5.75},

    // UTC+5:30 Regions (India, Sri Lanka)
    {31.3, 34.0, 75.0, 79.2, 5.5},
    {26.37, 31.3, 75.0, 80.0, 5.5},
    {25.54, 26.37, 82.5, 92.5, 5.5},
    {20.0, 29.6, 70.0, 75.0, 5.5},
    {5.66, 26.37, 75.0, 82.5, 5.5},
    {15.0, 20.0, 82.5, 87.4, 5.5},
    {8.0, 12.0, 71.5, 75.0, 5.5},
    {6.67, 15.0, 90.0, 95.0, 5.5},

    // UTC+5 Regions (Pakistan, parts of Russia, Kazakhstan, Uzbekistan)
    {83.0, 90.0, 67.5, 82.5, 5.0},
    {79.0, 83.0, 67.5, 75.0, 5.0},
    {61.0, 79.0, 69.64, 78.2, 5.0},
    {54.51, 61.0, 54.4, 69.64, 5.0},
    {37.27, 50.0, 60.6, 67.5, 5.0},
    {34.0, 37.27, 71.5, 75.0, 5.0},
    {31.3, 34.0, 69.2, 75.0, 5.0},
    {29.6, 31.3, 67.5, 75.0, 5.0},
    {20.0, 25.77, 67.5, 70.0, 5.0},
    {12.0, 20.0, 67.5, 71.5, 5.0},
    {-60.0, -8.0, 67.5, 82.5, 5.0},

    // UTC+4:30 Regions (Afghanistan)
    {34.0, 37.27, 60.6, 71.5, 4.5},
    {29.6, 34.0, 60.6, 70.0, 4.5},

    // UTC+4 Regions (Azerbaijan, Armenia, UAE, Oman, parts of Russia)
    {83.0, 90.0, 52.5, 67.5, 4.0},
    {52.4, 54.51, 46.0, 51.7, 4.0},
    {47.7, 52.4, 42.13, 46.0, 4.0},
    {39.4, 43.2, 44.0, 48.9, 4.0},
    {22.84, 25.77, 51.3, 67.5, 4.0},
    {20.0, 22.0, 55.0, 67.5, 4.0},
    {13.0, 20.0, 52.5, 67.7, 4.0},
    {-60.0, 11.0, 52.5, 67.5, 4.0},

    // UTC+3:30 Regions (Iran)
    {37.27, 39.4, 44.0, 48.9, 3.5},
    {33.0, 37.27, 45.5, 51.3, 3.5},
    {33.0, 37.27, 51.3, 60.6, 3.5},
    {29.96, 33.0, 47.86, 51.3, 3.5},

    // UTC+3 Regions (Moscow, Saudi Arabia, Eastern Africa)
    {83.0, 90.0, 37.5, 52.5, 3.0},
    {75.0, 83.0, 37.5, 69.64, 3.0},
    {63.0, 71.4, 30.0, 60.0, 3.0},
    {54.51, 63.0, 28.0, 51.7, 3.0},
    {45.0, 52.4, 40.0, 42.5, 3.0},
    {37.27, 41.3, 26.0, 44.0, 3.0},
    {33.0, 37.27, 41.2, 44.0, 3.0},
    {29.96, 33.0, 37.5, 47.86, 3.0},
    {22.84, 29.96, 37.5, 51.3, 3.0},
    {9.68, 22.84, 37.5, 52.5, 3.0},
    {-4.0, 9.68, 30.65, 52.5, 3.0},
    {-17.38, -10.86, 40.67, 52.5, 3.0},
    {-45.0, -17.38, 37.5, 52.5, 3.0},
    {-60.0, -47.23, 37.5, 52.5, 3.0},

    // UTC+2 Regions (Eastern Europe, Egypt, South Africa)
    {81.0, 90.0, 22.5, 37.5, 2.0},
    {71.4, 75.0, 22.5, 37.5, 2.0},
    {63.0, 69.7, 22.5, 30.0, 2.0},
    {54.51, 63.0, 19.8, 28.0, 2.0},
    {45.0, 50.0, 21.67, 40.0, 2.0},
    {41.3, 45.0, 22.5, 40.0, 2.0},
    {33.0, 39.7, 19.8, 26.0, 2.0},
    {33.0, 37.27, 26.0, 41.2, 2.0},
    {21.0, 33.0, 9.5, 37.5, 2.0},
    {9.68, 21.0, 23.9, 37.5, 2.0},
    {-17.38, -6.0, 22.0, 30.65, 2.0},
    {-30.0, -17.38, 11.58, 37.5, 2.0},
    {-60.0, -34.9, 22.5, 37.5, 2.0},

    // UTC+1 Regions (Western Europe, Central Africa)
    {63.0, 90.0, 7.5, 22.5, 1.0},
    {57.5, 63.0, 3.7, 19.8, 1.0},
    {50.0, 54.0, 2.3, 19.8, 1.0},
    {33.0, 50.0, -5.0, 19.8, 1.0},
    {21.0, 33.0, -18.75, 9.5, 1.0},
    {14.0, 21.0, 4.2, 23.9, 1.0},
    {6.0, 14.0, 1.96, 23.9, 1.0},
    {-17.38, 0.0, 7.0, 22.0, 1.0},
    {-30.0, -17.38, 7.5, 11.58, 1.0},
    {-60.0, -34.9, 7.5, 22.5, 1.0},

    // UTC+0 Regions (UK, Ireland, Portugal, Western Africa, Iceland)
    {63.0, 90.0, -7.5, 7.5, 0.0},
    {54.0, 60.0, -11.25, 3.7, 0.0},
    {50.0, 54.0, -11.25, 2.3, 0.0},
    {0.0, 25.0, -18.75, 0.0, 0.0},
    {-50.0, -1.5, -7.5, 7.5, 0.0},

    // UTC-1 Regions (Azores, Cape Verde)
    {84.0, 90.0, -22.5, -7.5, -1.0},
    {67.0, 71.5, -22.5, -7.5, -1.0},
    {45.0, 63.0, -22.5, -15.0, -1.0},
    {-60.0, 45.0, -22.5, -18.75, -1.0},

    // UTC-2 Regions (Mid-Atlantic)
    {84.0, 90.0, -37.5, -22.5, -2.0},
    {63.0, 67.0, -30.0, -25.0, -2.0},
    {40.0, 63.0, -37.5, -22.5, -2.0},
    {-60.0, 40.0, -37.5, -22.5, -2.0},

    // UTC-3 Regions (Argentina, Eastern Brazil, Greenland)
    {-9.3, 90.0, -52.5, -37.5, -3.0},
    {-60.0, -9.3, -52.5, -37.5, -3.0},

    // UTC-3:30 Regions (Newfoundland)
    {47.2, 52.0, -57.0, -52.5, -3.5},
    {46.0, 47.2, -55.0, -52.5, -3.5},

    // UTC-4 Regions (Eastern Canada, Eastern Caribbean, Chile)
    {54.0, 60.0, -64.0, -52.5, -4.0},
    {-21.5, 48.3, -67.5, -59.0, -4.0},
    {-60.0, -52.7, -67.5, -52.5, -4.0},

    // UTC-5 Regions (Eastern US, Colombia, Peru)
    {79.2, 90.0, -85.0, -67.5, -5.0},
    {28.0, 76.0, -85.0, -67.5, -5.0},
    {-60.0, 28.0, -82.5, -67.5, -5.0},

    // UTC-6 Regions (Central US, Central America)
    {80.0, 90.0, -97.5, -85.0, -6.0},
    {71.0, 80.0, -103.0, -85.0, -6.0},
    {49.0, 60.0, -110.0, -90.0, -6.0},
    {28.0, 49.0, -100.0, -90.0, -6.0},
    {-60.0, 28.0, -97.5, -82.5, -6.0},

    // UTC-7 Regions (Mountain US, Western Canada)
    {80.0, 90.0, -112.5, -97.5, -7.0},
    {71.0, 80.0, -127.5, -103.0, -7.0},
    {49.0, 55.0, -116.0, -110.0, -7.0},
    {28.0, 45.0, -114.0, -100.0, -7.0},
    {-60.0, 28.0, -112.5, -97.5, -7.0},

    // UTC-8 Regions (Pacific US, Western Canada)
    {79.0, 90.0, -127.5, -112.5, -8.0},
    {55.0, 60.0, -129.0, -116.0, -8.0},
    {45.0, 55.0, -135.0, -116.0, -8.0},
    {28.0, 45.0, -127.5, -114.0, -8.0},
    {-60.0, 24.0, -127.5, -112.5, -8.0},

    // UTC-9 Regions (Alaska)
    {71.0, 90.0, -142.5, -127.5, -9.0},
    {51.5, 71.0, -168.0, -141.0, -9.0},
    {45.0, 51.5, -142.5, -135.0, -9.0},
    {-60.0, 45.0, -142.5, -127.5, -9.0},

    // UTC-9:30 Regions (Marquesas Islands)
    {-13.0, -6.0, -142.5, -135.0, -9.5},

    // UTC-10 Regions (Hawaii, French Polynesia)
    {71.0, 90.0, -157.5, -142.5, -10.0},
    {30.0, 48.0, -157.5, -142.5, -10.0},
    {15.0, 30.0, -172.5, -142.5, -10.0},
    {-60.0, 15.0, -157.5, -142.5, -10.0},

    // UTC-11 Regions (American Samoa, Niue)
    {71.0, 90.0, -172.5, -157.5, -11.0},
    {30.0, 48.0, -172.5, -157.5, -11.0},
    {-2.5, 15.0, -172.5, -160.5, -11.0},
    {-60.0, -2.5, -172.5, -157.5, -11.0},

    // UTC-12 Regions (Baker Island, Howland Island)
    {75.0, 90.0, -179.99, -172.5, -12.0},
    {-2.5, 24.7, -179.99, -172.5, -12.0},
    {-90.0, -45.0, -179.99, -172.5, -12.0}};

/**
 * @brief Number of timezone regions in the TIMEZONE_REGIONS array
 */
const int NUM_TIMEZONE_REGIONS = sizeof(TIMEZONE_REGIONS) / sizeof(TimezoneRegion);

/**
 * @brief Determines the timezone offset based on geographical coordinates
 *
 * This function searches through the TIMEZONE_REGIONS array to find a region
 * that contains the given latitude and longitude coordinates. If a matching
 * region is found, its timezone offset is returned. If no matching region is
 * found, a simple approximation based on longitude is used as a fallback.
 *
 * @param LAT Latitude in decimal degrees (positive for North, negative for South)
 * @param LNG Longitude in decimal degrees (positive for East, negative for West)
 * @return float Timezone offset in hours from UTC
 */
float decodeTimezone(double LAT, double LNG)
{
    // Normalize longitude to -180 to 180 range
    if (LNG > 180.0)
        LNG -= 360.0;
    if (LNG < -180.0)
        LNG += 360.0;

    // Search for a matching timezone region
    for (int i = 0; i < NUM_TIMEZONE_REGIONS; i++)
    {
        const TimezoneRegion &region = TIMEZONE_REGIONS[i];

        if (LAT >= region.minLat && LAT <= region.maxLat &&
            LNG >= region.minLng && LNG <= region.maxLng)
        {
            return region.offset;
        }
    }

    // Fallback: approximate timezone based on longitude
    // Each 15 degrees of longitude corresponds to about 1 hour of timezone difference
    float approximateOffset = round(LNG / 15.0);

    return approximateOffset;
}
