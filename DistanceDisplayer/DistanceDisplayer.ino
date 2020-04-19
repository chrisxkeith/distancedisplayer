
# Please credit chris.keith@gmail.com .

const int trigPin = 6;
const int echoPin = 7;

const bool dbg = true;
void dbg_print(String s) {
  if (dbg) {
    String d = "debug: ";
    d.concat(s);
    Serial.println(d);
  }
}

long calc_distance() {
  int distanceInFeet = 200; 
  do {      
    digitalWrite(trigPin, LOW);   // Clear the trigPin
    delayMicroseconds(20);
    digitalWrite(trigPin, HIGH);  // Set the trigPin on HIGH state for 100 microseconds
    delayMicroseconds(100);
    digitalWrite(trigPin, LOW);
    
    // Read the echoPin, return the sound wave travel time in microseconds
    long duration = pulseIn(echoPin, HIGH);
    if (duration > 0) {
      String s = "duration(2) = ";
      s.concat(duration);
      dbg_print(s);
      distanceInFeet = round((duration * 0.034 / 2) * 0.032);
    }
  } while (distanceInFeet > 16);
  // Possibly In The Future: Figure out where the 103/104 distanceInFeet come from
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
  Serial.println("Finished setup...");
}

void loop() {
  Serial.println(String(calc_distance()) + " ft.");
  delay(1000);
}
