// =======================================================
// AI-Based Structural Collapse Risk Prediction System
// Logistic Regression Model (Pre-trained Weights)
// With Moving Average + Time-Based Risk Accumulation
// Non-Blocking Timing using millis()
// =======================================================

// ---------------- PIN DEFINITIONS ----------------
const int vibPin = A0;
const int loadPin = A1;

const int greenLED = 8;
const int yellowLED = 9;
const int redLED = 10;
const int buzzer = 11;

// ---------------- MODEL WEIGHTS ----------------
float w_vib = 5.4;
float w_load = 4.8;
float bias = -4.2;

// ---------------- FILTER SETTINGS ----------------
const int numSamples = 10;
float vibSamples[numSamples];
float loadSamples[numSamples];
int sampleIndex = 0;

// ---------------- TIME SETTINGS ----------------
const unsigned long readInterval = 200;     // 200ms loop
const unsigned long riskTimeLimit = 5000;   // 5 seconds

unsigned long previousMillis = 0;
unsigned long highRiskStartTime = 0;

bool highRiskActive = false;

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(9600);

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < numSamples; i++) {
    vibSamples[i] = 0;
    loadSamples[i] = 0;
  }

  Serial.println("AI Structural Collapse Prediction System Started");
}

// ---------------- SIGMOID FUNCTION ----------------
float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

// ---------------- MOVING AVERAGE ----------------
float movingAverage(float arr[]) {
  float sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += arr[i];
  }
  return sum / numSamples;
}

// ---------------- LOOP ----------------
void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= readInterval) {

    previousMillis = currentMillis;

    // -------- SENSOR READING --------
    int vibRaw = analogRead(vibPin);
    int loadRaw = analogRead(loadPin);

    float vibNorm = vibRaw / 1023.0;
    float loadNorm = loadRaw / 1023.0;

    // Store samples
    vibSamples[sampleIndex] = vibNorm;
    loadSamples[sampleIndex] = loadNorm;

    sampleIndex++;
    if (sampleIndex >= numSamples) {
      sampleIndex = 0;
    }

    // Smoothed values
    float V = movingAverage(vibSamples);
    float L = movingAverage(loadSamples);

    // Logistic Regression
    float z = (w_vib * V) + (w_load * L) + bias;
    float risk = sigmoid(z);

    // -------- SERIAL OUTPUT --------
    Serial.print("Vibration: ");
    Serial.print(V, 3);
    Serial.print(" | Load: ");
    Serial.print(L, 3);
    Serial.print(" | Collapse Probability: ");
    Serial.println(risk, 3);

    // -------- ALERT LOGIC --------

    if (risk < 0.4) {

      highRiskActive = false;
      highRiskStartTime = 0;

      digitalWrite(greenLED, HIGH);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      digitalWrite(buzzer, LOW);
    }

    else if (risk >= 0.4 && risk < 0.7) {

      highRiskActive = false;
      highRiskStartTime = 0;

      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, HIGH);
      digitalWrite(redLED, LOW);
      digitalWrite(buzzer, LOW);
    }

    else {  // risk >= 0.7

      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, LOW);

      if (!highRiskActive) {
        highRiskActive = true;
        highRiskStartTime = currentMillis;
      }

      if (currentMillis - highRiskStartTime >= riskTimeLimit) {
        digitalWrite(redLED, HIGH);
        digitalWrite(buzzer, HIGH);
      }
    }
  }
}
