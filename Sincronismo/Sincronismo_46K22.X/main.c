/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 10 de Julho de 2016, 22:38
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Início declarações
 * 10/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 12 de Julho de 2016, 23:27
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Teste de interrupção por SPI e implementação
 * 12/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 12 de Julho de 2016, 19:55
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Configurado e Finalizado
 * 14/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 17 de Setembro de 2016, 14:19
 * SW SINCRONISMO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Configurado placa MARK1
 * 17/09/2016
 ******************************************************************************************************/

/**************************************** INCLUDE LIBRARY ********************************************/
#include <xc.h>
#include <math.h>
#include "Config_XC8.h"
#include "ADC_K22.h"
#include "EXT_INT.h"
#include "SPI_XC8.h"
#include "TIMERS_XC8.h"

/******************************************* DEFINES ************************************************/
/* Definicao dos pinos dos sinais de entrada e saida */
#define sinal_ent       LATBbits.LATB0         /* Sinal da Roda Fonica */
#define nROTATION_CCP3  PORTBbits.RB5   

#define sinal_inj1      LATDbits.LATD0        /* Injecao 1  DIN0 */
#define sinal_inj2      LATDbits.LATD2        /* Injecao 2  DIN1 */
#define sinal_inj3      LATDbits.LATD3        /* Injecao 3  DIN2 */ //Invertido D2 e D3 na placa
#define sinal_inj4      LATDbits.LATD1        /* Injecao 4  DIN3 */

#define sinal_ig14      LATDbits.LATD4        /* Ignicao (do "fim") dos cilindros 1 e 4 */
#define sinal_ig23      LATDbits.LATD5        /* Ignicao (do "meio") dos cilindros 2 e 3 - calculada */

#define sinal_falha     LATBbits.LATB7        /* Sinal de Falha */      

#define LED_INJ_1       LATDbits.LATD2
#define LED_INJ_2       LATDbits.LATD3
#define LED_INJ_3       LATDbits.LATD0
#define LED_INJ_4       LATDbits.LATD1

#define LED_IGN_1       LATDbits.LATD4
#define LED_IGN_2       LATDbits.LATD5
#define LED_IGN_3       LATDbits.LATD6
#define LED_IGN_4       LATDbits.LATD7

#define OUTEN_33810     LATEbits.LATE0

#define nBOB_IGN1       LATCbits.LATC0
#define nBOB_IGN2       LATCbits.LATC1

#define L15             LATBbits.LATB1
#define PORT_L15        PORTBbits.RB1

#define SPI_MAX         12              /* Numero de bytes recebidos do Ger pelo SPI */ //Original 11 // ANTONIO F: 12
#define PORT_fase       PORTEbits.RE2

#define nVEL_VENT1      LATBbits.LATB3      //Dig Out Ventilador 1
#define nVEL_VENT2      LATBbits.LATB4      //Dig Out Ventilador 2
#define nCOLD_CRANK     LATBbits.LATB2      //Dig Out Partida a frio //Injetor
#define nAIR_COND       LATCbits.LATC2      //Dig Out Ar condicionado

#define nMAXI_33810     LATCbits.LATC6      //Máximo valor de corrente
#define nNOMI_33810     LATCbits.LATC7      //Nominal valor de corrente

#define nBOB_PENCOIL1   LATAbits.LATA0
#define nBOB_PENCOIL2   LATAbits.LATA1
#define nBOB_PENCOIL3   LATAbits.LATA2
#define nBOB_PENCOIL4   LATAbits.LATA3

#define nPIN_TEST       LATBbits.LATB6

#define nBOMB_COLDA_CRANK   LATAbits.LATA4 //Dig Out Partida a frio //Bomba
/*****************************************************************************************************/
#define     nPin_HIGH            1
#define     nPin_LOW             0
#define     nYes                 1
#define     nNo                  0
#define     nON                  1
#define     nOFF                 0

#define     nDigital_Pin         0
#define     nAnalog_Pin          1

#define     nFullGroup_RPM       6  // == 600 RPM

/********************************** Variaveis globais *************************************************/
short int a, b, c, d, e; /* Variaveis de status do programa */
short int dm;
int dente;  /* Incrementado a cada dente */
int ig_pos; // Variavel de controle da ignicao.
            // 0-sem ignicao
            // 1-sinal de ignicao ativo(2ms)
            // 23-inicio da ignicao do "meio" (cil. 2 e 3)
            // 14-inicio da ignicao do "fim" (cil. 1 e 4)

 int ij14_pos, ij23_pos; // Variavel de controle da injecao.
            // 0-sem injecao
            // 1-sinal de injecao cilindro 1
            // 2-sinal de injecao cilindro 2
            // 3-sinal de injecao cilindro 3
            // 4-sinal de injecao cilindro 4
            // 5-sinal de injecao ativo (contador disparado p/ baixar o sinal)

long long t_periodo,t_periodo_ref; /* Variaveis para armazenar os tempos
                                    de patamar em alto */

int volta14, volta23; /* Booleana - Utilizada na injecao */

/* Variaveis que controlam a ignicao no "meio" e no "fim" */
 int ig_tmr_disparo; /* Armazena o tempo de disparo do timer3 a partir
                          do dente de referencia (igual para as duas ignicoes */
int ig_dente_23, ig_dente_23_ant; /* Armazena o dente de referencia para a ignicao 23 */
int ig_dente_14, ig_dente_14_ant; /* Armazena o dente de referencia para a ignicao 14 */

long ij_tmr_disparo; /* Armazena o tempo de disparo para injecao a partir
                        do dente de referencia */

int ij_dente_14, ij_dente_14_ant;
int ij_dente_23, ij_dente_23_ant;
short flag_Vent = 0;

long int tempo_inj; /* recebe do Ger o tempo de injecao*/
int tempo_bob; /* tempo de carregamento da bobina de ign */
int valor[SPI_MAX]; /* Guarda os dados recebidos via interrupcao do SPI */
int spi_ctr = 0; /* Indice de controle do vetor "valor[]" */
int rotacao; // Recebe a rotacao do GER (utilizada na deteccao de fase)
int rotacao_ant;
short fase_ok; // Indica que ja identificou a fase pelo menos uma vez no inicio
short falha_ok;
short sinal_fase;
short cutoff;
int ij_dente_14_extra;
int ij_dente_23_extra;

unsigned int VEL_VENT2_count = 127;

int t_agua = 0;// ANTONIO FERREIRA
unsigned int Percentual_Etanol = 0;       // Percentual de etanol no combustível

unsigned int Set_Vent;
unsigned char Aux_PartidaFrio = 0;

short Flag_Spi = 0;

typedef enum
{
    Vet_Init = 0,
    Vet_IgDnt = Vet_Init,
    Vet_IgTrigger,
    Vet_IjDnt,
    Vet_InjTrigger1,
    Vet_InjTrigger2,
    Vet_TmrBob,  //5
    Vet_TmrInj1,
    Vet_TmrInj2,
    Vet_AddressRPM,
    Vet_InjDntExtra,
    Vet_SetVetVel2,
    Vet_ValAlcool,
    Vet_Total
}Vet_Num;

typedef enum
{
	enReleInit = 0,
	enRele_1 = enReleInit,
	enRele_2,
	enRele_3,
	enRele_4,
	enRele_5,
	enRele_6,
	enRele_7,
	enRele_8,
	enReleMax
}EnumReles;

static union
{
	unsigned int u8All;
	struct
	{
		unsigned Rele1:1;
		unsigned Rele2:1;
		unsigned Rele3:1;
		unsigned Rele4:1;
		unsigned Rele5:1;
		unsigned Rele6:1;
		unsigned Rele7:1;
		unsigned Rele8:1;
	}StatusAllReles;
}UnionAllReles;

/*ex: SetBit(UnionAllReles.u8aAll, enRele_1)*/
#define SetBit(value,Bit)		((value) |= (1 << (Bit)))

/*ex: GetBit(UnionAllReles.u8aAll, enRele_1)*/	
#define GetBit(value,Bit)		((value) >> (Bit) & 0x01)

/*ex: ClearBit(&UnionAllReles.u8aAll, enRele_1)*/
void ClearBit(unsigned char *value, unsigned char Bit)
{
	unsigned char aux = (1 << (Bit));					
	*value = *value & (unsigned char)(~aux);
}

/*Prototype*/
void conta_dente();

unsigned int Make_16(unsigned char data1, unsigned char data2 )
{
    return ((data1 << 8) | data2);
}

/***************************************** Interrupts *******************************************/
void interrupt high_priority isr2(void)
{
   __Interurpt_SPI1__       /* Recebe os avancos e a rotacao do Gerenciamento */
   {   
           Disable_IntSPI(nSPI1);
           Clear_IntFlag_SPI(nSPI1_Flag);
           Clear_IntFlag_SPI(SSP1CON1bits.SSPOV);
           
            valor[spi_ctr] = SSP1BUF;
            spi_ctr++;

          if (spi_ctr < SPI_MAX) 
          {
          Clear_IntFlag_SPI(nSPI1_Flag);
          Enable_IntSPI(nSPI1); 
          Flag_Spi = 1;
          }
   }
    
    __EXT_INTERRUPT0__
   {            
     Disable_ExtInt(nExt_Int0);
     Clear_ExtInt_Flag(nFLAG_ExtInt0);

     //LATCbits.LATC2 = !LATCbits.LATC2;
        
      if (dente != 255) 
      {
        OUTEN_33810 = nPin_LOW; //Habilita MC33810 na placa
        Disable_Interrupt(nINT_TIMER1);

        t_periodo = Get_Timer1();
      }
   
      if ((t_periodo > t_periodo_ref) && (dente > 50)) 
      {

      /*------------------ Nova Volta -------------------------- */   
      sinal_falha = nPin_HIGH; /* Sinal de Falha */
      falha_ok = 1;
      dente = 0; /* Reinicia contagem dos dentes */
      
      // Prepara a interrupcao SPI para a proxima volta 
      spi_ctr = 0;
      Clear_IntFlag_SPI(nSPI1_Flag);
                                                
      if((t_agua <= 14) && (Percentual_Etanol >= 80)) //Temp. do motor < 14º E Teor de álcool >= 80% /* ANTONIO FERREIRA */
      { 
          if(Aux_PartidaFrio < 20)            //Tempo que o sistema permanece ligado //Tempo a ser "ajustado"
          {
              nBOMB_COLDA_CRANK = nPin_HIGH; // Tempe. do motor menor que 14ºC e Teor de álcool maior que 80% Aciona eletrobomba partida a frio
              nCOLD_CRANK = nPin_HIGH;       // Tempe. do motor menor que 14ºC e Teor de álcool maior que 80% Aciona válvula solenoide partida a frio
              Aux_PartidaFrio++;
          }
      }
      else
      {
          nBOMB_COLDA_CRANK = nPin_LOW; // Tempe. do motor maior ou igual a 14ºC ou Teor de álcool menor que 80% Desliga eletrobomba partida a frio
          nCOLD_CRANK = nPin_LOW;       // Tempe. do motor maior ou igual a 14ºC ou Teor de álcool menor que 80% Desliga válvula solenoide partida a frio
      }
      
          
      Enable_IntSPI(nSPI1);
       
      a = b = c = d = e = 0;            
      /*------------------ Fim Nova Volta ----------------------- */
      }
   
      t_periodo = t_periodo << 1;         
      t_periodo_ref = t_periodo;
   
   //if(input(L15))
   conta_dente();

   Set_Timer1(0); // Inicia medicao do t_low
   ClearFlag_Interrupt(nFLAG_TMR1);
   Enable_Interrupt(nINT_TIMER1);
   
   if (dente == 2)      sinal_falha = nPin_LOW; /* Baixa o Pulso de Falha */

   e = 1;
   Clear_ExtInt_Flag(nFLAG_ExtInt0);
   Enable_Interrupt(nExt_Int0);
   }
    
   __Interrupt_TMR2__
   {
       Disable_Interrupt(nINT_TIMER2);
       ClearFlag_Interrupt(nFLAG_TMR2);       /*ALTERADO DIA 30/10*/
              
       //LATCbits.LATC2 = !LATCbits.LATC2;
       
    if (dm==0) //Isto para dobrar o funcionamento do timer senão falha ignicao
    {
   // Pulsos de ignicao 
   switch(ig_pos) 
    {

      case 0:  break;

      case 1:
          //sinal_ig23 = nPin_LOW; // Baixa o Sinal da Ignicao Ativa
          LED_IGN_2 = nPin_LOW;
          nBOB_IGN2 = nPin_LOW;
          //sinal_ig14 = nPin_LOW;
          LED_IGN_1 = nPin_LOW;
          nBOB_IGN1 = nPin_LOW;
          ig_pos = 0;
          dm=0;
          break;

      case 23: 
          if (tempo_bob > 1) 
            {
                //sinal_ig23 = nPin_HIGH; // Ativa Sinal da ignicao 23
                //LED_IGN_2 = nPin_HIGH;
                nBOB_IGN2 = nPin_HIGH;
   
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = tempo_bob;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                ig_pos = 1;
                dm=1;
            }
                else              ig_pos = 0; 
                break;
      
      case 14: 
          if (tempo_bob > 1) 
             { 
                  //sinal_ig14 = nPin_HIGH; // Ativa Sinal da ignicao 14
                  //LED_IGN_1 = nPin_HIGH; 
                  nBOB_IGN1 = nPin_HIGH;
   
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = tempo_bob;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                ig_pos = 1;
                dm=1;
             }
                else             ig_pos = 0; 
                break;
     }
      c = 1;
    }
       
    else
     {
      Set_Timer2(0);
      dm=0;
      ig_pos = 1;
      ClearFlag_Interrupt(nFLAG_TMR2);
      Enable_Interrupt(nINT_TIMER2);
     }       
   }
      
/***************************************************** - TIMER 3 - *****************************************************************/   
   __Interrupt_TMR3__
   { 
        Disable_Interrupt(nINT_TIMER3);                                                         // Desabilita a interrupção do Timer 3
        ClearFlag_Interrupt(nFLAG_TMR3);                                                        // Limpa a Flag do Timer 3
              
        /* Pulsos para injeção sequencial e Full Group ==> Injetor 2 e 3*/
        switch(ij23_pos) 
        {

            case 0:
            break;

            /***************************************Ativa Sinal do injetor 2*******************************************************/
            case 2:// Ativa Sinal do injetor 2
                   if (tempo_inj > 20) 
                   {                

                     if(rotacao < nFullGroup_RPM) /*Lógica Full Group*/
                     {
                         LED_INJ_1 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 1    */
                         LED_INJ_2 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 2    */
                         LED_INJ_3 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 3    */
                         LED_INJ_4 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 4    */
                     }
                     else /*Lógica Sequencial*/
                     {
                         LED_INJ_2 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 2    */
                         LED_INJ_1 = nPin_LOW;                                                  /* Desativa Sinal do injetor 1 */
                         LED_INJ_3 = nPin_LOW;                                                  /* Desativa Sinal do injetor 3 */
                         LED_INJ_4 = nPin_LOW;                                                  /* Desativa Sinal do injetor 4 */
                     }

                     /* Conta e entao baixa o sinal de injecao por interrupcao */
                     Set_Timer3(tempo_inj);                                                     // Carrega Tempo de injeção
                     ClearFlag_Interrupt(nFLAG_TMR3);                                           // Limpa a Flag do Timer 3
                     Enable_Interrupt(nINT_TIMER3);                                             // Habilita a interrupção do Timer 3

                     ij23_pos = 20;                                                             // Próxima interrupção vai Desativar o sinal
                    }
                    else  ij23_pos = 0;         
                    break;
            /**********************************************************************************************************************/

            /***************************************Ativa Sinal do injetor 3*******************************************************/
            case 3:// Ativa Sinal da injecao 3 
                   if (tempo_inj > 20) 
                   {

                    if(rotacao < nFullGroup_RPM) /*Lógica Full Group*/
                    {
                        LED_INJ_1 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 1    */
                        LED_INJ_2 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 2    */
                        LED_INJ_3 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 3    */
                        LED_INJ_4 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 4    */
                    }
                    else /*Lógica Sequencial*/
                    {
                          LED_INJ_3 = nPin_HIGH;                                               /* Ativa Sinal do injetor 3    */
                          LED_INJ_1 = nPin_LOW;                                                /* Desativa Sinal do injetor 1 */
                          LED_INJ_2 = nPin_LOW;                                                /* Desativa Sinal do injetor 2 */
                          LED_INJ_4 = nPin_LOW;                                                /* Desativa Sinal do injetor 4 */
                    }

                      /* Conta e entao baixa o sinal de injecao por interrupcao */
                    Set_Timer3(tempo_inj);                                                     // Carrega Tempo de injeção
                    ClearFlag_Interrupt(nFLAG_TMR3);                                           // Limpa a Flag do Timer 3
                    Enable_Interrupt(nINT_TIMER3);                                             // Habilita a interrupção do Timer 3

                    ij23_pos = 30;                                                             // Próxima interrupção vai Desativar o sinal
                  }
                  else  ij23_pos = 0;
                  break;
            /**********************************************************************************************************************/

            /*****************************************Desativa o sinal*************************************************************/
            case 20:// Desativa Sinal da injecao 1/2/3/4 
                    LED_INJ_1 = nPin_LOW;                                            /* Desativa Sinal do injetor 1 */
                    LED_INJ_2 = nPin_LOW;                                            /* Desativa Sinal do injetor 2 */
                    LED_INJ_3 = nPin_LOW;                                            /* Desativa Sinal do injetor 3 */
                    LED_INJ_4 = nPin_LOW;                                            /* Desativa Sinal do injetor 4 */

                    if ( (rotacao >= 6) && (rotacao_ant >= 6) )
                        volta23 = !volta23;

                    ij23_pos = 0;
                    break;
            /**********************************************************************************************************************/

            /*****************************************Desativa o sinal*************************************************************/
            case 30:// Desativa Sinal da injecao 1/2/3/4
                    LED_INJ_1 = nPin_LOW;                                            /* Desativa Sinal do injetor 1 */
                    LED_INJ_2 = nPin_LOW;                                            /* Desativa Sinal do injetor 2 */
                    LED_INJ_3 = nPin_LOW;                                            /* Desativa Sinal do injetor 3 */
                    LED_INJ_4 = nPin_LOW;                                            /* Desativa Sinal do injetor 4 */

                    if ( (rotacao >= 6) && (rotacao_ant >= 6) ) 
                    {
                        volta23 = !volta23;

                        // Verifica a logica da fase...
                        // Neste caso o sinal de injecao 3 SEMPRE desce DEPOIS da falha
                        if (sinal_fase == 0) // a fase estava errada!!!
                        volta23 = !volta23;
                    }

                    ij23_pos = 0;
                    break;
            /**********************************************************************************************************************/

        }
        d = 1;
   }
   /**************************************************FIM TIMER 3*****************************************************************/
   
/***************************************************** - TIMER 0 - ***************************************************************/
    __Interrupt_TMR0__
    {
        Disable_Interrupt(nINT_TIMER0);                                                         // Desabilita a interrupção do Timer 0
        ClearFlag_Interrupt(nFLAG_TMR0);                                                        // Limpa a Flag do Timer 0

       /* Pulsos para injecao sequencial e Full Group ==> Injetor 1 e 4*/
        switch(ij14_pos) 
        {

            case 0:
            break;

            /***************************************Ativa Sinal do injetor 1*******************************************************/        
            case 1:// Ativa Sinal da injecao 1 
                   if (tempo_inj > 20) 
                   {
                     if(rotacao < nFullGroup_RPM) /*Lógica Full Group*/
                     {
                         LED_INJ_1 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 1    */
                         LED_INJ_2 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 2    */
                         LED_INJ_3 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 3    */
                         LED_INJ_4 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 4    */
                     }
                     else /*Lógica Sequencial*/
                     {
                         LED_INJ_1 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 1    */
                         LED_INJ_2 = nPin_LOW;                                                  /* Desativa Sinal do injetor 2 */
                         LED_INJ_3 = nPin_LOW;                                                  /* Desativa Sinal do injetor 3 */
                         LED_INJ_4 = nPin_LOW;                                                  /* Desativa Sinal do injetor 4 */
                     }

                     /* Conta e entao baixa o sinal de injecao por interrupcao */
                     Set_Timer0(tempo_inj);                                                     // Carrega Tempo de injeção
                     ClearFlag_Interrupt(nFLAG_TMR0);                                           // Limpa a Flag do Timer 0
                     Enable_Interrupt(nINT_TIMER0);                                             // Habilita a interrupção do Timer 0

                       ij14_pos = 10;                                                           // Próxima interrupção vai Desativar o sinal
                   }
                   else  ij14_pos = 0;
                   break;
            /**********************************************************************************************************************/

            /***************************************Ativa Sinal do injetor 4*******************************************************/
            case 4:// Ativa Sinal da injecao 4 
                   if (tempo_inj > 20)
                   {
                     if(rotacao < nFullGroup_RPM) /*Lógica Full Group*/
                     {
                         LED_INJ_1 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 1    */
                         LED_INJ_2 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 2    */
                         LED_INJ_3 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 3    */
                         LED_INJ_4 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 4    */
                     }
                     else /*Lógica Sequencial*/
                     {
                         LED_INJ_4 = nPin_HIGH;                                                 /* Ativa Sinal do injetor 4    */
                         LED_INJ_1 = nPin_LOW;                                                  /* Desativa Sinal do injetor 1 */
                         LED_INJ_2 = nPin_LOW;                                                  /* Desativa Sinal do injetor 2 */
                         LED_INJ_3 = nPin_LOW;                                                  /* Desativa Sinal do injetor 3 */
                     }

                     /* Conta e entao baixa o sinal de injecao por interrupcao */
                     Set_Timer0(tempo_inj);                                                     // Carrega Tempo de injeção
                     ClearFlag_Interrupt(nFLAG_TMR0);                                           // Limpa a Flag do Timer 0
                     Enable_Interrupt(nINT_TIMER0);                                             // Habilita a interrupção do Timer 0

                       ij14_pos = 40;                                                           // Próxima interrupção vai Desativar o sinal
                   }
                   else  ij14_pos = 0;
                   break;
            /**********************************************************************************************************************/

            /*****************************************Desativa o sinal*************************************************************/
            case 10:
                    LED_INJ_1 = nPin_LOW;                                            /* Desativa Sinal do injetor 1 */
                    LED_INJ_2 = nPin_LOW;                                            /* Desativa Sinal do injetor 2 */
                    LED_INJ_3 = nPin_LOW;                                            /* Desativa Sinal do injetor 3 */
                    LED_INJ_4 = nPin_LOW;                                            /* Desativa Sinal do injetor 4 */

                    if ( (rotacao >= 6) && (rotacao_ant >= 6) )
                    {
                        volta14 = !volta14;

                        // Verifica a logica da fase...
                        // Neste caso o sinal de injecao 1 SEMPRE desce DEPOIS da falha
                        if (sinal_fase == 0) // a fase estava errada!!!
                            volta14 = !volta14;
                    }

                    ij14_pos = 0;
                    break;
            /**********************************************************************************************************************/

            /*****************************************Desativa o sinal*************************************************************/        
            case 40:
                   LED_INJ_1 = nPin_LOW;                                             /* Desativa Sinal do injetor 1 */
                   LED_INJ_2 = nPin_LOW;                                             /* Desativa Sinal do injetor 2 */
                   LED_INJ_3 = nPin_LOW;                                             /* Desativa Sinal do injetor 3 */
                   LED_INJ_4 = nPin_LOW;                                             /* Desativa Sinal do injetor 4 */

                    if ( (rotacao >= 6) && (rotacao_ant >= 6) )
                        volta14 = !volta14;

                    ij14_pos = 0;
                    break;
            /**********************************************************************************************************************/

        }
        b = 1;
    }
    /**************************************************FIM TIMER 0*****************************************************************/
}
/*********************************************FIM interrupt High_priority isr2*****************************************************/

void interrupt low_priority isr( void )
{
    /***************************************************** - TIMER 1 - ************************************************************/
    __Interrupt_TMR1__
    {
         Disable_Interrupt(nINT_TIMER1);
         ClearFlag_Interrupt(nFLAG_TMR1);                 /*ALTERADO DIA 30/10*/

         rotacao = 0;
         fase_ok = 0;
         falha_ok = 0;
         t_periodo_ref = 65535; // Reseta deteccao da falha
         t_periodo = 65530;
         dente = 255;
         
         /****************************** Controle do Ventilador *************************/
         if(flag_Vent)
         {
            if(!PORT_L15)
            {
                VEL_VENT2_count = 127;
            }
            else
            {
                VEL_VENT2_count = Set_Vent*4;
            }
            flag_Vent = 0;
         }
         else
         {
            if(!VEL_VENT2_count)
            {
                nVEL_VENT2 = nPin_LOW;
            }
            else
            {
                VEL_VENT2_count--;
            }
         }
         /****************************** Fim do Controle do Ventilador ******************/
         
         Set_Timer1(0);

         Enable_Interrupt(nINT_TIMER1);     
    }
    /**************************************************FIM TIMER 1*****************************************************************/
}
/*********************************************FIM interrupt low_priority isr*******************************************************/

/* Funcao conta_dente */
void conta_dente(void) 
{    
    dente++; // Contagem das dentes
    
    // Se nao ident. a falha uma vez nao deixa prosseguir...
    if (falha_ok == 0)      return;
    
        if (dente == 3)
        {
             // Armazena o sinal de fase
             if (PORT_fase)              sinal_fase = 1;
             else                        sinal_fase = 0;
        }
    if (dente == 30) 
    {        
        // Guarda os adiantamentos da volta anterior antes de atualizar....
        ig_dente_14_ant = ig_dente_14;
        ig_dente_23_ant = ig_dente_23;
        ij_dente_14_ant = ij_dente_14;
        ij_dente_23_ant = ij_dente_23;
        rotacao_ant = rotacao;  
        
        // Atualiza adiantamentos e tempos
        ig_dente_14 = 25; //teste
        
        ig_dente_14 = valor[Vet_IgDnt];
        if ((ig_dente_14 < 1) || (ig_dente_14 > 58)) 
        { // Verificacao de seguranca...
            ig_dente_14 = 255;
            ig_dente_23 = 255;
        }
        else 
        {
            ig_dente_23 = ig_dente_14 + 30;
            if (ig_dente_23 > 60) ig_dente_23 -= 60;
        }
        
        //ij_dente_14 = (unsigned char)(valor[0] >> 8); // O dente de inj foi enviado no byte mais sig. do SPI (SPIxDRH)
        ij_dente_14 = valor[Vet_IjDnt]; // O dente de inj foi enviado no byte mais sig. do SPI (SPIxDRH)
        if ((ij_dente_14 < 1) || (ij_dente_14 > 58)) 
        { // Verificacao de seguranca...
            if (ij_dente_14 == 200) // indica corte de injecao...
                cutoff = 1;
            else                cutoff = 0;

            ij_dente_14 = 255;
            ij_dente_23 = 255;
            //nPIN_TEST = 1;
        }
        else 
        {
            ij_dente_23 = ij_dente_14 + 30;
            if (ij_dente_23 > 60) ij_dente_23 -= 60;

            cutoff = 0;
            //nPIN_TEST = 0;
        }
        
        if(Flag_Spi)
        {
            ig_tmr_disparo = valor[Vet_IgTrigger];
            ij_tmr_disparo = 65535 - Make_16(valor[Vet_InjTrigger1],valor[Vet_InjTrigger2]);     
            tempo_bob = valor[Vet_TmrBob];
            tempo_inj = 65535 - Make_16(valor[Vet_TmrInj1],valor[Vet_TmrInj2]);  
            //tempo_inj = 10000;
            rotacao = valor[Vet_AddressRPM];
            ij_dente_14_extra = valor[Vet_InjDntExtra];
            ij_dente_23_extra = ij_dente_14_extra + 30;
            Set_Vent = valor[Vet_SetVetVel2];
            Percentual_Etanol = valor[Vet_ValAlcool];
            Flag_Spi = 0;
        }
        
        t_agua = Set_Vent;      //ANTONIO FERREIRA
        
        if(Set_Vent >= 78)    
        {
            nVEL_VENT2 = nPin_HIGH;
            flag_Vent = 1;
        }
        else if (Set_Vent <= 72) nVEL_VENT2 = nPin_LOW;
        
        // Logica de Deteccao de Fase
        if (rotacao < 6 && rotacao_ant < 6) 
        {
            if(sinal_fase == 1) 
            {
                volta14 = 255; // Injeta no 4
                //volta23 = 255; // Injeta no 2 - Invertia abaixo de 600 >corrigido                      
                volta23 = 0; // Injeta no 2                       
            }   
            else 
            {
                volta14 = 0; // Injeta no 1
                //volta23 = 0; // Injeta no 3 - Invertia abaixo de 600 >corrigido 
                volta23 = 255; // Injeta no 3
            }   
            fase_ok = 1; // Ja detectou a fase pelo meno uma vez (requisito p/ a primeira inj.)
        }
        
        // Correcao para evitar perda de ignicao na transicao do dente 20 para o 19...
        //if (ig_dente_14 < 20 && ig_dente_14_ant >= 20 && (ig_dente_14_ant - ig_dente_14) < 30) 
        if ( (ig_dente_14 < 30) && (ig_dente_14_ant >= 30) && ( (ig_dente_14_ant - ig_dente_14) < 30 )) 
        {
            // Forca ignicao 14 neste dente...
            if (ig_tmr_disparo > 1 && ig_pos == 0) 
            {   
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = ig_tmr_disparo;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                //LATCbits.LATC2 = !LATCbits.LATC2;
      
                ig_pos = 14;
            }
        }
        
        //if (ig_dente_23 < 20 && ig_dente_23_ant >= 20 && (ig_dente_23_ant - ig_dente_23) < 30) 
        if ((ig_dente_23 < 30) && (ig_dente_23_ant >= 30) && ((ig_dente_23_ant - ig_dente_23) < 30)) 
        {
            // Forca ignicao 23 neste dente...
            if (ig_tmr_disparo > 1 && ig_pos == 0) {   
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = ig_tmr_disparo;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);                
                ig_pos = 23;
            }
        }
        
        // Correcao para evitar perda de injecao na transicao do dente 20 para o 19...
        if ((ij_dente_14 < 30) && (ij_dente_14_ant >= 30) && ((ij_dente_14_ant - ij_dente_14) < 30)) 
        {               
                Set_Timer0(ij_tmr_disparo);
                ClearFlag_Interrupt(nFLAG_TMR0);
                Enable_Interrupt(nINT_TIMER0);
                if (!volta14)                  ij14_pos = 1;
                else                           ij14_pos = 4;   
        }
        
           if ((ij_dente_23 < 30) && (ij_dente_23_ant >= 30) && ((ij_dente_23_ant - ij_dente_23) < 30)) 
           {
                Set_Timer3(ij_tmr_disparo);
                ClearFlag_Interrupt(nFLAG_TMR3);
                Enable_Interrupt(nINT_TIMER3);
                if (!volta23)                  ij23_pos = 3;
                else                           ij23_pos = 2;
            }
    }
    
        if ((dente == ig_dente_14) && (ig_pos == 0)) 
        {
        if (ig_tmr_disparo >= 1) 
            {
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = ig_tmr_disparo;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                
                //LATCbits.LATC2 = !LATCbits.LATC2;
                ig_pos = 14;
            }
        }
    
        if ((dente == ig_dente_23) && (ig_pos == 0)) 
        {
            if (ig_tmr_disparo >= 1) 
             {
                //Config_Timer2();                    //setup_timer_2(T2_DIV_BY_16,ig_tmr_disparo,16);
                PR2 = ig_tmr_disparo;
                Set_Timer2(0);
                ClearFlag_Interrupt(nFLAG_TMR2);
                Enable_Interrupt(nINT_TIMER2);
                ig_pos = 23;
             }
        }
    
        // Se nao ident. a fase uma vez nao deixa dar os sinais de inj
    if (rotacao > 65 || fase_ok == 0)        return;    // O GER corta a inj, mas esta condicao reforca o corte (seguranca)
    if(PORT_L15 == 0)
    {
        return;
    }
    
    //estava comentado
//    
//    if ((dente == ij_dente_14_extra) && (ij23_pos==20)) 
//    {
//         sinal_inj1 = nPin_HIGH;         
//    }
//
//    if ((dente == ij_dente_14_extra) && (ij23_pos==30)) 
//    {
//         sinal_inj4 = nPin_HIGH;
//    }
//    
//    if ((dente == ij_dente_23_extra) && (ij14_pos==10)) 
//    {
//         sinal_inj3 = nPin_HIGH;      
//    }
//    
//    if ((dente == ij_dente_23_extra) && (ij14_pos==40)) 
//    {             
//         sinal_inj2 = nPin_HIGH;
//    }
//    
    //ate aqui
    
        if ((dente == ij_dente_14) && (ij14_pos == 0)) 
        {
            Set_Timer0(ij_tmr_disparo);
            ClearFlag_Interrupt(nFLAG_TMR0);
            Enable_Interrupt(nINT_TIMER0);
            if (!volta14)                ij14_pos = 1;
            else                         ij14_pos = 4;
        }
    
        if ((dente == ij_dente_23) && (ij23_pos == 0)) 
        {
            Set_Timer3(ij_tmr_disparo);
            ClearFlag_Interrupt(nFLAG_TMR3);
            Enable_Interrupt(nINT_TIMER3);
            if (!volta23)                ij23_pos = 3;
            else                         ij23_pos = 2;
        }
}

void No_Analogs( void )
{
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
}

void Configure_HW( void )
{
   /* Configuração das Portas */
   TRISA = 0b00100000;  //TRISA = 0b01100000;
   TRISB = 0b00100011;
   TRISC = 0b00011000;  //TRISC = 0b11011000;
   TRISD = 0b00000000;  //TRISD = 0b00000000;
   TRISE = 0b00000100;  //TRISE = 0b00000110;
   __delay_us(50);
   //__delay_ms(10);
   //__delay_ms(10);
   
   No_Analogs(); 
    
   SPI_SLAVE1();
   
   Confi_Int_Ext0();
   
   Config_Timer0();
   Config_Timer1();
   Config_Timer2();
   PR2 = 255;
   Config_Timer3();
}

void Delay_ms(unsigned int value)
{
    unsigned int i;
    for(i = 0; i < value; i++)
    {
        __delay_ms(1);
    }
}

void main(void) 
{
    Configure_HW();
   
    reset:
             /* Inicializacao das variaveis */
    t_periodo = 65530;
    t_periodo_ref = 65535; /* Garante que o pulso de detecção só acontecerá
                             após a comparação do segundo pulso de sinal */
    dente = 255;
    ig_pos = 0;
    ij23_pos = ij14_pos = 0;
    ig_tmr_disparo = 0;
    ij_tmr_disparo = 0;
    ig_dente_23 = 200;
    ig_dente_14 = 200;    
    ij_dente_14 = 200;
    ij_dente_23 = 200;
    ig_dente_23_ant = 200;
    ig_dente_14_ant = 200;    
    ij_dente_14_ant = 200;
    ij_dente_23_ant = 200;
    spi_ctr = 0;
    rotacao = 0;
    rotacao_ant = 0;
    
   volta14 = volta23 = 0;
   falha_ok = 0;
   fase_ok = 0;
   sinal_fase = 0;
   cutoff = 0;
          
   OUTEN_33810 = nPin_HIGH; //Desabilita 33810 no inicio
   
   nVEL_VENT1 = nOFF;
   nVEL_VENT2 = nOFF;
   nCOLD_CRANK = nOFF;     //Dig Out Partida a frio
   nAIR_COND = nOFF;
   nBOMB_COLDA_CRANK = nOFF;
   nVEL_VENT2 = nPin_LOW;
   
   // Desliga Ignicoes e injecoes por seguranca
   LED_IGN_1 = nPin_LOW;
   LED_IGN_2 = nPin_LOW;
   LED_IGN_3 = nPin_LOW;
   LED_IGN_4 = nPin_LOW;
   
   nBOB_IGN1 = nPin_LOW;
   nBOB_IGN2 = nPin_LOW;
   
   RCONbits.IPEN = nON;
   TMR0IP = nON;
   TMR1IP = nOFF;
   TMR3IP = nON;
   TMR2IP = nON;

   LED_INJ_1 = nPin_LOW;  
   LED_INJ_2 = nPin_LOW;
   LED_INJ_3 = nPin_LOW;
   LED_INJ_4 = nPin_LOW;
   
   //Enable_Interrupt(nINTERRUPT_GLOBAL);
   //Enable_Interrupt(nINTERRUPT_PERIPHERAL);
   //Enable_ExtInt(nExt_Int0);
         
   dm=0;
   STKFUL = 0;
   
   while(1)
   {    
                               
   }
   
  
   goto reset;
    
    return;
}
/*************************************
 *       END SW SINCRONISMO          *
 ************************************/

/* 
 *                                 PIC18F4550
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