#ifndef ASTROEVENTS_H
#define ASTROEVENTS_H

#include <vector>

// Data structure to store month, day, and event text.
struct AstroEvent
{
    int month;
    int day;
    const char *text;
};

// Extern declarations for the array of events and its size.
// 'extern' means they're defined elsewhere (in a .cpp file).
extern const AstroEvent astroEvents[];
extern const int numAstroEvents;

// Function prototype: returns a vector of event strings
// that match the given day/month.
std::vector<const char *> getAstroEventsOfTheDay(int day, int month);

#endif // ASTROEVENTS_H
