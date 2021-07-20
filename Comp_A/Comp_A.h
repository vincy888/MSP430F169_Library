/*
 * Comp_A.h
 *
 *  Created on: 2021年7月20日
 *      Author: Administrator
 */

#ifndef COMP_A_H_
#define COMP_A_H_

void Init_Clk();                    //系统时钟初始化
void Init_Com_A();                  //com_A初始化
void comp_Open(unsigned char doit); //com_A打开or关闭
void comp_BattCheck(void);          //电量检查
#endif
