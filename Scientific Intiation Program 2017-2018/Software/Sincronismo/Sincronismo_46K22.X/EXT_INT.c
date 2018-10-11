/* 
 * File:   EXT_INT.c
 * Author: Hiroki
 *
 * Created on 2 de Julho de 2016, 12:57
 */

#include "Config_XC8.h"
#include "EXT_INT.h"
#include <stdlib.h>

void Confi_Int_Ext0()
{
    TRISB0 = nINPUT;
    
    /*
     INTEDG0: External Interrupt 0 Edge Select bit
     1 = Interrupt on rising edge
     0 = Interrupt on falling edge
     */
    INTCON2bits.INTEDG0 = 0;
    
    /*
     RBIP: RB Port Change Interrupt Priority bit
     1 = High priority
     0 = Low priority
     */
    INTCONbits.RBIE = 1;
}

void Confi_Int_Ext1()
{
    TRISB1 = nINPUT;
    
    /*
    INTEDG1: External Interrupt 1 Edge Select bit
    1 = Interrupt on rising edge
    0 = Interrupt on falling edge
     */
    INTCON2bits.INTEDG1 = 1;
    
    /*
     RBIP: RB Port Change Interrupt Priority bit
     1 = High priority
     0 = Low priority
     */
    INTCONbits.RBIE = 0;
}

void Confi_Int_Ext2()
{
    TRISB2 = nINPUT;
    
    /*
    INTEDG1: External Interrupt 1 Edge Select bit
    1 = Interrupt on rising edge
    0 = Interrupt on falling edge
     */
    INTCON2bits.INTEDG2 = 1;
    
    /*
     RBIP: RB Port Change Interrupt Priority bit
     1 = High priority
     0 = Low priority
     */
}
