#include <AccelStepper.h>

// Chân stepper
#define STEP_Y 4   // Trước đây là STEP_X
#define DIR_Y 3    // Trước đây là DIR_X
#define STEP_X 7   // Trước đây là STEP_Y
#define DIR_X 6    // Trước đây là DIR_Y
#define STEP_Z 10
#define DIR_Z 9

// Công tắc hành trình gốc
#define Y_LIMIT 31  // Trước đây là X_LIMIT
#define X_LIMIT 30  // Trước đây là Y_LIMIT
#define Z_LIMIT 32
#define RELAY_PIN 12

AccelStepper stepperY(AccelStepper::DRIVER, STEP_Y, DIR_Y);  // Trước đây là stepperX
AccelStepper stepperX(AccelStepper::DRIVER, STEP_X, DIR_X);  // Trước đây là stepperY
AccelStepper stepperZ(AccelStepper::DRIVER, STEP_Z, DIR_Z);

void homeAxes() {
  
  // Home Y
  stepperY.setSpeed(-200);
  while (digitalRead(Y_LIMIT) == HIGH && millis()) {
    stepperY.runSpeed();
  }
  stepperY.setCurrentPosition(0);
  
  // Home X
  stepperX.setSpeed(-1000);
  while (digitalRead(X_LIMIT) == HIGH && millis()) {
    stepperX.runSpeed();
  }
  stepperX.setCurrentPosition(0);
  
  // Home Z
  stepperZ.setSpeed(-800);
  while (digitalRead(Z_LIMIT) == HIGH && millis()) {
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
  delay(500);  // Thời gian bật nam châm điện
  Serial.println("Nam châm điện đã bật");
}

void release() {
  digitalWrite(RELAY_PIN, LOW);  // Tắt nam châm điện
  delay(500);  // Thời gian tắt nam châm điện
  Serial.println("Nam châm điện đã tắt");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(Y_LIMIT, INPUT_PULLUP);  // Đã đổi X_LIMIT thành Y_LIMIT
  pinMode(X_LIMIT, INPUT_PULLUP);  // Đã đổi Y_LIMIT thành X_LIMIT
  pinMode(Z_LIMIT, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);  // Cấu hình chân relay
  
  stepperY.setMaxSpeed(200);  // stepperY
  stepperY.setAcceleration(200);  //stepperY
  stepperX.setMaxSpeed(1000);  // stepperX
  stepperX.setAcceleration(1000);  //stepperX
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
  Serial.println("RESTART - Về vị trí gốc và di chuyển đến vị trí làm việc");
  Serial.println("TEST - Kiểm tra hoạt động nam châm điện");  // Thêm lệnh kiểm tra
  Serial.println("-------------------------------------");
  
  Serial.println("Homing...");
  homeAxes();  // Thực hiện homing 3 trục
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
      stepperX.moveTo(pos);  // Đã đổi thành stepperX (khớp với tên trục mới)
    }
    else if (cmd.startsWith("Y")) {
      long pos = cmd.substring(1).toInt();
      Serial.print("Di chuyển Y đến: ");
      Serial.println(pos);
      stepperY.moveTo(pos);  // Đã đổi thành stepperY (khớp với tên trục mới)
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
else if (cmd == "HOP1") {
  moveTo(93, 1340, 300);   // Hạ xuống gần vật
  grab();                  // Bật nam châm
  moveTo(93, 1340, 10000); // Nâng vật lên
}


    else if (cmd == "grab") {  
      Serial.println("Bật nam châm (gắp vật)");
      grab();
    }
    else if (cmd == "rel") {  
      Serial.println("Tắt nam châm (nhả vật)");
      release();
    }
    else if (cmd == "home") {
      Serial.println("Về vị trí gốc");
      homeAxes();
    }
    else if (cmd == "restart") {
      Serial.println("Về vị trí gốc và đến vị trí làm việc");
      homeAxes();
      moveTo(16000, 813, 11000);
    }
    else if (cmd == "TEST") {
      Serial.println("Kiểm tra nam châm điện");
      grab();
      delay(2000);
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
