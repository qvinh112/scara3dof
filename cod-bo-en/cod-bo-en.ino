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

void controlRelay(bool state) {
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  Serial.print("Nam châm điện đã ");
  Serial.println(state ? "BẬT" : "TẮT");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(Y_LIMIT, INPUT_PULLUP);
  pinMode(X_LIMIT, INPUT_PULLUP);
  pinMode(Z_LIMIT, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Đảm bảo relay tắt khi khởi động
  
  stepperY.setMaxSpeed(200);
  stepperY.setAcceleration(200);
  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(500);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(500);
  
  Serial.println("Robot Control System (Nam châm điện)");
  Serial.println("-------------------------------------");
  Serial.println("Các lệnh có sẵn:");
  Serial.println("HOME - Về vị trí gốc");
  Serial.println("MOVE X Y Z - Di chuyển đến tọa độ");
  Serial.println("RELAY ON/OFF - Điều khiển nam châm điện");
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
    cmd.toUpperCase(); // Chuyển thành chữ hoa để xử lý nhất quán
    
    Serial.print("Nhận lệnh: ");
    Serial.println(cmd);
    
    if (cmd.startsWith("MOVE ")) {
      // Xử lý lệnh MOVE X Y Z
      String params = cmd.substring(5); // Bỏ "MOVE "
      params.trim();
      
      int firstSpace = params.indexOf(' ');
      int secondSpace = params.indexOf(' ', firstSpace + 1);
      
      if (firstSpace > 0 && secondSpace > 0) {
        long x = params.substring(0, firstSpace).toInt();
        long y = params.substring(firstSpace + 1, secondSpace).toInt();
        long z = params.substring(secondSpace + 1).toInt();
        
        Serial.print("Di chuyển đến (");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.print(", ");
        Serial.print(z);
        Serial.println(")");
        
        moveXYZ(x, y, z);
      } else {
        Serial.println("Định dạng sai. Sử dụng: MOVE X Y Z");
      }
    }
    else if (cmd == "HOP1") {
      moveXYZ(93, 1340, 1000, 0); // Hạ xuống gần vật
      delay(500);
      moveXYZ(93, 1340, 300, 1);  // Bật nam châm điện
      delay(500);
      moveXYZ(93, 1340, 5000, 1); // Nâng vật lên
      delay(500);
      moveXYZ(16000, 831, 5000, 1);
      delay(500);
      moveXYZ(16000, 831, 1000, 0);
      delay(500);
    }
    else if (cmd == "HOME") {
      Serial.println("Về vị trí gốc");
      homeAxes();
    }
    else if (cmd == "RELAY ON") {
      controlRelay(true);
    }
    else if (cmd == "RELAY OFF") {
      controlRelay(false);
    }
    else {
      Serial.println("Lệnh không hợp lệ. Vui lòng thử lại.");
    }
  }
  
  stepperX.run();
  stepperY.run();
  stepperZ.run();
}
