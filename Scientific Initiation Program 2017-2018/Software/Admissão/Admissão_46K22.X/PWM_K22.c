/*
 *  Library PWM_XC8
 *  Set 5 CCP´S
 *  Autor: Gustavo Hiroki
 * 
 */
#include "Config_XC8.h"

void Configure_PWM1(void)
{
    // Set the PWM to the options selected in the MPLAB(c) Code Configurator

    // CCP4M PWM; DC4B 1; 
    CCP1CON = 0x1C;
    
    // CCPR4L 12; 
    CCPR1L = 0x0C;
    
    // CCPR4H 0; 
    CCPR1H = 0x00;
    PR2 = 250;//PAULO
    
    ECCP1ASbits.CCP1ASE = 0;
    ECCP1ASbits.CCP1AS =  0;    
    
    // Selecting Timer 2
    /*
     C3TSEL<1:0>: CCP3 Timer Selection bits
     00 = CCP3 ? Capture/Compare modes use Timer1, PWM modes use Timer2
     01 = CCP3 ? Capture/Compare modes use Timer3, PWM modes use Timer4
     10 = CCP3 ? Capture/Compare modes use Timer5, PWM modes use Timer6
     11 = Reserved
     */
    CCPTMRS0bits.C1TSEL = 0x00;
}

void Configure_PWM2(void)
{
    // Set the PWM to the options selected in the MPLAB(c) Code Configurator

    // CCP4M PWM; DC4B 1; 
    CCP2CON = 0x1C;
    
    // CCPR4L 12; 
    CCPR2L = 0x0C;
    
    // CCPR4H 0; 
    CCPR2H = 0x00;
   
    
    PR2 = 250;//PAULO
    // Selecting Timer 2
    
    ECCP2ASbits.CCP2ASE = 0;
    ECCP2ASbits.CCP2AS =  0; 
    
    /*
     C3TSEL<1:0>: CCP3 Timer Selection bits
     00 = CCP3 ? Capture/Compare modes use Timer1, PWM modes use Timer2
     01 = CCP3 ? Capture/Compare modes use Timer3, PWM modes use Timer4
     10 = CCP3 ? Capture/Compare modes use Timer5, PWM modes use Timer6
     11 = Reserved
     */
    CCPTMRS0bits.C2TSEL0 = 0;
    CCPTMRS0bits.C2TSEL1 = 0;
}

void Configure_PWM3(void)
{
    // Set the PWM to the options selected in the MPLAB(c) Code Configurator

    // CCP4M PWM; DC4B 1; 
    CCP3CON = 0x1C;
    
    // CCPR4L 12; 
    CCPR3L = 0x0C;
    
    // CCPR4H 0; 
    CCPR3H = 0x00;
    
    PR2 = 74;//PAULO
    // Selecting Timer 2
    
    ECCP3ASbits.CCP3ASE = 0;
    ECCP3ASbits.CCP3AS =  0; 
 
    /*
     C3TSEL<1:0>: CCP3 Timer Selection bits
     00 = CCP3 ? Capture/Compare modes use Timer1, PWM modes use Timer2
     01 = CCP3 ? Capture/Compare modes use Timer3, PWM modes use Timer4
     10 = CCP3 ? Capture/Compare modes use Timer5, PWM modes use Timer6
     11 = Reserved
     */
    //CCPTMRS0bits.C3TSEL = 0x00;
    CCPTMRS0bits.C3TSEL0 = 0;
    CCPTMRS0bits.C3TSEL1 = 0;
}

void Configure_PWM4(void)
{
    // Set the PWM to the options selected in the MPLAB(c) Code Configurator

    // CCP4M PWM; DC4B 1; 
    CCP2CON = 0x1C;
    
    // CCPR4L 12; 
    CCPR2L = 0x0C;
    
    // CCPR4H 0; 
    CCPR2H = 0x00;
    
    // Selecting Timer 2
    /*
     C3TSEL<1:0>: CCP3 Timer Selection bits
     00 = CCP3 ? Capture/Compare modes use Timer1, PWM modes use Timer2
     01 = CCP3 ? Capture/Compare modes use Timer3, PWM modes use Timer4
     10 = CCP3 ? Capture/Compare modes use Timer5, PWM modes use Timer6
     11 = Reserved
     */
    CCPTMRS1bits.C4TSEL = 0x00;
}

void Configure_PWM5(void)
{
    // Set the PWM to the options selected in the MPLAB(c) Code Configurator

    // CCP4M PWM; DC4B 1; 
    CCP5CON = 0x1C;
    
    // CCPR4L 12; 
    CCPR5L = 0x0C;
    
    // CCPR4H 0; 
    CCPR5H = 0x00;
    
    // Selecting Timer 2
    /*
     C3TSEL<1:0>: CCP3 Timer Selection bits
     00 = CCP3 ? Capture/Compare modes use Timer1, PWM modes use Timer2
     01 = CCP3 ? Capture/Compare modes use Timer3, PWM modes use Timer4
     10 = CCP3 ? Capture/Compare modes use Timer5, PWM modes use Timer6
     11 = Reserved
     */
    CCPTMRS1bits.C5TSEL = 0x00;
}
/************************************************************************************/

void Set_PWM1_Duty(unsigned int dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR1L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP1CON = (CCP1CON & 0xCF) | ((dutyValue & 0x0003)<<4);
}

void Set_PWM2_Duty(unsigned int dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR2L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP2CON = (CCP2CON & 0xCF) | ((dutyValue & 0x0003)<<4);
}

void Set_PWM3_Duty(unsigned int dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR3L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP3CON = (CCP3CON & 0xCF) | ((dutyValue & 0x0003)<<4);
}

void Set_PWM4_Duty(unsigned int dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR4L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP4CON = (CCP4CON & 0xCF) | ((dutyValue & 0x0003)<<4);
}

void Set_PWM5_Duty(unsigned int dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR5L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP5CON = (CCP5CON & 0xCF) | ((dutyValue & 0x0003)<<4);
}
