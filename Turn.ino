/*
 * =============================================================================
 * Turn.ino - เลี้ยวหุ่นยนต์ (MyRP_Pico2 Version) - Updated
 * =============================================================================
 * การเปลี่ยนแปลง:
 * - Spin: Motor(0, speed) หรือ Motor(speed, 0) = ล้อข้างหนึ่งหยุด
 * - Turn: Motor(speed, -speed) หรือ Motor(-speed, speed) = ล้อสองข้างหมุนสวนทาง
 * - Digital Logic: 0=Black, 1=White
 * =============================================================================
 */
 
 // =============================================================================
 //  ค่า Config สำหรับการเลี้ยว
 // =============================================================================
 
 int TURN_Speed = 90;
 int TURN_BrakeSpeed = 100;
 int TURN_BrakeTime = 100;

// =============================================================================
//  Left / Right - หมุนตามเวลา (Timer) - ใช้ Spin
// =============================================================================
void Left(int Speed, int Time)
{
  MotorStop();
  SL(Speed);
  delay(Time);
  MotorStop();
}
void Right(int Speed, int Time)
{
  MotorStop();
  SR(Speed);
  delay(Time);
  MotorStop();
}


// =============================================================================
//  SetTurnSpeed - ตั้งค่าความเร็วเลี้ยว
// =============================================================================

void SetTurnSpeed(int speed)
{
  TURN_Speed = speed;
}

void SetTurnBrake(int speed, int time)
{
  TURN_BrakeSpeed = speed;
  TURN_BrakeTime = time;
}

// =============================================================================
//  Helper Function: อ่านค่าเป็น 0 หรือ 1
//  0 = ดำ (Black)
//  1 = ขาว (White)
// =============================================================================

int getSensor(int idx)
{
  // อ่านค่า Analog เปรียบเทียบกับค่ากลาง (Ref)
  if (read_sensorA(idx) < md_sensorA(idx))
  {
    return 0; // ค่าน้อยกว่าค่ากลาง = เจอเส้นดำ
  }
  else
  {
    return 1; // ค่ามากกว่าค่ากลาง = พื้นขาว
  }
}


// =============================================================================
//  TurnLeftSpeed - เลี้ยวซ้ายด้วยความเร็วที่กำหนด (ใช้เซ็นเซอร์แบบ fline4sensor)
// =============================================================================

void TurnLeftSpeed(int speed, String se, int delayTime)
{
  // หยุดมอเตอร์ก่อน
  Motor(0, 0);
  delay(50);

  // เลี้ยวซ้ายเบื้องต้นแบบนุ่มนวล
  Motor(-25, 25);
  delay(100);

  // แปลง sensor string เป็น int
  char sensors[4];
  se.toCharArray(sensors, sizeof(sensors));
  int sensor_f = atoi(&sensors[1]);

  // ความเร็วเลี้ยวช้า
  int turn_speed = (speed < 50) ? speed : 40; // จำกัดความเร็วสูงสุดที่ 50

  // เลี้ยวแบบค่อยเป็นค่อยไป โดยพยายามให้ center อยู่บนเส้น
  bool found_line = false;
  unsigned long start_time = millis();

  while (!found_line && (millis() - start_time < 3000)) // timeout 3 วินาที
  {
    // ตรวจสอบเซ็นเซอร์ตรงกลาง
    bool a3_on_line = read_sensorA(3) < md_sensorA(3);
    bool a4_on_line = read_sensorA(4) < md_sensorA(4);

    if (a3_on_line && a4_on_line)
    {
      // ทั้งคู่อยู่บนเส้น - ตำแหน่งสมบูรณ์แบบ
      found_line = true;
      break;
    }
    else if (a3_on_line || a4_on_line)
    {
      // มีเซ็นเซอร์ตัวใดตัวหนึ่งเจอเส้น - เลี้ยวช้ามาก
      Motor((clml * turn_speed) / 200, (clmr * turn_speed) / 200);
    }
    else
    {
      // ยังไม่เจอเส้น - เลี้ยวปกติแต่ช้า
      Motor((clml * turn_speed) / 100, (clmr * turn_speed) / 100);
    }

    delayMicroseconds(100); // delay นานขึ้นเพื่อให้เลี้ยวช้า
  }

  // ปรับละเอียดให้ a3, a4 อยู่ตรงกลางเส้นพอดี
  int fine_adjust = 0;
  while (fine_adjust < 200)
  {
    bool a3_on = read_sensorA(3) < md_sensorA(3);
    bool a4_on = read_sensorA(4) < md_sensorA(4);

    if (a3_on && a4_on)
    {
      // สมบูรณ์แบบแล้ว
      break;
    }
    else if (!a3_on && !a4_on)
    {
      // เลยเส้นไปแล้ว - ถอยกลับนิดหน่อย
      Motor(-5, 5);
    }
    else if (a3_on && !a4_on)
    {
      // a3 เจอแต่ a4 ไม่เจอ - เลี้ยวซ้ายนิดหน่อย
      Motor((clml * turn_speed) / 300, (clmr * turn_speed) / 300);
    }
    else if (!a3_on && a4_on)
    {
      // a4 เจอแต่ a3 ไม่เจอ - เลี้ยวขวานิดหน่อย
      Motor(-(clml * turn_speed) / 300, -(clmr * turn_speed) / 300);
    }

    delayMicroseconds(100);
    fine_adjust++;
  }

  // หยุดชั่วคราว
  Motor(0, 0);
  delay(20);

  // จัดการหลังเลี้ยว
  if (delayTime == 0)
  {
    turn_speed_fl_4s();
  }
  else
  {
    Motor(-((clml * speed) / 100), -((clmr * speed) / 100));
    delay(delayTime);
    Motor(-1, -1);
    delay(10);
  }

  delay(5);
}


void TurnLeft() {
  TurnLeftSpeed(TURN_Speed, "a0", 10);
}

// =============================================================================
//  TurnRightSpeed - เลี้ยวขวาด้วยความเร็วที่กำหนด (ใช้เซ็นเซอร์)
// =============================================================================

void TurnRightSpeed(int speed, String se, int delayTime)
{ // หยุดมอเตอร์ก่อน
  Motor(0, 0);
  delay(50);

  // เลี้ยวซ้ายเบื้องต้นแบบนุ่มนวล
  Motor(22, -22);
  delay(100);

  // แปลง sensor string เป็น int
  char sensors[4];
  se.toCharArray(sensors, sizeof(sensors));
  int sensor_f = atoi(&sensors[1]);

  // ความเร็วเลี้ยวช้า
  int turn_speed = (speed < 50) ? speed : 40; // จำกัดความเร็วสูงสุดที่ 50

  // เลี้ยวแบบค่อยเป็นค่อยไป โดยพยายามให้ center อยู่บนเส้น
  bool found_line = false;
  unsigned long start_time = millis();

  while (!found_line && (millis() - start_time < 3000)) // timeout 3 วินาที
  {
    // ตรวจสอบเซ็นเซอร์ตรงกลาง
    bool a3_on_line = read_sensorA(3) < md_sensorA(3);
    bool a4_on_line = read_sensorA(4) < md_sensorA(4);

    if (a3_on_line && a4_on_line)
    {
      // ทั้งคู่อยู่บนเส้น - ตำแหน่งสมบูรณ์แบบ
      found_line = true;
      break;
    }
    else if (a3_on_line || a4_on_line)
    {
      // มีเซ็นเซอร์ตัวใดตัวหนึ่งเจอเส้น - เลี้ยวช้ามาก
      Motor((clmr * turn_speed) / 200, (clml * turn_speed) / 200);
    }
    else
    {
      // ยังไม่เจอเส้น - เลี้ยวปกติแต่ช้า
      Motor((clmr * turn_speed) / 100, (clml * turn_speed) / 100);
    }

    delayMicroseconds(100); // delay นานขึ้นเพื่อให้เลี้ยวช้า
  }

  // ปรับละเอียดให้ a3, a4 อยู่ตรงกลางเส้นพอดี
  int fine_adjust = 0;
  while (fine_adjust < 200)
  {
    bool a3_on = read_sensorA(3) < md_sensorA(3);
    bool a4_on = read_sensorA(4) < md_sensorA(4);

    if (a3_on && a4_on)
    {
      // สมบูรณ์แบบแล้ว
      break;
    }
    else if (!a3_on && !a4_on)
    {
      // เลยเส้นไปแล้ว - ถอยกลับนิดหน่อย
      Motor(5, -5);
    }
    else if (a3_on && !a4_on)
    {
      // a3 เจอแต่ a4 ไม่เจอ - เลี้ยวชวานิดหน่อย
      Motor((clmr * turn_speed) / 300, (clml * turn_speed) / 300);
    }
    else if (!a3_on && a4_on)
    {
      // a4 เจอแต่ a3 ไม่เจอ - เลี้ยวซ้ายนิดหน่อย
      Motor(-(clmr * turn_speed) / 300, -(clml * turn_speed) / 300);
    }

    delayMicroseconds(100);
    fine_adjust++;
  }

  // หยุดชั่วคราว
  Motor(0, 0);
  delay(20);

  // จัดการหลังเลี้ยว
  if (delayTime == 0)
  {
    turn_speed_fr_4s();
  }
  else
  {
    Motor(-((clml * speed) / 100), -((clmr * speed) / 100));
    delay(delayTime);
    Motor(-1, -1);
    delay(10);
  }

  delay(5);
}
void TurnRight()
{
  fline4sensor(0, 0, 0, "a07", 'n', 'r', 90, "a7,", 10);
  // TurnRightSpeed(TURN_Speed, "a7", 10);
}

// =============================================================================
//  UTurnLeft / UTurnRight - กลับตัว 180 องศา (ใช้เซ็นเซอร์แบบ fline4sensor)
// =============================================================================

void UTurnLeftSpeed(int speed)
{
  Motor(0, 0);
  delay(2);

  // เริ่มหมุนซ้ายแบบ Turn (สวนทาง)
  Motor(-speed, speed);
  delay(300); // หมุนให้ผ่านเส้นไปก่อน

  // วนลูปเลี้ยวจนกว่าเซ็นเซอร์กลาง (2-4) จะเจอเส้นดำอีกครั้ง
  while (1)
  {
    // ตรวจสอบเซ็นเซอร์กลางว่าเจอเส้นยัง (หลังจากหมุนผ่าน 180 องศา)
    if (read_sensorA(2) < md_sensorA(2) ||
        read_sensorA(3) < md_sensorA(3) ||
        read_sensorA(4) < md_sensorA(4))
    {
      break; // เจอเส้นแล้ว ออกจากลูป
    }
    delayMicroseconds(50);
  }

  // Brake (หมุนกลับทิศเล็กน้อยเพื่อหยุดให้ตรง)
  if (TURN_BrakeTime > 0 && speed > 70)
  {
    Motor(TURN_BrakeSpeed, -TURN_BrakeSpeed);
    delay(TURN_BrakeTime);
  }

  Motor(0, 0);
  delay(2);
}

void UTurnLeft()
{
  UTurnLeftSpeed(TURN_Speed);
}

void UTurnRightSpeed(int speed)
{
  Motor(0, 0);
  delay(2);

  // เริ่มหมุนขวาแบบ Turn (สวนทาง)
  Motor(speed, -speed);
  delay(300); // หมุนให้ผ่านเส้นไปก่อน

  // วนลูปเลี้ยวจนกว่าเซ็นเซอร์กลาง (3-5) จะเจอเส้นดำอีกครั้ง
  while (1)
  {
    // ตรวจสอบเซ็นเซอร์กลางว่าเจอเส้นยัง (หลังจากหมุนผ่าน 180 องศา)
    if (read_sensorA(3) < md_sensorA(3) ||
        read_sensorA(4) < md_sensorA(4) ||
        read_sensorA(5) < md_sensorA(5))
    {
      break; // เจอเส้นแล้ว ออกจากลูป
    }
    delayMicroseconds(50);
  }

  // Brake (หมุนกลับทิศเล็กน้อยเพื่อหยุดให้ตรง)
  if (TURN_BrakeTime > 0 && speed > 70)
  {
    Motor(-TURN_BrakeSpeed, TURN_BrakeSpeed);
    delay(TURN_BrakeTime);
  }

  Motor(0, 0);
  delay(2);
}

void UTurnRight()
{
  UTurnRightSpeed(TURN_Speed);
}



// =============================================================================
//  TurnLeftDelay / TurnRightDelay - เลี้ยวโค้งแบบ Delay
// =============================================================================

void TurnLeftDelay(int speed, int timeMs)
{
  Motor(flml, flmr); // ค่าจาก myrp_pico2.h (ล้อซ้ายหมุนช้ากว่าขวา)
  delay(timeMs);
  Motor(0, 0);
}

void TurnRightDelay(int speed, int timeMs)
{
  Motor(frml, frmr); // ค่าจาก myrp_pico2.h (ล้อขวาหมุนช้ากว่าซ้าย)
  delay(timeMs);
  Motor(0, 0);
}

// =============================================================================
//  CurveLeft / CurveRight - โค้งอ้อมสิ่งกีดขวาง (ปรับให้ตรงเส้นแบบ fline4sensor)
// =============================================================================

void CurveLeft(int speed, int timeMs)
{
  Motor(0, 0);
  delay(2);

  // หมุนซ้ายเล็กน้อยเพื่อตั้งลำ (ใช้ Turn แบบสวนทาง)
  Motor(-speed, speed);
  delay(130);

  // เดินโค้ง (ล้อขวาเร็วกว่า)
  int innerSpeed = (int)(speed * 0.58);
  Motor(innerSpeed, speed);
  delay(900);

  // 1. รอจนหลุดเส้น (เซ็นเซอร์ 4 เห็นดำ -> เห็นขาว)
  while (read_sensorA(4) < md_sensorA(4))
  {
    delayMicroseconds(50);
  }
  
  // 2. รอจนเจอเส้นอีกครั้ง (เซ็นเซอร์ 4 เห็นขาว -> เห็นดำ)
  while (read_sensorA(4) >= md_sensorA(4))
  {
    delayMicroseconds(50);
  }

  delay(timeMs); // เดินเลยเส้นไปนิดหน่อยตามเวลาที่กำหนด

  // เลี้ยวซ้ายเข้าเส้น
  TurnLeft();
}

void CurveRight(int speed, int timeMs)
{
  Motor(0, 0);
  delay(2);

  // หมุนขวาเล็กน้อยเพื่อตั้งลำ (ใช้ Turn แบบสวนทาง)
  Motor(speed, -speed);
  delay(130);

  // เดินโค้ง (ล้อซ้ายเร็วกว่า)
  int innerSpeed = (int)(speed * 0.58);
  Motor(speed, innerSpeed);
  delay(900);

  // 1. รอจนหลุดเส้น (เซ็นเซอร์ 3 เห็นดำ -> เห็นขาว)
  while (read_sensorA(3) < md_sensorA(3))
  {
    delayMicroseconds(50);
  }
  
  // 2. รอจนเจอเส้นอีกครั้ง (เซ็นเซอร์ 3 เห็นขาว -> เห็นดำ)
  while (read_sensorA(3) >= md_sensorA(3))
  {
    delayMicroseconds(50);
  }

  delay(timeMs); // เดินเลยเส้นไปนิดหน่อย

  // เลี้ยวขวาเข้าเส้น
  TurnRight();
}

