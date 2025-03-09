/**
 * @file main.cpp
 * @brief Main program for e-ink display clock with moon phase visualization
 *
 * This program displays time, date, and moon phase information on an e-ink display.
 * It uses GPS for location and time data, and can show special content on specific dates.
 */

#include <GxEPD2_3C.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <time.h>

#include "moonphase.h"
#include "timezone.h"
#include "events.h"

#include "bitmaps/edwin_hubble.h"
#include "bitmaps/moon.h"
#include "bitmaps/steve_hawking.h"

// ===== CONFIGURATION =====

/**
 * @struct DisplayConfig
 * @brief Configuration for the e-ink display
 */
struct DisplayConfig
{
  const int16_t width = 800;
  const int16_t height = 480;
};

/**
 * @struct GPSConfig
 * @brief Configuration for the GPS module
 */
struct GPSConfig
{
  const int rxPin = D10;
  const int txPin = D11;
  const uint32_t baudRate = 9600;
};

/**
 * @struct SleepConfig
 * @brief Configuration for device sleep behavior
 */
struct SleepConfig
{
  const uint64_t sleepTimeSeconds = 3600; // Sleep for 1 hour
};

/**
 * @struct LocationConfig
 * @brief Configuration for location-specific settings
 */
struct LocationConfig
{
  const String hemisphere = "north";
  String units = "M";
};

/**
 * @struct MoonPhaseText
 * @brief Text descriptions for different moon phases
 */
struct MoonPhaseText
{
  const String newMoon = "New Moon";
  const String waxingCrescent = "Evening Crescent";
  const String firstQuarter = "First Quarter";
  const String waxingGibbous = "Waxing Gibbous";
  const String fullMoon = "Full Moon";
  const String waningGibbous = "Waning Gibbous";
  const String thirdQuarter = "Third Quarter";
  const String waningCrescent = "Morning Crescent";
};

// Global configuration instances
DisplayConfig displayConfig;
GPSConfig gpsConfig;
SleepConfig sleepConfig;
LocationConfig locationConfig;
MoonPhaseText moonText;

// Global variables
bool isTimeConfigured = false;
_MoonPhase moonPhase;

// Hardware instances
GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display(GxEPD2_750c_Z08(/*CS=5*/ D9, /*DC=*/SCL, /*RST=*/SDA, /*BUSY=*/D7));
TinyGPSPlus gps;
SoftwareSerial gpsSerial(gpsConfig.rxPin, gpsConfig.txPin);

// ===== DISPLAY UTILITIES =====

/**
 * @brief Draws a string at a specific position on the display
 *
 * @param x The x-coordinate position to start drawing the text
 * @param y The y-coordinate position to start drawing the text
 * @param text The string to be displayed
 */
void drawString(int x, int y, String text)
{
  display.setCursor(x, y);
  display.print(text);
}

/**
 * @brief Draws text centered horizontally on the display
 *
 * @param text The text to be displayed
 * @param yBaseline The y-coordinate baseline position for the text
 */
void drawCenteredText(const char *text, int16_t yBaseline)
{
  // Measure the text width & height at the current font settings
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, yBaseline, &x1, &y1, &w, &h);

  // Compute the left coordinate for centered text
  int16_t xCenter = (displayConfig.width - w) / 2;

  // Set the cursor and print
  display.setCursor(xCenter, yBaseline);
  display.println(text);
}

/**
 * @brief Initializes the e-ink display with the required settings
 */
void initDisplay()
{
  display.init(115200, true, 2, false);
  display.setRotation(0);
  display.setTextSize(1);
  display.setTextColor(GxEPD_BLACK);
  display.setTextWrap(false);
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
  display.firstPage();
}

// ===== TIME & DATE UTILITIES =====

/**
 * @brief Sets the system time based on provided date and time values
 *
 * @param year Year (e.g., 2025)
 * @param month Month (1-12)
 * @param day Day of the month (1-31)
 * @param hour Hour (0-23)
 * @param minute Minute (0-59)
 * @param second Second (0-59)
 */
void setSystemTime(int year, int month, int day, int hour, int minute, int second)
{
  struct tm tmTime;
  tmTime.tm_year = year - 1900;
  tmTime.tm_mon = month - 1;
  tmTime.tm_mday = day;
  tmTime.tm_hour = hour;
  tmTime.tm_min = minute;
  tmTime.tm_sec = second;

  time_t timeToSet = mktime(&tmTime);
  struct timeval tv = {.tv_sec = timeToSet};

  settimeofday(&tv, nullptr);
}

/**
 * @brief Formats a number as a two-digit string with leading zero if needed
 *
 * @param value The number to format
 * @return String The formatted two-digit string
 */
String formatTwoDigits(int value)
{
  return (value < 10) ? "0" + String(value) : String(value);
}

// ===== MOON PHASE VISUALIZATION =====

/**
 * @brief Displays the astronomy section with moon phase information
 *
 * @param x The x-coordinate position to start drawing the section
 * @param y The y-coordinate position to start drawing the section
 */
void displayAstronomySection(int x, int y)
{
  display.setFont(&FreeSans12pt7b);
  display.setTextSize(1);

  // Get current UTC time
  time_t now = time(NULL);
  struct tm *now_utc = gmtime(&now);
  const int day_utc = now_utc->tm_mday;
  const int month_utc = now_utc->tm_mon + 1;
  const int year_utc = now_utc->tm_year + 1900;

  // Calculate moon phase
  time_t utcOffset = mktime(now_utc) - now;
  moonPhase.calculate(now + utcOffset);

  // Display moon phase information
  drawString(x + 30, y, moonPhase.phaseName);
  drawString(x + 30, y + 90, String(moonPhase.fraction * 100, 1) + "% Illuminated");
  drawString(x + 30, y + 120, "Zodiac " + String(moonPhase.zodiacName));
  drawString(x + 30, y + 150, String(moonPhase.distance * 6371) + "km Distance");
  drawString(x + 30, y + 180, String(moonPhase.age) + " Days Moon Age");
}

/**
 * @brief Draws the moon background
 *
 * @param moonX X-coordinate for the moon image
 * @param moonY Y-coordinate for the moon image
 * @param moonWidth Width of the moon image
 * @param moonHeight Height of the moon image
 */
void drawMoonBackground(int moonX, int moonY, int moonWidth, int moonHeight)
{
  // Fill background with black
  display.fillScreen(GxEPD_BLACK);

  // Draw the full moon bitmap in white
  display.drawBitmap(moonX, moonY, moon_bitmap,
                     moonWidth, moonHeight, GxEPD_WHITE);
}

/**
 * @brief Draws Stephen Hawking's image for special date (March 8)
 *
 * @param moonX X-coordinate for the image
 * @param moonY Y-coordinate for the image
 */
void drawStephenHawkingImage(int moonX, int moonY)
{
  display.drawBitmap(moonX, moonY, steve_hawking_bitmap,
                     STEVE_HAWKING_SMALL_WIDTH, STEVE_HAWKING_SMALL_HEIGHT,
                     GxEPD_WHITE);
}

/**
 * @brief Draws Edwin Hubble's image for special date (January 1)
 *
 * @param moonX X-coordinate for the image
 * @param moonY Y-coordinate for the image
 */
void drawEdwinHubbleImage(int moonX, int moonY)
{
  display.drawBitmap(moonX, moonY, edwin_hubble_bitmap,
                     EDWIN_HUBBLE_WIDTH, EDWIN_HUBBLE_HEIGHT,
                     GxEPD_WHITE);
}

/**
 * @brief Draws the appropriate moon phase visualization
 *
 * @param phase Moon phase value between 0 and 1
 * @param moonX X-coordinate for the moon image
 * @param moonY Y-coordinate for the moon image
 * @param moonWidth Width of the moon image
 * @param moonHeight Height of the moon image
 */
void drawMoonPhase(double phase, int moonX, int moonY, int moonWidth, int moonHeight)
{
  bool waxing = (phase < 0.5);
  double fraction;

  if (waxing)
  {
    // Waxing: phase in [0..0.5)
    // fraction goes 0..1 as phase goes 0..0.5
    fraction = phase * 2.0;

    // Calculate boundary for the black rectangle
    int boundary = moonX + (int)(moonWidth * (1.0 - fraction));

    // Cover the left portion with black, leaving the right side visible
    display.fillRect(moonX, moonY,
                     boundary - moonX, // rect width
                     moonHeight,
                     GxEPD_BLACK);
  }
  else
  {
    // Waning: phase in [0.5..1]
    // fraction goes 1..0 as phase goes 0.5..1
    fraction = 2.0 * (1.0 - phase);

    // Calculate boundary for the black rectangle
    int boundary = moonX + (int)(moonWidth * fraction);

    // Cover the right portion with black, leaving the left side visible
    display.fillRect(boundary, moonY,
                     (moonX + moonWidth) - boundary,
                     moonHeight,
                     GxEPD_BLACK);
  }
}

/**
 * @brief Draws event text at the bottom of the display
 *
 * @param events Vector of event strings
 */
void drawEventText(std::vector<const char *> events)
{
  display.setCursor(100, 460);
  display.setTextColor(GxEPD_WHITE);
  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);

  if (!events.empty())
  {
    drawCenteredText(events[0], 470);
  }
  else
  {
    drawCenteredText("1974: Stephen Hawking proposes black hole radiation (Hawking Radiation)", 470);
  }
}

/**
 * @brief Draws a simple visualization of the moon phase for the given date
 *
 * This function draws a visual representation of the moon phase by using a white
 * circle for the full moon and covering portions with black to show the current phase.
 * On March 8th, it displays Stephen Hawking's image instead of the moon.
 *
 * @param day Day of the month (1-31)
 * @param month Month (1-12)
 * @param year Year (e.g., 2025)
 */
void drawMoonPhaseSimple(int day, int month, int year)
{
  // Where to draw the moon and its bitmap
  int moonX = 100, moonY = 50;
  int moonWidth = 400, moonHeight = 400;

  // Get astronomical events for today
  std::vector<const char *> events = getAstroEventsOfTheDay(day, month);

  // Calculate moon phase (0-1)
  moonPhase.calculate(time(NULL));
  double phase = moonPhase.phase;

  display.firstPage();
  do
  {
    // Special case for March 8 (Stephen Hawking)
    if (day == 8 && month == 3)
    {
      display.fillScreen(GxEPD_BLACK);
      drawStephenHawkingImage(moonX, moonY);
    }
    else if (day == 1 && month == 1)
    {
      display.fillScreen(GxEPD_BLACK);
      drawEdwinHubbleImage(moonX, moonY);
    }

    else
    {
      // Draw moon background and phase
      drawMoonBackground(moonX, moonY, moonWidth, moonHeight);
      drawMoonPhase(phase, moonX, moonY, moonWidth, moonHeight);
    }

    // Draw event text at the bottom
    drawEventText(events);

  } while (display.nextPage());
}

// ===== TIME & LOCATION DISPLAY =====

/**
 * @brief Formats time and date information for display
 *
 * @param timeinfo Pointer to the time structure
 * @param locationBuffer String to store location information
 * @param dateBuffer String to store date information
 * @param timeBuffer String to store time information
 * @param weekdayBuffer String to store weekday information
 */
void formatTimeAndDate(struct tm *timeinfo, String &locationBuffer, String &dateBuffer,
                       String &timeBuffer, String &weekdayBuffer)
{
  char timeWeekDay[10];
  char year_str[4];

  // Format weekday and year
  strftime(timeWeekDay, 10, "%A", timeinfo);
  strftime(year_str, 4, "%y", timeinfo);

  // Extract time components
  int hour = timeinfo->tm_hour;
  int minute = timeinfo->tm_min;
  int month = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;

  // Format time and date strings
  timeBuffer = formatTwoDigits(hour) + ":" + formatTwoDigits(minute);
  dateBuffer = formatTwoDigits(month) + "/" + formatTwoDigits(day) + "/" + String(year_str);
  weekdayBuffer = String(timeWeekDay);

  Serial.println(dateBuffer);
}

/**
 * @brief Prints the main display message with location, date, time, and weekday
 *
 * @param location The location string (latitude/longitude)
 * @param date The formatted date string
 * @param time The formatted time string
 * @param weekday The day of the week
 */
void printDisplayMessage(String location, String date, String time, String weekday)
{
  uint16_t x_time = 480;

  display.setFont(&FreeSans24pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setTextSize(3);

  do
  {
    display.setTextSize(1);
    display.setCursor(x_time + 30, 100);
    display.print(weekday);
    display.setCursor(x_time + 30, 150);
    display.print(date);
    display.drawFastHLine(x_time + 30, 190, 200, GxEPD_WHITE);
    displayAstronomySection(x_time, 250);
  } while (display.nextPage());
}

/**
 * @brief Displays the current time, date, and location information on the e-ink display
 *
 * This function formats the time, date, and location data from the GPS and displays
 * it on the e-ink screen along with the astronomy section.
 */
void displayInfo()
{
  String locationBuffer;
  String dateBuffer;
  String timeBuffer;
  String weekdayBuffer;

  // Format location string
  locationBuffer = String(gps.location.lat()) + " " + String(gps.location.lng());

  // Get local time with timezone adjustment
  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    int desiredTimezoneOffset = (int)decodeTimezone(gps.location.lat(), gps.location.lng());
    timeinfo.tm_hour += desiredTimezoneOffset;
    mktime(&timeinfo);

    // Format time and date information
    formatTimeAndDate(&timeinfo, locationBuffer, dateBuffer, timeBuffer, weekdayBuffer);

    // Print debug information
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  // Display the formatted information
  printDisplayMessage(locationBuffer, dateBuffer, timeBuffer, weekdayBuffer);
}

// ===== SYSTEM FUNCTIONS =====

/**
 * @brief Puts the device into deep sleep mode until midnight
 *
 * Calculates the time remaining until midnight and configures the device
 * to wake up at exactly midnight (00:00:00).
 */
void sleep()
{
  Serial.println("Calculating time until midnight...");

  // Get current time
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  // Calculate seconds until midnight
  int seconds_to_midnight = (23 - timeinfo.tm_hour) * 3600 +
                            (59 - timeinfo.tm_min) * 60 +
                            (60 - timeinfo.tm_sec);

  // If it's already past 23:59:00, we need to add 24 hours
  if (seconds_to_midnight < 60)
  {
    seconds_to_midnight += 24 * 3600;
  }

  Serial.print("Entering deep sleep for ");
  Serial.print(seconds_to_midnight);
  Serial.println(" seconds until midnight");

  // Wake up after calculated time
  esp_sleep_enable_timer_wakeup((uint64_t)seconds_to_midnight * 1000000ULL);

  // Enter deep sleep
  esp_deep_sleep_start();
}

/**
 * @brief A custom version of delay() that continues to process GPS data
 *
 * This function ensures that the GPS object continues to receive and process
 * data during the delay period, unlike the standard delay() function.
 *
 * @param ms The delay time in milliseconds
 */
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

// ===== MAIN PROGRAM FLOW =====

/**
 * @brief Arduino setup function, called once at startup
 *
 * Initializes serial communication, GPS module, and the e-ink display.
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Setup started");
  gpsSerial.begin(gpsConfig.baudRate);
  initDisplay();
  display.hibernate();
}

/**
 * @brief Arduino main loop function, called repeatedly
 *
 * Processes GPS data, configures system time when GPS data is valid,
 * updates the display with moon phase and time information, and then
 * puts the device into sleep mode to conserve power.
 */
void loop()
{
  // Process GPS data
  smartDelay(1000);

  // Check if GPS is connected
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }

  // Configure system time from GPS if not already done
  if (!isTimeConfigured && gps.time.isValid() && gps.location.isValid())
  {
    isTimeConfigured = true;
    setSystemTime(gps.date.year(), gps.date.month(), gps.date.day(),
                  gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.println("Time is configured");
  }

  // Get current UTC time
  time_t now = time(NULL);
  struct tm *now_utc = gmtime(&now);
  const int day_utc = now_utc->tm_mday;
  const int month_utc = now_utc->tm_mon + 1;
  const int year_utc = now_utc->tm_year + 1900;

  // Update display and sleep if time is configured
  if (isTimeConfigured)
  {
    Serial.println("Drawing the display");
    drawMoonPhaseSimple(day_utc, month_utc, year_utc);
    displayInfo();
    Serial.println("Hibernating the display");
    display.hibernate();
    sleep();
  }
}
