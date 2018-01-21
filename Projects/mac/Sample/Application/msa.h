/**************************************************************************************************
  Filename:       msa.h
  Revised:        $Date: 2013-04-18 10:57:13 -0700 (Thu, 18 Apr 2013) $
  Revision:       $Revision: 33955 $

  Description:    This file contains the the Mac Sample Application protypes and definitions


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef MACSAMPLEAPP_H
#define MACSAMPLEAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include <ioCC2530.h>
#include "OSAL_Clock.h"
#include "OSAL.h"
#include "saddr.h"
#include "rf.h"
#include "Serial.h"
  
#if(NODETYPE == ENDDEVICE)
    #if defined(JSQ_DECEIVE)
        #include "jBsp.h"
    #else
        #include "eBsp.h"
    #endif
#endif
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
// General I/O definitions
#define IO_GIO  0  // General purpose I/O
#define IO_PER  1  // Peripheral function
#define IO_IN   0  // Input pin
#define IO_OUT  1  // Output pin
#define IO_PUD  0  // Pullup/pulldn input
#define IO_TRI  1  // Tri-state input
#define IO_PUP  0  // Pull-up input pin
#define IO_PDN  1  // Pull-down input pin

#define PORT0   0
#define PORT1   1
#define PORT2   2



/* I/O PORT CONFIGURATION */
#define CAT1(x,y) x##y  // Concatenates 2 strings
#define CAT2(x,y) CAT1(x,y)  // Forces evaluation of CAT1

// LCD port I/O defintions
// Builds I/O port name: PNAME(1,INP) ==> P1INP
#define PNAME(y,z) CAT2(P,CAT2(y,z))
// Builds I/O bit name: BNAME(1,2) ==> P1_2

#define BNAME(port,pin) CAT2(CAT2(P,port),CAT2(_,pin))

#define IO_DIR_PORT_PIN(port, pin, dir) 	\
{											\
	if ( dir == IO_OUT ) 					\
		PNAME(port,DIR) |= (1<<(pin)); 		\
	else 									\
		PNAME(port,DIR) &= ~(1<<(pin)); 	\
}


#define IO_FUNC_PORT_PIN(port, pin, func) 	\
{ 											\
	if( port < 2 ) 							\
	{ 										\
		if ( func == IO_PER ) 				\
			PNAME(port,SEL) |= (1<<(pin)); 	\
		else 								\
			PNAME(port,SEL) &= ~(1<<(pin)); \
	} 										\
	else 									\
	{ 										\
		if ( func == IO_PER )				\
			P2SEL |= (1<<(pin>>1)); 		\
		else 								\
			P2SEL &= ~(1<<(pin>>1)); 		\
	} 										\
}

#define IO_IMODE_PORT_PIN(port, pin, mode) 	\
{ 											\
	if ( mode == IO_TRI ) 					\
		PNAME(port,INP) |= (1<<(pin)); 		\
	else 									\
		PNAME(port,INP) &= ~(1<<(pin)); 	\
}

#define IO_PUD_PORT(port, dir) 				\
{ 											\
	if ( dir == IO_PDN ) 					\
		P2INP |= (1<<(port+5)); 			\
	else 									\
		P2INP &= ~(1<<(port+5)); 			\
}    
/**************************************************************************************************
 *                                        User's  Defines
 **************************************************************************************************/
/**< 12 === 26高频段方式 */
#define MSA_MAC_CHANNEL           MAC_CHAN_11   /* Default channel - change it to desired channel */
#define MSA_WAIT_PERIOD           1000           /* Time between each packet - Lower = faster sending rate */

#define MSA_PAN_ID                0x2014        /* PAN ID */

#define MSA_DIRECT_MSG_ENABLED    TRUE          /* True if direct messaging is used, False if polling is used */

#define MSA_MAC_BEACON_ORDER      15            /* Setting beacon order to 15 will disable the beacon */
#define MSA_MAC_SUPERFRAME_ORDER  15            /* Setting superframe order to 15 will disable the superframe */

#define MSA_PACKET_LENGTH         8            /* Min = 4, Max = 102 */

#define MSA_PWR_MGMT_ENABLED      TRUE         /* Enable or Disable power saving */

#define MSA_KEY_INT_ENABLED       FALSE         /*
                                                 * FALSE = Key Polling
                                                 * TRUE  = Key interrupt
                                                 *
                                                 * Notes: Key interrupt will not work well with 2430 EB because
                                                 *        all the operations using up/down/left/right switch will
                                                 *        no longer work. Normally S1 + up/down/left/right is used
                                                 *        to invoke the switches but on the 2430 EB board,  the
                                                 *        GPIO for S1 is used by the LCD.
                                                 */

#if (MSA_MAC_SUPERFRAME_ORDER > MSA_MAC_BEACON_ORDER)
#error "ERROR! Superframe order cannot be greater than beacon order."
#endif

#if ((MSA_MAC_SUPERFRAME_ORDER != 15) || (MSA_MAC_BEACON_ORDER != 15)) && (MSA_DIRECT_MSG_ENABLED == FALSE)
#error "ERROR! Cannot run beacon enabled on a polling device"
#endif

#if (MSA_PACKET_LENGTH < 4) || (MSA_PACKET_LENGTH > 102)
#error "ERROR! Packet length has to be between 4 and 102"
#endif

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/

/*********************************************************************
*********************************************************************/    
#define RF_HARDWARE_VER     0x02        /**< CC2530 v1.0.0*/
//#define RF_HARDWARE_VER     0x81      /**< SI4438 v1.0.0*/
#define RF_SOFTWARE_VER     "v1.8.A"    /**< 25~31频道，映射1~7频道。 */ 
#define RF_DEFAULT_CHANNLE  0x01        /**< 实际是26频道 */ 
#define RF_DEFAULT_PANID    0xFFFF 
/*********************************************************************
*********************************************************************/
/* Event IDs */

// Application Events (OSAL) - These are bit weighted definitions.
#define HBEEAPP_SCAN_EVENT   		        0x0001
#define HBEEAPP_POLL_EVENT   		        0x0002

#define HBEEAPP_SYS_OFF_EVT                 0x0004
#define HBEEAPP_STOP_TEN_MS_EVT             0x0008
    
#define HBEEAPP_MCU_REST_EVT   		        0x0010
#define HBEEAPP_OLED_ON_EVT                 0x0020
#define HBEEAPP_OLED_OFF_EVT                0x0040
#define HBEEAPP_ONLINE_EVT                  0x0080
    
#define HBEEAPP_MOTOR_ON_EVT                0x0100
#define HBEEAPP_MOTOR_OFF_EVT               0x0200  
#define HBEEAPP_SHOW_TIME_EVT               0x0400 
#define HBEEAPP_LOW_BATTERY_EVT             0x0800
    
#define HBEEAPP_TEN_MS_EVT                  0x1000
#define HBEEAPP_TIME_UPDATEMSG_EVT          0x2000 
     
#define HBEEAPP_WATCHDOG_EVT                0x4000

 //0xE0 - 0xFC
#define HB_RF_RX_IND                0xE1
#define HB_RF_RX_CANCEL_IND         0xE2
#define HB_SET_BSP_EVENT            0xE3
#define HB_SET_MOTOR_EVENT          0xE4
     
/********************************************************************************************************/
#define MSA_SCAN_EVENT   		0x0001
#define MSA_POLL_EVENT   		0x0002

#define MSA_SEND_EVENT   		0x0004
#define MSA_KEY_EVENT    		0x0008
#define MSA_WATCHDOG_EVT        0x0010

#define MSA_LED_EVENT    		0x0010
#define MSA_HEART_BEAT_EVENT    0x0020

#define MSA_DIS_KEY_EVENT    	0x0100
#define MSA_EN_KEY_EVENT    	0x0200

#define MSA_END_SET_EVT         0x0800
#define MSA_END_SET_CHANNLE_EVT 0x1000
/********************************************************************************************************/
/*********************************************************************/
#define NetworkIsOk()               (GetTimerflg >= 3)
/*********************************************************************/
typedef osal_event_hdr_t HBeeEvent_t;

typedef struct
{
    uint8 On   :3;
    uint8 Off  :3;
    uint8 Flg  :2;
}SetMotorCnt_t;
 /*********************************************************************
 * 
 */   
extern uint8 MSA_TaskId;
extern uint16 ApsRssi;
extern uint8 GetTimerflg;
/**************************************************************************************************
 * fun
 **************************************************************************************************/
#define NLME_GetExtAddr()           (msa_ExtAddr)
#define NLME_GetCoordShortAddr()    (msa_CoordShortAddr)
#define NLME_GetShortAddr()         (msa_DevShortAddr)
/**************************************************************************************************
 * GLOBALS
 **************************************************************************************************/
extern uint8 MSA_TaskId;

extern sAddrExt_t   msa_ExtAddr;
extern uint16       msa_CoordShortAddr;
extern uint16       msa_DevShortAddr;

#if(NODETYPE == ENDDEVICE)
extern void HBee_EndRelpyMsg(RfPc2EndMsg_t *pRfPc2EndMsg);
extern void HBee_EndSimMsg(RfPc2EndMsg_t *pRfPc2EndMsg);
extern void HBee_SetUserEvent(uint8  event,uint8 status);
extern void HBee_LedRelpyMsg(RfPc2EndMsg_t *pRfPc2EndMsg);
extern void HBee_JsqRelpyMsg(RfPc2EndMsg_t *pRfPc2EndMsg);
extern void HBee_JsqSendData(uint8 autoManual,uint16 jsz);
#endif
/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Mac Sample Application
 */
extern void MSA_Init( uint8 task_id );

/*
 * Task Event Processor for the Mac Sample Application
 */
extern uint16 MSA_ProcessEvent( uint8 task_id, uint16 events );

/*
 * Handle keys
 */
extern void MSA_HandleKeys( uint8 keys, uint8 shift );

/*
 * Handle power saving
 */
extern void MSA_PowerMgr (uint8 mode);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MACSAMPLEAPP_H */
