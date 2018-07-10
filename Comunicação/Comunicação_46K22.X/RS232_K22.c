#include <pic18f46k22.h>
#include "RS232_K22.h"
#include "Config_XC8.h"
#include <string.h>
/*************************************************** Funções RS232 *********************************************/
void Init1_RS232(long velocity,unsigned int mode)
{////por padrão é usado o modo 8 bits e sem paridade, mas se necessario ajuste aqui a configuração desejada.
    //verifique datasheet para ver a porcentagem de erro e se a velocidade é possivel para o cristal utilizado.
    Pin_RX1 = Pin_IN;//configura portB  B1 (pino RX) como entrada
    Pin_TX1 = Pin_OUT;
    RCSTA1 = 0X90;//habilita porta serial,recepção de 8 bit em modo continuo,assincrono.     1001 0000
    int value;
        if(mode == 1)
        {//modo = 1 ,modo alta velocidade
         TXSTA1 = 0X24;//modo assincrono,trasmissao 8 bits.  0010 0100
         value =(unsigned int)(((_XTAL_FREQ/velocity)-16)/16);//calculo do valor do gerador de baud rate
        }
        else
        {//modo = 0 ,modo baixa velocidade
         TXSTA1 = 0X20;//modo assincrono,trasmissao 8 bits.
         value =(unsigned int)(((_XTAL_FREQ/velocity)-64)/64);//calculo do valor do gerador de baud rate
        }
    
    /*BRG16: 16-bit Baud Rate Generator bit
      1 = 16-bit Baud Rate Generator is used (SPBRGHx:SPBRGx)
      0 = 8-bit Baud Rate Generator is used (SPBRGx)*/
    BAUDCON1bits.BRG16 = 0;
    SPBRG1 = value;
    RX_COM1 = 1;//habilita interrupção de recepção
    TX_COM1 = 0;//deixa interrupção de transmissão desligado(pois corre se o risco de ter uma interrupção escrita e leitura ao mesmo tempo)
}

void Init2_RS232(long velocity,unsigned int mode)
{////por padrão é usado o modo 8 bits e sem paridade, mas se necessario ajuste aqui a configuração desejada.
    //verifique datasheet para ver a porcentagem de erro e se a velocidade é possivel para o cristal utilizado.
    Pin_TX2 = Pin_OUT;
    Pin_RX2 = Pin_IN;
    RCSTA2 = 0X90;//habilita porta serial,recepção de 8 bit em modo continuo,assincrono.     1001 0000
    int value;
        if(mode == 1)
        {//modo = 1 ,modo alta velocidade
         TXSTA2 = 0X24;//modo assincrono,trasmissao 8 bits.  0010 0100
         value =(unsigned int)(((_XTAL_FREQ/velocity)-16)/16);//calculo do valor do gerador de baud rate
        }
        else
        {//modo = 0 ,modo baixa velocidade
         TXSTA2 = 0X20;//modo assincrono,trasmissao 8 bits.
         value =(unsigned int)(((_XTAL_FREQ/velocity)-64)/64);//calculo do valor do gerador de baud rate
        }
    
    /*BRG16: 16-bit Baud Rate Generator bit
      1 = 16-bit Baud Rate Generator is used (SPBRGHx:SPBRGx)
      0 = 8-bit Baud Rate Generator is used (SPBRGx)*/
    BAUDCON2bits.BRG16 = value;
    //SPBRG = value;
    RX_COM2 = 1;//habilita interrupção de recepção
    TX_COM2 = 0;//deixa interrupção de transmissão desligado(pois corre se o risco de ter uma interrupção escrita e leitura ao mesmo tempo)
}

void Putc_TX1(unsigned char valor)
{
    TX1IF = 0;//limpa flag que sinaliza envio completo.
    TXREG1 = valor;
    while(TX1IF == 0);//espera enviar caracter
    __delay_us(10);
}

void Putc_TX2(unsigned char valor)
{
    TX2IF = 0;//limpa flag que sinaliza envio completo.
    TXREG2 = valor;
    while(TX2IF == 0);//espera enviar caracter
}

void Putc_Text_TX1(const char frase[])
{
     char indice = 0;
     char tamanho = strlen(frase);
      while(indice < tamanho ) ///veja que o programa pode travar se aqui não tiver as duas aspas
       {
           Putc_TX1(frase[indice]);
           indice++;
       }
}

void Putc_Text_TX2(const char frase[])
{
     char indice = 0;
     char tamanho = strlen(frase);
      while(indice < tamanho ) ///veja que o programa pode travar se aqui não tiver as duas aspas
       {
           Putc_TX2(frase[indice]);
           __delay_us(10);
           indice++;
       }
}

unsigned char Getc_RX1()
{
  return RCREG1;
}

unsigned char Getc_RX2()
{
  return RCREG2;
}

void Getc_Text_RX1(unsigned char *Output, unsigned int length)
{
  unsigned int i;
  for(int i=0;i<length;i++)
  Output[i] = Getc_RX1();
}

void Getc_Text_RX2(unsigned char *Output, unsigned int length)
{
  unsigned int i;
  for(int i=0;i<length;i++)
  Output[i] = Getc_RX2();
}

unsigned char UART_Data_Ready1()
{
  return RC1IF;
}

unsigned char UART_Data_Ready2()
{
  return RC2IF;
}

unsigned int Convert_Int(unsigned char data)
{
    return (data - 0x30);
}

/**************************************************************************************************************/
