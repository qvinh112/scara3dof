#include <AccelStepper.h>

// Chân stepper
#define STEP_Y 4  
#define DIR_Y 3   
#define STEP_X 7  
#define DIR_X 6    
#define STEP_Z 10
#define DIR_Z 9

// Công tắc hành trình gốc
#define Y_LIMIT 31  
#define X_LIMIT 30
#define Z_LIMIT 32
#define RELAY_PIN 12

AccelStepper stepperY(AccelStepper::DRIVER, STEP_Y, DIR_Y); 
AccelStepper stepperX(AccelStepper::DRIVER, STEP_X, DIR_X);  
AccelStepper stepperZ(AccelStepper::DRIVER, STEP_Z, DIR_Z);

void homeAxes() {
  // Home Y
  stepperY.setSpeed(-200);
  while (digitalRead(Y_LIMIT) == HIGH ) {
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);
  
  // Home X
  stepperX.setSpeed(-1000);
  while (digitalRead(X_LIMIT) == HIGH) {
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);
  
  // Home Z
  stepperZ.setSpeed(-800);
  while (digitalRead(Z_LIMIT) == HIGH) {
    stepperZ.runSpeed();
  }
  stepperZ.setCurrentPosition(0);
}

void moveXYZ(long x, long y, long z, int relayState = -1) {
  stepperX.moveTo(x);
  stepperY.moveTo(y);
  stepperZ.moveTo(z);
  
  while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0 || stepperZ.distanceToGo() != 0) {
    stepperX.run();
    stepperY.run();
    stepperZ.run();
    delay(1);
  }

  if (relayState == 1) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Nam châm điện đã bật (trong moveXYZ)");
  } else if (relayState == 0) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Nam châm điện đã tắt (trong moveXYZ)");
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(Y_LIMIT, INPUT_PULLUP);
  pinMode(X_LIMIT, INPUT_PULLUP);
  pinMode(Z_LIMIT, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  
  stepperY.setMaxSpeed(200);
  stepperY.setAcceleration(200);
  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(1000);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(1000);
  
  Serial.println("Robot Control System (Nam châm điện)");
  Serial.println("-------------------------------------");
  Serial.println("Các lệnh có sẵn:");
  Serial.println("HOME - Về vị trí gốc");
  Serial.println("MOVE");
  Serial.println("HOP1 - Gắp vật tại vị trí định sẵn");
  Serial.println("-------------------------------------");
  Serial.println("Homing...");
  homeAxes();
  delay(500);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    Serial.print("Nhận lệnh: ");
    Serial.println(cmd);
    if (cmd.startsWith("move ")) {
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
        
        moveXYZ(x, y, z);
      }
    }
    else if (cmd == "hop1") {
      moveXYZ(93, 1340, 1000, 0);// Hạ xuống gần vật
      delays(500);
      moveXYZ(93, 1340, 300, 1);  // Bật nam châm điện
      delays(500);
      moveXYZ(93, 1340, 5000, 1); // Nâng vật lên
      delays(500);
      moveXYZ(16000, 831, 5000, 1);
      delays(500);
      moveXYZ(16000, 831, 1000, 0);
      delays(500);
    }
    else if (cmd == "home") {
      Serial.println("Về vị trí gốc");
      homeAxes();
    }
    else {
      Serial.println("Lệnh không hợp lệ. Vui lòng thử lại.");
    }
  }
  stepperX.run();
  stepperY.run();
  stepperZ.run();
}
