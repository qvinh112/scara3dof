#include <AccelStepper.h>
#include <Servo.h>

// Stepper X
#define STEP_X 3
#define DIR_X 4

// Stepper Y
#define STEP_Y 6
#define DIR_Y 7

// Stepper Z
#define STEP_Z 9
#define DIR_Z 10

// Limit switches
#define X_LIMIT 31
#define Y_LIMIT 30
#define Z_LIMIT 32

// Servo gripper
#define GRIPPER_PIN 8
Servo gripper;

AccelStepper stepperX(AccelStepper::DRIVER, STEP_X, DIR_X);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_Y, DIR_Y);
AccelStepper stepperZ(AccelStepper::DRIVER, STEP_Z, DIR_Z);

void homeAxis(AccelStepper &stepper, int limitPin, bool reverse = false) {
  stepper.setMaxSpeed(400);
  stepper.setSpeed(reverse ?  -200 : 200);
  while (digitalRead(limitPin) == HIGH) {
    stepper.runSpeed();
  }
  stepper.setCurrentPosition(0);
}

void homeAll() {
  homeAxis(stepperX, X_LIMIT, true);
  homeAxis(stepperY, Y_LIMIT, true);
  homeAxis(stepperZ, Z_LIMIT, true);
}

void moveToPosition(long x, long y, long z) {
  stepperX.moveTo(x);
  stepperY.moveTo(y);
  stepperZ.moveTo(z);
  while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0 || stepperZ.distanceToGo() != 0) {
    stepperX.run();
    stepperY.run();
    stepperZ.run();
  }
}

void grab() {
  gripper.write(30); // đóng gripper
  delay(500);
}

void release() {
  gripper.write(90); // mở gripper
  delay(500);
}

void setup() {
  Serial.begin(9600);

  pinMode(X_LIMIT, INPUT_PULLUP);
  pinMode(Y_LIMIT, INPUT_PULLUP);
  pinMode(Z_LIMIT, INPUT_PULLUP);

  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(300);
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(300);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(300);

  gripper.attach(GRIPPER_PIN);
  release(); // mở kẹp ban đầu

  homeAll();
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "CMD1") {
      homeAll();
      moveToPosition(1000, 500, 200); // vị trí lấy vật
      grab();
      moveToPosition(0, 0, 500); // vị trí thả
      release();
    }
    else if (cmd == "CMD2") {
      homeAll();
      moveToPosition(800, 600, 200);
      grab();
      moveToPosition(0, 300, 500);
      release();
    }
    else if (cmd == "CMD3") {
      homeAll();
      moveToPosition(600, 700, 200);
      grab();
      moveToPosition(300, 0, 500);
      release();
    }
  }
}
