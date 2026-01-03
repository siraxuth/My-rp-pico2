// ===================================================
// SHORT COMMAND ALIASES - คำสั่งย่อ
// ===================================================
// ไฟล์นี้รวมคำสั่งย่อทั้งหมดเพื่อให้เขียนโค้ดได้กระชับขึ้น
// ใช้แทนคำสั่งเต็มได้เลย เช่น TC(60,1000) แทน TracSpeedTime(60,1000)
// ===================================================

// ----- คำสั่งย่อที่มีอยู่แล้ว (Existing Short Commands) -----

void TJCSS(int line) // TracJCSpeed + TracJC
{
    for (int i = 0; i < line; i++)
    {
        TracJCSpeed();
    }
    TracJC();
    AO();
}
void TJCSS() { TJCSS(1); }

void TJCSSL(int line) // TracJCSpeed + TracJCSlow
{
    for (int i = 0; i < line; i++)
    {
        TracJCSpeed();
    }
    TracJCSlow();
    AO();
}
void TJCSSL() { TJCSSL(1); }

void TJCSSL2(int line) // TracJC + TracJCSlow
{
    for (int i = 0; i < line; i++)
    {
        TracJC();
    }
    TracJCSlow();
    AO();
}
void TJCSSL2() { TJCSSL2(1); }

void TJCSSA(int line, int Tracs) // TracJCSpeed + TracJC (กำหนดจำนวน)
{
    for (int i = 0; i < line; i++)
    {
        TracJCSpeed();
    }
    for (int a = 0; a < Tracs; a++)
    {
        TracJC();
    }
    AO();
}

void TJCSSB(int line, int Tracc) // TracJCSpeed + TracJCSlow (กำหนดจำนวน)
{
    for (int i = 0; i < line; i++)
    {
        TracJCSpeed();
    }
    for (int a = 0; a < Tracc; a++)
    {
        TracJCSlow();
    }
    AO();
}

// ===================================================
// คำสั่งย่อใหม่ (New Short Command Aliases)
// ===================================================

// ----- เดินตามเส้น (Line Tracking) -----
void TC(int s, int t) { TracSpeedTime(s, t); } // TC = TracSpeedTime(speed, time)
void PA(int Num)
{ //  PA = pharmacy ใช้ข้ามเส้นทางเภสัชศาสตรืป้องกันหลุด
    switch (Num)
    {
    case 1:
        TracJC();
        TracJCSlow();
        break;

    case 2:
        TracJCSlow();
        TracJC();
        break;

    default:
        TracJC();
        TracJCSlow();
        break;
    }
}
void TCB(int s, int t) { TracSpeedTimeBack(s, t); } // TCB = TracSpeedTimeBack(speed, time)
void TJ() { TracJC(); }                             // TJ = TracJC
void TJS() { TracJCStop(); }                        // TJS = TracJCStop
void TJSP() { TracJCSpeed(); }                      // TJSP = TracJCSpeed
void TJSL() { TracJCSlow(); }                       // TJSL = TracJCSlow
void TJL() { TracJCLeft(); }                        // TJL = TracJCLeft
void TJR() { TracJCRight(); }                       // TJR = TracJCRight
void TJLR() { TracJCLorR(); }                       // TJLR = TracJCLorR
void TJSPL(int n) { TracJCSpeedLine(n); }           // TJSPL = TracJCSpeedLine(line)
void TJST(int s, int t) { TracJCSpeedTime(s, t); }  // TJST = TracJCSpeedTime(speed, time)

// ----- เดินตามเส้นถอยหลัง (Backward Line Tracking) -----
void TJB() { TracJCBack(); }                            // TJB = TracJCBack
void TJSB() { TracJCStopBack(); }                       // TJSB = TracJCStopBack
void TJSPB() { TracJCSpeedBack(); }                     // TJSPB = TracJCSpeedBack
void TJSTB(int s, int t) { TracJCSpeedTimeBack(s, t); } // TJSTB = TracJCSpeedTimeBack(speed, time)

// ----- เดินหน้า/ถอยหลัง (Forward/Backward) -----
void FD() { Forward(); }                            // FD = Forward
void FDD(int t) { ForwardDelay(t); }                // FDD = ForwardDelay(time)
void FST(int s, int t) { ForwardSpeedTime(s, t); }  // FST = ForwardSpeedTime(speed, time)
void FSD(int t) { ForwardSpeedDelay(t); }           // FSD = ForwardSpeedDelay(time)
void BK() { Backward(); }                           // BK = Backward
void BKD(int t) { BackwardDelay(t); }               // BKD = BackwardDelay(time)
void BST(int s, int t) { BackwardSpeedTime(s, t); } // BST = BackwardSpeedTime(speed, time)
void FJS() { ForwardJCStop(); }                     // FJS = ForwardJCStop
void FJ() { ForwardJC(); }                          // FJ = ForwardJC
void MS() { MotorStop(); }                          // MS = MotorStop

// ----- เลี้ยว (Turn) -----
// TL(), TR() - ใช้ความเร็ว TurnSpeed (ค่าเริ่มต้น)
// TL(speed), TR(speed) - กำหนดความเร็วเอง
void TL() { TurnLeft(); }                        // TL = TurnLeft (ใช้ TurnSpeed)
void TL(int s) { TurnLeftSpeed(s); }             // TL(speed) = TurnLeftSpeed
void TR() { TurnRight(); }                       // TR = TurnRight (ใช้ TurnSpeed)
void TR(int s) { TurnRightSpeed(s); }            // TR(speed) = TurnRightSpeed
void UTL() { UTurnLeft(); }                      // UTL = UTurnLeft (ใช้ TurnSpeed)
void UTL(int s) { UTurnLeftSpeed(s); }           // UTL(speed) = UTurnLeftSpeed
void UTR() { UTurnRight(); }                     // UTR = UTurnRight (ใช้ TurnSpeed)
void UTR(int s) { UTurnRightSpeed(s); }          // UTR(speed) = UTurnRightSpeed
void LT(int s, int t) { Left(s, t); }            // LT = Left(speed, time)
void RT(int s, int t) { Right(s, t); }           // RT = Right(speed, time)
void TLD(int s, int t) { TurnLeftDelay(s, t); }  // TLD = TurnLeftDelay(speed, time)
void TRD(int s, int t) { TurnRightDelay(s, t); } // TRD = TurnRightDelay(speed, time)
void CL(int s, int t) { CurveLeft(s, t); }       // CL = CurveLeft(speed, time)
void CR(int s, int t) { CurveRight(s, t); }      // CR = CurveRight(speed, time)

// ----- เลี้ยวใช้ IMU (Turn with IMU) -----
void TLG(int s, float d) { TL_deg(s, d); }        // TLG = TurnLeftDegree(speed, degree, origin)
void TRG(int s, float d) { TR_deg(s, d); }       // TRG = TurnRightDegree(speed, degree, origin)
void SPD(int s, int d) { SpinDegree(s, d); }              // SPD = SpinDegree(speed, degree, pid)
// void SPDB(int s, int d, int p) { SpinDegreeBack(s, d, p); }         // SPDB = SpinDegreeBack(speed, degree, pid)


// ----- อื่นๆ (Others) -----
void CS(int s, int a) { ChangeSpeed(s, a); } // CS = ChangeSpeed(speed, accSpeed)
