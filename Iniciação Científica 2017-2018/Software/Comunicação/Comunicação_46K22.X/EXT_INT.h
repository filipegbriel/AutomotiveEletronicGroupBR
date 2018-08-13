/* 
 * File:   EXT_INT.h
 * Author: Hiroki
 *
 * Created on 2 de Julho de 2016, 12:57
 */

#ifndef EXT_INT_H
#define	EXT_INT_H

#include "Config_XC8.h"

/******************************************** DEFINES ************************************************/
#define nENABLE        1
#define nDISABLE       0
#define nCLEAR         0
#define nSET           1
#define nOUTPUT        0
#define nINPUT         1

#define nINTERRUPT_GLOBAL         INTCONbits.GIE
#define nINTERRUPT_PERIPHERAL     INTCONbits.PEIE

#define nExt_Int0                 INTCONbits.INT0IE
#define nExt_Int1                 INTCON3bits.INT1IE
#define nExt_Int2                 INTCON3bits.INT2IE

#define nFLAG_ExtInt0             INTCONbits.INT0IF
#define nFLAG_ExtInt1             INTCON3bits.INT1IF
#define nFLAG_ExtInt2             INTCON3bits.INT2IF

#define Enable_ExtInt(value)        (value = nENABLE)
#define Clear_ExtInt_Flag(value)    (value = nCLEAR)
#define Disable_ExtInt(value)       (value = nDISABLE)

#define __EXT_INTERRUPT0__          if(INTCONbits.INT0IF && INTCONbits.INT0IE)
#define __EXT_INTERRUPT1__          if(INTCON3bits.INT1IF && INTCON3bits.INT1IE)
#define __EXT_INTERRUPT2__          if(INTCON3bits.INT2IF && INTCON3bits.INT2IE)
    
#ifdef	__cplusplus
extern "C" {
#endif
    
extern void Confi_Int_Ext0();

extern void Confi_Int_Ext1();

extern void Confi_Int_Ext2();

#ifdef	__cplusplus
}
#endif

#endif	/* EXT_INT_H */

