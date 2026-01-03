/*
 * =============================================================================
 *  Debug.ino - ฟังก์ชัน Debug (MyRP_Pico2 Version)
 * =============================================================================
 *
 *  ฟังก์ชันหลัก:
 *    debug_sensors()   - แสดงค่าเซ็นเซอร์
 *    debug_gyro()      - แสดงค่า Gyro
 *    debug_position()  - แสดงค่าตำแหน่ง
 *    debug_all()       - แสดงทั้งหมด
 *
 *  Implementation: ใช้ myrp_pico2.h API
 * =============================================================================
 */

// extern variables from Initial.ino
extern float INIT_KP;
extern float INIT_KI;
extern float INIT_KD;
extern int INIT_Speed;

// =============================================================================
//  debug_sensors - แสดงค่าเซ็นเซอร์
// =============================================================================

void debug_sensors() {
  Serial.print("A: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(read_sensorA(i));
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("B: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(read_sensorB(i));
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("C: ");
  Serial.print(analogRead(26));
  Serial.print(" ");
  Serial.println(analogRead(27));
}

// =============================================================================
//  debug_sensors_digital - แสดงค่าเซ็นเซอร์แบบ 0/1
// =============================================================================

void debug_sensors_digital() {
  int s0 = (read_sensorA(0) < md_sensorA(0)) ? 0 : 1;
  int s2 = (read_sensorA(2) < md_sensorA(2)) ? 0 : 1;
  int s3 = (read_sensorA(3) < md_sensorA(3)) ? 0 : 1;
  int s4 = (read_sensorA(4) < md_sensorA(4)) ? 0 : 1;
  int s5 = (read_sensorA(5) < md_sensorA(5)) ? 0 : 1;
  int s7 = (read_sensorA(7) < md_sensorA(7)) ? 0 : 1;

  Serial.print("S: ");
  Serial.print(s0); Serial.print(" ");
  Serial.print(s2); Serial.print(" ");
  Serial.print(s3); Serial.print(" ");
  Serial.print(s4); Serial.print(" ");
  Serial.print(s5); Serial.print(" ");
  Serial.println(s7);
}

// =============================================================================
//  debug_sensors_mid - แสดงค่ากลางเซ็นเซอร์
// =============================================================================

void debug_sensors_mid() {
  Serial.print("Mid A: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(md_sensorA(i));
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Mid B: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(md_sensorB(i));
    Serial.print(" ");
  }
  Serial.println();
}

// =============================================================================
//  debug_gyro - แสดงค่า Gyro
// =============================================================================

void debug_gyro() {
  Serial.print("Gyro Z: ");
  Serial.println(my.gyro('z'));
}

void debug_gyro_all() {
  Serial.print("Gyro X: "); Serial.print(my.gyro('x'));
  Serial.print(" Y: "); Serial.print(my.gyro('y'));
  Serial.print(" Z: "); Serial.println(my.gyro('z'));
}

// =============================================================================
//  debug_position - แสดงค่าตำแหน่ง
// =============================================================================

void debug_position() {
  Serial.print("Position A: ");
  Serial.print(position_A());
  Serial.print(" Error: ");
  Serial.println(error_A());
}

void debug_position_all() {
  Serial.print("Pos A: "); Serial.print(position_A());
  Serial.print(" Err A: "); Serial.print(error_A());
  Serial.print(" | Pos B: "); Serial.print(position_B());
  Serial.print(" Err B: "); Serial.println(error_B());
}

// =============================================================================
//  debug_pid - แสดงค่า PID
// =============================================================================

void debug_pid() {
  Serial.println("=== PID Settings ===");
  Serial.print("INIT_KP: "); Serial.println(INIT_KP);
  Serial.print("INIT_KI: "); Serial.println(INIT_KI);
  Serial.print("INIT_KD: "); Serial.println(INIT_KD);
  Serial.print("INIT_Speed: "); Serial.println(INIT_Speed);
  Serial.println("====================");
}

// =============================================================================
//  debug_all - แสดงทั้งหมด
// =============================================================================

void debug_all() {
  Serial.println("========== DEBUG ALL ==========");
  debug_sensors();
  debug_sensors_digital();
  debug_gyro_all();
  debug_position_all();
  Serial.println("===============================");
}

// =============================================================================
//  debug_loop - แสดงค่าต่อเนื่อง
// =============================================================================

void debug_loop(int delayMs) {
  while (1) {
    debug_sensors_digital();
    debug_gyro();
    delay(delayMs);
  }
}

void debug_sensor_loop(int delayMs) {
  while (1) {
    debug_sensors();
    delay(delayMs);
  }
}

void debug_gyro_loop(int delayMs) {
  while (1) {
    debug_gyro_all();
    delay(delayMs);
  }
}

// =============================================================================
//  Test Functions
// =============================================================================

void test_motor() {
  Serial.println("Testing Motor...");

  Serial.println("Forward");
  Motor(50, 50);
  delay(1000);

  Serial.println("Backward");
  Motor(-50, -50);
  delay(1000);

  Serial.println("Turn Left");
  Motor(-50, 50);
  delay(500);

  Serial.println("Turn Right");
  Motor(50, -50);
  delay(500);

  Motor(0, 0);
  Serial.println("Motor Test Complete");
}

void test_servo() {
  Serial.println("Testing Servo...");

  for (int i = 0; i <= 180; i += 30) {
    Serial.print("Servo 0: "); Serial.println(i);
    servo(0, i);
    delay(500);
  }

  Serial.println("Servo Test Complete");
}

void test_all() {
  test_motor();
  delay(1000);
  test_servo();
}

