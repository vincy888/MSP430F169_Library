#include "Comp_A.h"
#include <msp430f169.h>
#define uchar unsigned char
#define uint unsigned int
unsigned int i;
uchar flag = 0;
/*
 * 延时函数
 */
#define CPU_F ((double)8000000)   //外部高频晶振8MHZ
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
/*********************************************************************************
 * 函数名称：系统时钟初始化
 * 功能：对时钟进行初始化设置
 * 函数返回值：无
 * 说明：
 ********************************************************************************/
void Init_Clk()
{
  uchar i;
  BCSCTL1&=~XT2OFF;                 //打开XT2振荡器
  BCSCTL2|=SELM1+SELS;              //MCLK为8MHZ，SMCLK为8MHZ
  do{
    IFG1&=~OFIFG;                   //清除振荡器错误标志
    for(i=0;i<100;i++)
       _NOP();
  }
  while((IFG1&OFIFG)!=0);           //如果标志位1，则继续循环等待
  IFG1&=~OFIFG;
}
/*********************************************************************************
 * 函数名称：比较器A初始化
 * 功能：对比较器进行初始化设置
 * 函数返回值：无
 * 说明：
 ********************************************************************************/
void Init_Com_A(){
    P2SEL = 0x0C;         //配置端口 P2.2 P2.3为第二功能端口
    P2DIR = 0x04;         //设置P2.2为输出（比较输出）， P2.3为输入（比较输入）
    CACTL1 &= ~CAEX;      //反向输出
    CACTL1 |= CARSEL + CAREF1 + CAON + CAIE + CAIES;  //CA0为正向输入，参考电压0.5VCC反向输入
    CACTL2 |= CAF + P2CA0;
}
/*
 * 比较器A中断函数
 */
#pragma vector = COMPARATORA_VECTOR
__interrupt void COM_A(){
    flag = 1;
}
/*********************************************************************************
 * 函数名称：比较器A打开or关闭
 * 功能：对比较器进行开启or关闭
 * 函数返回值：无
 * 参数说明：
 * 说明：
 ********************************************************************************/
void comp_Open(unsigned char doit)
{
    if(doit==100)
    {
        CACTL1 |=CAON;
    }
    else if(doit==0)
    {
        CACTL1 &= ~CAON;
    }
}

/*********************************************************************************
 * 函数名称：电量检查
 * 功能：检查电量
 * 函数返回值：无
 * 说明：
 ********************************************************************************/
void comp_BattCheck(void) {
  CACTL1 = CAREF_1 + CAON;                  // 0.25*Vcc on P2.3, Comp. on
  i = 16384;                                // delay
  while(i>0) {
    i--;
  }
  CACTL1 = CARSEL + CAREF_2 + CAREF_1 + CAON; // 0.55V on -, Comp. on
  if ((CACTL2 & CAOUT) > 0)
    P1OUT ^= 0x01;                          // P1.0 切换
  else P1OUT |= 0x01;                       // P1.0 复位
  CACTL1 = 0x00;                            // Disable Comp_A, save power
}

/**
 * 主函数
*/
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // 关闭看门狗
    Init_Clk();
    Init_Com_A();
    _EINT();
    uchar k;
    while(1)
    {
        while(flag)
        {
            P6DIR |= BIT0;
            for(k=0;k<10;k++)
            {
                P1OUT = (~(P1OUT&BIT0));
                delay_ms(500);
            }
            flag = 0;
        }

    }

}

