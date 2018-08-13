/*
 * File:   main.c
 * Author: Hiroki
 *  ADC_XC8 TCC 18F46K22
 * Created on 30 de Abril de 2016, 00:35
 */
#include <pic18f46k22.h>

#include "Config_XC8.h"
#include "ADC_K22.h"

#define     nDigital_Pin         0
#define     nAnalog_Pin          1

void Config_ADC( void )                                 //Configuração dos registradores pertencentes ao ADC
{
    /*1 = ANALOG        0 = DIGITAL*/
    
	  //ANSELA = 0b00100111;          /*0001 0111 AN0,AN1,AN2,AN4*/ 
    
    ANSELAbits.ANSA0 = nAnalog_Pin;           //Pedal1 Analog Input   AN0 
    //ANSELAbits.ANSA1 = nAnalog_Pin;         //Pedal2 Analog Input   AN1 
    ANSELAbits.ANSA2 = nAnalog_Pin;           //Lambda1 Analog Input  AN2
    //ANSELAbits.ANSA3 = nAnalog_Pin;         
    //ANSELAbits.ANSA4 = nAnalog_Pin;
    ANSELAbits.ANSA5 = nAnalog_Pin;         //Water Temp Analog Input AN4
    
    ANSELEbits.ANSE0 = nAnalog_Pin;         //Map Analog Input AN5
    
    ANSELBbits.ANSB4 = nAnalog_Pin;         //Air Temp Analog Input AN11
    
    ANSELBbits.ANSB2 = nAnalog_Pin;         //Battery Analog Input AN8
	   
    // ADC Setup //
    ADCON2bits.ADFM = 0;        /* ADFM: A/D Result Format Select bit */        /*Configurado para 8 Bit´s*/
	/*				  |			 0 - ADC Result is left justified */
	/*				  |--------- 1 - ADC Result is right justified */
    
    //Page 260
    ADCON1bits.PVCFG = 00;
    ADCON1bits.NVCFG = 00;        /* VCFG0: Voltage Reference Configuration bit (VREF+ source) */

    
    //ADCON2bits.ADCS = 0b100;    //Fosc/64 is the conversion clock
                                //With a Fosc of 20MHz, Fosc/8 Results in a Tad of 3,2us.
    ADCON2bits.ADCS0 = 1;       //Page 261
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS2 = 0;
    
    ADCON2bits.ACQT = 100;
    
    //ADCON0bits.CHS = 0;         //select analog input, AN0  Page 259
    
    Enable_ADC(REQ_ADC);        //Turn on the ADC       Page 259
}

unsigned int Read_ADC(unsigned char channel)             //Função para leitura do ADC
{   
    ADCON0bits.CHS = channel;
    __delay_us(10);
    ADCON0bits.GO = 1;              //start the conversion Page 259
    while(ADCON0bits.GO == 1);    //wait for the conversion to end
    //return ((ADRESH<<8)+ADRESL);	//combine the 10 bits of the conversion
    return ADRESH;
}
