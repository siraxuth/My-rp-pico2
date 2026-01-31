/*
 * =============================================================================
 *  Movement.ino - คำสั่งเดินหน้า/ถอยหลัง (Forward/Backward)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    Forward(speed)             - เดินหน้า
 *    Backward(speed)            - ถอยหลัง
 *    ForwardPID()               - เดินหน้าพร้อม PID (ใช้ค่าจาก Initial.ino)
 *    BackwardPID()              - ถอยหลังพร้อม PID
 *    ForwardTime(speed, ms)     - เดินหน้าตามเวลา
 *    BackwardTime(speed, ms)    - ถอยหลังตามเวลา
 *    ForwardUntilLine()         - เดินหน้าจนเจอเส้น
 *    BackwardUntilLine()        - ถอยหลังจนเจอเส้น
 *
 *  ใช้ตัวแปรจาก Initial.ino:
 *    - LeftBaseSpeed, RightBaseSpeed
 *    - Kp, Ki, Kd, Kt (Forward PID)
 *    - KpB, KiB, KdB, KtB (Backward PID)
 *
 * =============================================================================
 */

// ตัวแปร Local สำหรับ PID
float _error = 0;
float _lastError = 0;
float _integral = 0;
float _derivative = 0;
float _output = 0;

// =============================================================================
//  Forward - เดินหน้าพื้นฐาน
// =============================================================================

void Forward() {
  Motor(LeftBaseSpeed, RightBaseSpeed);
}

void Forward(int speed) {
  Motor(speed, speed);
}

void Forward(int leftSpeed, int rightSpeed) {
  Motor(leftSpeed, rightSpeed);
}

// =============================================================================
//  Backward - ถอยหลังพื้นฐาน
// =============================================================================

void Backward() {
  Motor(-LeftBaseSpeed, -RightBaseSpeed);
}

void Backward(int speed) {
  Motor(-speed, -speed);
}

void Backward(int leftSpeed, int rightSpeed) {
  Motor(-leftSpeed, -rightSpeed);
}

// =============================================================================
//  Stop - หยุดมอเตอร์
// =============================================================================

void Stop() {
  Motor(0, 0);
}

void Brake(int power, int ms) {
  // เบรคด้วยการหมุนกลับทิศ
  Motor(-power, -power);
  delay(ms);
  Motor(0, 0);
}

// =============================================================================
//  ForwardTime / BackwardTime - เดิน/ถอยตามเวลา
// =============================================================================

void ForwardSpeedTime(int speed, int ms) {
  Motor(speed, speed);
  delay(ms);
  Motor(0, 0);
}

void ForwardTime(int leftSpeed, int rightSpeed, int ms) {
  Motor(leftSpeed, rightSpeed);
  delay(ms);
  Motor(0, 0);
}

void BackwardSpeedTime(int speed, int ms)
{
  Motor(-speed, -speed);
  delay(ms);
  Motor(0, 0);
}

void BackwardTime(int leftSpeed, int rightSpeed, int ms) {
  Motor(-leftSpeed, -rightSpeed);
  delay(ms);
  Motor(0, 0);
}

// =============================================================================
//  ForwardPID - เดินหน้าพร้อม PID (ใช้ค่าจาก Initial.ino)
// =============================================================================

void ForwardPID() {
  // อ่านค่า error จากเซ็นเซอร์
  _error = error_A();
  
  // คำนวณ PID
  _integral += _error;
  _derivative = _error - _lastError;
  _lastError = _error;
  
  // จำกัด Integral
  if (_integral > 1000) _integral = 1000;
  if (_integral < -1000) _integral = -1000;
  
  // คำนวณ output
  _output = (Kp * _error) + (Ki * _integral) + (Kd * _derivative);
  
  // ควบคุมมอเตอร์
  int left = LeftBaseSpeed - _output;
  int right = RightBaseSpeed + _output;
  
  // จำกัดค่า
  left = constrain(left, -100, 100);
  right = constrain(right, -100, 100);
  
  Motor(left, right);
}

void ForwardPID(float kp, float kd) {
  _error = error_A();
  _derivative = _error - _lastError;
  _lastError = _error;
  
  _output = (kp * _error) + (kd * _derivative);
  
  int left = LeftBaseSpeed - _output;
  int right = RightBaseSpeed + _output;
  
  left = constrain(left, -100, 100);
  right = constrain(right, -100, 100);
  
  Motor(left, right);
}

// =============================================================================
//  BackwardPID - ถอยหลังพร้อม PID (ใช้ค่าจาก Initial.ino)
// =============================================================================

void BackwardPID() {
  // อ่านค่า error จากเซ็นเซอร์หลัง
  _error = error_B();
  
  // คำนวณ PID
  _integral += _error;
  _derivative = _error - _lastError;
  _lastError = _error;
  
  // จำกัด Integral
  if (_integral > 1000) _integral = 1000;
  if (_integral < -1000) _integral = -1000;
  
  // คำนวณ output
  _output = (KpB * _error) + (KiB * _integral) + (KdB * _derivative);
  
  // ควบคุมมอเตอร์ (ถอยหลัง)
  int left = -(LeftBaseSpeed + _output);
  int right = -(RightBaseSpeed - _output);
  
  left = constrain(left, -100, 100);
  right = constrain(right, -100, 100);
  
  Motor(left, right);
}

void BackwardPID(float kp, float kd) {
  _error = error_B();
  _derivative = _error - _lastError;
  _lastError = _error;
  
  _output = (kp * _error) + (kd * _derivative);
  
  int left = -(LeftBaseSpeed + _output);
  int right = -(RightBaseSpeed - _output);
  
  left = constrain(left, -100, 100);
  right = constrain(right, -100, 100);
  
  Motor(left, right);
}

// =============================================================================
//  ResetPID - รีเซ็ตค่า PID
// =============================================================================

void ResetPID() {
  _error = 0;
  _lastError = 0;
  _integral = 0;
  _derivative = 0;
  _output = 0;
  Error = 0;
  PreError = 0;
}

// =============================================================================
//  ForwardUntilLine - เดินหน้าจนเจอเส้น (แยก)
// =============================================================================

void ForwardUntilLine() {
  ResetPID();
  while (1) {
    // ForwardPID();
    delayMicroseconds(50);
    
    // เจอเส้นซ้ายหรือขวา = แยก
    if ((read_sensorA(0) < md_sensorA(0) && read_sensorA(2) < md_sensorA(2)) ||
        (read_sensorA(7) < md_sensorA(7) && read_sensorA(5) < md_sensorA(5))) {
      break;
    }
  }
  Motor(0, 0);
}

void ForwardUntilLine(int speed) {
  int tempLeft = LeftBaseSpeed;
  int tempRight = RightBaseSpeed;
  
  LeftBaseSpeed = speed;
  RightBaseSpeed = speed;
  
  ForwardUntilLine();
  
  LeftBaseSpeed = tempLeft;
  RightBaseSpeed = tempRight;
}

// =============================================================================
//  BackwardUntilLine - ถอยหลังจนเจอเส้น (แยก)
// =============================================================================

void BackwardUntilLine() {
  ResetPID();
  while (1) {
    BackwardPID();
    delayMicroseconds(50);
    
    // เจอเส้นซ้ายหรือขวา = แยก
    if ((read_sensorB(0) < md_sensorB(0) && read_sensorB(1) < md_sensorB(1)) ||
        (read_sensorB(7) < md_sensorB(7) && read_sensorB(6) < md_sensorB(6))) {
      break;
    }
  }
  Motor(0, 0);
}

void BackwardUntilLine(int speed) {
  int tempLeft = LeftBaseSpeed;
  int tempRight = RightBaseSpeed;
  
  LeftBaseSpeed = speed;
  RightBaseSpeed = speed;
  
  BackwardUntilLine();
  
  LeftBaseSpeed = tempLeft;
  RightBaseSpeed = tempRight;
}

// =============================================================================
//  ForwardDistance - เดินหน้าตามระยะทาง (cm) โดยใช้เวลา
// =============================================================================

void ForwardDistance(int speed, float cm) {
  // คำนวณเวลาจากความเร็วและระยะทาง (ปรับค่าตามหุ่นยนต์จริง)
  float cmPerMs = 0.015 * (speed / 50.0);  // ประมาณ 15 cm/s ที่ speed 50
  int timeMs = (int)(cm / cmPerMs);
  
  ResetPID();
  unsigned long startTime = millis();
  
  while ((millis() - startTime) < timeMs) {
    ForwardPID();
    delayMicroseconds(50);
  }
  
  Motor(0, 0);
}

void BackwardDistance(int speed, float cm) {
  float cmPerMs = 0.015 * (speed / 50.0);
  int timeMs = (int)(cm / cmPerMs);
  
  ResetPID();
  unsigned long startTime = millis();
  
  while ((millis() - startTime) < timeMs) {
    BackwardPID();
    delayMicroseconds(50);
  }
  
  Motor(0, 0);
}

// =============================================================================
//  Soft Start/Stop - เร่งความเร็วแบบนุ่มนวล
// =============================================================================

void ForwardSoftStart(int targetSpeed, int rampTime) {
  int step = 2;
  int currentSpeed = 0;
  int delayPerStep = rampTime / (targetSpeed / step);
  
  while (currentSpeed < targetSpeed) {
    Motor(currentSpeed, currentSpeed);
    currentSpeed += step;
    if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
    delay(delayPerStep);
  }
}

void ForwardSoftStop(int currentSpeed, int rampTime) {
  int step = 2;
  int delayPerStep = rampTime / (currentSpeed / step);
  
  while (currentSpeed > 0) {
    Motor(currentSpeed, currentSpeed);
    currentSpeed -= step;
    if (currentSpeed < 0) currentSpeed = 0;
    delay(delayPerStep);
  }
  Motor(0, 0);
}

// =============================================================================
//  Shorthand Commands (คำสั่งย่อ)
// =============================================================================

// void FW()                  { Forward(); }
// void FW(int s)             { Forward(s); }
// void FW(int l, int r)      { Forward(l, r); }

// void BW()                  { Backward(); }
// void BW(int s)             { Backward(s); }
// void BW(int l, int r)      { Backward(l, r); }

// void FWT(int s, int t)     { ForwardTime(s, t); }
// void BWT(int s, int t)     { BackwardTime(s, t); }

// void FWP()                 { ForwardPID(); }
// void BWP()                 { BackwardPID(); }

// void FWUL()                { ForwardUntilLine(); }
// void BWUL()                { BackwardUntilLine(); }

// void FWD(int s, float cm)  { ForwardDistance(s, cm); }
// void BWD(int s, float cm)  { BackwardDistance(s, cm); }
