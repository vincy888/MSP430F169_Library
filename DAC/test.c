#include <msp430x16x.h>
#include "DAC12.h"

/****************************************************************************
 * * 名    称：ClkInit
 * * 功    能：时钟系统初始化  MCLK为8MHz，SMCLK为1MHz
 * * 入口参数：无
 * * 出口参数：无
 * ****************************************************************************/
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

/****************************************************************************
 * * 名    称：main主程序
 * * 功    能：演示DAC12程序库 的使用
 * * 入口参数：无
 * * 出口参数：无
 * ****************************************************************************/
void main( void )
{
	WDTCTL = WDTPW + WDTHOLD;		//关闭开门狗
	ClkInit();
	DAC12Init(3,5);                 //初始化
	DAC12Cal(3);                    //校准
	DAC12_2Init(1);    //开启内部参考源 2.5v 必须有；以供DA使用
	DAC12Out(3,0x300);
}
