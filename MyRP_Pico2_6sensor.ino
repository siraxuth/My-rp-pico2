/*
 * =============================================================================
 *  MyRP_Pico2_6sensor.ino - Main File
 * =============================================================================
 *
 *  Example code for MyRP Pico2 Robot with 6 sensors
 *  Uses myrp_pico2.h library functions directly
 *
 *  ไฟล์ที่แยกออกไป:
 *    - Initial.ino   : ตั้งค่าเริ่มต้น, Speed Profile, PID
 *    - Turn.ino      : เลี้ยว TL, TR, UTL, UTR, Curve
 *    - TracJC.ino    : เดินตามเส้น TracJC, FL, BL
 *    - Arm.ino       : ควบคุมแขน arm_open, arm_close, arm_pick
 *    - ShortCmd.ino  : คำสั่งย่อ FD, BK, TJC, etc.
 *    - Debug.ino     : Debug sensors, gyro, position
 *    - TurnIMU.ino   : เลี้ยวด้วย Gyro (degree)
 *    - MoveCM.ino    : เดินหน้า/ถอยหลังตาม cm
 *    - Trac.ino      : TracSpeedTime, TracJCSpeed (legacy)
 *
 * =============================================================================
 */

#include "myrp_pico2.h"
// =============================================================================
//  ค่าเริ่มต้น Servo สำหรับแขน
// =============================================================================
#define D(x) delay(x)
int servo_down = 35; // ตำแหน่งแขนลง (ระดับ 0)
int servo_up = 50;   // ตำแหน่งแขนขึ้นสุด (ระดับ 5)

// ค่า servo Left/Right สำหรับกาง/หุบ (ตั้งค่าตามแขนจริง)
int servoL_open = 120; // มือซ้าย: ตำแหน่งกาง
int servoL_close = 60; // มือซ้าย: ตำแหน่งหุบ
int servoR_open = 120; // มือขวา: ตำแหน่งกาง
int servoR_close = 60; // มือขวา: ตำแหน่งหุบ

// ตำแหน่ง servo ตามความสูง (ปรับได้)
// height 1 = ต่ำสุด, height 5 = สูงสุด
int arm_height_pos[6] = {
    85, // height 0 (down) = servo_down
    75, // height 1
    65, // height 2
    60, // height 3
    55, // height 4
    50  // height 5 (up) = servo_up
};

// =============================================================================
//  Setup
// =============================================================================

void setup() {
  Serial.begin(9600);
  // arm_init();
  setup_robot();
  set_motor();
  encoder.setupEncoder();  //-------------------->> เรียกฟังก์ชัน setupEncoder
  encoder.resetEncoders(); //--------------------->> ฟังก์ชันรอก
  // arm_5cm_open();
  arm_down_open();

  // ตั้งค่าเริ่มต้น (เลือกวิธีใดวิธีหนึ่ง)
  // InitRobot();           // ค่า default (speed 50)
  // InitRobot(60);         // กำหนดความเร็ว
  // InitRobot(60, 0.6, 0.04); // กำหนดความเร็ว + PID

  // Quick setup
  // InitSlow();    // speed 30
  // InitMedium();  // speed 50
  // InitFast();    // speed 70
  // InitTurbo();   // speed 90

  // รอกดปุ่ม
  sw();
  // arm_down_close();
  // FCM(30, 20);

  // ===== ใส่โค้ดหุ่นยนต์ที่นี่ =====

  // ตัวอย่าง: เดินจนเจอแยก แล้วเลี้ยวขวา
  TracJC(50);
  TurnRight();

  // ตัวอย่าง: เดินหน้า 10 cm
  // FD(50, 10);

  // ตัวอย่าง: เลี้ยวด้วย Gyro
  // TR_deg(50, 90);

  // =====================================

  Motor(0, 0);
  delay(100);
}

// =============================================================================
//  Loop
// =============================================================================

void loop() {

}

void set_motor()
{
  set_Freq("Coreless_Motors"); // กำหนดความถี่ให้กับมอเตอร์เกาหลีหรือ มอเตอร์ Coreless ("Coreless_Motors")
                               // set_Freq("DC_Motors");        // กำหนดความถี่ให้กับมอเตอร์ธรรมกา(DC_Motors)

  distance_scale_fw(1.00); // ปรับค่าเพื่อให้ได้ระยะทางที่เป็นจริงสำหรับเดินหน้า
  distance_scale_bw(1.10); // ปรับค่าเพื่อให้ได้ระยะทางที่เป็นจริงสำหรับถอยหลัง

  set_slow_motor(20, 20);
  set_turn_center_l(-90, 90);
  set_turn_center_r(90, -90);
  set_turn_front_l(-10, 100);
  set_turn_front_r(100, -10);
  set_brake_fc(5, 20);
  set_brake_bc(5, 20);
  set_delay_f(10);
}
