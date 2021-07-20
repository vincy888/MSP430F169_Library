/*******************************************************************************
@文件名：car.c
@说明   ：
@日期   ：2021年7月19日
@作者   ：楸壳
*******************************************************************************/
#include"TimerA.h"
#include"car.h"
#include"msp430f169.h"




void stop()
{
    P4OUT = 0X0f;//p4.0,p4.1,p4.2,p4.3
}

void forward()
{
    P4OUT = 0X05;//P4.0,P4.2
    TAPwmSetDuty(1,200);//p1.2,p1.3
    TAPwmSetDuty(2,200);
}

void back()
{
    P4OUT = 0X0a;
    TAPwmSetDuty(1,80);
    TAPwmSetDuty(2,80);
}

void left()
{
    P4OUT = 0X09;
    TAPwmSetDuty(1,30);
    TAPwmSetDuty(2,80);
}

void right()
{
    P4OUT = 0X06;
    TAPwmSetDuty(1,80);
    TAPwmSetDuty(2,30);
}


