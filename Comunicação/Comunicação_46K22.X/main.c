/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 14 de Julho de 2016, 20:33
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Início declarações e RS232
 * 14/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 14 de Outubro de 2016, 00:38
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Implementação de Timer para COM GOL
 * 14/10/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 18 de Outubro de 2016, 13:06
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Condilção de atualização.
 * 18/10/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 09 de Setembro de 2017, 23:48
 * SW COMUNICAÇÃO VERSÃO 1.0
 * Autor: Antonio Ferreira
 * Modificação: 
 * 09/09/2017
 ******************************************************************************************************/

/**************************************** INCLUDE LIBRARY ********************************************/
#include "RS232_K22.h"
#include "ADC_K22.h"
#include "Config_XC8.h"
#include "EXT_INT.h"
#include "SPI_XC8.h"
#include "TIMERS_XC8.h"
#include "stdlib.h"
#include "MCP2515.h"

/******************************************* DEFINES ************************************************/
#define nPin_LOW    0
#define nPin_HIGH   1

#define nLED_TEST   LATAbits.LA0
#define nPORT_L15   PORTBbits.RB0
#define nON         1
#define nOFF        0
#define nFT_CTS         LATCbits.LATC1
#define nFT_RTS         LATCbits.LATC0

#define _1msTmr     55535       /*For Preescaler 1:1*/
#define _5msTmr     15536       /*For Preescaler 1:1*/
#define _10msTmr    40535       /*For Preescaler 1:4*/
#define _20msTmr    15535       /*For Preescaler 1:4*/

#define RotTimeOut  10
#define TempTimeOut 20
#define Serial_Time 15

#define SetTime_Update 250
/********************************** Variaveis globais *************************************************/
unsigned int spi_ctr; /* Indice de controle do vetor "valor[]" */

unsigned int pedal_ad = 0;
unsigned int map = 0;
unsigned int ig_dente_14 = 0;
unsigned int tempo_bob = 0;
unsigned int tempo_inj = 0;
unsigned int ref_vb = 0;
unsigned int t_ar_com = 0;
unsigned int t_agua = 0;
unsigned int Sensor_comb = 0;
unsigned int Estequiometria = 0;
long long rotacao_4;
unsigned int rotacao;
unsigned int Rot2;
bit Linha_15;

/* Testes */
unsigned int Lambda = 1;
unsigned int BAT = 12;

typedef enum
{
   Serial_Init = 0,
   Serial_Pedal = Serial_Init,
   Serial_Lambda,
   Serial_MAP,
   Serial_Temp,
   Serial_Rot,
   Serial_VB,
   Serial_Ag,   
   Serial_Inj,
   Serial_Bob,
   Serial_Bat,
   Serial_L15,
   Serial_Comb,
   Serial_Esteq,
   Serial_DataMax
}Serial_Data;
Serial_Data Select_Data;

unsigned int Old_Value[Serial_DataMax] = 0;
unsigned int TempMotor = 0;
unsigned int CountCANRot = 0;
unsigned int CountCANTemp = 0;
unsigned int CountSerial = 0;
unsigned int TimeUpdate = 0;

/*Prototype*/
unsigned int Make16(unsigned char data1, unsigned char data2);

typedef enum
{
  Vet_Init = 0,
  Vet_Rot1 = Vet_Init,
  Vet_Rot2,
  Vet_Pedal,
  Vet_MAP,
  Vet_TmrBob,
  Vet_TmrInj1,
  Vet_TmrInj2,
  Vet_TempAr,
  Vet_TempAg,
  Vet_VB,
  Vet_Comb,
  Vet_Esteq,
  Vet_Total
}Vet_Num;
unsigned int valor[Vet_Total]; /* Guarda os dados recebidos via interrupcao do SPI */

void PutRXRotation( unsigned int Rot );
void PutRXTempMotor( unsigned int Temp );

/***************************************** Interrupts *******************************************/
void interrupt isr(void)//vetor de interrupção
 {
        __Interurpt_SPI1__
        {
        Disable_Interrupt(nSPI1);
        Clear_IntFlag_SPI(nSPI1_Flag);

        valor[spi_ctr] = SLAVE_SPI_READ1();
        spi_ctr++;
        
           if (spi_ctr >= Vet_Total/*SPI_MAX*/) 
           {
              spi_ctr=0;
              rotacao = Make16(valor[Vet_Rot1],valor[Vet_Rot2]); 
              Rot2 = rotacao - 100;
              pedal_ad = valor[Vet_Pedal];
              map = valor[Vet_MAP];
              tempo_bob = valor[Vet_TmrBob];
              tempo_inj = Make16(valor[Vet_TmrInj1],valor[Vet_TmrInj2]);
              t_ar_com = valor[Vet_TempAr];
              t_agua = valor[Vet_TempAg] + 12;
              ref_vb = valor[Vet_VB];
              Sensor_comb = valor[Vet_Comb];
              Estequiometria = valor[Vet_Esteq];
           }
           Enable_IntSPI(nSPI1);
        }
        
        __Interrupt_TMR3__
        {
            ClearFlag_Interrupt(nFLAG_TMR3);
            Set_Timer3(_1msTmr);
            
            if(CountCANRot)  CountCANRot--;
            if(CountCANTemp) CountCANTemp--;
            if(CountSerial)  CountSerial--;
            if(TimeUpdate) TimeUpdate--;
            
            if(!CountSerial)
            {
                CountSerial = Serial_Time;
                Select_Data++;
                Linha_15 = nPORT_L15;
            }            
        }
 }

unsigned int Make16(unsigned char data1, unsigned char data2)
{
    return ((data1 << 8) | data2);
}

unsigned int Putc_TX1_Data(unsigned int data)
{
   unsigned char vet[5];
   
    vet[0] = (data/10000) + 0x30;    
    data = data%10000;   
    vet[1] = (data/1000) + 0x30;    
    data = data%1000;  
    vet[2] = (data/100) + 0x30;    
    data = data%100;
    vet[3] = (data / 10) + 0x30;    
    vet[4] = (data%10) + 0x30;
    
    Putc_TX1(vet[0]);
    Putc_TX1(' ');
    Putc_TX1(vet[1]);
    Putc_TX1(' ');
    Putc_TX1(vet[2]);
    Putc_TX1(' ');
    Putc_TX1(vet[3]);
    Putc_TX1(' ');
    Putc_TX1(vet[4]);
    Putc_TX1(' ');
    Putc_TX1(')');   
}

void No_Analogs( void )
{
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
}

void ConfigureHW( void )
{
    No_Analogs();
    Pin_RX1 = Pin_IN;//configura portB  B1 (pino RX) como entrada
    Pin_TX1 = Pin_OUT;
    TRISB7 = 0;
    TRISA0 = 0;
    PORTB = 0b00000010;  // limpar as portas que estão configuradas como saidas
    TRISC1 = 0;
    TRISC0 = 0;
    TRISC2 = 0;
    
    Init1_RS232(19200,1);
    SPI_SLAVE1();
    SPI_MASTER2();
    Config_Timer1();
    Config_Timer3();
    RESET_CAN();
    __delay_ms(1);
    CONFIG_MCP2515();
    __delay_ms(1);
}

    short Update_CurrentValue(unsigned int Old, unsigned int Current)
    {
        short boReturn = nOFF;
        boReturn = (Current != Old);
        return boReturn;
    }
    
    void Update_OldValue(unsigned int index,unsigned int NewValue)
    {
        Old_Value[index] = NewValue;
    }
 
 void PutRXRotation( unsigned int Rot )
 {
         rotacao_4 = Rot << 2;
              
         CAN_SPI2_WRITE(0x31, 0x50);    //ID = 280h
         CAN_SPI2_WRITE(0x32, 0x00);    //ID = 280h
         CAN_SPI2_WRITE(0x35, 0x08);    //DLC = 8 byte
         
         CAN_SPI2_WRITE(0x36, 0xA9);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x37, 0xA4);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x38, (unsigned char)rotacao_4);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x39, (unsigned char)(rotacao_4 >> 8));    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3A, 0x38);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3B, 0x00);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3C, 0x32);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3D, 0x38);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x30, 0x08);    //Máxima prioridade, força envio da msg
         CAN_SPI2_WRITE(0x30, 0x00);    //Máxima prioridade, força envio da msg
 }
 
  void PutRXTempMotor( unsigned int Temp )
 {
        TempMotor = (unsigned char)((Temp * 4)/3 + 48);
              
         CAN_SPI2_WRITE(0x31, 0x51);    //ID = 288h
         CAN_SPI2_WRITE(0x32, 0x00);    //ID = 288h
         CAN_SPI2_WRITE(0x35, 0x08);    //DLC = 8 byte
         
         CAN_SPI2_WRITE(0x36, 0x5D);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x37, TempMotor);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x38, 0x10);//Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x39, 0x00);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3A, 0xFF);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3B, 0x64);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3C, 0x67);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x3D, 0x9C);    //Envia msg 0x01 pela rede CAN
         CAN_SPI2_WRITE(0x30, 0x08);    //Máxima prioridade, força envio da msg
         CAN_SPI2_WRITE(0x30, 0x00);    //Máxima prioridade, força envio da msg
 }
 
void main(void) 
{
    ConfigureHW();
    __delay_ms(10);
    
    Enable_Interrupt(nINTERRUPT_GLOBAL);
    Enable_Interrupt(nINTERRUPT_PERIPHERAL);    
    Enable_Interrupt(nSPI1);
    Enable_Interrupt(nINT_TIMER3);
    Set_Timer3(_1msTmr);
    
    nFT_CTS = 0;
    nFT_RTS = 0;
    nLED_TEST = nON;
    nSS_MCP2515 = nPin_HIGH;
                
    while(1)
    {  
        if(nPORT_L15)
        {
            if(!CountCANRot)
            {
                PutRXRotation(Rot2);
                CountCANRot = RotTimeOut;
            }
            if(!CountCANTemp)
            {
                PutRXTempMotor(t_agua);
                CountCANTemp = TempTimeOut;
            }
        }
        
        if(!TimeUpdate)
        {
            unsigned char i = 0;
            TimeUpdate = SetTime_Update;
            for(i = 0; i<=13 ;i++)
            {
                Old_Value[i] = Old_Value[i]+1;
            }
        }

        if(Select_Data >= Serial_DataMax)
        {
            Select_Data = Serial_Init;
        }            
                                                
        switch(Select_Data)
        {
            case Serial_Pedal:
                if(Update_CurrentValue(Old_Value[Serial_Pedal],pedal_ad))
                {
                    Putc_Text_TX1("( S P ");       //Pedal
                    Putc_TX1_Data(pedal_ad);
                    Update_OldValue(Serial_Pedal,pedal_ad);
                }
                break;

            case Serial_Lambda:
//                if(Update_CurrentValue(Old_Value[Serial_Lambda],Lambda))
//                {
//                    Putc_Text_TX1("( S L ");        //Lambda
//                    Putc_TX1_Data(Lambda);
//                    Update_OldValue(Serial_Lambda,Lambda);
//                }
                break;

            case Serial_MAP:
                if(Update_CurrentValue(Old_Value[Serial_MAP],map))
                {
                    Putc_Text_TX1("( S M ");        //MAP
                    Putc_TX1_Data(map);
                    Update_OldValue(Serial_MAP,map);
                }
                break;

            case Serial_Temp:
                if(Update_CurrentValue(Old_Value[Serial_Temp],t_ar_com))
                {
                    Putc_Text_TX1("( S T ");        //Temperatura
                    Putc_TX1_Data(t_ar_com);
                    Update_OldValue(Serial_Temp,t_ar_com);
                }
                break;

            case Serial_Rot:
                if(Update_CurrentValue(Old_Value[Serial_Rot],rotacao))
                {
                    Putc_Text_TX1("( S R ");        //Rotação
                    Putc_TX1_Data(rotacao);
                    Update_OldValue(Serial_Rot,rotacao);  
                }
                
                break;

            case Serial_VB:
                if(Update_CurrentValue(Old_Value[Serial_VB],ref_vb))
                {
                    Putc_Text_TX1("( S V ");        //Valvula Borboleta
                    Putc_TX1_Data(ref_vb);
                    Update_OldValue(Serial_VB,ref_vb);
                }
                break;
                
            case Serial_Ag:
                if(Update_CurrentValue(Old_Value[Serial_Ag],t_agua))
                {
                    Putc_Text_TX1("( S m ");        //Temp agua(motor)
                    Putc_TX1_Data(t_agua);
                    Update_OldValue(Serial_Ag,t_agua);
                }
                break;

            case Serial_Inj:
                if(Update_CurrentValue(Old_Value[Serial_Inj],tempo_inj))
                {
                    Putc_Text_TX1("( S I ");        //Injeção
                    Putc_TX1_Data(tempo_inj);
                    Update_OldValue(Serial_Inj,tempo_inj);
                }
                break;

            case Serial_Bob:
                if(Update_CurrentValue(Old_Value[Serial_Bob],tempo_bob))
                {
                    Putc_Text_TX1("( S O ");        //Bobina
                    Putc_TX1_Data(tempo_bob);
                    Update_OldValue(Serial_Bob,tempo_bob);
                }
                break;
                
            case Serial_Bat:
//                if(Update_CurrentValue(Old_Value[Serial_Bat],BAT))
//                {
//                    Putc_Text_TX1("( S B ");        //Bateria
//                    Putc_TX1_Data(BAT);
//                    Update_OldValue(Serial_Bat,BAT);
//                }
                break;

            case Serial_L15:
                if(Linha_15)
                {
                    Putc_Text_TX1("( S l ");       //Linha15 ON
                    Putc_TX1_Data(nON);
                }
                else
                {
                    Putc_Text_TX1("( S l ");       //Linha 15 OFF 
                    Putc_TX1_Data(nOFF);   
                }
                break;
                
            case Serial_Comb:
                if(Update_CurrentValue(Old_Value[Serial_Comb],Sensor_comb))
                {
                    Putc_Text_TX1("( S C ");       //Percentual de etanol
                    Putc_TX1_Data(Sensor_comb);
                }
                break;
                
            case Serial_Esteq:
                if(Update_CurrentValue(Old_Value[Serial_Esteq],Estequiometria))
                {
                    Putc_Text_TX1("( S E ");       //Estequiometria
                    Putc_TX1_Data(Estequiometria);
                }
                break;

            default:
                break;
        }
                                
  }
    return;
}
/*************************************
 *       END SW COMUNICAÇÃO          *
 ************************************/

/* 
 *                                 PIC18F46K22
 *                          +---------:_:---------+
 *              VPP MCLR -> : 01 VPP       PGD 40 : <> RB7 PGD
 *                   RA0 <> : 02           PGC 39 : <> RB6 PGC LCD_RW
 *                   RA1 <> : 03           PGM 38 : <> RB5     LCD_RS
 *                   RA2 <> : 04               37 : <> RB4     LCD_E
 *                   RA3 <> : 05               36 : <> RB3     LCD_D7
 *                   RA4 <> : 06 T0CKI         35 : <> RB2     LCD_D6
 *                   RA5 <> : 07               34 : <> RB1     LCD_D5
 *                   RE0 <> : 08          INT0 33 : <> RB0     LCD_D4
 *                   RE1 <> : 09               32 : <----- VDD
 *                   RE2 <> : 10               31 : <----- VSS
 *              VDD ------> : 11               30 : <> RD7
 *              VSS ------> : 12               29 : <> RD6
 *    10MHZ_CRYSTAL OSC1 -> : 13 OSC1          28 : <> RD5
 *    10MHZ_CRYSTAL OSC2 <- : 14 OSC2          27 : <> RD4
 *                   RC0 <> : 15 T1CKI         26 : <> RC7
 *                   RC1 <> : 16               25 : <> RC6
 *                   RC2 <> : 17               24 : <> RC5
 *                   RC3 <> : 18               23 : <> RC4
 *                   RD0 <> : 19               22 : <> RD3
 *                   RD1 <> : 20               21 : <> RD2
 *                          +---------------------+
 *                                  DIP-40
 */