#ifndef _MENU_H_
#define _MENU_H_
/********************************************************************************************************

********************************************************************************************************/

#include "jBsp.h"

/********************************************************************************************************

********************************************************************************************************/

extern uint8 DisData[];
/********************************************************************************************************

********************************************************************************************************/
extern void MENU_Init(void);
extern void MENU_DisCountValue(void);

extern void MENU_DisAutoManual(void);
extern void MENU_DisRfWorkMOde(void);
extern void MENU_DisChannel(void);

extern void MENU_DisSetAutoManual(void);
extern void MENU_DisSetRfWorkMOde(void);
extern void MENU_DisSetChannel(void);
/********************************************************************************************************

********************************************************************************************************/



/********************************************************************************************************

********************************************************************************************************/
#endif