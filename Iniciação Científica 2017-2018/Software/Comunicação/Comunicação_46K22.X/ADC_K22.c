/*
 * File:   main.c
 * Author: Hiroki
 *  ADC_XC8 TCC 18F46K22
 * Created on 30 de Abril de 2016, 00:35
 */
#include "Config_XC8.h"
#include "ADC_K22.h"

void Config_ADC( void )                                 //Configuração dos registradores pertencentes ao ADC
{
    /*1 = ANALOG        0 = DIGITAL*/
    
	  ANSELA = 0b00100111;          /*0001 0111 AN0,AN1,AN2,AN4*/ 
//    ANSELB = 0x00;
//    ANSELC = 0x00;
//    ANSELD = 0x00;
//    ANSELE = 0x00;
	   
    // ADC Setup //
    ADCON2bits.ADFM = 0;        /* ADFM: A/D Result Format Select bit */        /*Configrado para 8 Bit´s*/
	/*				  |			 0 - ADC Result is left justified */
	/*				  |--------- 1 - ADC Result is right justified */
    
    //Page 260
    ADCON1bits.PVCFG = 00;
    ADCON1bits.NVCFG = 00;        /* VCFG0: Voltage Reference Configuration bit (VREF+ source) */

    
    //ADCON2bits.ADCS = 0b100;    //Fosc/64 is the conversion clock
                                //With a Fosc of 20MHz, Fosc/8 Results in a Tad of 3,2us.
    ADCON2bits.ADCS0 = 1;       //Page 261
    ADCON2bits.ADCS1 = 0;
    ADCON2bits.ADCS2 = 1;
    
    ADCON2bits.ACQT = 110;
    
    //ADCON0bits.CHS = 0;         //select analog input, AN0  Page 259
    
    Enable_ADC(REQ_ADC);        //Turn on the ADC       Page 259
}

unsigned int Read_ADC(unsigned char channel)             //Função para leitura do ADC
{   
    ADCON0bits.CHS = channel;
    __delay_ms(15);
    ADCON0bits.GO = 1;              //start the conversion Page 259
    while(ADCON0bits.GO == 1){};    //wait for the conversion to end
    //return ((ADRESH<<8)+ADRESL);	//combine the 10 bits of the conversion
    return ADRESH;
}
