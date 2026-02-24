#include <DHT.h>
#include <math.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Analog inputs
int vibrationPin = A0;
int loadPin = A1;

// LED pins
int greenLED = 8;
int yellowLED = 9;
int redLED = 10;
int buzzer = 11;

// ======= TRAINED MODEL COEFFICIENTS =======
// Replace these with YOUR trained values if different
float w_vibration = 0.7;
float w_temperature = 0.03;
float w_load = 0.5;
float w_duration = 0.02;   // simulated internally
float bias = -8.0;
// ===========================================

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {

  // Read Sensors
  float vibration = analogRead(vibrationPin) / 100.0;   // scale down
  float load = analogRead(loadPin) / 150.0;             // scale down
  float temperature = dht.readTemperature();

  // Simulated vibration duration (seconds)
  float duration = millis() / 1000.0;

  // ===== Logistic Regression Equation =====
  float z = (w_vibration * vibration) +
            (w_temperature * temperature) +
            (w_load * load) +
            (w_duration * duration) +
            bias;

  float probability = 1.0 / (1.0 + exp(-z));

  Serial.print("Collapse Probability: ");
  Serial.println(probability);

  // Reset LEDs
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzer, LOW);

  // Risk Levels
  if (probability < 0.4) {
    digitalWrite(greenLED, HIGH);
  }
  else if (probability < 0.7) {
    digitalWrite(yellowLED, HIGH);
  }
  else {
    digitalWrite(redLED, HIGH);
    digitalWrite(buzzer, HIGH);
  }

  delay(1000);
}
