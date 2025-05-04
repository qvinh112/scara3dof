#define DIR1 9            // Direction
#define STEP1 10          // Step
#define LIMIT_SWITCH_PIN 32 // Công tắc hành trình nối xuống GND khi nhấn

// Thông số động cơ
const int STEPS_PER_REVOLUTION = 50;
const int REVOLUTIONS = 100;
const int STEP_DELAY = 1000;
const int PAUSE_DELAY = 1000;

const int STEPS_FOR_REVOLUTIONS = REVOLUTIONS * STEPS_PER_REVOLUTION;

void setup() {
  pinMode(DIR1, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); // Sử dụng trở kéo lên
}

void loop() {
  // Nếu công tắc đã nhấn => Dừng toàn bộ, không làm gì
  if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
    return; // hoặc dùng while(1); để đứng luôn
  }

  // Quay theo chiều thuận (CW)
  digitalWrite(DIR1, HIGH);
  for (int i = 0; i < STEPS_FOR_REVOLUTIONS; i++) {
    if (digitalRead(LIMIT_SWITCH_PIN) == LOW) return; // Dừng nếu nhấn
    digitalWrite(STEP1, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP1, LOW);
    delayMicroseconds(STEP_DELAY);
  }

  delay(PAUSE_DELAY);

  // Quay ngược lại (CCW)
  digitalWrite(DIR1, LOW);
  for (int i = 0; i < STEPS_FOR_REVOLUTIONS; i++) {
    if (digitalRead(LIMIT_SWITCH_PIN) == LOW) return;
    digitalWrite(STEP1, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP1, LOW);
    delayMicroseconds(STEP_DELAY);
  }

  delay(PAUSE_DELAY);
}
