/*
 * flash.h
 *
 *  Created on: 2019��5��30��
 *      Author: zengzhaoyou
 */

#ifndef FLASH_H_
#define FLASH_H_


//flash������չ
extern unsigned int SegAddr,SegPre;
//��ʼ��
unsigned char Flash_Init(unsigned char Div,unsigned char Seg);
//д����
unsigned char Flash_Direct_WriteChar(unsigned int addr , unsigned char Data);
//������
unsigned char Flash_ReadChar(unsigned int Addr);
//���ú���
void Flash_Erase(void);



#endif /* FLASH_H_ */
