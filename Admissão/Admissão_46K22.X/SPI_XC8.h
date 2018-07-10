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

#define nENABLE        1
#define nDISABLE       0
#define nCLEAR         0
#define nSET           1

#define nINTERRUPT_GLOBAL            INTCONbits.GIE
#define nINTERRUPT_PERIPHERAL        INTCONbits.PEIE

#define nSPI1                        PIE1bits.SSP1IE
#define nSPI1_Flag                   PIR1bits.SSP1IF
#define nSPI2                        PIE1bits.SSP2IE
#define nSPI2_Flag                   PIR3bits.SSP2IF

#define __Interurpt_SPI1__           if(PIR1bits.SSP1IF  && PIE1bits.SSP1IE)
#define __Interurpt_SPI2__           if(PIR3bits.SSP2IF  && PIE1bits.SSP1IE)

#define Enable_IntSPI(value)          (value = nENABLE)
#define Disable_IntSPI(value)         (value = nDISABLE)
#define Clear_IntFlag_SPI(value)   (value = nCLEAR)
#define Enable_Interrupt(value)     (value = nENABLE)
#define Disable_Interrupt(value)    (value = nDISABLE)

extern void delay_ms(long mili);

extern unsigned int Get_SSP1BUF();

extern void Put_SSP1BUF(unsigned char data);

extern void SPI_MASTER1();

extern void MASTER_SPI_WRITE1(unsigned char dados);

extern unsigned char MASTER_SPI_READ1();

extern unsigned char SLAVE_SPI_READ1();

extern void SPI_SLAVE1() ;

/******************************************************************/
extern unsigned int Get_SSP2BUF();

extern void Put_SSP2BUF(unsigned char data);

extern void SPI_MASTER2();

extern void MASTER_SPI_WRITE2(unsigned char dados);

extern unsigned char MASTER_SPI_READ2();

extern unsigned char SLAVE_SPI_READ2();

extern void SPI_SLAVE2();

#endif

