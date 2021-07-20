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
	P4DIR = 0xff;    //����Ϊ���ģʽ�������Ƶ��
	TAPwmInit('A',1,'P','P');
	TAPwmSetPeriod(500);
	//TAPwmSetPermill(1,900);     //2ͨ�� 20.0%
	//TAPwmSetPermill(2,900);     //2ͨ�� 20.0%
	forward();
	return 0;
}


void ClkInit()
{
    char i;
    BCSCTL1 &= ~XT2OFF;             //��XT2����
    IFG1&=~OFIFG;                   //����񵴴����־
    while((IFG1&OFIFG)!=0)
    {
        for(i=0;i<0xff;i++);
        IFG1&=~OFIFG;               //����񵴴����־
    }
    BCSCTL2 |= SELM_2+SELS+DIVS_3;  //MCLKΪ8MHz��SMCLKΪ1MHz
}
