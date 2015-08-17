#include <TinyGPS++.h>
/*
   Original source and place to get library;
    https://github.com/mikalhart/TinyGPSPlus/tree/master/examples/FullExample

   This sample code demonstrates the use of a Adafruit Huzzah ESP8266 and it's two serial ports.

   FYI. Adafruit Huzzah ESP8266 --> Serial1 uses UART1, TX pin is GPIO2. 
   UART1 can not be used to receive data because normally it's RX pin is 
   occupied for flash chip connection. To use Serial1, call Serial1.begin(baudrate).

   This is the setup after you have programed the ESP8266
   
   Serial1 on GPIO#2 for debugging as it is TX only.
   Serial(0) is TX/RX and is the upload port as wellfor the GPS.

   Huzzah ESP8266 RX <- GPS TX (serial)
   Huzzah ESP8266 TX (GPIO#2) -> Console Cable RX (serial1) -> used for debugging only.
   
   I am using a Adafruit GPS, so the baud rate is 9600.
   
*/

// The TinyGPS++ object
TinyGPSPlus gps;


void setup()
{
  Serial.begin(9600);  //   GPS
  Serial1.begin(115200);  // Console Debug

  Serial1.println(F("FullExample.ino"));
  Serial1.println(F("An extensive example of many interesting TinyGPS++ features"));
  Serial1.print(F("Testing TinyGPS++ library v. ")); Serial1.println(TinyGPSPlus::libraryVersion());
  Serial1.println(F("by Mikal Hart"));
  Serial1.println();
  Serial1.println(F("Sats HDOP Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial1.println(F("          (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial1.println(F("---------------------------------------------------------------------------------------------------------------------------------------"));
}

void loop()
{
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printInt(gps.hdop.value(), gps.hdop.isValid(), 5);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.value()) : "*** ", 6);

  unsigned long distanceKmToLondon =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON);

  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial1.println();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial1.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial1.print('*');
    Serial1.print(' ');
  }
  else
  {
    Serial1.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial1.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial1.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial1.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial1.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial1.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial1.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial1.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
