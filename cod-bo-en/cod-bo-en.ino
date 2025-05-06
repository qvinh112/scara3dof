#include <AccelStepper.h>

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

// Chân relay điều khiển nam châm điện
#define RELAY_PIN 12

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
  stepperY.setSpeed(-800);
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
  digitalWrite(RELAY_PIN, HIGH);  // Bật nam châm điện
  delay(500);
}

void release() {
  digitalWrite(RELAY_PIN, LOW);  // Tắt nam châm điện
  delay(500);
}

void setup() {
  Serial.begin(9600);

  pinMode(X_LIMIT, INPUT_PULLUP);
  pinMode(Y_LIMIT, INPUT_PULLUP);
  pinMode(Z_LIMIT, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);

  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(1000);
  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(1000);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(1000);

  Serial.println("Robot Control System (Nam châm điện)");
  Serial.println("-------------------------------------");
  Serial.println("Các lệnh có sẵn:");
  Serial.println("HOME - Về vị trí gốc");
  Serial.println("X123 - Di chuyển trục X đến vị trí 123 xung");
  Serial.println("Y123 - Di chuyển trục Y đến vị trí 123 xung");
  Serial.println("Z123 - Di chuyển trục Z đến vị trí 123 xung");
  Serial.println("MOVE X Y Z - Di chuyển đến vị trí (X, Y, Z)");
  Serial.println("GRAB - Bật nam châm");
  Serial.println("RELEASE - Tắt nam châm");
  Serial.println("-------------------------------------");

  Serial.println("Homing...");
  homeAxes();  // Thực hiện homing 3 trục
  Serial.println("Homing xong. Đang di chuyển đến vị trí làm việc...");
  moveTo(16000, 813, 1000);  // Di chuyển đến vị trí cố định
  Serial.println("Sẵn sàng.");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    Serial.print("Nhận lệnh: ");
    Serial.println(cmd);

    if (cmd.startsWith("X")) {
      long pos = cmd.substring(1).toInt();
      Serial.print("Di chuyển X đến: ");
      Serial.println(pos);
      stepperX.moveTo(pos);
    }
    else if (cmd.startsWith("Y")) {
      long pos = cmd.substring(1).toInt();
      Serial.print("Di chuyển Y đến: ");
      Serial.println(pos);
      stepperY.moveTo(pos);
    }
    else if (cmd.startsWith("Z")) {
      long pos = cmd.substring(1).toInt();
      Serial.print("Di chuyển Z đến: ");
      Serial.println(pos);
      stepperZ.moveTo(pos);
    }
    else if (cmd.startsWith("MOVE ")) {
      cmd.replace("MOVE ", "");
      int firstSpace = cmd.indexOf(' ');
      int secondSpace = cmd.indexOf(' ', firstSpace + 1);

      if (firstSpace > 0 && secondSpace > 0) {
        long x = cmd.substring(0, firstSpace).toInt();
        long y = cmd.substring(firstSpace + 1, secondSpace).toInt();
        long z = cmd.substring(secondSpace + 1).toInt();

        Serial.print("Di chuyển đến (");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.print(", ");
        Serial.print(z);
        Serial.println(")");

        moveTo(x, y, z);
      }
    }
    else if (cmd == "GRAB") {
      Serial.println("Bật nam châm (gắp vật)");
      grab();
    }
    else if (cmd == "RELEASE") {
      Serial.println("Tắt nam châm (nhả vật)");
      release();
    }
    else if (cmd == "HOME") {
      Serial.println("Về vị trí gốc");
      homeaxes();
    else if (cmd == "RESTART") {
      Serial.println("Về vị trí gốc");
      homeaxes();
      moveTo(16000, 813, 1000);

    else {
      Serial.println("Lệnh không hợp lệ. Vui lòng thử lại.");
    }
  }

  stepperX.run();
  stepperY.run();
  stepperZ.run();
}
