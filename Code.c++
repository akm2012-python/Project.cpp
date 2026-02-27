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
// Logistic Model: Risk = 1 / (1 + e^-(aD + bC + c))
float a = 4.0;   // Deflection effect
float b = 5.0;   // Crack effect
float c = -6.0;  // Bias

// ---------------- STRUCTURAL SETTINGS ----------------
float baselineDistance = 15.0;   // Initial distance (cm)
float fatigueIndex = 0;
float totalDesignYears = 50;

// ---------------- FUNCTION TO GET STABLE DISTANCE ----------------
float getDistance() {

  long total = 0;

  for(int i=0; i<5; i++) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
    float distance = duration * 0.034 / 2;
    total += distance;
    delay(10);
  }

  return total / 5.0;
}

void setup() {

  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(CRACK_PIN, INPUT_PULLUP);

  pinMode(WHITE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println("===============================================");
  Serial.println("        SMARTBRIDGE AI SYSTEM STARTED");
  Serial.println("===============================================");
  delay(2000);
}

void loop() {

  // ---------------- SENSOR READINGS ----------------
  float currentDistance = getDistance();
  float deflection = baselineDistance - currentDistance;
  if(deflection < 0) deflection = 0;

  int crack = digitalRead(CRACK_PIN) == LOW ? 1 : 0;

  // ---------------- FATIGUE MODEL ----------------
  if(deflection > 0.2)
    fatigueIndex += deflection * 1.5;

  if(deflection < 0.1)
    fatigueIndex -= 0.2;

  if(fatigueIndex < 0) fatigueIndex = 0;
  if(fatigueIndex > 100) fatigueIndex = 100;

  float remainingLife = 100 - fatigueIndex;
  float remainingYears = (remainingLife / 100.0) * totalDesignYears;

  // ---------------- AI LOGISTIC MODEL ----------------
  float z = (a * deflection) + (b * crack) + c;
  float risk = 1.0 / (1.0 + exp(-z));
  float riskPercent = risk * 100;

  // ---------------- HEALTH GRADE ----------------
  String grade;
  if(remainingLife > 75) grade = "A (Excellent)";
  else if(remainingLife > 50) grade = "B (Good)";
  else if(remainingLife > 25) grade = "C (Weak)";
  else grade = "D (Critical)";

  // ---------------- DECISION SYSTEM ----------------
  String status;

  if(crack == 1 || riskPercent > 70 || remainingLife < 20) {
    redMode();
    status = "DANGER";
  }
  else if(riskPercent > 40) {
    yellowMode();
    status = "WARNING";
  }
  else {
    whiteMode();
    status = "SAFE";
  }

  // ---------------- PROFESSIONAL SERIAL REPORT ----------------
  Serial.println("=================================================");
  Serial.println("              SMARTBRIDGE AI REPORT");
  Serial.println("=================================================");

  Serial.print("Baseline Distance: ");
  Serial.print(baselineDistance);
  Serial.println(" cm");

  Serial.print("Current Distance: ");
  Serial.print(currentDistance);
  Serial.println(" cm");

  Serial.print("Bridge Deflection: ");
  Serial.print(deflection);
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

  Serial.print("Estimated Remaining Service Years: ");
  Serial.print(remainingYears);
  Serial.println(" years");

  Serial.print("Structural Health Grade: ");
  Serial.println(grade);

  Serial.print("Traffic Signal Status: ");
  Serial.println(status);

  Serial.println("=================================================\n");

  delay(1000);
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
