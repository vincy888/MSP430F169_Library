/*******************************************************************************
@�ļ�����TA_timer.c
@˵��   ��
@����   ��2021��7��15��
@����   ��鱿�
*******************************************************************************/
#include<msp430f169.h>
#include"TimerA.h"

/*******************************************************************************
*  �������ƣ�TimerInit(unsigned cahr timer)
*  ����˵������ʼ����ʱȥ
*  ����˵����timer ��ʱʱ��
*  �������أ���
*  ʹ�����̣�void TimerInit(5000)��ʱ5000ms
********************************************************************************/
void TimerInit(unsigned char timer)
{
    TACTL |= TACLR;//��λ
    TACCTL0 |= CCIE;//������ʱ���ж�
    TACTL |= TASSEL_2+MC_1;//MCLKϵͳʱ��,������ģʽ
    TACCR0 = timer;//��ʱʱ��ѡȡ
}

/*******************************************************************************
*  �������ƣ�TimerCaptureInit(unsigned char mode,unsigned char type)
*  ����˵������ʼ������
*  ����˵����mode ����ģʽ
*           type ����ͨ��
*  �������أ���
*  ʹ�����̣�TimerCaptureInit(up,0)�����ز��񣬲���ͨ��0
********************************************************************************/
void TimerCaptureInit(unsigned char mode,unsigned char type)
{
    TACTL |= TACLR;//��λ
    TACTL |= TASSEL_2 + ID_2 + MC_3;//ʹ��SMCLKʱ�ӣ�8��Ƶ��ʹ����������ģʽ
    TACCTL0 &=~ COV;//����COV
    switch(mode)
    {
    case 'up':
        TACCTL0 |= CM_1;//�����ز���
        break;
    case 'down':
        TACCTL0 |= CM_2;//�½��ز���
        break;
    case 'all':
        TACCTL0 |= CM_3;//�����غ��½��ؾ�����
        break;
    default:
        return 0;
    }
    TACCTL0 |= CAP + SCS + CCIE + CCIFG;//����ģʽ�������ж�ʹ�ܣ�ͬ������

    switch(type)
    {
    case 0:
        TACCTL0 |= CCIS0;//CCI0A
        break;
    case 1:
        TACCTL0 |= CCIS1;//CCI0B
        break;
    default:
        return 0;
    }


}


#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A()
{

    switch(TAIV)
    {
    case 2:

        break;
    case 4:

        break;
    case 10:

        break;
    }
}

/****************************************************************************
* ��    �ƣ�TAPwmInit
* ��    �ܣ�TA��ʱ����ΪPWM�������ĳ�ʼ�����ú���
* ��ڲ�����Clk:ʱ��Դ 'S'=SMCLK;   'A'=ACLK ;     _____
                       'E'=TACLK(�ⲿ����)    'e'= TACLK(TACLKȡ��)
            Div:ʱ�ӷ�Ƶϵ��: 1/2/4/8
            Mode1:ͨ��1�����ģʽ 'P'��Ϊ�ߵ�ƽ�����'N'�͵�ƽ ��0=����
            Mode2:ͨ��2�����ģʽ 'P'��Ϊ�ߵ�ƽ�����'N'�͵�ƽ ��0=����

* ���ڲ�����1��ʾ���óɹ���0��ʾ������������ʧ�ܡ�
* ˵    ��: �ڵ���PWM��غ���֮ǰ����Ҫ���øú�������TA��ģʽ��ʱ��Դ��
* ��    ��: TAPwmInit('A',1,'P','P')TAʱ����ΪACLK,ͨ��1��ͨ��2��Ϊ�ߵ�ƽ���
            TAPwmInit('S',4,'N','N')TAʱ����ΪSMCLK/4, ͨ��1��2��Ϊ�͵�ƽ���
            TAPwmInit('A',1,'P',0)TAʱ����ΪACLK,ͨ��1�ߵ�ƽ���,ͨ��2����,��
            ���õ�PWMͨ��������ܽ��Կ���Ϊ��ͨIO��ʹ�á�
****************************************************************************/
char TAPwmInit(char Clk,char Div,char Mode1,char Mode2)
{
    TACTL = 0;                  //�����ǰ����
    TACTL |= MC_1;              //��ʱ��TA��Ϊ������ģʽ
    switch(Clk)                 //ѡ��ʱ��Դ
    {
        case 'A': case 'a':  TACTL|=TASSEL_1; break;    //ACLK
        case 'S': case 's':  TACTL|=TASSEL_2; break;    //SMCLK
        case 'E':            TACTL|=TASSEL_0; break;    //�ⲿ����(TACLK)
        case 'e':            TACTL|=TASSEL_3; break;    //�ⲿ����(TACLKȡ��)
        default :  return(0);                           //��������
    }
    switch(Div)                 //ѡ���Ƶϵ��
    {
        case 1:   TACTL|=ID_0; break;   //1
        case 2:   TACTL|=ID_1; break;   //2
        case 4:   TACTL|=ID_2; break;   //4
        case 8:   TACTL|=ID_3; break;   //8
        default :  return(0);           //��������
    }
    switch(Mode1)               //����PWMͨ��1�����ģʽ��
    {
        case 'P':case 'p':          //�������Ϊ�ߵ�ƽģʽ
            TACCTL1 = OUTMOD_7;     //�ߵ�ƽPWM���
            P1SEL |= BIT2;          //��P1.2��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            P1DIR |= BIT2;          //��P1.2��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            break;
        case 'N':case 'n':          //�������Ϊ�͵�ƽģʽ
            TACCTL1 = OUTMOD_3;     //�͵�ƽPWM���
            P1SEL |= BIT2;          //��P1.2��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            P1DIR |= BIT2;          //��P1.2��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            break;
        case '0':case 0:            //�������Ϊ����
            P1SEL &= ~BIT2;         //P1.2�ָ�Ϊ��ͨIO��
            break;
        default :  return(0);       //��������
    }
    switch(Mode2)                   //����PWMͨ��1�����ģʽ��
    {
        case 'P':case 'p':          //�������Ϊ�ߵ�ƽģʽ
            TACCTL2 =OUTMOD_7;      //�ߵ�ƽPWM���
            P1SEL |= BIT3;          //��P1.3��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            P1DIR |= BIT3;          //��P1.3��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            break;
        case 'N':case 'n':          //�������Ϊ�͵�ƽģʽ
            TACCTL2 =OUTMOD_3;      //�͵�ƽPWM���
            P1SEL |= BIT3;          //��P1.3��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            P1DIR |= BIT3;          //��P1.3��� (��ͬ�ͺŵ�Ƭ�����ܲ�һ��)
            break;
        case '0':case 0:            //�������Ϊ����
            P1SEL &= ~BIT3;         //P1.3�ָ�Ϊ��ͨIO��
            break;
        default :  return(0);       //��������
    }
    return(1);
}

/****************************************************************************
* ��    �ƣ�TAPwmSetPeriod
* ��    �ܣ�����PWM������������
* ��ڲ�����Period:����(0~65535) ʱ�Ӹ���
* ���ڲ�������
* ˵    ��: ���ö�2·PWMͬʱ��Ч
* ��    ��: TAPwmSetPeriod(500)����PWM��������Ϊ500��ʱ������
****************************************************************************/
void TAPwmSetPeriod(unsigned int Period)
{
    TACCR0 = Period;
}

/****************************************************************************
* ��    �ƣ�TAPwmSetDuty
* ��    �ܣ�����PWM���"��Ч"��ʱ��
* ��ڲ�����Channel: ��ǰ���õ�ͨ����  1/2
            Duty:    PWM��Чʱ��(0~65535) ʱ�Ӹ���
* ���ڲ�������
* ˵    ��: ��ͨ�����ģʽ��Ϊ�ߵ�ƽ���ģʽ��Dutyֵ���ڸߵ�ƽʱ�Ӹ���
            ��ͨ�����ģʽ��Ϊ�͵�ƽ���ģʽ��Dutyֵ���ڵ͵�ƽʱ�Ӹ���
* ��    ��: TAPwmSetDuty(1,30)����PWMͨ��1�����ĸߵ�ƽ����Ϊ30��ʱ��
            TAPwmSetDuty(2,80)����PWMͨ��2�����ĸߵ�ƽ����Ϊ80��ʱ��
****************************************************************************/
void TAPwmSetDuty(char Channel,unsigned int Duty)
{
    switch(Channel)
    {
        case 1: TACCR1=Duty; break;
        case 2: TACCR2=Duty; break;
    }
}

/****************************************************************************
* ��    �ƣ�TAPwmSetPermill
* ��    �ܣ�����PWM�����ռ�ձ�(ǧ�ֱ�)
* ��ڲ�����Channel: ��ǰ���õ�ͨ����  1/2
            Percent: PWM��Чʱ���ǧ�ֱ� (0~1000)
* ���ڲ�������
* ˵    ��: 1000=100.0%  500=50.0% ����������
* ��    ��: TAPwmSetPermill(1,300)����PWMͨ��1������ռ�ձ�Ϊ30.0%
            TAPwmSetPermill(2,825)����PWMͨ��2������ռ�ձ�Ϊ82.5%
****************************************************************************/
void TAPwmSetPermill(char Channel,unsigned int Percent)
{
    unsigned long int Period;
    unsigned int Duty;
    Period = TACCR0;
    Duty = Period * Percent / 1000;
    TAPwmSetDuty(Channel,Duty);
}

