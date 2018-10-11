/* 
 * File:   ADC_K22.h
 * Author: Hiroki
 *
 * Created on 1 de Maio de 2016, 00:18
 */

#ifndef ADC_K22_H
#define	ADC_K22_H

#include "Config_XC8.h"

/*_____________________TABELA DE ADC_________________________*/

#define AN_0	0b00000		//(pin 2    RA0)
#define AN_1	0b00001		//(pin 3    RA1)
#define AN_2	0b00010		//(pin 4    RA2)
#define AN_3	0b00011		//(pin 5    RA3)
#define AN_4	0b00100		//(pin 7    RA5)
#define AN_5	0b00101		//(pin 8    RE0)
#define AN_6	0b00110		//(pin 9    RE1)
#define AN_7	0b00111		//(pin 10   RE2)
#define AN_8	0b01000		//(pin 35   RB2)
#define AN_9	0b01001		//(pin 36   RB3)
#define AN_10	0b01010		//(pin 34   RB1)
#define AN_11	0b01011		//(pin 37   RB4)
#define AN_12	0b01100		//(pin 33   RB0)
#define AN_13	0b01101		//(pin 38   RB5)
#define AN_14	0b01110		//(pin 17   RC2)
#define AN_15	0b01111		//(pin 18   RC3)
#define AN_16	0b10000		//(pin 23   RC4)
#define AN_17	0b10001		//(pin 24   RC5)
#define AN_18	0b10010		//(pin 25   RC6)
#define AN_19	0b10011		//(pin 26   RC7)
#define AN_20	0b10100		//(pin 19   RD0)
#define AN_21	0b10101		//(pin 20   RD1)
#define AN_22	0b10110		//(pin 21   RD2)
#define AN_23	0b10111		//(pin 22   RD3)
#define AN_24	0b11000		//(pin 27   RD4)
#define AN_25	0b11001		//(pin 28   RD5)
#define AN_26	0b11010		//(pin 29   RD6)
#define AN_27	0b11011		//(pin 30   RD7)

#define nENABLE        1
#define nDISABLE       0
#define ON             1
#define OFF            0

#define REQ_ADC        ADCON0bits.ADON

#define Enable_ADC(value)   (value = ON)
#define Disable_ADC(value)  (value = OFF)
/*________________________________________________________________*/

#ifdef	__cplusplus
extern "C" {
#endif

    extern void Config_ADC( void );
    
    extern unsigned int Read_ADC(unsigned char channel);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_K22_H */

