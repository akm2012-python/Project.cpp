#include <math.h>

// ---------------- PIN DEFINITIONS ----------------
#define TRIG 6
#define ECHO 7
#define CRACK_PIN 4

#define WHITE_LED 8
#define YELLOW_LED 9
#define RED_LED 10
#define BUZZER 11

// ---------------- AI MODEL COEFFICIENTS ----------------
// Tuned for:
// Yellow (40%) at 0.3 cm
// Red (70%) at 0.5 cm
float a = 6.26;
float b = 5.0;      // Strong crack influence
float c = -2.28;

// ---------------- STRUCTURAL SETTINGS ----------------
float baselineDistance = 0;
float fatigueIndex = 0;
float totalDesignYears = 50;

// ---------------- STABILITY SETTINGS ----------------
float filteredDeflection = 0;
float alpha = 0.25;   // 0.1 = very stable, 0.5 = fast response
String currentState = "SAFE";

// ---------------- FUNCTION TO GET STABLE DISTANCE ----------------
float getDistance() {

  float total = 0;
  int validReadings = 0;

  for(int i = 0; i < 5; i++) {

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH, 30000); 
    float distance = duration * 0.034 / 2;

    if(distance > 2 && distance < 400) {
      total += distance;
      validReadings++;
    }

    delay(10);
  }

  if(validReadings == 0) return baselineDistance;
  return total / validReadings;
}

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(CRACK_PIN, INPUT_PULLUP);

  pinMode(WHITE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println("===========================================");
  Serial.println("      SMARTBRIDGE AI SYSTEM STARTED");
  Serial.println("      Calibrating Baseline...");
  Serial.println("===========================================");

  delay(2000);

  baselineDistance = getDistance();

  Serial.print("Calibrated Baseline Distance: ");
  Serial.print(baselineDistance);
  Serial.println(" cm");
  Serial.println("System Ready.\n");

  delay(2000);
}

// ---------------- MAIN LOOP ----------------
void loop() {

  float currentDistance = getDistance();
  float deflection = baselineDistance - currentDistance;
  if(deflection < 0) deflection = 0;

  // -------- SMOOTHING --------
  filteredDeflection = (alpha * deflection) + 
                       ((1 - alpha) * filteredDeflection);

  int crack = digitalRead(CRACK_PIN) == LOW ? 1 : 0;

  // -------- FATIGUE MODEL --------
  if(filteredDeflection > 0.2) {
    fatigueIndex += (filteredDeflection * filteredDeflection) * 0.8;
  }
  else if(filteredDeflection < 0.1) {
    fatigueIndex -= 0.3;
  }

  if(fatigueIndex < 0) fatigueIndex = 0;
  if(fatigueIndex > 100) fatigueIndex = 100;

  float remainingLife = 100 - fatigueIndex;
  float remainingYears = (remainingLife / 100.0) * totalDesignYears;

  // -------- LOGISTIC AI MODEL --------
  float z = (a * filteredDeflection) + (b * crack) + c;
  float risk = 1.0 / (1.0 + exp(-z));
  float riskPercent = risk * 100;

  // -------- HYSTERESIS STATE CONTROL --------
  if(currentState == "SAFE") {
      if(riskPercent > 40) currentState = "WARNING";
  }
  else if(currentState == "WARNING") {
      if(riskPercent > 70 || crack == 1)
          currentState = "DANGER";
      else if(riskPercent < 35)
          currentState = "SAFE";
  }
  else if(currentState == "DANGER") {
      if(riskPercent < 65 && crack == 0)
          currentState = "WARNING";
  }

  // -------- OUTPUT CONTROL --------
  if(currentState == "SAFE") {
      whiteMode();
  }
  else if(currentState == "WARNING") {
      yellowMode();
  }
  else {
      redMode();
  }

  // -------- PROFESSIONAL REPORT --------
  Serial.println("===========================================");
  Serial.println("           SMARTBRIDGE AI REPORT");
  Serial.println("===========================================");

  Serial.print("Baseline Distance: ");
  Serial.print(baselineDistance);
  Serial.println(" cm");

  Serial.print("Filtered Deflection: ");
  Serial.print(filteredDeflection);
  Serial.println(" cm");

  Serial.print("Crack Status: ");
  Serial.println(crack ? "CRACK DETECTED" : "NO CRACK");

  Serial.print("Collapse Probability: ");
  Serial.print(riskPercent);
  Serial.println(" %");

  Serial.print("Fatigue Index: ");
  Serial.print(fatigueIndex);
  Serial.println(" %");

  Serial.print("Remaining Structural Life: ");
  Serial.print(remainingLife);
  Serial.println(" %");

  Serial.print("Estimated Remaining Years: ");
  Serial.print(remainingYears);
  Serial.println(" years");

  Serial.print("System Status: ");
  Serial.println(currentState);

  Serial.println("===========================================\n");

  delay(500);
}

// ---------------- LED + BUZZER MODES ----------------

void whiteMode() {
  digitalWrite(WHITE_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  noTone(BUZZER);
}

void yellowMode() {
  digitalWrite(WHITE_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  tone(BUZZER, 1000);
}

void redMode() {
  digitalWrite(WHITE_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 2000);
}
