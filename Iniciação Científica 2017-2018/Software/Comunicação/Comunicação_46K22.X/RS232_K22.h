/* 
 * File:   RS232_K22.h
 * Author: Hiroki
 *
 * Created on 7 de Julho de 2016, 12:48
 */

#ifndef RS232_K22_H
#define	RS232_K22_H
#include "Config_XC8.h"
#include <string.h>

#define Pin_TX1     TRISC6
#define Pin_RX1     TRISC7
#define Pin_TX2     TRISD6
#define Pin_RX2     TRISD7

#define nENABLE        1
#define nDISABLE       0
#define nCLEAR         0
#define nSET           1
#define Pin_OUT        0
#define Pin_IN         1
#define OFF        0
#define ON         1

#define RX_COM1     RC1IE
#define TX_COM1     TX1IE
#define RX_COM2     RC2IE
#define TX_COM2     TX2IE

#define TX_Flag1    TX1IF   
#define TX_Flag2    TX2IF
#define RX_Flag1    RC1IF
#define RX_Flag2    RC2IF

#define nINTERRUPT_GLOBAL         INTCONbits.GIE
#define nINTERRUPT_PERIPHERAL     INTCONbits.PEIE

#define __Interrupt_RX1__    if(RC1IF && RC1IE)
#define __Interrupt_RX2__    if(RC2IF && RC2IE)
#define __Interrupt_TX1__    if(TX1IF && TX1IE)
#define __Interrupt_TX2__    if(TX2IF && TX2IE)

#define Enable_ComTX1(value)            (value = nENABLE)
#define Enable_ComTX2(value)            (value = nENABLE)
#define Enable_ComRX1(value)            (value = nENABLE)
#define Enable_ComRX2(value)            (value = nENABLE)
#define Enable_Interrupt(value)         (value = nENABLE)
#define ClearFlag_Interrupt(value)      (value = nCLEAR)

#ifdef	__cplusplus
extern "C" {
#endif

    extern void Init1_RS232(long velocity,unsigned int mode);
    
    extern void Init2_RS232(long velocity,unsigned int mode);
    
    extern void Putc_TX1(unsigned char valor);
    
    extern void Putc_TX2(unsigned char valor);
    
    extern void Putc_Text_TX1(const char frase[]);
    
    extern void Putc_Text_TX2(const char frase[]);
    
    extern unsigned char Getc_RX1();
    
    extern unsigned char Getc_RX2();
    
    extern void Getc_Text_RX1(unsigned char *Output, unsigned int length);
    
    extern void Getc_Text_RX2(unsigned char *Output, unsigned int length);
    
    extern unsigned char UART_Data_Ready1();
    
    extern unsigned char UART_Data_Ready2();
    
    extern unsigned int Convert_Int(unsigned char data);

#ifdef	__cplusplus
}
#endif

#endif	/* RS232_K22_H */

