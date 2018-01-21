/**************************************************************************************************
  Filename:       msa.c
  Revised:        $Date: 2013-02-25 14:58:47 -0800 (Mon, 25 Feb 2013) $
  Revision:       $Revision: 33292 $

  Description:    This file contains the sample application that can be use to test
                  the functionality of the MAC, HAL and low level.


  Copyright 2006-2013 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/**************************************************************************************************

    Description:

                                KEY UP (or S1)
                              - Non Beacon
                              - First board in the network will be setup as the coordinator
                              - Any board after the first one will be setup as the device
                                 |
          KEY LEFT               |      KEY RIGHT(or S2)
                             ----+----- - Start transmitting
                                 |
                                 |
                                KEY DOWN


**************************************************************************************************/


/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/
#include <string.h>

/* Hal Driver includes */
#include "hal_types.h"
#include "hal_timer.h"
#include "hal_drivers.h"
#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_uart.h"

/* OS includes */
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "OSAL_PwrMgr.h"
#include "OSAL_Nv.h"
/* Application Includes */
#include "OnBoard.h"

/* For Random numbers */
#include "mac_radio_defs.h"

/* MAC Application Interface */
#include "mac_api.h"
#include "mac_main.h"

#ifdef FEATURE_MAC_SECURITY
#include "mac_spec.h"

/* MAC Sceurity */
#include "mac_security_pib.h"
#include "mac_security.h"
#endif /* FEATURE_MAC_SECURITY */

/* Application */
#include "msa.h"
#include "rf.h"
#include "SHal.h"
#include "SAps.h"
#include "Serial.h"

#include "jBsp.h"
#include "hal_Rkey.h"
/********************************************************************************************************

********************************************************************************************************/

#define MSA_SCAN_STEP		0
#define MSA_BEACON_STEP		1
#define MSA_ASSOC_STEP		2
#define MSA_NWK_OK_STEP		3

/* WATCHDOG TIMER DEFINITIONS */
// WDCTL bit definitions

#define WDINT0     0x01  // Interval, bit0
#define WDINT1     0x02  // Interval, bit1
#define WDINT      0x03  // Interval, mask
#define WDMODE     0x04  // Mode: watchdog=0, timer=1
#define WDEN       0x08  // Timer: disabled=0, enabled=1
#define WDCLR0     0x10  // Clear timer, bit0
#define WDCLR1     0x20  // Clear timer, bit1
#define WDCLR2     0x40  // Clear timer, bit2
#define WDCLR3     0x80  // Clear timer, bit3
#define WDCLR      0xF0  // Clear timer, mask

// WD timer intervals
#define WDTISH     0x03  // Short: clk * 64
#define WDTIMD     0x02  // Medium: clk * 512
#define WDTILG     0x01  // Long: clk * 8192
#define WDTIMX     0x00  // Maximum: clk * 32768

// WD clear timer patterns
#define WDCLP1     0xA0  // Clear pattern 1
#define WDCLP2     0x50  // Clear pattern 2

/* WATCHDOG TIMER DEFINITIONS */
#define WatchDogEnable(wdti)              \
{                                         \
  WDCTL = WDCLP1 | WDEN | (wdti & WDINT); \
  WDCTL = WDCLP2 | WDEN | (wdti & WDINT); \
}
/**************************************************************************************************
 *                                           Constant
 **************************************************************************************************/
#define MSA_MAC_MAX_RESULTS       10             /* Maximun number of scan result that will be accepted */

#if defined (HAL_BOARD_CC2420DB)
#define MSA_HAL_ADC_CHANNEL     HAL_ADC_CHANNEL_0             /* AVR - Channel 0 and Resolution 10 */
#define MSA_HAL_ADC_RESOLUTION  HAL_ADC_RESOLUTION_10
#elif defined (HAL_BOARD_DZ1611) || defined (HAL_BOARD_DZ1612) || defined (HAL_BOARD_DRFG4618) || defined (HAL_BOARD_F2618)
#define MSA_HAL_ADC_CHANNEL     HAL_ADC_CHANNEL_0             /* DZ1611 and DZ1612 - Channel 0 and Resolution 12 */
#define MSA_HAL_ADC_RESOLUTION  HAL_ADC_RESOLUTION_12
#else
#define MSA_HAL_ADC_CHANNEL     HAL_ADC_CHANNEL_7             /* CC2430 EB & DB - Channel 7 and Resolution 14 */
#define MSA_HAL_ADC_RESOLUTION  HAL_ADC_RESOLUTION_14
#endif

#define MSA_EBR_PERMITJOINING    TRUE
#define MSA_EBR_LINKQUALITY      1
#define MSA_EBR_PERCENTFILTER    0xFF

/* Size table for MAC structures */
const CODE uint8 msa_cbackSizeTable [] =
{
    0,                                   /* unused */
    sizeof(macMlmeAssociateInd_t),       /* MAC_MLME_ASSOCIATE_IND */
    sizeof(macMlmeAssociateCnf_t),       /* MAC_MLME_ASSOCIATE_CNF */
    sizeof(macMlmeDisassociateInd_t),    /* MAC_MLME_DISASSOCIATE_IND */
    sizeof(macMlmeDisassociateCnf_t),    /* MAC_MLME_DISASSOCIATE_CNF */
    sizeof(macMlmeBeaconNotifyInd_t),    /* MAC_MLME_BEACON_NOTIFY_IND */
    sizeof(macMlmeOrphanInd_t),          /* MAC_MLME_ORPHAN_IND */
    sizeof(macMlmeScanCnf_t),            /* MAC_MLME_SCAN_CNF */
    sizeof(macMlmeStartCnf_t),           /* MAC_MLME_START_CNF */
    sizeof(macMlmeSyncLossInd_t),        /* MAC_MLME_SYNC_LOSS_IND */
    sizeof(macMlmePollCnf_t),            /* MAC_MLME_POLL_CNF */
    sizeof(macMlmeCommStatusInd_t),      /* MAC_MLME_COMM_STATUS_IND */
    sizeof(macMcpsDataCnf_t),            /* MAC_MCPS_DATA_CNF */
    sizeof(macMcpsDataInd_t),            /* MAC_MCPS_DATA_IND */
    sizeof(macMcpsPurgeCnf_t),           /* MAC_MCPS_PURGE_CNF */
    sizeof(macEventHdr_t),               /* MAC_PWR_ON_CNF */
    sizeof(macMlmePollInd_t)             /* MAC_MLME_POLL_IND */
};

/**************************************************************************************************
 *                                        Local Variables
 **************************************************************************************************/

/*
  Extended address of the device, for coordinator, it will be msa_ExtAddr1
  For any device, it will be msa_ExtAddr2 with the last 2 bytes from a ADC read
*/
sAddrExt_t    msa_ExtAddr;
///sAddrExt_t    msa_ExtAddr1 = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
///sAddrExt_t    msa_ExtAddr2 = {0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x00, 0x00};

/* Coordinator and Device information */
uint16        msa_PanId = MSA_PAN_ID;
uint16        msa_CoordShortAddr = 0xFFFF;
uint16        msa_DevShortAddr   = 0xFFFF;
#ifdef FEATURE_MAC_SECURITY
/* Current number of devices (including coordinator) communicating to this device */
uint8       msa_NumOfSecuredDevices = 0;
#endif /* FEATURE_MAC_SECURITY */


/* TRUE and FALSE value */
bool          msa_MACTrue = TRUE;
bool          msa_MACFalse = FALSE;

/* Beacon payload, this is used to determine if the device is not zigbee device */
uint8         msa_BeaconPayload[] = {0x20, 0x14, 0x04};
uint8         msa_BeaconPayloadLen = 3;

/* Contains pan descriptor results from scan */
macPanDesc_t  msa_PanDesc[MSA_MAC_MAX_RESULTS];

/* flags used in the application */
bool          msa_IsCoordinator  = FALSE;   /* True if the device is started as a Pan Coordinate */
bool          msa_IsStarted      = FALSE;   /* True if the device started, either as Pan Coordinator or device */
bool          msa_IsSampleBeacon = FALSE;   /* True if the beacon payload match with the predefined */
bool          msa_IsDirectMsg    = FALSE;   /* True if the messages will be sent as direct messages */

uint8 	      msa_Step = MSA_SCAN_STEP;
uint8 	      msa_SendFailCnt = 0;
/* Structure that used for association request */
macMlmeAssociateReq_t msa_AssociateReq;

/* Structure that used for association response */
macMlmeAssociateRsp_t msa_AssociateRsp;

uint8 msa_SuperFrameOrder;
uint8 msa_BeaconOrder;

/* Task ID */
uint8 MSA_TaskId;
uint8 LedCnt=0;
uint32 KeyValue=0x00;
uint32 PowerAdc = 0;
uint8 DisIntFlg = 0;
uint8 HeatBatCnt = 120;
uint16 EndTxAddr = 0xFFFF;


uint8 GetTimerflg = 0;
uint8 LostNwkCnt = 0;

uint8 ResetDly = FALSE;

uint8 PowerCnt = 0;
uint8 BattLowCnt=0;
uint8 SetMotorCnt = 0;
uint8 BeepCnt = 0;


uint8 RfChannle=0x00;
uint32 MacChannelList = ((uint32)1)<<MSA_MAC_CHANNEL;
/********************************************************************************************************

********************************************************************************************************/


#ifdef FEATURE_MAC_SECURITY
/**************************************************************************************************
 *                                  MAC security related constants
 **************************************************************************************************/
#define MSA_KEY_TABLE_ENTRIES         1
#define MSA_KEY_ID_LOOKUP_ENTRIES     1
#define MSA_KEY_DEVICE_TABLE_ENTRIES  8
#define MSA_KEY_USAGE_TABLE_ENTRIES   1
#define MSA_DEVICE_TABLE_ENTRIES      0 /* can grow up to MAX_DEVICE_TABLE_ENTRIES */
#define MSA_SECURITY_LEVEL_ENTRIES    1
#define MSA_MAC_SEC_LEVEL             MAC_SEC_LEVEL_ENC_MIC_32
#define MSA_MAC_KEY_ID_MODE           MAC_KEY_ID_MODE_1
#define MSA_MAC_KEY_SOURCE            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define MSA_MAC_DEFAULT_KEY_SOURCE    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33}
#define MSA_MAC_KEY_INDEX             3  /* cannot be zero for implicit key identifier */


/* Default MSA Security Parameters for outgoing frames */
bool  macSecurity       = TRUE;
uint8 msa_securityLevel = MSA_MAC_SEC_LEVEL;
uint8 msa_keyIdMode     = MSA_MAC_KEY_ID_MODE;
uint8 msa_keySource[]   = MSA_MAC_KEY_SOURCE;
uint8 msa_keyIndex      = MSA_MAC_KEY_INDEX;


/**************************************************************************************************
 *                                 Security PIBs for outgoing frames
 **************************************************************************************************/
const keyIdLookupDescriptor_t msa_keyIdLookupList[] =
{
    {
        {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x03}, 0x01 /* index 3, 9 octets */
    }
};

/* Key device list can be modified at run time
 */
keyDeviceDescriptor_t msa_keyDeviceList[] =
{
    {0x00, false, false},
    {0x00, false, false},
    {0x00, false, false},
    {0x00, false, false},
    {0x00, false, false},
    {0x00, false, false},
    {0x00, false, false},
    {0x00, false, false}
};

const keyUsageDescriptor_t msa_keyUsageList[] =
{
    {MAC_FRAME_TYPE_DATA, MAC_DATA_REQ_FRAME}
};

const keyDescriptor_t msa_keyTable[] =
{
    {
        (keyIdLookupDescriptor_t *)msa_keyIdLookupList, MSA_KEY_ID_LOOKUP_ENTRIES,
        (keyDeviceDescriptor_t *)msa_keyDeviceList, MSA_KEY_DEVICE_TABLE_ENTRIES,
        (keyUsageDescriptor_t *)msa_keyUsageList, MSA_KEY_USAGE_TABLE_ENTRIES,
        {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    }
};

const uint8 msa_keyDefaultSource[] = MSA_MAC_DEFAULT_KEY_SOURCE;


/**************************************************************************************************
 *                                 Security PIBs for incoming frames
 **************************************************************************************************/

/* Device table can be modified at run time. */
deviceDescriptor_t msa_deviceTable[] =
{
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE},
    {0, 0xFFFF, 0, 0, FALSE}
};

const securityLevelDescriptor_t msa_securityLevelTable[] =
{
    {MAC_FRAME_TYPE_DATA, MAC_DATA_REQ_FRAME, MAC_SEC_LEVEL_NONE, FALSE}
};
#else /* FEATURE_MAC_SECURITY */
uint8 msa_securityLevel = MAC_SEC_LEVEL_NONE;
uint8 msa_keyIdMode     = MAC_KEY_ID_MODE_NONE;
uint8 msa_keySource[]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8 msa_keyIndex      = 0;
#endif /* FEATURE_MAC_SECURITY */


/**************************************************************************************************
 *                                     Local Function Prototypes
 **************************************************************************************************/
/* Setup routines */
void MSA_CoordinatorStartup(void);
void MSA_DeviceStartup(void);

/* MAC related routines */
void MSA_AssociateReq(void);
void MSA_AssociateRsp(macCbackEvent_t* pMsg);
void MSA_McpsDataReq(uint8* data, uint8 dataLength, bool directMsg, uint16 dstShortAddr);
void MSA_McpsPollReq(void);
void MSA_ScanReq(uint8 scanType, uint8 scanDuration);
void MSA_SyncReq(void);

/* Support */
bool MSA_BeaconPayLoadCheck(uint8* pSdu);

void BSP_InitInterrupt(void);
void BSP_LedProcessEvent(uint8 n);
void BSP_InitSerial(void);
void BSP_SerialCallBack(uint8 port, uint8 event);


uint16 POWER_ReadBattery(void);
void Onboard_wait(uint16 timeout);

void HBee_JsqGetSysTimer(void);
void HBee_JsqSendHeatBeat(void);
void RF_SendData(uint8 cmd,uint16 addr,uint8 len,uint8 *pBuf);

void SN74LS164SegInit(void);
void SN74LS164SendData(uint8 data);
void SN74LS164SegDisplay(void);
/**************************************************************************************************/

/**************************************************************************************************/
#ifdef FEATURE_MAC_SECURITY
/**************************************************************************************************
 *
 * @fn          MSA_SecurityInit
 *
 * @brief       Initialize the security part of the application
 *
 * @param       none
 *
 * @return      none
 *
 **************************************************************************************************/
static void MSA_SecurityInit(void)
{
    uint8   keyTableEntries      = MSA_KEY_TABLE_ENTRIES;
    uint8   autoRequestSecLevel  = MAC_SEC_LEVEL_NONE;
    uint8   securityLevelEntries = MSA_SECURITY_LEVEL_ENTRIES;

    /* Write key table PIBs */
    MAC_MlmeSetSecurityReq(MAC_KEY_TABLE, (void *)msa_keyTable);
    MAC_MlmeSetSecurityReq(MAC_KEY_TABLE_ENTRIES, &keyTableEntries );

    /* Write default key source to PIB */
    MAC_MlmeSetSecurityReq(MAC_DEFAULT_KEY_SOURCE, (void *)msa_keyDefaultSource);

    /* Write security level table to PIB */
    MAC_MlmeSetSecurityReq(MAC_SECURITY_LEVEL_TABLE, (void *)msa_securityLevelTable);
    MAC_MlmeSetSecurityReq(MAC_SECURITY_LEVEL_TABLE_ENTRIES, &securityLevelEntries);

    /* TBD: MAC_AUTO_REQUEST is true on by default
    * need to set auto request security PIBs.
    * dieable auto request security for now
    */
    MAC_MlmeSetSecurityReq(MAC_AUTO_REQUEST_SECURITY_LEVEL, &autoRequestSecLevel);

    /* Turn on MAC security */
    MAC_MlmeSetReq(MAC_SECURITY_ENABLED, &macSecurity);
}


/**************************************************************************************************
 *
 * @fn          MSA_SecuredDeviceTableUpdate
 *
 * @brief       Update secured device table and key device descriptor handle
 *
 * @param       panID - Secured network PAN ID
 * @param       shortAddr - Other device's short address
 * @param       extAddr - Other device's extended address
 * @param       pNumOfSecuredDevices - pointer to number of secured devices
 *
 * @return      none
 *
 **************************************************************************************************/
static void MSA_SecuredDeviceTableUpdate(uint16 panID,
        uint16 shortAddr,
        sAddrExt_t extAddr,
        uint8 *pNumOfSecuredDevices)
{
    /* Update key device list */
    msa_keyDeviceList[*pNumOfSecuredDevices].deviceDescriptorHandle = *pNumOfSecuredDevices;

    /* Update device table */
    msa_deviceTable[*pNumOfSecuredDevices].panID = panID;
    msa_deviceTable[*pNumOfSecuredDevices].shortAddress = shortAddr;
    sAddrExtCpy(msa_deviceTable[*pNumOfSecuredDevices].extAddress, extAddr);
    MAC_MlmeSetSecurityReq(MAC_DEVICE_TABLE, msa_deviceTable);

    /* Increase the number of secured devices */
    (*pNumOfSecuredDevices)++;
    MAC_MlmeSetSecurityReq(MAC_DEVICE_TABLE_ENTRIES, pNumOfSecuredDevices);
}
#endif /* FEATURE_MAC_SECURITY */
/********************************************************************************************************

********************************************************************************************************/
void BSP_InitInterrupt(void)
{
    /***************Key Interrupt Init****************/
    //APCFG |= 0x20;//P0.5ADC
    P0IFG = 0x00; //Clear any pending interrupt
    P0IF  = 0;
    PICTL |= 0x01;  //Falling edge
    //P0IEN |= 0x0F; //P0.0123 Enable interrupt Mask
    IEN1 |= 0x20;  //BV(5)P0 Interrupt enable
    /*
    KEY
    P0.3
    P0.2
    P0.1
    P0.0
    */
#define KEY3_PIN 3   //P0.3
#define KEY2_PIN 2   //P0.2
#define KEY1_PIN 1   //P0.1
#define KEY0_PIN 0   //P0.0

    IO_FUNC_PORT_PIN(PORT0,KEY3_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT0,KEY3_PIN,IO_IN);
    IO_IMODE_PORT_PIN(PORT0,KEY3_PIN,IO_PUP);

    IO_FUNC_PORT_PIN(PORT0,KEY2_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT0,KEY2_PIN,IO_IN);
    IO_IMODE_PORT_PIN(PORT0,KEY2_PIN,IO_PUP);

    IO_FUNC_PORT_PIN(PORT0,KEY1_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT0,KEY1_PIN,IO_IN);
    IO_IMODE_PORT_PIN(PORT0,KEY1_PIN,IO_PUP);
    
    IO_FUNC_PORT_PIN(PORT0,KEY0_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT0,KEY0_PIN,IO_IN);
    IO_IMODE_PORT_PIN(PORT0,KEY0_PIN,IO_PUP);
    
	IO_PUD_PORT(0,IO_PUP);
    
#define BEEP_PIN    7   //P0.7
    
    P0_7 = 0;
    
    IO_FUNC_PORT_PIN(PORT0,BEEP_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT0,BEEP_PIN,IO_OUT);
}
/********************************************************************************************************

********************************************************************************************************/
void Onboard_wait( uint16 timeout )
{
  while (timeout--)
  {
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }
}
/**************************************************************************************************
 *
 * @fn          MSA_Init
 *
 * @brief       Initialize the application
 *
 * @param       taskId - taskId of the task after it was added in the OSAL task queue
 *
 * @return      none
 *
 **************************************************************************************************/
void MSA_Init(uint8 taskId)
{
    /* Initialize the task id */
    MSA_TaskId = taskId;

    /* initialize MAC features */
    MAC_InitDevice();
    MAC_InitCoord();

    /* Initialize MAC beacon */
    MAC_InitBeaconDevice();
    MAC_InitBeaconCoord();

    /* Reset the MAC */
    MAC_MlmeResetReq(TRUE);

#ifdef FEATURE_MAC_SECURITY
    /* Initialize the security part of the application */
    MSA_SecurityInit();
#endif /* FEATURE_MAC_SECURITY */

    msa_BeaconOrder = MSA_MAC_BEACON_ORDER;
    msa_SuperFrameOrder = MSA_MAC_SUPERFRAME_ORDER;

    //第一个长地址 0x780C
    //第二个长地址 0x3FFE8
#define EXADDR0     PXREG( 0x780C )
#define EXADDR1     PXREG( 0x780D )
#define EXADDR2     PXREG( 0x780E )
#define EXADDR3     PXREG( 0x780F )
#define EXADDR4     PXREG( 0x7810 )
#define EXADDR5     PXREG( 0x7811 )
#define EXADDR6     PXREG( 0x7812 )
#define EXADDR7     PXREG( 0x7813 )

    HalFlashRead(HAL_FLASH_IEEE_PAGE, HAL_FLASH_IEEE_OSET, msa_ExtAddr, 8);
    if(((msa_ExtAddr[0]==0xFF)&&(msa_ExtAddr[1]==0xFF)&&(msa_ExtAddr[2]==0xFF)&&(msa_ExtAddr[3]==0xFF)&&
        (msa_ExtAddr[4]==0xFF)&&(msa_ExtAddr[5]==0xFF)&&(msa_ExtAddr[6]==0xFF)&&(msa_ExtAddr[7]==0xFF)))
    {
        msa_ExtAddr[0] = *EXADDR0;
        msa_ExtAddr[1] = *EXADDR1;
        msa_ExtAddr[2] = *EXADDR2;
        msa_ExtAddr[3] = *EXADDR3;
        msa_ExtAddr[4] = *EXADDR4;
        msa_ExtAddr[5] = *EXADDR5;
        msa_ExtAddr[6] = *EXADDR6;
        msa_ExtAddr[7] = *EXADDR7;
    }
    
    
    BSP_InitInterrupt();
    BSP_Init();
    
    /**< 电池检测 */
    //osal_set_event( MSA_TaskId,HBEEAPP_LOW_BATTERY_EVT);
    
    msa_PanId           = 0x2014;
    MacChannelList      = ((uint32)1)<<MSA_MAC_CHANNEL;

    RfChannle = 11+StoreBspFlg.Channel;
    
    msa_DevShortAddr  = msa_ExtAddr[1];
    msa_DevShortAddr <<=8;
    msa_DevShortAddr |= msa_ExtAddr[0];
    
    
    MAC_MlmeSetReq(MAC_LOGICAL_CHANNEL,&RfChannle);
    MAC_MlmeSetReq(MAC_PAN_ID, &msa_PanId);
    
    MAC_MlmeSetReq(MAC_SHORT_ADDRESS, &msa_ExtAddr);
    MAC_MlmeSetReq(MAC_EXTENDED_ADDRESS, &msa_ExtAddr);
    
    /* PWRMGR_ALWAYS_ON */
    MSA_PowerMgr (TRUE);

    PowerAdc = POWER_ReadBattery();
    PowerAdc+= POWER_ReadBattery();
    PowerAdc+= POWER_ReadBattery();
    PowerAdc+= POWER_ReadBattery();
    PowerAdc >>=2;

    /**< 显示打开 */
    osal_start_timerEx(MSA_TaskId,MSA_SCAN_EVENT,100);
    
    osal_start_timerEx( MSA_TaskId,MSA_LED_EVENT,100);
    osal_start_timerEx(MSA_TaskId,MSA_KEY_EVENT,100);
    
    HBee_SetUserEvent(HB_SET_BSP_EVENT,BSP_KEY_CLEAR);
    
    WatchDogEnable(WDTIMX);
    osal_start_timerEx( MSA_TaskId,HBEEAPP_WATCHDOG_EVT,50);
}
/**************************************************************************************************
 *
 * @fn          MSA_ProcessEvent
 *
 * @brief       This routine handles events
 *
 * @param       taskId - ID of the application task when it registered with the OSAL
 *              events - Events for this task
 *
 * @return      16bit - Unprocessed events
 *
 **************************************************************************************************/
uint16 MSA_ProcessEvent(uint8 taskId, uint16 events)
{
    uint8* pMsg;
    macCbackEvent_t* pData;
    
#if !defined(COORD_NODE)
    
#ifdef FEATURE_MAC_SECURITY
    uint16       panID;
    uint16       panCoordShort;
    sAddrExt_t   panCoordExtAddr;
#endif /* FEATURE_MAC_SECURITY */
    
    uint8 lqi = 0,n=0,i=0;
    BspEvent_t event;
    HBeeEvent_t *hBeeEvent;
#endif

    if (events & SYS_EVENT_MSG)
    {
        while ((pMsg = osal_msg_receive(MSA_TaskId)) != NULL)
        {
            switch ( *pMsg )
            {
                case MAC_MLME_SCAN_CNF:
					/* Check if there is any Coordinator out there */
					pData = (macCbackEvent_t *) pMsg;

                    if ((msa_IsSampleBeacon) && pData->scanCnf.hdr.status == MAC_SUCCESS)
                    {
						/**< 选取最好的协调器 */
                        for(i=0; i<pData->scanCnf.resultListSize; i++)
                        {
                            if(pData->scanCnf.result.pPanDescriptor[i].linkQuality > lqi)
                            {
                                lqi = pData->scanCnf.result.pPanDescriptor[i].linkQuality;
                                n = i;
                            }
                        }
                        msa_AssociateReq.coordAddress.addrMode = SADDR_MODE_SHORT;
                        msa_AssociateReq.logicalChannel = pData->scanCnf.result.pPanDescriptor[n].logicalChannel;//MSA_MAC_CHANNEL
                        msa_AssociateReq.coordAddress.addr.shortAddr = pData->scanCnf.result.pPanDescriptor[n].coordAddress.addr.shortAddr;
                        msa_AssociateReq.coordPanId = pData->scanCnf.result.pPanDescriptor[n].coordPanId;

                        /* Retrieve beacon order and superframe order from the beacon */
                        msa_BeaconOrder = MAC_SFS_BEACON_ORDER(pData->scanCnf.result.pPanDescriptor[n].superframeSpec);
                        msa_SuperFrameOrder = MAC_SFS_SUPERFRAME_ORDER(pData->scanCnf.result.pPanDescriptor[n].superframeSpec);

                        if (msa_IsDirectMsg)
                        {
                            msa_AssociateReq.capabilityInformation = MAC_CAPABLE_ALLOC_ADDR | MAC_CAPABLE_RX_ON_IDLE;
                        }
                        else
                        {
                            msa_AssociateReq.capabilityInformation = MAC_CAPABLE_ALLOC_ADDR;
                        }
                        msa_AssociateReq.sec.securityLevel = MAC_SEC_LEVEL_NONE;

                        /**<  获取协调器地址 */
                        msa_PanId           = msa_AssociateReq.coordPanId;
                        msa_CoordShortAddr  = msa_AssociateReq.coordAddress.addr.shortAddr;


                        /* Start the devive up as beacon enabled or not */
                        MSA_DeviceStartup();

                        /* Call Associate Req */
                        MSA_AssociateReq();
                    }
					else
					{
						/**< 清除信标正确标记 */
						msa_IsSampleBeacon = FALSE;
					}
                    break;
                case MAC_MLME_COMM_STATUS_IND:
                    break;
                case MAC_MLME_BEACON_NOTIFY_IND:
                    /* Retrieve the message */
                    pData = (macCbackEvent_t *) pMsg;

                    /* Check for correct beacon payload */
                    if (!msa_IsStarted)
                    {
						if(MSA_BeaconPayLoadCheck(pData->beaconNotifyInd.pSdu))
						{
							msa_IsSampleBeacon = TRUE; 
						}
						else
						{
							/**< 不是自己网络里的信标清除 */ 
							pData->beaconNotifyInd.pPanDesc->linkQuality = 0;
						}
                    }
                    break;
                case MAC_MLME_ASSOCIATE_CNF:
                    /* Retrieve the message */
                    pData = (macCbackEvent_t *) pMsg;

                    if ((!msa_IsStarted) && (pData->associateCnf.hdr.status == MAC_SUCCESS))
                    {
                        /* Retrieve MAC_SHORT_ADDRESS */
                        /**< 判断是否是自己长地址的后两位 */
                        if(msa_DevShortAddr == pData->associateCnf.assocShortAddress)
                        {
                            msa_DevShortAddr = pData->associateCnf.assocShortAddress;

                            /* Setup MAC_SHORT_ADDRESS - obtained from Association */
                            MAC_MlmeSetReq(MAC_SHORT_ADDRESS, &msa_DevShortAddr);

						#ifdef FEATURE_MAC_SECURITY
                            /* Add the coordinator to device table and key device table for security */
                            MAC_MlmeGetReq(MAC_PAN_ID, &panID);
                            MAC_MlmeGetSecurityReq(MAC_PAN_COORD_SHORT_ADDRESS, &panCoordShort);
                            MAC_MlmeGetSecurityReq(MAC_PAN_COORD_EXTENDED_ADDRESS, &panCoordExtAddr);
                            MSA_SecuredDeviceTableUpdate(panID, panCoordShort,
                                                         panCoordExtAddr, &msa_NumOfSecuredDevices);
						#endif /* FEATURE_MAC_SECURITY */

							msa_IsStarted = TRUE;
                            /**< 启动呼叫器心跳 */
                            osal_start_timerEx(MSA_TaskId,MSA_HEART_BEAT_EVENT,10);
                        }
                    }
					/**< 清除信标正确标记 */
					msa_IsSampleBeacon = FALSE;
                    break;
                    break;
                case MAC_MLME_POLL_CNF:
                    pData = (macCbackEvent_t*)pMsg;
                    if(pData->pollCnf.hdr.status == MAC_NO_ACK)
                    {
                    }
                    else if(pData->pollCnf.hdr.status == MAC_SUCCESS)
                    {
                    }
                    break;
                case MAC_MCPS_DATA_IND:
                    pData = (macCbackEvent_t*)pMsg;
                    if((pData->dataInd.msdu.len <= 64)&&(pData->dataInd.msdu.len > 0))
                    {
                      
                    }
                    break;
                case MAC_MCPS_DATA_CNF:
                    pData = (macCbackEvent_t*) pMsg;
                    /* If last transmission completed, ready to send the next one */
                    if (pData->dataCnf.hdr.status == MAC_NO_ACK)
                    {
                    }
                    else if(pData->pollCnf.hdr.status == MAC_SUCCESS)
                    {
                    }
                    mac_msg_deallocate((uint8**)&pData->dataCnf.pDataReq);
                    break;
                case HB_SET_BSP_EVENT:

                    hBeeEvent = (HBeeEvent_t*)pMsg;//MSGpkt
                    BSP_EventTask((BspEvent_t)hBeeEvent->status);
                    break;
            }
            /* Deallocate */
            mac_msg_deallocate((uint8 **)&pMsg);
        }

        return (events ^ SYS_EVENT_MSG);
    }
    if(events&MSA_SCAN_EVENT)
    {
        /**< 网络启动标记 */							
		msa_IsStarted = FALSE;  
        msa_IsSampleBeacon = FALSE;
		/**< 设备心跳 */
		//osal_stop_timerEx(MSA_TaskId,MSA_HEART_BEAT_EVENT);
		/**< 清空信标缓存区 */
		memset(&msa_PanDesc[0], 0,(MSA_MAC_MAX_RESULTS*sizeof(macPanDesc_t)));
        /**< 设备主动扫描 */
        MSA_ScanReq(MAC_SCAN_ACTIVE,0);
        
        return (events ^ MSA_SCAN_EVENT);
    }
    if(events & HBEEAPP_WATCHDOG_EVT)
    {
    #define WD_INT_15_MSEC      (BV(1))
    #define WD_INT_250_MSEC     (BV(0))
    #define WD_INT_1000_MSEC    (0)
        WDCTL = (0xA0 | WD_EN | WD_INT_1000_MSEC);
        WDCTL = (0x50 | WD_EN | WD_INT_1000_MSEC);
      
        HBee_SetUserEvent(HB_SET_BSP_EVENT,BSP_TIME_FLASH);

        osal_start_timerEx( MSA_TaskId,HBEEAPP_WATCHDOG_EVT,500);
        return (events ^ HBEEAPP_WATCHDOG_EVT);
    }
    /**< 计数器心跳 */
    if(events&MSA_HEART_BEAT_EVENT)
    {
        /**< 呼叫器2小时发送一次心跳 */
        HeatBatCnt++;
        if(HeatBatCnt > 120)
        {
            HeatBatCnt = 0;
            
            /**< 读取电池电量 */
            PowerAdc+= POWER_ReadBattery();
            PowerAdc+= POWER_ReadBattery();
            PowerAdc+= POWER_ReadBattery();
            PowerAdc >>=2;
            
            HBee_JsqSendHeatBeat();
        }
        osal_start_timerEx(MSA_TaskId, MSA_HEART_BEAT_EVENT,60000);
        return (events ^ MSA_HEART_BEAT_EVENT);
    }
	if(events&HBEEAPP_SHOW_TIME_EVT)
	{
		return (events ^ HBEEAPP_SHOW_TIME_EVT);
	}
    if(events&MSA_LED_EVENT)
    {
        SN74LS164SegDisplay();
        osal_start_timerEx(MSA_TaskId,MSA_LED_EVENT,5);
        return(events^MSA_LED_EVENT);
    }
    /**< 10MS平台 */
	if(events&MSA_KEY_EVENT)
	{
		event = KEY_Read();
        if(event != BSP_NONE)
        {
            if(event != BSP_KEY_AUTO_ADD)
            {
                P0_7 = 1;
                BeepCnt = 5;
            }
            HBee_SetUserEvent(HB_SET_BSP_EVENT,event);
        }
        
        if(BeepCnt > 0)
        {
            BeepCnt--;
        }
        else
        {
            P0_7 = 0;
        }
        osal_start_timerEx(MSA_TaskId,MSA_KEY_EVENT,10);
		return(events^MSA_KEY_EVENT);	
	}
    /**< 系统复位 */
    if(events&HBEEAPP_MCU_REST_EVT)
    {
        SystemReset();
        //return(events^HBEEAPP_MCU_REST_EVT);
    }
    return 0;
}

/********************************************************************************************************

********************************************************************************************************/
#define KEY0_INTERRUPT_FLAG_MASK        0x01//P0.0
#define KEY1_INTERRUPT_FLAG_MASK        0x02//P0.1
#define KEY2_INTERRUPT_FLAG_MASK        0x04//P0.2
#define KEY3_INTERRUPT_FLAG_MASK        0x08//P0.3
HAL_ISR_FUNCTION( MyPort0Isr, P0INT_VECTOR )
{
    P0IFG = 0;
    P0IF  = 0;
}
/********************************************************************************************************

********************************************************************************************************/
void HBee_SetUserEvent(uint8  event,uint8 status)
{
    HBeeEvent_t *msgPtr;
    
    msgPtr = (HBeeEvent_t *)osal_msg_allocate(sizeof(HBeeEvent_t));
    if(msgPtr)
    {
        msgPtr->event   = event;
        msgPtr->status  = status;
        
        osal_msg_send(MSA_TaskId,(uint8 *)msgPtr);
    }
}
/*********************************************************************
*********************************************************************/
void HBee_JsqSendHeatBeat(void)
{
    uint8 *p,n=0;
    uint8 buf[12];
    uint16 addr;

    p       = NLME_GetExtAddr();
    addr    = NLME_GetCoordShortAddr();

    /**< MessageType */
    buf[n++] = RF_J2P_HEART_BEAT_MSG;
    /**< TaskID*/
    buf[n++] = p[0];
    buf[n++] = p[1];
    buf[n++] = p[2];
    buf[n++] = p[3];
    /**< DataLen */
    buf[n++] = 4;
    /**< Data */
    buf[n++] = (uint8)(addr);	        /**< 父短地址 */
    buf[n++] = (uint8)(addr>>8); 

    buf[n++] = (uint8)(PowerAdc);	        /**< 电量 */
    buf[n++] = (uint8)(PowerAdc>>8); 

    RF_SendData(RF_J2P_HEART_BEAT_MSG,addr,n,buf);
}
/*********************************************************************
*********************************************************************/
void HBee_JsqGetSysTimer(void)
{
    uint8 *p,n=0;
    uint8 buf[12];
    uint16 addr;
    uint32 seconds;

    p       = NLME_GetExtAddr();
    addr    = NLME_GetCoordShortAddr();

    /**< MessageType */
    buf[n++] = RF_D2P_2_TIMER_MSG;
    /**< TaskID*/
    buf[n++] = p[0];
    buf[n++] = p[1];
    buf[n++] = p[2];
    buf[n++] = p[3];
    /**< DataLen */
    buf[n++] = 4;
    /**< Data */
    seconds = osal_getClock();

    buf[n++] = (uint8)(seconds);	    /**< UTC */
    buf[n++] = (uint8)(seconds>>8);
    buf[n++] = (uint8)(seconds>>16);
    buf[n++] = (uint8)(seconds>>24);

    RF_SendData(RF_D2P_2_TIMER_MSG,addr,n,buf);
}
/*********************************************************************
*********************************************************************/
void HBee_JsqRelpyMsg(RfPc2EndMsg_t *pRfPc2EndMsg)
{
    uint8 *p;
    uint16 addr;
    RfPc2EndDTime_t RfPc2EndDTime;

    addr    = NLME_GetCoordShortAddr();
    p       = NLME_GetExtAddr();
    
    /**< MessageType */
    RfPc2EndDTime.MsgType   = pRfPc2EndMsg->MsgType;
    /**< TaskID*/
    RfPc2EndDTime.TaskID    = p[3];
    RfPc2EndDTime.TaskID  <<= 8;
    RfPc2EndDTime.TaskID   |= p[2];
    RfPc2EndDTime.TaskID  <<= 8;
    RfPc2EndDTime.TaskID   |= p[1];
    RfPc2EndDTime.TaskID  <<= 8;
    RfPc2EndDTime.TaskID   |= p[0];
    /**< DataLen */
    RfPc2EndDTime.MsgLen    = 3;
    /**< Data */
    RfPc2EndDTime.Msg[0]    = pRfPc2EndMsg->Msg[0];
    RfPc2EndDTime.Msg[1]    = pRfPc2EndMsg->Msg[1];
    RfPc2EndDTime.Msg[2]    = pRfPc2EndMsg->Msg[2]; 
    
    RF_SendData(RfPc2EndDTime.MsgType,addr,9,(uint8*)&RfPc2EndDTime);
}
/********************************************************************************************************

********************************************************************************************************/
#define HAL_ADC_REF_125V    0x00    /* Internal 1.25V Reference */
#define HAL_ADC_DEC_128     0x10    /* Decimate by 128 : 10-bit resolution */
#define HAL_ADC_DEC_256     0x20    /* Decimate by 256 : 12-bit resolution */
#define HAL_ADC_DEC_512     0x30    /* Decimate by 512 : 14-bit resolution */
/********************************************************************************************************

********************************************************************************************************/
uint16 POWER_ReadBattery(void)
{
    uint16 value;

    /* Clear ADC interrupt flag */
    ADCIF = 0;

    ADCCON3 = (HAL_ADC_REF_125V | HAL_ADC_DEC_512 | HAL_ADC_CHN_VDD3);

    /* Wait for the conversion to finish */
    while ( !ADCIF );

    /* Get the result */
    value = ADCL;
    value |= ((uint16) ADCH) << 8;
    value = value >> 2;

    return value;
}
/********************************************************************************************************

********************************************************************************************************/
/********************************************************************************************************

********************************************************************************************************/
void RF_SendData(uint8 cmd,uint16 addr,uint8 len,uint8 *pBuf)
{
    uint8 i,n=0;
    static uint8 *pMsg;
    
    pMsg = osal_mem_alloc(len+2);
    if(pMsg != NULL)
    {
        pMsg[n++] = cmd;
        pMsg[n++] = len;
        
        for(i=0;i<len;i++)
        {
             pMsg[n++] = pBuf[i];
        }
		
        /**< 添加CMD 和 len的长度 */
        len +=2;
#if(NODETYPE == ENDDEVICE)
        MSA_McpsDataReq(pMsg,len,true,addr);
#else
        if((cmd >= 0x40 )&&(cmd <= 0x5F))
        {
            /**< 发送给不睡眠节点数据 */
            MSA_McpsDataReq(pMsg,len,true,addr);
        }
        else
        {
            MSA_McpsDataReq(pMsg,len,false,addr);
        }
#endif
        osal_mem_free(pMsg);
    }
}
/**************************************************************************************************/
void HBee_JsqSendData(uint8 autoManual,uint16 jsz)
{
    RfPc2EndDTime_t RfPc2End;
    
    msa_CoordShortAddr = 0xFFFF;

    RfPc2End.MsgType = RF_J2P_COUNTER_MSG;
    RfPc2End.MsgLen  = 3;
    
    RfPc2End.Msg[0]  = autoManual;      //1 = 手动, 0 = 自动
    
    RfPc2End.Msg[1]  = (uint8)(jsz);
    RfPc2End.Msg[2]  = (uint8)(jsz>>8);  
    HBee_JsqRelpyMsg((RfPc2EndMsg_t*)&RfPc2End);
}
/**************************************************************************************************
 *
 * @fn          MAC_CbackEvent
 *
 * @brief       This callback function sends MAC events to the application.
 *              The application must implement this function.  A typical
 *              implementation of this function would allocate an OSAL message,
 *              copy the event parameters to the message, and send the message
 *              to the application's OSAL event handler.  This function may be
 *              executed from task or interrupt context and therefore must
 *              be reentrant.
 *
 * @param       pData - Pointer to parameters structure.
 *
 * @return      None.
 *
 **************************************************************************************************/
void MAC_CbackEvent(macCbackEvent_t *pData)
{

    macCbackEvent_t *pMsg = NULL;

    uint8 len = msa_cbackSizeTable[pData->hdr.event];

    switch (pData->hdr.event)
    {
        case MAC_MLME_BEACON_NOTIFY_IND:

            len += sizeof(macPanDesc_t) + pData->beaconNotifyInd.sduLength +
                   MAC_PEND_FIELDS_LEN(pData->beaconNotifyInd.pendAddrSpec);
            if ((pMsg = (macCbackEvent_t *) osal_msg_allocate(len)) != NULL)
            {
                /* Copy data over and pass them up */
                osal_memcpy(pMsg, pData, sizeof(macMlmeBeaconNotifyInd_t));
                pMsg->beaconNotifyInd.pPanDesc = (macPanDesc_t *) ((uint8 *) pMsg + sizeof(macMlmeBeaconNotifyInd_t));
                osal_memcpy(pMsg->beaconNotifyInd.pPanDesc, pData->beaconNotifyInd.pPanDesc, sizeof(macPanDesc_t));
                pMsg->beaconNotifyInd.pSdu = (uint8 *) (pMsg->beaconNotifyInd.pPanDesc + 1);
                osal_memcpy(pMsg->beaconNotifyInd.pSdu, pData->beaconNotifyInd.pSdu, pData->beaconNotifyInd.sduLength);
            }
            break;
        case MAC_MCPS_DATA_IND:
            pMsg = pData;
            break;
        default:
            if ((pMsg = (macCbackEvent_t *) osal_msg_allocate(len)) != NULL)
            {
                osal_memcpy(pMsg, pData, len);
            }
            break;
    }

    if (pMsg != NULL)
    {
        osal_msg_send(MSA_TaskId, (uint8 *) pMsg);
    }
}

/**************************************************************************************************
 *
 * @fn      MAC_CbackCheckPending
 *
 * @brief   Returns the number of indirect messages pending in the application
 *
 * @param   None
 *
 * @return  Number of indirect messages in the application
 *
 **************************************************************************************************/
uint8 MAC_CbackCheckPending(void)
{
    return (0);
}

/**************************************************************************************************
 * @fn          MAC_CbackQueryRetransmit
 *
 * @brief       This function callback function returns whether or not to continue MAC
 *              retransmission.
 *              A return value '0x00' will indicate no continuation of retry and a return value
 *              '0x01' will indicate to continue retransmission. This callback function shall be
 *              used to stop continuing retransmission for RF4CE.
 *              MAC shall call this callback function whenever it finishes transmitting a packet
 *              for macMaxFrameRetries times.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      0x00 to stop retransmission, 0x01 to continue retransmission.
 **************************************************************************************************
*/
uint8 MAC_CbackQueryRetransmit(void)
{
    /* Stub */
    return(0);
}

/**************************************************************************************************
 *
 * @fn      MSA_CoordinatorStartup()
 *
 * @brief   Update the timer per tick
 *
 * @param   n/a
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_CoordinatorStartup()
{
    macMlmeStartReq_t   startReq;

    /* Setup MAC_EXTENDED_ADDRESS */
    ///sAddrExtCpy(msa_ExtAddr, msa_ExtAddr1);
    MAC_MlmeSetReq(MAC_EXTENDED_ADDRESS, &msa_ExtAddr);

    /* Setup MAC_SHORT_ADDRESS */
    MAC_MlmeSetReq(MAC_SHORT_ADDRESS, &msa_CoordShortAddr);

    /* Setup MAC_BEACON_PAYLOAD_LENGTH */
    MAC_MlmeSetReq(MAC_BEACON_PAYLOAD_LENGTH, &msa_BeaconPayloadLen);

    /* Setup MAC_BEACON_PAYLOAD */
    MAC_MlmeSetReq(MAC_BEACON_PAYLOAD, &msa_BeaconPayload);

    /* Enable RX */
    MAC_MlmeSetReq(MAC_RX_ON_WHEN_IDLE, &msa_MACTrue);

    /* Setup MAC_ASSOCIATION_PERMIT */
    MAC_MlmeSetReq(MAC_ASSOCIATION_PERMIT, &msa_MACTrue);

    /* Fill in the information for the start request structure */
    startReq.startTime = 0;
    startReq.panId = msa_PanId;
    startReq.logicalChannel = RfChannle;
    startReq.beaconOrder = msa_BeaconOrder;
    startReq.superframeOrder = msa_SuperFrameOrder;
    startReq.panCoordinator = TRUE;
    startReq.batteryLifeExt = FALSE;
    startReq.coordRealignment = FALSE;
    startReq.realignSec.securityLevel = FALSE;
    startReq.beaconSec.securityLevel = FALSE;

    /* Call start request to start the device as a coordinator */
    MAC_MlmeStartReq(&startReq);

    /* Allow Beacon mode coordinator to sleep */
    if (msa_BeaconOrder != 15)
    {
        /* Power saving */
        //MSA_PowerMgr (MSA_PWR_MGMT_ENABLED);
    }

}

/**************************************************************************************************
 *
 * @fn      MSA_DeviceStartup()
 *
 * @brief   Update the timer per tick
 *
 * @param   beaconEnable: TRUE/FALSE
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_DeviceStartup()
{

    ///sAddrExtCpy(msa_ExtAddr, msa_ExtAddr2);
    MAC_MlmeSetReq(MAC_EXTENDED_ADDRESS, &msa_ExtAddr);

    /* Setup MAC_BEACON_PAYLOAD_LENGTH */
    MAC_MlmeSetReq(MAC_BEACON_PAYLOAD_LENGTH, &msa_BeaconPayloadLen);

    /* Setup MAC_BEACON_PAYLOAD */
    MAC_MlmeSetReq(MAC_BEACON_PAYLOAD, &msa_BeaconPayload);

    /* Setup PAN ID */
    MAC_MlmeSetReq(MAC_PAN_ID, &msa_PanId);

    /* This device is setup for Direct Message */
    if (msa_IsDirectMsg)
    {
        MAC_MlmeSetReq(MAC_RX_ON_WHEN_IDLE, &msa_MACTrue);
    }
    else
    {
        MAC_MlmeSetReq(MAC_RX_ON_WHEN_IDLE, &msa_MACFalse);
    }

    /* Setup Coordinator short address */
    MAC_MlmeSetReq(MAC_COORD_SHORT_ADDRESS, &msa_AssociateReq.coordAddress.addr.shortAddr);

#ifdef FEATURE_MAC_SECURITY
    /* Setup Coordinator short address for security */
    MAC_MlmeSetSecurityReq(MAC_PAN_COORD_SHORT_ADDRESS, &msa_AssociateReq.coordAddress.addr.shortAddr);
#endif /* FEATURE_MAC_SECURITY */

    if (msa_BeaconOrder != 15)
    {
        /* Setup Beacon Order */
        MAC_MlmeSetReq(MAC_BEACON_ORDER, &msa_BeaconOrder);

        /* Setup Super Frame Order */
        MAC_MlmeSetReq(MAC_SUPERFRAME_ORDER, &msa_SuperFrameOrder);

        /* Sync request */
        MSA_SyncReq();
    }

    /* Power saving */
    MSA_PowerMgr (MSA_PWR_MGMT_ENABLED);

}

/**************************************************************************************************
 *
 * @fn      MSA_AssociateReq()
 *
 * @brief
 *
 * @param    None
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_AssociateReq(void)
{
    MAC_MlmeAssociateReq(&msa_AssociateReq);
}

/**************************************************************************************************
 *
 * @fn      MSA_AssociateRsp()
 *
 * @brief   This routine is called by Associate_Ind inorder to return the response to the device
 *
 * @param   pMsg - pointer to the structure recieved by MAC_MLME_ASSOCIATE_IND
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_AssociateRsp(macCbackEvent_t* pMsg)
{
    /* Assign the short address  for the Device, from pool */
    uint16 assocShortAddress;

    assocShortAddress = pMsg->associateInd.deviceAddress[1];
    assocShortAddress <<= 8;
    assocShortAddress |= pMsg->associateInd.deviceAddress[0];

#ifdef FEATURE_MAC_SECURITY
    uint16 panID;

    /* Add device to device table for security */
    MAC_MlmeGetReq(MAC_PAN_ID, &panID);
    MSA_SecuredDeviceTableUpdate(panID, assocShortAddress,
                                 pMsg->associateInd.deviceAddress,
                                 &msa_NumOfSecuredDevices);
#endif /* FEATURE_MAC_SECURITY */

    ///MAC_MlmeSetReq(MAC_ASSOCIATION_PERMIT, &msa_MACFalse);

    /* Fill in association respond message */
    sAddrExtCpy(msa_AssociateRsp.deviceAddress, pMsg->associateInd.deviceAddress);
    msa_AssociateRsp.assocShortAddress = assocShortAddress;
    msa_AssociateRsp.status = MAC_SUCCESS;

    msa_AssociateRsp.sec.securityLevel = MAC_SEC_LEVEL_NONE;

    /* Call Associate Response */
    MAC_MlmeAssociateRsp(&msa_AssociateRsp);
}

/**************************************************************************************************
 *
 * @fn      MSA_McpsDataReq()
 *
 * @brief   This routine calls the Data Request
 *
 * @param   data       - contains the data that would be sent
 *          dataLength - length of the data that will be sent
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_McpsDataReq(uint8* data, uint8 dataLength, bool directMsg, uint16 dstShortAddr)
{
    macMcpsDataReq_t  *pData;
    static uint8      handle = 0;

    if ((pData = MAC_McpsDataAlloc(dataLength, msa_securityLevel, msa_keyIdMode)) != NULL)
    {
        pData->mac.srcAddrMode = SADDR_MODE_SHORT;
        pData->mac.dstAddr.addrMode = SADDR_MODE_SHORT;
        pData->mac.dstAddr.addr.shortAddr = dstShortAddr;
        pData->mac.dstPanId = msa_PanId;
        pData->mac.msduHandle = handle++;
        if(dstShortAddr == 0xFFFF)
        {
            pData->mac.txOptions = 0;
        }
        else
        {
            pData->mac.txOptions = MAC_TXOPTION_ACK;
        }

        /* MAC security parameters */
        osal_memcpy( pData->sec.keySource, msa_keySource, MAC_KEY_SOURCE_MAX_LEN );
        pData->sec.securityLevel = msa_securityLevel;
        pData->sec.keyIdMode = msa_keyIdMode;
        pData->sec.keyIndex = msa_keyIndex;

        /* If it's the coordinator and the device is in-direct message */
        if (msa_IsCoordinator)
        {
            if (!directMsg)
            {
                pData->mac.txOptions |= MAC_TXOPTION_INDIRECT;
            }
        }

        /* Copy data */
        osal_memcpy (pData->msdu.p, data, dataLength);

        /* Send out data request */
        MAC_McpsDataReq(pData);
    }

}

/**************************************************************************************************
 *
 * @fn      MSA_McpsPollReq()
 *
 * @brief   Performs a poll request on the coordinator
 *
 * @param   None
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_McpsPollReq(void)
{
    macMlmePollReq_t  pollReq;

    /* Fill in information for poll request */
    pollReq.coordAddress.addrMode = SADDR_MODE_SHORT;
    pollReq.coordAddress.addr.shortAddr = msa_CoordShortAddr;
    pollReq.coordPanId = msa_PanId;
    pollReq.sec.securityLevel = MAC_SEC_LEVEL_NONE;

    /* Call poll reuqest */
    MAC_MlmePollReq(&pollReq);
}

/**************************************************************************************************
 *
 * @fn      MacSampelApp_ScanReq()
 *
 * @brief   Performs active scan on specified channel
 *
 * @param   None
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_ScanReq(uint8 scanType, uint8 scanDuration)
{
    macMlmeScanReq_t scanReq;

    /* Fill in information for scan request structure */
    scanReq.scanChannels = MacChannelList;
    scanReq.scanType = scanType;
    scanReq.scanDuration = scanDuration;
    scanReq.maxResults = MSA_MAC_MAX_RESULTS;
    scanReq.permitJoining = MSA_EBR_PERMITJOINING;
    scanReq.linkQuality = MSA_EBR_LINKQUALITY;
    scanReq.percentFilter = MSA_EBR_PERCENTFILTER;
    scanReq.result.pPanDescriptor = msa_PanDesc;
    osal_memset(&scanReq.sec, 0, sizeof(macSec_t));

    /* Call scan request */
    MAC_MlmeScanReq(&scanReq);
}

/**************************************************************************************************
 *
 * @fn      MSA_SyncReq()
 *
 * @brief   Sync Request
 *
 * @param   None
 *
 * @return  None
 *
 **************************************************************************************************/
void MSA_SyncReq(void)
{
    macMlmeSyncReq_t syncReq;

    /* Fill in information for sync request structure */
    syncReq.logicalChannel = RfChannle;
    syncReq.channelPage    = MAC_CHANNEL_PAGE_0;
    syncReq.trackBeacon    = TRUE;

    /* Call sync request */
    MAC_MlmeSyncReq(&syncReq);
}

/**************************************************************************************************
 *
 * @fn      MSA_BeaconPayLoadCheck()
 *
 * @brief   Check if the beacon comes from MSA but not zigbee
 *
 * @param   pSdu - pointer to the buffer that contains the data
 *
 * @return  TRUE or FALSE
 *
 **************************************************************************************************/
bool MSA_BeaconPayLoadCheck(uint8* pSdu)
{
    uint8 i = 0;
    for (i=0; i<msa_BeaconPayloadLen; i++)
    {
        if (pSdu[i] != msa_BeaconPayload[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/**************************************************************************************************
 *
 * @fn      MSA_HandleKeys
 *
 * @brief   Callback service for keys
 *
 * @param   keys  - keys that were pressed
 *          state - shifted
 *
 * @return  void
 *
 **************************************************************************************************/
void MSA_HandleKeys(uint8 keys, uint8 shift)
{

}
/**************************************************************************************************
 **************************************************************************************************/
