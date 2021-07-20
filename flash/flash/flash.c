#include "flash.h"
#include"Config.h"
#include <msp430f169.h>
/*******************************************************************************
*  �������ƣ�Flash_Init��ʼ������
*  ��       �ܣ���Flashʱ�ӽ��г�ʼ������
*  ����˵����Div������SMCLKƵ�ʼ���������Ƶֵ�����趨Ϊ1-64
*  �������أ���
*  ˵       ��������SegAddr������ȷ����ȡ��������Ȳ�����
********************************************************************************/
unsigned char Flash_Init(unsigned char Div,unsigned char Seg)
{
    //-----����Flash��ʱ�Ӻͷ�Ƶ����ƵΪǡ��Ϊ���λ��ֱ����Div-1����-----
    if(Div<1) Div=1;
    if(Div>64) Div=64;
    FCTL2 = FWKEY + FSSEL_2 + Div-1;         // Ĭ��ʹ��SMCLK����Ƶϵ����������
    SegPre = Seg;                            //��ȡ��ǰ��
    if (Seg <= 31)                           //�ж��Ƿ�����Flash��
    {
        SegAddr=0xFFFF-(Seg+1)*128+1;        //�������ʼ��ַ
        return(1);                           //��ֵ�ɹ��󼴿��˳������سɹ���־��1
    }
    switch(Seg){                             //�ж��Ƿ�����ϢFlash��
        case 'A': SegAddr=0x1080; break;     //ѡ���ʼ������
        case 'B': SegAddr=0x1000; break;
        default: SegAddr=0x20FF; return(0);  //0x20FF��ַΪ�հ���������Flash
        }
    return(1);
}
/*******************************************************************************
*  �������ƣ�Flash_Direct_WriteChaд�뺯��
*  ��       �ܣ�ǿ����Flash��д��һ���ֽ�(Char�ͱ���)���������Ƿ�Ϊ��
*  ����˵����addr��ƫ����    Data��Ҫд�������
*  �������أ���
*  ˵       ������ʼ����ʹ�ô˺�������д��
********************************************************************************/
unsigned char Flash_Direct_WriteChar(unsigned int addr , unsigned char Data)
{
    unsigned int temp = 0 ;
    unsigned char *Ptr_SegAddr;           //Segment pointer
    //----- �η�Χ�޶���Ϊ���ڴ����ȫ��ֻ�����β���-----//
    if((SegPre <= 31 &&addr >= 512)|| (SegPre > 31 && addr >= 64))
        return 0 ;
    temp = SegAddr + addr ;
    Ptr_SegAddr = (unsigned char *)temp ;  //Initialize Flash pointer
    FCTL1 = FWKEY + WRT ;                  //����д״̬
    FCTL3 = FWKEY ;                        //�������
    _disable_interrupts();                 //�����ж�
    *Ptr_SegAddr = Data ;                  //ָ����ַ��д1�ֽ�
    while( FCTL3 & BUSY );                 //�ȴ��������
    _enable_interrupts();                  //�����ж�
    FCTL1 = FWKEY ;                        //�˳�д״̬
    FCTL3 = FWKEY + LOCK ;                 //�ָ���������������
    return 1 ;
}
/*******************************************************************************
*  �������ƣ�Flash_ReadCha��ȡ����
*  ��       �ܣ���Flash�ж�ȡһ���ֽ�
*  ����˵����addr��ƫ����    
*  �������أ���
*  ˵       ������ʼ����ʹ�ô˺������ɶ�ȡ
*******************************************************************************/
unsigned char Flash_ReadChar(unsigned int Addr)
{
    unsigned char Data=0;
    unsigned int *Ptr_SegAddr,temp=0;             //Segment pointer
    //----- �η�Χ�޶���Ϊ���ڴ����ȫ��ֻ�����β���-----//
    if((SegPre<=31&&Addr>512)||(SegPre>31&&Addr>=64))
        return 0;
    temp =SegAddr+Addr;
    Ptr_SegAddr =(void*)temp;                    //initialize Flash pointer
    Data=*(Ptr_SegAddr);
    return Data ;                                //���ز���
}
/*******************************************************************************
*  �������ƣ�Flash_Erase()��������
*  ��       �ܣ���Flash�ж�ȡһ���ֽ�
*  ����˵����addr��ƫ����    
*  �������أ���
*  ˵       ������ʼ����ʹ�ô˺������ɲ���
*******************************************************************************/
void Flash_Erase(void)
{
    unsigned char *Ptr_SegAddr;             //Segment pointer
    Ptr_SegAddr = (unsigned char *)SegAddr; //Initialize Flash pointer
    FCTL1 = FWKEY + ERASE;                  //�β���ģʽ
    FCTL3 = FWKEY;                          //����
    _disable_interrupts();
    *Ptr_SegAddr = 0;                       //������������
    while(FCTL3&BUSY);                      //Busy
    _enable_interrupts();
    FCTL1 = FWKEY;                          //ȡ����ģʽ
    FCTL3 = FWKEY+LOCK;                     //����
    }
