/*******************************************************************************
@文件名：i2c.c
@说明   ：这个程序库完成I2C的初始化、发送数据、接收数据；
@日期   ：2021年7月12日
@作者   ：楸壳
*******************************************************************************/
#include"i2c.h"
#include<msp430f169.h>
unsigned char I2CBuffer[3];
int tx_count;
int rx_count;

/*****************************************************************************
@函数名：I2CMasterInit
@功能：I2C主机模式初始化
@输入：SlaveAddr-从机地址，baud-波特率，fen-分频，HCLK-高电平时间，LCLK-低电平时间
@输出：无
@使用示例：I2CMasterInit(0x048,38400,0x01,0x18,0x18);//从机地址为0x48，I2C时钟 = SMCLK/2，
SCL高电平周期 = 20*I2C时钟，SCL低电平周期为20*I2C时钟
******************************************************************************/
void I2CMasterInit(unsigned char SlaveAddr,long baud,unsigned char fen,unsigned char HCLK,unsigned char LCLK)
{
    long int brclk; //波特率发生器时钟频率
    P3SEL |= 0x0a;//定义I2C引脚p3.1-SDA,p3.3-SCL
    P3DIR &= ~0x0A;
    U0CTL |= I2C + SYNC + MST;//I2C主机模式
    U0CTL &= ~I2CEN;//清除I2C使能
    I2CSA = SlaveAddr;//从机地址
    //时钟源设置
    U0TCTL &= ~(SSEL0 + SSEL1);       //清除之前的时钟设置
    if (baud <= 16364)                 //
    {
        U0TCTL |= SSEL0;              //ACLK，降低功耗
        brclk = 32768;                //波特率发生器时钟频率=ACLK(32768)
    }
    else
    {
        U0TCTL |= SSEL1;              //SMCLK，保证速度
        brclk = 1000000;              //波特率发生器时钟频率=SMCLK(1MHz)
    }

    //------------------------设置波特率-------------------------
    if (baud < 300 || baud > 115200)   //波特率超出范围
    {
        return 0;
    }
    I2CPSC = fen;//I2C时钟
    I2CSCLH = HCLK;//SCL高电平周期
    I2CSCLL = LCLK;//SCL低电平周期
     //I2CTCTL |= I2CSSEL1;//时钟源为SMCLK
    U0CTL |= I2CEN;//I2C使能
    tx_count = 0;
    rx_count = 0;
}


/*****************************************************************************
@函数名：I2CWriteByte
@功能：主机传输一字节数据至从机
@输入：Addr-从机地址，Val-数据
@输出：无
@使用示例：I2CWriteByte(0x48,0x12);
******************************************************************************/
void I2CWriteByte(unsigned char Addr,unsigned char Val)
{
    U0CTL |= MST;//主模式
    I2CTCTL |= I2CTRX ;//传输模式
    I2CIFG &= ~TXRDYIFG;// 清除中断标志
    I2CIE = TXRDYIE;// 发送中断使能
    while(I2CDCTL & I2CBUSY);//等待I2C空闲
    I2CBuffer[1] = Addr;
    I2CBuffer[0] = Val;
    tx_count = 1;    // 设置缓冲区指针
    I2CNDAT = 0X02;
    I2CTCTL |= I2CSTT + I2CTRX;//开始和停止条件产生
}

/*****************************************************************************
@函数名：I2CReadByte
@功能：读取传输的数据
@输入：Addr-从机地址
@输出：数据
@使用示例：I2CreadByte(0x48);
******************************************************************************/
unsigned char I2CReadByte(unsigned char Addr)
{
     I2CTCTL &= ~I2CTRX;    // 接收模式， R/W 为： 1
     I2CIE = RXRDYIE;// 接收中断使能
     while(I2CDCTL & I2CBUSY);//等待I2C空闲
     U0CTL |= MST;//主机模式
     I2CNDAT = 0X02;//接收两个字节的数据
     I2CIFG &=~ARDYIFG;//清除中断标志
     I2CTCTL |= I2CSTT + I2CSTP;//开始接收
     tx_count = 0;    // 设置缓冲区指针
     while((~I2CIFG)&ARDYIFG);//等待传输完成
     return I2CBuffer[0];//返回数据

}


/*****************************************************************************
@函数名：I2CWriteStr
@功能：主机向从机传输多字节数据
@输入：Addr-从机地址，*Val-数据，Length-数据长度
@输出：无
@使用示例：char *Val[10] = "hello";
           I2CWriteStr(0x48,*Val,strlen("hello));
******************************************************************************/
unsigned char I2CWriteStr(unsigned char Addr,unsigned char *Val,int Length)
{
    int num = 0;
    int Len = Length - 1;
    for(num = 0;num<Len;num++)
    {
        I2CWriteByte(Addr,*Val);
        while((I2CIFG & 0X02) == NACKIFG)
        {
            I2CTCTL |= I2CSTP;//无响应，结束总线
            return 0;
        }
        Val++;
    }
    I2CWriteByte(Addr,*Val);
    while((I2CIFG & 0x02) == NACKIFG)// 等待数据的应答
    {
        I2CTCTL |= I2CSTP; // 无应答，结束总线
        return(0);
    }
    return 1;
}

#pragma vector=USART0TX_VECTOR
__interrupt void ISR_I2C(void)
{
    switch (I2CIV)
    {
        case I2CIV_AL:
        {
            // 仲裁中断
            break;
        }
        case I2CIV_NACK:
        {
            //NACK 中断
            break;
        }
        case I2CIV_OA:
        {
            // 自己地址中断
            break;
        }
        case I2CIV_ARDY:
        {
            // 访问准备好中断
            break;
        }
        case I2CIV_RXRDY:
        {
            // 接收准备好中断
            I2CBuffer[0]=I2CDRB;
            break;
        }
        case I2CIV_TXRDY:
        {
            // 发送准备好中断
            I2CDRB = I2CBuffer[tx_count];
            tx_count = tx_count - 1;
            if (tx_count < 0)
            {
                // 禁止发送中断
                I2CIE &= ~TXRDYIE;
            }
            break;
        }
        case I2CIV_GC:
        {
            // 一般调用中断
            break;
        }
        case I2CIV_STT:
        {
            // 起始条件中断
            break;
        }
    }
}

