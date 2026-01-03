/*
 * =============================================================================
 *  TurnIMU.ino - เลี้ยวด้วย Gyro/IMU (MyRP_Pico2 Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    TL_deg() / TR_deg()        - เลี้ยวซ้าย/ขวาตามองศา
 *    UTL_deg() / UTR_deg()      - กลับตัวตามองศา
 *    SpinDegree()               - หมุนไปยังองศาที่กำหนด
 *    TracDegree()               - เดินตรงตามองศา
 *
 *  Implementation: ใช้ my_GYRO160 จาก myrp_pico2.h
 * =============================================================================
 */

// =============================================================================
//  ค่า Config สำหรับ IMU Turn
// =============================================================================

int IMU_TurnSpeed = 70;
int IMU_BrakeSpeed = 100;
int IMU_BrakeTime = 10;
float IMU_Tolerance = 2.0;  // ความคลาดเคลื่อนที่ยอมรับได้ (องศา)

// =============================================================================
//  SetIMUTurnConfig - ตั้งค่าการเลี้ยวด้วย IMU
// =============================================================================

void SetIMUTurnSpeed(int speed) {
  IMU_TurnSpeed = speed;
}

void SetIMUBrake(int speed, int time) {
  IMU_BrakeSpeed = speed;
  IMU_BrakeTime = time;
}

void SetIMUTolerance(float tol) {
  IMU_Tolerance = tol;
}

// =============================================================================
//  getYaw - อ่านค่า Yaw จาก Gyro (ใช้ my จาก myrp_pico2.h)
// =============================================================================

float getYaw() {
  return my.gyro('z');
}

// =============================================================================
//  normalizeAngle - ทำให้มุมอยู่ในช่วง -180 ถึง 180
// =============================================================================

float normalizeAngle(float angle) {
  while (angle > 180) angle -= 360;
  while (angle < -180) angle += 360;
  return angle;
}

// =============================================================================
//  TL_deg - เลี้ยวซ้ายตามองศา (ใช้ Gyro)
// =============================================================================

void TL_deg(int speed, float degree) {
  Motor(0, 0);
  my.resetYaw();
  delay(50);

  float targetAngle = degree;  // เลี้ยวซ้าย = มุม +

  // หมุนเร็ว
  Motor(-speed, speed);

  // รอจนใกล้ถึงมุม
  while (getYaw() < targetAngle - 30) {
    delay(1);
  }

  // ช้าลง
  Motor(-20, 20);

  // รอจนถึงมุม
  while (getYaw() < targetAngle - IMU_Tolerance) {
    delay(1);
  }

  // Brake
  if (IMU_BrakeTime > 0 && speed > 50) {
    Motor(IMU_BrakeSpeed, -IMU_BrakeSpeed);
    delay(IMU_BrakeTime);
  }

  Motor(0, 0);
}

void TL_deg(float degree) {
  TL_deg(IMU_TurnSpeed, degree);
}

// =============================================================================
//  TR_deg - เลี้ยวขวาตามองศา (ใช้ Gyro)
// =============================================================================

void TR_deg(int speed, float degree) {
  Motor(0, 0);
  my.resetYaw();
  delay(50);

  float targetAngle = -degree;  // เลี้ยวขวา = มุม -

  // หมุนเร็ว
  Motor(speed, -speed);

  // รอจนใกล้ถึงมุม
  while (getYaw() > targetAngle + 30) {
    delay(1);
  }

  // ช้าลง
  Motor(20, -20);

  // รอจนถึงมุม
  while (getYaw() > targetAngle + IMU_Tolerance) {
    delay(1);
  }

  // Brake
  if (IMU_BrakeTime > 0 && speed > 50) {
    Motor(-IMU_BrakeSpeed, IMU_BrakeSpeed);
    delay(IMU_BrakeTime);
  }

  Motor(0, 0);
}

void TR_deg(float degree) {
  TR_deg(IMU_TurnSpeed, degree);
}

// =============================================================================
//  UTL_deg / UTR_deg - กลับตัวตามองศา
// =============================================================================

void UTL_deg(int speed, float degree) {
  TL_deg(speed, degree);
}

void UTL_deg(float degree) {
  TL_deg(IMU_TurnSpeed, degree);
}

void UTR_deg(int speed, float degree) {
  TR_deg(speed, degree);
}

void UTR_deg(float degree) {
  TR_deg(IMU_TurnSpeed, degree);
}

// =============================================================================
//  SpinDegree - หมุนไปยังองศาสัมบูรณ์
// =============================================================================

void SpinDegree(int speed, float targetDegree) {
  Motor(0, 0);

  float currentYaw = getYaw();
  float diff = normalizeAngle(targetDegree - currentYaw);

  if (abs(diff) < IMU_Tolerance) return;  // ถึงแล้ว

  if (diff > 0) {
    // หมุนซ้าย
    Motor(-speed, speed);
    while (normalizeAngle(targetDegree - getYaw()) > IMU_Tolerance) {
      delay(1);
    }
  } else {
    // หมุนขวา
    Motor(speed, -speed);
    while (normalizeAngle(targetDegree - getYaw()) < -IMU_Tolerance) {
      delay(1);
    }
  }

  Motor(0, 0);
}

// =============================================================================
//  TracDegreeSpeedTime - เดินตรงตามองศาและเวลา (ใช้ Gyro รักษาทิศทาง)
// =============================================================================

void TracDegreeSpeedTime(int speed, float targetDegree, int timeMs) {
  unsigned long startTime = millis();
  my.resetYaw();

  float integral = 0;
  float prevError = 0;
  float kp = 2.0;
  float ki = 0.001;
  float kd = 0.1;

  while ((millis() - startTime) < timeMs) {
    float yawError = normalizeAngle(targetDegree - getYaw());

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (kp * yawError) + (ki * integral) + (kd * derivative);

    int leftSpeed = constrain(speed + correction, 0, 100);
    int rightSpeed = constrain(speed - correction, 0, 100);

    Motor(leftSpeed, rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

void TracDegreeSpeedTimeBack(int speed, float targetDegree, int timeMs) {
  unsigned long startTime = millis();
  my.resetYaw();

  float integral = 0;
  float prevError = 0;
  float kp = 2.0;
  float ki = 0.001;
  float kd = 0.1;

  while ((millis() - startTime) < timeMs) {
    float yawError = normalizeAngle(targetDegree - getYaw());

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (kp * yawError) + (ki * integral) + (kd * derivative);

    int leftSpeed = constrain(speed - correction, 0, 100);
    int rightSpeed = constrain(speed + correction, 0, 100);

    Motor(-leftSpeed, -rightSpeed);
    delay(5);
  }

  Motor(0, 0);
}

// =============================================================================
//  TracJCDegree - เดินจนเจอแยกโดยรักษาองศา
// =============================================================================

void TracJCDegree(int speed, float targetDegree, int backTimeMs) {
  my.resetYaw();

  float integral = 0;
  float prevError = 0;
  float kp = 2.0;
  float ki = 0.001;
  float kd = 0.1;

  // เดินจนเจอเซ็นเซอร์กลางเห็นเส้น
  while (!checkCenterBlack()) {
    float yawError = normalizeAngle(targetDegree - getYaw());

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (kp * yawError) + (ki * integral) + (kd * derivative);

    int leftSpeed = constrain(speed + correction, 0, 100);
    int rightSpeed = constrain(speed - correction, 0, 100);

    Motor(leftSpeed, rightSpeed);
    delay(5);
  }

  Motor(0, 0);

  // ถอยกลับเล็กน้อยเพื่อตั้งตำแหน่ง
  if (backTimeMs > 0) {
    TracDegreeSpeedTimeBack(speed, targetDegree, backTimeMs);
  }
}

void TracJCDegreeBack(int speed, float targetDegree, int forwardTimeMs) {
  my.resetYaw();

  float integral = 0;
  float prevError = 0;
  float kp = 2.0;
  float ki = 0.001;
  float kd = 0.1;

  // ถอยจนเจอเซ็นเซอร์หลังเห็นเส้น
  int sL = read_sensorB(0);
  int sR = read_sensorB(7);
  int midL = md_sensorB(0);
  int midR = md_sensorB(7);

  while (!(sL < midL || sR < midR)) {
    float yawError = normalizeAngle(targetDegree - getYaw());

    integral += yawError;
    integral = constrain(integral, -500, 500);
    float derivative = yawError - prevError;
    prevError = yawError;

    float correction = (kp * yawError) + (ki * integral) + (kd * derivative);

    int leftSpeed = constrain(speed - correction, 0, 100);
    int rightSpeed = constrain(speed + correction, 0, 100);

    Motor(-leftSpeed, -rightSpeed);
    delay(5);

    sL = read_sensorB(0);
    sR = read_sensorB(7);
  }

  Motor(0, 0);

  // เดินหน้าเล็กน้อยเพื่อตั้งตำแหน่ง
  if (forwardTimeMs > 0) {
    TracDegreeSpeedTime(speed, targetDegree, forwardTimeMs);
  }
}

// =============================================================================
//  CurveDegree - โค้งตามองศา
// =============================================================================

void CurveLeftDegree(int speed, float degree) {
  my.resetYaw();

  // โค้งซ้าย (ล้อซ้ายช้ากว่าขวา)
  int innerSpeed = speed / 3;
  Motor(innerSpeed, speed);

  while (getYaw() < degree - IMU_Tolerance) {
    delay(1);
  }

  Motor(0, 0);
}

void CurveRightDegree(int speed, float degree) {
  my.resetYaw();

  // โค้งขวา (ล้อขวาช้ากว่าซ้าย)
  int innerSpeed = speed / 3;
  Motor(speed, innerSpeed);

  while (getYaw() > -degree + IMU_Tolerance) {
    delay(1);
  }

  Motor(0, 0);
}

void CurveLeftDegreeBack(int speed, float degree) {
  my.resetYaw();

  int innerSpeed = speed / 3;
  Motor(-innerSpeed, -speed);

  while (getYaw() > -degree + IMU_Tolerance) {
    delay(1);
  }

  Motor(0, 0);
}

void CurveRightDegreeBack(int speed, float degree) {
  my.resetYaw();

  int innerSpeed = speed / 3;
  Motor(-speed, -innerSpeed);

  while (getYaw() < degree - IMU_Tolerance) {
    delay(1);
  }

  Motor(0, 0);
}

// =============================================================================
//  ChangeSpeed - เปลี่ยนความเร็ว
// =============================================================================

void ChangeSpeed(int speed, int accSpeed) {
  TRAC_Speed = speed;
  TRAC_FastSpeed = accSpeed;
}

