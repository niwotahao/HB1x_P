/*********************************************************************************************************
*FileName:	Com.c
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   WWW.OURZIGBEE.COM
**********************************************************************************************************
*Author			Date				Comment
*LXP			2011/12/21			Original
*********************************************************************************************************/
#include "Serial.h"
#include "hal_uart.h"
#include "OSAL_Clock.h"
#include "OSAL_Nv.h"
#include "msa.h"
/********************************************************************************************************

********************************************************************************************************/
extern uint8 GetTimerflg;
/********************************************************************************************************

********************************************************************************************************/
void APP_CoordReceiveMsg(uint8 len,uint8 *pBuf)
{
    uint32 sysTimeSeconds;
    
    RfPc2EndMsg_t *p = (RfPc2EndMsg_t*)pBuf;
    
    switch(p->MsgType)
    {
        case RF_P2D_TIMER_MSG:                  /**< PC下发时间 */
            sysTimeSeconds = pBuf[9];
            sysTimeSeconds <<= 8;
            sysTimeSeconds |= pBuf[8];
            sysTimeSeconds <<= 8;
            sysTimeSeconds |= pBuf[7];
            sysTimeSeconds <<= 8;
            sysTimeSeconds |= pBuf[6];
            
            osal_setClock(sysTimeSeconds);

            GetTimerflg = 2;
            break;
        case RF_P2C_HEART_BEAT_MSG:             /**< PC 获取协调器心跳 */
            break;
        default:
            break;
    }
}
/********************************************************************************************************

********************************************************************************************************/
void APP_SearilGetTopologyStruct(uint8 len,uint8 *pBuf)
{
#define COM_TX_BUF_SIZE		7
    /*
	uint8 i;
	uint8 ComTxBuf[COM_TX_BUF_SIZE];

	i = pBuf[0];

    if(i < MAX_NODE_SIZE)
    {	 
        ComTxBuf[0] =  i;
        ComTxBuf[1] =  NwkNodeTable[i].dev.type;
        ComTxBuf[2] =  NwkNodeTable[i].extAddr;
        ComTxBuf[3] =  NwkNodeTable[i].nextHopsAddr;
        ComTxBuf[4] =  NwkNodeTable[i].PExtAddr;
        ComTxBuf[5]	=  NwkNodeTable[i].lastSeq;
        ComTxBuf[6]	=  NwkNodeTable[i].age;
    }
    else
    {
        ComTxBuf[0] =  i;
        ComTxBuf[1] =  0X00;
        ComTxBuf[2] =  0X00;
        ComTxBuf[3] =  0X00;
        ComTxBuf[4] =  0X00;
        ComTxBuf[5]	=  0X00;
        ComTxBuf[6]	=  0X00;   
    }
    Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_GET_NWK_TOPOLOGY_CMD,(sizeof(NwkNodeTable_t)+1),ComTxBuf);
    */
	len = len;
	pBuf = pBuf;
#undef COM_TX_BUF_SIZE
}
/********************************************************************************************************

********************************************************************************************************/
void APP_SearilGetAddr(uint8 len,uint8 *pBuf)
{
    uint16 addr;
    ComSetAddr_t  ComSetAddr;
    
    if(len == 1)
    {
        osal_nv_item_init(WRITER_SET_RF_MSG_ID,sizeof(ComSetAddr_t),NULL);
        osal_nv_read(WRITER_SET_RF_MSG_ID,0,sizeof(ComSetAddr_t),&ComSetAddr);
        
        addr    = NLME_GetShortAddr();
        Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_GET_ADDR_CMD,addr,sizeof(ComSetAddr_t),(uint8*)&ComSetAddr);
    }
}
/********************************************************************************************************

********************************************************************************************************/
void APP_SearilSetAddr(uint8 len,uint8 *pBuf)
{
#if(NODETYPE == COORD)
    
#define TRX_LED         P1_0
    uint8 n;
    uint16 i;
    uint16 addr;
    
    ComSetAddr_t  ComSetAddr;
	ComSetAddr_t *pComSetAddr;

    pComSetAddr = (ComSetAddr_t*)&pBuf[0];
    
    if(pComSetAddr->ver == 0x01)
	{
        if((pComSetAddr->channle <= 7)||(pComSetAddr->channle == 0xFF))
        {
            if(pComSetAddr->channle == 7)
            {
                pComSetAddr->channle = 1;
            }
            osal_nv_item_init(WRITER_SET_RF_MSG_ID,sizeof(ComSetAddr_t),NULL);
            osal_nv_write(WRITER_SET_RF_MSG_ID,0,sizeof(ComSetAddr_t),pComSetAddr);

            for(i=0;i<2000;i++)
            {
                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");
            }
            
            osal_nv_item_init(WRITER_SET_RF_MSG_ID,sizeof(ComSetAddr_t),NULL);
            osal_nv_read(WRITER_SET_RF_MSG_ID,0,sizeof(ComSetAddr_t),&ComSetAddr);
            
            if((pComSetAddr->ver == ComSetAddr.ver)&&\
               (pComSetAddr->channle == ComSetAddr.channle)&&\
               (pComSetAddr->shortAddr == ComSetAddr.shortAddr))
            {
                n = 0x01;
                addr    = NLME_GetShortAddr();
                Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_SET_ADDR_CMD,addr,1,(uint8*)&n);
                n =0;
                
            #if(HAL_UART_USB == 1)
                /**< 关闭USB D+上拉 */
                HAL_USB_PULLUP_DISABLE(); //添加
            #endif
                
                while(1)
                {
                    TRX_LED = !TRX_LED;
                    
    #define WD_INT_15_MSEC      (BV(1))
    #define WD_INT_250_MSEC     (BV(0))
    #define WD_INT_1000_MSEC    (0)
                    WDCTL = (0xA0 | WD_EN | WD_INT_1000_MSEC);
                    WDCTL = (0x50 | WD_EN | WD_INT_1000_MSEC);
                    
                    for(i=0;i<50000;i++)
                    {
                        asm("NOP");
                        asm("NOP");
                        asm("NOP");
                        asm("NOP");
                    }
                    
                    n++;
                    if(n > 100)
                    {
                        while(1);
                    }
                }
            }
        }
	}
#endif
    pBuf =pBuf;
	len =len;
}
/********************************************************************************************************

********************************************************************************************************/
void APP_SearilReplyHawVer(uint8 len,uint8 *pBuf)
{
    uint8 haw = RF_HARDWARE_VER;
    
    Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_GET_HDW_VER,0x00,1,&haw);

}
/********************************************************************************************************

********************************************************************************************************/
