// Main program file for towercopter.

// Includes.
#include <Servo.h>

// Pins.
#define echoPin 2
#define trigPin 3
#define switchPin 4
#define escSignalPin 9

// ESC as servo.
Servo esc;

// Ultrasonic sensor variables.
long duration; // Duration of sound wave travel.
int distance; // Distance measurement.

const int min_distance = 2; // Min and max measurable distance limit.
const int max_distance = 100;

// Switch variables.
int switchState = 0;

// Pot variables.
int potValue = 0; // Potentiometer values ()
int potEscValue = 0;

// Motor constants.
const int MIN = 1000;
const int MAX = 2500;

// PID variables.
double proc_time = 0.6;
double error = 0;
double prev_error = 0;

double p = 0;
double i = 0;
double d = 0;

float kp = 0.4;
float ki = 0.005;
float kd = 0.03;

double integral = 0;
double derivative = 0;

int mv = 0;
double out_val;

const int setpoint = 20;

void setup() {
  // Start serial communication.
  Serial.begin(9600);

  // Define as in or output.
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(switchPin, INPUT);

  // Attach.
  esc.attach(escSignalPin, MIN, MAX);
  esc.write(0);
}

void loop() {
  switchState = digitalRead(switchPin); // Read switch state.

  // Compare switch states.
  if (switchState == HIGH) {
    pidControl(); // Auto mode (PID control).
  } else {
    potControl(); // Manual mode (potentiometer control).
  }
}

void pidControl() {
  // -- Ultrasonic sensor code. --
  digitalWrite(trigPin, LOW); // Clear trigPin.
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH); // trigPin HIGH.
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); // trigPin LOW.

  duration = pulseIn(echoPin, HIGH); // Read echoPin (sound wave travel time).
  distance = duration * 0.034 / 2; // Calculate distance.

  // Limit distance values.
  if (distance > max_distance) {
    distance = max_distance;
  } else if (distance < min_distance) {
    distance = min_distance;
  }

  // -- End ultrasonic sensor code --

  //in_val = map(distance, min_distance, max_distance, 0, 100);
  error = setpoint - distance;

  // Proportional.
  p = kp * error;

  // Integral.
  if (!(i < -20) || (i > 20)) {
    integral = error * proc_time;
    i += ki * integral;
  }

  // differential.
  derivative = (error - prev_error) / proc_time;
  d = kd * derivative;

  // Total.
  out_val = p + i + d;

  if (out_val > 100) {
    out_val = 100;
  }
  if (out_val < 0) {
    out_val = 0;
  }

  prev_error = error;

  mv = map(out_val, 0, 100, 0, 180);
  esc.write(mv);
  delay(10);


  Serial.println();
  Serial.print("Switch state: " + String(switchState) + (" (auto mode)"));
  Serial.print("---");
  Serial.print("Setpoint: " + String(setpoint));
  Serial.print("---");
  Serial.print("Distance: " + String(distance));
  Serial.print("---");
  Serial.print("Error: " + String(error));
  Serial.print("---");
  Serial.print("P: " + String(p));
  Serial.print("---");
  Serial.print("I: " + String(i));
  Serial.print("---");
  Serial.print("D: " + String(d));
  Serial.print("---");
  Serial.print("Out: " + String(out_val));
  Serial.print("---");
  Serial.print("Mv: " + String(mv));
  Serial.print("---");
}

void potControl() {
  potValue = analogRead(A0);
  potEscValue = map(potValue, 0, 1023, 0, 180); // Map from 0-1023 (pot) to 0-180 (servo).
  esc.write(potEscValue);

  Serial.println();
  Serial.print("---");
  Serial.print("Switch state: " + String(switchState) + (" (manual mode)"));
  Serial.print("---");
  Serial.print("Pot value (0-1023): " + String(potValue));
  Serial.print("---");
  Serial.print("Pot => ESC value (0-180): " + String(potEscValue));
  Serial.print("---");
}
