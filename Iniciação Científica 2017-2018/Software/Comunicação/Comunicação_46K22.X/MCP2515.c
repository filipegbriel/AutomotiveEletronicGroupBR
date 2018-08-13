/*
 * File:   MCP2515.c
 * Author: Hiroki
 *
 * Created on 18 de Novembro de 2016, 23:41
 */


#include "Config_XC8.h"
#include "SPI_XC8.h"
#include "MCP2515.h"



void CAN_SPI2_WRITE(unsigned char addr ,unsigned char data)
{
    nSS_MCP2515 = 0;           // ativa CS chip select
    nSS_MCP2515 = 1;           // ativa CS chip select
    __delay_us(10);
    nSS_MCP2515 = 0;           // ativa CS chip select
    MASTER_SPI_WRITE2(Req_Write);   //Comando de Escrita
    __delay_us(40);            //espera slave processar informação
    MASTER_SPI_WRITE2(addr);
    __delay_us(40);    
    MASTER_SPI_WRITE2(data);
    __delay_us(40);

    nSS_MCP2515 = 1;          //desabilita CS ship select;

}

 void RESET_CAN(void)
{
    nSS_MCP2515= 1;
    nSS_MCP2515 = 0;
    nSS_MCP2515 = 1;
    nSS_MCP2515 = 0;
    __delay_us(10);
    MASTER_SPI_WRITE2(0xC0); //Envia instrução de reset
    nSS_MCP2515 = 1;
}

 void CONFIG_MCP2515(void)
 {
   CAN_SPI2_WRITE(0x0F, 0x80);          //Coloca em modo de configuração
   CAN_SPI2_WRITE(0x28, 0x02);          //CNF3 Fase2 = 3Tq   0x28,0b00000010
   CAN_SPI2_WRITE(0x29, 0xA0);          //CNF2 Fase1 = 3Tq, Propagação = 1Tq 0x29,0b10011001
   CAN_SPI2_WRITE(0x2A, 0x01);          //CNF1 BRP = 1Tq, SJW = 2Tq  0x2A,0b00000001
   CAN_SPI2_WRITE(0x0F, 0x00);          //Coloca em modo Normal
 }

