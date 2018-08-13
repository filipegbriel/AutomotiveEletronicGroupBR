/*
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 7 de Abril de 2016, 22:39
 */
#include "SPI_XC8.h"
#include "Config_XC8.h"

void delay_ms(long mili)
{
   while(mili > 0)
   {
      _delay(2927);//aprox. 1ms com clock de 12mhz
      mili--;
   }
}

void SPI_MASTER1()                   //Slecionar modo mestre
{
  //SSP1IP = 1;
  SSP1CON1 = 0b00110001;                    //habilita modo SPI // FOSC/4 //
  SSP1STAT = 0b00000000;              //pega amostras no fim do bit e a trasmiss�o ser� na borda de descida
  //TRISA5 = 0;                         //A5 SSC = out
  TRISC3 = 0;                         //C3 = SCK out/ C4 = SD1 in/ C5 = SDO out
  TRISC4 = 1;
  TRISC5 = 0;
  //ANSELA = 0x00;                     //desabilita porta analogicas(para n�o atrapalhar recep��o no pino)
  //ANSELH = 0x00;                   //em alguns casos n�o funciona nem a interrup��o de recep��o.
}

void MASTER_SPI_WRITE1(unsigned char dados)
{

    //LATAbits.LA5 = 0;               // ativa CS chip select Podendo ser qualquer pino
    //SSP1BUF = 0X01;                  //opcode que sinaliza que quer deixar um dado
    //while(!SSP1STATbits.BF);         //espera completar envio de dados
    //__delay_us(10);                    //espera slave processar informa��o
    SSP1BUF = dados;                 //envia os dados que ser� colocado no registrador
    while(!SSP1STATbits.BF);         //espera completar envio de dados
    //SSP1BUF = 0;
    //LATAbits.LA5 = 1;               //desabilita chipANSELA = 0x00;                     //desabilita porta analogicas(para n�o atrapalhar recep��o no pino
    //return(SSPBUF);
}

unsigned char MASTER_SPI_READ1()
{
   //LATAbits.LA5 = 0;                // ativa SS(slave select)
   //SSP1BUF = 0X00;                   //um valor qualquer para pegar os dados
                                    //desse registrador(assim gera o clock necessario para isto)
   while(!SSP1STATbits.BF);          //espera completar envio de dados
   //LATAbits.LA5 = 1;                //desabilita chip
  return(SSP1BUF);
}

void SPI_SLAVE1()                 //Seleciona modo slave
{
  SSP1CON1 = 0X24;                   //habilita modo SPI // habilita pino SS(slave select) pois � mais usual assim
  SSP1STAT = 0X40;                   //trasmiss�o ser� na borda de descida
  TRISA5 = 1;               //A5 SS in
  TRISC3 = 1;               //C3 = SCK in/ C4 = SDI in/ C5 SDO out
  TRISC4 = 1;
  TRISC5 = 0;
  ANSELA = 0x00;                     //desabilita porta analogicas(para n�o atrapalhar recep��o no pino
}

unsigned char SLAVE_SPI_READ1()
{
  while(!SSP1STATbits.BF);//espera completar recep��o
  return(SSP1BUF);
}

/***********************************************************************************************/

void SPI_MASTER2()                   //Slecionar modo mestre
{
  //SSP2IP = 1;
  SSP2CON1 = 0b00110000;              //habilita modo SPI // FOSC/4 //
  SSP2STAT = 0X00;                    //pega amostras no fim do bit e a trasmiss�o ser� na borda de descida
  TRISD0 = 0;
  TRISD1 = 1;
  //TRISD3 = 0;
  TRISD4 = 0;
  //ANSELA = 0x00;                     //desabilita porta analogicas(para n�o atrapalhar recep��o no pino)
  //ANSELH = 0x00;                   //em alguns casos n�o funciona nem a interrup��o de recep��o.
}

void MASTER_SPI_WRITE2(unsigned char dados)
{

    //LATAbits.LA5 = 0;               // ativa CS chip select
    //SSP2BUF = 0X01;                  //opcode que sinaliza que quer deixar um dado
    //while(!SSP2STATbits.BF);         //espera completar envio de dados
//    __delay_us(10);                    //espera slave processar informa��o
    SSP2BUF = dados;                 //envia os dados que ser� colocado no registrador
    while(!SSP2STATbits.BF);         //espera completar envio de dados
    //LATAbits.LA5 = 1;               //desabilita chipANSELA = 0x00;                     //desabilita porta analogicas(para n�o atrapalhar recep��o no pino
    //return(SSPBUF);
}

unsigned char MASTER_SPI_READ2()
{
   //LATAbits.LA5 = 0;                // ativa SS(slave select)
   SSP2BUF = 0X00;                   //um valor qualquer para pegar os dados
                                    //desse registrador(assim gera o clock necessario para isto)
   while(!SSP2STATbits.BF);          //espera completar envio de dados
   //LATAbits.LA5 = 1;                //desabilita chip
  return(SSP2BUF);
}

void SPI_SLAVE2()                 //Seleciona modo slave
{
  SSP2CON1 = 0X24;                   //habilita modo SPI // habilita pino SS(slave select) pois � mais usual assim
  SSP2STAT = 0X40;                   //trasmiss�o ser� na borda de descida
  TRISD0 = 1;
  TRISD1 = 1;
  TRISD3 = 1;
  TRISD4 = 0;
  ANSELA = 0x00;                     //desabilita porta analogicas(para n�o atrapalhar recep��o no pino
}

unsigned char SLAVE_SPI_READ2()
{
  while(!SSP2STATbits.BF);//espera completar recep��o
  return(SSP2BUF);
}

/*  Registradores para INTERRUP��O
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.SSPIE = 1; 
 * 
 * PIE3bits.SSP2IE
 * PIE1bits.SSP1IE
 */