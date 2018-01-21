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
        ComTxBuf[n++] = eComCmd;                  /**<  串口命令 */
        /**> 这里违背了低地址先发的原则，上位机调整过来了，这里就不改 */
        //ComTxBuf[n++] = (uint8)(addr >>8);        /**<  设备地址高8位 */
        //ComTxBuf[n++] = (uint8)addr;              /**<  设备地址低8位 */
        
        ComTxBuf[n++] = (uint8)(addr);              /**<  设备地址低8位 */
        ComTxBuf[n++] = (uint8)(addr>>8);           /**<  设备地址高8位 */
        for(i=0;i<len;i++)
        {
            ComTxBuf[n++] = pBuf[i];             /**<  数据包 */
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

	dCmd = *pBuf++;			/**<  串口数据命令 */
	addr = *pBuf++;			/**<  接收地址低8位 */
	addr2 = *pBuf++;		/**<  接收地址高8位 */
	
    addr |= (addr2<<8)&0xFF00;
    
    len = len - 3;
    /**< dCmd + addr*2 + pBuf[x] */
    
    //HalUARTWrite(HAL_UART_PORT_0,pBuf,len);
    //return;
    
    rfCmd = pBuf[0];
    
	switch(dir)
	{
		case SERIAL_PC_TX_CMD:  /** < PC串口下发 */
            switch(dCmd)
            {
                case COM_P2D_RF_MSG_CMD:
                
                    switch(rfCmd)
                    {
                        case RF_P2W_CALL_MSG:                   /**< PC下发呼叫信息 */
                        case RF_P2W_CANCEL_CALL_MSG:            /**< PC下发取消呼叫信息 */
                        case RF_P2W_SHORT_MSG:                  /**< PC下发短信息 */
                        case RF_P2W_CANCEL_SHORT_MSG:           /**< PC下发取消短信息 */
                        
                        case RF_P2L_ONOFF_MSG:
                        case RF_P2J_COUNTER_MSG:
                        
                        case RF_P2W_OPEN_SHORT_MSG:             /**< PC 获取被打开短信息的 */
                        case RF_P2D_TIMER_MSG:                  /**< PC下发时间 */
                        case RF_D2P_2_TIMER_MSG:
                        //case RF_P2C_HEART_BEAT_MSG:             /**< PC 获取协调器心跳 */
                        //case RF_P2R_HEART_BEAT_MSG:             /**< PC 获取路由器心跳 */
                        //case RF_P2W_HEART_BEAT_MSG:             /**< PC 获取手表器心跳 */
                        //case RF_P2E_HEART_BEAT_MSG:             /**< PC 获取呼叫器心跳 */
                        //case RF_P2E_MSG:                        /**< PC 获取呼叫器信息 */
  
                        /**< 发送给从设备的信息命令 */
                        case (RF_P2W_CALL_MSG|0x80):            /**< PC下发呼叫信息 */
                        case (RF_P2W_CANCEL_CALL_MSG|0x80):     /**< PC下发取消呼叫信息 */
                        case (RF_P2W_SHORT_MSG|0x80):           /**< PC下发短信息 */
                        case (RF_P2W_CANCEL_SHORT_MSG|0x80):    /**< PC下发取消短信息 */
                        case (RF_P2L_ONOFF_MSG|0x80):
                        case (RF_P2J_COUNTER_MSG|0x80):
                        
                            if(len <= sizeof(RfPc2EndMsg_t))
                            {
                                /**< 发给从模块数据 */
                                if(addr == msa_CoordShortAddr)
                                {
                                    if(rfCmd&0x80)
                                    {
                                        if(len <= sizeof(RfPc2EndMsg_t))
                                        {
                                            pP2EMsg = (RfPc2EndMsg_t*)pBuf;
                                            if(pP2EMsg->MsgLen >= 3)
                                            {
                                                pP2EMsg->MsgLen -= 2;       /**< 去除最终地址长度 */
                                                len             -= 2;       /**< 去除数据总长度 */
                                                pP2EMsg->MsgType&= 0x7F;    /**< 去除0x80 */
                                                
                                                addr    = pP2EMsg->Msg[pP2EMsg->MsgLen+1];
                                                addr  <<=8;
                                                addr   |= pP2EMsg->Msg[pP2EMsg->MsgLen];
      
                                                Serial_vTx(SERIAL_PC_TX_CMD,COM_P2D_RF_MSG_CMD,addr,len,pBuf);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        /**< 串口发送给协调器的数据 */
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
											pP2EMsg->MsgLen -= 2;		/**< 去除最终地址长度 */
											len 			-= 2;		/**< 去除数据总长度 */
											pP2EMsg->MsgType&= 0x7F;	/**< 去除0x80 */
											
											addr	= pP2EMsg->Msg[pP2EMsg->MsgLen+1];
											addr  <<=8;
											addr   |= pP2EMsg->Msg[pP2EMsg->MsgLen];
										}
										else
										{
											break;
										}
									}

                                    /**< 这里接收到从模块的数据不能转发 */
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
				    APP_SearilSetAddr(len,pBuf);            /**< 串口设置地址 */
                    break;
                case COM_P2D_RF_SET_ADDR_CMD:
                #if defined(SET_ADDR_DECEIVE)
				    RF_SetAddr(len,pBuf);                   /**< 无线转发设置地址 */
			    #endif
                    break;
                case COM_P2D_GET_NWK_TOPOLOGY_CMD:
				    APP_SearilGetTopologyStruct(len,pBuf);  /**< 获取网络拓扑数据 */ 
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
		case SERIAL_PC_RX_CMD: /** < 串口上发 */
            /**< 路由接收到转发给协调器 */
            switch(dCmd)
            {
                case COM_D2P_RF_MSG_CMD:
                
                    switch(rfCmd)
                    {
                        case RF_W2P_CALL_MSG:                   /**< 回复PC下发呼叫信息 */
                        case RF_W2P_CANCEL_CALL_MSG:            /**< 回复PC下发取消呼叫信息 */
                        case RF_W2P_SHORT_MSG:                  /**< 回复PC下发短信息 */
                        case RF_W2P_CANCEL_SHORT_MSG:           /**< 回复PC下发取消短信息 */
                        case RF_W2P_OPEN_SHORT_MSG:             /**< 读取短消息发送信息 */
                        //case RF_D2P_TIMER_MSG:					/**< 回复PC下发时间 */
                        //case RF_C2P_HEART_BEAT_MSG:             /**< 协调器心跳信息 */    
                        //case RF_R2P_HEART_BEAT_MSG:             /**< 路由器心跳信息 */
                        case RF_W2P_HEART_BEAT_MSG:             /**< 手表器心跳信息 */    
                        case RF_E2P_HEART_BEAT_MSG:             /**< 呼叫器心跳信息 */
                        case RF_L2P_HEART_BEAT_MSG:             /**< 灯心跳 */
                        case RF_J2P_HEART_BEAT_MSG:             /**< 计数器心跳 */
                        case RF_E2P_MSG:                        /**< 呼叫器信息 */
                        case RF_L2P_ONOFF_MSG:                  /**< PC发送控制灯 */
                        case RF_J2P_COUNTER_MSG:                /**< PC 发送计算值 */
                        //case RF_D2P_2_TIMER_MSG:                 /**< 回复PC 下发时间2 */
                        
                        #if(NODETYPE==COORD)
                        
                            addr = NLME_GetShortAddr();
                            Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_RF_MSG_CMD,addr,len,pBuf);
                        #elif(NODETYPE==ROUTER)
                            
                            RF_SendData(rfCmd,0X00,len,pBuf);
                        #else
                                
                        #endif
                            break;
                        case RF_P2D_TIMER_MSG:                  /**< PC下发时间 ???*/
                        case RF_D2P_2_TIMER_MSG:                /**< 回复PC 下发时间2 */
                          
                            /**< 网络还没建立之前下发时间不成功 */
                            if(NetworkIsOk())
                            {
                                pP2EMsg = (RfPc2EndMsg_t*)pBuf;

                                RfPc2EndDTime.MsgType = rfCmd;
                                RfPc2EndDTime.TaskID  = pP2EMsg->TaskID;
                                RfPc2EndDTime.MsgLen  = 4;
                              
                                sysTimeSeconds = osal_getClock();
                                RfPc2EndDTime.Msg[0]  = (uint8)(sysTimeSeconds);	        /**< 时间 */
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
