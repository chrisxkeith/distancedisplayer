
// Please credit chris.keith@gmail.com .

const bool dbg = false;
void dbg_print(String msg, long val) {
  if (dbg) {
    String d = "debug: ";
    d.concat(msg);
    d.concat("=");
    d.concat(val);
    Serial.println(d);
  }
}

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

void draw_screen(int val) {
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

const int trigPin = 6;
const int echoPin = 7;

long sample() {
    digitalWrite(trigPin, LOW);   // Clear the trigPin
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);  // Set the trigPin on HIGH state for n microseconds
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Read the echoPin, return the sound wave travel time in microseconds
    return pulseIn(echoPin, HIGH);
}

long calc_distance() {
  int distanceInFeet = 17;
  do {
    long duration = sample();
    if (duration > 0) {
      dbg_print("duration", duration);
      distanceInFeet = round((duration * 0.034 / 2) * 0.032);
    }
  } while (distanceInFeet > 16);
  // Possibly In The Future: Figure out where the 100+ distanceInFeet come from.
  return(distanceInFeet);
}

void setup_distance_sensor() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Started setup...");
  setup_distance_sensor();
  setup_OLED();
  Serial.println("Finished setup...");
  delay(2000);
}

long previous_dist = -1;
void loop() {
  long dist = calc_distance();
  if (previous_dist != dist) {
    Serial.println(String(dist) + " ft.");
    draw_screen(dist);
    previous_dist = dist;
  } else {
    delay(1000);
  }
}
