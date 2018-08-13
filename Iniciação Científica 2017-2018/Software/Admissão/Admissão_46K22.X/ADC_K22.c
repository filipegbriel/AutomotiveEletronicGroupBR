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
    
	  ANSELA = 0b00000011;          /*AN0*/ 
//    ANSELB = 0x00;
//    ANSELC = 0x00;
//    ANSELD = 0x00;
//    ANSELE = 0x00;
	   
    // ADC Setup //
    ADCON2bits.ADFM = 1;        /* ADFM: A/D Result Format Select bit */
	/*				  |			 0 - ADC Result is left justified */
	/*				  |--------- 1 - ADC Result is right justified */
    
    //Page 260
    ADCON1bits.PVCFG = 00;
    ADCON1bits.NVCFG = 00;        /* VCFG0: Voltage Reference Configuration bit (VREF+ source) */

    
    //ADCON2bits.ADCS = 0b100;    //Fosc/64 is the conversion clock
                                //With a Fosc of 20MHz, Fosc/8 Results in a Tad of 3,2us.
    /*
    ADCS<2:0>: A/D Conversion Clock Select bits
    000 = FOSC/2
    001 = FOSC/8
    010 = FOSC/32
    011 = FRC(1) (clock derived from a dedicated internal oscillator = 600 kHz nominal)
    100 = FOSC/4
    101 = FOSC/16
    110 = FOSC/64
    111 = FRC(1) (clock derived from a dedicated internal oscillator = 600 kHz nominal)
     */
    ADCON2bits.ADCS0 = 1;       //Page 261
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS2 = 0;
    
    
    /*
    ACQT<2:0>: A/D Acquisition time select bits. Acquisition time is the duration that the A/D charge
    holding capacitor remains connected to A/D channel from the instant the GO/DONE bit is set until
    conversions begins.
    000 = 0(1)
    001 = 2 TAD
    010 = 4 TAD
    011 = 6 TAD
    100 = 8 TAD
    101 = 12 TAD
    110 = 16 TAD
    111 = 20 TAD
     */
    ADCON2bits.ACQT = 0b100;
    
    //ADCON0bits.CHS = 0;         //select analog input, AN0  Page 259
    
    Enable_ADC(REQ_ADC);        //Turn on the ADC       Page 259
}

unsigned int Read_ADC(unsigned char channel)             //Função para leitura do ADC
{   
    ADCON0bits.CHS = channel;
    //__delay_us(60);
    ADCON0bits.GO = 1;              //start the conversion Page 259
    while(ADCON0bits.GO == 1){};    //wait for the conversion to end
    return ((ADRESH<<8)+ADRESL);	//combine the 10 bits of the conversion
    //return ADRESH;
}
