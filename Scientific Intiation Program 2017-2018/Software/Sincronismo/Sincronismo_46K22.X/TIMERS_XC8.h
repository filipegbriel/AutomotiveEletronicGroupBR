/* 
 * File:   TIMERS_XC8.h
 * Author: Hiroki
 *
 * Created on 25 de Abril de 2016, 21:44
 */

#ifndef TIMERS_XC8_H
#define	TIMERS_XC8_H

#include "Config_XC8.h"

/******************************************** DEFINES ************************************************/
#define nENABLE        1
#define nDISABLE       0
#define nCLEAR         0
#define nSET           1

#define nINTERRUPT_GLOBAL         INTCONbits.GIE
#define nINTERRUPT_PERIPHERAL     INTCONbits.PEIE

#define nINT_TIMER0   INTCONbits.TMR0IE
#define nINT_TIMER1   PIE1bits.TMR1IE
#define nINT_TIMER2   PIE1bits.TMR2IE
#define nINT_TIMER3   PIE2bits.TMR3IE
#define nINT_TIMER4   PIE5bits.TMR4IE
#define nINT_TIMER5   PIE5bits.TMR5IE
#define nINT_TIMER6   PIE5bits.TMR6IE

#define nFLAG_TMR0          INTCONbits.TMR0IF
#define nFLAG_TMR1          PIR1bits.TMR1IF
#define nFLAG_TMR2          PIR1bits.TMR2IF
#define nFLAG_TMR3          PIR2bits.TMR3IF
#define nFLAG_TMR4          PIR5bits.TMR4IF
#define nFLAG_TMR5          PIR5bits.TMR5IF
#define nFLAG_TMR6          PIR5bits.TMR6IF

#define ClearFlag_Interrupt(value)      (value = nCLEAR)
#define Enable_Interrupt(value)         (value = nENABLE)
#define Disable_Interrupt(value)        (value = nDISABLE)

#define __Interrupt_TMR0__      if(INTCONbits.TMR0IF && INTCONbits.TMR0IE)
#define __Interrupt_TMR1__      if(PIR1bits.TMR1IF && PIE1bits.TMR1IE)
#define __Interrupt_TMR2__      if(PIR1bits.TMR2IF && PIE1bits.TMR2IE)
#define __Interrupt_TMR3__      if(PIR2bits.TMR3IF && PIE2bits.TMR3IE)
#define __Interrupt_TMR4__      if(PIR5bits.TMR4IF && PIE5bits.TMR4IE)
#define __Interrupt_TMR5__      if(PIR5bits.TMR5IF && PIE5bits.TMR5IE)
#define __Interrupt_TMR6__      if(PIR5bits.TMR6IF && PIE5bits.TMR6IE)

/*****************************************************************************************************/

/******************************************* 16 Bit´s Timer ******************************************/
extern void Config_Timer1( void );
extern void Set_Timer1(unsigned int data_timer );
extern unsigned int Get_Timer1();

extern void Config_Timer3( void );
extern void Set_Timer3(unsigned int data_timer );
extern unsigned int Get_Timer3();

extern void Config_Timer5( void );
extern void Set_Timer5(unsigned int data_timer );
extern unsigned int Get_Timer5();
/*****************************************************************************************************/

/******************************************* 8 Bit´s Timer *******************************************/
extern void Config_Timer2( void );
extern void Set_Timer2(unsigned int data_timer );
extern unsigned int Get_Timer2();

extern void Config_Timer4( void );
extern void Set_Timer4(unsigned int data_timer );
extern unsigned int Get_Timer4();

extern void Config_Timer6( void );
extern void Set_Timer6(unsigned int data_timer );
extern unsigned int Get_Timer6();

extern void Config_Timer0( void );
extern void Set_Timer0(unsigned int data_timer );
extern unsigned int Get_Timer0();
/*****************************************************************************************************/

#endif	/* TIMERS_XC8_H */

