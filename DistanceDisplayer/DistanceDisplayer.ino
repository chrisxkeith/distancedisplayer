const bool dbg = false;
const int trigPin = 6;
const int echoPin = 7;

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
      distanceInFeet = round((duration * 0.034 / 2) * 0.032);
      if (dbg) {
        String s = "debug: duration = ";
        s.concat(duration);
        Serial.println(s);
      }
    }
  } while (distanceInFeet > 100);
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
