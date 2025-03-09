/**
 * @file events.h
 * @brief Header file for astronomical events functionality
 *
 * Provides data structures and functions to retrieve astronomical
 * and space history events for specific dates.
 */

#ifndef ASTROEVENTS_H
#define ASTROEVENTS_H

#include <vector>

/**
 * @struct AstroEvent
 * @brief Data structure to store month, day, and event text
 *
 * This structure represents a significant astronomical or space history
 * event that occurred on a specific date.
 */
struct AstroEvent
{
    int month;        ///< Month (1-12)
    int day;          ///< Day of the month (1-31)
    const char *text; ///< Description of the event
};

// Extern declarations for the array of events and its size.
// 'extern' means they're defined elsewhere (in a .cpp file).
extern const AstroEvent astroEvents[];
extern const int numAstroEvents;

/**
 * @brief Retrieves astronomical events for a specific date
 *
 * This function searches through the astroEvents array and returns
 * all events that match the specified day and month.
 *
 * @param day Day of the month (1-31)
 * @param month Month (1-12)
 * @return std::vector<const char *> Vector of event description strings
 */
std::vector<const char *> getAstroEventsOfTheDay(int day, int month);

#endif // ASTROEVENTS_H
