/*
 * =============================================================================
 *  Arm.ino - ควบคุมแขนหุ่นยนต์ (MyRP_Pico2 Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    arm_ready()       - แขนลง กางฝ่ามือออกเตรียมเข้าไปคีบ
 *    arm_open_down()   - กางฝ่ามือออก และเอาแขนลง
 *    arm_down_open()   - เอาแขนลง และกางฝ่ามือออก
 *    arm_open_up()     - กางฝ่ามือออก และยกแขนขึ้น
 *    arm_up_open()     - เอาแขนขึ้น และกางฝ่ามือออก
 *    arm_down_close()  - เอาแขนลง และหุบมือเข้า
 *    arm_up_close()    - ยกแขนขึ้น และหุบมือเข้า
 *
 *  Smooth Servo:
 *    servo_smooth(ch, target, speed)  - เคลื่อน servo แบบ smooth
 *    arm_smooth(height)               - ยกแขนแบบ smooth ตามความสูง
 *
 *  ฟังก์ชันความสูง (Height 1-5):
 *    arm_height(h)     - ยกแขนตามความสูง 1-5 (smooth)
 *    GGU(), GGU1()-GGU5()  - Gripper Go Up ยกขึ้นระดับ 1-5
 *    GGD(), GGD1()-GGD5()  - Gripper Go Down ลงระดับ 1-5
 *
 *  คำสั่งย่อ (Gripper):
 *    GDP1() - GDP4()  - Gripper Drop Position 1-4 (วางของ)
 *    GGP1() - GGP4()  - Gripper Grab Position 1-4 (จับของ)
 *    GO()   - Gripper Open (กางมือ)
 *    GC()   - Gripper Close (หุบมือ)
 *
 *  Implementation: ใช้ myrp_pico2.h API
 * =============================================================================
 */


// ตำแหน่งปัจจุบันของ servo (ติดตามค่าองศาปัจจุบัน)
int current_arm_pos = 85;      // เริ่มที่ down
int current_gripperL_pos = 0;  // เริ่มที่ open
int current_gripperR_pos = 0;  // เริ่มที่ open

// สถานะ gripper (true = open, false = close)
bool gripper_is_open = true;

// =============================================================================
//  servo_smooth - เคลื่อน servo แบบ smooth
//  ch = channel, target = ตำแหน่งเป้าหมาย, spd = ความเร็ว (1-10, 1=ช้า, 10=เร็ว)
// =============================================================================

void servo_smooth(int ch, int target, int spd) {
  int current;

  // ดึงค่าปัจจุบันตาม channel
  if (ch == 1) current = current_arm_pos;
  else if (ch == 10) current = current_gripperL_pos;
  else if (ch == 0) current = current_gripperR_pos;
  else {
    servo(ch, target);
    return;
  }

  // คำนวณ delay ตาม speed (1=20ms, 10=2ms)
  int delayTime = 22 - (spd * 2);
  if (delayTime < 2) delayTime = 2;
  if (delayTime > 20) delayTime = 20;

  // เคลื่อนที่แบบ smooth
  if (current < target) {
    for (int pos = current; pos <= target; pos++) {
      servo(ch, pos);
      delay(delayTime);
    }
  } else {
    for (int pos = current; pos >= target; pos--) {
      servo(ch, pos);
      delay(delayTime);
    }
  }

  // อัพเดทค่าปัจจุบัน
  if (ch == 1) current_arm_pos = target;
  else if (ch == 10) current_gripperL_pos = target;
  else if (ch == 0) current_gripperR_pos = target;
}

// =============================================================================
//  servo_smooth_dual - เคลื่อน 2 servo พร้อมกันแบบ smooth (สำหรับ gripper)
// =============================================================================

void servo_smooth_dual(int ch1, int target1, int ch2, int target2, int spd) {
  int current1 = (ch1 == 10) ? current_gripperL_pos : current_gripperR_pos;
  int current2 = (ch2 == 10) ? current_gripperL_pos : current_gripperR_pos;

  int delayTime = 22 - (spd * 2);
  if (delayTime < 2) delayTime = 2;
  if (delayTime > 20) delayTime = 20;

  int steps = max(abs(target1 - current1), abs(target2 - current2));
  if (steps == 0) return;

  float step1 = (float)(target1 - current1) / steps;
  float step2 = (float)(target2 - current2) / steps;

  for (int i = 0; i <= steps; i++) {
    servo(ch1, current1 + (int)(step1 * i));
    servo(ch2, current2 + (int)(step2 * i));
    delay(delayTime);
  }

  // อัพเดทค่าปัจจุบัน
  if (ch1 == 10) current_gripperL_pos = target1;
  if (ch1 == 0) current_gripperR_pos = target1;
  if (ch2 == 10) current_gripperL_pos = target2;
  if (ch2 == 0) current_gripperR_pos = target2;
}

// =============================================================================
//  arm_height - ยกแขนตามความสูง 0-5 แบบ smooth
//  h = 0 (ลงสุด) ถึง 5 (ขึ้นสุด)
// =============================================================================

void arm_height(int h) {
  if (h < 0) h = 0;
  if (h > 5) h = 5;
  servo_smooth(1, arm_height_pos[h], 5);
}

void arm_height(int h, int spd) {
  if (h < 0) h = 0;
  if (h > 5) h = 5;
  servo_smooth(1, arm_height_pos[h], spd);
}

// =============================================================================
//  arm_up / arm_down - ยก/ลดแขนแบบ smooth
// =============================================================================

void arm_up() {
  servo_smooth(1, servo_up, 5);
}

void arm_up(int spd) {
  servo_smooth(1, servo_up, spd);
}

void arm_down() {
  servo_smooth(1, servo_down, 5);
}

void arm_down(int spd) {
  servo_smooth(1, servo_down, spd);
}

// =============================================================================
//  gripper_open / gripper_close - กาง/หุบมือแบบ smooth
//  ใช้ค่า servoL_open, servoL_close, servoR_open, servoR_close ที่กำหนดไว้
// =============================================================================

void gripper_open() {
  servo_smooth_dual(10, servoL_open, 0, servoR_open, 5);
  gripper_is_open = true;
}

void gripper_open(int spd) {
  servo_smooth_dual(10, servoL_open, 0, servoR_open, spd);
  gripper_is_open = true;
}

void gripper_close() {
  servo_smooth_dual(10, servoL_close, 0, servoR_close, 5);
  gripper_is_open = false;
}

void gripper_close(int spd) {
  servo_smooth_dual(10, servoL_close, 0, servoR_close, spd);
  gripper_is_open = false;
}

// =============================================================================
//  gripper_to - เคลื่อน gripper ไปที่ตำแหน่งที่กำหนด (องศา)
// =============================================================================

void gripper_to(int leftPos, int rightPos) {
  servo_smooth_dual(10, leftPos, 0, rightPos, 5);
}

void gripper_to(int leftPos, int rightPos, int spd) {
  servo_smooth_dual(10, leftPos, 0, rightPos, spd);
}

// =============================================================================
//  get_gripper_pos - ดึงค่าองศาปัจจุบันของ gripper
// =============================================================================

int get_gripperL_pos() { return current_gripperL_pos; }
int get_gripperR_pos() { return current_gripperR_pos; }
int get_arm_pos() { return current_arm_pos; }
bool is_gripper_open() { return gripper_is_open; }

// =============================================================================
//  set_gripper_config - ตั้งค่าตำแหน่งกาง/หุบของ gripper
// =============================================================================

void set_gripperL_config(int openPos, int closePos) {
  servoL_open = openPos;
  servoL_close = closePos;
}

void set_gripperR_config(int openPos, int closePos) {
  servoR_open = openPos;
  servoR_close = closePos;
}

void set_gripper_config(int L_open, int L_close, int R_open, int R_close) {
  servoL_open = L_open;
  servoL_close = L_close;
  servoR_open = R_open;
  servoR_close = R_close;
}

// =============================================================================
//  sync_gripper_pos - sync ค่าปัจจุบันของ gripper (เรียกหลัง setup)
// =============================================================================

void sync_gripper_pos() {
  current_gripperL_pos = servoL_open;
  current_gripperR_pos = servoR_open;
  current_arm_pos = servo_down;
  gripper_is_open = true;
}

void sync_gripper_pos(int armPos, int leftPos, int rightPos) {
  current_arm_pos = armPos;
  current_gripperL_pos = leftPos;
  current_gripperR_pos = rightPos;
  // ตรวจสอบสถานะ open/close
  gripper_is_open = (leftPos == servoL_open && rightPos == servoR_open);
}

// =============================================================================
//  arm_init - เริ่มต้น servo ไปตำแหน่งที่แน่นอน (เรียกใน setup)
//  เนื่องจาก servo ไม่มี feedback ไม่สามารถรู้ตำแหน่งจริงได้
//  ต้องสั่งให้ไปตำแหน่งที่รู้แน่นอนก่อน
// =============================================================================

void arm_init() {
  // สั่ง servo ไปตำแหน่ง open ทันที (ไม่ใช้ smooth เพราะไม่รู้ตำแหน่งเริ่มต้น)
  servo(1, servo_down);
  servo(10, servoL_open);
  servo(0, servoR_open);
  delay(500);  // รอให้ servo ไปถึงตำแหน่ง

  // sync ค่าปัจจุบัน
  current_arm_pos = servo_down;
  current_gripperL_pos = servoL_open;
  current_gripperR_pos = servoR_open;
  gripper_is_open = true;
}

void arm_init_close() {
  // สั่ง servo ไปตำแหน่ง close ทันที
  servo(1, servo_down);
  servo(10, servoL_close);
  servo(0, servoR_close);
  delay(500);

  current_arm_pos = servo_down;
  current_gripperL_pos = servoL_close;
  current_gripperR_pos = servoR_close;
  gripper_is_open = false;
}

void arm_init_pos(int armPos, int leftPos, int rightPos) {
  // สั่ง servo ไปตำแหน่งที่กำหนด
  servo(1, armPos);
  servo(10, leftPos);
  servo(0, rightPos);
  delay(500);

  current_arm_pos = armPos;
  current_gripperL_pos = leftPos;
  current_gripperR_pos = rightPos;
  gripper_is_open = (leftPos == servoL_open && rightPos == servoR_open);
}

// =============================================================================
//  คำสั่งย่อ GO / GC - Gripper Open / Close
// =============================================================================

void GO() { gripper_open(); }
void GO(int spd) { gripper_open(spd); }
void GC() { gripper_close(); }
void GC(int spd) { gripper_close(spd); }

// =============================================================================
//  GGU - Gripper Go Up (ยกขึ้นระดับ 0-5)
// =============================================================================

void GGU() { arm_height(5); }        // ยกสุด
void GGU(int spd) { arm_height(5, spd); }
void GGU1() { arm_height(1); }
void GGU1(int spd) { arm_height(1, spd); }
void GGU2() { arm_height(2); }
void GGU2(int spd) { arm_height(2, spd); }
void GGU3() { arm_height(3); }
void GGU3(int spd) { arm_height(3, spd); }
void GGU4() { arm_height(4); }
void GGU4(int spd) { arm_height(4, spd); }
void GGU5() { arm_height(5); }
void GGU5(int spd) { arm_height(5, spd); }

// =============================================================================
//  GGD - Gripper Go Down (ลงระดับ 0-5)
// =============================================================================

void GGD() { arm_height(0); }        // ลงสุด
void GGD(int spd) { arm_height(0, spd); }
void GGD1() { arm_height(1); }
void GGD1(int spd) { arm_height(1, spd); }
void GGD2() { arm_height(2); }
void GGD2(int spd) { arm_height(2, spd); }
void GGD3() { arm_height(3); }
void GGD3(int spd) { arm_height(3, spd); }
void GGD4() { arm_height(4); }
void GGD4(int spd) { arm_height(4, spd); }
void GGD5() { arm_height(5); }
void GGD5(int spd) { arm_height(5, spd); }

// =============================================================================
//  GDP - Gripper Drop Position (ลงไปวางของ)
//  ลงไประดับที่กำหนด แล้วกางมือ
// =============================================================================

void GDP1() { arm_height(1); delay(100); gripper_open(); }
void GDP2() { arm_height(2); delay(100); gripper_open(); }
void GDP3() { arm_height(3); delay(100); gripper_open(); }
void GDP4() { arm_height(4); delay(100); gripper_open(); }

void GDP1(int spd) { arm_height(1, spd); delay(100); gripper_open(spd); }
void GDP2(int spd) { arm_height(2, spd); delay(100); gripper_open(spd); }
void GDP3(int spd) { arm_height(3, spd); delay(100); gripper_open(spd); }
void GDP4(int spd) { arm_height(4, spd); delay(100); gripper_open(spd); }

// =============================================================================
//  GGP - Gripper Grab Position (ลงไปจับของ)
//  กางมือ ลงไประดับที่กำหนด แล้วหุบมือ
// =============================================================================

void GGP1() { gripper_open(); arm_height(1); delay(100); gripper_close(); }
void GGP2() { gripper_open(); arm_height(2); delay(100); gripper_close(); }
void GGP3() { gripper_open(); arm_height(3); delay(100); gripper_close(); }
void GGP4() { gripper_open(); arm_height(4); delay(100); gripper_close(); }

void GGP1(int spd) { gripper_open(spd); arm_height(1, spd); delay(100); gripper_close(spd); }
void GGP2(int spd) { gripper_open(spd); arm_height(2, spd); delay(100); gripper_close(spd); }
void GGP3(int spd) { gripper_open(spd); arm_height(3, spd); delay(100); gripper_close(spd); }
void GGP4(int spd) { gripper_open(spd); arm_height(4, spd); delay(100); gripper_close(spd); }

// =============================================================================
//  ฟังก์ชันเดิม (Legacy) - ใช้งานได้เหมือนเดิม
// =============================================================================

void arm_ready() {  //--------->> แขนลง กางฝ่ามือออกเตรียมเข้าไปคีบ
  servo(1, servo_down);
  servo(10, servoL_open - 30);
  servo(0, servoR_open - 30);
}

void arm_open_down() {  //--------->>  กางฝ่ามือออก  และเอาแขนลง
  servo(10, servoL_open);
  servo(0, servoR_open);
  delay(300);
  servo(1, servo_down);
}

void arm_down_open() {  //--------->>  เอาแขนลง  และ กางฝ่ามือออก
  servo(1, servo_down);
  delay(300);
  servo(10, servoL_open);
  servo(0, servoR_open);
}

void arm_open_up() {  //--------->>  กางฝ่ามือออก  และยกแขนขึ้น
  servo(10, servoL_open);
  servo(0, servoR_open);
  delay(300);
  servo(1, servo_down + 95);
}

void arm_up_open() {  //--------->>  เอาแขนขึ้น  และ กางฝ่ามือออก
  servo(1, servo_down + 95);
  delay(300);
  servo(10, servoL_open);
  servo(0, servoR_open);
}

void arm_down_close() {  //--------->>  เอาแขนลง  และ หุบมือเข้า
  servo(1, servo_down);
  delay(100);
  servo(10, servoL_open - 115);
  servo(0, servoR_open - 111);
}

void arm_up_close() {  //--------->>  ยกแขนขึ้น  และ หุบมือเข้า
  servo(1, servo_down + 95);
  delay(100);
  servo(10, servoL_open - 91);
  servo(0, servoR_open - 91);
}

void arm_big_box() {  //--------->>  คีบกล่องใหญ่
  servo(1, servo_down);
  servo(10, servoL_open - 30);
  servo(0, servoR_open - 30);
}

// =============================================================================
//  arm_Xcm_open - กางมือแล้วลงไประดับต่างๆ (เตรียมคีบ)
//  ใช้ความสูงจาก arm_height_pos และกางมือ
// =============================================================================

void arm_5cm_open() {
  servo(1, arm_height_pos[5]);
  servo(10, servoL_open);
  servo(0, servoR_open);
  current_arm_pos = arm_height_pos[5];
  current_gripperL_pos = servoL_open;
  current_gripperR_pos = servoR_open;
  gripper_is_open = true;
}

void arm_4cm_open() {
  servo(1, arm_height_pos[4]);
  servo(10, servoL_open);
  servo(0, servoR_open);
  current_arm_pos = arm_height_pos[4];
  current_gripperL_pos = servoL_open;
  current_gripperR_pos = servoR_open;
  gripper_is_open = true;
}

void arm_3cm_open() {
  servo(1, arm_height_pos[3]);
  servo(10, servoL_open);
  servo(0, servoR_open);
  current_arm_pos = arm_height_pos[3];
  current_gripperL_pos = servoL_open;
  current_gripperR_pos = servoR_open;
  gripper_is_open = true;
}

void arm_2cm_open() {
  servo(1, arm_height_pos[2]);
  servo(10, servoL_open);
  servo(0, servoR_open);
  current_arm_pos = arm_height_pos[2];
  current_gripperL_pos = servoL_open;
  current_gripperR_pos = servoR_open;
  gripper_is_open = true;
}

// =============================================================================
//  arm_pick / arm_drop - จับ/วางของ
// =============================================================================

void arm_pick() {
  gripper_close();
  delay(300);
  arm_up();
}

void arm_pick(int spd) {
  gripper_close(spd);
  delay(300);
  arm_up(spd);
}

void arm_drop() {
  arm_down();
  delay(300);
  gripper_open();
}

void arm_drop(int spd) {
  arm_down(spd);
  delay(300);
  gripper_open(spd);
}

// =============================================================================
//  ตั้งค่าความสูงแต่ละระดับ (ปรับ calibrate)
// =============================================================================

void set_arm_height(int level, int pos) {
  if (level >= 0 && level <= 5) {
    arm_height_pos[level] = pos;
  }
}

void set_servo_limits(int down, int up) {
  servo_down = down;
  servo_up = up;
  arm_height_pos[0] = down;
  arm_height_pos[5] = up;
}

void set_gripper_open(int left, int right) {
  servoL_open = left;
  servoR_open = right;
}

