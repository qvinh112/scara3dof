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
  stepperY.setSpeed(-400);
  while (digitalRead(Y_LIMIT) == HIGH) {
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
  stepperZ.setSpeed(-1000);
  while (digitalRead(Z_LIMIT) == HIGH) {
    stepperZ.runSpeed();
  }
  stepperZ.setCurrentPosition(0);
}

void moveXYZ(long x, long y, long z) {
  stepperX.moveTo(x);
  stepperY.moveTo(y);
  stepperZ.moveTo(z / 2);
  
  while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0 || stepperZ.distanceToGo() != 0) {
    stepperX.run();
    stepperY.run();
    stepperZ.run();
    delay(1);
  }
}

void grab() {
  digitalWrite(RELAY_PIN, HIGH);
  delay(500);
}

void release() {
  digitalWrite(RELAY_PIN, LOW);
  delay(500);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(Y_LIMIT, INPUT_PULLUP);
  pinMode(X_LIMIT, INPUT_PULLUP);
  pinMode(Z_LIMIT, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  stepperY.setMaxSpeed(800);
  stepperY.setAcceleration(400);
  stepperX.setMaxSpeed(2000);
  stepperX.setAcceleration(1000);
  stepperZ.setMaxSpeed(2000);
  stepperZ.setAcceleration(1000);
  
  Serial.println("Robot Control System (Nam châm điện)");
  Serial.println("-------------------------------------");
  Serial.println("Các lệnh có sẵn:");
  Serial.println("HOME - Về vị trí gốc");
  Serial.println("MOVE X Y Z - Di chuyển đến tọa độ");
  Serial.println("GRAB - Bật nam châm");
  Serial.println("RELEASE - Tắt nam châm");
  Serial.println("HOP1 - Chu trình gắp & thả vật tự động");
  Serial.println("-------------------------------------");
  Serial.println("Homing...");
  homeAxes();
  delay(500);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    Serial.print("Nhận lệnh: ");
    Serial.println(cmd);
    
    if (cmd.startsWith("MOVE ")) {
      String params = cmd.substring(5);
      params.trim();
      
      int firstSpace = params.indexOf(' ');
      int secondSpace = params.indexOf(' ', firstSpace + 1);
      
      if (firstSpace > 0 && secondSpace > 0) {
        long x = params.substring(0, firstSpace).toInt();
        long y = params.substring(firstSpace + 1, secondSpace).toInt();
        long z = params.substring(secondSpace + 1).toInt();
        
        Serial.print("Di chuyển đến (");
        Serial.print(x); Serial.print(", ");
        Serial.print(y); Serial.print(", ");
        Serial.print(z); Serial.println(")");
        
        moveXYZ(x, y, z);
      } else {
        Serial.println("Định dạng sai. Sử dụng: MOVE X Y Z");
      }
    }
    else if (cmd == "HOP1") {
      Serial.println("Thực hiện chu trình gắp và thả vật - HOP1");
      moveXYZ(0, 2900, 14000);
      delay(500);
      moveXYZ(0, 2900, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(0, 2900, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP2") {
      moveXYZ(51, 3257, 14000);
      delay(500);
      moveXYZ(51, 3257, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(51, 3257, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP3") {
      moveXYZ(4015, 3455, 14000);
      delay(500);
      moveXYZ(4015, 3455, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(4015, 3455, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP4") {
      moveXYZ(3050, 2635, 14000);
      delay(500);
      moveXYZ(3050, 2635, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(3050, 2635, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP5") {
      moveXYZ(3652, 2913, 14000);
      delay(500);
      moveXYZ(3652, 2913, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(3652, 2913, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP6") {
      moveXYZ(6227, 3070, 14000);
      delay(500);
      moveXYZ(6227, 3070, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(6227, 3070, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP7") {
      moveXYZ(7981, 2015, 14000);
      delay(500);
      moveXYZ(7981, 2015, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(7981, 2015, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP8") {
      moveXYZ(7739, 2383, 14000);
      delay(500);
      moveXYZ(7739, 2383, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(7739, 2383, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOP9") {
      moveXYZ(9329, 2551, 14000);
      delay(500);
      moveXYZ(9329, 2551, 0);
      delay(500);
      grab();
      delay(1000);
      moveXYZ(9329, 2551, 14000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "CMD1") {
      moveXYZ(16000, 1662, 35000);
      delay(500);
      moveXYZ(22741, 2729, 35000);
      release();
      delay(1000);
      moveXYZ(16000, 1662, 35000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "CMD2") {
      moveXYZ(16000, 1662, 35000);
      delay(500);
      moveXYZ(24000, 1800, 35000);
      release();
      delay(1000);
      moveXYZ(16000, 1662, 35000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "CMD3") {
      moveXYZ(16000, 1662, 1000);
      delay(500);
      moveXYZ(22741, 2729, 1000);
      release();
      delay(1000);
      moveXYZ(16000, 1662, 1000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "CMD4") {
      moveXYZ(16000, 1662, 1000);
      delay(500);
      moveXYZ(24000, 1800, 1000);
      release();
      delay(1000);
      moveXYZ(16000, 1662, 1000);
      delay(500);
      moveXYZ(16000, 1662, 14000);
      delay(500);
    }
    else if (cmd == "HOME") {
      Serial.println("Về vị trí gốc");
      homeAxes();
    }
    else if (cmd == "GRAB") {
      grab();
    }
    else if (cmd == "RELEASE") {
      release();
    }
    else {
      Serial.println("Lệnh không hợp lệ. Vui lòng thử lại.");
    }
  }

  stepperX.run();
  stepperY.run();
  stepperZ.run();
}
