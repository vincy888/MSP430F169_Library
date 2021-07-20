/*
 * flash.h
 *
 *  Created on: 2019年5月30日
 *      Author: zengzhaoyou
 */

#ifndef FLASH_H_
#define FLASH_H_


//flash变量拓展
extern unsigned int SegAddr,SegPre;
//初始化
unsigned char Flash_Init(unsigned char Div,unsigned char Seg);
//写函数
unsigned char Flash_Direct_WriteChar(unsigned int addr , unsigned char Data);
//读函数
unsigned char Flash_ReadChar(unsigned int Addr);
//擦拭函数
void Flash_Erase(void);



#endif /* FLASH_H_ */
