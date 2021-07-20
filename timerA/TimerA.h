/*
 * TimerA.h
 *
 *  Created on: 2021Äê7ÔÂ19ÈÕ
 *      Author: 86182
 */

#ifndef TIMERA_H_
#define TIMERA_H_

void TimerInit(unsigned char timer);
void TimerCaptureInit(unsigned char mode,unsigned char type);
char TAPwmInit(char Clk,char Div,char Mode1,char Mode2);
void TAPwmSetPeriod(unsigned int Period);
void TAPwmSetDuty(char Channel,unsigned int Duty);
void TAPwmSetPermill(char Channel,unsigned int Percent);



#endif /* TIMERA_H_ */
