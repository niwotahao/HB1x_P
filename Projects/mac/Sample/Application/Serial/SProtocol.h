/*********************************************************************************************************
*FileName:	SProtocol.h
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   
**********************************************************************************************************
*Author			Date				Comment
*LXP			2014/3/11			Original
*********************************************************************************************************/
#ifndef _S_PROTOCOL_H_
#define _S_PROTOCOL_H_


#include "Serial.h"
/********************************************************************************************************

********************************************************************************************************/
#define USE_MAX_UART            2

#define COM_RX_BUFFTER_SIZE		64
#define COM_TX_BUFFTER_SIZE		64
/********************************************************************************************************/
#define COM_DEBUG_LABEL_HEAD  	(0xAA)
#define COM_DEBUG_LABEL_END		(0x55)

#define COM_DEBUG_STEP_HEAD	 	(0x00)
#define COM_DEBUG_STEP_CMD		(0x01)
#define COM_DEBUG_STEP_LEN1		(0x02)
#define COM_DEBUG_STEP_LEN2		(0x03)
#define COM_DEBUG_STEP_DATA1    (0x04)   

#define COM_DEBUG_STEP_CRC1		(0x05)
#define COM_DEBUG_STEP_CRC2		(0x06)
#define COM_DEBUG_STEP_END		(0x07)

#define COM_DEBUG_STEP_DATA2    (0x08) 

#define COM_DEBUG_STEP_LEN		(0x10)
#define COM_DEBUG_STEP_DATA		(0x11)
#define COM_DEBUG_STEP_CRC		(0x12)
/********************************************************************************************************

********************************************************************************************************/
extern void Serial_Init(void);
extern void Serial_vLoadingTx(uint8 dir,uint8 len,uint8 *pBuf);
extern void Serial_vRx(uint8 port,uint8 len,uint8 *pBuf);
/********************************************************************************************************

********************************************************************************************************/
#endif


