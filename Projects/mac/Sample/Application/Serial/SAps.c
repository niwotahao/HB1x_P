/*********************************************************************************************************
*FileName:	SApp.c
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   
**********************************************************************************************************
*Author			Date				Comment
*LXP			2014/3/11			Original
*********************************************************************************************************/
#include "OnBoard.h"
#include "Serial.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "msa.h"
/********************************************************************************************************

********************************************************************************************************/
void Serial_vTx(uint8 dir,uint8 eComCmd,uint16 addr,uint8 len,uint8 *pBuf)
{	
	uint8 i,n = 0;
	uint8 *ComTxBuf = NULL;

    ComTxBuf = (uint8 *)osal_msg_allocate(len+3);
    if(ComTxBuf != NULL)
    {
        ComTxBuf[n++] = eComCmd;                  /**<  �������� */
        /**> ����Υ���˵͵�ַ�ȷ���ԭ����λ�����������ˣ�����Ͳ��� */
        //ComTxBuf[n++] = (uint8)(addr >>8);        /**<  �豸��ַ��8λ */
        //ComTxBuf[n++] = (uint8)addr;              /**<  �豸��ַ��8λ */
        
        ComTxBuf[n++] = (uint8)(addr);              /**<  �豸��ַ��8λ */
        ComTxBuf[n++] = (uint8)(addr>>8);           /**<  �豸��ַ��8λ */
        for(i=0;i<len;i++)
        {
            ComTxBuf[n++] = pBuf[i];             /**<  ���ݰ� */
        }
        Serial_vLoadingTx(dir,n,(uint8*)ComTxBuf);
        
        osal_msg_deallocate((uint8*)ComTxBuf);
    }
}
#include "hal_uart.h"
/********************************************************************************************************

********************************************************************************************************/
void Serial_vRxSplit(uint8 dir,uint8 len,uint8 *pBuf)
{
	uint8 dCmd,rfCmd;
	uint16 addr,addr2;
    RfPc2EndMsg_t *pP2EMsg;
    RfPc2EndDTime_t RfPc2EndDTime;
    uint32 sysTimeSeconds;

	dCmd = *pBuf++;			/**<  ������������ */
	addr = *pBuf++;			/**<  ���յ�ַ��8λ */
	addr2 = *pBuf++;		/**<  ���յ�ַ��8λ */
	
    addr |= (addr2<<8)&0xFF00;
    
    len = len - 3;
    /**< dCmd + addr*2 + pBuf[x] */
    
    //HalUARTWrite(HAL_UART_PORT_0,pBuf,len);
    //return;
    
    rfCmd = pBuf[0];
    
	switch(dir)
	{
		case SERIAL_PC_TX_CMD:  /** < PC�����·� */
            switch(dCmd)
            {
                case COM_P2D_RF_MSG_CMD:
                
                    switch(rfCmd)
                    {
                        case RF_P2W_CALL_MSG:                   /**< PC�·�������Ϣ */
                        case RF_P2W_CANCEL_CALL_MSG:            /**< PC�·�ȡ��������Ϣ */
                        case RF_P2W_SHORT_MSG:                  /**< PC�·�����Ϣ */
                        case RF_P2W_CANCEL_SHORT_MSG:           /**< PC�·�ȡ������Ϣ */
                        
                        case RF_P2L_ONOFF_MSG:
                        case RF_P2J_COUNTER_MSG:
                        
                        case RF_P2W_OPEN_SHORT_MSG:             /**< PC ��ȡ���򿪶���Ϣ�� */
                        case RF_P2D_TIMER_MSG:                  /**< PC�·�ʱ�� */
                        case RF_D2P_2_TIMER_MSG:
                        //case RF_P2C_HEART_BEAT_MSG:             /**< PC ��ȡЭ�������� */
                        //case RF_P2R_HEART_BEAT_MSG:             /**< PC ��ȡ·�������� */
                        //case RF_P2W_HEART_BEAT_MSG:             /**< PC ��ȡ�ֱ������� */
                        //case RF_P2E_HEART_BEAT_MSG:             /**< PC ��ȡ���������� */
                        //case RF_P2E_MSG:                        /**< PC ��ȡ��������Ϣ */
  
                        /**< ���͸����豸����Ϣ���� */
                        case (RF_P2W_CALL_MSG|0x80):            /**< PC�·�������Ϣ */
                        case (RF_P2W_CANCEL_CALL_MSG|0x80):     /**< PC�·�ȡ��������Ϣ */
                        case (RF_P2W_SHORT_MSG|0x80):           /**< PC�·�����Ϣ */
                        case (RF_P2W_CANCEL_SHORT_MSG|0x80):    /**< PC�·�ȡ������Ϣ */
                        case (RF_P2L_ONOFF_MSG|0x80):
                        case (RF_P2J_COUNTER_MSG|0x80):
                        
                            if(len <= sizeof(RfPc2EndMsg_t))
                            {
                                /**< ������ģ������ */
                                if(addr == msa_CoordShortAddr)
                                {
                                    if(rfCmd&0x80)
                                    {
                                        if(len <= sizeof(RfPc2EndMsg_t))
                                        {
                                            pP2EMsg = (RfPc2EndMsg_t*)pBuf;
                                            if(pP2EMsg->MsgLen >= 3)
                                            {
                                                pP2EMsg->MsgLen -= 2;       /**< ȥ�����յ�ַ���� */
                                                len             -= 2;       /**< ȥ�������ܳ��� */
                                                pP2EMsg->MsgType&= 0x7F;    /**< ȥ��0x80 */
                                                
                                                addr    = pP2EMsg->Msg[pP2EMsg->MsgLen+1];
                                                addr  <<=8;
                                                addr   |= pP2EMsg->Msg[pP2EMsg->MsgLen];
      
                                                Serial_vTx(SERIAL_PC_TX_CMD,COM_P2D_RF_MSG_CMD,addr,len,pBuf);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        /**< ���ڷ��͸�Э���������� */
                                        APP_CoordReceiveMsg(len,pBuf);
                                    }
                                }
                                else
                                {
									if(rfCmd&0x80)
									{
										rfCmd &=0x7F;
										pP2EMsg = (RfPc2EndMsg_t*)pBuf;
										if(pP2EMsg->MsgLen >= 3)
										{
											pP2EMsg->MsgLen -= 2;		/**< ȥ�����յ�ַ���� */
											len 			-= 2;		/**< ȥ�������ܳ��� */
											pP2EMsg->MsgType&= 0x7F;	/**< ȥ��0x80 */
											
											addr	= pP2EMsg->Msg[pP2EMsg->MsgLen+1];
											addr  <<=8;
											addr   |= pP2EMsg->Msg[pP2EMsg->MsgLen];
										}
										else
										{
											break;
										}
									}

                                    /**< ������յ���ģ������ݲ���ת�� */
                                    RF_SendData(rfCmd,addr,len,pBuf); 
                                }
                            }
                            
                            break;
                        default:
                            break;
                    }
                    break;
                /**< --------------------------------------------------------- */    
                case COM_P2D_SET_ADDR_CMD:
				    APP_SearilSetAddr(len,pBuf);            /**< �������õ�ַ */
                    break;
                case COM_P2D_RF_SET_ADDR_CMD:
                #if defined(SET_ADDR_DECEIVE)
				    RF_SetAddr(len,pBuf);                   /**< ����ת�����õ�ַ */
			    #endif
                    break;
                case COM_P2D_GET_NWK_TOPOLOGY_CMD:
				    APP_SearilGetTopologyStruct(len,pBuf);  /**< ��ȡ������������ */ 
                    break;
				case COM_P2D_RESET_DECEIVE_CMD:
                    SystemReset();
					break;
				case COM_P2D_GET_HDW_VER:
                    APP_SearilReplyHawVer(len,pBuf);
					break;
                case COM_P2D_GET_ADDR_CMD:
                    APP_SearilGetAddr(len,pBuf);
                    break;
                default:
                    break;
            }
			break;
		case SERIAL_PC_RX_CMD: /** < �����Ϸ� */
            /**< ·�ɽ��յ�ת����Э���� */
            switch(dCmd)
            {
                case COM_D2P_RF_MSG_CMD:
                
                    switch(rfCmd)
                    {
                        case RF_W2P_CALL_MSG:                   /**< �ظ�PC�·�������Ϣ */
                        case RF_W2P_CANCEL_CALL_MSG:            /**< �ظ�PC�·�ȡ��������Ϣ */
                        case RF_W2P_SHORT_MSG:                  /**< �ظ�PC�·�����Ϣ */
                        case RF_W2P_CANCEL_SHORT_MSG:           /**< �ظ�PC�·�ȡ������Ϣ */
                        case RF_W2P_OPEN_SHORT_MSG:             /**< ��ȡ����Ϣ������Ϣ */
                        //case RF_D2P_TIMER_MSG:					/**< �ظ�PC�·�ʱ�� */
                        //case RF_C2P_HEART_BEAT_MSG:             /**< Э����������Ϣ */    
                        //case RF_R2P_HEART_BEAT_MSG:             /**< ·����������Ϣ */
                        case RF_W2P_HEART_BEAT_MSG:             /**< �ֱ���������Ϣ */    
                        case RF_E2P_HEART_BEAT_MSG:             /**< ������������Ϣ */
                        case RF_L2P_HEART_BEAT_MSG:             /**< ������ */
                        case RF_J2P_HEART_BEAT_MSG:             /**< ���������� */
                        case RF_E2P_MSG:                        /**< ��������Ϣ */
                        case RF_L2P_ONOFF_MSG:                  /**< PC���Ϳ��Ƶ� */
                        case RF_J2P_COUNTER_MSG:                /**< PC ���ͼ���ֵ */
                        //case RF_D2P_2_TIMER_MSG:                 /**< �ظ�PC �·�ʱ��2 */
                        
                        #if(NODETYPE==COORD)
                        
                            addr = NLME_GetShortAddr();
                            Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_RF_MSG_CMD,addr,len,pBuf);
                        #elif(NODETYPE==ROUTER)
                            
                            RF_SendData(rfCmd,0X00,len,pBuf);
                        #else
                                
                        #endif
                            break;
                        case RF_P2D_TIMER_MSG:                  /**< PC�·�ʱ�� ???*/
                        case RF_D2P_2_TIMER_MSG:                /**< �ظ�PC �·�ʱ��2 */
                          
                            /**< ���绹û����֮ǰ�·�ʱ�䲻�ɹ� */
                            if(NetworkIsOk())
                            {
                                pP2EMsg = (RfPc2EndMsg_t*)pBuf;

                                RfPc2EndDTime.MsgType = rfCmd;
                                RfPc2EndDTime.TaskID  = pP2EMsg->TaskID;
                                RfPc2EndDTime.MsgLen  = 4;
                              
                                sysTimeSeconds = osal_getClock();
                                RfPc2EndDTime.Msg[0]  = (uint8)(sysTimeSeconds);	        /**< ʱ�� */
                                RfPc2EndDTime.Msg[1]  = (uint8)(sysTimeSeconds>>8);
                                RfPc2EndDTime.Msg[2]  = (uint8)(sysTimeSeconds>>16);
                                RfPc2EndDTime.Msg[3]  = (uint8)(sysTimeSeconds>>24); 
                                
                                Serial_vTx(SERIAL_PC_TX_CMD,COM_P2D_RF_MSG_CMD,addr,sizeof(RfPc2EndDTime_t),(uint8*)&RfPc2EndDTime);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
			break;
		//case 0x03~0x0F:
		default:
			break;
	}
}
/********************************************************************************************************

********************************************************************************************************/
