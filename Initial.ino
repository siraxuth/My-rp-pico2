/*
 * =============================================================================
 *  Initial.ino - ตั้งค่า PID หุ่นยนต์ 2 มอเตอร์ (Simplified Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    InitialSpeed()   - ตั้งค่า PID อัตโนมัติตาม BaseSpeed
 *    InitRobot()      - เรียก InitialSpeed()
 *
 *  ตาราง PID ตามความเร็ว (2 มอเตอร์):
 *    Speed <=50:  Kp=20, Kd=25,  KpB=12, KdB=60
 *    Speed <=60:  Kp=10, Kd=37,  KpB=15, KdB=80
 *    Speed <=70:  Kp=30, Kd=50,  KpB=16, KdB=100
 *    Speed <=80:  Kp=35, Kd=60,  KpB=20, KdB=120
 *    Speed <=90:  Kp=40, Kd=70,  KpB=25, KdB=140
 *    Speed >90:   Kp=45, Kd=80,  KpB=30, KdB=160
 *
 *  หมายเหตุ: ค่า Reference เซ็นเซอร์ใช้จาก md_sensorA() ที่ calibrate แล้ว
 *
 * =============================================================================
 */

// =============================================================================
//  ตัวแปร Global สำหรับความเร็วและ PID
// =============================================================================

// PID เดินหน้า
float Kp = 0.65;                 // Proportional
float Ki = 0;                    // Integral
float Kd = 0.55;                 // Derivative
float Kt = 10;                   // Turn gain

// PID ถอยหลัง
float KpB = 12;               // Proportional (Backward)
float KiB = 0;                // Integral (Backward)
float KdB = 0.025;            // Derivative (Backward)
float KtB = 10;               // Turn gain (Backward)



// =============================================================================
//  InitialSpeed - ตั้งค่า PID อัตโนมัติตาม BaseSpeed (2 มอเตอร์)
// =============================================================================

void InitialSpeed() {
  MaxSpeed = 100;
  Ki = 0;
  KiB = 0;
  
  // Reset error ถ้า ErrorClear = 0
  if (ErrorClear == 0) {
    Error = 0;
    PreError = 0;
  }

  // ตั้งค่า PID ตามช่วงความเร็ว (Normalized to 0.x format)
  if (BaseSpeed <= 50)
  {
    LeftBaseSpeed = BaseSpeed -4;
    RightBaseSpeed = BaseSpeed;
    Kp = 0.65;   // Forward PID (ใช้เท่าเดิม - ค่าต่ำ)
    Kd = 0.75;   // ใช้เท่าเดิม
    Kt = 0.10;   // 10 / 100
    KpB = 0.13;  // 1.3 / 10 - Backward PID
    KdB = 0.025; // ใช้เท่าเดิม
    KtB = 0.10;  // 10 / 100
  }
  else if (BaseSpeed <= 60)
  {
    LeftBaseSpeed = BaseSpeed;
    RightBaseSpeed = BaseSpeed;
    Kp = 1.00;  // 10 / 10 (หารด้วย 10)
    Kd = 3.70;  // 37 / 10
    Kt = 0.10;  // 10 / 100
    KpB = 1.50; // 15 / 10
    KdB = 8.00; // 80 / 10
    KtB = 0.10;
  }
  else if (BaseSpeed <= 70)
  {
    LeftBaseSpeed = BaseSpeed;
    RightBaseSpeed = BaseSpeed;
    Kp = 3.00; // 30 / 10
    Kd = 5.00; // 50 / 10
    Kt = 0.10;
    KpB = 1.60;  // 16 / 10
    KdB = 10.00; // 100 / 10
    KtB = 0.10;
  }
  else if (BaseSpeed <= 80)
  {
    LeftBaseSpeed = BaseSpeed;
    RightBaseSpeed = BaseSpeed;
    Kp = 3.50; // 35 / 10
    Kd = 6.00; // 60 / 10
    Kt = 0.10;
    KpB = 2.00;  // 20 / 10
    KdB = 12.00; // 120 / 10
    KtB = 0.10;
  }
  else if (BaseSpeed <= 90)
  {
    LeftBaseSpeed = BaseSpeed;
    RightBaseSpeed = BaseSpeed;
    Kp = 4.00; // 40 / 10
    Kd = 7.00; // 70 / 10
    Kt = 0.10;
    KpB = 2.50;  // 25 / 10
    KdB = 14.00; // 140 / 10
    KtB = 0.10;
  }
  else
  {
    // Speed > 90
    LeftBaseSpeed = BaseSpeed;
    RightBaseSpeed = BaseSpeed;
    Kp = 4.50; // 45 / 10
    Kd = 8.00; // 80 / 10
    Kt = 0.10;
    KpB = 3.00;  // 30 / 10
    KdB = 16.00; // 160 / 10
    KtB = 0.15;  // 15 / 100
  }
}

// =============================================================================
//  InitRobot - เรียก InitialSpeed
// =============================================================================

void InitRobot() {
  InitialSpeed();
}

void InitRobot(int speed) {
  BaseSpeed = speed;
  InitialSpeed();
}

// =============================================================================
//  SetBaseSpeed - ตั้งความเร็วและอัพเดท PID
// =============================================================================

void SetBaseSpeed(int speed) {
  BaseSpeed = speed;
  InitialSpeed();
}

// =============================================================================
//  SetPID - ตั้งค่า PID แบบ manual
// =============================================================================

void SetPID(float kp, float kd) {
  Kp = kp;
  Kd = kd;
}

void SetPID(float kp, float ki, float kd) {
  Kp = kp;
  Ki = ki;
  Kd = kd;
}

void SetPID_Backward(float kp, float kd) {
  KpB = kp;
  KdB = kd;
}

void SetPID_Backward(float kp, float ki, float kd) {
  KpB = kp;
  KiB = ki;
  KdB = kd;
}

// =============================================================================
//  SetMotorOffset - ปรับ offset มอเตอร์ซ้าย/ขวา
// =============================================================================

void SetMotorOffset(int leftOffset, int rightOffset) {
  LeftBaseSpeed = BaseSpeed + leftOffset;
  RightBaseSpeed = BaseSpeed + rightOffset;
}

// =============================================================================
//  Quick Setup - ตั้งค่าเร็ว
// =============================================================================

void InitSlow()   { InitRobot(30); }
void InitMedium() { InitRobot(50); }
void InitFast()   { InitRobot(70); }
void InitTurbo()  { InitRobot(90); }

// =============================================================================
//  Print Settings - แสดงค่าปัจจุบัน (Debug)
// =============================================================================

void PrintPIDSettings() {
  Serial.println("===== PID Settings =====");
  Serial.print("BaseSpeed: "); Serial.println(BaseSpeed);
  Serial.print("Left/Right: "); Serial.print(LeftBaseSpeed); 
  Serial.print("/"); Serial.println(RightBaseSpeed);
  Serial.println("--- Forward ---");
  Serial.print("Kp: "); Serial.print(Kp);
  Serial.print("  Ki: "); Serial.print(Ki);
  Serial.print("  Kd: "); Serial.print(Kd);
  Serial.print("  Kt: "); Serial.println(Kt);
  Serial.println("--- Backward ---");
  Serial.print("KpB: "); Serial.print(KpB);
  Serial.print("  KiB: "); Serial.print(KiB);
  Serial.print("  KdB: "); Serial.print(KdB);
  Serial.print("  KtB: "); Serial.println(KtB);
  Serial.println("========================");
}

// =============================================================================
//  Shorthand Commands
// =============================================================================

void SBS(int speed) { SetBaseSpeed(speed); }  // Set Base Speed
void SP(float kp, float kd) { SetPID(kp, kd); }  // Set PID
void SPB(float kp, float kd) { SetPID_Backward(kp, kd); }  // Set PID Backward
