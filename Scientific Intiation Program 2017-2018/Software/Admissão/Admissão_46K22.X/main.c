/*******************************************************************************************************
 * File:   main.c
 * Author: RODRIGO PERALTA
 *
 * Created on 30 de MARÇO de 2017,
 * SW ADMISSÃO VERSÃO 2.0
 Controle de subida e descida implementado na válvula borboleta do POLO de Sorocaba com leitura de ângulo ao invés de valor ad
 * Leitura do valor AD da borboleta com 10 bitis justificado à direita

 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 14 de Julho de 2016, 19:25
 * SW ADMISSÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Teste ADC 8 Bit´s e continuação
 * 14/07/2016
 ******************************************************************************************************/
/*******************************************************************************************************
 * File:   main.c
 * Author: Hiroki
 *
 * Created on 14 de Outubro de 2016, 15:12
 * SW ADMISSÃO VERSÃO 1.0
 * Autor: Gustavo Hiroki
 * Modificação: Teste em Bancada pedal e Válvula borboleta
 * 14/10/2016
 ******************************************************************************************************/

/**************************************** INCLUDE LIBRARY ********************************************/
#include <xc.h>
#include <math.h>

#include "PWM_K22.h"
#include "Config_XC8.h"
#include "ADC_K22.h"
#include "EXT_INT.h"
#include "SPI_XC8.h"
#include "TIMERS_XC8.h"

/******************************************* DEFINES ************************************************/
#define code        LATBbits.LATB2
#define DI1         LATBbits.LATB1
#define DI2         LATAbits.LATA3
#define SF          LATBbits.LATB4
#define PORT_L15    PORTBbits.RB0
#define nLED_TEST   LATCbits.LATC0
#define MAX_ANGLE 90.0f  // máximo ângulo da válvula
#define MIN_ANGLE 0.0f   // mínimo ângulo da válvula
#define ANGLE_LimpHome   5.5f   // valor do angulo com a borboleta completamente relaxada
#define MIN_ADC_ANGLE 117 //valor polo tps 1 98 //valor gol 102 //  valor valv bancada 106
#define MAX_ADC_ANGLE 922 //valor polo 954 //  valor gol 942 // 912
#define SPI_MAX     1 /* Numero de bytes recebidos do Ger pelo SPI */
/*****************************************************************************************************/
#define     nPin_HIGH            1
#define     nPin_LOW             0
#define     nYes                 1
#define     nNo                  0
#define     nON                  1
#define     nOFF                 0

#define     nDigital_Pin         0
#define     nAnalog_Pin          1

#define     VB_Adjust             5.0

/********************************** Variaveis globais *************************************************/
/* Variaveis utilizadas no algoritmo PI - Valvula Borboleta */
unsigned int ref_vb, value, media = 0; // Sinal de referencia (pedal convertido p/ escala da vb
float soma_erro_vb;
float erro_vb;
float Kp2 = 3.5, Kp = 0.605; // ganho proporcional do controlador de descida e do controlador de subida (nesta ordem)
float u1_ant = 0, e1_ant = 0, u2_ant, e2_ant, u_i = 0, ui_ant = 0, up_ant = 0, u_p = 0, u_i2 = 0, ui_ant2 = 0, u_p2 = 0, constante_integrador = 0.01807, constante_integrador2 = 0.18;

const float Kp_vb = 8.0; // Anterior: 12.0
const float Ki_vb = 0.02; // Anterior: 0.07
//int       Kp_vb = 12;
//int       Ki_vb = 14;

int pedal_ad;
unsigned int pwm_10bits = 0;
unsigned int tps_ad, tps_ad2, vetor_leitura[10] = {0};
unsigned int counter2 = 0;
unsigned int pwm = 0;
char inv;
float SetPoint = 6.0, Angle = 6.0;
signed int Dif_vb = 0;
unsigned char ref_vbReq = 0, EnableControl, counter, i = 0;
short flag_vb = 1;
signed float Erro1 = 0, Erro2 = 0;
float Out1, Out2;

/**************************** Variaveis de leitura SPI *********************************/
int valor[SPI_MAX]; /* Guarda os dados recebidos via interrupcao do SPI */
int spi_ctr = 0; /* Indice de controle do vetor "valor[]" */

unsigned int ValorTotal = 0;

/* Variaveis utilizadas no algoritmo PI - Injecao */
//const float Kp_inj = 0.0001;

//void controle_vb();
void Control_up(void);
void Control_down(void);
//void controle_rpm();

/***************************************** Interrupts *******************************************/
void interrupt isr(void)
{
    __Interrupt_TMR0__{
        ClearFlag_Interrupt(nFLAG_TMR0);
        Set_Timer0(64286); //1ms - 64286

        if (counter == 10)
        {

            EnableControl = 1;
            counter = 0;

            // counter2++;
            /*
              if(counter2==1000)
             {
                 SetPoint=8.0;
                 counter2=0;
                 Disable_Interrupt(nSPI1);

             }

             */

        }
        else
        {
            counter++;
        }
        LATDbits.LATD0 = nPin_HIGH;
        //Pedal_Read();
        //tps_read();
        //controle_vb();
        LATDbits.LATD0 = nPin_LOW;
        //controle_rpm();
        //ClearFlag_Interrupt(nFLAG_TMR0);
    }

    __Interurpt_SPI1__{
        //    Disable_Interrupt(nSPI1);
        //
        //        valor[spi_ctr] = SSP1BUF;
        //
        //        nLED_TEST = !nLED_TEST;
        //        spi_ctr++;
        //
        //           if (spi_ctr < SPI_MAX)
        //           {
        //                Clear_IntFlag_SPI(nSPI1_Flag);
        //                Enable_IntSPI(nSPI1);
        //            }
        //          else
        //          {
        //                spi_ctr = 0;
        //                ref_vb = 255 - valor[0];
        //          }
        //   Clear_IntFlag_SPI(nSPI1_Flag);
        //   Enable_IntSPI(nSPI1);

        Disable_Interrupt(nSPI1);
        Clear_IntFlag_SPI(nSPI1_Flag);

        valor[spi_ctr] = SSP1BUF;
        ValorTotal = ValorTotal + valor[spi_ctr];
        spi_ctr++;

        nLED_TEST = !nLED_TEST;

        if (spi_ctr < SPI_MAX)
        {
            Clear_IntFlag_SPI(nSPI1_Flag);
            Enable_IntSPI(nSPI1);
        }
        else
        {
            spi_ctr = 0;
            //ref_vb = 255 - (unsigned char)(ValorTotal/SPI_MAX);

            ref_vb = ValorTotal; //255 - (unsigned char) (ValorTotal / SPI_MAX); // tira a média dos valores recebidos da SPI
            ValorTotal /= SPI_MAX; // VALORTOTAL/SPI_MAX
            ValorTotal <<= 2; //  multiplica por 4 para converter para 10 bits
            SetPoint = ((MAX_ANGLE - MIN_ANGLE)*(ValorTotal - MIN_ADC_ANGLE) / (MAX_ADC_ANGLE - MIN_ADC_ANGLE)) + MIN_ANGLE;

            //Dif_vb = ref_vb - ref_vbReq;    // pega a diferença para abir a válvula mais devagar
            //                if(flag_vb)
            //                {
            //                    //ref_vbReq = 255 - (unsigned char)(ValorTotal/SPI_MAX);
            //                    //Dif_vb = ref_vb - ref_vbReq;
            //                    flag_vb = 0;
            //                }

            ValorTotal = 0;

            //if(valor[0] == 40) nLED_TEST = 0;
            //else nLED_TEST = 1;
        }


        //Clear_IntFlag_SPI(nSPI1_Flag);
        Enable_IntSPI(nSPI1);
    }
}

void Pedal_Read()
{
    //set_adc_channel(1);
    __delay_us(90);
    pedal_ad = Read_ADC(AN1);
}

void tps_read()
{
    //set_adc_channel(0);
    __delay_us(90);
    tps_ad = Read_ADC(AN1);
}

//Rotina de borboleta Bruno

//Rotina de borboleta Bruno

void controle_vb()
{
    float _control; /* Sinal de controle */
    unsigned int pwm2; // pwm /* Valor de PWM a ser gerado */
    // unsigned int pwm_10bits;

    //ref_vb = ref_vb * 4;
    //if (ref_vb > 223)                             ref_vb = 223;        // Impede fechamento total da vb

    if (ref_vbReq > 223) ref_vbReq = 223; // Impede fechamento total da vb

    //Pedal_Read();
    //tps_read();
    tps_ad2 = value & 0b1111111100; //0x3FC ;
    tps_ad2 = tps_ad2 >> 2;
    //   if(Dif_vb > 0)
    //   {
    //       if(ref_vb >= ref_vbReq)    ref_vb--;
    //       else
    //       {
    //           //ref_vb = ref_vbReq;
    //           flag_vb = 1;
    //           //nLED_TEST = !nLED_TEST;
    //       }
    //       //nLED_TEST = !nLED_TEST;
    //   }
    //   else   ref_vb = ref_vbReq;
    //
    //ref_vb = 38;
    /*----------------------Controle da Valvula Borboleta-------------------*/

    /* Algoritmo de controle PID */
    //erro_vb =  (float)pedal_ad - (float)tps_ad;
    //erro_vb = (float)pedal_ad - (float)tps_ad;
    erro_vb = (float) ref_vb - (float) tps_ad2; //Original Bruno
    //erro_vb = 38 - (signed int16)tps_ad;  //Original Bruno


    soma_erro_vb += erro_vb;
    // Limita a somatoria (Anti-Windup)
    if (soma_erro_vb > 5000.0) soma_erro_vb = 5000.0;
        //if (soma_erro_vb > 20000.0)           soma_erro_vb = 20000.0;

        //if (soma_erro_vb > 10000) soma_erro_vb = 10000; //Original PIC
    else if (soma_erro_vb < -5000.0) soma_erro_vb = -5000.0;
    //else if (soma_erro_vb < -20000.0)     soma_erro_vb = -20000.0;

    //else if (soma_erro_vb < -10000) soma_erro_vb = -10000; //Original PIC

    _control = (soma_erro_vb * Ki_vb) + (Kp_vb * erro_vb); // + (soma_erro_vb/ki_vb);
    //control = Kp_vb*(erro_vb + soma_erro_vb/Ti_vb);



    /* Valvula precisa ser aberta */
    inv = 0;
    if (_control > 230.0) // Limitacao do sinal (PWM = 80%)
        _control = 230.0;
    //if (_control > 921.0) // Limitacao do sinal (PWM = 90%)
    //_control = 921.0;

    //      else
    //      { // Valvula precisa ser fechada
    //        _control =_control * (-1.0);
    //        inv = 1;
    //        if (_control > 200.0)
    //            _control = 200.0; // Limitacao do sinal (PWM = 80%)
    //            //if (_control > 819.0)
    //            //_control = 819.0; // Limitacao do sinal (PWM = 80%)
    //      }


    if (!PORT_L15)
    {
        _control = 0;
        DI1 = nPin_HIGH;
        DI2 = nPin_LOW;
    }
    else
    {
        DI1 = nPin_LOW;
        DI2 = nPin_HIGH;
    }

    pwm2 = (unsigned int) _control; /* Passa o calculo do pwm p/ a "variavel final" */

    pwm_10bits = (unsigned int) pwm2 * 4; /*Conversão 8bit´s para 10bit´s*/
    //pwm_10bits = (unsigned int)pwm;

    if (pwm_10bits > 1023) pwm_10bits = 1023;

    //   if (!inv)
    //   {
    //      Set_PWM3_Duty(0);
    //      Set_PWM2_Duty(pwm_10bits);
    //   }
    //   else
    //   {
    //      Set_PWM3_Duty(pwm_10bits);//Set_PWM3_Duty(pwm_10bits);
    //      Set_PWM2_Duty(0);
    //   }

    //if(pwm>700) pwm=700;
    //else pwm=pwm;
    //set_pwm1_duty(pwm); //Original Bruno
    //set_pwm2_duty(pwm);
    //set_pwm1_duty((pedal_ad-30)/3); //Le pedal, tira 34 e divide por 4 para calibrar com o do kit para 0 a 90°

    //LATDbits.LATD0 = nPin_LOW;
}

void Control_up()
{

    Erro1 = SetPoint * 1.0 - Angle * 1.0; // Erro em relação ao set-point

    nLED_TEST = !nLED_TEST;
    u_i = ui_ant + constante_integrador * (Erro1 + e1_ant); // Cálculo da parcela integrativa

    u_p = Kp*Erro1; // cálculo da parcela proporcional

    Out1 = u_i + u_p; // soma das parcelas para atuar na saída do pwm

    e1_ant = Erro1;
    ui_ant = u_i;

    if (Out1 > 500.0) // Satura saída
        Out1 = 500.0;

    if (Out1 < 0.0) // Satura saída
        Out1 = 0.0;

}

void Control_down()
{

    Erro2 = SetPoint * 1.0 - Angle * 1.0; // Erro em relação ao set-point
    Erro2 = Erro2*-1; // inverte o sinal do erro

    // LED= 1; // acende o led para o programador ver se o software está executando esta função


    u_i2 = ui_ant2 + constante_integrador2 * (Erro2 + e2_ant); // Cálculo da parcela integrativa

    u_p2 = Kp2*Erro2; // cálculo da parcela proporcional

    Out2 = u_i2 + u_p2; // soma das parcelas para atuar na saída do pwm

    e2_ant = Erro2;
    ui_ant2 = u_i2;

    if (Out2 > 100.0) // Satura saída
        Out2 = 100.0;

    if (Out2 < 0.0) // Satura saída
        Out2 = 0.0;


}

void Configure_HW(void)
{
    ANSELA = 0b00000011;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;

    /* Configuração das Portas */
    TRISA = 0b11110111;
    TRISB = 0b00010001;
    TRISC = 0x00;
    //set_tris_c(0b10111111);
    TRISD = 0x00;
    TRISE = 0x00;
    __delay_ms(1);

    //Config_ADC();
    SPI_SLAVE1();

    ADCON0 = 0b00000101; // liga o conversor AD seleciona o canal 1
    ADCON1 = 0b00000000; // referencias do conversor são VDD e VSS
    ADCON2 = 0b10100101; // resultado justificado à direita, tempo de aquisição
    // igual 4*TAD, conversion CLOCK = FOSC/16

    Configure_PWM2();
    Configure_PWM3();
    Config_Timer0();
    Config_Timer2();
}

void main(void)
{
    Configure_HW();
    Enable_Interrupt(nINTERRUPT_GLOBAL);
    Enable_Interrupt(nINTERRUPT_PERIPHERAL);
    Enable_Interrupt(nINT_TIMER0);
    Enable_IntSPI(nSPI1);
    //Disable_Interrupt(nSPI1);
    Set_PWM2_Duty(0); //inicia o pino do PWM em zero
    Set_PWM3_Duty(0);
    code = nPin_LOW;
    DI1 = nPin_LOW; // HABILITA O DRIVER
    DI2 = nPin_HIGH; // HABILITA O DRIVER
    //SF = nPin_LOW;
    ref_vb = 32; //223; //32;
    ref_vbReq = 32;
    valor[0] = 32;

    while (1)
    {


        for (i = 0; i < 5; i++)
        {
            ADCON0 = 0b00000101; // liga o conversor AD seleciona o canal 1
            ADCON0bits.GO = 1; // inicia conversão
            while (ADCON0bits.GO == 1)
            {
            }; // espera
            tps_ad = ADRESH * 256 + ADRESL; // pega o valor da conversão

            //tps_read();

            vetor_leitura[i] = tps_ad;
        }

        //Set_PWM3_Duty(pwm); // aciona o pwm de subida
        //Set_PWM2_Duty(0); //

        media = vetor_leitura[0] + vetor_leitura[1] + vetor_leitura[2] + vetor_leitura[3] + vetor_leitura[4]; //+vetor_leitura[5]+vetor_leitura[6]+vetor_leitura[7]+vetor_leitura[8]+vetor_leitura[9];
        media = media / 5;
        value = media; //ReadADC();
        media = 0;
        if (value < MIN_ADC_ANGLE)
            value = MIN_ADC_ANGLE;
        if (value > MAX_ADC_ANGLE)
            value = MAX_ADC_ANGLE;
        Angle = ((MAX_ANGLE - MIN_ANGLE)*(value - MIN_ADC_ANGLE) / (MAX_ADC_ANGLE - MIN_ADC_ANGLE)) + MIN_ANGLE;

        if (EnableControl)
        {
            EnableControl = 0;
            // Control_down();

            // pwm= Out2*10.23;
            //pwm= 40*10.23;
            // Set_PWM2_Duty(pwm); // aciona o pwm de subida
            //Set_PWM3_Duty(0); //

            if ((SetPoint >= ANGLE_LimpHome && Angle >= 4.9) || (SetPoint < ANGLE_LimpHome && Angle >= ANGLE_LimpHome))
            {


                Set_PWM3_Duty(0);
                //controle_vb();
                Control_up(); // chama a função de controle de subida
                pwm = (int) (Out1 * 10.23); // atualiza valor do pwm
                Set_PWM2_Duty(pwm); // aciona o pwm de subida
                //flag_control =1; // sinaliza a flag dizendo que utilizou o controle 1
                // setando o IN1 para 0 a válvula recebe o valor de pwm fornecido ao pino IN2
            }
            //else
            if ((SetPoint < ANGLE_LimpHome && Angle < 6.1) || (SetPoint > ANGLE_LimpHome && Angle < ANGLE_LimpHome))
            {

                Set_PWM2_Duty(0);
                // zera o pwm do controle de abertura

                Control_down(); // chama a função de controle de subida
                pwm = (int) (Out2 * 10.23); // atualiza valor do pwm
                Set_PWM3_Duty(pwm); // aciona o pwm de subida
            }









        }

    }


}
