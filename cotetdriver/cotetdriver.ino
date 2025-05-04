// Định nghĩa các chân
#define DIR1 6  // Direction
#define STEP1 7 // Step

// Thông số động cơ
const int STEPS_PER_REVOLUTION = 200; // Số bước cho 1 vòng (tùy driver và động cơ)
const int REVOLUTIONS =100;           // Số vòng quay (4 vòng)
const int STEP_DELAY = 1000;         // Độ trễ giữa các bước (micro giây)
const int PAUSE_DELAY = 1000;        // Thời gian dừng giữa các lần quay (ms)

// Tính số bước cho 4 vòng
const int STEPS_FOR_REVOLUTIONS = REVOLUTIONS * STEPS_PER_REVOLUTION;

void setup() {
  // Cấu hình các chân là OUTPUT
  pinMode(DIR1, OUTPUT);
  pinMode(STEP1, OUTPUT);
}

void loop() {
  // Quay theo hướng thuận (CW)
  digitalWrite(DIR1, HIGH); // Đặt hướng
  for (int i = 0; i < STEPS_FOR_REVOLUTIONS; i++) {
    digitalWrite(STEP1, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP1, LOW);
    delayMicroseconds(STEP_DELAY);
  }

  // Dừng một chút
  delay(PAUSE_DELAY);

  // Quay ngược lại (CCW)
  digitalWrite(DIR1, LOW); // Đổi hướng
  for (int i = 0; i < STEPS_FOR_REVOLUTIONS; i++) {
    digitalWrite(STEP1, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP1, LOW);
    delayMicroseconds(STEP_DELAY);
  }

  // Dừng trước khi lặp lại
  delay(PAUSE_DELAY);
}
