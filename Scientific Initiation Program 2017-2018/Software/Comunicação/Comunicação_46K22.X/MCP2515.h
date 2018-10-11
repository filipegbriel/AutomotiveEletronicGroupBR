/* 
 * File:   MCP2515.h
 * Author: Hiroki
 *
 * Created on 18 de Novembro de 2016, 23:42
 */

#ifndef MCP2515_H
#define	MCP2515_H

#define nSS_MCP2515     LATCbits.LATC2
#define Req_Write       (0x02)

#ifdef	__cplusplus
extern "C" {
#endif

void CAN_SPI2_WRITE(unsigned char addr ,unsigned char data);

extern void RESET_CAN(void);

extern void CONFIG_MCP2515(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MCP2515_H */

