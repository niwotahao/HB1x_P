/*********************************************************************************************************
*FileName:	SHal.h
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   
**********************************************************************************************************
*Author			Date				Comment
*LXP			2014/3/11			Original
*********************************************************************************************************/
#ifndef _SHAL_H_
#define _SHAL_H_
/*********************************************************************************************************

*********************************************************************************************************/
#include "Serial.h"

/*********************************************************************************************************

*********************************************************************************************************/


/*********************************************************************************************************

*********************************************************************************************************/
extern void SerialApp_Init(void);
extern void SerialApp_CallBack(uint8 port, uint8 event);





/*********************************************************************************************************

*********************************************************************************************************/
#endif
