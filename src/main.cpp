#include <GxEPD2_3C.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <time.h>

#include "MoonRise.h"
#include "MoonPhase.h"
#include "timezone.h"
#include "events.h"

#include "bitmaps/moon.h"
#include "bitmaps/steve_hawking.h"

const int16_t displayWidth = 800;
const int16_t displayHeight = 480;

static const int RXPin = D10, TXPin = D11;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Wave e-ink display
GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display(GxEPD2_750c_Z08(/*CS=5*/ D9, /*DC=*/SCL, /*RST=*/SDA, /*BUSY=*/D7));

bool isTimeConfigured = false;

// Sleep for 1 hour
const uint64_t sleepTimeSeconds = 3600;

const String Hemisphere = "north";
String Units = "M";

// Sun
const String TXT_SUNRISE = "Sunrise";
const String TXT_SUNSET = "Sunset";

// Moon
const String TXT_MOON_NEW = "New Moon";
const String TXT_MOON_WAXING_CRESCENT = "Evening Crescent";
const String TXT_MOON_FIRST_QUARTER = "First Quarter";
const String TXT_MOON_WAXING_GIBBOUS = "Waxing Gibbous";
const String TXT_MOON_FULL = "Full Moon";
const String TXT_MOON_WANING_GIBBOUS = "Waning Gibbous";
const String TXT_MOON_THIRD_QUARTER = "Third Quarter";
const String TXT_MOON_WANING_CRESCENT = "Morning Crescent";

MoonRise mr;
_MoonPhase m;

String ConvertUnixTime(int unix_time)
{
  // Returns either '21:12  ' or ' 09:12pm' depending on Units mode
  time_t tm = unix_time;
  struct tm *now_tm = gmtime(&tm);
  char output[40];
  if (Units == "M")
  {
    strftime(output, sizeof(output), "%H:%M %d/%m/%y", now_tm);
  }
  else
  {
    strftime(output, sizeof(output), "%I:%M%P %m/%d/%y", now_tm);
  }
  return output;
}

void drawString(int x, int y, String text)
{
  display.setCursor(x, y);
  display.print(text);
}
// #########################################################################################

int JulianDate(int d, int m, int y)
{
  int mm, yy, k1, k2, k3, j;
  yy = y - (int)((12 - m) / 10);
  mm = m + 9;
  if (mm >= 12)
    mm = mm - 12;
  k1 = (int)(365.25 * (yy + 4712));
  k2 = (int)(30.6001 * mm + 0.5);
  k3 = (int)((int)((yy / 100) + 49) * 0.75) - 38;
  // 'j' for dates in Julian calendar:
  j = k1 + k2 + d + 59 + 1;
  if (j > 2299160)
    j = j - k3; // 'j' is the Julian date at 12h UT (Universal Time) For Gregorian calendar:
  return j;
}

double NormalizedMoonPhase(int d, int m, int y)
{
  int j = JulianDate(d, m, y);
  // Calculate the approximate phase of the moon
  double Phase = (j + 4.867) / 29.53059;
  return (Phase - (int)Phase);
}

String MoonPhase(int d, int m, int y, String hemisphere)
{
  int c, e;
  double jd;
  int b;
  if (m < 3)
  {
    y--;
    m += 12;
  }
  ++m;
  c = 365.25 * y;
  e = 30.6 * m;
  jd = c + e + d - 694039.09; /* jd is total days elapsed */
  jd /= 29.53059;             /* divide by the moon cycle (29.53 days) */
  b = jd;                     /* int(jd) -> b, take integer part of jd */
  jd -= b;                    /* subtract integer part to leave fractional part of original jd */
  b = jd * 8 + 0.5;           /* scale fraction from 0-8 and round by adding 0.5 */
  b = b & 7;                  /* 0 and 8 are the same phase so modulo 8 for 0 */
  if (hemisphere == "south")
    b = 7 - b;
  if (b == 0)
    return TXT_MOON_NEW; // New;              0%  illuminated
  if (b == 1)
    return TXT_MOON_WAXING_CRESCENT; // Waxing crescent; 25%  illuminated
  if (b == 2)
    return TXT_MOON_FIRST_QUARTER; // First quarter;   50%  illuminated
  if (b == 3)
    return TXT_MOON_WAXING_GIBBOUS; // Waxing gibbous;  75%  illuminated
  if (b == 4)
    return TXT_MOON_FULL; // Full;            100% illuminated
  if (b == 5)
    return TXT_MOON_WANING_GIBBOUS; // Waning gibbous;  75%  illuminated
  if (b == 6)
    return TXT_MOON_THIRD_QUARTER; // Third quarter;   50%  illuminated
  if (b == 7)
    return TXT_MOON_WANING_CRESCENT; // Waning crescent; 25%  illuminated
  return "";
}

String MoonAge(int d, int m, int y, String hemisphere)
{
  int c, e;
  double jd;
  int b;
  if (m < 3)
  {
    y--;
    m += 12;
  }
  ++m;
  c = 365.25 * y;
  e = 30.6 * m;
  jd = c + e + d - 694039.09; /* jd is total days elapsed */
  jd /= 29.53059;             /* divide by the moon cycle (29.53 days) */
  b = jd;                     /* int(jd) -> b, take integer part of jd */
  jd -= b;                    /* subtract integer part to leave fractional part of original jd */
  jd = abs(jd - 0.5);         /* 0 = new - 50 = full - 100 again new  --> 0 = 0% ; 0.5 = 100% */
  b = 100 - jd * 200;
  if (hemisphere == "south")
    b = 100 - b;
  return String(b) + "% Illumination";
}

void DisplayAstronomySection(int x, int y)
{
  // display.drawRect(x, y, 409, 59, GxEPD_BLACK);
  display.setFont(&FreeSans12pt7b);
  display.setTextSize(1);
  time_t _now = time(NULL);
  struct tm *now_utc = gmtime(&_now);
  const int day_utc = now_utc->tm_mday;
  const int month_utc = now_utc->tm_mon + 1;
  const int year_utc = now_utc->tm_year + 1900;
  drawString(x + 30, y, MoonPhase(day_utc, month_utc, year_utc, Hemisphere));
  // DrawMoon(x + 250, y, day_utc, month_utc, year_utc, Hemisphere);

  time_t utcOffset = mktime(now_utc) - _now;
  m.calculate(_now + utcOffset);
  mr.calculate(gps.location.lat(), gps.location.lng(), _now + utcOffset);
  time_t moonRiseTime = mr.riseTime - utcOffset;
  struct tm *moonRiseTimeInfo = localtime(&moonRiseTime);
  time_t moonSetTime = mr.setTime - utcOffset;
  struct tm *moonSetTimeInfo = localtime(&moonSetTime);
  char LCDTime[] = "HH:MM";
  // sprintf(LCDTime, "%02d:%02d", moonRiseTimeInfo->tm_hour, moonRiseTimeInfo->tm_min);
  // drawString(x + 30, y + 30, String(LCDTime) + " Moonrise");
  // sprintf(LCDTime, "%02d:%02d", moonSetTimeInfo->tm_hour, moonSetTimeInfo->tm_min);
  // drawString(x + 30, y + 60, String(LCDTime) + " Moonset");
  drawString(x + 30, y + 90, String(m.fraction * 100, 1) + "% Illuminated");
  drawString(x + 30, y + 120, "Zodiac " + String(m.zodiacName));
  drawString(x + 30, y + 150, String(m.distance * 6371) + "km Distance");
  drawString(x + 30, y + 180, String(m.age) + " Days Moon Age");
}

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
  return;
}

void printDisplayMessage(String location, String date, String time, String weekday)
{
  uint16_t x_time, y_time;

  display.setFont(&FreeSans24pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setTextSize(3);

  x_time = 480;

  do
  {
    display.setTextSize(1);
    display.setCursor(x_time + 30, 100);
    display.print(weekday);
    display.setCursor(x_time + 30, 150);
    display.print(date);
    //  display.setCursor(x, y + 60);
    //  display.print(location);
    display.drawFastHLine(x_time + 30, 190, 200, GxEPD_WHITE);
    DisplayAstronomySection(x_time, 250);
  } while (display.nextPage());
}

void drawCenteredText(const char *text, int16_t yBaseline)
{
  // 1) Measure the text width & height at the current font settings.
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, yBaseline, &x1, &y1, &w, &h);

  // 2) Compute the left coordinate so that the text is centered horizontally.
  //    (displayWidth - w) / 2 is the center. We might also shift by x1 if needed.
  int16_t xCenter = (displayWidth - w) / 2;

  // 3) Set the cursor to that position and print.
  display.setCursor(xCenter, yBaseline);
  display.println(text);
}

void drawMoonPhaseSimple(int day, int month, int year)
{
  // NormalizedMoonPhase(d, m, y) should return [0..1]:
  //   0   = new moon
  //   0.5 = half moon
  //   1   = full moon
  double phase = NormalizedMoonPhase(day, month, year);

  std::vector<const char *> events = getAstroEventsOfTheDay(day, month);

  // Where to draw the moon and its bitmap
  int moonX = 100, moonY = 50;
  int moonWidth = 400, moonHeight = 400;

  // We’ll do a simple interpretation:
  // - If phase < 0.5, it’s waxing (lit area grows from right to left).
  // - If phase >= 0.5, it’s waning (lit area shrinks from right to left).
  //
  // We want a fraction that goes from 0.0 (no moon visible) to 1.0 (half or more visible),
  // so that we can figure out how wide our black rectangle should be.

  double fraction;
  bool waxing = (phase < 0.5);

  display.firstPage();
  do
  {
    // 1) Entire background black
    display.fillScreen(GxEPD_BLACK);

    // 2) Draw the full moon bitmap in white (so now we have a big white disc).
    // display.drawBitmap(moonX, moonY, Bitmap3c800x480_1_black,
    //                   moonWidth, moonHeight, GxEPD_WHITE);

    // if (day == 8 && month == 1)
    if (day == 8 && month == 3)
    {
      // Draw Hawking bitmap in white
      display.drawBitmap(moonX, moonY, steve_hawking_small,
                         STEVE_HAWKING_SMALL_WIDTH, STEVE_HAWKING_SMALL_HEIGHT,
                         GxEPD_WHITE);
    }
    else
    {
      display.drawBitmap(moonX, moonY, Bitmap3c800x480_1_black,
                         moonWidth, moonHeight, GxEPD_WHITE);

      // 3) Cover the portion that’s NOT lit with a black rectangle.
      if (waxing)
      {
        // Waxing: phase in [0..0.5).
        // fraction = 2 * phase → goes 0..1 as phase goes 0..0.5
        fraction = phase * 2.0;

        // fraction = 0 means no part is lit (new moon).
        // fraction = 1 means half the disc is lit (right half).
        //
        // So we’ll draw a black rectangle on the left portion of the moon.
        // boundary = how far from the left we keep black.
        // If fraction=0 → boundary = entire width → all black
        // If fraction=1 → boundary = half the width → half black
        int boundary = moonX + (int)(moonWidth * (1.0 - fraction));

        // Cover from the left edge of the moon to 'boundary'.
        // That hides the left portion, leaving the right side visible.
        display.fillRect(moonX, moonY,
                         boundary - moonX, // rect width
                         moonHeight,
                         GxEPD_BLACK);
      }
      else
      {
        // Waning: phase in [0.5..1].
        // fraction = 2 * (1 - phase) → goes 1..0 as phase goes 0.5..1
        //   (Alternatively, you could do fraction = (phase - 0.5)*2 if you want 0..1)
        // Let’s define fraction = 2 * (1 - phase):
        fraction = 2.0 * (1.0 - phase);

        // fraction=1 means half the disc is lit (left half).
        // fraction=0 means full disc is lit (phase=1).
        //
        // We'll draw a black rectangle on the right portion of the moon.
        // boundary = left coordinate of the black rectangle.
        // If fraction=1 → boundary=moonX + halfWidth → half black on the right
        // If fraction=0 → boundary=moonX + fullWidth → no black area
        int boundary = moonX + (int)(moonWidth * (1.0 - fraction));

        // Cover from 'boundary' to the right edge of the moon.
        display.fillRect(boundary, moonY,
                         (moonX + moonWidth) - boundary,
                         moonHeight,
                         GxEPD_BLACK);
      }
    }

    int textX = 100;
    int textY = 460;
    display.setCursor(textX, textY);
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
      // display.println("1942: Stephen Hawking is born");
    }
  } while (display.nextPage());
}

void displayInfo()
{
  String locationBuffer;
  String dateBuffer;
  String timeBuffer;

  locationBuffer = String(gps.location.lat()) + " " + String(gps.location.lng());

  char timeWeekDay[10];
  char year_str[4];
  uint8_t hour, minute, month, day;

  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    int desiredTimezoneOffset = (int)decodeTimezone(gps.location.lat(), gps.location.lng());
    timeinfo.tm_hour += desiredTimezoneOffset;
    mktime(&timeinfo);

    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    month = timeinfo.tm_mon;
    day = timeinfo.tm_mday;

    strftime(timeWeekDay, 10, "%A", &timeinfo);
    strftime(year_str, 4, "%y", &timeinfo);

    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  String hour_str, minute_str, month_str, day_str;

  if (hour < 10)
  {
    hour_str = "0" + String(hour);
  }
  else
  {
    hour_str = String(hour);
  }

  if (minute < 10)
  {
    minute_str = "0" + String(minute);
  }
  else
  {
    minute_str = String(minute);
  }

  if (month < 10)
  {
    month_str = "0" + String(month + 1);
  }
  else
  {
    month_str = String(month + 1);
  }

  if (day < 10)
  {
    day_str = "0" + String(day);
  }
  else
  {
    day_str = String(day);
  }

  timeBuffer = hour_str + ":" + minute_str;
  dateBuffer = String(month_str) + "/" + String(day_str) + "/" + String(year_str);

  Serial.println(dateBuffer);

  printDisplayMessage(locationBuffer, dateBuffer, timeBuffer, String(timeWeekDay));
}

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

void setup()
{
  Serial.begin(115200);

  ss.begin(GPSBaud);

  initDisplay();
  display.hibernate();
}

void sleep()
{
  Serial.println("Entering deep sleep");
  // Wake up after amount of time
  esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
  // Enter deep sleep
  esp_deep_sleep_start();
}

int gpsReadData;

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                         : vi >= 10    ? 2
                                       : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void loop()
{
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }

  if (!isTimeConfigured && gps.time.isValid() && gps.location.isValid())
  {
    isTimeConfigured = true;
    setSystemTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.println("Time is configured");
  }

  time_t _now = time(NULL);
  struct tm *now_utc = gmtime(&_now);
  const int day_utc = now_utc->tm_mday;
  const int month_utc = now_utc->tm_mon + 1;
  const int year_utc = now_utc->tm_year + 1900;

  if (isTimeConfigured)
  {
    // drawMoon();
    drawMoonPhaseSimple(day_utc, month_utc, year_utc);
    displayInfo();
    display.hibernate();
    sleep();
  }
}