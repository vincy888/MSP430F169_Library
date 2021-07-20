/*
 * spi.h
 *
 *  Created on: 2021Äê7ÔÂ11ÈÕ
 *      Author: 86182
 */

#ifndef SPI_H_
#define SPI_H_

char SpiInit(long baud, char dataBits, char mode, char clkMode);
unsigned char SpiWrite(unsigned char dat);
unsigned char SpiWriteString(char *SendData,,unit16_t Length)£»



#endif /* SPI_H_ */
