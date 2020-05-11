// Please credit chris.keith@gmail.com .

const String githubHash("to be filled in after 'git push'");

// #define USE_LASER_SENSOR

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
    Serial.println("Laser distance sensor online.");
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
  digitalWrite(trigPin, LOW);   // Clear the trigPin
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Set the trigPin on HIGH state for n microseconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long ret = pulseIn(echoPin, HIGH);
  // Read the echoPin, return the sound wave travel time in microseconds
  return ret;
}

void setup_distance_sensor() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

long calc_distance() {
  long start = millis();
  int distanceInFeet = 17;
  do {
    long duration = sample();
    if (duration > 0) {
      distanceInFeet = round((duration * 0.034 / 2) * 0.032);
    }
    if (millis() - start > (1000 * 60)) {
      return -1; // No data after 1 minute? Return error code.
    }
  } while ((distanceInFeet > 16));
  return(distanceInFeet);
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
void loop() {
  long dist = calc_distance();
  if (dist < 0) {
    drawUTF8("No data...");
    delay(5 * 1000);
  } else {
    if (previous_dist != dist) {
      drawInt(dist);
      previous_dist = dist;
    } else {
    }
  }
}
