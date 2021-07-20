#include"car.h"
#include"msp430f169.h"
#include"TimerA.h"
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	ClkInit();
	P4SEL = 0x00;
	P4DIR = 0xff;    //设置为输出模式用来控制电机
	TAPwmInit('A',1,'P','P');
	TAPwmSetPeriod(500);
	//TAPwmSetPermill(1,900);     //2通道 20.0%
	//TAPwmSetPermill(2,900);     //2通道 20.0%
	forward();
	return 0;
}


void ClkInit()
{
    char i;
    BCSCTL1 &= ~XT2OFF;             //打开XT2振荡器
    IFG1&=~OFIFG;                   //清除振荡错误标志
    while((IFG1&OFIFG)!=0)
    {
        for(i=0;i<0xff;i++);
        IFG1&=~OFIFG;               //清除振荡错误标志
    }
    BCSCTL2 |= SELM_2+SELS+DIVS_3;  //MCLK为8MHz，SMCLK为1MHz
}
