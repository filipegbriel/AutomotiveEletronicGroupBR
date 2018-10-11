/* 
 * File:   SPI_MASTER_18F.h
 * Author: Hiroki
 *
 * Created on 14 de Abril de 2016, 23:26
 */

#ifndef __SPI_MASTER_18F
#define __SPI_MASTER_18F
//#include <xc.h>
#include "Config_XC8.h"

extern void delay_ms(long mili);

extern void SPI_MASTER1();

extern void MASTER_SPI_WRITE1(unsigned char dados);

extern unsigned char MASTER_SPI_READ1();

extern unsigned char SLAVE_SPI_READ1();

extern void SPI_SLAVE1() ;

/******************************************************************/

extern void SPI_MASTER2();

extern void MASTER_SPI_WRITE2(unsigned char dados);

extern unsigned char MASTER_SPI_READ2();

extern unsigned char SLAVE_SPI_READ2();

extern void SPI_SLAVE2();

#endif

