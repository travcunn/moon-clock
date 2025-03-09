/**
 * @file events.cpp
 * @brief Implementation of astronomical events functionality
 *
 * Contains a database of significant astronomical and space history events
 * organized by date, and functions to retrieve events for specific dates.
 */

#include <vector>

struct AstroEvent
{
    int month;
    int day;
    const char *text;
};

/**
 * @brief Database of astronomical and space history events
 *
 * This array contains significant events in astronomy and space exploration
 * history, organized by month and day. Each entry includes the month, day,
 * and a text description of the event.
 */
static const AstroEvent astroEvents[] = {
    // --- January (1) ---
    {1, 1, "1801: Piazzi discovers Ceres (first asteroid)"},
    {1, 1, "1929: Edwin Hubble publishes expansion law (Hubble's Law)"},
    {1, 2, "1959: Luna 1 passes near the Moon"},
    {1, 4, "2004: Spirit rover lands on Mars"},
    {1, 7, "1610: Galileo observes Jupiter's 4 largest moons"},
    {1, 8, "1942: Stephen Hawking is born"},
    {1, 31, "1961: Ham the Chimp launched (Mercury-Redstone 2)"},

    // --- February (2) ---
    {2, 1, "2003: Space Shuttle Columbia disaster (STS-107)"},
    {2, 14, "1990: Voyager 1's 'Pale Blue Dot' photo"},
    {2, 18, "1930: Clyde Tombaugh discovers Pluto"},
    {2, 20, "1962: John Glenn orbits Earth (Friendship 7)"},

    // --- March (3) ---
    {3, 10, "1974: Stephen Hawking proposes black hole radiation (Hawking Radiation)"},
    {3, 13, "1781: William Herschel discovers Uranus"},
    {3, 14, "1879: Albert Einstein is born (Pi Day)"},
    {3, 16, "1926: Goddard launches first liquid-fueled rocket"},
    {3, 18, "1965: Alexei Leonov performs first spacewalk"},
    {3, 23, "1965: Gemini 3, first crewed Gemini flight"},

    // --- April (4) ---
    {4, 10, "2019: First black hole image (M87*) from Event Horizon Telescope"},
    {4, 12, "1961: Yuri Gagarin, first human in space (Vostok 1)"},
    {4, 12, "1981: STS-1 Columbia, first Space Shuttle flight"},
    {4, 24, "1990: Hubble Space Telescope launched"},

    // --- May (5) ---
    {5, 5, "1961: Alan Shepard, first American in space"},
    {5, 14, "1973: Skylab launched"},
    {5, 20, "1964: Penzias & Wilson detect cosmic microwave background (CMB)"},
    {5, 25, "1961: JFK proposes Moon landing goal to Congress"},
    {5, 29, "1919: Eddington's eclipse confirms Einstein's relativity"},

    // --- June (6) ---
    {6, 2, "1966: Surveyor 1 lands on the Moon"},
    {6, 16, "1963: Valentina Tereshkova, first woman in space"},
    {6, 18, "1983: Sally Ride, first American woman in space"},
    {6, 30, "1905: Einstein publishes Special Relativity"},

    // --- July (7) ---
    {7, 4, "2012: Discovery of the Higgs boson announced at the LHC"},
    {7, 14, "2015: New Horizons flies by Pluto"},
    {7, 16, "1969: Apollo 11 launched (Saturn V)"},
    {7, 20, "1969: Apollo 11 lands on the Moon"},
    {7, 20, "1976: Viking 1 lands on Mars"},
    {7, 21, "1969: Neil Armstrong's first steps on the Moon (UTC)"},

    // --- August (8) ---
    {8, 6, "1967: Jocelyn Bell Burnell detects the first pulsar"},
    {8, 6, "2012: Curiosity rover lands on Mars"},
    {8, 20, "1975: Viking 1 launched to Mars"},
    {8, 24, "2006: Pluto reclassified as a dwarf planet"},

    // --- September (9) ---
    {9, 5, "1977: Voyager 1 launched"},
    {9, 12, "1959: Luna 2 first spacecraft to impact the Moon"},
    {9, 12, "1992: Mae Jemison, first African American woman in space"},
    {9, 14, "2015: LIGO detects gravitational waves"},
    {9, 23, "1846: Johann Galle observes Neptune for the first time"},

    // --- October (10) ---
    {10, 1, "1958: NASA is officially formed"},
    {10, 4, "1957: Sputnik 1, first artificial satellite"},
    {10, 4, "1959: Luna 3 photographs far side of the Moon"},

    // --- November (11) ---
    {11, 2, "2000: First resident crew arrives at ISS (Expedition 1)"},
    {11, 3, "1957: Laika, first animal to orbit Earth (Sputnik 2)"},
    {11, 25, "1915: Einstein presents General Relativity field equations"},
    {11, 28, "1964: Mariner 4 launched to Mars"},

    // --- December (12) ---
    {12, 7, "1972: Apollo 17 launched (last crewed Moon mission)"},
    {12, 14, "1962: Mariner 2, first successful planetary encounter (Venus)"},
    {12, 14, "1972: Eugene Cernan, last man on the Moon (Apollo 17)"},
    {12, 21, "1968: Apollo 8 launched, first crew to orbit the Moon"},
    {12, 25, "2021: James Webb Space Telescope launched"},
    {12, 30, "1924: Edwin Hubble announces galaxies beyond the Milky Way"},
};

/**
 * @brief Number of events in the astroEvents array
 *
 * Calculated by dividing the total size of the array by the size of a single element.
 */
static const int numAstroEvents = sizeof(astroEvents) / sizeof(astroEvents[0]);

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
std::vector<const char *> getAstroEventsOfTheDay(int day, int month)
{
    std::vector<const char *> matches;
    for (int i = 0; i < numAstroEvents; i++)
    {
        if (astroEvents[i].day == day && astroEvents[i].month == month)
        {
            matches.push_back(astroEvents[i].text);
        }
    }
    return matches;
}
