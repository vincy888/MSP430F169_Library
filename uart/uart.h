#ifndef UART_H_
#define UART_H_



void UART_Init();                     //初始化UART模块
void Send_Byte(uchar data)            //向串口发送字节数据
void Send_String(unsigned char *p) //向串口发送字符串数据
 
#endif
