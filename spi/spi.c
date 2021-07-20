/*******************************************************************************
@文件名	：spi.c
@说明	：  这个程序库完成Spi的初始化、发送数据、接收数据；
 *         本程序只完成msp430的spi的主机模式，从机模式只需把时
 *        钟和波特率去掉即可，具体参考用户指南；
 *         MSP430 SPI 同步串行传输时，时钟不再提供小数分频，本
 *        程序采用brclk/baud作为分频系数，小数分频直接忽略
 *        使Spi功能用方法：把Spi.c加入到项目，同时在要调用此
 *         程序库的程序文件中加入包含Spi.h的语句；这样即可正常
 *          使用头文件中声明的程序了；详细可参考 本工程 和
 *          main.c
 *              如 果要改为USART1模块，只需把宏定义的x 对应的0改为
 *              1即可；如果要同时使用0、1两个模块，就需要把函数的名
 *              字该为不同，两个文件分别访问0、1两个模块
@日期	：2021年7月10日
@作者	：楸壳
*******************************************************************************/
#include<msp430f169.h>
#define SPI3LINE P3SEL |= 0X0E //三线模式
#define SPI4LINE P4SEL |= 0X0F //四线模式


/*****************************************************************************
* 名    称：SpiMasterInit
* 功    能：初始化SPI。设置波特率等，主机模式
* 入口参数：
*           baud:       波特率      (300~500k)
                        只能是串口使用时钟的整数分频，spi模式UxMCTL寄存器无效
                        若为整数分频，将分频系数设为 整数部分，将不支持小数分
                        频
            datsBits:   数据位位数  (7或8)
            mode:       停止位位数  (3或4)
                        3：三线模式
                        4：四线模式
            clkMode     时钟模式    (0-3)   控制时钟的极性和相位
                        0： 上升沿发送，下降沿接收 正常UCLK
                        1： 下降沿发送，上升沿接收 正常UCLK
                        2： 下降沿发送，上升沿接收 延时半个周期UCLK
                        3： 上升沿发送，下降沿接收 延时半个周期UCLK
* 出口参数：返回值为1时表初化成功，为0表示参数有错误
* 范    例: SpiMasterInit(9600,8,3,0)  //设成9600bps，8位数据，三线模式，时钟
                                        模式0,9600波特率，是32768三分频，小数
                                        分频无效，实际波特率，将大于9600
            SpiMasterInit(16364,7,4,1) //设成16364bps，7位数据，四线模式,模式
                                        32768，二分频
******************************************************************************/
char SpiInit(long baud, char dataBits, char mode, char clkMode)
{
    long int brclk; //波特率发生器时钟频率

    U0CTL |= SWRST; //初始化
    U0ME |= USPIE0;
    U0CTL &= ~SWRST;
    U0IE |= URXIE0 + UTXIE0;

    U0CTL |= SYNC + MM;//设置spi主机模式

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
    //设置波特率寄存器
    int fen = brclk / baud;         //分频系数
    if (fen < 2)
        return (0);            //分频系数必须大于2
    else
    {
        U0BR0 = fen / 256;
        U0BR1 = fen % 256;
    }

    //------------------------设置数据位-------------------------
    switch (dataBits)
    {
    case 7:
    case '7':
        U0CTL &= ~CHAR;
        break;      //7位数据
    case 8:
    case '8':
        U0CTL |= CHAR;
        break;      //8位数据
    default:
        return (0);                  //参数错误
    }
    //------------------------设置模式---------------------------
    switch (mode)
    {
    case 3:
    case '3':
        U0TCTL |= STC;
        SPI3LINE;
        break;  //三线模式
    case 4:
    case '4':
        U0TCTL &= ~STC;
        SPI4LINE;
        break;  //四线模式
    default:
        return (0);                          //参数错误
    }

    //------------------------设置UCLK模式-----------------------
    switch (clkMode)
    {
    case 0:
    case '0':
        U0TCTL &= ~CKPH;
        U0TCTL &= ~CKPL;
        break;  //模式0
    case 1:
    case '1':
        U0TCTL &= ~CKPH;
        U0TCTL |= CKPL;
        break;  //模式1
    case 2:
    case '2':
        U0TCTL |= CKPH;
        U0TCTL &= ~CKPL;
        break;  //模式2
    case 3:
    case '3':
        U0TCTL |= CKPH;
        U0TCTL |= CKPL;
        break;  //模式3
    default:
        return (0);                                  //参数错误
    }
    return (1);

}
/*****************************************************************************
@函数名    ：SpiWrite(unsigned char dat)
@功能 ：传输一字节数据并同时接收数据
@输入 ：dat数据
@输出 ：dat数据
******************************************************************************/
unsigned char SpiWrite(unsigned char c)
{
    U0TXBUF = c;
    while((IFG1 & UTXIFG0) == 0);
    return (U0RXBUF);
}
/*****************************************************************************
@函数名    ：SpiWrite
@功能 ：传输多字节数据
@输入 ：SendData传输的数据,Length数据的长度
@输出 ：物
@示例：char SendData[10] = "hello";SpiWriteString(SendData,Strlen("hello"));
******************************************************************************/
unsigned char SpiWriteString(char *SendData,char *ReceiveData,int Length)
{
    while(Length--)
    {
      *ReceiveData++=SPI_SendReadByte(*SendData++);
    }
}






