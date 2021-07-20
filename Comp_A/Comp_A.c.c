#include "Comp_A.h"
#include <msp430f169.h>
#define uchar unsigned char
#define uint unsigned int
unsigned int i;
uchar flag = 0;
/*
 * ��ʱ����
 */
#define CPU_F ((double)8000000)   //�ⲿ��Ƶ����8MHZ
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
/*********************************************************************************
 * �������ƣ�ϵͳʱ�ӳ�ʼ��
 * ���ܣ���ʱ�ӽ��г�ʼ������
 * ��������ֵ����
 * ˵����
 ********************************************************************************/
void Init_Clk()
{
  uchar i;
  BCSCTL1&=~XT2OFF;                 //��XT2����
  BCSCTL2|=SELM1+SELS;              //MCLKΪ8MHZ��SMCLKΪ8MHZ
  do{
    IFG1&=~OFIFG;                   //������������־
    for(i=0;i<100;i++)
       _NOP();
  }
  while((IFG1&OFIFG)!=0);           //�����־λ1�������ѭ���ȴ�
  IFG1&=~OFIFG;
}
/*********************************************************************************
 * �������ƣ��Ƚ���A��ʼ��
 * ���ܣ��ԱȽ������г�ʼ������
 * ��������ֵ����
 * ˵����
 ********************************************************************************/
void Init_Com_A(){
    P2SEL = 0x0C;         //���ö˿� P2.2 P2.3Ϊ�ڶ����ܶ˿�
    P2DIR = 0x04;         //����P2.2Ϊ������Ƚ�������� P2.3Ϊ���루�Ƚ����룩
    CACTL1 &= ~CAEX;      //�������
    CACTL1 |= CARSEL + CAREF1 + CAON + CAIE + CAIES;  //CA0Ϊ�������룬�ο���ѹ0.5VCC��������
    CACTL2 |= CAF + P2CA0;
}
/*
 * �Ƚ���A�жϺ���
 */
#pragma vector = COMPARATORA_VECTOR
__interrupt void COM_A(){
    flag = 1;
}
/*********************************************************************************
 * �������ƣ��Ƚ���A��or�ر�
 * ���ܣ��ԱȽ������п���or�ر�
 * ��������ֵ����
 * ����˵����
 * ˵����
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
 * �������ƣ��������
 * ���ܣ�������
 * ��������ֵ����
 * ˵����
 ********************************************************************************/
void comp_BattCheck(void) {
  CACTL1 = CAREF_1 + CAON;                  // 0.25*Vcc on P2.3, Comp. on
  i = 16384;                                // delay
  while(i>0) {
    i--;
  }
  CACTL1 = CARSEL + CAREF_2 + CAREF_1 + CAON; // 0.55V on -, Comp. on
  if ((CACTL2 & CAOUT) > 0)
    P1OUT ^= 0x01;                          // P1.0 �л�
  else P1OUT |= 0x01;                       // P1.0 ��λ
  CACTL1 = 0x00;                            // Disable Comp_A, save power
}

/**
 * ������
*/
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // �رտ��Ź�
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

