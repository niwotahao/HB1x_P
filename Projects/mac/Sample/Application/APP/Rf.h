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
/**< �������ݸ�ʽ���� */
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
/**< PC �������� */
#define RF_P2D_NONE_MSG             (0x00)                      /**< ����Ϣ */

#define RF_P2W_CALL_MSG             (0x01)                      /**< PC�·�������Ϣ */
#define RF_P2W_CANCEL_CALL_MSG      (0x02)                      /**< PC �·�ȡ��������Ϣ */
#define RF_P2W_SHORT_MSG            (0x03)                      /**< PC�·�����Ϣ */
#define RF_P2W_CANCEL_SHORT_MSG     (0x04)                      /**< PC�·�ȡ������Ϣ */
#define RF_P2W_OPEN_SHORT_MSG       (0x05)                      /**< PC ��ȡ���򿪶���Ϣ�� */

#define RF_P2D_TIMER_MSG            (0x10)                      /**< PC�·�ʱ�� */

#define RF_P2C_HEART_BEAT_MSG       (0x20)                      /**< PC ��ȡЭ�������� */
#define RF_P2R_HEART_BEAT_MSG       (0x21)                      /**< PC ��ȡ·�������� */
#define RF_P2W_HEART_BEAT_MSG       (0x22)                      /**< PC ��ȡ�ֱ������� */
#define RF_P2E_HEART_BEAT_MSG       (0x23)                      /**< PC ��ȡ���������� */
#define RF_P2L_HEART_BEAT_MSG       (0x24)                      /**< PC ��ȡ������ */
#define RF_P2J_HEART_BEAT_MSG       (0x25)                      /**< PC ��ȡ���������� */

#define RF_P2E_MSG                  (0x30)                      /**< PC ��ȡ��������Ϣ */

#define RF_P2L_ONOFF_MSG            (0x40)                      /**< PC���Ϳ��Ƶ� */
#define RF_P2J_COUNTER_MSG          (0x41)                      /**< PC���ͼ���ֵ */

#define RF_P2D_2_TIMER_MSG          (0x50)                      /**< PC�·�ʱ��2 */

/**< PC �������� */
#define RF_D2P_NONE_MSG             (RF_P2D_NONE_MSG)           /**< ����Ϣ */

#define RF_W2P_CALL_MSG             (RF_P2W_CALL_MSG)           /**< �ظ� PC �·�������Ϣ */
#define RF_W2P_CANCEL_CALL_MSG      (RF_P2W_CANCEL_CALL_MSG)    /**< �ظ� PC �·�ȡ��������Ϣ */
#define RF_W2P_SHORT_MSG            (RF_P2W_SHORT_MSG)          /**< �ظ� PC �·�����Ϣ */
#define RF_W2P_CANCEL_SHORT_MSG     (RF_P2W_CANCEL_SHORT_MSG)   /**< �ظ� PC �·�ȡ������Ϣ */
#define RF_W2P_OPEN_SHORT_MSG       (RF_P2W_OPEN_SHORT_MSG)     /**< ��ȡ����Ϣ������Ϣ */

#define RF_D2P_TIMER_MSG            (RF_P2D_TIMER_MSG)          /**< �ظ�PC �·�ʱ�� */

#define RF_C2P_HEART_BEAT_MSG       (RF_P2C_HEART_BEAT_MSG)     /**< Э����������Ϣ */
#define RF_R2P_HEART_BEAT_MSG       (RF_P2R_HEART_BEAT_MSG)     /**< ·����������Ϣ */
#define RF_W2P_HEART_BEAT_MSG       (RF_P2W_HEART_BEAT_MSG)     /**< �ֱ���������Ϣ */
#define RF_E2P_HEART_BEAT_MSG       (RF_P2E_HEART_BEAT_MSG)     /**< ������������Ϣ */
#define RF_L2P_HEART_BEAT_MSG       (RF_P2L_HEART_BEAT_MSG)     /**< ������ */
#define RF_J2P_HEART_BEAT_MSG       (RF_P2J_HEART_BEAT_MSG)     /**< ���������� */

#define RF_E2P_MSG                  (RF_P2E_MSG)                /**< ��������Ϣ */

#define RF_L2P_ONOFF_MSG            (RF_P2L_ONOFF_MSG)          /**< PC���Ϳ��Ƶ� */
#define RF_J2P_COUNTER_MSG          (RF_P2J_COUNTER_MSG)        /**< PC ���ͼ���ֵ */

#define RF_D2P_2_TIMER_MSG          (RF_P2D_2_TIMER_MSG)        /**< �ظ�PC �·�ʱ��2 */
/********************************************************************************************************/
/**<  ���������ͽṹ */
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
                                               ���ߺ�������
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



