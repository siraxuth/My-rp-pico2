/*
 * =============================================================================
 *  MoveCM.ino - เดินหน้า/ถอยหลังตามระยะทาง (cm) ด้วยเวลา + Gyro
 * =============================================================================
 *
 *  ใช้เวลาในการคำนวณระยะทาง (ไม่ใช้ Encoder)
 *  ปรับค่า CM_PER_MS ตามความเร็วจริงของหุ่นยนต์
 *
 *  Implementation: ใช้ my_GYRO160 จาก myrp_pico2.h
 * =============================================================================
 */

// =============================================================================
//  ค่าคงที่สำหรับการคำนวณระยะทางด้วยเวลา
// =============================================================================

// ค่า calibration: cm ต่อ ms ที่ speed 50
// ปรับค่านี้ตามหุ่นยนต์จริง (ทดสอบแล้วปรับ)
float CM_PER_MS_AT_50 = 0.015;  // ประมาณ 15 cm/sec ที่ speed 50

// ค่า PID สำหรับ Gyro รักษาทิศทาง
float GYRO_STRAIGHT_KP = 2.0;
float GYRO_STRAIGHT_KI = 0.001;
float GYRO_STRAIGHT_KD = 0.1;

// ค่า PID สำหรับ Line following
float LINE_CM_KP = 0.5;
float LINE_CM_KI = 0.001;
float LINE_CM_KD = 0.05;

// =============================================================================
//  ฟังก์ชันแปลงหน่วย
// =============================================================================

int cmToMs(float cm, int speed) {
  // คำนวณเวลาที่ต้องใช้ (ms) ตาม cm และ speed
  float speedRatio = (float)speed / 50.0;
  float cmPerMs = CM_PER_MS_AT_50 * speedRatio;
  return (int)(cm / cmPerMs);
}

float msToCm(int ms, int speed) {
  // แปลง ms กลับเป็น cm
  float speedRatio = (float)speed / 50.0;
  float cmPerMs = CM_PER_MS_AT_50 * speedRatio;
  return ms * cmPerMs;
}

// =============================================================================
//  ForwardCM - เดินหน้าตาม cm (ใช้เวลา ไม่ใช้ Encoder)
// =============================================================================

void ForwardCM(int speed, float cm) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  while ((millis() - startTime) < cm * 21)
  {
    Motor(speed, speed);
    // delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  BackwardCM - ถอยหลังตาม cm (ใช้เวลา ไม่ใช้ Encoder)
// =============================================================================

void BackwardCM(int speed, float cm) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  while ((millis() - startTime) < targetMs) {
    Motor(-speed, -speed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  ForwardCM_Gyro - เดินหน้าตาม cm พร้อม Gyro รักษาทิศทาง
// =============================================================================

void ForwardCM_Gyro(int speed, float cm) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  my.resetYaw();

  float integral = 0;
  float prevError = 0;

  while ((millis() - startTime) < targetMs) {
    float yawError = my.gyro('z');
    if (yawError > 180) yawError -= 360;

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (GYRO_STRAIGHT_KP * yawError) + (GYRO_STRAIGHT_KI * integral) + (GYRO_STRAIGHT_KD * derivative);

    int leftSpeed = constrain(speed + correction, 0, 100);
    int rightSpeed = constrain(speed - correction, 0, 100);

    Motor(leftSpeed, rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  BackwardCM_Gyro - ถอยหลังตาม cm พร้อม Gyro รักษาทิศทาง
// =============================================================================

void BackwardCM_Gyro(int speed, float cm) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  my.resetYaw();

  float integral = 0;
  float prevError = 0;

  while ((millis() - startTime) < targetMs) {
    float yawError = my.gyro('z');
    if (yawError > 180) yawError -= 360;

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (GYRO_STRAIGHT_KP * yawError) + (GYRO_STRAIGHT_KI * integral) + (GYRO_STRAIGHT_KD * derivative);

    int leftSpeed = constrain(speed - correction, 0, 100);
    int rightSpeed = constrain(speed + correction, 0, 100);

    Motor(-leftSpeed, -rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  ForwardCM_Line - เดินตามเส้นตาม cm (ใช้เวลา + เซ็นเซอร์เส้น)
// =============================================================================

void ForwardCM_Line(int speed, float cm) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  float integral = 0;
  float prevError = 0;

  while ((millis() - startTime) < targetMs) {
    float lineError = error_A();

    integral += lineError;
    integral = constrain(integral, -1000, 1000);
    float derivative = lineError - prevError;
    prevError = lineError;

    float correction = (LINE_CM_KP * lineError) + (LINE_CM_KI * integral) + (LINE_CM_KD * derivative);

    int leftSpeed = constrain(speed - correction, 0, 100);
    int rightSpeed = constrain(speed + correction, 0, 100);

    Motor(leftSpeed, rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  BackwardCM_Line - ถอยตามเส้นตาม cm (ใช้เวลา + เซ็นเซอร์เส้น)
// =============================================================================

void BackwardCM_Line(int speed, float cm) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  float integral = 0;
  float prevError = 0;

  while ((millis() - startTime) < targetMs) {
    float lineError = error_B();

    integral += lineError;
    integral = constrain(integral, -1000, 1000);
    float derivative = lineError - prevError;
    prevError = lineError;

    float correction = (LINE_CM_KP * lineError) + (LINE_CM_KI * integral) + (LINE_CM_KD * derivative);

    int leftSpeed = constrain(speed + correction, 0, 100);
    int rightSpeed = constrain(speed - correction, 0, 100);

    Motor(-leftSpeed, -rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  ForwardCM_Line_Gyro - เดินตามเส้น + Gyro ตาม cm
// =============================================================================

void ForwardCM_Line_Gyro(int speed, float cm, float kp_line) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  my.resetYaw();

  float lineIntegral = 0;
  float linePrevError = 0;
  float gyroIntegral = 0;
  float gyroPrevError = 0;

  float lineWeight = 0.7;
  float gyroWeight = 0.3;

  while ((millis() - startTime) < targetMs) {
    // Line PID
    float lineError = error_A();
    lineIntegral += lineError;
    lineIntegral = constrain(lineIntegral, -1000, 1000);
    float lineDerivative = lineError - linePrevError;
    linePrevError = lineError;

    float lineCorrection = (kp_line * lineError) + (LINE_CM_KI * lineIntegral) + (LINE_CM_KD * lineDerivative);

    // Gyro PID
    float yawError = my.gyro('z');
    if (yawError > 180) yawError -= 360;

    gyroIntegral += yawError;
    gyroIntegral = constrain(gyroIntegral, -500, 500);
    float gyroDerivative = yawError - gyroPrevError;
    gyroPrevError = yawError;

    float gyroCorrection = (GYRO_STRAIGHT_KP * yawError) + (GYRO_STRAIGHT_KI * gyroIntegral) + (GYRO_STRAIGHT_KD * gyroDerivative);

    float totalCorrection = (lineWeight * lineCorrection) + (gyroWeight * gyroCorrection);

    int leftSpeed = constrain(speed - totalCorrection, 0, 100);
    int rightSpeed = constrain(speed + totalCorrection, 0, 100);

    Motor(leftSpeed, rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  BackwardCM_Line_Gyro - ถอยตามเส้น + Gyro ตาม cm
// =============================================================================

void BackwardCM_Line_Gyro(int speed, float cm, float kp_line) {
  int targetMs = cmToMs(cm, speed);
  unsigned long startTime = millis();

  my.resetYaw();

  float lineIntegral = 0;
  float linePrevError = 0;
  float gyroIntegral = 0;
  float gyroPrevError = 0;

  float lineWeight = 0.7;
  float gyroWeight = 0.3;

  while ((millis() - startTime) < targetMs) {
    // Line PID
    float lineError = error_B();
    lineIntegral += lineError;
    lineIntegral = constrain(lineIntegral, -1000, 1000);
    float lineDerivative = lineError - linePrevError;
    linePrevError = lineError;

    float lineCorrection = (kp_line * lineError) + (LINE_CM_KI * lineIntegral) + (LINE_CM_KD * lineDerivative);

    // Gyro PID
    float yawError = my.gyro('z');
    if (yawError > 180) yawError -= 360;

    gyroIntegral += yawError;
    gyroIntegral = constrain(gyroIntegral, -500, 500);
    float gyroDerivative = yawError - gyroPrevError;
    gyroPrevError = yawError;

    float gyroCorrection = (GYRO_STRAIGHT_KP * yawError) + (GYRO_STRAIGHT_KI * gyroIntegral) + (GYRO_STRAIGHT_KD * gyroDerivative);

    float totalCorrection = (lineWeight * lineCorrection) + (gyroWeight * gyroCorrection);

    int leftSpeed = constrain(speed + totalCorrection, 0, 100);
    int rightSpeed = constrain(speed - totalCorrection, 0, 100);

    Motor(-leftSpeed, -rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  Smooth versions with acceleration/deceleration (ใช้เวลา)
// =============================================================================

void ForwardCM_Smooth(int maxSpeed, float cm, float accelCm, float decelCm) {
  int targetMs = cmToMs(cm, maxSpeed);
  int accelMs = cmToMs(accelCm, maxSpeed);
  int decelMs = cmToMs(decelCm, maxSpeed);

  unsigned long startTime = millis();
  my.resetYaw();

  float integral = 0;
  float prevError = 0;
  int currentSpeed = 20;

  while ((millis() - startTime) < targetMs) {
    unsigned long elapsed = millis() - startTime;
    int remaining = targetMs - elapsed;

    // ช่วง acceleration
    if (elapsed < accelMs) {
      currentSpeed = map(elapsed, 0, accelMs, 20, maxSpeed);
    }
    // ช่วง deceleration
    else if (remaining < decelMs) {
      currentSpeed = map(remaining, 0, decelMs, 20, maxSpeed);
    }
    // ช่วง constant speed
    else {
      currentSpeed = maxSpeed;
    }

    currentSpeed = constrain(currentSpeed, 20, maxSpeed);

    // Gyro PID
    float yawError = my.gyro('z');
    if (yawError > 180) yawError -= 360;

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (GYRO_STRAIGHT_KP * yawError) + (GYRO_STRAIGHT_KI * integral) + (GYRO_STRAIGHT_KD * derivative);

    int leftSpeed = constrain(currentSpeed + correction, 0, 100);
    int rightSpeed = constrain(currentSpeed - correction, 0, 100);

    Motor(leftSpeed, rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

void BackwardCM_Smooth(int maxSpeed, float cm, float accelCm, float decelCm) {
  int targetMs = cmToMs(cm, maxSpeed);
  int accelMs = cmToMs(accelCm, maxSpeed);
  int decelMs = cmToMs(decelCm, maxSpeed);

  unsigned long startTime = millis();
  my.resetYaw();

  float integral = 0;
  float prevError = 0;
  int currentSpeed = 20;

  while ((millis() - startTime) < targetMs) {
    unsigned long elapsed = millis() - startTime;
    int remaining = targetMs - elapsed;

    // ช่วง acceleration
    if (elapsed < accelMs) {
      currentSpeed = map(elapsed, 0, accelMs, 20, maxSpeed);
    }
    // ช่วง deceleration
    else if (remaining < decelMs) {
      currentSpeed = map(remaining, 0, decelMs, 20, maxSpeed);
    }
    // ช่วง constant speed
    else {
      currentSpeed = maxSpeed;
    }

    currentSpeed = constrain(currentSpeed, 20, maxSpeed);

    // Gyro PID
    float yawError = my.gyro('z');
    if (yawError > 180) yawError -= 360;

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (GYRO_STRAIGHT_KP * yawError) + (GYRO_STRAIGHT_KI * integral) + (GYRO_STRAIGHT_KD * derivative);

    int leftSpeed = constrain(currentSpeed - correction, 0, 100);
    int rightSpeed = constrain(currentSpeed + correction, 0, 100);

    Motor(-leftSpeed, -rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  คำสั่งย่อ (Short Commands)
// =============================================================================

void FCM(int speed, float cm) { ForwardCM(speed, cm); }
void BCM(int speed, float cm) { BackwardCM(speed, cm); }
void FCMG(int speed, float cm) { ForwardCM_Gyro(speed, cm); }
void BCMG(int speed, float cm) { BackwardCM_Gyro(speed, cm); }
void FCML(int speed, float cm) { ForwardCM_Line(speed, cm); }
void BCML(int speed, float cm) { BackwardCM_Line(speed, cm); }
void FCMLG(int speed, float cm, float kp) { ForwardCM_Line_Gyro(speed, cm, kp); }
void BCMLG(int speed, float cm, float kp) { BackwardCM_Line_Gyro(speed, cm, kp); }
void FCMS(int speed, float cm) { ForwardCM_Smooth(speed, cm, 5, 5); }
void BCMS(int speed, float cm) { BackwardCM_Smooth(speed, cm, 5, 5); }

// =============================================================================
//  ฟังก์ชัน Calibration
// =============================================================================

void setCmPerMs(float cmPerMs) {
  CM_PER_MS_AT_50 = cmPerMs;
}

void setGyroStraightPID(float kp, float ki, float kd) {
  GYRO_STRAIGHT_KP = kp;
  GYRO_STRAIGHT_KI = ki;
  GYRO_STRAIGHT_KD = kd;
}

void setLineCM_PID(float kp, float ki, float kd) {
  LINE_CM_KP = kp;
  LINE_CM_KI = ki;
  LINE_CM_KD = kd;
}

// =============================================================================
//  ฟังก์ชันทดสอบ Calibration
// =============================================================================

void testCalibration() {
  Serial.println("=== Time-based Calibration Test ===");
  Serial.print("CM_PER_MS_AT_50: ");
  Serial.println(CM_PER_MS_AT_50, 4);
  Serial.print("Time for 10cm at speed 50: ");
  Serial.print(cmToMs(10, 50));
  Serial.println(" ms");
  Serial.print("Time for 10cm at speed 100: ");
  Serial.print(cmToMs(10, 100));
  Serial.println(" ms");
  Serial.println("====================================");
}

// ทดสอบเดินไป 10 cm แล้ววัดจริง ปรับค่า CM_PER_MS_AT_50
void calibrateCM() {
  Serial.println("Calibration: Moving 10cm at speed 50...");
  delay(1000);
  ForwardCM(50, 10);
  Serial.println("Done! Measure actual distance and adjust CM_PER_MS_AT_50");
  Serial.println("Formula: new_value = old_value * (10 / actual_cm)");
}

