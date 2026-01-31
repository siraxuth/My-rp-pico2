#include <myrp_pico2.h>

#include "CONFIG.h"

int Speed, BaseSpeed, ACCSpeed, SlowSpeed, LeftBaseSpeed, RightBaseSpeed, MaxSpeed, TurnSpeed;
long Timer, Timer2, Timer3;

//----------------------------------------------------------------------------->> ตั้งค่ามือจับ
int servo_down = 55;   //-------------------->> ตั้งค่า มือจับลงขนานกับพื้น
int servoL_open = 120; //-------------------->> ตั้งค่า กางฝ่ามือด้านซ้าย
int servoR_open = 60; //-------------------->> ตั้งค่า กางฝ่ามือด้านขวา
//----------------------------------------------------------------------------->> ตั้งค่ามือจับ

#define MCP3421_ADDR 0x68 // I2C Address when A0 = GND

// ประกาศตัวแปรสำหรับการควบคุมแขนกล
int servo_up = 90;                                      // ตำแหน่งแขนยกสูงสุด
int servoL_close = 90;                                  // ตำแหน่งปิดของ gripper ซ้าย
int servoR_close = 90;                                  // ตำแหน่งปิดของ gripper ขวา
int arm_height_pos[] = {163, 151, 150, 143, 142, 126 , 70}; // อาร์เรย์เก็บตำแหน่งความสูงของแขน (0-6 ระดับ)
// int arm_height_pos[] = {137, 126, 115, 108, 97, 90, 70}; // อาร์เรย์เก็บตำแหน่งความสูงของแขน (0-6 ระดับ)

enum SpinDir
{
  LEFT,
  RIGHT
};
enum SpinMode
{
  IN,
  OUT
};
// enum SensorDir
// {
//   S0,
//   S1,
//   S2,
//   S3,
//   S4,
//   S5,
//   S6,
//   S7
// }; // <-- เพิ่ม semicolon ตรงนี้
// const char *SensorDirToStr[] = {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};

// Error สำหรับ PID
float Error = 0;
float PreError = 0;
int ErrorClear = 0; // Flag สำหรับ reset error
int BlackLine = 0; // Flag สำหรับเส้นดำ

void setup()
{
  // Serial.begin(115200);
  Speed = 50;
  ACCSpeed = 60;
  TurnSpeed = 60;
  BaseSpeed = Speed; // ความเร็วพื้นฐาน (ตั้งค่าก่อนเรียก InitialSpeed)
  setup_robot();
  set_motor();

  //----------------------------------------------------------------------------->> ตั้งค่ามือจับ
  // s34_trim(0);
  // s35_trim(20);
  // s36_trim(0);
  // servo(1, 135);
  //----------------------------------------------------------------------------->> ตั้งค่ามือจับ

  arm_down_open();


  //------------------------->>  ส่วนของคำสั่งในการเตรียมแขนกล
  // my.resetAngles();
  // my.reCalibrateGyro();
  // sw();
  // bw_gyro(30, 30, 0.25, 20, "c0", 's', 1);

  // TurnRight();
  // TracJC();
  // TurnLeft();
  // TC(50, 2000);
  // fline4sensor(30, 30, 1.2, "a70", 'f', 'p', 0, "a0",1);
  // for (int i = 0; i < 30; i++)
  // {
    /* code */
  // }
  // GC();


  // TracSpeedTime(30, 114, my, 2);
  // sw();
  // GGU();
  // arm_height(0);
  // GC();
  // arm_height(5);
  // delay(500);
  // GO();
  
  // sw();
  // arm_height(1);
  // GC();
  // arm_height(5);
  // delay(500);
  // GO();

  // sw();
  // arm_height(2);
  // GC();
  // arm_height(5);
  // delay(500);
  // GO();

  // sw();

  // arm_height(3);
  // GC();
  // arm_height(5);
  // delay(500);
  // GO();

  // fline4sensor(30, 30, 0.95, "a7", 'f', 's', 0, "0", 1);
  // arm_height(4);
  // GC();
  // arm_height(5);
  // delay(500);
  // GO();

  // arm_height(5);
  // GC();
  // arm_height(5);
  // delay(500);
  // GO();
  sw();
  GGU();
  sw();
  fw_gyro(20, 20, 0.55, 12, 1);
  BackwardSpeedTime(20, 105);
  delay(1000);
  GDP();
  
  sw();
  GGU1();
  delay(2000);
  GDP1();
  
  sw();
  GGU2();
  delay(2000);
  GDP2();
  
  sw();
  GGU3();
  delay(2000);
  GDP3();

  sw();
  GGU4();
  delay(2000);
  GDP4();
  
  sw();
  GGU5();
  delay(2000);
  

  // TracSpeedDistance(30, 10.0, my);
  // fw_gyro(25,25, 0.25, 15, 0);
  sw(); //--->> คำสั่งรอกดปุ่ม
  // TurnLeft();
  ///----------------------------------------------------- ------------------------->> Ball1
  Ball1();
  // sw();
  ///----------------------------------------------------- ------------------------->> Ball2
  Ball2();
  ///----------------------------------------------------- ------------------------->> Ball3
  Ball3();
  ///----------------------------------------------------- ------------------------->> Ball4
  Ball4();
  ///----------------------------------------------------- ------------------------->> Ball5
  Ball5();
  ///----------------------------------------------------- ------------------------->> Ball6
  Ball6();
  ///----------------------------------------------------- ------------------------->> Ball7
  Ball7();
  /// ----------------------------------------------------- ------------------------->> Ball8
  Ball8();
  ///----------------------------------------------------- ------------------------->> Ball9


  Finish();

  ////----------------------------------------------------- ------------------------->> จบการรันคำสั่งต่าง ๆ
}

void loop()
{
  // Serial.print(analogRead(A0));
  // Serial.print("  ");
  // Serial.print(analogRead(A1));
  // Serial.print("     ");
  // Serial.print(md_sensorC(0));
  // Serial.print("  ");
  // Serial.println(md_sensorC(1));

  // Serial.print(sensorMinC[0]);
  // Serial.print("   ");
  // Serial.print(sensorMinC[1]);
  // Serial.print("   ");
  // delay(10);

  // Serial.print("X: ");
  // Serial.print(my.gyro('x'), 2);
  // Serial.print("  Y: ");
  // Serial.print(my.gyro('y'), 2);
  // Serial.print("  Z: ");
  // Serial.println(my.gyro('z'), 2);

  // delay(100);
}

void set_motor()
{
  set_Freq("Coreless_Motors"); // กำหนดความถี่ให้กับมอเตอร์เกาหลีหรือ มอเตอร์ Coreless ("Coreless_Motors")
  // set_Freq("DC_Motors");    // กำหนดความถี่ให้กับมอเตอร์ธรรมกา(DC_Motors)

  distance_scale_fw(1.00); // ปรับค่าเพื่อให้ได้ระยะทางที่เป็นจริงสำหรับเดินหน้า
  distance_scale_bw(1.10); // ปรับค่าเพื่อให้ได้ระยะทางที่เป็นจริงสำหรับถอยหลัง
  set_kd_f(1.12);          // ปรับค่า kd สำหรับการเบรกเดินหน้า
  set_slow_motor(20, 20);
  set_turn_center_l(-90, 90);
  set_turn_center_r(90, -90);
  set_turn_front_l(-100, 100);
  set_turn_front_r(100, -100);
  set_brake_fc(5, 20);
  set_brake_bc(5, 20);
  set_delay_f(10);
}