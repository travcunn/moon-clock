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
#include <Fonts/FreeMonoBold12pt7b.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RXPin = 17, TXPin = 16;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Wave e-ink display
GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display(GxEPD2_750c_Z08(/*CS=5*/ D9, /*DC=*/SCL, /*RST=*/SDA, /*BUSY=*/D7));

void printDisplayMessage(String location, String date, String time)
{
  display.setRotation(1);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(time, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(location);
    display.setCursor(x, y + 30);
    display.print(date);
    display.setCursor(x, y + 60);
    display.print(time);
  } while (display.nextPage());
}

String locationBuffer;
String dateBuffer;
String timeBuffer;

void displayInfo()
{

  Serial.println(gps.location.lat());
  Serial.println(gps.location.lng());
  Serial.println(gps.date.month());
  Serial.println(gps.date.day());
  Serial.println(gps.date.year());
  Serial.println(gps.time.hour());
  Serial.println(gps.time.minute());
  Serial.println(gps.time.second());

  locationBuffer = "Location: " + String(gps.location.lat()) + " " + String(gps.location.lng());
  dateBuffer = "Date: " + String(gps.date.month()) + "/" + String(gps.date.day()) + "/" + String(gps.date.year());
  timeBuffer = "Time: " + String(gps.time.hour()) + ":" + String(gps.time.minute());

  printDisplayMessage(locationBuffer, dateBuffer, timeBuffer);
}

void setup()
{
  Serial.begin(115200);

  ss.begin(GPSBaud);

  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse

  display.clearScreen(GxEPD_WHITE);
  display.hibernate();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
  {
    gps.encode(ss.read());
  }

  if (gps.time.isUpdated())
  {
    displayInfo();
    display.hibernate();
    delay(180000);
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }
}
