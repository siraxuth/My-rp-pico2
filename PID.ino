int a0 = read_sensorA(0);
int a2 = read_sensorA(2);
int a3 = read_sensorA(3);
int a4 = read_sensorA(4);
int a5 = read_sensorA(5);
int a7 = read_sensorA(7);

void ConvertADC() {
    if (a0<md_sensorA(0)) a0 = (0+BlackLine)%2;
    else a0 = (1+BlackLine)%2;
    if (a2<md_sensorA(2)) a2 = (0+BlackLine)%2;
    else a2 = (1+BlackLine)%2;
    if (a3<md_sensorA(3)) a3 = (0+BlackLine)%2;
    else a3 = (1+BlackLine)%2;
    if (a4<md_sensorA(4)) a4 = (0+BlackLine)%2;
    else a4 = (1+BlackLine)%2;
    if (a5<md_sensorA(5)) a5 = (0+BlackLine)%2;
    else a5 = (1+BlackLine)%2;
    if (a7<md_sensorA(7)) a7 = (0+BlackLine)%2;
    else a7 = (1+BlackLine)%2;
}

// ฟังก์ชัน CalError สำหรับ 6 เซนเซอร์
void CalError6Sensor()
{
    ConvertADC();

    // เซนเซอร์ทั้งหมดเห็นขาว (ออกจากเส้น)
    if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = previous_error; // คงค่า error เดิม
    }
    // เซนเซอร์ทางขวาสุด
    else if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 1 && a5 == 1 && a7 == 0)
    {
        errors = 5;
    }
    else if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 1 && a5 == 0 && a7 == 0)
    {
        errors = 4;
    }
    else if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 1 && a5 == 0 && a7 == 1)
    {
        errors = 3;
    }
    // เซนเซอร์กลางขวา
    else if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 0 && a5 == 0 && a7 == 1)
    {
        errors = 2;
    }
    else if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 0 && a5 == 1 && a7 == 1)
    {
        errors = 1;
    }
    // เซนเซอร์กลาง (ตรง)
    else if (a0 == 1 && a2 == 1 && a3 == 0 && a4 == 0 && a5 == 1 && a7 == 1)
    {
        errors = 0;
    }
    else if (a0 == 1 && a2 == 1 && a3 == 0 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = 0;
    }
    else if (a0 == 1 && a2 == 0 && a3 == 0 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = 0;
    }
    // เซนเซอร์กลางซ้าย
    else if (a0 == 1 && a2 == 0 && a3 == 1 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = -1;
    }
    else if (a0 == 1 && a2 == 0 && a3 == 0 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = -2;
    }
    // เซนเซอร์ทางซ้าย
    else if (a0 == 1 && a2 == 0 && a3 == 1 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = -3;
    }
    else if (a0 == 0 && a2 == 0 && a3 == 1 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = -4;
    }
    else if (a0 == 0 && a2 == 1 && a3 == 1 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = -5;
    }
    // แยกทาง (เจอแยก)
    else if (a0 == 0 && a2 == 0 && a3 == 0 && a4 == 0 && a5 == 0 && a7 == 0)
    {
        errors = 100; // T-Junction
    }
    else if (a0 == 0 && a2 == 0 && a3 == 0 && a4 == 1 && a5 == 1 && a7 == 1)
    {
        errors = 101; // Left Junction
    }
    else if (a0 == 1 && a2 == 1 && a3 == 1 && a4 == 0 && a5 == 0 && a7 == 0)
    {
        errors = 102; // Right Junction
    }
}

// ฟังก์ชัน PID สำหรับเดินตามเส้น
void TracPID6Sensor(int LeftBaseSpeed, int RightBaseSpeed, float kp, float ki, float kd)
{
    CalError6Sensor();

    // คำนวณ PID
    P = errors;
    I += errors;
    D = errors - previous_error;

    // จำกัด I-term
    if (I > 1000)
        I = 1000;
    if (I < -1000)
        I = -1000;

    PID_output = (kp * P) + (ki * I) + (kd * D);

    int LeftSpeed = LeftBaseSpeed - PID_output;
    int RightSpeed = RightBaseSpeed + PID_output;

    // จำกัดความเร็ว
    if (LeftSpeed > 100)
        LeftSpeed = 100;
    if (LeftSpeed < -100)
        LeftSpeed = -100;
    if (RightSpeed > 100)
        RightSpeed = 100;
    if (RightSpeed < -100)
        RightSpeed = -100;

    // ส่งค่าไปมอเตอร์
    Motor(LeftSpeed, RightSpeed);
    // motor(2, RightSpeed);

    previous_error = errors;
}

// ฟังก์ชัน PID แบบละเอียด (ตาม fline4sensor)
void TracPID6SensorAdvanced(int spl, int spr, float kp, float ki, float kd)
{
    ConvertADC();
    CalError6Sensor();

    // ลด Kp เมื่ออยู่ตรงกลาง (error เล็ก)
    float kp_used;
    if (abs(errors) < 2)
    {
        kp_used = kp * 0.3; // ลดเหลือ 30% เมื่อใกล้ตรง
    }
    else
    {
        kp_used = kp;
    }

    // คำนวณ PID
    P = errors;
    I += errors * 0.00005;
    D = errors - previous_error;
    previous_error = errors;

    PID_output = (kp_used * P) + (ki * I) + (kd * D);

    // ควบคุมมอเตอร์
    int LeftSpeed = spl - PID_output;
    int RightSpeed = spr + PID_output;

    // จำกัดความเร็ว
    if (LeftSpeed > 100)
        LeftSpeed = 100;
    if (LeftSpeed < 0)
        LeftSpeed = 0;
    if (RightSpeed > 100)
        RightSpeed = 100;
    if (RightSpeed < 0)
        RightSpeed = 0;

    Motor(LeftSpeed, RightSpeed);

    delayMicroseconds(50);
}