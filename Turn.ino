/*
 * =============================================================================
 *  Turn.ino - เลี้ยวหุ่นยนต์ (MyRP_Pico2 Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    TurnLeft() / TurnRight()   - เลี้ยวซ้าย/ขวา (หมุนกลางตัว)
 *    UTurnLeft() / UTurnRight() - กลับตัวซ้าย/ขวา 180 องศา
 *    Left() / Right()           - หมุนซ้าย/ขวาตามเวลา
 *    CurveLeft() / CurveRight() - โค้งซ้าย/ขวา
 *
 *  Implementation: ใช้ myrp_pico2.h API
 * =============================================================================
 */

// =============================================================================
//  ค่า Config สำหรับการเลี้ยว (ใช้จาก myrp_pico2.h)
// =============================================================================

int TURN_Speed = 70;
int TURN_BrakeSpeed = 100;
int TURN_BrakeTime = 30;

// =============================================================================
//  SetTurnSpeed - ตั้งค่าความเร็วเลี้ยว
// =============================================================================

void SetTurnSpeed(int speed) {
  TURN_Speed = speed;
}

void SetTurnBrake(int speed, int time) {
  TURN_BrakeSpeed = speed;
  TURN_BrakeTime = time;
}

// =============================================================================
//  checkCenterSensor - ตรวจสอบเซ็นเซอร์กลาง
// =============================================================================

bool checkCenterBlack() {
  int sC = read_sensorA(3);  // เซ็นเซอร์กลาง
  int midC = md_sensorA(3);
  return (sC < midC);
}

bool checkLeftSensor(int idx) {
  int s = read_sensorA(idx);
  int mid = md_sensorA(idx);
  return (s < mid);
}

bool checkRightSensor(int idx) {
  int s = read_sensorA(idx);
  int mid = md_sensorA(idx);
  return (s < mid);
}

// =============================================================================
//  SpinLeft / SpinRight - หมุนตัว (ล้อซ้ายและขวาหมุนสวนทาง)
// =============================================================================

void SpinLeft(int speed) {
  Motor(-speed, speed);
}

void SpinRight(int speed) {
  Motor(speed, -speed);
}

// =============================================================================
//  TurnLeftSpeed - เลี้ยวซ้ายด้วยความเร็วที่กำหนด
// =============================================================================

void TurnLeftSpeed(int speed) {
  Motor(0, 0);
  SpinLeft(speed);
  delay(7000 / speed);

  // ลดความเร็วถ้าเร็วเกิน
  if (speed > 90) {
    SpinLeft(90);
  }

  // รอจนเซ็นเซอร์เจอเส้น
  if (speed <= 40) {
    while (!checkCenterBlack()) { delay(1); }
  } else if (speed <= 50) {
    while (!checkLeftSensor(2)) { delay(1); }
  } else if (speed <= 60) {
    while (!checkLeftSensor(1)) { delay(1); }
  } else {
    while (!checkLeftSensor(0)) { delay(1); }
  }

  // Brake
  if (TURN_BrakeTime > 0 && speed > 70) {
    SpinRight(TURN_BrakeSpeed);
    delay(TURN_BrakeTime);
  }

  Motor(0, 0);
}

void TurnLeft() {
  TurnLeftSpeed(TURN_Speed);
}

// =============================================================================
//  TurnRightSpeed - เลี้ยวขวาด้วยความเร็วที่กำหนด
// =============================================================================

void TurnRightSpeed(int speed) {
  Motor(0, 0);
  SpinRight(speed);
  delay(7000 / speed);

  // ลดความเร็วถ้าเร็วเกิน
  if (speed > 90) {
    SpinRight(90);
  }

  // รอจนเซ็นเซอร์เจอเส้น
  if (speed <= 40) {
    while (!checkCenterBlack()) { delay(1); }
  } else if (speed <= 50) {
    while (!checkRightSensor(5)) { delay(1); }
  } else if (speed <= 60) {
    while (!checkRightSensor(6)) { delay(1); }
  } else {
    while (!checkRightSensor(7)) { delay(1); }
  }

  // Brake
  if (TURN_BrakeTime > 0 && speed > 70) {
    SpinLeft(TURN_BrakeSpeed);
    delay(TURN_BrakeTime);
  }

  Motor(0, 0);
}

void TurnRight() {
  TurnRightSpeed(TURN_Speed);
}

// =============================================================================
//  UTurnLeft / UTurnRight - กลับตัว 180 องศา
// =============================================================================

void UTurnLeftSpeed(int speed) {
  Motor(0, 0);
  SpinLeft(speed);
  delay(20000 / speed);  // หมุนนานกว่าเลี้ยวปกติ

  // ลดความเร็ว
  if (speed > 70) {
    SpinLeft(60);
  }

  // รอจนเซ็นเซอร์เจอเส้น
  if (speed <= 40) {
    while (!checkCenterBlack()) { delay(1); }
  } else if (speed <= 50) {
    while (!checkLeftSensor(2)) { delay(1); }
  } else if (speed <= 60) {
    while (!checkLeftSensor(1)) { delay(1); }
  } else {
    while (!checkLeftSensor(0)) { delay(1); }
  }

  // Brake
  if (TURN_BrakeTime > 0 && speed > 70) {
    SpinRight(TURN_BrakeSpeed);
    delay(TURN_BrakeTime);
  }

  Motor(0, 0);
}

void UTurnLeft() {
  UTurnLeftSpeed(TURN_Speed);
}

void UTurnRightSpeed(int speed) {
  Motor(0, 0);
  SpinRight(speed);
  delay(20000 / speed);  // หมุนนานกว่าเลี้ยวปกติ

  // ลดความเร็ว
  if (speed > 70) {
    SpinRight(60);
  }

  // รอจนเซ็นเซอร์เจอเส้น
  if (speed <= 40) {
    while (!checkCenterBlack()) { delay(1); }
  } else if (speed <= 50) {
    while (!checkRightSensor(5)) { delay(1); }
  } else if (speed <= 60) {
    while (!checkRightSensor(6)) { delay(1); }
  } else {
    while (!checkRightSensor(7)) { delay(1); }
  }

  // Brake
  if (TURN_BrakeTime > 0 && speed > 70) {
    SpinLeft(TURN_BrakeSpeed);
    delay(TURN_BrakeTime);
  }

  Motor(0, 0);
}

void UTurnRight() {
  UTurnRightSpeed(TURN_Speed);
}

// =============================================================================
//  Left / Right - หมุนตามเวลา
// =============================================================================

void Left(int speed, int timeMs) {
  Motor(0, 0);
  SpinLeft(speed);
  delay(timeMs);
  Motor(0, 0);
}

void Right(int speed, int timeMs) {
  Motor(0, 0);
  SpinRight(speed);
  delay(timeMs);
  Motor(0, 0);
}

// =============================================================================
//  TurnLeftDelay / TurnRightDelay - เลี้ยวแบบโค้ง (ล้อข้างนึงหมุน)
// =============================================================================

void TurnLeftDelay(int speed, int timeMs) {
  Motor(flml, flmr);  // ใช้ค่า front left motor จาก myrp_pico2.h
  delay(timeMs);
}

void TurnRightDelay(int speed, int timeMs) {
  Motor(frml, frmr);  // ใช้ค่า front right motor จาก myrp_pico2.h
  delay(timeMs);
}

// =============================================================================
//  CurveLeft / CurveRight - โค้งอ้อมสิ่งกีดขวาง
// =============================================================================

void CurveLeft(int speed, int timeMs) {
  Motor(0, 0);
  Left(speed, 130);

  // เดินโค้ง
  int innerSpeed = (int)(speed * 0.58);
  Motor(speed, innerSpeed);
  delay(900);

  // รอจนเจอเส้น
  while (checkRightSensor(4)) { delay(1); }  // รอหลุดเส้น
  while (!checkRightSensor(4)) { delay(1); }  // รอเจอเส้น

  delay(timeMs);
  TurnLeft();
}

void CurveRight(int speed, int timeMs) {
  Motor(0, 0);
  Right(speed, 130);

  // เดินโค้ง
  int innerSpeed = (int)(speed * 0.58);
  Motor(innerSpeed, speed);
  delay(900);

  // รอจนเจอเส้น
  while (checkLeftSensor(3)) { delay(1); }  // รอหลุดเส้น
  while (!checkLeftSensor(3)) { delay(1); }  // รอเจอเส้น

  delay(timeMs);
  TurnRight();
}

