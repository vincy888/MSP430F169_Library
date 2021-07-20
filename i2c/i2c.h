/*
 * i2c.h
 *
 *  Created on: 2021Äê7ÔÂ14ÈÕ
 *      Author: 86182
 */

#ifndef I2C_H_
#define I2C_H_

void I2CMasterInit(unsigned char SlaveAddr,long baud,unsigned char fen,unsigned char HCLK,unsigned char LCLK);
void I2CWriteByte(unsigned char Addr,unsigned char Val);
unsigned char I2CReadByte(unsigned char Addr);
unsigned char I2CWriteStr(unsigned char Addr,unsigned char *Val,int Length);




#endif /* I2C_H_ */
