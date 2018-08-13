/* 
 * File:   PWM_K22.h
 * Author: Hiroki
 *
 * Created on 13 de Julho de 2016, 22:43
 */

#ifndef PWM_K22_H
#define	PWM_K22_H

#ifdef	__cplusplus
extern "C" {
#endif

    extern void Configure_PWM1(void);
    
    extern void Configure_PWM2(void);
    
    extern void Configure_PWM3(void);
    
    extern void Configure_PWM4(void);
    
    extern void Configure_PWM5(void);
    
    extern void Set_PWM1_Duty(unsigned int dutyValue);
    
    extern void Set_PWM2_Duty(unsigned int dutyValue);
    
    extern void Set_PWM3_Duty(unsigned int dutyValue);
    
    extern void Set_PWM4_Duty(unsigned int dutyValue);
    
    extern void Set_PWM5_Duty(unsigned int dutyValue);

#ifdef	__cplusplus
}
#endif

#endif	/* PWM_K22_H */

