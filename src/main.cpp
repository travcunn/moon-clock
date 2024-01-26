// GxEPD2_HelloWorld.ino by Jean-Marc Zingg
//
// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: https://www.good-display.com/companyfile/32/
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics (closed, read only):
// Good Display ePaper for Arduino: https://forum.arduino.cc/t/good-display-epaper-for-arduino/419657
// Waveshare e-paper displays with SPI: https://forum.arduino.cc/t/waveshare-e-paper-displays-with-spi/467865
//
// Add new topics in https://forum.arduino.cc/c/using-arduino/displays/23 for new questions and issues

// see GxEPD2_wiring_examples.h for wiring suggestions and examples
// if you use a different wiring, you need to adapt the constructor parameters!

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
// #include <GFX.h>

#include <GxEPD2_3C.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <time.h>

#include "MoonRise.h"
#include "MoonPhase.h"

static const int RXPin = 17, TXPin = 16;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Wave e-ink display
GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display(GxEPD2_750c_Z08(/*CS=5*/ D9, /*DC=*/SCL, /*RST=*/SDA, /*BUSY=*/D7));

bool isTimeConfigured = false;

const uint16_t WIDTH = 128;
const uint16_t HEIGHT = 128;
const uint16_t sleepTimeSeconds = 180;

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

enum alignment
{
  LEFT,
  RIGHT,
  CENTER
};

MoonRise mr;
_MoonPhase m;

const unsigned char battery_0_bar_0deg_128x128[] PROGMEM = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00,
    0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00,
    0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00,
    0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

float decodeTimezone(double LAT, double LNG)
{
  float zone_hours;
  // if(LAT>=2.0 && LAT<=2.03 && LNG>=103 && LNG<=104){zone_hours=+8L;}//initial test around my area

  //(1/38TZ)UTC+14,3 Region
  if (LAT >= 0 && LAT <= 4 && LNG >= -160.5 && LNG <= -158 || LAT >= -6 && LAT <= 4 && LNG >= -158 && LNG <= -150 || LAT >= -12 && LAT <= -6 && LNG >= -155 && LNG <= -150)
  {
    zone_hours = +14L;
  } // UTC+14.R1/3

  //(2/38TZ)UTC+13,5 Region
  if (LAT >= -6 && LAT <= -2.5 && LNG >= -179 && LNG <= -158 || LAT >= -10 && LAT <= -6 && LNG >= -179 && LNG <= -169 || LAT >= -15 && LAT <= -10 && LNG >= -174 && LNG <= -171)
  {
    zone_hours = +13L;
  } // UTC+13.R1-3/5
  if (LAT >= -26 && LAT <= -15 && LNG >= -178.5 && LNG <= -172.5 || LAT >= -26 && LAT <= -22 && LNG >= -179.99 && LNG <= -178.5)
  {
    zone_hours = +13L;
  } // UTC+13.R4-5/5

  //(3/38TZ)UTC+12:45,1 Region
  if (LAT >= -45 && LAT <= -42 && LNG >= -179.9 && LNG <= -172.5)
  {
    zone_hours = +12.75L;
  } // UTC+12 3/4.R1/1

  //(4/38TZ)UTC+12,19 Region
  if (LAT >= 59.00 && LAT <= 90.00 && LNG >= 172.5 && LNG <= 180 || LAT >= 70.00 && LAT <= 75.00 && LNG >= -179.9 && LNG <= -172.5 || LAT >= 63.5 && LAT <= 70.00 && LNG >= -179.9 && LNG <= -168)
  {
    zone_hours = +12L;
  } // UTC+12.R1-3/20
  if (LAT >= 54 && LAT <= 70.00 && LNG >= 162 && LNG <= 172.5 || LAT >= 65 && LAT <= 67.7 && LNG >= 159.5 && LNG <= 162 || LAT >= 58.6 && LAT <= 60.5 && LNG >= 159.5 && LNG <= 162)
  {
    zone_hours = +12L;
  } // UTC+12.R4-6/20
  if (LAT >= 51 && LAT <= 58.6 && LNG >= 153 && LNG <= 162 || LAT >= 51 && LAT <= 54 && LNG >= 162 && LNG <= 167 || LAT >= 20 && LAT <= 48 && LNG >= 172.5 && LNG <= 180)
  {
    zone_hours = +12L;
  } // UTC+12.R7-9/20
  if (LAT >= 9 && LAT <= 20 && LNG >= 160 && LNG <= 180 || LAT >= -3 && LAT <= 9 && LNG >= 165 && LNG <= 180 || LAT >= -8 && LAT <= -3 && LNG >= 172.5 && LNG <= 180)
  {
    zone_hours = +12L;
  } // UTC+12.R10-12/20
  if (LAT >= -15 && LAT <= -8 && LNG >= -179.9 && LNG <= -179 || LAT >= -15 && LAT <= -8 && LNG >= 172.5 && LNG <= 180 || LAT >= -26 && LAT <= -22 && LNG >= 172.5 && LNG <= 180)
  {
    zone_hours = +12L;
  } // UTC+12.R13&15/20
  if (LAT >= -22 && LAT <= -15 && LNG >= 172.5 && LNG <= 180 || LAT >= -22 && LAT <= -15 && LNG >= -179.9 && LNG <= -177 || LAT >= -42 && LAT <= -26 && LNG >= 172.5 && LNG <= 180)
  {
    zone_hours = +12L;
  } // UTC+12.R14,17,18/20
  if (LAT >= -42 && LAT <= -26 && LNG >= -179.9 && LNG <= -172.5 || LAT >= -42 && LAT <= -30 && LNG >= 165 && LNG <= 172.5 || LAT >= -53 && LAT <= -42 && LNG >= 165 && LNG <= 180)
  {
    zone_hours = +12L;
  } // UTC+12.R16/20
  if (LAT >= -60 && LAT <= -53 && LNG >= 172.5 && LNG <= 180 || LAT >= -15 && LAT <= -12 && LNG >= -179 && LNG <= -174)
  {
    zone_hours = +12L;
  } // UTC+12.R19&20/20

  //(5/38TZ)UTC+11,16 Region
  if (LAT >= 70 && LAT <= 90 && LNG >= 157.5 && LNG <= 172.5 || LAT >= 65 && LAT <= 72.38 && LNG >= 141 && LNG <= 150 || LAT >= 70 && LAT <= 72.38 && LNG >= 150 && LNG <= 157.5)
  {
    zone_hours = +11L;
  } // UTC+11.R1-3/16
  if (LAT >= 67.7 && LAT <= 70 && LNG >= 150 && LNG <= 162 || LAT >= 65 && LAT <= 67.5 && LNG >= 150 && LNG <= 159.5 || LAT >= 60.5 && LAT <= 65 && LNG >= 145.5 && LNG <= 162)
  {
    zone_hours = +11L;
  } // UTC+11.R4-6/16
  if (LAT >= 58.6 && LAT <= 60.5 && LNG >= 145.5 && LNG <= 159 || LAT >= 51 && LAT <= 58.6 && LNG >= 141 && LNG <= 153 || LAT >= 45.7 && LAT <= 51 && LNG >= 141 && LNG <= 157.5)
  {
    zone_hours = +11L;
  } // UTC+11.R7-9/16
  if (LAT >= 42.8 && LAT <= 45.7 && LNG >= 145.26 && LNG <= 157.5 || LAT >= 20 && LAT <= 51 && LNG >= 157.5 && LNG <= 172.5 || LAT >= 9 && LAT <= 20 && LNG >= 157.5 && LNG <= 160)
  {
    zone_hours = +11L;
  } // UTC+11.R10-12/16
  if (LAT >= 3 && LAT <= 9 && LNG >= 154 && LNG <= 165 || LAT >= -10 && LAT <= -3 && LNG >= 154 && LNG <= 172.5 || LAT >= -30 && LAT <= -10 && LNG >= 157.5 && LNG <= 172.5)
  {
    zone_hours = +11L;
  } // UTC+11.R13-15/16
  if (LAT >= -53 && LAT <= -32 && LNG >= 157.5 && LNG <= 165 || LAT >= -60 && LAT <= -53 && LNG >= 157.5 && LNG <= 172.5)
  {
    zone_hours = +11L;
  } // UTC+11.16/16R

  //(6/38TZ)UTC+10:30,1 Region
  if (LAT >= -32 && LAT <= -30 && LNG >= 157.5 && LNG <= 165)
  {
    zone_hours = +10.5L;
  } // UTC+10 1/2.1R

  //(7/38TZ)UTC+10,18 Region
  if (LAT >= 77 && LAT <= 90 && LNG >= 142.5 && LNG <= 157.5 || LAT >= 73 && LAT <= 77 && LNG >= 153 && LNG <= 157.5 || LAT >= 72.38 && LAT <= 73 && LNG >= 130 && LNG <= 157.5)
  {
    zone_hours = +10L;
  } // UTC+10.1-3/18R
  if (LAT >= 65 && LAT <= 72.38 && LNG >= 130 && LNG <= 141 || LAT >= 58.6 && LAT <= 65 && LNG >= 137 && LNG <= 145.5 || LAT >= 45.7 && LAT <= 58.6 && LNG >= 131 && LNG <= 141)
  {
    zone_hours = +10L;
  } // UTC+10.4-6/18R
  if (LAT >= 42.8 && LAT <= 45.7 && LNG >= 131 && LNG <= 137.5 || LAT >= 30 && LAT <= 42.8 && LNG >= 145.26 && LNG <= 157.5 || LAT >= 24.5 && LAT <= 30 && LNG >= 150 && LNG <= 157.5)
  {
    zone_hours = +10L;
  } // UTC+10.7-9/18R
  if (LAT >= 16 && LAT <= 30 && LNG >= 142.5 && LNG <= 150 || LAT >= 16 && LAT <= 24 && LNG >= 150 && LNG <= 157.5 || LAT >= 7.5 && LAT <= 16 && LNG >= 136 && LNG <= 140.9)
  {
    zone_hours = +10L;
  } // UTC+10.10,11,18-12/18R
  if (LAT >= -15 && LAT <= 16 && LNG >= 140.9 && LNG <= 154 || LAT >= 9 && LAT <= 16 && LNG >= 154 && LNG <= 157.5 || LAT >= -15 && LAT <= -10 && LNG >= 154 && LNG <= 157.5)
  {
    zone_hours = +10L;
  } // UTC+10.13-15/18R
  if (LAT >= -26 && LAT <= -15 && LNG >= 138 && LNG <= 140.9 || LAT >= -38 && LAT <= -15 && LNG >= 140.9 && LNG <= 157.5 || LAT >= -60 && LAT <= -38 && LNG >= 142.5 && LNG <= 157.5)
  {
    zone_hours = +10L;
  } // UTC+10.16&17/18R

  //(8/38TZ)UTC+09:30,
  if (LAT >= -15 && LAT <= -11 && LNG >= 127.5 && LNG <= 140.9 || LAT >= -33 && LAT <= -15 && LNG >= 129 && LNG <= 138)
  {
    zone_hours = +9.5L;
  } // UTC+9 1/2.1-2/4R
  if (LAT >= -33 && LAT <= -26 && LNG >= 138 && LNG <= 140.9 || LAT >= -38 && LAT <= -33 && LNG >= 127.5 && LNG <= 140.9)
  {
    zone_hours = +9.5L;
  } // UTC+9 1/2.3-4/4R

  //(9/38TZ)UTC+9,18 Region
  if (LAT >= 77 && LAT <= 90 && LNG >= 127.5 && LNG <= 142.5 || LAT >= 73 && LAT <= 77 && LNG >= 115.5 && LNG <= 153 || LAT >= 69.5 && LAT <= 73 && LNG >= 110 && LNG <= 130)
  {
    zone_hours = +9L;
  } // UTC+9.1-3/18R
  if (LAT >= 65 && LAT <= 69.5 && LNG >= 105.5 && LNG <= 130 || LAT >= 58.6 && LAT <= 65 && LNG >= 109 && LNG <= 137.5 || LAT >= 54 && LAT <= 58.6 && LNG >= 116 && LNG <= 131)
  {
    zone_hours = +9L;
  } // UTC+9.4-6/18R
  if (LAT >= 51 && LAT <= 54 && LNG >= 113.2 && LNG <= 120 || LAT >= 49.5 && LAT <= 51 && LNG >= 107 && LNG <= 120 || LAT >= 49.5 && LAT <= 54 && LNG >= 127 && LNG <= 131)
  {
    zone_hours = +9L;
  } // UTC+9.7-9/18R
  if (LAT >= 42.8 && LAT <= 45.7 && LNG >= 137.5 && LNG <= 145.26 || LAT >= 30 && LAT <= 42.8 && LNG >= 124.2 && LNG <= 145.26 || LAT >= 23 && LAT <= 30 && LNG >= 123 && LNG <= 127.5)
  {
    zone_hours = +9L;
  } // UTC+9.10-12/18R
  if (LAT >= 16 && LAT <= 30 && LNG >= 127.5 && LNG <= 142.5 || LAT >= 7.5 && LAT <= 16 && LNG >= 127.5 && LNG <= 136 || LAT >= -11 && LAT <= 7.5 && LNG >= 127.5 && LNG <= 140.9)
  {
    zone_hours = +9L;
  } // UTC+9.13-15/18R
  if (LAT >= -11 && LAT <= 0 && LNG >= 127.5 && LNG <= 125.5 || LAT >= 24 && LAT <= 24.5 && LNG >= 150 && LNG <= 157.5 || LAT >= 73 && LAT <= 75 && LNG >= 110 && LNG <= 115.5)
  {
    zone_hours = +9L;
  } // UTC+9.16,17,2b/18R

  //(10/38TZ)UTC+08:45,1 Region
  if (LAT >= -33 && LAT <= -30 && LNG >= 126 && LNG <= 129)
  {
    zone_hours = +8.75L;
  } // UTC+8 3/4 .R1/1

  //(11/38TZ)UTC+8,30+2 Region
  if (LAT >= 77 && LAT <= 90 && LNG >= 115.5 && LNG <= 127.5 || LAT >= 58.6 && LAT <= 65 && LNG >= 105.5 && LNG <= 109 || LAT >= 53 && LAT <= 58.3 && LNG >= 97 && LNG <= 105.5)
  {
    zone_hours = +8L;
  } // UTC+8.1-3/30R
  if (LAT >= 54 && LAT <= 58.6 && LNG >= 105.5 && LNG <= 116.3 || LAT >= 51 && LAT <= 53 && LNG >= 98.4 && LNG <= 105.5 || LAT >= 51 && LAT <= 54 && LNG >= 105.5 && LNG <= 113.2)
  {
    zone_hours = +8L;
  } // UTC+8.4-6/30R
  if (LAT >= 49.5 && LAT <= 51 && LNG >= 98.4 && LNG <= 107 || LAT >= 42.8 && LAT <= 49.5 && LNG >= 98.4 && LNG <= 131 || LAT >= 45 && LAT <= 47 && LNG >= 82.6 && LNG <= 85.6)
  {
    zone_hours = +8L;
  } // UTC+8.7-9/30R
  if (LAT >= 45 && LAT <= 48 && LNG >= 85.6 && LNG <= 90.88 || LAT >= 41.1 && LAT <= 45 && LNG >= 82 && LNG <= 82.5 || LAT >= 34 && LAT <= 41.4 && LNG >= 75 && LNG <= 82.5)
  {
    zone_hours = +8L;
  } // UTC+8.10-12/30R
  if (LAT >= 31.3 && LAT <= 45 && LNG >= 82.5 && LNG <= 97 || LAT >= 29.6 && LAT <= 31.3 && LNG >= 88.4 && LNG <= 97 || LAT >= 31.3 && LAT <= 34 && LNG >= 79.2 && LNG <= 82.5)
  {
    zone_hours = +8L;
  } // UTC+8.13,13b,14/30R
  if (LAT >= 28.8 && LAT <= 42.8 && LNG >= 97 && LNG <= 98.7 || LAT >= 30 && LAT <= 42.8 && LNG >= 98.7 && LNG <= 124.2 || LAT >= 22.26 && LAT <= 30 && LNG >= 98.7 && LNG <= 123)
  {
    zone_hours = +8L;
  } // UTC+8.15-17/30R
  if (LAT >= 15 && LAT <= 22.26 && LNG >= 108 && LNG <= 123 || LAT >= 0 && LAT <= 23 && LNG >= 123 && LNG <= 127.5 || LAT >= 1.2 && LAT <= 15 && LNG >= 112.5 && LNG <= 123)
  {
    zone_hours = +8L;
  } // UTC+8.18-20/30R
  if (LAT >= 0 && LAT <= 1.2 && LNG >= 112.5 && LNG <= 123 || LAT >= -9 && LAT <= 0 && LNG >= 114.6 && LNG <= 125.5 || LAT >= -9 && LAT <= -9 && LNG >= 112.5 && LNG <= 125.5)
  {
    zone_hours = +8L;
  } // UTC+8.20b,21,22/30R
  if (LAT >= -30 && LAT <= -11 && LNG >= 112.5 && LNG <= 127.5 || LAT >= -30 && LAT <= -15 && LNG >= 127.5 && LNG <= 129 || LAT >= -33 && LAT <= -30 && LNG >= 112.5 && LNG <= 126)
  {
    zone_hours = +8L;
  } // UTC+8.23-25/30R
  if (LAT >= -60 && LAT <= -33 && LNG >= 112.5 && LNG <= 127.5 || LAT >= 4.7 && LAT <= 6.67 && LNG >= 99 && LNG <= 112.5 || LAT >= 3 && LAT <= 4.7 && LNG >= 100 && LNG <= 105)
  {
    zone_hours = +8L;
  } // UTC+8.26-28/30R
  if (LAT >= 1.2 && LAT <= 3 && LNG >= 101 && LNG <= 105 || LAT >= 1.2 && LAT <= 4.7 && LNG >= 109.5 && LNG <= 112.5)
  {
    zone_hours = +8L;
  } // UTC+8.29-30/30R

  //(12/38TZ)UTC+7,26 Region
  if (LAT >= 81.6 && LAT <= 90 && LNG >= 97.5 && LNG <= 112.5 || LAT >= 79 && LAT <= 81.6 && LNG >= 75 && LNG <= 112.5 || LAT >= 77 && LAT <= 79 && LNG >= 78.2 && LNG <= 112.5)
  {
    zone_hours = +7L;
  } // UTC+7.1-3/26R
  if (LAT >= 71.4 && LAT <= 77 && LNG >= 78.2 && LNG <= 110 || LAT >= 75 && LAT <= 77 && LNG >= 112.5 && LNG <= 115.5 || LAT >= 65 && LAT <= 71.4 && LNG >= 82.5 && LNG <= 105.5)
  {
    zone_hours = +7L;
  } // UTC+7.4-6/26R
  if (LAT >= 69.5 && LAT <= 71.4 && LNG >= 105.5 && LNG <= 110 || LAT >= 61 && LAT <= 65 && LNG >= 86 && LNG <= 105.5 || LAT >= 58.3 && LAT <= 61 && LNG >= 75.8 && LNG <= 105.5)
  {
    zone_hours = +7L;
  } // UTC+7.7-9/26R
  if (LAT >= 54.51 && LAT <= 58.3 && LNG >= 75.8 && LNG <= 97 || LAT >= 51 && LAT <= 54.51 && LNG >= 78.2 && LNG <= 84.3 || LAT >= 50 && LAT <= 54.51 && LNG >= 84.3 && LNG <= 97)
  {
    zone_hours = +7L;
  } // UTC+7.10-12/26R
  if (LAT >= 48 && LAT <= 50 && LNG >= 85.6 && LNG <= 90.88 || LAT >= 45 && LAT <= 50 && LNG >= 90.88 && LNG <= 97 || LAT >= 42.8 && LAT <= 53 && LNG >= 97 && LNG <= 98.4)
  {
    zone_hours = +7L;
  } // UTC+7.13-15/26R
  if (LAT >= 20 && LAT <= 22.26 && LNG >= 101 && LNG <= 108 || LAT >= 9.92 && LAT <= 20 && LNG >= 98.7 && LNG <= 108 || LAT >= 9.92 && LAT <= 15 && LNG >= 108 && LNG <= 112.5)
  {
    zone_hours = +7L;
  } // UTC+7.16-18/26R
  if (LAT >= 6.67 && LAT <= 9.92 && LNG >= 97.5 && LNG <= 112.5 || LAT >= 0 && LAT <= 6.67 && LNG >= 95 && LNG <= 99 || LAT >= 1.2 && LAT <= 4.7 && LNG >= 105 && LNG <= 109.5)
  {
    zone_hours = +7L;
  } // UTC+7.19-21/26R
  if (LAT >= 0 && LAT <= 3 && LNG >= 99 && LNG <= 101 || LAT >= 0 && LAT <= 1.2 && LNG >= 101 && LNG <= 112.5 || LAT >= -9 && LAT <= 1.2 && LNG >= 112.5 && LNG <= 114.6)
  {
    zone_hours = +7L;
  } // UTC+7.22-24/26R
  if (LAT >= -60 && LAT <= 0 && LNG >= 97.5 && LNG <= 112.5 || LAT >= 3 && LAT <= 4.7 && LNG >= 99 && LNG <= 100.5)
  {
    zone_hours = +7L;
  } // UTC+7.25-26/26R

  //(13/38TZ)UTC+06:30,6 Region
  if (LAT >= 25.54 && LAT <= 28.8 && LNG >= 97 && LNG <= 98.7 || LAT >= 20 && LAT <= 25.54 && LNG <= 98.7 && LNG >= 92.5 || LAT >= 20 && LAT <= 22.26 && LNG <= 101 && LNG >= 98.7)
  {
    zone_hours = +6.5L;
  } // UTC+06:30.R9,1-3/6
  if (LAT >= 15 && LAT <= 20 && LNG >= 87.4 && LNG <= 92.5 || LAT >= 9.92 && LAT <= 20 && LNG <= 98.7 && LNG >= 97.5 || LAT >= -15 && LAT <= -11.5 && LNG <= 97.5 && LNG >= 96.5)
  {
    zone_hours = +6.5L;
  } // UTC+06:30.R9,4-6/6

  //(14/38TZ)UTC+6,17 Region
  if (LAT >= -8 && LAT <= -5 && LNG >= 71 && LNG <= 75 || LAT >= 26.37 && LAT <= 29.6 && LNG <= 92.5 && LNG >= 88.4 || LAT >= 20 && LAT <= 25.54 && LNG <= 92.5 && LNG >= 88.4)
  {
    zone_hours = +6L;
  } // UTC+6.R9,1&2/17
  if (LAT >= 81.6 && LAT <= 90 && LNG >= 82.5 && LNG <= 97.5 || LAT >= 15 && LAT <= 20 && LNG <= 92.5 && LNG >= 87.4 || LAT >= 6.67 && LAT <= 15 && LNG <= 90 && LNG >= 82.5)
  {
    zone_hours = +6L;
  } // UTC+6.R3-5/17
  if (LAT >= 0 && LAT <= 6.67 && LNG >= 82.5 && LNG <= 95 || LAT >= 6.67 && LAT <= 15 && LNG <= 97.5 && LNG >= 95 || LAT >= -11.5 && LAT <= 0 && LNG <= 97.5 && LNG >= 82.5)
  {
    zone_hours = +6L;
  } // UTC+6.R6-8/17
  if (LAT >= -15 && LAT <= -11.5 && LNG >= 82.5 && LNG <= 96.5 || LAT >= -60 && LAT <= -15 && LNG <= 97.5 && LNG >= 82.5 || LAT >= 54.51 && LAT <= 58.3 && LNG <= 75.8 && LNG >= 69.64)
  {
    zone_hours = +6L;
  } // UTC+6.R10-12/17
  if (LAT >= 50 && LAT <= 54.51 && LNG >= 60.6 && LNG <= 78.2 || LAT >= 50 && LAT <= 51 && LNG <= 84.3 && LNG >= 78.2 || LAT >= 41.1 && LAT <= 50 && LNG <= 82 && LNG >= 67.5)
  {
    zone_hours = +6L;
  } // UTC+6.R13-15/17
  if (LAT >= 47 && LAT <= 50 && LNG >= 82 && LNG <= 85.6 || LAT >= 45 && LAT <= 47 && LNG <= 82.5 && LNG >= 82)
  {
    zone_hours = +6L;
  } // UTC+6.R16-17/17

  //(15/38TZ)UTC+05:45,1 Region
  if (LAT >= 26.37 && LAT <= 31.3 && LNG >= 80 && LNG <= 88.4)
  {
    zone_hours = +5.75L;
  } //+05:45 zone_hours.R1/1

  //(16/38TZ)UTC+05:30,9 Region
  if (LAT >= 31.3 && LAT <= 34 && LNG <= 79.2 && LNG >= 75 || LAT >= 26.37 && LAT <= 31.3 && LNG <= 80 && LNG >= 75 || LAT >= 25.54 && LAT <= 26.37 && LNG <= 92.5 && LNG >= 82.5)
  {
    zone_hours = +5.5L;
  } // UTC+05:30.R1,2ab,2c/9
  if (LAT >= 25.54 && LAT <= 29.6 && LNG <= 97 && LNG >= 92.5 || LAT >= 20 && LAT <= 29.6 && LNG <= 75 && LNG >= 70)
  {
    zone_hours = +5.5L;
  } //+05:30 zone_hours.R2d&3/9
  if (LAT >= 5.66 && LAT <= 26.37 && LNG <= 82.5 && LNG >= 75 || LAT >= 25.54 && LAT <= 29.6 && LNG <= 97 && LNG >= 82.5 || LAT >= 20 && LAT <= 25.54 && LNG <= 92.5 && LNG >= 82.5)
  {
    zone_hours = +5.5L;
  } // UTC+05:30.R4-6/9
  if (LAT >= 15 && LAT <= 20 && LNG <= 87.4 && LNG >= 82.5 || LAT >= 8 && LAT <= 12 && LNG <= 75 && LNG >= 71.5 || LAT >= 6.67 && LAT <= 15 && LNG <= 95 && LNG >= 90)
  {
    zone_hours = +5.5L;
  } // UTC+05:30.R4-6/9

  //(17/38TZ)UTC+5,26 Region
  if (LAT >= 83 && LAT <= 90 && LNG <= 82.5 && LNG >= 67.5 || LAT >= 79 && LAT <= 83 && LNG <= 75 && LNG >= 67.5 || LAT >= 81 && LAT <= 83 && LNG <= 82.5 && LNG >= 75)
  {
    zone_hours = +5L;
  } // UTC+5.R1-3/26
  if (LAT >= 61 && LAT <= 79 && LNG <= 78.2 && LNG >= 69.64 || LAT >= 65.7 && LAT <= 75 && LNG <= 69.64 && LNG >= 65.3 || LAT >= 61 && LAT <= 71.4 && LNG <= 82.5 && LNG >= 78.2)
  {
    zone_hours = +5L;
  } // UTC+5.R4-6/26
  if (LAT >= 61 && LAT <= 65.7 && LNG <= 69.64 && LNG >= 60 || LAT >= 61 && LAT <= 65 && LNG <= 86 && LNG >= 82.5 || LAT >= 54.51 && LAT <= 61 && LNG <= 69.64 && LNG >= 54.4)
  {
    zone_hours = +5L;
  } // UTC+5.R7-9/26
  if (LAT >= 58.3 && LAT <= 61 && LNG <= 75.8 && LNG >= 69.64 || LAT >= 50 && LAT <= 54.51 && LNG <= 60.6 && LNG >= 51.7 || LAT >= 37.27 && LAT <= 50 && LNG <= 60.6 && LNG >= 48.9)
  {
    zone_hours = +5L;
  } // UTC+5.R10-12/26
  if (LAT >= 37.27 && LAT <= 50 && LNG <= 67.5 && LNG >= 60.6 || LAT >= 37.27 && LAT <= 41.1 && LNG <= 69 && LNG >= 67.5 || LAT >= 37.27 && LAT <= 39.5 && LNG <= 75 && LNG >= 69)
  {
    zone_hours = +5L;
  } // UTC+5.R13-15/26
  if (LAT >= 34 && LAT <= 37.27 && LNG <= 75 && LNG >= 71.5 || LAT >= 31.3 && LAT <= 34 && LNG <= 75 && LNG >= 69.2 || LAT >= 29.6 && LAT <= 31.3 && LNG <= 75 && LNG >= 67.5)
  {
    zone_hours = +5L;
  } // UTC+5.R16-18/26
  if (LAT >= 25.77 && LAT <= 29.6 && LNG <= 70 && LNG >= 60.6 || LAT >= 20 && LAT <= 25.77 && LNG <= 70 && LNG >= 67.5 || LAT >= 12 && LAT <= 20 && LNG <= 71.5 && LNG >= 67.5)
  {
    zone_hours = +5L;
  } // UTC+5.R19-21/26
  if (LAT >= 8 && LAT <= 12 && LNG <= 71.5 && LNG >= 67.5 || LAT >= -5 && LAT <= 8 && LNG <= 75 && LNG >= 67.5 || LAT >= -8 && LAT <= 5.66 && LNG <= 82.5 && LNG >= 75)
  {
    zone_hours = +5L;
  } // UTC+5.R22-24/26
  if (LAT >= -8 && LAT <= -5 && LNG <= 71 && LNG >= 67.5 || LAT >= -60 && LAT <= -8 && LNG <= 82.5 && LNG >= 67.5)
  {
    zone_hours = +5L;
  } // UTC+5.R25-26/26

  //(18/38TZ)UTC+04:30,3 Region
  if (LAT >= 34 && LAT <= 37.27 && LNG <= 71.5 && LNG >= 60.6 || LAT >= 29.6 && LAT <= 34 && LNG <= 70 && LNG >= 60.6 || LAT >= 31.3 && LAT <= 34 && LNG <= 69 && LNG >= 67.5)
  {
    zone_hours = +4.5L;
  } // UTC+4.5.R1-3/3

  //(19/38TZ)UTC+4,16 Region
  if (LAT >= 83 && LAT <= 90 && LNG <= 67.5 && LNG >= 52.5 || LAT >= 52.4 && LAT <= 54.51 && LNG <= 51.7 && LNG >= 46 || LAT >= 47.7 && LAT <= 52.4 && LNG <= 46 && LNG >= 42.13)
  {
    zone_hours = +4L;
  } // UTC+4.R1-3/15
  if (LAT >= 50 && LAT <= 52.4 && LNG <= 51.7 && LNG >= 46 || LAT >= 45 && LAT <= 47.7 && LNG <= 48.9 && LNG >= 46 || LAT >= 41.3 && LAT <= 43.2 && LNG <= 44 && LNG >= 41.8)
  {
    zone_hours = +4L;
  } // UTC+4.R4-6/15
  if (LAT >= 39.4 && LAT <= 43.2 && LNG <= 48.9 && LNG >= 44 || LAT >= 22.84 && LAT <= 25.77 && LNG <= 67.5 && LNG >= 51.3 || LAT >= 20 && LAT <= 22 && LNG <= 67.5 && LNG >= 55)
  {
    zone_hours = +4L;
  } // UTC+4.R7-9/15
  if (LAT >= 13 && LAT <= 20 && LNG <= 67.7 && LNG >= 52.5 || LAT >= 11 && LAT <= 13 && LNG <= 67.5 && LNG >= 54 || LAT >= -60 && LAT <= 11 && LNG <= 67.5 && LNG >= 52.5)
  {
    zone_hours = +4L;
  } // UTC+4.R10-12/15
  if (LAT >= -10.86 && LAT <= -4 && LNG <= 52.5 && LNG >= 45 || LAT >= -47.23 && LAT <= -45 && LNG <= 52.5 && LNG >= 49.5 || LAT >= 54.51 && LAT <= 58.6 && LNG <= 54.4 && LNG >= 51.7)
  {
    zone_hours = +4L;
  } // UTC+4.R13-15/15

  //(20/38TZ)UTC+03:30,4 Region
  if (LAT >= 37.27 && LAT <= 39.4 && LNG <= 48.9 && LNG >= 44 || LAT >= 33 && LAT <= 37.27 && LNG <= 51.3 && LNG >= 45.5)
  {
    zone_hours = +3.5L;
  } // UTC+3.5.R1-2/4
  if (LAT >= 33 && LAT <= 37.27 && LNG <= 60.6 && LNG >= 51.3 || LAT >= 29.96 && LAT <= 33 && LNG <= 51.3 && LNG >= 47.86)
  {
    zone_hours = +3.5L;
  } // UTC+3.5.R3-4/4

  //(21/29TZ)UTC+3,28 Region
  if (LAT >= 83 && LAT <= 90 && LNG <= 52.5 && LNG >= 37.5 || LAT >= 75 && LAT <= 81 && LNG <= 37.5 && LNG >= 34 || LAT >= 75 && LAT <= 83 && LNG <= 69.64 && LNG >= 37.5)
  {
    zone_hours = +3L;
  } // UTC+3.R1-3/29
  if (LAT >= 71.4 && LAT <= 75 && LNG <= 65.3 && LNG >= 37.5 || LAT >= 63 && LAT <= 71.4 && LNG <= 60 && LNG >= 30 || LAT >= 65.7 && LAT <= 71.4 && LNG <= 65.3 && LNG >= 60)
  {
    zone_hours = +3L;
  } // UTC+3.R4-6/29
  if (LAT >= 54.51 && LAT <= 63 && LNG <= 51.7 && LNG >= 28 || LAT >= 61 && LAT <= 63 && LNG <= 60 && LNG >= 51.7 || LAT >= 58.6 && LAT <= 61 && LNG <= 54.4 && LNG >= 51.7)
  {
    zone_hours = +3L;
  } // UTC+3.R7-10/29 Omit R8
  if (LAT >= 51.67 && LAT <= 54.51 && LNG <= 40 && LNG >= 23.6 || LAT >= 52.4 && LAT <= 54.51 && LNG <= 46 && LNG >= 40 || LAT >= 45 && LAT <= 52.4 && LNG <= 42.5 && LNG >= 40)
  {
    zone_hours = +3L;
  } // UTC+3.R11-13/29
  if (LAT >= 45 && LAT <= 47.7 && LNG <= 46 && LNG >= 42.13 || LAT >= 43.2 && LAT <= 45 && LNG <= 48.9 && LNG >= 40 || LAT >= 37.27 && LAT <= 41.3 && LNG <= 44 && LNG >= 26)
  {
    zone_hours = +3L;
  } // UTC+3.R14-16/29
  if (LAT >= 33 && LAT <= 37.27 && LNG <= 44 && LNG >= 41.2 || LAT >= 29.96 && LAT <= 33 && LNG <= 47.86 && LNG >= 37.5 || LAT >= 22.84 && LAT <= 29.96 && LNG <= 51.3 && LNG >= 37.5)
  {
    zone_hours = +3L;
  } // UTC+3.R17-19/29
  if (LAT >= 9.68 && LAT <= 22.84 && LNG <= 52.5 && LNG >= 37.5 || LAT >= 20 && LAT <= 22.84 && LNG <= 55 && LNG >= 52.5 || LAT >= 11 && LAT <= 13 && LNG <= 54 && LNG >= 52.5)
  {
    zone_hours = +3L;
  } // UTC+3.R20-22/29
  if (LAT >= 6 && LAT <= 9.68 && LNG <= 23.9 && LNG >= 30.65 || LAT >= -4 && LAT <= 9.68 && LNG <= 52.5 && LNG >= 30.65 || LAT >= -10.86 && LAT <= -4 && LNG <= 45 && LNG >= 30.65)
  {
    zone_hours = +3L;
  } // UTC+3.R23-25/29
  if (LAT >= -17.38 && LAT <= -10.86 && LNG <= 52.5 && LNG >= 40.67 || LAT >= -45 && LAT <= -17.38 && LNG <= 52.5 && LNG >= 37.5)
  {
    zone_hours = +3L;
  } // UTC+3.R26-27/29
  if (LAT >= -47.23 && LAT <= -45 && LNG <= 49.5 && LNG >= 37.5 || LAT >= -60 && LAT <= -47.23 && LNG <= 52.5 && LNG >= 37.5)
  {
    zone_hours = +3L;
  } // UTC+3.R28-29/29

  //(22/38TZ)UTC+2,17 Region
  if (LAT >= 81 && LAT <= 90 && LNG <= 37.5 && LNG >= 22.5 || LAT >= 71.4 && LAT <= 75 && LNG <= 37.5 && LNG >= 22.5 || LAT >= 63 && LAT <= 69.7 && LNG <= 30 && LNG >= 22.5)
  {
    zone_hours = +2L;
  } // UTC.R1-3/17
  if (LAT >= 54.51 && LAT <= 63 && LNG <= 28 && LNG >= 19.8 || LAT >= 50 && LAT <= 51.67 && LNG <= 40 && LNG >= 23.6 || LAT >= 45 && LAT <= 50 && LNG <= 40 && LNG >= 21.67)
  {
    zone_hours = +2L;
  } // UTC+2.R4-6/17
  if (LAT >= 41.3 && LAT <= 45 && LNG <= 40 && LNG >= 22.5 || LAT >= 41.3 && LAT <= 43.2 && LNG <= 41.8 && LNG >= 40 || LAT >= 39.7 && LAT <= 41.3 && LNG <= 26 && LNG >= 20.5)
  {
    zone_hours = +2L;
  } // UTC+2.R7a,7b,8/17
  if (LAT >= 33 && LAT <= 39.7 && LNG <= 26 && LNG >= 19.8 || LAT >= 33 && LAT <= 37.27 && LNG <= 41.2 && LNG >= 26 || LAT >= 21 && LAT <= 33 && LNG <= 37.5 && LNG >= 9.5)
  {
    zone_hours = +2L;
  } // UTC+2.R9-11/17
  if (LAT >= 9.68 && LAT <= 21 && LNG <= 37.5 && LNG >= 23.9 || LAT >= -17.38 && LAT <= -6 && LNG <= 30.65 && LNG >= 22 || LAT >= -17.38 && LAT <= -10.86 && LNG <= 40.67 && LNG >= 30.65)
  {
    zone_hours = +2L;
  } // UTC+2.R12-14/17
  if (LAT >= -30 && LAT <= -17.38 && LNG <= 37.5 && LNG >= 11.58 || LAT >= -34.9 && LAT <= -30 && LNG <= 30.65 && LNG >= 22 || LAT >= -60 && LAT <= -34.9 && LNG <= 37.5 && LNG >= 22.5)
  {
    zone_hours = +2L;
  } // UTC+2.R15-17/17

  //(23/38TZ)UTC+1,24 Region
  if (LAT >= 63 && LAT <= 90 && LNG <= 22.5 && LNG >= 7.5 || LAT >= 75 && LAT <= 81 && LNG <= 34 && LNG >= 22.5 || LAT >= 57.5 && LAT <= 63 && LNG <= 19.8 && LNG >= 3.7)
  {
    zone_hours = +1L;
  } // UTC+1.R1-3/24
  if (LAT >= 54 && LAT <= 57.5 && LNG <= 19.8 && LNG >= 7 || LAT >= 50 && LAT <= 54 && LNG <= 19.8 && LNG >= 2.3 || LAT >= 50 && LAT <= 54.51 && LNG <= 23.6 && LNG >= 19.8)
  {
    zone_hours = +1L;
  } // UTC+1.R4-6/24
  if (LAT >= 33 && LAT <= 50 && LNG <= 19.8 && LNG >= -5 || LAT >= 45 && LAT <= 50 && LNG <= 21.67 && LNG >= 19.8 || LAT >= 41.3 && LAT <= 45 && LNG <= 22.5 && LNG >= 19.8)
  {
    zone_hours = +1L;
  } // UTC+1.R7-9/24
  if (LAT >= 26 && LAT <= 45 && LNG <= -5 && LNG >= -18.75 || LAT >= 25 && LAT <= 33 && LNG <= 9.5 && LNG >= -5 || LAT >= 21 && LAT <= 25 && LNG <= 9.5 && LNG >= 0)
  {
    zone_hours = +1L;
  } // UTC+1.R10-12/24
  if (LAT >= 14 && LAT <= 21 && LNG <= 23.9 && LNG >= 4.2 || LAT >= 6 && LAT <= 14 && LNG <= 23.9 && LNG >= 1.96 || LAT >= 2.1 && LAT <= 6 && LNG <= 22 && LNG >= 6)
  {
    zone_hours = +1L;
  } // UTC+1.R13-15/24
  if (LAT >= 0 && LAT <= 2.1 && LNG <= 22 && LNG >= 7.5 || LAT >= -1.5 && LAT <= 0 && LNG <= 7 && LNG >= 5.2 || LAT >= -17.38 && LAT <= 0 && LNG <= 22 && LNG >= 7)
  {
    zone_hours = +1L;
  } // UTC+1.R16-18/24
  if (LAT >= -30 && LAT <= -17.38 && LNG <= 11.58 && LNG >= 7.5 || LAT >= -34.9 && LAT <= -30 && LNG <= 17 && LNG >= 7.5 || LAT >= -60 && LAT <= -34.9 && LNG <= 22.5 && LNG >= 7.5)
  {
    zone_hours = +1L;
  } // UTC+1.R19-21/24
  if (LAT >= -56 && LAT <= -50 && LNG <= 7.5 && LNG >= 1.5 || LAT >= 69.7 && LAT <= 71.4 && LNG <= 30 && LNG >= 22.5 || LAT >= 39.7 && LAT <= 41.3 && LNG <= 20.5 && LNG >= 19.8)
  {
    zone_hours = +1L;
  } // UTC+1.R22-24/24

  //(24/38TZ)UTC±0,20+1 Region
  if (LAT >= 76 && LAT <= 79.2 && LNG <= -15 && LNG >= -22.5 || LAT >= 63 && LAT <= 90 && LNG <= 7.5 && LNG >= -7.5 || LAT >= 63 && LAT <= 67 && LNG <= -7.5 && LNG >= -25)
  {
    zone_hours = +0L;
  } // UTC±0.R1-3/20
  if (LAT >= 60 && LAT <= 63 && LNG <= 3.7 && LNG >= -7.5 || LAT >= 54 && LAT <= 60 && LNG <= 3.7 && LNG >= -11.25 || LAT >= 54 && LAT <= 57.5 && LNG <= 3.5 && LNG >= -7.5)
  {
    zone_hours = +0L;
  } // UTC±0.R4-6/20
  if (LAT >= 50 && LAT <= 54 && LNG <= 2.3 && LNG >= -11.25 || LAT >= 45 && LAT <= 50 && LNG <= -5 && LNG >= -7.5 || LAT >= 0 && LAT <= 20 && LNG <= -5 && LNG >= -18.75)
  {
    zone_hours = +0L;
  } // UTC±0.R7-9/20
  if (LAT >= 0 && LAT <= 25 && LNG <= 0 && LNG >= -5 || LAT >= 14 && LAT <= 21 && LNG <= 4.2 && LNG >= 0 || LAT >= 6 && LAT <= 14 && LNG <= 1.96 && LNG >= 0)
  {
    zone_hours = +0L;
  } // UTC±0.R10-12/20
  if (LAT >= 0 && LAT <= 6 && LNG <= 6 && LNG >= 0 || LAT >= 0 && LAT <= 2.1 && LNG <= 7.5 && LNG >= 6 || LAT >= -1.5 && LAT <= 0 && LNG <= 5.2 && LNG >= -7.5)
  {
    zone_hours = +0L;
  } // UTC±0.R13-15/20
  if (LAT >= -50 && LAT <= -1.5 && LNG <= 7.5 && LNG >= -7.5 || LAT >= -8 && LAT <= -7.8 && LNG <= -7.5 && LNG >= -15 || LAT >= -41 && LAT <= -35 && LNG <= -7.5 && LNG >= -15)
  {
    zone_hours = +0L;
  } // UTC±0.R16-18/20
  if (LAT >= -60 && LAT <= -50 && LNG <= 1.5 && LNG >= -7.5 || LAT >= -60 && LAT <= -56 && LNG <= 7.5 && LNG >= 1.5 || LAT >= 20 && LAT <= 26 && LNG <= -5 && LNG >= -12)
  {
    zone_hours = +0L;
  } // UTC±0.R19,19.2,20/20

  //(25/38TZ)UTC-1,16 Region
  if (LAT >= 84 && LAT <= 90 && LNG <= -7.5 && LNG >= -22.5 || LAT >= 79.2 && LAT <= 84 && LNG <= -7.5 && LNG >= -9 || LAT >= 71.5 && LAT <= 79.2 && LNG <= -7.5 && LNG >= -15)
  {
    zone_hours = -1L;
  } // UTC-1.R1-3/16
  if (LAT >= 70.2 && LAT <= 71.5 && LNG <= -22.5 && LNG >= -25.5 || LAT >= 67 && LAT <= 71.5 && LNG <= -7.5 && LNG >= -22.5 || LAT >= 45 && LAT <= 63 && LNG <= -15 && LNG >= -22.5)
  {
    zone_hours = -1L;
  } // UTC-1.R4-6/16
  if (LAT >= 60 && LAT <= 63 && LNG <= -7.5 && LNG >= -15 || LAT >= 50 && LAT <= 60 && LNG <= -11.25 && LNG >= -15 || LAT >= 45 && LAT <= 50 && LNG <= -7.5 && LNG >= -15)
  {
    zone_hours = -1L;
  } // UTC-1.R7-9/16
  if (LAT >= 36.5 && LAT <= 40 && LNG <= -22.5 && LNG >= -32 || LAT >= -60 && LAT <= 45 && LNG <= -18.75 && LNG >= -22.5 || LAT >= -41 && LAT <= 0 && LNG <= -15 && LNG >= -18.75)
  {
    zone_hours = -1L;
  } // UTC-1.R10-12/16
  if (LAT >= -7.8 && LAT <= 0 && LNG <= -7.5 && LNG >= -15 || LAT >= -35 && LAT <= -8 && LNG <= -7.5 && LNG >= -15)
  {
    zone_hours = -1L;
  } // UTC-1.R13,14/16
  if (LAT >= -60 && LAT <= -41 && LNG <= -7.5 && LNG >= -18.75 || LAT >= 14 && LAT <= 18 && LNG <= -22.5 && LNG >= -30)
  {
    zone_hours = -1L;
  } // UTC-1.R15,16/16

  //(26/38TZ)UTC-2,11 Region
  if (LAT >= 84 && LAT <= 90 && LNG <= -22.5 && LNG >= -37.5 || LAT >= 63 && LAT <= 65.5 && LNG <= -30 && LNG >= -37.5 || LAT >= 63 && LAT <= 67 && LNG <= -25 && LNG >= -30)
  {
    zone_hours = -2L;
  } // UTC-2.R1-3/11
  if (LAT >= 40 && LAT <= 63 && LNG <= -22.5 && LNG >= -37.5 || LAT >= 36.5 && LAT <= 40 && LNG <= -32 && LNG >= -37.5 || LAT >= 18 && LAT <= 36.5 && LNG <= -22.5 && LNG >= -37.5)
  {
    zone_hours = -2L;
  } // UTC-2.R4-6/11
  if (LAT >= 14 && LAT <= 18 && LNG <= -30 && LNG >= -37.5 || LAT >= -4 && LAT <= -14 && LNG <= -22.5 && LNG >= -37.5 || LAT >= -12 && LAT <= -4 && LNG <= -22.5 && LNG >= -34)
  {
    zone_hours = -2L;
  } // UTC-2.R7-9/11
  if (LAT >= -60 && LAT <= -12 && LNG <= -22.5 && LNG >= -37.5 || LAT >= -55.5 && LAT <= -52.7 && LNG <= -37.5 && LNG >= -45)
  {
    zone_hours = -2L;
  } //-2 zone_hours.R10-11/11

  //(27/38TZ)UTC-3,23 Region
  if (LAT >= -9.3 && LAT <= 90 && LNG <= -37.5 && LNG >= -52.5 || LAT >= 79.2 && LAT <= 83 && LNG <= -52.5 && LNG >= -61 || LAT >= 79.2 && LAT <= 81 && LNG <= -61 && LNG >= -67.5)
  {
    zone_hours = -3L;
  } // UTC-3.R1-3/23
  if (LAT >= 77 && LAT <= 79.2 && LNG <= -52.5 && LNG >= -73.2 || LAT >= 76 && LAT <= 84 && LNG <= -22.5 && LNG >= -37.5 || LAT >= 79.2 && LAT <= 84 && LNG <= -9 && LNG >= -22.5)
  {
    zone_hours = -3L;
  } // UTC-3.R4-6/23
  if (LAT >= 71.5 && LAT <= 76 && LNG <= -15 && LNG >= -37.5 || LAT >= 70.2 && LAT <= 71.5 && LNG <= -25.5 && LNG >= -37.5 || LAT >= 67 && LAT <= 70.2 && LNG <= -22.5 && LNG >= -37.5)
  {
    zone_hours = -3L;
  } // UTC-3.R7-9/23
  if (LAT >= 65.5 && LAT <= 67 && LNG <= -30 && LNG >= -37.5 || LAT >= -9.3 && LAT <= 5.8 && LNG <= -52.5 && LNG >= -57 || LAT >= -12 && LAT <= -4 && LNG <= -34 && LNG >= -37.5)
  {
    zone_hours = -3L;
  } // UTC-3.R10-12/23
  if (LAT >= -21.5 && LAT <= -9.3 && LNG <= -37.5 && LNG >= -54 || LAT >= -27.5 && LAT <= -21.5 && LNG <= -37.5 && LNG >= -54 || LAT >= -52.7 && LAT <= -27.5 && LNG <= -37.5 && LNG >= -67.5)
  {
    zone_hours = -3L;
  } // UTC-3.R13-15/23
  if (LAT >= -27.5 && LAT <= -21.5 && LNG <= -59 && LNG >= -67.5 || LAT >= -60 && LAT <= -52.7 && LNG <= -45 && LNG >= -52.5 || LAT >= -60 && LAT <= -55.5 && LNG <= -37.5 && LNG >= -45)
  {
    zone_hours = -3L;
  } // UTC-3.R16-18/23
  if (LAT >= -49 && LAT <= -31 && LNG <= -67.5 && LNG >= -71.7 || LAT >= -56 && LAT <= -49 && LNG <= -67.5 && LNG >= -75.5 || LAT >= 70.8 && LAT <= 76 && LNG <= -52.5 && LNG >= -60)
  {
    zone_hours = -3L;
  } // UTC-3.R19-21/23
  if (LAT >= 60 && LAT <= 70.8 && LNG <= -52.5 && LNG >= -57 || LAT >= 46 && LAT <= 47.2 && LNG <= -55 && LNG >= -57)
  {
    zone_hours = -3L;
  } // UTC-3.R22-23/23

  //(28/38TZ)UTC−03:30,1 Region
  if (LAT >= 47.2 && LAT <= 52 && LNG <= -52.5 && LNG >= -57 || LAT >= 46 && LAT <= 50.2 && LNG <= -57 && LNG >= -59 || LAT >= 46 && LAT <= 47.2 && LNG <= -52.5 && LNG >= -55)
  {
    zone_hours = -3.5L;
  } // UTC-3 1/2 .R1/1

  //(29/38TZ)UTC-4,18 Region
  if (LAT >= 83 && LAT <= 90 && LNG <= -52.5 && LNG >= -67.5 || LAT >= 70.8 && LAT <= 76 && LNG <= -60 && LNG >= -67.5 || LAT >= 60 && LAT <= 70.8 && LNG <= -57 && LNG >= -60)
  {
    zone_hours = -4L;
  } // UTC-4.R1-3/18
  if (LAT >= 54 && LAT <= 60 && LNG <= -52.5 && LNG >= -64 || LAT >= 52 && LAT <= 54 && LNG <= -52.5 && LNG >= -67.5 || LAT >= 48.3 && LAT <= 52 && LNG <= -59 && LNG >= -61.5)
  {
    zone_hours = -4L;
  } // UTC-4.R4-6/18
  if (LAT >= 50.2 && LAT <= 52 && LNG <= -57 && LNG >= -59 || LAT >= -21.5 && LAT <= 48.3 && LNG <= -59 && LNG >= -67.5 || LAT >= 5.8 && LAT <= 46 && LNG <= -52.5 && LNG >= -59)
  {
    zone_hours = -4L;
  } // UTC-4.R7,9,10/18
  if (LAT >= 7 && LAT <= 27.7 && LNG <= -67.5 && LNG >= -71.7 || LAT >= -21.5 && LAT <= 5.8 && LNG <= -57 && LNG >= -59 || LAT >= -21.5 && LAT <= -9.3 && LNG <= -51 && LNG >= -57)
  {
    zone_hours = -4L;
  } // UTC-4.R11-13/18
  if (LAT >= -18.5 && LAT <= 1.3 && LNG <= -67.5 && LNG >= -69 || LAT >= -27.5 && LAT <= -21.5 && LNG <= -54 && LNG >= -59 || LAT >= -60 && LAT <= -52.7 && LNG <= -52.5 && LNG >= -67.5)
  {
    zone_hours = -4L;
  } // UTC-4.R8,14,15/18
  if (LAT >= -25 && LAT <= -18.5 && LNG <= -67.5 && LNG >= -71.7 || LAT >= -31 && LAT <= -25 && LNG <= -67.5 && LNG >= -81.6 || LAT >= -49 && LAT <= -31 && LNG <= -71.7 && LNG >= -75.5)
  {
    zone_hours = -4L;
  } // UTC-4.R16-18/18

  //(30/38TZ)UTC-5,15 Region
  if (LAT >= 79.2 && LAT <= 90 && LNG <= -67.5 && LNG >= -85 || LAT >= 81 && LAT <= 83 && LNG <= -61 && LNG >= -67.5 || LAT >= 76 && LAT <= 79.2 && LNG <= -73.2 && LNG >= -85)
  {
    zone_hours = -5L;
  } // UTC-5.R1-3/15
  if (LAT >= 28 && LAT <= 76 && LNG <= -67.5 && LNG >= -85 || LAT >= 60 && LAT <= 70.8 && LNG <= -60 && LNG >= -67.5 || LAT >= 46 && LAT <= 60 && LNG <= -85 && LNG >= -90)
  {
    zone_hours = -5L;
  } // UTC-5.R4-6/15
  if (LAT >= 54 && LAT <= 60 && LNG <= -64 && LNG >= -67.5 || LAT >= 48.3 && LAT <= 52 && LNG <= -61.5 && LNG >= -67.5 || LAT >= 22.7 && LAT <= 28 && LNG <= -67.5 && LNG >= -82.5)
  {
    zone_hours = -5L;
  } // UTC-5.R7-9/15
  if (LAT >= -25 && LAT <= 22.7 && LNG <= -71.7 && LNG >= -82.5 || LAT >= 1.3 && LAT <= 7 && LNG <= -67.5 && LNG >= -71.7 || LAT >= -18.5 && LAT <= 1.3 && LNG <= -69 && LNG >= -71.7)
  {
    zone_hours = -5L;
  } // UTC-5.R10-12/15
  if (LAT >= -31 && LAT <= -25 && LNG <= -81.6 && LNG >= -82.5 || LAT >= -60 && LAT <= -31 && LNG <= -75.5 && LNG >= -82.5 || LAT >= -60 && LAT <= -56 && LNG <= -67.5 && LNG >= -75.5)
  {
    zone_hours = -5L;
  } // UTC-5.R13-15/15

  //(31/38TZ)UTC-6,10 Region
  if (LAT >= 80 && LAT <= 90 && LNG <= -85 && LNG >= -97.5 || LAT >= 71 && LAT <= 80 && LNG <= -85 && LNG >= -103 || LAT >= 67 && LAT <= 71 && LNG <= -85 && LNG >= -90)
  {
    zone_hours = -6L;
  } // UTC-6.R1-3/10
  if (LAT >= 60 && LAT <= 67 && LNG <= -85 && LNG >= -103 || LAT >= 49 && LAT <= 60 && LNG <= -90 && LNG >= -110 || LAT >= 28 && LAT <= 49 && LNG <= -90 && LNG >= -100)
  {
    zone_hours = -6L;
  } // UTC-6.R4-6/10
  if (LAT >= 28 && LAT <= 46 && LNG <= -85 && LNG >= -90 || LAT >= 15 && LAT <= 28 && LNG <= -82.5 && LNG >= -105)
  {
    zone_hours = -6L;
  } // UTC-6.R7-8/10
  if (LAT >= -28 && LAT <= -26 && LNG <= -97.5 && LNG >= -112.5 || LAT >= -60 && LAT <= 15 && LNG <= -82.5 && LNG >= -97.5)
  {
    zone_hours = -6L;
  } // UTC-6.R9-10/10

  //(32/38TZ)UTC-7,14 Region
  if (LAT >= 80 && LAT <= 90 && LNG <= -97.5 && LNG >= -112.5 || LAT >= 71 && LAT <= 80 && LNG <= -103 && LNG >= -112.5 || LAT >= 71 && LAT <= 79 && LNG <= -112.5 && LNG >= -127.5)
  {
    zone_hours = -7L;
  } // UTC-7.R1-3/14
  if (LAT >= 60 && LAT <= 71 && LNG <= -103 && LNG >= -141 || LAT >= 67 && LAT <= 71 && LNG <= -90 && LNG >= -103 || LAT >= 55 && LAT <= 60 && LNG <= -110 && LNG >= -122)
  {
    zone_hours = -7L;
  } // UTC-7.R4-6/14
  if (LAT >= 49 && LAT <= 55 && LNG <= -110 && LNG >= -116 || LAT >= 45 && LAT <= 49 && LNG <= -100 && LNG >= -116 || LAT >= 28 && LAT <= 45 && LNG <= -100 && LNG >= -114)
  {
    zone_hours = -7L;
  } // UTC-7.R7-9/14
  if (LAT >= 24 && LAT <= 28 && LNG <= -112.5 && LNG >= -116 || LAT >= 10.3 && LAT <= 28 && LNG <= -105 && LNG >= -112.5 || LAT >= 10.2 && LAT <= 15 && LNG <= -97.5 && LNG >= -105)
  {
    zone_hours = -7L;
  } // UTC-7.R10-12/14
  if (LAT >= -26 && LAT <= 10.2 && LNG <= -105 && LNG >= -112.5 || LAT >= -60 && LAT <= -28 && LNG <= -97.5 && LNG >= -112.5)
  {
    zone_hours = -7L;
  } //-7 zone_hours.R13&14/14

  //(33/38TZ)UTC-8,8 Region
  if (LAT >= 79 && LAT <= 90 && LNG <= -112 && LNG >= -127.5 || LAT >= 55 && LAT <= 60 && LNG <= -116 && LNG >= -129 || LAT >= 45 && LAT <= 55 && LNG <= -116 && LNG >= -135)
  {
    zone_hours = -8L;
  } // UTC-8.R1-3/8
  if (LAT >= 28 && LAT <= 45 && LNG <= -114 && LNG >= -127.5 || LAT >= 24 && LAT <= 28 && LNG <= -116 && LNG >= -127.5 || LAT >= -60 && LAT <= 24 && LNG <= -112.5 && LNG >= -127.5)
  {
    zone_hours = -8L;
  } // UTC-8.R4-6/8
  if (LAT >= 10.2 && LAT <= 10.3 && LNG <= -105 && LNG >= -112.5 || LAT >= -27 && LAT <= -22 && LNG <= -127.5 && LNG >= -132)
  {
    zone_hours = -8L;
  } // UTC-8.R7&8/8

  //(34/38TZ)UTC-9,11 Region
  if (LAT >= 71 && LAT <= 90 && LNG <= -127.5 && LNG >= -142.5 || LAT >= 51.5 && LAT <= 71 && LNG <= -141 && LNG >= -168 || LAT >= 60 && LAT <= 63.5 && LNG <= -168 && LNG >= -172.5)
  {
    zone_hours = -9L;
  } // UTC-9.R1-3/11
  if (LAT >= 55 && LAT <= 60 && LNG <= -129 && LNG >= -141 || LAT >= 51.5 && LAT <= 55 && LNG <= -135 && LNG >= -141 || LAT >= 45 && LAT <= 51.5 && LNG <= -135 && LNG >= -142.5)
  {
    zone_hours = -9L;
  } // UTC-9.R4-6/11
  if (LAT >= -26 && LAT <= -22 && LNG <= -133 && LNG >= -135 || LAT >= -30 && LAT <= -26 && LNG <= -127.5 && LNG >= -135 || LAT >= -60 && LAT <= -30 && LNG <= -127 && LNG >= -142.5)
  {
    zone_hours = -9L;
  } // UTC-9.R9-11/11
  if (LAT >= -7 && LAT <= 45 && LNG <= -127.5 && LNG >= -142.5 || LAT >= -22 && LAT <= -7 && LNG <= -127.5 && LNG >= -135)
  {
    zone_hours = -9L;
  } // UTC-9.R7-9/11

  //(35/38TZ)UTC-9 1/2,1 Region
  if (LAT >= -13 && LAT <= -6 && LNG <= -135 && LNG >= -142.5)
  {
    zone_hours = -9.5;
  } //-9 1/2UTC.R1/1

  //(36/38TZ)UTC-10,18 Region
  if (LAT >= 71 && LAT <= 90 && LNG <= -142.5 && LNG >= -157.5 || LAT >= 59 && LAT <= 63.5 && LNG <= -172.5 && LNG >= -179.9 || LAT >= 54 && LAT <= 59 && LNG <= -168 && LNG >= -179.9)
  {
    zone_hours = -10L;
  } // UTC-10.R1-3/18
  if (LAT >= 54 && LAT <= 59 && LNG <= 180 && LNG >= 172.5 || LAT >= 48 && LAT <= 54 && LNG <= -168 && LNG >= -179.9 || LAT >= 48 && LAT <= 54 && LNG <= 180 && LNG >= 168)
  {
    zone_hours = -10L;
  } // UTC-10.R3-4/18
  if (LAT >= 48 && LAT <= 51 && LNG <= -142.5 && LNG >= -168 || LAT >= 30 && LAT <= 48 && LNG <= -142.5 && LNG >= -157.5 || LAT >= 27 && LAT <= 30 && LNG <= -179 && LNG >= -179.9)
  {
    zone_hours = -10L;
  } // UTC-10.R5-7/18
  if (LAT >= 24.7 && LAT <= 27 && LNG <= -176.5 && LNG >= -179.9 || LAT >= 24.7 && LAT <= 30 && LNG <= -172.5 && LNG >= -176.5 || LAT >= 15 && LAT <= 30 && LNG <= -142.5 && LNG >= -172.5)
  {
    zone_hours = -10L;
  } // UTC-10.R8-10/18
  if (LAT >= 4 && LAT <= 15 && LNG <= -150 && LNG >= -157.5 || LAT >= -13 && LAT <= 15 && LNG <= -142.5 && LNG >= -150 || LAT >= -13 && LAT <= -6 && LNG <= -155 && LNG >= -167.5)
  {
    zone_hours = -10L;
  } // UTC-10.R11-13/18
  if (LAT >= -13 && LAT <= -12 && LNG <= -150 && LNG >= -155 || LAT >= -22 && LAT <= -13 && LNG <= -135 && LNG >= -167 || LAT >= -25 && LAT <= -22 && LNG <= -157.5 && LNG >= -163)
  {
    zone_hours = -10L;
  } // UTC-10.R14-16/18
  if (LAT >= -30 && LAT <= -22 && LNG <= -135 && LNG >= -142.5 || LAT >= -60 && LAT <= -22 && LNG <= -142.5 && LNG >= -157.5)
  {
    zone_hours = -10L;
  } // UTC-10.R17&18/18

  //(37/38TZ)UTC-11,13 Region
  if (LAT >= 71 && LAT <= 90 && LNG <= -157.5 && LNG >= -172.5 || LAT >= 30 && LAT <= 48 && LNG <= -157.5 && LNG >= -172.5 || LAT >= 30 && LAT <= 35 && LNG <= -172.5 && LNG >= -179.9)
  {
    zone_hours = -11L;
  } // UTC-11.R1-3/13
  if (LAT >= 27 && LAT <= 30 && LNG <= -176.5 && LNG >= -179 || LAT >= -2.5 && LAT <= 15 && LNG <= -160.5 && LNG >= -172.5 || LAT >= 4 && LAT <= 15 && LNG <= -157.5 && LNG >= -160.5)
  {
    zone_hours = -11L;
  } // UTC-11.R4-6/13
  if (LAT >= -2.5 && LAT <= 0 && LNG <= -158.5 && LNG >= -160.5 || LAT >= -10 && LAT <= -6 && LNG <= -167.5 && LNG >= -169 || LAT >= -13 && LAT <= -10 && LNG <= -167.5 && LNG >= -171)
  {
    zone_hours = -11L;
  } // UTC-11.R7-9/13
  if (LAT >= -15 && LAT <= -13 && LNG <= -167 && LNG >= -171 || LAT >= -22 && LAT <= -15 && LNG <= -167 && LNG >= -172.5 || LAT >= -25 && LAT <= -22 && LNG <= -163 && LNG >= -172.5)
  {
    zone_hours = -11L;
  } // UTC-11.R10-12/13
  if (LAT >= -60 && LAT <= -25 && LNG <= -157.5 && LNG >= -172.5)
  {
    zone_hours = -11;
  } //-11 zone_hours.R13/13

  //(38/38TZ)UTC-12,4 Region
  if (LAT >= 75.00 && LAT <= 90.00 && LNG >= -179.99 && LNG <= -172.5 || LAT >= 75.00 && LAT <= 90.00 && LNG >= -179.99 && LNG <= -172.5 || LAT >= -2.50 && LAT <= 24.70 && LNG >= -179.99 && LNG <= -172.5)
  {
    zone_hours = -12L;
  } // UTC-12.R1&2/4
  if (LAT >= -2.50 && LAT <= 24.70 && LNG >= -179.99 && LNG <= -172.5 || LAT >= -90.0 && LAT <= -45.00 && LNG >= -179.99 && LNG <= -172.5)
  {
    zone_hours = -12L;
  } // UTC-12.R3&4/4

  // Antartica
  // if(LAT>=  -99999 && LAT <= -90  && LNG >=  180     && LNG <= -179.99 ||LAT>= 90.0    && LAT <=  99999 && LNG >= 180     && LNG <= -179.999 ){ Serial.print("Invalid Coordinate");}//...Initial Input error remark for invalid Serial read ,logically long <=360 degree & lat <=180 degree else invalid input.
  // if(LAT>=  -90    && LAT <= -60  && LNG >= -179.99  && LNG <=  180    ){ zone_hours=+0L;}//UTC0.R1&2/4  Temporary set to UTC 0 until next update
  if (LAT >= -88.75 && LAT <= -60 && LNG >= -20 && LNG <= 15 || LAT >= -88.75 && LAT <= -80 && LNG >= 15 && LNG <= 160)
  {
    zone_hours = +0L;
  } // UTC 0.R1&2/4
  if (LAT >= -88.75 && LAT <= -80 && LNG >= -150 && LNG <= -90 || LAT >= -80 && LAT <= -77 && LNG >= 100 && LNG <= 110)
  {
    zone_hours = +0L;
  } // UTC 0.R3&4/4
  if (LAT >= -80 && LAT <= -60 && LNG >= 15 && LNG <= 45)
  {
    zone_hours = +3L;
  } // UTC+3.R1/1
  if (LAT >= -80 && LAT <= -60 && LNG >= 45 && LNG <= 70)
  {
    zone_hours = +6L;
  } // UTC+6.R1/1
  if (LAT >= -80 && LAT <= -60 && LNG >= 70 && LNG <= 100)
  {
    zone_hours = +7L;
  } // UTC+7.R1/1
  if (LAT >= -77 && LAT <= -60 && LNG >= 100 && LNG <= 110 || LAT >= -80 && LAT <= -60 && LNG >= 110 && LNG <= 135)
  {
    zone_hours = +8L;
  } // UTC+8.R1&2/2
  if (LAT >= -80 && LAT <= -60 && LNG >= 135 && LNG <= 160)
  {
    zone_hours = +10L;
  } // UTC+10.R1/1
  if (LAT >= -90 && LAT <= -88.75 && LNG >= -179.99 && LNG <= 180 || LAT >= -88.75 && LAT <= -60 && LNG >= 160 && LNG <= 180 || LAT >= -88.75 && LAT <= -60 && LNG >= -179.99 && LNG <= -150)
  {
    zone_hours = +13L;
  } // UTC+13.R1-3/3
  if (LAT >= -80 && LAT <= -60 && LNG >= -150 && LNG <= -90)
  {
    zone_hours = -6L;
  } // UTC-6.R1/1
  if (LAT >= -88.75 && LAT <= -60 && LNG >= -90 && LNG <= -20)
  {
    zone_hours = -3L;
  } // UTC-3.R1/1

  return zone_hours;
}

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

int drawString(int x, int y, String text, alignment align)
{
  uint16_t w, h;
  display.setTextWrap(false);

  display.setCursor(x, y);
  display.print(text);
  //    display.drawRect(x, y - u8g2Fonts.getFontDescent(), w, h, GxEPD_BLACK);
  return x + w;
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

double DrawMoon(int x, int y, int dd, int mm, int yy, String hemisphere)
{
  const int diameter = 47;
  double Phase = NormalizedMoonPhase(dd, mm, yy);
  hemisphere.toLowerCase();
  if (hemisphere == "south")
    Phase = 1 - Phase;

  // Draw dark part of moon
  display.fillCircle(x + diameter - 1, y + diameter, diameter / 2 + 1, GxEPD_BLACK);
  const int number_of_lines = 90;
  for (double Ypos = 0; Ypos <= number_of_lines / 2; Ypos++)
  {
    double Xpos = sqrt(number_of_lines / 2 * number_of_lines / 2 - Ypos * Ypos);
    // Determine the edges of the lighted part of the moon
    double Rpos = 2 * Xpos;
    double Xpos1, Xpos2;
    if (Phase < 0.5)
    {
      Xpos1 = -Xpos;
      Xpos2 = Rpos - 2 * Phase * Rpos - Xpos;
    }
    else
    {
      Xpos1 = Xpos;
      Xpos2 = Xpos - 2 * Phase * Rpos + Rpos;
    }
    // Draw light part of moon
    // Marani: Fixed the calculation to draw the moon phases smoothly without straying pixels
    double pW1x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
    double pW1y = ceil((number_of_lines - Ypos) / number_of_lines * diameter + y);
    double pW2x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
    double pW2y = floor((Ypos + number_of_lines) / number_of_lines * diameter + y);
    bool draw_fill = false;
    if (Phase < 0.48)
    {
      pW1x = ceil(pW1x);
      pW2x = floor(pW2x - 1);
      draw_fill = true;
    }
    else if (Phase > 0.52 && Phase <= 1.0)
    {
      pW1x = floor(pW1x - 1);
      pW2x = ceil(pW2x);
      draw_fill = true;
    }
    if (draw_fill)
    {
      display.drawLine(pW1x, pW1y, pW2x, pW1y, GxEPD_WHITE);
      display.drawLine(pW1x, pW2y, pW2x, pW2y, GxEPD_WHITE);
    }
  }
  display.drawCircle(x + diameter - 1, y + diameter, diameter / 2, GxEPD_BLACK);
  return Phase;
}

void DisplayAstronomySection(int x, int y)
{
  display.drawRect(x, y + 16, 409, 59, GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  time_t _now = time(NULL);
  struct tm *now_utc = gmtime(&_now);
  const int day_utc = now_utc->tm_mday;
  const int month_utc = now_utc->tm_mon + 1;
  const int year_utc = now_utc->tm_year + 1900;
  drawString(x + 170, y + 50, MoonPhase(day_utc, month_utc, year_utc, Hemisphere), LEFT);
  DrawMoon(x + 320, y - 2, day_utc, month_utc, year_utc, Hemisphere);

  time_t utcOffset = mktime(now_utc) - _now;
  // TODO FIX THIS:
  // m.calculate(_now + utcOffset + WxConditions[0].Timezone);
  m.calculate(_now + utcOffset);
  mr.calculate(gps.location.lat(), gps.location.lng(), _now + utcOffset);
  time_t moonRiseTime = mr.riseTime - utcOffset;
  struct tm *moonRiseTimeInfo = localtime(&moonRiseTime);
  time_t moonSetTime = mr.setTime - utcOffset;
  struct tm *moonSetTimeInfo = localtime(&moonSetTime);
  char LCDTime[] = "HH:MM";
  int xt = 0;
  sprintf(LCDTime, "%02d:%02d", moonRiseTimeInfo->tm_hour, moonRiseTimeInfo->tm_min);
  xt = drawString(x + 170, y + 17, LCDTime, LEFT);
  drawString(xt + 5, y + 17, "Moonrise", LEFT);
  sprintf(LCDTime, "%02d:%02d", moonSetTimeInfo->tm_hour, moonSetTimeInfo->tm_min);
  xt = drawString(x + 170, y + 28, LCDTime, LEFT);
  drawString(xt + 5, y + 28, "Moonset", LEFT);
  drawString(x + 8, y + 39, String(m.fraction * 100, 1) + "% Illuminated", LEFT);
  xt = drawString(x + 8, y + 50, "Zodiac", LEFT);
  drawString(xt + 5, y + 50, m.zodiacName, LEFT);
  drawString(x + 170, y + 39, String(m.distance * 6371) + "km Distance", LEFT);
  drawString(x + 8, y + 61, String(m.age) + " Days Moonage", LEFT);
  drawString(x + 170, y + 61, String(m.longitude, 2) + "/" + String(m.latitude, 2) + " Lon/Lat", LEFT);
}

void initDisplay()
{
  display.init(115200, true, 2, false);

  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(GxEPD_BLACK);
  display.setTextWrap(false);
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
  display.firstPage();
  return;
} // end initDisplay

void printDisplayMessage(String location, String date, String time, String weekday)
{
  int16_t tbx, tby;
  uint16_t tbw, tbh, x_time, y_time, x_weekday, y_weekday;

  display.setFont(&FreeSans24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);

  // Time: center the bounding box by transposition of the origin:
  display.getTextBounds(time, 0, 0, &tbx, &tby, &tbw, &tbh);
  x_time = ((display.width() - tbw) / 2) - tbx;
  y_time = 200;
  // Weekday: center the bounding box by transposition of the origin:
  display.getTextBounds(weekday, 0, 0, &tbx, &tby, &tbw, &tbh);
  x_weekday = ((display.width() - tbw) / 2) - tbx;
  y_weekday = 250;

  Serial.println("x_time: " + String(x_time) + " y_time: " + String(y_time));
  Serial.println("x_weekday: " + String(x_weekday) + " y_weekday: " + String(y_weekday));

  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x_time, y_time);
    display.print(time);
    display.setTextSize(1);
    display.setCursor(x_time, y_weekday);
    display.print(weekday);
    //  display.print(date);
    //  display.setCursor(x, y + 60);
    //  display.print(location);
    DisplayAstronomySection(50, 300);
    // display.drawInvertedBitmap(x_time, 300, battery_0_bar_0deg_128x128, WIDTH, HEIGHT, GxEPD_BLACK);
  } while (display.nextPage());
}

String locationBuffer;
String dateBuffer;
String timeBuffer;

void displayInfo()
{
  locationBuffer = String(gps.location.lat()) + " " + String(gps.location.lng());

  char timeWeekDay[10];
  uint8_t hour;
  uint8_t minute;

  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    int desiredTimezoneOffset = (int)decodeTimezone(gps.location.lat(), gps.location.lng());
    timeinfo.tm_hour += desiredTimezoneOffset;
    mktime(&timeinfo);

    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    dateBuffer = String(timeinfo.tm_mon) + "/" + String(timeinfo.tm_mday) + "/" + String(timeinfo.tm_year);
    strftime(timeWeekDay, 10, "%A", &timeinfo);

    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  String hour_str;
  String minute_str;

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

  timeBuffer = hour_str + ":" + minute_str;

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
  // Wake up after amount of time
  esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
  // Enter deep sleep
  esp_deep_sleep_start();
}

void loop()
{
  while (ss.available() > 0)
  {
    gps.encode(ss.read());
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }

  if (!isTimeConfigured && gps.time.isValid() && gps.location.isValid())
  {
    isTimeConfigured = true;
    setSystemTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.println("Time is configured");
  }

  if (isTimeConfigured)
  {
    displayInfo();
    display.hibernate();
    sleep();
  }
}
