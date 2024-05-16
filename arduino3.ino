
// ARDUINO 3: ULTRA SONIC SENSOR

// Define constants for ultrasonic sensor pins
const int TRIGGER_PIN = 8;
const int ECHO_PIN = 9;

// Define constants for RGB LED pins
const int RED_LED_PIN = 13;
const int GREEN_LED_PIN = 12;
const int BLUE_LED_PIN = 11;

// Define the speed of sound in cm/us
const float SPEED_OF_SOUND = 0.0343;

// Define thresholds for the food levels in cm
const int FULL_THRESHOLD = 8; // arbitrary number
const int HALF_THRESHOLD = 12; // arbitrary number
const int LOW_THRESHOLD = 20; // arbitrary number


void setup() {
  // Initialize ultrasonic sensor pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize RGB LED pins
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Send ultrasonic pulse
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  // Read the echo time (round trip)
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance based on the speed of sound
  float distance = duration * SPEED_OF_SOUND / 2;
  Serial.println(distance);

  // Determine the food level and set the LED color
  if (distance <= FULL_THRESHOLD) {
    // Green LED: Container is full
    setColor(0, 255, 0); // Green
    Serial.println("GREEN INDICATED");

  } else if (distance > FULL_THRESHOLD && distance <= HALF_THRESHOLD) {
    // Yellow LED: Container is at half capacity
    setColor(255, 0, 255); // Purple
    Serial.println("PURPLE INDICATED");

  } else if (distance > HALF_THRESHOLD && distance <= LOW_THRESHOLD) {
    // Red LED: Container is low
    setColor(255, 0, 0); // Red
    Serial.println("RED INDICATED");
    sendMsg();

  } else {
    // No Light: Container is empty or sensor out of range
    setColor(0, 0, 0); // Off
    sendMsg();
  }

  // Delay for a short period before measuring again
  delay(500);
}

void setColor(int R, int G, int B) {
  analogWrite(RED_LED_PIN,   R);
  analogWrite(GREEN_LED_PIN, G);
  analogWrite(BLUE_LED_PIN, B);
}  

void sendMsg(){
  Serial.write('T');
}

