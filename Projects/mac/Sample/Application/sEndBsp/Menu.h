

#ifndef _MENU_H_
#define _MENU_H_
/********************************************************************************************************

********************************************************************************************************/

#include "eBsp.h"
#include "Rf.h"
#include "Serial.h"
/********************************************************************************************************
                                        信号线定义
********************************************************************************************************/
#define TX_POWER_0   0
#define TX_POWER_1   1
#define TX_POWER_2   2
#define TX_POWER_3   3
#define TX_POWER_4   4
#define TX_POWER_5   5
#define TX_POWER_6   6
#define TX_POWER_X   7
/********************************************************************************************************

********************************************************************************************************/
#define MENU_MSG_SIZE   12
/********************************************************************************************************

********************************************************************************************************/
#define WRITER_NUM_MSG_ID		(0x0410)        /**< 信息数量数 */
#define WRITER_AREA_NAME_MSG_ID (0x04A0)        /**< 手表区域信息存储 */
#define WRITER_SET_STORE_ID     (0x0500)        /**< 设置参数存储区 */ 
#define WRITER_RESET_MSG_ID	    (0x0600)        /**< 信息缓冲区头 */ 
/********************************************************************************************************

********************************************************************************************************/
typedef struct
{
    ///*
    uint8 DisStartPtr  :4;
    uint8 DisEndPtr    :4;
    uint8 Size;
    //*/
    /*
    uint16 DisStartPtr :5;
    uint16 DisEndPtr   :5;
    uint16 Size        :6;
    */
    RfPc2EndMsg_t   Buf[MENU_MSG_SIZE];
}MenuMsg_t;
/********************************************************************************************************/
typedef struct
{
    uint8 b3        :4;     /**< 低位 IAR中 */
    uint8 b2        :4;
    uint8 b1        :4;
    uint8 b0        :4;     /**< 高位 IAR中 */
}Bit2_t;
/********************************************************************************************************/
typedef struct
{
    uint8 b7        :4;     /**< 低位 IAR中 */
    uint8 b6        :4;
    uint8 b5        :4;
    uint8 b4        :4;     
    uint8 b3        :4;     
    uint8 b2        :4;
    uint8 b1        :4;
    uint8 b0        :4;     /**< 高位 IAR中 */
}Bit4_t;
/********************************************************************************************************/
typedef union
{
    Bit2_t  Bit;
    uint16  Data;
}SixteenBit_t;
/********************************************************************************************************/
typedef union
{
    Bit4_t   Bit;
    uint32   Data;
}ThirtyTwoBit_t;
/********************************************************************************************************/
typedef struct
{
    uint8           TipNumber           :4;     /**< 信息提示次数 */
    uint8           KeyMotor            :4;     /**< 信息提示次数  */ 
    SixteenBit_t    UserPassword;               /**< 用户密码 */
    uint8           Channel;                    /**< 信道 */
    SixteenBit_t    PANID;                      /**< 局域网ID */
    ThirtyTwoBit_t  NetID;                      /**< NetID */
    ThirtyTwoBit_t  Addr;                       /**< 地址 */
    SixteenBit_t    SysPassword;                /**< 系统密码 */    
    uint8           AutoGetRf           :1;     /**< 自动获取RF参数 */
    uint8           DisMode             :2;     /**< 显示模式 */
    uint8           DisSort             :1;     /**< 显示排列 */
    uint8           EnShutdown          :1;     /**< 使能关机 */
    uint8           ResetToDefault      :1;     /**< 复位出厂设置 */
}SysSetStoreValue_t;
/********************************************************************************************************/
typedef struct
{
    uint8 SacnKey           :1;         /**< 扫描按键 */
    uint8 ShutDown          :1;         /**< 关机标记 */
    uint8 ShowFlash         :1;         /**< 闪烁标记 */
    uint8 SelectBit         :4;         /**< 选择密码位数 */
    uint8 Reserve           :1;         /**< 保留位 */
}MenuFlg_t;
/********************************************************************************************************
*											
********************************************************************************************************/ 
extern MenuFlg_t MenuFlg;
extern __no_init MenuMsg_t MMsg;
extern RfPc2EndAreaMsg_t RfPc2EndAreaMsg;
extern SysSetStoreValue_t SysSetValue;         /**< 用户操作系统设置缓冲 */
extern SysSetStoreValue_t SysSetStoreValue;    /**< 系统设置值 */
/********************************************************************************************************

********************************************************************************************************/
extern void MENU_RestetDefault(void);

extern void MENU_Init(void);
extern void MENU_ShowTx(void);
extern void MENU_ShowBatt(uint8 j);
extern void MENU_FillRAM(uint8 Data);
extern void MENU_ShowTxPOWER(uint8 i);
extern void MENU_ShowTime( uint8 hour,uint8 minute);
extern void MENU_ShowBatt( uint8 j);
extern void MENU_SelectPage(uint8 page);
extern void MENU_ShowShuZi(uint8 data, uint8 shuzipage, uint8 shuzifont,uint8 of);
extern void MENU_ShowString(uint8 * data, uint8 datalen, uint8 page,uint8 font,uint8 of);
extern void MENU_ClearScreen(uint8 start, uint8 end);
extern void MENU_ShowMsg(BOOL clear,int8 msgNum );
/********************************************************************************************************/
extern void MENU_MsgBufInit(void);
extern uint8 MENU_MsgSize(void);
extern uint8 MENU_ReSortMsg(uint8 s);
extern RfPc2EndMsg_t* MENU_AppendMsg(void);
extern RfPc2EndMsg_t *MENU_SearchOrRemoveMsg(RfPc2EndMsg_t *pMsg);
extern void MENU_DisPgdnKeyMsg(BOOL nAdd);
extern void MENU_DisRefreshMsg(void);
extern uint8 MENU_DisOpenKeyMsg(void);
extern void MENU_DisDeleteShortMsg(BOOL tf);
extern void MENU_ResetWriterMsg(void);
extern void  MENU_DisDeceiveMsg(void);
extern void MENU_ResetReadMsg(void);
/********************************************************************************************************/
extern void  MENU_DisResetMsg(void);
extern void  MENU_DisOffMsg(void);
extern void  MENU_DisListMsg(void);
extern void  MENU_DisOpenMsg(void);
extern void  MENU_DisLowPower(void);
extern void  MENU_DisIncPower(void);
/********************************************************************************************************/
extern void  MENU_DisUserPassword(void);
extern void  MENU_DisUserSetList(void);
extern void  MENU_DisUserSetTipNumber(void);
extern void  MENU_DisUserSetKeyMotor(void);
extern void  MENU_DisEnterPassword(void);
extern void  MENU_DisUserSetPassword(void);
extern void  MENU_DisSysPassword(void);
/********************************************************************************************************/
extern void MENU_DisSysGetRfParam(void);
extern void MENU_DisSysSetChannel(void);
extern void MENU_DisSysSetPanid(void);
extern void MENU_DisSysSetAddr(void);
extern void MENU_DisSysSetShowMode(void);
extern void MENU_DisEnShutDown(void);
extern void MENU_DisSysResetToDefault(void);
/********************************************************************************************************

********************************************************************************************************/

#endif



