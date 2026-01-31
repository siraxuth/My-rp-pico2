/*
 * =============================================================================
 *  TracJC_New.ino - ระบบเดินตามเส้นแบบใหม่ (ใช้งานง่าย)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    TracJC()      - เดินจนเจอแยก 4 ทาง (a0 และ a7)
 *    TracJCL()     - เดินจนเจอแยกซ้าย (a0)
 *    TracJCR()     - เดินจนเจอแยกขวา (a7)
 *    TracJCC()     - เดินจนเจอเส้นกลาง (analog 26, 27)
 *    TracTime()    - เดินตามเวลา
 *    TracPID()     - เดินตามเส้นด้วย PID
 *    TracSpeedTime() - เดินด้วยความเร็วและเวลาที่กำหนด
 *
 * =============================================================================
 */

// =============================================================================
//  ตัวแปร Global สำหรับ TracPID
// =============================================================================

float SumError = 0;          // สำหรับ Integral
int JCStatus = 0;            // 0=ตรวจจับแยก, 1=ไม่ตรวจจับแยก
unsigned long TracTimer = 0; // Timer สำหรับ TracSpeedTime

// =============================================================================
//  TracSpeedTime - เดินตามเส้นด้วยความเร็วและเวลาที่กำหนด
// =============================================================================

void TracSpeedTime(int TracSpeed, int TracTime, my_GYRO1600 &gyro)
{
  int TempSpeed = BaseSpeed;
  JCStatus = 1;
  TracTimer = millis();

  // Reset PID terms
  I = 0;
  previous_error = 0;

  BaseSpeed = TracSpeed;
  InitialSpeed();

  while (1)
  {
    // ตรวจจับเส้นข้างซ้าย (sensor 0) และเซ็นเซอร์กลางไม่เจอเส้น
    if (read_sensorA(0) < md_sensorA(0) &&
        read_sensorA(2) > md_sensorA(2) &&
        read_sensorA(3) > md_sensorA(3) &&
        read_sensorA(4) > md_sensorA(4) &&
        read_sensorA(5) > md_sensorA(5))
    {
      // หมุนซ้ายกลับมาหาเส้น
      while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
      {
        Motor(-TracSpeed * 0.5, TracSpeed * 0.5);
        delayMicroseconds(50);
      }
      I = 0;
      previous_error = 0;
    }

    // ตรวจจับเส้นข้างขวา (sensor 7) และเซ็นเซอร์กลางไม่เจอเส้น
    else if (read_sensorA(7) < md_sensorA(7) &&
             read_sensorA(2) > md_sensorA(2) &&
             read_sensorA(3) > md_sensorA(3) &&
             read_sensorA(4) > md_sensorA(4) &&
             read_sensorA(5) > md_sensorA(5))
    {
      // หมุนขวากลับมาหาเส้น
      while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
      {
        Motor(TracSpeed * 0.5, -TracSpeed * 0.5);
        delayMicroseconds(50);
      }
      I = 0;
      previous_error = 0;
    }

    // PID ปกติ
    if (Kp <= 0.65)
    {
      if (read_sensorA(2) > md_sensorA(2) && read_sensorA(3) > md_sensorA(3) &&
          read_sensorA(4) > md_sensorA(4) && read_sensorA(5) > md_sensorA(5))
      {
        errors = 0;
      }
      else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
      {
        errors = 10;
      }
      else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
      {
        errors = -10;
      }
      else
      {
        errors = error_4sensor();
      }
    }
    else
    {
      errors = error_4sensor();
    }

    P = errors;
    I += errors * 0.00005;
    D = errors - previous_error;
    previous_error = errors;
    PID_output = (Kp * P) + (0.000001 * I) + (kd_f * D);
    Motor(LeftBaseSpeed - PID_output, RightBaseSpeed + PID_output);
    delayMicroseconds(50);

    // ครบเวลา = หยุดและปรับมุม
    if (millis() - TracTimer >= TracTime)
    {
      break;
    }
  }

  // ========== Auto-Align ด้วย Gyro ==========

  // เบรคเบาๆ
  Motor(-15, -15);
  delay(30);
  Motor(0, 0);
  delay(50);
  my.resetAngles();
  my.reCalibrateGyro();
  // my.resetRunningBias();
  my.resetAngles();
  // อ่านมุมปัจจุบัน
  float roll, pitch, yaw;
  gyro.readAngles(roll, pitch, yaw);

  // ถ้ามุมเอียงเกิน threshold ให้แก้
  const float ANGLE_THRESHOLD = 2.0; // องศา (ปรับได้)
  const float KP_GYRO = 1.2;         // gain สำหรับ correction

  if (abs(yaw) > ANGLE_THRESHOLD)
  {
    unsigned long align_start = millis();
    const unsigned long MAX_ALIGN_TIME = 500; // timeout 500ms

    while (abs(yaw) > 0.5 && (millis() - align_start < MAX_ALIGN_TIME))
    {
      gyro.readAngles(roll, pitch, yaw);

      // คำนวณ correction
      float correction = yaw * KP_GYRO;

      // จำกัด correction
      if (correction > 30)
        correction = 30;
      if (correction < -30)
        correction = -30;

      // หมุนแก้มุม (yaw > 0 = เอียงขวา, ต้องหมุนซ้าย)
      if (yaw > 0)
      {
        Motor(-correction, correction);
      }
      else
      {
        Motor(-correction, correction);
      }

      delayMicroseconds(50);
    }
  }

  // หยุดสนิท
  Motor(0, 0);
  delay(10);

  BaseSpeed = TempSpeed;
  InitialSpeed();
  JCStatus = 0;
}

void TracSpeedTime(int TracSpeed, int TracTime, my_GYRO1600 &gyro, float targetAngle)
{
  my.resetAngles();
  my.reCalibrateGyro();
  my.resetAngles();
  int TempSpeed = BaseSpeed;
  JCStatus = 1;
  TracTimer = millis();

  // Reset PID terms
  I = 0;
  previous_error = 0;

  BaseSpeed = TracSpeed;
  InitialSpeed();

  while (1)
  {
    // ตรวจจับเส้นข้างซ้าย (sensor 0) และเซ็นเซอร์กลางไม่เจอเส้น
    if (read_sensorA(0) < md_sensorA(0) &&
        read_sensorA(2) > md_sensorA(2) &&
        read_sensorA(3) > md_sensorA(3) &&
        read_sensorA(4) > md_sensorA(4) &&
        read_sensorA(5) > md_sensorA(5))
    {
      // หมุนซ้ายกลับมาหาเส้น
      while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
      {
        Motor(-TracSpeed * 0.5, TracSpeed * 0.5);
        delayMicroseconds(50);
      }
      I = 0;
      previous_error = 0;
    }

    // ตรวจจับเส้นข้างขวา (sensor 7) และเซ็นเซอร์กลางไม่เจอเส้น
    else if (read_sensorA(7) < md_sensorA(7) &&
             read_sensorA(2) > md_sensorA(2) &&
             read_sensorA(3) > md_sensorA(3) &&
             read_sensorA(4) > md_sensorA(4) &&
             read_sensorA(5) > md_sensorA(5))
    {
      // หมุนขวากลับมาหาเส้น
      while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
      {
        Motor(TracSpeed * 0.5, -TracSpeed * 0.5);
        delayMicroseconds(50);
      }
      I = 0;
      previous_error = 0;
    }

    // PID ปกติ
    if (Kp <= 0.65)
    {
      if (read_sensorA(2) > md_sensorA(2) && read_sensorA(3) > md_sensorA(3) &&
          read_sensorA(4) > md_sensorA(4) && read_sensorA(5) > md_sensorA(5))
      {
        errors = 0;
      }
      else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
      {
        errors = 10;
      }
      else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
      {
        errors = -10;
      }
      else
      {
        errors = error_4sensor();
      }
    }
    else
    {
      errors = error_4sensor();
    }

    P = errors;
    I += errors * 0.00005;
    D = errors - previous_error;
    previous_error = errors;
    PID_output = (Kp * P) + (0.000001 * I) + (kd_f * D);
    Motor(LeftBaseSpeed - PID_output, RightBaseSpeed + PID_output);
    delayMicroseconds(50);

    // ครบเวลา = หยุดและปรับมุม
    if (millis() - TracTimer >= TracTime)
    {
      break;
    }
  }

  // ========== Auto-Align ด้วย Gyro ไปยังมุมที่กำหนด ==========

  // เบรคเบาๆ
  Motor(-15, -15);
  delay(30);
  Motor(0, 0);
  delay(50);

  // ========== รีเซ็ตมุมเป็น 0 องศา ==========
  gyro.resetAngles(); // รีเซ็ตมุมให้เป็น 0
  delay(50);          // รอให้ gyro อัพเดท
  // =========================================

  // อ่านมุมปัจจุบัน (ควรเป็น 0 หรือใกล้ๆ 0)
  float roll, pitch, yaw;
  gyro.readAngles(roll, pitch, yaw);

  // คำนวณ error จากมุมเป้าหมาย
  // ตอนนี้ yaw ≈ 0 แล้ว ดังนั้น angleError ≈ targetAngle
  float angleError = targetAngle - yaw;

  // Normalize angle to -180 to +180
  if (angleError > 180)
    angleError -= 360;
  if (angleError < -180)
    angleError += 360;

  const float ANGLE_THRESHOLD = 1.5;
  const float DEAD_ZONE = 3.0;
  const unsigned long MAX_ALIGN_TIME = 500;

  if (abs(angleError) > ANGLE_THRESHOLD)
  {
    unsigned long align_start = millis();

    while (abs(angleError) > ANGLE_THRESHOLD && (millis() - align_start < MAX_ALIGN_TIME))
    {
      gyro.readAngles(roll, pitch, yaw);
      angleError = targetAngle - yaw;

      // Normalize angle
      if (angleError > 180)
        angleError -= 360;
      if (angleError < -180)
        angleError += 360;

      // ถ้าใกล้มาก ลดความเร็ว
      float KP = (abs(angleError) < DEAD_ZONE) ? 0.4 : 1.0;
      float correction = angleError * KP;

      // จำกัด correction
      if (correction > 30)
        correction = 30;
      if (correction < -30)
        correction = -30;

      // หมุนแก้มุม
      Motor(-correction, correction);
      delay(10);
    }
  }

  // หยุดสนิท
  Motor(0, 0);
  delay(10);

  BaseSpeed = TempSpeed;
  InitialSpeed();
  JCStatus = 0;
}



void TracSpeedDistance(int TracSpeed, float distance_cm, my_GYRO1600 &gyro)
  {
    int TempSpeed = BaseSpeed;
    JCStatus = 1;

    float traveled_distance = 0;
    unsigned long last_time = millis();

    // ⭐ ค่า speed_scale สำหรับ line tracking (ต้อง calibrate)
    float speed_scale = 1.5; // ปรับตามหุ่นของคุณ

    // Reset PID terms
    I = 0;
    previous_error = 0;

    BaseSpeed = TracSpeed;
    InitialSpeed();

    while (1)
    {
      // ตรวจจับเส้นข้างซ้าย (sensor 0) และเซ็นเซอร์กลางไม่เจอเส้น
      if (read_sensorA(0) < md_sensorA(0) &&
          read_sensorA(2) > md_sensorA(2) &&
          read_sensorA(3) > md_sensorA(3) &&
          read_sensorA(4) > md_sensorA(4) &&
          read_sensorA(5) > md_sensorA(5))
      {
        // หมุนซ้ายกลับมาหาเส้น
        while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
        {
          Motor(-TracSpeed * 0.5, TracSpeed * 0.5);
          delayMicroseconds(50);
        }
        I = 0;
        previous_error = 0;
      }

      // ตรวจจับเส้นข้างขวา (sensor 7) และเซ็นเซอร์กลางไม่เจอเส้น
      else if (read_sensorA(7) < md_sensorA(7) &&
               read_sensorA(2) > md_sensorA(2) &&
               read_sensorA(3) > md_sensorA(3) &&
               read_sensorA(4) > md_sensorA(4) &&
               read_sensorA(5) > md_sensorA(5))
      {
        // หมุนขวากลับมาหาเส้น
        while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
        {
          Motor(TracSpeed * 0.5, -TracSpeed * 0.5);
          delayMicroseconds(50);
        }
        I = 0;
        previous_error = 0;
      }

      // PID ปกติ
      if (Kp <= 0.65)
      {
        if (read_sensorA(2) > md_sensorA(2) && read_sensorA(3) > md_sensorA(3) &&
            read_sensorA(4) > md_sensorA(4) && read_sensorA(5) > md_sensorA(5))
        {
          errors = 0;
        }
        else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
        {
          errors = 10;
        }
        else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
        {
          errors = -10;
        }
        else
        {
          errors = error_4sensor();
        }
      }
      else
      {
        errors = error_4sensor();
      }

      P = errors;
      I += errors * 0.00005;
      D = errors - previous_error;
      previous_error = errors;
      PID_output = (Kp * P) + (0.000001 * I) + (kd_f * D);
      Motor(LeftBaseSpeed - PID_output, RightBaseSpeed + PID_output);
      delayMicroseconds(50);

      // ⭐⭐⭐ คำนวณระยะทางที่เดินไปแล้ว ⭐⭐⭐
      unsigned long current_time = millis();
      float delta_time = (current_time - last_time) / 1000.0; // แปลงเป็นวินาที
      traveled_distance += (TracSpeed * speed_scale) * delta_time;
      last_time = current_time;

      // เช็คว่าเดินครบระยะทางแล้วหรือยัง
      if (traveled_distance >= distance_cm)
      {
        break;
      }
    }

    // ========== Auto-Align ด้วย Gyro ==========

    // เบรคเบาๆ
    Motor(-15, -15);
    delay(30);
    Motor(0, 0);
    delay(50);
    my.resetAngles();
    my.reCalibrateGyro();
    // my.resetRunningBias();
    my.resetAngles();
    // อ่านมุมปัจจุบัน
    float roll, pitch, yaw;
    gyro.readAngles(roll, pitch, yaw);

    // ถ้ามุมเอียงเกิน threshold ให้แก้
    const float ANGLE_THRESHOLD = 2.0; // องศา (ปรับได้)
    const float KP_GYRO = 1.2;         // gain สำหรับ correction

    if (abs(yaw) > ANGLE_THRESHOLD)
    {
      unsigned long align_start = millis();
      const unsigned long MAX_ALIGN_TIME = 500; // timeout 500ms

      while (abs(yaw) > 0.5 && (millis() - align_start < MAX_ALIGN_TIME))
      {
        gyro.readAngles(roll, pitch, yaw);

        // คำนวณ correction
        float correction = yaw * KP_GYRO;

        // จำกัด correction
        if (correction > 30)
          correction = 30;
        if (correction < -30)
          correction = -30;

        // หมุนแก้มุม
        Motor(-correction, correction);

        delayMicroseconds(50);
      }
    }

    // หยุดสนิท
    Motor(0, 0);
    delay(10);

    BaseSpeed = TempSpeed;
    InitialSpeed();
    JCStatus = 0;
  }




void TracSpeedDistances(int TracSpeed, float distance_cm)
{
  int TempSpeed = BaseSpeed;
  JCStatus = 1;

  float traveled_distance = 0;
  unsigned long last_time = millis();

  // ⭐ ค่า speed_scale สำหรับ line tracking (ต้อง calibrate)
  float speed_scale = 1.5; // ปรับตามหุ่นของคุณ

  // Reset PID terms
  I = 0;
  previous_error = 0;

  // เก็บค่า error สะสมเพื่อตรวจจับการเบี่ยงเบน
  float cumulative_error = 0;
  int error_count = 0;

  BaseSpeed = TracSpeed;
  InitialSpeed();

  while (1)
  {
    // ตรวจจับเส้นข้างซ้าย (sensor 0) และเซ็นเซอร์กลางไม่เจอเส้น
    if (read_sensorA(0) < md_sensorA(0) &&
        read_sensorA(2) > md_sensorA(2) &&
        read_sensorA(3) > md_sensorA(3) &&
        read_sensorA(4) > md_sensorA(4) &&
        read_sensorA(5) > md_sensorA(5))
    {
      // หมุนซ้ายกลับมาหาเส้น
      while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
      {
        Motor(-TracSpeed * 0.5, TracSpeed * 0.5);
        delayMicroseconds(50);
      }
      I = 0;
      previous_error = 0;
    }

    // ตรวจจับเส้นข้างขวา (sensor 7) และเซ็นเซอร์กลางไม่เจอเส้น
    else if (read_sensorA(7) < md_sensorA(7) &&
             read_sensorA(2) > md_sensorA(2) &&
             read_sensorA(3) > md_sensorA(3) &&
             read_sensorA(4) > md_sensorA(4) &&
             read_sensorA(5) > md_sensorA(5))
    {
      // หมุนขวากลับมาหาเส้น
      while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
      {
        Motor(TracSpeed * 0.5, -TracSpeed * 0.5);
        delayMicroseconds(50);
      }
      I = 0;
      previous_error = 0;
    }

    // PID ปกติ
    if (Kp <= 0.65)
    {
      if (read_sensorA(2) > md_sensorA(2) && read_sensorA(3) > md_sensorA(3) &&
          read_sensorA(4) > md_sensorA(4) && read_sensorA(5) > md_sensorA(5))
      {
        errors = 0;
      }
      else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
      {
        errors = 10;
      }
      else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
      {
        errors = -10;
      }
      else
      {
        errors = error_4sensor();
      }
    }
    else
    {
      errors = error_4sensor();
    }

    // เก็บค่า error สะสมเพื่อดูแนวโน้มการเบี่ยงเบน
    cumulative_error += errors;
    error_count++;

    P = errors;
    I += errors * 0.00005;
    D = errors - previous_error;
    previous_error = errors;
    PID_output = (Kp * P) + (0.000001 * I) + (kd_f * D);
    Motor(LeftBaseSpeed - PID_output, RightBaseSpeed + PID_output);
    delayMicroseconds(50);

    // ⭐⭐⭐ คำนวณระยะทางที่เดินไปแล้ว ⭐⭐⭐
    unsigned long current_time = millis();
    float delta_time = (current_time - last_time) / 1000.0; // แปลงเป็นวินาที
    traveled_distance += (TracSpeed * speed_scale) * delta_time;
    last_time = current_time;

    // เช็คว่าเดินครบระยะทางแล้วหรือยัง
    if (traveled_distance >= distance_cm)
    {
      break;
    }
  }

  // ========== Auto-Align ด้วยการวิเคราะห์ error สะสม ==========

  // เบรคเบาๆ
  Motor(-15, -15);
  delay(30);
  Motor(0, 0);
  delay(50);

  // คำนวณ error เฉลี่ย
  float avg_error = (error_count > 0) ? (cumulative_error / error_count) : 0;

  // ถ้า error เฉลี่ยมีค่ามาก แสดงว่าหุ่นเบี่ยงเบนไปทางใดทางหนึ่ง
  const float ERROR_THRESHOLD = 1.0; // ปรับค่าตามการทดสอบ
  const float CORRECTION_SPEED = 20; // ความเร็วในการแก้มุม

  if (abs(avg_error) > ERROR_THRESHOLD)
  {
    unsigned long align_start = millis();
    const unsigned long MAX_ALIGN_TIME = 300; // timeout 300ms

    // ใช้ค่า error เฉลี่ยเป็นทิศทางในการแก้
    int correction_direction = (avg_error > 0) ? 1 : -1;

    // หมุนเล็กน้อยเพื่อแก้มุม
    while (millis() - align_start < MAX_ALIGN_TIME)
    {
      Motor(-CORRECTION_SPEED * correction_direction,
            CORRECTION_SPEED * correction_direction);
      delayMicroseconds(50);

      // ตรวจสอบว่าเซ็นเซอร์กลางอยู่บนเส้นหรือไม่
      if (read_sensorA(3) < md_sensorA(3) || read_sensorA(4) < md_sensorA(4))
      {
        break;
      }
    }
  }

  // หยุดสนิท
  Motor(0, 0);
  delay(10);

  BaseSpeed = TempSpeed;
  InitialSpeed();
  JCStatus = 0;
}
void TracSpeedTime(int TracTime)
{
  InitialSpeed();
  TracSpeedTime(BaseSpeed,TracTime, my);
  // TracSpeedTime(BaseSpeed, BaseSpeed, Kp, TracTime);
}



// =============================================================================
//  TracJC - เดินจนเจอแยก 4 ทาง (เจอทั้ง a0 และ a7)
// =============================================================================

void TracJC(int spl, int spr, float kp)
{
  // เดินจนเจอแยก 4 ทาง แล้วหยุด
  fline4sensor(spl, spr, kp, "a07", 'n', 's', 50, "a5", 0);
}

void TracJC(int speed, float kp)
{
  TracJC(speed, speed, kp);
}

void TracJC(int speed)
{
  TracJC(speed, speed, 0.85);
}

void TracJC()
{
  TracJC(50, 50, 0.85);
}

void TracJCSpeedTime(int speed, int timeMs)
{
  TracJC();
  TracTimeA(speed, speed, 0.85, timeMs);
}

// =============================================================================
//  TracJCL - เดินจนเจอแยกซ้าย (a0)
// =============================================================================

void TracJCL(int spl, int spr, float kp)
{
  // เดินจนเจอแยกซ้าย แล้วหยุด
  fline4sensor(spl, spr, kp, "a0", 'f', 's', 50, "a5", 0);
}

void TracJCL(int speed, float kp)
{
  TracJCL(speed, speed, kp);
}

void TracJCL(int speed)
{
  TracJCL(speed, speed, 0.85);
}

void TracJCL()
{
  TracJCL(50, 50, 0.85);
}

// =============================================================================
//  TracJCR - เดินจนเจอแยกขวา (a7)
// =============================================================================

void TracJCR(int spl, int spr, float kp)
{
  // เดินจนเจอแยกขวา แล้วหยุด
  fline4sensor(spl, spr, kp, "a7", 'f', 's', 50, "a5", 0);
}

void TracJCR(int speed, float kp)
{
  TracJCR(speed, speed, kp);
}

void TracJCR(int speed)
{
  TracJCR(speed, speed, 0.85);
}

void TracJCR()
{
  TracJCR(50, 50, 0.85);
}

// =============================================================================
//  TracJCC - เดินจนเจอเส้นกลาง (analog 26, 27)
// =============================================================================

void TracJCC(int spl, int spr, float kp)
{
  // เดินจนเจอเส้นกลาง (ใช้เซ็นเซอร์ analog 26, 27)
  fline4sensor(spl, spr, kp, "a07", 'c', 's', 50, "a5", 0);
}

void TracJCC(int speed, float kp)
{
  TracJCC(speed, speed, kp);
}

void TracJCC(int speed)
{
  TracJCC(speed, speed, 0.85);
}

void TracJCC()
{
  TracJCC(50, 50, 0.85);
}

// =============================================================================
//  TracJCC_P - เดินจนเจอเส้นกลาง แล้วเดินผ่าน
// =============================================================================

void TracJCC_P(int spl, int spr, float kp)
{
  // เดินจนเจอเส้นกลาง แล้วเดินผ่าน
  fline4sensor(spl, spr, kp, "a07", 'c', 'p', 50, "a5", 0);
}

void TracJCC_P(int speed, float kp)
{
  TracJCC_P(speed, speed, kp);
}

void TracJCC_P(int speed)
{
  TracJCC_P(speed, speed, 0.85);
}


// =============================================================================
//  TracStop - เดินแล้วหยุดตามเงื่อนไข
// =============================================================================

void TracStop(int spl, int spr, float kp, String distance, int brake_time)
{
  // เดินจนเจอเงื่อนไข แล้วเบรก
  // distance: "a0" = แยกซ้าย, "a7" = แยกขวา, "a07" = แยก 4 ทาง
  fline4sensor(spl, spr, kp, distance, 'n', 's', 50, "a5", brake_time);
}

void TracStop(int speed, float kp, String distance, int brake_time)
{
  TracStop(speed, speed, kp, distance, brake_time);
}

void TracStop(int speed, String distance, int brake_time)
{
  TracStop(speed, speed, 0.85, distance, brake_time);
}

void TracStop(int speed, String distance)
{
  TracStop(speed, speed, 0.85, distance, 100);
}

// =============================================================================
//  TracPass - เดินผ่านแยก
// =============================================================================

void TracPass(int spl, int spr, float kp, String distance)
{
  // เดินจนเจอแยก แล้วเดินผ่าน (ไม่หยุด)
  fline4sensor(spl, spr, kp, distance, 'n', 'p', 50, "a5", 0);
}

void TracPass(int speed, float kp, String distance)
{
  TracPass(speed, speed, kp, distance);
}

void TracPass(int speed, String distance)
{
  TracPass(speed, speed, 0.85, distance);
}

// =============================================================================
//  TracForward - เดินจนเจอแยก แล้วเดินหน้าต่อ (ใช้ nfc='f')
// =============================================================================

void TracForward(int spl, int spr, float kp, String distance, int brake_time)
{
  // เดินจนเจอแยก แล้วเดินหน้าต่อจนหลุดเส้น แล้วเบรก
  fline4sensor(spl, spr, kp, distance, 'f', 's', 50, "a5", brake_time);
}

void TracForward(int speed, float kp, String distance, int brake_time)
{
  TracForward(speed, speed, kp, distance, brake_time);
}

void TracForward(int speed, String distance, int brake_time)
{
  TracForward(speed, speed, 0.85, distance, brake_time);
}

void TracForward(int speed, String distance)
{
  TracForward(speed, speed, 0.85, distance, 100);
}

// =============================================================================
//  TracSlow / TracFast - เดินช้า/เร็ว จนเจอแยก 4 ทาง
// =============================================================================

void TracSlow()
{
  TracJC(30, 30, 0.65);
}

void TracFast()
{
  TracJC(70, 70, 0.85);
}

void TracTurbo()
{
  TracJC(90, 90, 1.00);
}

// =============================================================================
//  TracJC_Line - ข้ามแยก n ครั้ง
// =============================================================================

void TracJC_Line(int n, int speed, float kp)
{
  for (int i = 0; i < n; i++)
  {
    TracJC(speed, speed, kp);
    delay(50);
  }
}

void TracJC_Line(int n, int speed)
{
  TracJC_Line(n, speed, 0.85);
}

void TracJC_Line(int n)
{
  TracJC_Line(n, 50, 0.85);
}

// =============================================================================
//  AO / Finish - หยุดและจบ
// =============================================================================

void AO()
{
  Motor(0, 0);
}

void MotorStop()
{
  Motor(0, 0);
}

void Finish()
{
  Motor(0, 0);
  tone(9, 2000, 200);
  delay(250);
  tone(9, 2500, 200);
  delay(250);
  tone(9, 3000, 300);
}

// =============================================================================
//  bline_until_C - ถอยหลังจนเจอเส้นกลาง
// =============================================================================

void bline_until_C(int spl, int spr, float kp)
{
  // ========== ตรวจสอบว่าเริ่มต้นที่ดำหรือไม่ ==========
  bool start_on_black = false;
  if (analogRead(A0) < (sensorMinC[0] + md_sensorC(0)) / 2 ||
      analogRead(A1) < (sensorMinC[1] + md_sensorC(1)) / 2)
  {
    start_on_black = true;

    // ========== ถอยหลังจนเจอขาว ==========
    my.resetAngles();
    my.reCalibrateGyro();
    my.resetAngles();

    float roll, pitch, yaw;
    float target_angle = 0;    // เป้าหมายคือเดินตรง (0 องศา)
    const float KP_GYRO = 2.0; // gain สำหรับควบคุมมุม

    while (1)
    {
      // อ่านมุมจาก gyro
      my.readAngles(roll, pitch, yaw);

      // คำนวณ correction เพื่อให้เดินตรง
      float angle_error = target_angle - yaw;
      float correction = angle_error * KP_GYRO;

      // จำกัด correction
      if (correction > 30)
        correction = 30;
      if (correction < -30)
        correction = -30;

      // ถอยหลังพร้อมแก้มุม
      Motor(-spl - correction, -spr + correction);
      delayMicroseconds(50);

      // เช็คว่าเจอขาวแล้วหรือยัง (ทั้งสองเซ็นเซอร์เป็นขาว)
      if (analogRead(A0) > (sensorMinC[0] + md_sensorC(0)) / 2 &&
          analogRead(A1) > (sensorMinC[1] + md_sensorC(1)) / 2)
      {
        break;
      }
    }

    // ========== ถอยหลังต่อจนเจอดำอีกครั้ง ==========
    my.resetAngles();

    while (1)
    {
      // อ่านมุมจาก gyro
      my.readAngles(roll, pitch, yaw);

      // คำนวณ correction เพื่อให้เดินตรง
      float angle_error = target_angle - yaw;
      float correction = angle_error * KP_GYRO;

      // จำกัด correction
      if (correction > 30)
        correction = 30;
      if (correction < -30)
        correction = -30;

      // ถอยหลังพร้อมแก้มุม
      Motor(-spl - correction, -spr + correction);
      delayMicroseconds(50);

      // เช็คว่าเจอดำอีกครั้งหรือยัง
      if (analogRead(A0) < (sensorMinC[0] + md_sensorC(0)) / 2 ||
          analogRead(A1) < (sensorMinC[1] + md_sensorC(1)) / 2)
      {
        Motor(-1, -1);
        delay(10);
        return; // หยุดและออกจากฟังก์ชัน
      }
    }
  }

  // ========== ถ้าไม่เริ่มที่ดำ ให้ทำงานปกติ (ตามเส้น) ==========
  while (1)
  {
    if (read_sensorA(1) > md_sensorA(1) && read_sensorA(2) > md_sensorA(2) &&
        read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4) &&
        read_sensorA(5) > md_sensorA(5) && read_sensorA(6) > md_sensorA(6))
    {
      errors = 0;
    }
    else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
    {
      errors = 10;
    }
    else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
    {
      errors = -10;
    }
    else
    {
      errors = error_4sensor();
    }

    P = errors;
    I += errors * 0.00005;
    D = errors - previous_error;
    previous_error = errors;
    PID_output = (kp * P) + (0.0000001 * I) + (0.0125 * D);
    Motor(-slmotor - PID_output, -slmotor + PID_output);
    delayMicroseconds(50);

    if (analogRead(A0) < (sensorMinC[0] + md_sensorC(0)) / 2 ||
        analogRead(A1) < (sensorMinC[1] + md_sensorC(1)) / 2)
    {
      break;
    }
  }

  Motor(-1, -1);
  delay(10);
}
// =============================================================================
//  ตัวอย่างการใช้งานทั้งหมด
// =============================================================================

/*
// 1. ใช้ TracPID แบบ Manual
InitRobot(50);           // ตั้งค่า PID ตามความเร็ว 50
TracSpeedTime(50, 2000); // เดิน 2000ms ด้วย TracPID
TracJCStop();            // เดินจนเจอแยก
TuneJC();                // ปรับตำแหน่ง

// 2. ใช้ฟังก์ชัน TracJC แบบง่าย (ใช้ fline4sensor)
TracJC();                    // เดินจนเจอแยก 4 ทาง
TracJC(60);                  // ความเร็ว 60
TracJCL();                   // เจอแยกซ้าย
TracJCR();                   // เจอแยกขวา
TracJCC();                   // เจอเส้นกลาง

// 3. เดินตามเวลา
TracTime(50, 1000);          // เดิน 1000ms
TracSpeedTime(1000);         // เดิน 1000ms ด้วย TracPID

// 4. ฟังก์ชันพิเศษ
TracJCSlow(30);              // เดินช้าจนเจอแยก
TracNormalTime(1000);        // เดิน 1000ms ไม่ตรวจจับแยก
TracUntilSensor(0);          // เดินจนเซ็นเซอร์ 0 เจอเส้น
TracJC_Line(3);              // ข้ามแยก 3 ครั้ง

// 5. หยุดและจบ
AO();                        // หยุด
Finish();                    // หยุดพร้อมเสียง
*/
// void TracSpeedTime(int spl, int spr, float kp, int TracTime)
// {
//   _fw = true;
//   float traveled_distance = 0;
//   unsigned long TracTimer = millis();
//   unsigned long last_time = millis();

//   float I_max = 1000.0, I_min = -1000.0;
//   float D_max = 50.0, D_min = -50.0;
//   float alpha = 0.1;
//   static float filtered_D = 0.0;
//   float output_max = 100.0, output_min = -100.0;

//   if (kp == 0)
//   {
//     I = kp_slow = ki_slow = 0;
//   }
//   if (kp < 6.5)
//   {
//     pid_error = true;
//   }
//   else
//   {
//     pid_error = false;
//   }

//   int current_speed = 0;
//   int target_speed = min(spl, spr);
//   const int ramp_step = 3;

//   // Soft start
//   if (!ch_p)
//   {
//     while (current_speed < target_speed)
//     {
//       if (kp <= 0.45)
//       {
//         if (read_sensorA(2) > md_sensorA(2) && read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4) && read_sensorA(5) > md_sensorA(5))
//         {
//           errors = 0;
//         }
//         else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
//         {
//           errors = 10;
//         }
//         else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
//         {
//           errors = -10;
//         }
//         else
//         {
//           errors = error_AA();
//         }
//       }
//       else
//       {
//         errors = error_AA();
//       }

//       P = errors;
//       I += errors * 0.00005;
//       D = errors - previous_error;
//       previous_error = errors;
//       PID_output = (kp * P) + (0.000001 * I) + (kd_f * D);
//       Motor(spl - PID_output, spr + PID_output);
//       delayMicroseconds(50);

//       current_speed += ramp_step;
//       if (current_speed > target_speed)
//       {
//         current_speed = target_speed;
//         delay(ramp_delay);
//       }

//       // เช็คเวลาตั้งแต่ soft start
//       if (millis() - TracTimer >= TracTime)
//       {
//         Motor(0, 0);
//         return;
//       }
//     }
//   }

//   // วิ่งปกติ
//   while (1)
//   {
//     // ตรวจจับเส้นข้างซ้าย (sensor 0) และเซ็นเซอร์กลางไม่เจอเส้น
//     if (read_sensorA(0) < md_sensorA(0) &&
//         read_sensorA(2) > md_sensorA(2) &&
//         read_sensorA(3) > md_sensorA(3) &&
//         read_sensorA(4) > md_sensorA(4) &&
//         read_sensorA(5) > md_sensorA(5))
//     {
//       // หมุนซ้ายกลับมาหาเส้น
//       while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
//       {
//         Motor(-target_speed * 0.5, target_speed * 0.5);
//         delayMicroseconds(50);

//         if (millis() - TracTimer >= TracTime)
//         {
//           Motor(0, 0);
//           return;
//         }
//       }
//       I = 0;
//       previous_error = 0;
//     }

//     // ตรวจจับเส้นข้างขวา (sensor 7) และเซ็นเซอร์กลางไม่เจอเส้น
//     else if (read_sensorA(7) < md_sensorA(7) &&
//              read_sensorA(2) > md_sensorA(2) &&
//              read_sensorA(3) > md_sensorA(3) &&
//              read_sensorA(4) > md_sensorA(4) &&
//              read_sensorA(5) > md_sensorA(5))
//     {
//       // หมุนขวากลับมาหาเส้น
//       while (read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4))
//       {
//         Motor(target_speed * 0.5, -target_speed * 0.5);
//         delayMicroseconds(50);

//         if (millis() - TracTimer >= TracTime)
//         {
//           Motor(0, 0);
//           return;
//         }
//       }
//       I = 0;
//       previous_error = 0;
//     }

//     // PID ปกติ
//     if (kp <= 0.45)
//     {
//       if (read_sensorA(2) > md_sensorA(2) && read_sensorA(3) > md_sensorA(3) && read_sensorA(4) > md_sensorA(4) && read_sensorA(5) > md_sensorA(5))
//       {
//         errors = 0;
//       }
//       else if (read_sensorA(5) < md_sensorA(5) && read_sensorA(7) < md_sensorA(7))
//       {
//         errors = 10;
//       }
//       else if (read_sensorA(2) < md_sensorA(2) && read_sensorA(0) < md_sensorA(0))
//       {
//         errors = -10;
//       }
//       else
//       {
//         errors = error_AA();
//       }
//     }
//     else
//     {
//       errors = error_AA();
//     }

//     P = errors;
//     I += errors * 0.00005;
//     D = errors - previous_error;
//     previous_error = errors;
//     PID_output = (kp * P) + (0.000001 * I) + (kd_f * D);
//     Motor(spl - PID_output, spr + PID_output);
//     delayMicroseconds(50);

//     // เช็คเวลา - ครบเวลาหยุดทันที
//     if (millis() - TracTimer >= TracTime)
//     {
//       // เบรคเบาๆ สั้นๆ
//       Motor(-10, -10);
//       delay(20);
//       Motor(0, 0);
//       break;
//     }
//   }

//   ch_p = 0;
// }