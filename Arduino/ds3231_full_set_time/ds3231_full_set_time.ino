// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(57600);

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);


#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power — it will be set below.");
  }

  // ***** SET THE CLOCK HERE — EDIT THIS LINE FOR EVERY BOARD *****
  // Type the date & time in the DEPLOYMENT time zone, a minute or two in the
  // FUTURE, then upload and let the real clock reach that moment.
  //   Format: DateTime(YYYY, MM, DD, hh, mm, ss)   (24-hour clock)
  //   Example below = 2026-08-12, 14:35:00.
  //
  // Why edit it every board: changing this line forces the IDE to recompile,
  // which defeats the build-cache trap that otherwise re-stamps the PREVIOUS
  // board's time. It also sets an exact, known value (no dependence on when the
  // sketch was compiled or on your computer's time zone).
  //
  // This runs unconditionally (not gated on lostPower), so it sets the clock on
  // every upload. Always confirm afterward with the ds3231_print_date sketch.
  rtc.adjust(DateTime(2026, 8, 12, 14, 35, 0));
}

void loop () {
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(3000);
}
