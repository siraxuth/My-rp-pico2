/*
 * =============================================================================
 *  Initial.ino - ตั้งค่าเริ่มต้นหุ่นยนต์ (MyRP_Pico2 Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    InitRobot()              - ตั้งค่าเริ่มต้นทั้งหมด
 *    SetSpeedProfile(speed)   - ตั้งค่า PID ตามความเร็ว
 *    SetCustomPID(kp, kd)     - ตั้งค่า PID แบบกำหนดเอง
 *
 *  ตาราง PID ตามความเร็ว:
 *    Speed 20-30: kp=0.30, kd=0.015 (ช้า นุ่มนวล)
 *    Speed 31-40: kp=0.38, kd=0.020
 *    Speed 41-50: kp=0.45, kd=0.025 (ปานกลาง)
 *    Speed 51-60: kp=0.55, kd=0.035
 *    Speed 61-70: kp=0.65, kd=0.045
 *    Speed 71-80: kp=0.80, kd=0.060
 *    Speed 81-90: kp=0.95, kd=0.080
 *    Speed 91-100: kp=1.10, kd=0.100 (เร็ว ตอบสนองไว)
 *
 *  Implementation: ใช้ myrp_pico2.h API
 * =============================================================================
 */

// =============================================================================
//  ตาราง PID ตามความเร็ว (Speed Profile)
// =============================================================================

#define NUM_SPEED_PROFILES 8

struct SpeedPIDProfile {
  int minSpeed;
  int maxSpeed;
  float kp;
  float ki;
  float kd;
  int motorOffset;
};

SpeedPIDProfile speedProfiles[NUM_SPEED_PROFILES] = {
  {20, 30,   0.30, 0.0001, 0.015, 0},
  {31, 40,   0.38, 0.0001, 0.020, 1},
  {41, 50,   0.45, 0.0001, 0.025, 2},
  {51, 60,   0.55, 0.0001, 0.035, 3},
  {61, 70,   0.65, 0.0001, 0.045, 3},
  {71, 80,   0.80, 0.0002, 0.060, 4},
  {81, 90,   0.95, 0.0002, 0.080, 4},
  {91, 100,  1.10, 0.0003, 0.100, 5}
};

// =============================================================================
//  ตัวแปร Global สำหรับการตั้งค่า
// =============================================================================

float INIT_KP = 0.45;
float INIT_KI = 0.0001;
float INIT_KD = 0.025;

int INIT_Speed = 50;
int INIT_SlowSpeed = 25;
int INIT_FastSpeed = 70;

int INIT_MotorLeftOffset = 0;
int INIT_MotorRightOffset = 2;

int INIT_TurnSpeed = 50;
int INIT_BrakeSpeed = 80;
int INIT_BrakeTime = 25;

int INIT_Kt = 5;

// =============================================================================
//  SetSpeedProfile - ตั้งค่า PID อัตโนมัติตามความเร็ว
// =============================================================================

void SetSpeedProfile(int speed) {
  for (int i = 0; i < NUM_SPEED_PROFILES; i++) {
    if (speed >= speedProfiles[i].minSpeed && speed <= speedProfiles[i].maxSpeed) {
      INIT_KP = speedProfiles[i].kp;
      INIT_KI = speedProfiles[i].ki;
      INIT_KD = speedProfiles[i].kd;
      INIT_MotorRightOffset = speedProfiles[i].motorOffset;
      break;
    }
  }

  INIT_Speed = speed;
}

// =============================================================================
//  SetCustomPID - ตั้งค่า PID แบบกำหนดเอง
// =============================================================================

void SetCustomPID(float kp, float kd) {
  INIT_KP = kp;
  INIT_KD = kd;
}

void SetCustomPID(float kp, float ki, float kd) {
  INIT_KP = kp;
  INIT_KI = ki;
  INIT_KD = kd;
}

// =============================================================================
//  SetSpeedPID - ตั้งค่าความเร็วพร้อม PID กำหนดเอง
// =============================================================================

void SetSpeedPID(int speed, float kp, float kd) {
  SetSpeedProfile(speed);
  SetCustomPID(kp, kd);
}

void SetSpeedPID(int speed, float kp, float ki, float kd) {
  SetSpeedProfile(speed);
  SetCustomPID(kp, ki, kd);
}

// =============================================================================
//  SetAllSpeed - ตั้งค่าความเร็วทั้ง 3 โหมด
// =============================================================================

void SetAllSpeed(int normalSpeed, int slowSpeed, int fastSpeed) {
  INIT_Speed = normalSpeed;
  INIT_SlowSpeed = slowSpeed;
  INIT_FastSpeed = fastSpeed;
  SetSpeedProfile(normalSpeed);
}

// =============================================================================
//  SetTurnConfig - ตั้งค่าการเลี้ยว
// =============================================================================

void SetTurnConfig(int turnSpeed, int brakeSpeed, int brakeTime) {
  INIT_TurnSpeed = turnSpeed;
  INIT_BrakeSpeed = brakeSpeed;
  INIT_BrakeTime = brakeTime;
}

// =============================================================================
//  SetMotorOffsetConfig - ตั้งค่า Motor Offset
// =============================================================================

void SetMotorOffsetConfig(int leftOffset, int rightOffset) {
  INIT_MotorLeftOffset = leftOffset;
  INIT_MotorRightOffset = rightOffset;
}

// =============================================================================
//  AddPIDProfile - เพิ่มโปรไฟล์ PID ใหม่หรือแก้ไขที่มีอยู่
// =============================================================================

bool AddPIDProfile(int minSpeed, int maxSpeed, float kp, float ki, float kd, int offset) {
  for (int i = 0; i < NUM_SPEED_PROFILES; i++) {
    if (speedProfiles[i].minSpeed == minSpeed && speedProfiles[i].maxSpeed == maxSpeed) {
      speedProfiles[i].kp = kp;
      speedProfiles[i].ki = ki;
      speedProfiles[i].kd = kd;
      speedProfiles[i].motorOffset = offset;
      return true;
    }
  }
  return false;
}

// =============================================================================
//  UpdateProfilePID - อัพเดท PID ของโปรไฟล์โดยใช้ index
// =============================================================================

void UpdateProfilePID(int profileIndex, float kp, float ki, float kd) {
  if (profileIndex >= 0 && profileIndex < NUM_SPEED_PROFILES) {
    speedProfiles[profileIndex].kp = kp;
    speedProfiles[profileIndex].ki = ki;
    speedProfiles[profileIndex].kd = kd;
  }
}

void UpdateProfilePID(int profileIndex, float kp, float kd) {
  if (profileIndex >= 0 && profileIndex < NUM_SPEED_PROFILES) {
    speedProfiles[profileIndex].kp = kp;
    speedProfiles[profileIndex].kd = kd;
  }
}

// =============================================================================
//  GetPIDForSpeed - ดึงค่า PID สำหรับความเร็วที่กำหนด
// =============================================================================

void GetPIDForSpeed(int speed, float &kp, float &ki, float &kd) {
  for (int i = 0; i < NUM_SPEED_PROFILES; i++) {
    if (speed >= speedProfiles[i].minSpeed && speed <= speedProfiles[i].maxSpeed) {
      kp = speedProfiles[i].kp;
      ki = speedProfiles[i].ki;
      kd = speedProfiles[i].kd;
      return;
    }
  }
  kp = 0.45;
  ki = 0.0001;
  kd = 0.025;
}

// =============================================================================
//  InterpolatePID - คำนวณ PID แบบ Interpolate ระหว่างโปรไฟล์
// =============================================================================

void InterpolatePID(int speed, float &kp, float &ki, float &kd) {
  int lowerIdx = -1, upperIdx = -1;

  for (int i = 0; i < NUM_SPEED_PROFILES - 1; i++) {
    if (speed >= speedProfiles[i].minSpeed && speed <= speedProfiles[i].maxSpeed) {
      kp = speedProfiles[i].kp;
      ki = speedProfiles[i].ki;
      kd = speedProfiles[i].kd;
      return;
    }
    if (speed > speedProfiles[i].maxSpeed && speed < speedProfiles[i+1].minSpeed) {
      lowerIdx = i;
      upperIdx = i + 1;
      break;
    }
  }

  if (lowerIdx >= 0 && upperIdx >= 0) {
    float t = (float)(speed - speedProfiles[lowerIdx].maxSpeed) /
              (float)(speedProfiles[upperIdx].minSpeed - speedProfiles[lowerIdx].maxSpeed);

    kp = speedProfiles[lowerIdx].kp + t * (speedProfiles[upperIdx].kp - speedProfiles[lowerIdx].kp);
    ki = speedProfiles[lowerIdx].ki + t * (speedProfiles[upperIdx].ki - speedProfiles[lowerIdx].ki);
    kd = speedProfiles[lowerIdx].kd + t * (speedProfiles[upperIdx].kd - speedProfiles[lowerIdx].kd);
  } else {
    GetPIDForSpeed(speed, kp, ki, kd);
  }
}

// =============================================================================
//  SetSpeedProfileInterpolate - ตั้งค่า PID แบบ Interpolate
// =============================================================================

void SetSpeedProfileInterpolate(int speed) {
  float kp, ki, kd;
  InterpolatePID(speed, kp, ki, kd);

  INIT_KP = kp;
  INIT_KI = ki;
  INIT_KD = kd;
  INIT_Speed = speed;

  if (speed <= 30) INIT_MotorRightOffset = 0;
  else if (speed <= 50) INIT_MotorRightOffset = 2;
  else if (speed <= 70) INIT_MotorRightOffset = 3;
  else INIT_MotorRightOffset = 4;
}

// =============================================================================
//  InitRobot - ตั้งค่าเริ่มต้นทั้งหมด
// =============================================================================

void InitRobot() {
  SetAllSpeed(50, 25, 70);
  SetTurnConfig(50, 80, 25);
  INIT_Kt = 5;
}

void InitRobot(int speed) {
  InitRobot();
  SetSpeedProfile(speed);
}

void InitRobot(int speed, float kp, float kd) {
  InitRobot();
  SetSpeedPID(speed, kp, kd);
}

// =============================================================================
//  PrintPIDSettings - แสดงค่า PID ปัจจุบัน (Debug)
// =============================================================================

void PrintPIDSettings() {
  Serial.println("=== PID Settings ===");
  Serial.print("Speed: "); Serial.println(INIT_Speed);
  Serial.print("Kp: "); Serial.println(INIT_KP, 4);
  Serial.print("Ki: "); Serial.println(INIT_KI, 4);
  Serial.print("Kd: "); Serial.println(INIT_KD, 4);
  Serial.print("Motor Offset L/R: ");
  Serial.print(INIT_MotorLeftOffset); Serial.print("/");
  Serial.println(INIT_MotorRightOffset);
  Serial.println("====================");
}

void PrintAllProfiles() {
  Serial.println("=== Speed PID Profiles ===");
  for (int i = 0; i < NUM_SPEED_PROFILES; i++) {
    Serial.print("Profile "); Serial.print(i);
    Serial.print(": Speed "); Serial.print(speedProfiles[i].minSpeed);
    Serial.print("-"); Serial.print(speedProfiles[i].maxSpeed);
    Serial.print(" | Kp="); Serial.print(speedProfiles[i].kp, 2);
    Serial.print(" Ki="); Serial.print(speedProfiles[i].ki, 4);
    Serial.print(" Kd="); Serial.print(speedProfiles[i].kd, 3);
    Serial.print(" | Offset="); Serial.println(speedProfiles[i].motorOffset);
  }
  Serial.println("==========================");
}

// =============================================================================
//  Quick Setup Functions
// =============================================================================

void InitSlow() { InitRobot(30); }
void InitMedium() { InitRobot(50); }
void InitFast() { InitRobot(70); }
void InitTurbo() { InitRobot(90); }

// =============================================================================
//  Shorthand Commands
// =============================================================================

void SSP(int speed) { SetSpeedProfile(speed); }
void SSPI(int speed) { SetSpeedProfileInterpolate(speed); }
void SPID(float kp, float kd) { SetCustomPID(kp, kd); }
void SPID(float kp, float ki, float kd) { SetCustomPID(kp, ki, kd); }
void SAS(int normal, int slow, int fast) { SetAllSpeed(normal, slow, fast); }
void STC(int turnSpd, int brakeSpd, int brakeTime) { SetTurnConfig(turnSpd, brakeSpd, brakeTime); }
void SMO(int left, int right) { SetMotorOffsetConfig(left, right); }

