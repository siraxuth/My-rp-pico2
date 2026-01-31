// #include <myrp_pico2.h>

/*
 * =============================================================================
 * TurnIMU.ino - รวมฟังก์ชัน Gyro/IMU ล้วน (ไม่รวมเดินตามเส้น)
 * =============================================================================
 * ฟังก์ชันครบถ้วน: TL_deg, TR_deg, SpinDegree, TracDegree, Curve, etc.
 * =============================================================================
 */

// =============================================================================
//  1. CONFIG & VARIABLES
// =============================================================================

int IMU_TurnSpeed = 70;
int IMU_BrakeSpeed = 100;
int IMU_BrakeTime = 50;
float IMU_Tolerance = 2.0; // ความคลาดเคลื่อนที่ยอมรับได้ (องศา)

// Helper: อ่านค่า Yaw
float getYaw()
{
  return my.gyro('z');
}

// Helper: แปลงมุมให้อยู่ในช่วง -180 ถึง 180
float normalizeAngle(float angle)
{
  while (angle > 180)
    angle -= 360;
  while (angle < -180)
    angle += 360;
  return angle;
}

// Helper: เช็คแยก (จำเป็นต้องมีเพื่อให้ TracJCDegree ทำงานได้ในไฟล์นี้)
bool isJunction()
{
  // เช็คดำซ้ายสุด(0) และ ขวาสุด(7) พร้อมกัน
  return (read_sensorA(0) < md_sensorA(0)) && (read_sensorA(7) < md_sensorA(7));
}

// =============================================================================
//  2. SETTING FUNCTIONS
// =============================================================================

void SetIMUTurnSpeed(int speed) { IMU_TurnSpeed = speed; }
void SetIMUBrake(int speed, int time)
{
  IMU_BrakeSpeed = speed;
  IMU_BrakeTime = time;
}
void SetIMUTolerance(float tol) { IMU_Tolerance = tol; }

// =============================================================================
//  3. RELATIVE TURN (ระบบ 0-180) - รีเซ็ตศูนย์ก่อนเลี้ยว
// =============================================================================

// เลี้ยวซ้าย
void TL_deg(int speed, float degree, int offset)
{
  my.resetAngles();      // รีเซ็ตมุมทั้งหมด
  my.reCalibrateGyro();  // calibrate offset ใหม่ (เร็ว ~150 ms)
  my.resetAngles();      // รีเซ็ตมุมทั้งหมด
  // คำนวณค่ามุมเริ่มต้น
  float initialDegree = 0;
  for (int i = 0; i < 5; i++)
  {
    initialDegree += my.gyro('z'); // ใช้ค่าที่อ่านได้จากเซ็นเซอร์
    delay(10);
  }
  initialDegree /= 5.0;

  // คำนวณมุมเป้าหมาย
  float targetDegree = initialDegree + (-degree);

  // กำหนดค่าของ PID

  float lr_kp = 2.05;    // ปรับค่า Kp เพื่อให้การหมุนเร็วขึ้น
  float lr_ki = 0.00001; // ค่า Ki ปรับตามความแม่นยำในการหยุด
  float lr_kd = 0.015;   // ปรับค่า Kd เพื่อให้การหยุดมีความแม่นยำขึ้น

  float error = 0, previous_error = 0;
  float integral = 0, output = 0;
  float currentDegree = 0;

  unsigned long lastTime = millis();
  unsigned long timeout = 500; // กำหนดเวลา timeout
  unsigned long startTime = millis();

  while (true)
  {
    currentDegree = my.gyro('z');         // อ่านค่ามุมปัจจุบัน
    error = targetDegree - currentDegree; // คำนวณความผิดพลาด

    // ตรวจสอบเงื่อนไขการหยุดเมื่อใกล้ถึงมุมที่ต้องการ
    if (abs(error) < 1 && abs(output) < 5)
      break;

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;
    lastTime = now;

    if (dt > 0)
    {
      integral += error * dt;
      float derivative = (error - previous_error) / dt;
      previous_error = error;

      output = lr_kp * error + lr_ki * integral + lr_kd * derivative;
    }

    // จำกัดค่าของ output ให้อยู่ในช่วงความเร็วที่ต้องการ
    output = constrain(output, -speed, speed);

    // ควบคุมมอเตอร์ให้หมุนตาม PID ที่คำนวณ
    if (degree > 0)
    {
      Motor(output, -output);
      delay(5);
    }
    else
    {

      Motor(-output, output);
      delay(5);
    }

    // ตรวจจับ timeout หากใช้เวลานานเกินไป
    if (millis() - startTime > timeout)
    {
      break; // ออกจาก loop หากเวลาผ่านไปนานเกินไป
    }
  }

  // หยุดมอเตอร์หลังจากหมุนเสร็จ
  if (degree > 0)
  {

    Motor(output, -output);
    delay(offset);
  }
  else
  {
    Motor(-output, output);
    delay(offset);
  }
  Motor(-1, -1);
  delay(10);
}

// เลี้ยวขวา
void TR_deg(int speed, float degree, int offset)
{
  my.resetAngles();      // รีเซ็ตมุมทั้งหมด
  my.reCalibrateGyro();  // calibrate offset ใหม่ (เร็ว ~150 ms)
  my.resetAngles();      // รีเซ็ตมุมทั้งหมด
  // คำนวณค่ามุมเริ่มต้น
  float initialDegree = 0;
  for (int i = 0; i < 5; i++)
  {
    initialDegree += my.gyro('z'); // ใช้ค่าที่อ่านได้จากเซ็นเซอร์
    delay(10);
  }
  initialDegree /= 5.0;

  // คำนวณมุมเป้าหมาย
  float targetDegree = initialDegree + degree;

  // กำหนดค่าของ PID

  float lr_kp = 2.05;    // ปรับค่า Kp เพื่อให้การหมุนเร็วขึ้น
  float lr_ki = 0.00001; // ค่า Ki ปรับตามความแม่นยำในการหยุด
  float lr_kd = 0.015;   // ปรับค่า Kd เพื่อให้การหยุดมีความแม่นยำขึ้น

  float error = 0, previous_error = 0;
  float integral = 0, output = 0;
  float currentDegree = 0;

  unsigned long lastTime = millis();
  unsigned long timeout = 500; // กำหนดเวลา timeout
  unsigned long startTime = millis();

  while (true)
  {
    currentDegree = my.gyro('z');         // อ่านค่ามุมปัจจุบัน
    error = targetDegree - currentDegree; // คำนวณความผิดพลาด

    // ตรวจสอบเงื่อนไขการหยุดเมื่อใกล้ถึงมุมที่ต้องการ
    if (abs(error) < 1 && abs(output) < 5)
      break;

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;
    lastTime = now;

    if (dt > 0)
    {
      integral += error * dt;
      float derivative = (error - previous_error) / dt;
      previous_error = error;

      output = lr_kp * error + lr_ki * integral + lr_kd * derivative;
    }

    // จำกัดค่าของ output ให้อยู่ในช่วงความเร็วที่ต้องการ
    output = constrain(output, -speed, speed);

    // ควบคุมมอเตอร์ให้หมุนตาม PID ที่คำนวณ
    if (degree > 0)
    {
      Motor(output, -output);
      delay(5);
    }
    else
    {
      Motor(-output, output);
      delay(5);
    }

    // ตรวจจับ timeout หากใช้เวลานานเกินไป
    if (millis() - startTime > timeout)
    {
      break; // ออกจาก loop หากเวลาผ่านไปนานเกินไป
    }
  }

  // หยุดมอเตอร์หลังจากหมุนเสร็จ
  if (degree > 0)
  {
    Motor(output, -output);
    delay(offset);
  }
  else
  {
    Motor(-output, output);
    delay(offset);
  }
  Motor(-1, -1);
  delay(10);
}

// Wrapper Functions (เรียกแบบไม่ใส่ความเร็ว)
void TL_deg(int speed, float degree) { TL_deg(speed, degree, 10); }
void TR_deg(int speed, float degree) { TR_deg(speed, degree, 10); }

// กลับหลังหัน (ยูเทิร์น)
void UTL_deg(int speed, float degree) { TL_deg(speed, degree, 10); }
void UTL_deg(float degree) { TL_deg(IMU_TurnSpeed, degree, 10); }
void UTR_deg(int speed, float degree) { TR_deg(speed, degree, 10); }
void UTR_deg(float degree) { TR_deg(IMU_TurnSpeed, degree, 10); }

// =============================================================================
//  4. ABSOLUTE TURN (ระบบ 0-360) - หมุนหาองศาจริง
// =============================================================================


void spinTurn(
    SpinDir dir,
    SpinMode mode,
    int speed,
    int degree,
    int offset)
{
  my.resetAngles();
  my.reCalibrateGyro();
  my.resetAngles();
  delay(50);

  bool useGyro = gyroOK();

  int sign = (dir == LEFT) ? -1 : 1;
  if (mode == OUT)
    sign *= -1;

  // ================== GYRO MODE ==================
  if (useGyro && abs(my.gyro('z')) < 200)
  {
    float base = 0;
    for (int i = 0; i < 5; i++)
    {
      base += my.gyro('z');
      delay(10);
    }
    base /= 5.0;

    float target = base + sign * degree;

    float kp = 2.05, ki = 0.00001, kd = 0.015;
    float err = 0, lastErr = 0, integral = 0, out = 0;

    unsigned long lastT = millis();
    unsigned long timeout = 500;
    unsigned long start = millis();

    while (true)
    {
      float cur = my.gyro('z');
      err = target - cur;

      if (abs(err) < 1 && abs(out) < 5)
        break;
      if (millis() - start > 500)
        break;

      float dt = (millis() - lastT) / 1000.0;
      lastT = millis();

      if (dt > 0)
      {
        integral += err * dt;
        float d = (err - lastErr) / dt;
        lastErr = err;
        out = kp * err + ki * integral + kd * d;
      }

      out = constrain(out, -speed, speed);

      // หมุน
      Motor(sign * out, -sign * out);
      delay(5);

    }

    // offset
    Motor(-sign * speed, sign * speed);
    delay(offset);

    Motor(-1, -1);
    delay(10);
    return;
  }
}

// LEFT
void spinLeftIn(int speed, int degree, int offset)
{
  spinTurn(LEFT, IN, speed, degree, offset);
}

void spinLeftOut(int speed, int degree, int offset)
{
  spinTurn(LEFT, OUT, speed, degree, offset);
}

// RIGHT
void spinRightIn(int speed, int degree, int offset)
{
  spinTurn(RIGHT, IN, speed, degree, offset);
}

void spinRightOut(int speed, int degree, int offset)
{
  spinTurn(RIGHT, OUT, speed, degree, offset);
}


// =============================================================================
//  5. GYRO TRACKING (เดินตรงโดยใช้ Gyro)
// =============================================================================

// เดินตรงรักษามุม (Absolute) ตามเวลา
void TracDegree(int speed, float targetDegree, int timeMs)
{
  unsigned long start = millis();
  float kp = 2.0;

  while ((millis() - start) < timeMs)
  {
    float error = normalizeAngle(targetDegree - getYaw());
    int adj = (int)(error * kp);

    // Limit adj
    if (adj > 30)
      adj = 30;
    if (adj < -30)
      adj = -30;

    Motor(speed + adj, speed - adj);
    delay(1);
  }
  Motor(0, 0);
}
void TracDegree(int speed, float targetDegree) { TracDegree(speed, targetDegree, 1000); }

// ถอยตรงรักษามุม (Absolute)
void TracDegreeBack(int speed, float targetDegree, int timeMs)
{
  unsigned long start = millis();
  float kp = 2.0;

  while ((millis() - start) < timeMs)
  {
    float error = normalizeAngle(targetDegree - getYaw());
    int adj = (int)(error * kp); // error บวก = หน้าหันซ้าย -> ต้องให้ล้อซ้ายถอยช้าลง (เพิ่มค่าลบ)

    // Logic ถอยหลัง: ถ้าหน้าหันซ้าย(error+), ท้ายปัดขวา -> ต้องถอยล้อซ้ายเร็วขึ้น, ล้อขวาช้าลง
    Motor(-(speed - adj), -(speed + adj));
    delay(1);
  }
  Motor(0, 0);
}

// เดินตรงโดยรีเซ็ตมุมเป็น 0 (Relative)
void TracDegreeSpeedTime(int speed, int timeMs)
{
  my.resetYaw();
  TracDegree(speed, 0, timeMs);
}
void TracDegreeSpeedTimeBack(int speed, int timeMs)
{
  my.resetYaw();
  TracDegreeBack(speed, 0, timeMs);
}

// =============================================================================
//  6. GYRO UNTIL LINE (เดิน Gyro จนเจอเส้น)
// =============================================================================

// เดินหน้าตรง (Gyro) จนกว่าจะเจอแยก
void TracJCDegree(int speed, int backTimeMs)
{
  my.resetYaw();  
  float kp = 2.0;

  while (!isJunction())
  { // หยุดเมื่อเจอแยก
    float error = 0 - getYaw();
    int adj = error * kp;
    Motor(speed + adj, speed - adj);
    delay(1);
  }
  Motor(0, 0);

  if (backTimeMs > 0)
    TracDegreeSpeedTimeBack(speed, backTimeMs);
}

// ถอยหลังตรง (Gyro) จนกว่าจะเจอแยก
void TracJCDegreeBack(int speed, int forwardTimeMs)
{
  my.resetYaw();
  float kp = 2.0;

  // ใช้ sensorB สำหรับเช็คเส้นด้านหลัง (ถ้าไม่มี sensorB ให้แก้เป็น A)
  // สมมติว่าใช้ logic เดียวกันกับ A ไปก่อนถ้าไม่มี B
  while (true)
  {
    // เช็คแยกจาก sensor หลัง (0 และ 7)
    if (read_sensorA(0) < md_sensorA(0) || read_sensorA(7) < md_sensorA(7))
      break;

    float error = 0 - getYaw();
    int adj = error * kp;
    Motor(-(speed - adj), -(speed + adj));
    delay(1);
  }
  Motor(0, 0);

  if (forwardTimeMs > 0)
    TracDegreeSpeedTime(speed, forwardTimeMs);
}

// =============================================================================
//  7. CURVE DEGREES (เดินโค้งตามองศา)
// =============================================================================

void CurveLeftDegree(int speed, float degree)
{
  my.resetYaw();
  Motor(speed*0.58, speed); // ซ้ายช้า ขวาเร็ว
  while (getYaw() > -degree)
    delay(1); // มุมซ้ายเป็นลบ
  Motor(0, 0);
}

void CurveRightDegree(int speed, float degree)
{
  my.resetYaw();
  Motor(speed, speed*0.58); // ซ้ายเร็ว ขวาช้า
  while (getYaw() < degree)
    delay(1); // มุมขวาเป็นบวก
  Motor(0, 0);
}

void CurveLeftDegreeBack(int speed, float degree)
{
  my.resetYaw();
  Motor(-(speed*0.58), -speed);
  while (getYaw() > -degree)
    delay(1);
  Motor(0, 0);
}

void CurveRightDegreeBack(int speed, float degree)
{
  my.resetYaw();
  Motor(-speed, -(speed / 3));
  while (getYaw() < degree)
    delay(1);
  Motor(0, 0);
}

// =============================================================================
//  ChangeSpeed - เปลี่ยนความเร็ว
// =============================================================================

void ChangeSpeed(int speed, int accSpeed)
{
  TRAC_SPEED = speed;
  TRAC_FAST = accSpeed;
}

// =============================================================================
//  ตัวอย่างการใช้งาน
// =============================================================================
/*
void setup() {
  my.begin();
  SetIMUTurnSpeed(70);
  SetIMUTolerance(2.0);
}

void loop() {
  // ===== [ระบบ 1] ใช้มุมสัมพัทธ์ 0-180 (รีเซ็ทก่อนเลี้ยวทุกครั้ง) =====

  TL_deg(90);               // เลี้ยวซ้าย 90 องศา
  delay(1000);

  TR_deg(90);               // เลี้ยวขวา 90 องศา
  delay(1000);

  UTL_deg(180);             // กลับตัวซ้าย 180 องศา
  delay(1000);

  TracDegreeSpeedTime(50, 2000);  // เดินตรง 2 วินาที (รักษาองศา 0)

  CurveLeftDegree(50, 45);  // โค้งซ้าย 45 องศา


  // ===== [ระบบ 2] ใช้มุมสัมบูรณ์ 0-360 (ไม่รีเซ็ท) =====

  my.resetYaw();            // รีเซ็ทมุมเริ่มต้น 1 ครั้ง

  SpinDegree(90);           // หมุนไปมุม 90 องศา
  delay(500);

  TracDegree(50, 90, 2000); // เดินตรง 2 วินาที รักษามุม 90 องศา

  SpinDegree(180);          // หมุนไปมุม 180 องศา
  delay(500);

  SpinDegree(0);            // หมุนกลับมุม 0 องศา

  while(1);
}
*/