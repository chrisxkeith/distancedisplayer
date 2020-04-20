
// Please credit chris.keith@gmail.com .

const int trigPin = 6;
const int echoPin = 7;

const bool dbg = false;
void dbg_print(String s) {
  if (dbg) {
    String d = "debug: ";
    d.concat(s);
    Serial.println(d);
  }
}

long sample() {
    digitalWrite(trigPin, LOW);   // Clear the trigPin
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);  // Set the trigPin on HIGH state for n microseconds
    delayMicroseconds(20);
    digitalWrite(trigPin, LOW);
    
    // Read the echoPin, return the sound wave travel time in microseconds
    return pulseIn(echoPin, HIGH);
}

long calc_distance() {
  int distanceInFeet = 200;
  do {
    long sum_samples = 0;
    const int MAX_SAMPLES = 5;
    for (int n_samples = 0; n_samples < MAX_SAMPLES; n_samples++) {
        sum_samples = sample();
    }
    long duration = sum_samples / MAX_SAMPLES;
    if (duration > 0) {
      String s = "duration(2) = ";
      s.concat(duration);
      dbg_print(s);
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
  Serial.println("Finished setup...");
}

void loop() {
  Serial.println(String(calc_distance()) + " ft.");
  delay(1000);
}
