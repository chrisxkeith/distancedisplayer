// Please credit chris.keith@gmail.com .

const String githubHash("to be filled in after 'git push'");

class EventSaver {
#define NUM_EVENTS 100
  private:
    const int MAX_STRING_LENGTH = 32256;
    String    events[NUM_EVENTS];
    int       nextEventIndex = 0;

    void increment(int* i) {
      if (*i < NUM_EVENTS - 1) {
        (*i)++;
      } else {
        *i = 0;
      }
    }
  public:
    EventSaver() {
      for (int i = 0; i < NUM_EVENTS; i++) {
        events[i] = "";
      }
    }
    void addEvent(String event) {
      events[nextEventIndex] = String(millis());
      events[nextEventIndex].concat(",");
      events[nextEventIndex].concat(event);
      events[nextEventIndex].concat(";");
      increment(&nextEventIndex);
    }
    String dump() {
      String s("Eventsaver: nextEventIndex=");
      s.concat(nextEventIndex);
      return s;
    }
    void printEvents() {
      String s = "";
      int lastEventIndex = nextEventIndex;
      while (events[lastEventIndex].length() > 0) {
        if (events[lastEventIndex].length() + s.length() < MAX_STRING_LENGTH) {
          s.concat(events[lastEventIndex]);
        } else {
          Serial.println(s);
          s = "";
        }
        increment(&lastEventIndex);
        if (lastEventIndex == nextEventIndex) {
          Serial.println(s);
          break;
        }
      }
    }
};
EventSaver eventSaver;

// #define USE_LASER_SENSOR

#ifdef USE_LASER_SENSOR
#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
*/

#include <Wire.h>
#include "SparkFun_VL53L1X.h"

SFEVL53L1X distanceSensor;

void setup_distance_sensor() {
  Wire.begin();
  if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
  {
    Serial.println("Laser distance sensor online!");
  }
}

long calc_distance() {
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady()) {
    delay(1);
  }
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  float distanceInches = distance * 0.0393701;
  float distanceFeet = distanceInches / 12.0;
  return (long)round(distanceFeet);
}
#else // USE_LASER_SENSOR

const int trigPin = 6;
const int echoPin = 7;

long sample() {
  eventSaver.addEvent("sample");
  digitalWrite(trigPin, LOW);   // Clear the trigPin
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Set the trigPin on HIGH state for n microseconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long ret = pulseIn(echoPin, HIGH);
  // Read the echoPin, return the sound wave travel time in microseconds
  return ret;
}

long calc_distance() {
  eventSaver.addEvent("calc_distance");
  String samples;
  long start = millis();
  int distanceInFeet = 17;
  do {
    long duration = sample();
    if (duration > 0) {
      distanceInFeet = round((duration * 0.034 / 2) * 0.032);
    }
    samples.concat(millis());
    samples.concat(",");
    samples.concat(duration);
    samples.concat(",");
    samples.concat(distanceInFeet);
    samples.concat(";");
  } while (distanceInFeet > 16); // TODO : Add timeout and fail message.
  // Possibly In The Future: Figure out where the 100+ distanceInFeet come from.
  long d = millis() - start;
  if (d > 1000) {
    String s("delay: ");
    s.concat(d);
    Serial.println(s);
    Serial.println(samples);
  }
  return(distanceInFeet);
}

void setup_distance_sensor() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
#endif // USE_LASER_SENSOR

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1327_EA_W128128_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); /* Uno: A4=SDA, A5=SCL, add "u8g2.setBusClock(400000);" into setup() for speedup if possible */

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_inb63_mn);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void drawUTF8(String val) {
  u8g2.firstPage();
  do {
      u8g2_prepare();
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawUTF8(0, 0, val.c_str());
  } while( u8g2.nextPage() );
}

void drawInt(int val) {
  eventSaver.addEvent("drawInt");
  u8g2.firstPage();
  do {
      u8g2_prepare();
      u8g2.drawUTF8(30, 10, String(val).c_str());
  } while( u8g2.nextPage() );
}

void setup_OLED() {
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(10, 0);
  digitalWrite(9, 0);
  u8g2.begin();
  u8g2.setBusClock(400000);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Started setup...");
  setup_distance_sensor();
  setup_OLED();
  Serial.println("Finished setup...");
  Serial.println(githubHash);
  drawUTF8(githubHash.substring(0,12));
  delay(5000);
}

long previous_dist = -1;
long lastHeartBeat = 0;
void loop() {
  eventSaver.addEvent("loop");
  long dist = calc_distance();
  if (previous_dist != dist) {
    drawInt(dist);
    previous_dist = dist;
  } else {
    delay(500);
  }
}
