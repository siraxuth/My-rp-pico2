// ===================================================
// SHORT COMMAND ALIASES (NEW TracJC VERSION)
// รองรับ Trac ใหม่ทั้งหมด ไม่มีของ legacy
// ===================================================

/**************** LINE TRACK ****************/

void TJ() { TracJC(); }
void TJSP() { TracFast(); }
void TJSL() { TracSlow(); }
void TJL() { TracJCL(); }
void TJR() { TracJCR(); }
// void TJLR() { TracJCR(); }

// void TC(int s, int t) { TracTime(s, t); }
// void TCB(int s, int t) { TracSpeedTimeBack(s, t); }
void TJSPL(int n) { TracJC_Line(n); }
void TJST(int s, int t) { TracJCSpeedTime(s, t); }

/**************** COMBO ****************/

void TJCSS(int n)
{
    for (int i = 0; i < n; i++)
    {
        fline4sensor(40, 40, 1.25, "a07", 'f', 'p', 90, "a0", 1);
    }
    fline4sensor(25, 25, 1.2, "a07", 'f', 's', 90, "a0", 1);
    MotorStop();
}

void TJCSSL(int n = 1)
{
    for (int i = 0; i < n; i++)
        TracFast();
    TracSlow();
    AO();
}

void TJCSSL2(int n = 1)
{
    for (int i = 0; i < n; i++)
        TracJC();
    TracSlow();
    AO();
}

void TJCSSA(int fast, int normal)
{
    for (int i = 0; i < fast; i++)
        TracFast();
    for (int i = 0; i < normal; i++)
        TracJC();
    AO();
}

void TJCSSB(int fast, int slow)
{
    for (int i = 0; i < fast; i++)
        TracFast();
    for (int i = 0; i < slow; i++)
        TracSlow();
    AO();
}

/**************** MOVE ****************/

void FD(int s, int t) { ForwardSpeedTime(s, t); }
void BK(int s, int t) { BackwardSpeedTime(s, t); }
void MS() { AO(); }

/**************** TURN ****************/

void TL() { TurnLeft(); }
void TL(int s, String se) { TurnLeftSpeed(s, se, 10); }
void TR() { TurnRight(); }
void TR(int s, String se) { TurnRightSpeed(s, se, 10); }

void UTL() { UTurnLeft(); }
void UTL(int s) { UTurnLeftSpeed(s); }
void UTR() { UTurnRight(); }
void UTR(int s) { UTurnRightSpeed(s); }

void LT(int s, int t) { Left(s, t); }
void RT(int s, int t) { Right(s, t); }
void TLD(int s, int t) { TurnLeftDelay(s, t); }
void TRD(int s, int t) { TurnRightDelay(s, t); }

void CL(int s, int t) { CurveLeft(s, t); }
void CR(int s, int t) { CurveRight(s, t); }

/**************** IMU ****************/

void TLG(int s, float d) { rotate_left(s, d, 10); }
void TRG(int s, float d) { rotate_right(s, d, 10); }
void SRI(int s, float d) { place_right_in(s, d, 10); }
void SRO(int s, float d) { place_right_out(s, d, 10); }
void SLI(int s, float d) { place_left_in(s, d, 10); }
void SLO(int s, float d) { place_left_out(s, d, 10); }

void SRI(int s, float d, int o) { place_right_in(s, d, o); }
void SRO(int s, float d, int o) { place_right_out(s, d, o); }
void SLI(int s, float d, int o) { place_left_in(s, d, o); }
void SLO(int s, float d, int o) { place_left_out(s, d, o); }
// void SPD(int s, int d) { SpinDegree(s, d); }

/**************** SPEED ****************/

void CS(int s, int a) { ChangeSpeed(s, a); }



void TGST(int s, int t) {
    arm_height(0);
    delay(100);
    // TracSpeedTime(s,s, 0.85, t);
    TracSpeedTime(s, t, my);
    // arm_up_close();
    GC();
    arm_height(5);
}