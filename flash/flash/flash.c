#include "flash.h"
#include"Config.h"
#include <msp430f169.h>
/*******************************************************************************
*  函数名称：Flash_Init初始化函数
*  功       能：对Flash时钟进行初始化设置
*  参数说明：Div：根据SMCLK频率计算的所需分频值，可设定为1-64
*  函数返回：无
*  说       明：产生SegAddr变量，确定读取插入擦除等操作段
********************************************************************************/
unsigned char Flash_Init(unsigned char Div,unsigned char Seg)
{
    //-----设置Flash的时钟和分频，分频为恰好为最低位，直接用Div-1即可-----
    if(Div<1) Div=1;
    if(Div>64) Div=64;
    FCTL2 = FWKEY + FSSEL_2 + Div-1;         // 默认使用SMCLK，分频系数参数传入
    SegPre = Seg;                            //获取当前段
    if (Seg <= 31)                           //判断是否处于主Flash段
    {
        SegAddr=0xFFFF-(Seg+1)*128+1;        //计算段起始地址
        return(1);                           //赋值成功后即可退出并返回成功标志”1
    }
    switch(Seg){                             //判断是否处于信息Flash段
        case 'A': SegAddr=0x1080; break;     //选择初始化区间
        case 'B': SegAddr=0x1000; break;
        default: SegAddr=0x20FF; return(0);  //0x20FF地址为空白区，保护Flash
        }
    return(1);
}
/*******************************************************************************
*  函数名称：Flash_Direct_WriteCha写入函数
*  功       能：强行向Flash中写入一个字节(Char型变量)，而不管是否为空
*  参数说明：addr：偏移量    Data：要写入的数据
*  函数返回：无
*  说       明：初始化后使用此函数即可写入
********************************************************************************/
unsigned char Flash_Direct_WriteChar(unsigned int addr , unsigned char Data)
{
    unsigned int temp = 0 ;
    unsigned char *Ptr_SegAddr;           //Segment pointer
    //----- 段范围限定。为了内存管理安全，只允许本段操作-----//
    if((SegPre <= 31 &&addr >= 512)|| (SegPre > 31 && addr >= 64))
        return 0 ;
    temp = SegAddr + addr ;
    Ptr_SegAddr = (unsigned char *)temp ;  //Initialize Flash pointer
    FCTL1 = FWKEY + WRT ;                  //正常写状态
    FCTL3 = FWKEY ;                        //解除锁定
    _disable_interrupts();                 //关总中断
    *Ptr_SegAddr = Data ;                  //指定地址，写1字节
    while( FCTL3 & BUSY );                 //等待操作完成
    _enable_interrupts();                  //开总中断
    FCTL1 = FWKEY ;                        //退出写状态
    FCTL3 = FWKEY + LOCK ;                 //恢复锁定，保护数据
    return 1 ;
}
/*******************************************************************************
*  函数名称：Flash_ReadCha读取函数
*  功       能：从Flash中读取一个字节
*  参数说明：addr：偏移量    
*  函数返回：无
*  说       明：初始化后使用此函数即可读取
*******************************************************************************/
unsigned char Flash_ReadChar(unsigned int Addr)
{
    unsigned char Data=0;
    unsigned int *Ptr_SegAddr,temp=0;             //Segment pointer
    //----- 段范围限定。为了内存管理安全，只允许本段操作-----//
    if((SegPre<=31&&Addr>512)||(SegPre>31&&Addr>=64))
        return 0;
    temp =SegAddr+Addr;
    Ptr_SegAddr =(void*)temp;                    //initialize Flash pointer
    Data=*(Ptr_SegAddr);
    return Data ;                                //返回参数
}
/*******************************************************************************
*  函数名称：Flash_Erase()擦除函数
*  功       能：从Flash中读取一个字节
*  参数说明：addr：偏移量    
*  函数返回：无
*  说       明：初始化后使用此函数即可擦除
*******************************************************************************/
void Flash_Erase(void)
{
    unsigned char *Ptr_SegAddr;             //Segment pointer
    Ptr_SegAddr = (unsigned char *)SegAddr; //Initialize Flash pointer
    FCTL1 = FWKEY + ERASE;                  //段擦除模式
    FCTL3 = FWKEY;                          //解锁
    _disable_interrupts();
    *Ptr_SegAddr = 0;                       //擦除待操作段
    while(FCTL3&BUSY);                      //Busy
    _enable_interrupts();
    FCTL1 = FWKEY;                          //取消擦模式
    FCTL3 = FWKEY+LOCK;                     //上锁
    }
