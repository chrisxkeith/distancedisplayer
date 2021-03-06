// Please credit chris.keith@gmail.com .

const String githubHash("to be filled in after 'git push'");
const String githubRepo("https://github.com/chrisxkeith/distancedisplayer");
const bool debug = true;

void to_serial(String s) {
  char buf[12];
  sprintf(buf, "%10u", millis());
  String s1(buf);
  s1.concat(" ");
  s1.concat(s);
  Serial.println(s1);
}

class MicrosecondTimer {
  private:
    long start;
    String name;
  public:
    MicrosecondTimer(String name) {
        start = micros();
        this->name = name;
        this->name.replace(" ", "_");
    }
    ~MicrosecondTimer() {
      if (debug) {
        long duration = micros() - start;
        name.concat("_microseconds = ");
        name.concat(duration);
        name.concat(";");
        to_serial(name);
      }
    }
};

#define USE_LASER_SENSOR

#ifdef USE_LASER_SENSOR

#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

#include <Wire.h>
#include "SparkFun_VL53L1X.h"

SFEVL53L1X distanceSensor;

void setup_distance_sensor() {
  Wire.begin();
  if (distanceSensor.begin() == 0) { // Begin returns 0 on a good init
    to_serial("Laser distance sensor online.");
  }
}

long calc_distance() {
  MicrosecondTimer("laser sensor calc distance");
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

void sample() {
}
String dump() {
  return String("");
}
#else // USE_LASER_SENSOR

class UltrasonicSensor {
  private:
    const int trigPin = 6;
    const int echoPin = 7;

    long      nSamples = 0;
    double    sumSamples = 0.0;

    long do_sample() {
      digitalWrite(trigPin, LOW);   // Clear the trigPin
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);  // Set the trigPin on HIGH state for n microseconds
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
    
      long ret = pulseIn(echoPin, HIGH);
      // Read the echoPin, return the sound wave travel time in microseconds
      return ret;
    }
    
  public:
    void setup_distance_sensor() {
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }
    
    void sample() {
      int distanceInFeet;
      do {
          distanceInFeet = round((do_sample() * 0.034 / 2) * 0.032);
      } while (distanceInFeet < 0);
      nSamples++;
      sumSamples += distanceInFeet;
    }

    long calc_distance() {
      long v = round(sumSamples / nSamples);
      nSamples = 0;
      sumSamples = 0.0;
      return v;
    }

    String dump() {
      String s("nSamples = ");
      s.concat(nSamples);
      s.concat("; sumSamples = ");
      s.concat(sumSamples);
      s.concat(";");
      return s;
    }
};
UltrasonicSensor ultrasonicSensor;

long calc_distance() {
  return ultrasonicSensor.calc_distance();
}
void setup_distance_sensor() {
  ultrasonicSensor.setup_distance_sensor();
}
void sample() {
  ultrasonicSensor.sample();
}
String dump() {
  return ultrasonicSensor.dump();
}

#endif // else USE_LASER_SENSOR

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
  MicrosecondTimer("drawInt");
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
  to_serial("Started setup...");
  setup_distance_sensor();
  setup_OLED();
  to_serial(githubRepo);
  to_serial(githubHash);
  drawUTF8(githubHash.substring(0,12));
  delay(3000);
  to_serial("Finished setup...");
}

long previous_dist = -1;
long previous_display_time = 0;

void do_dump(long dist) {
  if (debug) {
    String d = dump();
    d.concat("distance_in_feet = ");
    d.concat(dist);
    d.concat(";");
    to_serial(d);
  }
}

void loop() {
  // Don't redraw faster than once a second.
  long now = millis();
  if (now - previous_display_time > 1000) {
    previous_display_time = now;
    sample();
    long dist = calc_distance();
    if ((dist < 16) && (previous_dist != dist)) {
      drawInt(dist);
      previous_dist = dist;
      do_dump(dist);
    }
  }
}
