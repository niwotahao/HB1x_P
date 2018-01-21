/*********************************************************************************************************
*FileName:	rf.h
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   WWW.OURZIGBEE.COM
**********************************************************************************************************
*Author			Date				Comment
*LXP			2011/12/21			Original
*********************************************************************************************************/
#ifndef _RF_H_
#define _RF_H_
/********************************************************************************************************

********************************************************************************************************/
#include "comdef.h"
/********************************************************************************************************

********************************************************************************************************/
typedef struct
{
    uint16 	NetID;
	uint16 	PANID;
	uint8 	ExtAddr;
	uint8 	TxPower;
	uint8	DataRate;
	uint8	Channel;

	uint8	NodeType;
	uint8	Random;
	uint8	TxCnt;
	uint8 	BindNum;

	uint8 	CoordExtAddr;
}HalPib_t;
/********************************************************************************************************

********************************************************************************************************/
/**< 三级数据格式命令 */
/**
L2P == LED_TO_PC
T2P == TOUCH_TO_PC

C2P == COORD_TO_PC
R2P == ROUTER_TO_PC
W2P == WATCH_T0_PC
E2P == CALL_TO_PC
D2P == DECEIVE_TO_PC
P2D == PC_TO_DECEIVE
*/
/**< PC 下行数据 */
#define RF_P2D_NONE_MSG             (0x00)                      /**< 无信息 */

#define RF_P2W_CALL_MSG             (0x01)                      /**< PC下发呼叫信息 */
#define RF_P2W_CANCEL_CALL_MSG      (0x02)                      /**< PC 下发取消呼叫信息 */
#define RF_P2W_SHORT_MSG            (0x03)                      /**< PC下发短信息 */
#define RF_P2W_CANCEL_SHORT_MSG     (0x04)                      /**< PC下发取消短信息 */
#define RF_P2W_OPEN_SHORT_MSG       (0x05)                      /**< PC 获取被打开短信息的 */

#define RF_P2D_TIMER_MSG            (0x10)                      /**< PC下发时间 */

#define RF_P2C_HEART_BEAT_MSG       (0x20)                      /**< PC 获取协调器心跳 */
#define RF_P2R_HEART_BEAT_MSG       (0x21)                      /**< PC 获取路由器心跳 */
#define RF_P2W_HEART_BEAT_MSG       (0x22)                      /**< PC 获取手表器心跳 */
#define RF_P2E_HEART_BEAT_MSG       (0x23)                      /**< PC 获取呼叫器心跳 */
#define RF_P2L_HEART_BEAT_MSG       (0x24)                      /**< PC 获取灯心跳 */
#define RF_P2J_HEART_BEAT_MSG       (0x25)                      /**< PC 获取计数器心跳 */

#define RF_P2E_MSG                  (0x30)                      /**< PC 获取呼叫器信息 */

#define RF_P2L_ONOFF_MSG            (0x40)                      /**< PC发送控制灯 */
#define RF_P2J_COUNTER_MSG          (0x41)                      /**< PC发送计算值 */

#define RF_P2D_2_TIMER_MSG          (0x50)                      /**< PC下发时间2 */

/**< PC 上行数据 */
#define RF_D2P_NONE_MSG             (RF_P2D_NONE_MSG)           /**< 无信息 */

#define RF_W2P_CALL_MSG             (RF_P2W_CALL_MSG)           /**< 回复 PC 下发呼叫信息 */
#define RF_W2P_CANCEL_CALL_MSG      (RF_P2W_CANCEL_CALL_MSG)    /**< 回复 PC 下发取消呼叫信息 */
#define RF_W2P_SHORT_MSG            (RF_P2W_SHORT_MSG)          /**< 回复 PC 下发短信息 */
#define RF_W2P_CANCEL_SHORT_MSG     (RF_P2W_CANCEL_SHORT_MSG)   /**< 回复 PC 下发取消短信息 */
#define RF_W2P_OPEN_SHORT_MSG       (RF_P2W_OPEN_SHORT_MSG)     /**< 读取短消息发送信息 */

#define RF_D2P_TIMER_MSG            (RF_P2D_TIMER_MSG)          /**< 回复PC 下发时间 */

#define RF_C2P_HEART_BEAT_MSG       (RF_P2C_HEART_BEAT_MSG)     /**< 协调器心跳信息 */
#define RF_R2P_HEART_BEAT_MSG       (RF_P2R_HEART_BEAT_MSG)     /**< 路由器心跳信息 */
#define RF_W2P_HEART_BEAT_MSG       (RF_P2W_HEART_BEAT_MSG)     /**< 手表器心跳信息 */
#define RF_E2P_HEART_BEAT_MSG       (RF_P2E_HEART_BEAT_MSG)     /**< 呼叫器心跳信息 */
#define RF_L2P_HEART_BEAT_MSG       (RF_P2L_HEART_BEAT_MSG)     /**< 灯心跳 */
#define RF_J2P_HEART_BEAT_MSG       (RF_P2J_HEART_BEAT_MSG)     /**< 计数器心跳 */

#define RF_E2P_MSG                  (RF_P2E_MSG)                /**< 呼叫器信息 */

#define RF_L2P_ONOFF_MSG            (RF_P2L_ONOFF_MSG)          /**< PC发送控制灯 */
#define RF_J2P_COUNTER_MSG          (RF_P2J_COUNTER_MSG)        /**< PC 发送计算值 */

#define RF_D2P_2_TIMER_MSG          (RF_P2D_2_TIMER_MSG)        /**< 回复PC 下发时间2 */
/********************************************************************************************************/
/**<  个数据类型结构 */
typedef struct
{
    uint8   MsgType;
    uint32  TaskID;
    uint8   MsgLen;
    uint8   Msg[42];
}RfPc2EndMsg_t;

typedef struct
{
    uint8   MsgType;
    uint32  TaskID;
    uint8   MsgLen;
    uint8   Msg[16];
}RfPc2EndAreaMsg_t;

typedef struct
{
    uint8   MsgType;  
    uint32  TaskID;
    uint8   MsgLen;
    uint8   Msg[6];
}RfW2PCall_t;

typedef struct
{
    uint8   MsgType;  
    uint32  TaskID;
    uint8   MsgLen;
    uint8   Msg[4];
}RfPc2EndDTime_t;

typedef struct
{
    uint8   MsgType;
    uint32  TaskID;
    uint8   MsgLen;
    uint8   Msg[2];
}Rf2PcMsg_t;

typedef Rf2PcMsg_t RfP2EdCmd_t;
typedef Rf2PcMsg_t RfE2PMsg_t;
typedef Rf2PcMsg_t RfC2PMsg_t;
typedef Rf2PcMsg_t RfR2PMsg_t;
/********************************************************************************************************
                                               无线函数发送
********************************************************************************************************/
#define RF_EndSendAckCallMsg(len,pBuf)         		RF_SendData(RF_W2P_CALL_MSG,0x00,len,pBuf)
#define RF_EndSendAckCancelCallMsg(len,pBuf)   		RF_SendData(RF_W2P_CANCEL_CALL_MSG,0x00,len,pBuf)
#define RF_EndSendAckShortMsg(len,pBuf)        		RF_SendData(RF_W2P_SHORT_MSG,0x00,len,pBuf)
#define RF_EndSendAckCancelShortMsg(len,pBuf)  		RF_SendData(RF_W2P_CANCEL_SHORT_MSG,0x00,len,pBuf)
#define RF_EndSendAckTimeMsg(len,pBuf)         		RF_SendData(RF_D2P_TIMER_MSG,0x00,len,pBuf)

#define RF_EndSendOpenShortMsg(len,pBuf)       		RF_SendData(RF_W2P_OPEN_SHORT_MSG,0x00,len,pBuf)
#define RF_CallSendMsg(len,pBuf)               		RF_SendData(RF_E2P_MSG,0x00,len,pBuf)
#define RF_EndSendHeatBeatMsg(len,pBuf)        		RF_SendData(RF_W2P_HEART_BEAT_MSG,0x00,len,pBuf)
#define RF_RouterSendHeatBeatMsg(len,pBuf)     		RF_SendData(RF_R2P_HEART_BEAT_MSG,0x00,len,pBuf)


#define RF_EndReceiveShortMsg(len,pBuf)			    RF_EndReceiveCallMsg(len,pBuf)
#define RF_EndReceiveCancelShortMsg(len,pBuf)	    RF_EndReceiveCancelCallMsg(len,pBuf)
/********************************************************************************************************

********************************************************************************************************/

extern void RF_EndReceiveMsgInit(void);
extern void RF_EndReceiveCallMsg(uint8 len,uint8 *pBuf);
extern void RF_EndReceiveCancelCallMsg(uint8 len,uint8 *pBuf);

extern void RF_EndAddReceiveMsg(void);
extern void RF_EndDecReceiveMsg(void);

extern void RF_SendRfSetAddr(uint8 addr,uint8 len,uint8 *pBuf);
extern void RF_SendAckRfSetAddr(uint8 addr,uint8 len,uint8 *pBuf);

extern void RF_GetRadioConfigMsg(void);

extern void RF_SetAddr(uint8 len,uint8 *pBuf);
extern void RF_SetAddrCallBack(void);

extern void APP_DataIndCallBack(uint16 addr,uint8 len,uint8 *pBuf);
extern void APP_HeatIndCallBack(uint16 addr,uint8 len,uint8 *pBuf);
extern void APP_DataCnfCallBack(uint8 state,void *m);
extern void RF_SendData(uint8 cmd,uint16 addr,uint8 len,uint8 *pBuf);
/********************************************************************************************************

********************************************************************************************************/
#endif



