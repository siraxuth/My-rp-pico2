/*
 * =============================================================================
 *  CONFIG.h - ค่า Config รวมศูนย์ทั้งหมด
 * =============================================================================
 *
 *  รวมค่าคงที่และตัวแปรปรับแต่งจากทุกไฟล์ไว้ที่เดียว
 *  แก้ไขที่ไฟล์นี้แล้วจะมีผลกับทั้งโปรเจค
 *
 * =============================================================================
 */

#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
//  INITIAL - ค่าเริ่มต้น PID และความเร็ว (จาก Initial.ino)
// =============================================================================

extern float Kp, Ki, Kd, Kt;      // PID เดินหน้า
extern float KpB, KiB, KdB, KtB;  // PID ถอยหลัง
extern float Error, PreError;
extern int ErrorClear;

// =============================================================================
//  TRAC - เดินตามเส้น (จาก TracJC.ino)
// =============================================================================

extern int TRAC_SPEED;            // ความเร็วปกติ (default: 50)
extern int TRAC_SLOW;             // ความเร็วช้า (default: 30)
extern int TRAC_FAST;             // ความเร็วเร็ว (default: 70)

// =============================================================================
//  TURN - เลี้ยวด้วยเซ็นเซอร์ (จาก Turn.ino)
// =============================================================================

extern int TURN_Speed;            // ความเร็วเลี้ยว (default: 40)
extern int TURN_BrakeSpeed;       // ความเร็วเบรค (default: 100)
extern int TURN_BrakeTime;        // เวลาเบรค ms (default: 30)

// =============================================================================
//  IMU/GYRO - เลี้ยวด้วย Gyro (จาก TurnIMU.ino)
// =============================================================================

extern int IMU_TurnSpeed;         // ความเร็วหมุน (default: 70)
extern int IMU_BrakeSpeed;        // ความเร็วเบรค (default: 100)
extern int IMU_BrakeTime;         // เวลาเบรค ms (default: 50)
extern float IMU_Tolerance;       // ความคลาดเคลื่อน องศา (default: 2.0)

// =============================================================================
//  MOVECM - เดินตามระยะทาง (จาก MoveCM.ino)
// =============================================================================

extern float CM_PER_MS_AT_50;     // cm/ms ที่ speed 50 (default: 0.015)

extern float GYRO_STRAIGHT_KP;    // Gyro PID (default: 2.0)
extern float GYRO_STRAIGHT_KI;    // (default: 0.001)
extern float GYRO_STRAIGHT_KD;    // (default: 0.1)

extern float LINE_CM_KP;          // Line PID (default: 0.5)
extern float LINE_CM_KI;          // (default: 0.001)
extern float LINE_CM_KD;          // (default: 0.05)

// =============================================================================
//  ARM - แขนหุ่นยนต์ (จาก Arm.ino)
// =============================================================================

extern int current_arm_pos;       // ตำแหน่งแขน (default: 85)
extern int current_gripperL_pos;  // ตำแหน่ง Gripper ซ้าย (default: 0)
extern int current_gripperR_pos;  // ตำแหน่ง Gripper ขวา (default: 0)
extern bool gripper_is_open;      // สถานะ gripper (default: true)

// =============================================================================
//  SERVO POSITIONS (จาก MyRP_Pico2_6sensor.ino)
// =============================================================================

#define SERVO_DOWN      85        // แขนลง
#define SERVO_UP        170       // แขนขึ้น  
#define SERVOL_OPEN     0         // Gripper ซ้าย กาง
#define SERVOL_CLOSE    75        // Gripper ซ้าย หุบ
#define SERVOR_OPEN     0         // Gripper ขวา กาง
#define SERVOR_CLOSE    75        // Gripper ขวา หุบ
#define ARM_HEIGHT_POS  85        // ตำแหน่งแขนเริ่มต้น

// =============================================================================
//  ERROR CODES สำหรับ TracJC
// =============================================================================

#define ERROR_OFF_LINE   99       // หลุดเส้น
#define ERROR_JUNCTION   100      // แยก 4 ทาง
#define ERROR_LEFT_JC    101      // แยกซ้าย
#define ERROR_RIGHT_JC   102      // แยกขวา

// =============================================================================
//  DEFAULT VALUES (ใช้ใน InitialSpeed)
// =============================================================================

// PID Table ตาม BaseSpeed
// Speed <=50:  Kp=20, Kd=25,  KpB=12, KdB=60
// Speed <=60:  Kp=10, Kd=37,  KpB=15, KdB=80
// Speed <=70:  Kp=30, Kd=50,  KpB=16, KdB=100
// Speed <=80:  Kp=35, Kd=60,  KpB=20, KdB=120
// Speed <=90:  Kp=40, Kd=70,  KpB=25, KdB=140
// Speed >90:   Kp=45, Kd=80,  KpB=30, KdB=160

#endif // CONFIG_H
