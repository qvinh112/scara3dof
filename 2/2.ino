#include <AccelStepper.h>
#include <Servo.h>

// Chân stepper
#define STEP_X 4
#define DIR_X 3
#define STEP_Y 7
#define DIR_Y 6
#define STEP_Z 10
#define DIR_Z 9

// Công tắc hành trình gốc
#define X_LIMIT 31
#define Y_LIMIT 30
#define Z_LIMIT 32

// Servo gắp
#define SERVO_PIN 8
Servo gripper;

AccelStepper stepperX(AccelStepper::DRIVER, STEP_X, DIR_X);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_Y, DIR_Y);
AccelStepper stepperZ(AccelStepper::DRIVER, STEP_Z, DIR_Z);

void homeAxes() {
  unsigned long timeout;

  // Home X
  stepperX.setSpeed(-200);
  timeout = millis();
  while (digitalRead(X_LIMIT) == HIGH && millis() - timeout < 5000) {
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);

  // Home Y
  stepperY.setSpeed(-200);
  timeout = millis();
  while (digitalRead(Y_LIMIT) == HIGH && millis() - timeout < 5000) {
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);

  // Home Z
  stepperZ.setSpeed(-200);
  timeout = millis();
  while (digitalRead(Z_LIMIT) == HIGH && millis() - timeout < 5000) {
    stepperZ.runSpeed();
  }
  stepperZ.setCurrentPosition(0);
}

void moveTo(long x, long y, long z) {
  stepperX.moveTo(x);
  stepperY.moveTo(y);
  stepperZ.moveTo(z);
  while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0 || stepperZ.distanceToGo() != 0) {
    stepperX.run();
    stepperY.run();
    stepperZ.run();
    delay(1);
  }
}

void grab() {
  gripper.write(30); // Gắp
  delay(500);
}

void release() {
  gripper.write(90); // Nhả
  delay(500);
}

void setup() {
  Serial.begin(9600);

  pinMode(X_LIMIT, INPUT_PULLUP);
  pinMode(Y_LIMIT, INPUT_PULLUP);
  pinMode(Z_LIMIT, INPUT_PULLUP);

  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(500);
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(500);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(500);

  gripper.attach(SERVO_PIN);

  Serial.println("Homing...");
  homeAxes();
  Serial.println("Ready.");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    Serial.print("Received: "); Serial.println(cmd);

    if (cmd == "CMD1") {
      homeAxes();
      moveTo(1000, 500, 200);
      grab();
      moveTo(0, 0, 200);
      release();
    } 
    else if (cmd == "CMD2") {
      homeAxes();
      moveTo(800, 800, 150);
      grab();
      moveTo(0, 200, 150);
      release();
    } 
    else if (cmd == "CMD3") {
      homeAxes();
      moveTo(600, 400, 100);
      grab();
      moveTo(200, 0, 100);
      release();
    }
  }

  // Cập nhật động cơ liên tục
  stepperX.run();
  stepperY.run();
  stepperZ.run();
}
