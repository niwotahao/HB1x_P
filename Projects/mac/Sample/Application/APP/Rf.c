/********************************************************************************************************
*FileName:	rf.c
*Processor: PIC18F67J11+AT86RF212
*Complier:	MCC18 | HI-TECH18
*Company:   WWW.OURZIGBEE.COM
*********************************************************************************************************
*Author			Date				Comment
*NIWOTA			2010/8/28			Original
*NIWOTA			2010/9/13			 ����Դ
********************************************************************************************************/
#include "Rf.h"
#include "Serial.h"
#include "msa.h"

#if(NODETYPE == ENDDEVICE)
    #include "menu.h"
    #include "OSAL.h"
    #include "OSAL_Nv.h"
#endif
#if 0
/********************************************************************************************************

********************************************************************************************************/
HalPib_t *pHalPib;

/********************************************************************************************************

********************************************************************************************************/
void RF_GetRadioConfigMsg(void)
{
    //EEPRAM_ReadShortAddr((uint8*)pHalPib,sizeof(HalPib_t));
    if((pHalPib->Channel == 0xFF)||(pHalPib->NetID == 0xFFFF))
    {
        pHalPib->NetID 	    = 0x2014;
        pHalPib->PANID 	    = 0x1234;
        
    #if(NODETYPE == COORD)
        pHalPib->ExtAddr	= 0;
    #elif(NODETYPE == ROUTER)
        pHalPib->ExtAddr	= 2;
    #else
        pHalPib->ExtAddr	= 0x0A;
    #endif
        
        pHalPib->TxPower	= 0;
        pHalPib->DataRate	= 0;
        
    #if defined(HI_TECH_C)
        pHalPib->Channel	= 1;
    #else
        pHalPib->Channel	= 16;
    #endif
    
        pHalPib->NodeType	= 0;
        pHalPib->Random	    = 0xFF;
        pHalPib->TxCnt 	    = 0x03;
        pHalPib->BindNum    = 4;
    }
}
#endif
/********************************************************************************************************

********************************************************************************************************/
#if(NODETYPE == ENDDEVICE)

#if defined(WATCH_DISPLY) 
/********************************************************************************************************

********************************************************************************************************/
void RF_EndReceiveMsg(uint8 len,uint8 *pBuf)
{
    uint8 i,*p;
    RfPc2EndMsg_t *pSaMsg,*pRxMsg;
    
    if(len <= sizeof(RfPc2EndMsg_t))
    {
        pRxMsg = (RfPc2EndMsg_t*)pBuf;
        
        /**< ������λ�����͵���Ϣ�ظ� */
        HBee_EndRelpyMsg(pRxMsg);

        /**< ����  */
        if((pRxMsg->Msg[0] == 0xA1)&&(pRxMsg->Msg[1] == 0xF9))
        {
            if(len > sizeof(RfPc2EndAreaMsg_t) )
            {
                len = sizeof(RfPc2EndAreaMsg_t);
            }
            p = (uint8*)&RfPc2EndAreaMsg;
            for(i=0;i<len;i++)
            {
                *(p++) = *(pBuf++);
            }
            osal_nv_item_init(WRITER_AREA_NAME_MSG_ID,sizeof(RfPc2EndAreaMsg_t),NULL);
            osal_nv_write(WRITER_AREA_NAME_MSG_ID,0,sizeof(RfPc2EndAreaMsg_t),(uint8*)&RfPc2EndAreaMsg);
            
            /**<  ת����ʾ */
            HBee_SetUserEvent(HB_SET_BSP_EVENT,BSP_KEY_LONG_OPEN_REPLY);
            /**< �ֱ������� */
            return;
        }

        pSaMsg = MENU_SearchOrRemoveMsg(pRxMsg); 
        if( pSaMsg == NULL)
        {
            p = (uint8*)MENU_AppendMsg();
        }
        else
        {
            p = (uint8*)pSaMsg;
        }
        
        if(p != NULL)
        {
            for(i=0;i<len;i++)
            {
                *(p++) = *(pBuf++);
            }
            HBee_SetUserEvent(HB_RF_RX_IND,BSP_KEY_LONG_OPEN_REPLY);
        }
    }
}
/********************************************************************************************************

********************************************************************************************************/
void RF_EndReceiveCancelMsg(uint8 len,uint8 *pBuf)
{
#if 0
    if(len <= sizeof(RfPc2EndMsg_t))
    {
        /**< ������λ�����͵���Ϣ�ظ� */
        HBee_EndRelpyMsg((RfPc2EndMsg_t*)pBuf);
        
        if(MENU_SearchOrRemoveMsg((RfPc2EndMsg_t*)pBuf) != NULL)
        {
            HBee_SetUserEvent(HB_RF_RX_CANCEL_IND,BSP_KEY_LONG_OPEN_REPLY);
        }
    }
#else
    uint8 n,i;
    uint16 KeyId,m;
    uint32 tmp;
    RfPc2EndMsg_t *pRfPc2EndMsg;
        
    if(len <= sizeof(RfPc2EndMsg_t))
    {
        /**< ������λ�����͵���Ϣ�ظ� */
        HBee_EndRelpyMsg((RfPc2EndMsg_t*)pBuf);

        pRfPc2EndMsg = (RfPc2EndMsg_t*)pBuf;
        KeyId  = (uint16)((pRfPc2EndMsg->TaskID&0xFFFF0000)>>16);
        if((KeyId&0xC000)==0x8000)
        {
            /**<  ����ȡ�� */
            if(MENU_SearchOrRemoveMsg((RfPc2EndMsg_t*)pBuf) != NULL)
            {
                HBee_SetUserEvent(HB_RF_RX_CANCEL_IND,BSP_KEY_LONG_OPEN_REPLY);
            }
        }
        else if((KeyId&0xC000)==0xC000)
        {
#if 0
            /**<  ȫ��ȡ�� */
            KeyId &= 0x3FFF;                        /**< ��ȡ��ֵλ */
            m = 0x2000;                             /**< ����󰴼�Ϊ��ʼ��ѯ */
            for(i=13;i>0;i--)                       /**< ����������һ���� */
            {
                if((KeyId&m) == m)                  /**< �ж��Ƿ񰴼�λΪ1 */
                {
                    n = i+1;                        /**< ������1��ʼ */
                    break;
                }
                m >>=1;
            }
            m = 0;
            for(i=1;i<=n;i++)
            {
                tmp   = i;
                tmp <<= 16;
                pRfPc2EndMsg->TaskID &= 0x0000FFFF;
                pRfPc2EndMsg->TaskID |= 0x80000000;
                pRfPc2EndMsg->TaskID |= tmp;
                if(MENU_SearchOrRemoveMsg((RfPc2EndMsg_t*)pBuf) != NULL)
                {
                    /**< ��ֹ���ɾ����Ļ */
                    if(m == 0)
                    {
                        m =1;
                        HBee_SetUserEvent(HB_RF_RX_CANCEL_IND,BSP_KEY_LONG_OPEN_REPLY);
                    }
                }
            }
#else
            /**<  ȫ��ȡ�� */
            KeyId &= 0x3FFF;                        /**< ��ȡ��ֵλ */
            m = 0x2000;                             /**< ����󰴼�Ϊ��ʼ��ѯ */
            n = 0;
            for(i=14;i>=1;i--)                      /**< ������1��ʼ */
            {
                if((KeyId&m) == m)                  /**< �ж��Ƿ񰴼�λΪ1 */
                {                      
                    tmp   = i;                    
                    tmp <<= 16;
                    pRfPc2EndMsg->TaskID &= 0x0000FFFF;
                    pRfPc2EndMsg->TaskID |= 0x80000000;
                    pRfPc2EndMsg->TaskID |= tmp;
                    if(MENU_SearchOrRemoveMsg((RfPc2EndMsg_t*)pBuf) != NULL)
                    {
                        /**< ��ֹ���ɾ����Ļ */
                        if(n == 0)
                        {
                            n =1;
                            HBee_SetUserEvent(HB_RF_RX_CANCEL_IND,BSP_KEY_LONG_OPEN_REPLY);
                        }
                    }
                }
                m >>=1;                             /**< ��ѯ��һ������λ */
            } 
#endif
        }
        else
        {
            /**<  ���� */
            /**<  Ϊ���� */
        }
    }
#endif
}
#elif defined(LED_AlARM_DISPLY)
/********************************************************************************************************

********************************************************************************************************/
void RF_LedReceiveOnOffMsg(uint8 len,uint8 *pBuf)
{
    uint8 mode,light;
    Rf2PcMsg_t *pRf2PcMsg;
    
    pRf2PcMsg = (Rf2PcMsg_t*)pBuf;
    if(len <= sizeof(Rf2PcMsg_t))
    {
        mode  = pRf2PcMsg->Msg[0];
        light = pRf2PcMsg->Msg[1];
        if(mode == 0)
        {
            P0 = light;
        }
        else if(mode == 1)
        {
            P0 = 1<<light;
        }
        else
        {
        
        }
        HBee_LedRelpyMsg((RfPc2EndMsg_t*)pBuf); 
    } 
}
#elif defined(JSQ_DECEIVE)
/********************************************************************************************************

********************************************************************************************************/
void RF_JsqReceiveCounterMsg(uint8 len,uint8 *pBuf)
{
    uint16 jsz;
    Rf2PcMsg_t *pRf2PcMsg;
    
    pRf2PcMsg = (Rf2PcMsg_t*)pBuf;
    if(len < sizeof(RfPc2EndDTime_t))
    {
        jsz = pRf2PcMsg->TaskID>>16;

        HBee_JsqRelpyMsg((RfPc2EndMsg_t*)pBuf); 
    } 
}
#endif

/********************************************************************************************************

********************************************************************************************************/
void RF_EndReceiveTimeMsg(uint8 len,uint8 *pBuf)
{
    uint32 sysTimeSeconds;
    
    if(len <= sizeof(RfPc2EndDTime_t))
    {
        sysTimeSeconds = pBuf[9];
        sysTimeSeconds <<= 8;
        sysTimeSeconds |= pBuf[8];
        sysTimeSeconds <<= 8;
        sysTimeSeconds |= pBuf[7];
        sysTimeSeconds <<= 8;
        sysTimeSeconds |= pBuf[6];
        
        osal_setClock(sysTimeSeconds);
        
        GetTimerflg = 2;
        osal_set_event(MSA_TaskId,HBEEAPP_SHOW_TIME_EVT);
    } 
}

#endif

#if(NODETYPE == ROUTER)
/********************************************************************************************************

********************************************************************************************************/
extern uint8 GetTimerflg;
void RF_RouterReceiveTimeMsg(uint8 len,uint8 *pBuf)
{
    uint32 sysTimeSeconds;
    
    if(len <= sizeof(RfPc2EndDTime_t))
    {
        sysTimeSeconds = pBuf[9];
        sysTimeSeconds <<= 8;
        sysTimeSeconds |= pBuf[8];
        sysTimeSeconds <<= 8;
        sysTimeSeconds |= pBuf[7];
        sysTimeSeconds <<= 8;
        sysTimeSeconds |= pBuf[6];
        
        osal_setClock(sysTimeSeconds);
        
        GetTimerflg = 2;
    } 
}
/********************************************************************************************************

********************************************************************************************************/
void RF_RouterReceiveMsg(uint8 len,uint8 *pBuf)
{
    RfPc2EndMsg_t *pRxMsg;
    
    pRxMsg = (RfPc2EndMsg_t*)pBuf;
    
    if((pRxMsg->MsgType == RF_P2W_CALL_MSG)||(pRxMsg->MsgType == RF_P2W_CANCEL_CALL_MSG))
    {
        
        /**< ������λ�����͵���Ϣ�ظ� */
        HBee_RouterRelpyMsg(pRxMsg);
        
        
        
        /**< ·�ɴ���ʾ�����·��ĺ�����Ϣ��ȡ��������Ϣת���ɺ�������Ϣ */
        pRxMsg->MsgType = RF_E2P_MSG;
        Serial_vTx(SERIAL_PC_RX_CMD,COM_P2D_RF_MSG_CMD,0x00,len,pBuf);
    }
}
#endif
/********************************************************************************************************

********************************************************************************************************/
void RF_SetAddr(uint8 len,uint8 *pBuf)
{
}
/********************************************************************************************************

********************************************************************************************************/
void RF_SetAddrCallBack(void)
{   
}
/********************************************************************************************************
********************************************************************************************************/
void APP_HeatIndCallBack(uint16 addr,uint8 len,uint8 *pBuf)
{
    /**< ����������PC */
    Serial_vTx(SERIAL_PC_RX_CMD,COM_P2D_RF_MSG_CMD,addr,len,pBuf);
}
/********************************************************************************************************

********************************************************************************************************/
void APP_DataIndCallBack(uint16 addr,uint8 len,uint8 *pBuf)
{
    uint8 cmd 		= *pBuf++;		/**< �û����������� */   
    uint8 dLen		= *pBuf++; 		/**< �û������ݳ��� */
    
    if(len == (dLen+2))
	{
        len = dLen; 
        
		switch(cmd)
        {
        #if(NODETYPE == ENDDEVICE)
#if defined(WATCH_DISPLY) 
            case RF_P2W_CALL_MSG:                	/**< PC�·�������Ϣ */
            case RF_P2W_SHORT_MSG:               	/**< PC�·�����Ϣ */
                RF_EndReceiveMsg(len,pBuf);
                break;
            case RF_P2W_CANCEL_CALL_MSG:         	/**< PC �·�ȡ��������Ϣ */
            case RF_P2W_CANCEL_SHORT_MSG:        	/**< PC�·�ȡ������Ϣ */
                RF_EndReceiveCancelMsg(len,pBuf);
                break;
#elif defined(LED_AlARM_DISPLY)
            case RF_P2L_ONOFF_MSG:              	/**< PC ���ͼ���ֵ */
				RF_LedReceiveOnOffMsg(len,pBuf);
                break;
#elif defined(JSQ_DECEIVE)
            case RF_P2J_COUNTER_MSG:            	/**< PC���Ϳ��Ƶ� */
				RF_JsqReceiveCounterMsg(len,pBuf);
                break;
#endif
            case RF_P2D_TIMER_MSG:               	/**< PC�·�ʱ�� */
            case RF_P2D_2_TIMER_MSG:         		/**< PC �·�ʱ��2 */
                RF_EndReceiveTimeMsg(len,pBuf);
                break;
            /**< -------------------------------------------------------------- */  
        #elif(NODETYPE == ROUTER)
            case RF_P2W_CALL_MSG:                	/**< PC�·�������Ϣ */
            case RF_P2W_SHORT_MSG:               	/**< PC�·�����Ϣ */
            case RF_P2W_CANCEL_CALL_MSG:         	/**< PC �·�ȡ��������Ϣ */
            case RF_P2W_CANCEL_SHORT_MSG:        	/**< PC�·�ȡ������Ϣ */
                RF_RouterReceiveMsg(len,pBuf);
                break;
            case RF_P2D_TIMER_MSG:               	/**< PC�·�ʱ�� */
            case RF_P2D_2_TIMER_MSG:         		/**< PC �·�ʱ��2 */
                RF_RouterReceiveTimeMsg(len,pBuf);
                break; 
            /**< -------------------------------------------------------------- */     
        #elif(NODETYPE == COORD)
            case RF_W2P_CALL_MSG:                   /**< �ظ�PC�·�������Ϣ */
            case RF_W2P_CANCEL_CALL_MSG:            /**< �ظ�PC�·�ȡ��������Ϣ */
            case RF_W2P_SHORT_MSG:                  /**< �ظ�PC�·�����Ϣ */
            case RF_W2P_CANCEL_SHORT_MSG:           /**< �ظ�PC�·�ȡ������Ϣ */
            case RF_W2P_OPEN_SHORT_MSG:             /**< ��ȡ����Ϣ������Ϣ */
			case RF_D2P_TIMER_MSG:					/**< �ظ�PC�·�ʱ�� */
            case RF_C2P_HEART_BEAT_MSG:             /**< Э����������Ϣ */    
            case RF_R2P_HEART_BEAT_MSG:             /**< ·����������Ϣ */
            case RF_W2P_HEART_BEAT_MSG:             /**< �ֱ���������Ϣ */    
            case RF_E2P_HEART_BEAT_MSG:             /**< ������������Ϣ */
            case RF_L2P_HEART_BEAT_MSG:             /**< ������ */
            case RF_J2P_HEART_BEAT_MSG:             /**< ���������� */
            case RF_E2P_MSG:                        /**< ��������Ϣ */
            case RF_L2P_ONOFF_MSG:                  /**< PC���Ϳ��Ƶ� */
            case RF_J2P_COUNTER_MSG:                /**< PC ���ͼ���ֵ */
            case RF_D2P_2_TIMER_MSG:                 /**< �ظ�PC �·�ʱ��2 */
            
                Serial_vTx(SERIAL_PC_RX_CMD,COM_D2P_RF_MSG_CMD,addr,len,pBuf);
                break;
        #endif
            default:
    
                break;	
        }
	}
	len = len;
}
/********************************************************************************************************

********************************************************************************************************/
void APP_DataCnfCallBack(uint8 state,void *m)
{
#if 0
	uint8 cmd;
	ZFrameInfo_t *pFrameInfo = (ZFrameInfo_t*)BMM_BUFFER_POINTER((Buffer_t *)m);
	
	cmd = *pFrameInfo->psdu;                        /**< �û����������� */
	if(state == ZSUCCESS)
    {
        switch(cmd)
        {
        #if(NODETYPE == COORD)
            case RF_W2P_CALL_MSG:                   /**< �ظ�PC�·�������Ϣ */
                break;
            case RF_W2P_CANCEL_CALL_MSG:            /**< �ظ�PC�·�ȡ��������Ϣ */
                break;
            case RF_W2P_SHORT_MSG:                  /**< �ظ�PC�·�����Ϣ */
                break;
            case RF_W2P_CANCEL_SHORT_MSG:           /**< �ظ�PC�·�ȡ������Ϣ */
                break;
            case RF_W2P_OPEN_SHORT_MSG:             /**< ��ȡ����Ϣ������Ϣ */
                break;
			case RF_D2P_TIMER_MSG:					/**< �ظ�PC�·�ʱ�� */
                break;
            case RF_C2P_HEART_BEAT_MSG:             /**< Э����������Ϣ */    
                break;
            case RF_R2P_HEART_BEAT_MSG:             /**< ·����������Ϣ */
                break;
            case RF_W2P_HEART_BEAT_MSG:             /**< �ֱ���������Ϣ */  
                break;
            case RF_E2P_HEART_BEAT_MSG:             /**< ������������Ϣ */
                break;
            case RF_E2P_MSG:                        /**< ��������Ϣ */
                break;
        /**< -------------------------------------------------------------- */
        #elif(NODETYPE == ENDDEVICE)
            case RF_P2W_CALL_MSG:                /**< PC�·�������Ϣ */
                break;
            case RF_P2W_CANCEL_CALL_MSG:         /**< PC �·�ȡ��������Ϣ */
                break;
            case RF_P2W_SHORT_MSG:               /**< PC�·�����Ϣ */
                break;
            case RF_P2W_CANCEL_SHORT_MSG:        /**< PC�·�ȡ������Ϣ */
                break;
            case RF_P2W_OPEN_SHORT_MSG:          /**< PC ��ȡ���򿪶���Ϣ�� */
                break;
            case RF_P2D_TIMER_MSG:               /**< PC�·�ʱ�� */
                break;
            case RF_P2C_HEART_BEAT_MSG:          /**< PC ��ȡЭ�������� */
                break;
            case RF_P2R_HEART_BEAT_MSG:          /**< PC ��ȡ·�������� */
                break;
            case RF_P2W_HEART_BEAT_MSG:          /**< PC ��ȡ�ֱ������� */
                break;
            case RF_P2E_HEART_BEAT_MSG:          /**< PC ��ȡ���������� */
                break;
            case RF_P2E_MSG:                     /**< PC ��ȡ��������Ϣ */
                break;
        #endif
            default:
                break;	
        }
    }
	state = state;	
#else
	m = m;
	state = state;
#endif
}
/********************************************************************************************************

********************************************************************************************************/


