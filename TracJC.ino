/*
 * =============================================================================
 *  TracJC.ino - เดินตามเส้นและเช็คแยก (MyRP_Pico2 Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    TracJC()           - เดินตามเส้นจนเจอแยก
 *    TracJCStop()       - เดินจนเจอแยกแล้วหยุด
 *    TracJCSlow()       - เดินช้าจนเจอแยก
 *    TracJCSpeed()      - เดินเร็วจนเจอแยก
 *    TracSpeedTime()    - เดินตามเส้นตามเวลา
 *    TracJCBack()       - ถอยตามเส้นจนเจอแยก
 *
 *  Implementation: ใช้ myrp_pico2.h API (Motor, error_A, error_B, etc.)
 * =============================================================================
 */

// =============================================================================
//  ตัวแปร Global สำหรับ PID
// =============================================================================

float TRAC_KP = 0.5;
float TRAC_KI = 0.0001;
float TRAC_KD = 0.025;
int TRAC_Speed = 50;
int TRAC_SlowSpeed = 25;
int TRAC_FastSpeed = 70;
int TRAC_Kt = 5;

float trac_previous_error = 0;
float trac_integral = 0;

// =============================================================================
//  SetTracPID - ตั้งค่า PID สำหรับ TracJC
// =============================================================================

void SetTracPID(float kp, float kd) {
  TRAC_KP = kp;
  TRAC_KD = kd;
}

void SetTracPID(float kp, float ki, float kd) {
  TRAC_KP = kp;
  TRAC_KI = ki;
  TRAC_KD = kd;
}

void SetTracSpeed(int speed) {
  TRAC_Speed = speed;
}

void SetTracSpeed(int speed, int slowSpeed, int fastSpeed) {
  TRAC_Speed = speed;
  TRAC_SlowSpeed = slowSpeed;
  TRAC_FastSpeed = fastSpeed;
}

// =============================================================================
//  TracPID - PID สำหรับเดินตามเส้น (ใช้ error_A จาก myrp_pico2.h)
// =============================================================================

void TracPID(int speed) {
  float error = error_A();

  trac_integral += error;
  trac_integral = constrain(trac_integral, -1000, 1000);

  float derivative = error - trac_previous_error;
  trac_previous_error = error;

  float output = (TRAC_KP * error) + (TRAC_KI * trac_integral) + (TRAC_KD * derivative);

  int leftSpeed = constrain(speed - output, -100, 100);
  int rightSpeed = constrain(speed + output, -100, 100);

  Motor(leftSpeed, rightSpeed);
}

void TracPIDBack(int speed) {
  float error = error_B();

  trac_integral += error;
  trac_integral = constrain(trac_integral, -1000, 1000);

  float derivative = error - trac_previous_error;
  trac_previous_error = error;

  float output = (TRAC_KP * error) + (TRAC_KI * trac_integral) + (TRAC_KD * derivative);

  int leftSpeed = constrain(speed + output, -100, 100);
  int rightSpeed = constrain(speed - output, -100, 100);

  Motor(-leftSpeed, -rightSpeed);
}

// =============================================================================
//  checkJunction - ตรวจสอบแยก
//  ใช้เซ็นเซอร์ A ตัวที่ 0 และ 7 (ซ้ายสุดและขวาสุด)
// =============================================================================

bool checkJunction() {
  int sL = read_sensorA(0);
  int sR = read_sensorA(7);
  int midL = md_sensorA(0);
  int midR = md_sensorA(7);

  // ถ้าเซ็นเซอร์ซ้ายสุดและขวาสุดเห็นดำ = แยก
  return (sL < midL && sR < midR);
}

bool checkJunctionLeft() {
  int sL = read_sensorA(0);
  int midL = md_sensorA(0);
  return (sL < midL);
}

bool checkJunctionRight() {
  int sR = read_sensorA(7);
  int midR = md_sensorA(7);
  return (sR < midR);
}

bool checkJunctionBack() {
  int sL = read_sensorB(0);
  int sR = read_sensorB(7);
  int midL = md_sensorB(0);
  int midR = md_sensorB(7);

  return (sL < midL && sR < midR);
}

// =============================================================================
//  TracSpeedTime - เดินตามเส้นตามเวลา (ms)
// =============================================================================

void TracSpeedTime(int speed, int timeMs) {
  unsigned long startTime = millis();
  trac_previous_error = 0;
  trac_integral = 0;

  while ((millis() - startTime) < timeMs) {
    TracPID(speed);
    delay(TRAC_Kt);
  }
}

void TracSpeedTimeBack(int speed, int timeMs) {
  unsigned long startTime = millis();
  trac_previous_error = 0;
  trac_integral = 0;

  while ((millis() - startTime) < timeMs) {
    TracPIDBack(speed);
    delay(TRAC_Kt);
  }
}

// =============================================================================
//  TracJCStop - เดินตามเส้นจนเจอแยกแล้วหยุด
// =============================================================================

void TracJCStop() {
  trac_previous_error = 0;
  trac_integral = 0;

  // เดินเลยแยกเล็กน้อยก่อน
  TracSpeedTime(TRAC_Speed, 5000 / TRAC_Speed);

  // เดินจนเจอแยก
  while (!checkJunction()) {
    TracPID(TRAC_Speed);
    delay(TRAC_Kt);
  }

  Motor(0, 0);
}

void TracJCStopBack() {
  trac_previous_error = 0;
  trac_integral = 0;

  // ถอยเลยแยกเล็กน้อยก่อน
  TracSpeedTimeBack(TRAC_Speed, 5000 / TRAC_Speed);

  // ถอยจนเจอแยก
  while (!checkJunctionBack()) {
    TracPIDBack(TRAC_Speed);
    delay(TRAC_Kt);
  }

  Motor(0, 0);
}

// =============================================================================
//  TuneJC - ปรับตำแหน่งหลังเจอแยก (เดินเล็กน้อย)
// =============================================================================

void TuneJC() {
  int tuneTime;
  if (TRAC_Speed <= 50) tuneTime = 4000 / TRAC_Speed;
  else if (TRAC_Speed <= 60) tuneTime = 3000 / TRAC_Speed;
  else if (TRAC_Speed <= 70) tuneTime = 1500 / TRAC_Speed;
  else if (TRAC_Speed <= 80) tuneTime = 500 / TRAC_Speed;
  else tuneTime = 100 / TRAC_Speed;

  Motor(TRAC_Speed, TRAC_Speed);
  delay(tuneTime);
  Motor(0, 0);
}

void TuneJCBack() {
  int tuneTime;
  if (TRAC_Speed <= 50) tuneTime = 4000 / TRAC_Speed;
  else if (TRAC_Speed <= 60) tuneTime = 3000 / TRAC_Speed;
  else if (TRAC_Speed <= 70) tuneTime = 1500 / TRAC_Speed;
  else if (TRAC_Speed <= 80) tuneTime = 500 / TRAC_Speed;
  else tuneTime = 100 / TRAC_Speed;

  Motor(-TRAC_Speed, -TRAC_Speed);
  delay(tuneTime);
  Motor(0, 0);
}

// =============================================================================
//  TracJC - เดินตามเส้นจนเจอแยก พร้อม TuneJC
// =============================================================================

void TracJC() {
  TracJCStop();
  TuneJC();
}

void TracJC(int speed) {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = speed;
  TracJCStop();
  TuneJC();
  TRAC_Speed = tempSpeed;
}

void TracJCBack() {
  TracJCStopBack();
  TuneJCBack();
}

void TracJCBack(int speed) {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = speed;
  TracJCStopBack();
  TuneJCBack();
  TRAC_Speed = tempSpeed;
}

// =============================================================================
//  TracJCSlow - เดินช้าจนเจอแยก
// =============================================================================

void TracJCSlowStop() {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = TRAC_SlowSpeed;
  TracJCStop();
  TRAC_Speed = tempSpeed;
}

void TracJCSlow() {
  TracJCSlowStop();

  // เดินจนออกจากแยก
  Motor(TRAC_SlowSpeed, TRAC_SlowSpeed);
  while (checkJunction()) {
    delay(1);
  }
}

// =============================================================================
//  TracJCSpeed - เดินเร็วจนเจอแยก
// =============================================================================

void TracJCSpeedStop() {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = TRAC_FastSpeed;
  TracJCStop();
  TRAC_Speed = tempSpeed;
}

void TracJCSpeed() {
  TracJCSpeedStop();

  // เดินต่อเล็กน้อย
  Motor(TRAC_FastSpeed, TRAC_FastSpeed);
  delay(5000 / TRAC_FastSpeed);
  Motor(0, 0);
}

void TracJCSpeedBack() {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = TRAC_FastSpeed;
  TracJCStopBack();
  TRAC_Speed = tempSpeed;

  // ถอยต่อเล็กน้อย
  Motor(-TRAC_FastSpeed, -TRAC_FastSpeed);
  delay(5000 / TRAC_FastSpeed);
  Motor(0, 0);
}

// =============================================================================
//  TracJCSpeedTime - เดินตามความเร็วและเวลาที่กำหนด
// =============================================================================

void TracJCSpeedTime(int speed, int timeMs) {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = speed;
  TracJCStop();
  Motor(speed, speed);
  delay(timeMs);
  Motor(0, 0);
  TRAC_Speed = tempSpeed;
}

void TracJCSpeedTimeBack(int speed, int timeMs) {
  int tempSpeed = TRAC_Speed;
  TRAC_Speed = speed;
  TracJCStopBack();
  Motor(-speed, -speed);
  delay(timeMs);
  Motor(0, 0);
  TRAC_Speed = tempSpeed;
}

// =============================================================================
//  TracJCSpeedLine - เดินข้ามแยกตามจำนวน line
// =============================================================================

void TracJCSpeedLine(int numLines) {
  for (int i = 0; i < numLines; i++) {
    TracJCSpeed();
  }
}

// =============================================================================
//  TracJCLeft / TracJCRight - เดินจนเจอแยกซ้าย/ขวา
// =============================================================================

void TracJCStopLeft() {
  trac_previous_error = 0;
  trac_integral = 0;

  while (!checkJunctionLeft()) {
    TracPID(TRAC_Speed);
    delay(TRAC_Kt);
  }

  Motor(0, 0);
}

void TracJCLeft() {
  TracJCStopLeft();
  TuneJC();
}

void TracJCStopRight() {
  trac_previous_error = 0;
  trac_integral = 0;

  while (!checkJunctionRight()) {
    TracPID(TRAC_Speed);
    delay(TRAC_Kt);
  }

  Motor(0, 0);
}

void TracJCRight() {
  TracJCStopRight();
  TuneJC();
}

void TracJCLorR() {
  trac_previous_error = 0;
  trac_integral = 0;

  TracSpeedTime(TRAC_Speed, 10000 / TRAC_Speed);

  while (!checkJunctionLeft() && !checkJunctionRight()) {
    TracPID(TRAC_Speed);
    delay(TRAC_Kt);
  }

  TuneJC();
}

// =============================================================================
//  TracJCStraight - เดินจนเจอแยก แล้วตรงไปต่อ
// =============================================================================

void TracJCStraight() {
  TracJC();
  Motor(TRAC_Speed, TRAC_Speed);
  delay(5000 / TRAC_Speed);
  Motor(0, 0);
}

// =============================================================================
//  ForwardJC - เดินหน้าจนเจอแยก (ไม่ใช้ PID)
// =============================================================================

void ForwardJCStop() {
  // เดินหน้าก่อน
  Motor(TRAC_Speed, TRAC_Speed);
  delay(5000 / TRAC_Speed);

  // วนจนเจอแยก
  while (!checkJunction()) {
    delay(1);
  }

  Motor(0, 0);
}

void ForwardJC() {
  ForwardJCStop();
  TuneJC();
}

// =============================================================================
//  คำสั่งเดินหน้า/ถอยหลังพื้นฐาน
// =============================================================================

void Forward() {
  Motor(TRAC_Speed, TRAC_Speed);
}

void ForwardDelay(int timeMs) {
  Motor(TRAC_Speed, TRAC_Speed);
  delay(timeMs);
  Motor(0, 0);
}

void ForwardSpeedTime(int speed, int timeMs) {
  Motor(speed, speed);
  delay(timeMs);
  Motor(0, 0);
}

void ForwardSpeedDelay(int timeMs) {
  Motor(TRAC_FastSpeed, TRAC_FastSpeed);
  delay(timeMs);
  Motor(0, 0);
}

void Backward() {
  Motor(-TRAC_Speed, -TRAC_Speed);
}

void BackwardDelay(int timeMs) {
  Motor(-TRAC_Speed, -TRAC_Speed);
  delay(timeMs);
  Motor(0, 0);
}

void BackwardSpeedTime(int speed, int timeMs) {
  Motor(-speed, -speed);
  delay(timeMs);
  Motor(0, 0);
}

void MotorStop() {
  Motor(0, 0);
}

// =============================================================================
//  Finish / AO (All Off)
// =============================================================================

void Finish() {
  Motor(0, 0);
  tone(9, 2000, 200);
  delay(300);
  tone(9, 2500, 200);
  delay(300);
  tone(9, 3000, 400);
}

void FinishWaitOK() {
  Finish();
  while (1) {
    delay(100);
  }
}

void AO() {
  Motor(0, 0);
}

// =============================================================================
//  คำสั่ง Delay ย่อ
// =============================================================================

void D(int ms) {
  delay(ms);
}

