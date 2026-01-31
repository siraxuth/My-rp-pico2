void Ball1()
{
    fline4sensor(30, 30, 0.95, "a07", 'f', 's', 90, "a0", 1);
    rotate_right(20, 1, 10);
    arm_height(0);
    fw_gyro(25, 25, 0.95, 23.0, 1);
    // GGU();
    GC();
    delay(200);
    arm_height(5);
    bw_gyro(25, 25, 0.95, 22.5, 1);
    rotate_right(20, 86, 10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a0", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a5", 1);
    rotate_right(20, 92, 10);
    fw_gyro(25, 25, 0.95, 12.5, 1);
    GDP();
}

void Ball2()
{
    bw_gyro(20, 20, 0.80, 10.0, 0);
    // rotate_right(20, 77, 10);
    fline4sensor(0, 0, 0, "a07", 'n', 'r', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    // Motor(0, 0);
    // delay(200);
    
    fline4sensor(30, 30, 0.95, "a0", 'f', 'l', 90, "a2", 1);
    // Motor(0, 0);
    // delay(200);

    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);

    fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a2", 1);
    rotate_right(20, 91, 10);
    arm_height(0);
    fw_gyro(20, 20, 0.85, 9.5, 1);
    GC();
    delay(200);
    arm_height(4);
    bw_gyro(20, 20, 0.85, 10.0, 1);
    rotate_left(20, 83, 1);


    // fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a7", 1);
    fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a2", 1);
    rotate_left(20, 75, 1);
    fw_gyro(20, 20, 0.25, 22, 1);
    BackwardSpeedTime(20, 80);
    GDP();
}

void Ball3()
{
    bw_gyro(25, 25, 0.25, 19.0, 0);
    rotate_left(20, 180, 10);
    // rotate_left(20, 60, 10);
    TracSpeedDistances(30, 45.0);
    BackwardSpeedTime(20, 50);
    GGU1();
    rotate_left(20, 155, 10);
    fline4sensor(30, 30, 0.95, "a70", 'f', 'l', 90,"a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a2", 1);
    rotate_left(20, 93, 10);
    // arm_height(0);
    fw_gyro(20, 20, 0.85, 11.0, 1);
    BackwardSpeedTime(20, 60);
    MotorStop();
    delay(200);
    // GC();
    // GO();
    // delay(200);
    // arm_height(5);
    GDP();
    bw_gyro(20, 20, 0.85, 10.5, 1);
    rotate_right(20, 80, 1);
    // TLG(20, 190);
    // sw();
}

void Ball4()
{
fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a2", 1);
rotate_left(20, 90, 10);
fw_gyro(20, 20, 0.25, 15, 1);
BackwardSpeedTime(20, 50);
GGU5();
bw_gyro(20, 20, 0.85, 8,1);
rotate_right(20, 80, 1);
TracSpeedDistances(30, 90);
BackwardSpeedTime(20, 50);
GDP1();
sw();
}

void Ball5()
{
    // bw_gyro(30, 30, 0.25, 5, 0);
    rotate_left(20, 170, 10);
    fline4sensor(35, 35,1.0, "a07", 'f', 'p', 90, "a7", 1);
    fline4sensor(35, 35,1.0, "a07", 'f', 'r', 90, "a5", 1);
    TracSpeedDistances(30, 32);
    rotate_right(20, 90, 10);
    fw_gyro(20, 20, 0.85,25, 1);
    BackwardSpeedTime(20, 50);
    GGU3();
    bw_gyro(30, 30, 1.25, 19, 10);
    fline4sensor(0, 0, 0, "a", 'n', 'l', 90, "a2", 1);
    fline4sensor(35, 35, 1.0, "a07", 'f', 'l', 90, "a2", 10);
    fline4sensor(35, 35, 1.0, "a07", 'c', 's', 90, "a2", 10);
    rotate_left(20, 90, 10);
    fw_gyro(20, 20, 0.85, 15, 1);
    BackwardSpeedTime(20, 50);
    GDP4();
}

void Ball6()
{
    bw_gyro(20, 20, 0.85, 8, 1);
    rotate_left(20, 80, 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
    arm_height(0);
    TracSpeedDistances(20, 40);
    GC();
    delay(200);
    arm_height(4);
    rotate_left(20, 180, 10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a5", 1);
    TracSpeedDistances(30, 40);
    rotate_right(20, 90, 10);
    fw_gyro(20, 20, 0.25, 25, 1);
    BackwardSpeedTime(20, 50);
    GDP3();
}

void Ball7()
{
    bw_gyro(30, 30, 0.25, 20, 1);
    rotate_right(20, 90, 10);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'p', 90, "0", 1);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'p', 90, "0", 1);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'p', 90, "0", 1);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'p', 90, "0", 1);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'p', 90, "0", 1);
    TracSpeedDistances(20, 15);
    GGU();
    rotate_right(20, 180, 1);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'p', 90, "0", 1);
    fline4sensor(33, 33, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    TracSpeedDistances(20, 50);
    BackwardSpeedTime(20, 50);
    GDP();
}

void Ball8()
{
    rotate_left(20, 180,10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a2", 1);
    rotate_right(20, 90, 10);
    fw_gyro(20, 20, 0.25, 25, 1);
    BackwardSpeedTime(20, 50);
    GGU2();
    bw_gyro(20, 20, 0.25, 20, 1);
    rotate_right(20, 90, 10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    TracSpeedDistances(20, 20);
    BackwardSpeedTime(20, 50);
    GDP();
}   

void Ball9()
{
    rotate_left(20, 180, 10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a7", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a7", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'c', 'p', 90, "a2", 1);
    rotate_right(20, 92, 10);
    fw_gyro(25, 25, 0.95, 12.5, 1);
    GGU();
    fw_gyro(20, 20, 0.85, 13, 1);
    rotate_right(20, 90, 10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'c', 's', 90, "a2", 1);
    rotate_right(20, 90, 10);

    fw_gyro(20, 20, 0.25, 25, 1);
    BackwardSpeedTime(20, 50);
    GDP2();
    bw_gyro(20, 20, 0.25, 20,1);

    rotate_left(20, 90, 10);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'r', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'p', 90, "a2", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 'l', 90, "a5", 1);
    fline4sensor(30, 30, 0.95, "a07", 'f', 's', 90, "a5", 1);
}