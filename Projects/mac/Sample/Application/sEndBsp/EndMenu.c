/********************************************************************************************************
*FileName:	EndMsgMenu.c
*Processor: SI1000
*Complier:	IAR
*Company:   
*********************************************************************************************************
*Author			Date				Comment
*NIWOTA			2014/1/25			Original
********************************************************************************************************/
#if 1
/********************************************************************************************************

********************************************************************************************************/
#include <string.h>	
#include "EndMenu.h"
#include "OSAL.h"
#include "HBeeApp.h"
/********************************************************************************************************

********************************************************************************************************/
__no_init MenuMsg_t MMsg;

/********************************************************************************************************

********************************************************************************************************/
void MENU_MsgBufInit(void)
{
    memset(&MMsg,0,sizeof(MenuMsg_t));
    
    MENU_ResetReadMsg();
}
/********************************************************************************************************
                                        返回可以存储数据的地址索引
********************************************************************************************************/
uint8 MENU_ReSortMsg(uint8 s)
{
    uint8 i,n;
    
    if(MMsg.Size > 0)
    {
        n = MMsg.Size-1;
        
        /**< 缓冲区满将缓冲最先接收到的清除 */
        for(i=s;i<n;i++)
        {
            MMsg.Buf[i] = MMsg.Buf[i+1];
        }
        if(s <= n)
        {
            MMsg.Size--;
        }
        MMsg.Buf[i].MsgType = RF_P2D_NONE_MSG;
        
        return i;
    }
    return 0xFF;
}
/********************************************************************************************************

********************************************************************************************************/
RfPc2EndMsg_t* MENU_AppendMsg(void)
{
    uint8 n;
    
    n = MMsg.Size;
    
    /**< 判断缓冲区是否满 */
    if(MMsg.Size >= MENU_MSG_SIZE)
    {
        /**< 缓冲区满将缓冲最先接收到的清除 */
        n = MENU_ReSortMsg(0);
    }
    MMsg.Size++;
 
    return &MMsg.Buf[n];
}
/********************************************************************************************************

********************************************************************************************************/
BOOL MENU_SearchOrRemoveMsg(RfPc2EndMsg_t *pMsg)
{
    uint8 i;
    
    if((pMsg->MsgType == RF_P2W_CALL_MSG)||(pMsg->MsgType == RF_P2W_SHORT_MSG))
    {
        /**< 接收到呼叫和短消息 */
        for(i=0;i<MMsg.Size;i++)
        {
            if((pMsg->MsgType == MMsg.Buf[i].MsgType)&&(pMsg->TaskID == MMsg.Buf[i].TaskID))
            {
                return TRUE;
            }
        }
    }
    else if((pMsg->MsgType == RF_P2W_CANCEL_CALL_MSG)||(pMsg->MsgType == RF_P2W_CANCEL_SHORT_MSG))
    {
        /**< 接收到取消呼叫和短消息 */
        for(i=0;i<MMsg.Size;i++)
        {
            if(((pMsg->MsgType == RF_P2W_CANCEL_CALL_MSG)&&(MMsg.Buf[i].MsgType == RF_P2W_CALL_MSG)&&(pMsg->TaskID == (MMsg.Buf[i].TaskID|0x80000000)))||
               ((pMsg->MsgType == RF_P2W_CANCEL_SHORT_MSG)&&(MMsg.Buf[i].MsgType == RF_P2W_SHORT_MSG)&&(pMsg->TaskID == MMsg.Buf[i].TaskID)))
            {
                MENU_ReSortMsg(i);
                return TRUE;
            }
        }
    }
    return FALSE;
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisPgdnKeyMsg(BOOL nAdd)
{
    uint8 i,n,len,of=0;
	
    if(nAdd == TRUE)
    {
		n = MMsg.Size;
		MMsg.DisStartPtr++;
	    if(MMsg.DisStartPtr >= n)
	    {
	        MMsg.DisStartPtr = 0;
	    }
    }
    else
    {
        n = MMsg.Size;
        if((MMsg.DisStartPtr+1) >= n)
        {
            MMsg.DisStartPtr = 0;
            MMsg.DisEndPtr = 0;
        }
    }

	MMsg.DisEndPtr = MMsg.DisStartPtr - (MMsg.DisStartPtr%3);

	/**< Page2---Page7清零 */
	MENU_ClearScreen(2,7);

	for(i=0;i<3;i++)
	{
		if(MMsg.Buf[MMsg.DisEndPtr].MsgType != RF_P2D_NONE_MSG)      /**<  最大为12，15标记指示为空 */
		{
            if(MMsg.Buf[MMsg.DisEndPtr].MsgLen > 16)
            {
                len = 16;
            }
            else
            {
                len = MMsg.Buf[MMsg.DisEndPtr].MsgLen;
            }

            if((MMsg.DisStartPtr%3) == i)
            {
                of = 1;
            }
			else
			{
				of = 0;
			}
			/**< 显示数据队列 */
            MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,i,0,of); 
			MMsg.DisEndPtr++;
		}
		else
		{
			/**< 显示“空” */
			MENU_ShowString(KONG_MSG,2,i,0,0);
			break;
		}
	}
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisRefreshMsg(void)
{
    uint8 i,len,m=0,of=0;
    
	m = MMsg.DisEndPtr%3;
	if((m == 0)&&(MMsg.DisEndPtr > 0))
	{
		/**< 显示页满 */
		return;
	}

	MMsg.DisEndPtr = MMsg.DisEndPtr - (MMsg.DisEndPtr%3);
	
	/**< Page2---Page7清零 */
	MENU_ClearScreen(2,7);

	for(i= 0;i< 3;i++)
	{
		if(MMsg.Buf[MMsg.DisEndPtr].MsgType != RF_P2D_NONE_MSG)
		{
            if(MMsg.Buf[MMsg.DisEndPtr].MsgLen > 16)
            {
                len = 16;
            }
            else
            {
                len = MMsg.Buf[MMsg.DisEndPtr].MsgLen;
            }

            if((MMsg.DisStartPtr%3) == i)
            {
                of = 1;
            }
			else
			{
				of = 0;
			}

			/**< 显示数据队列 */
			MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,i,0,of); 
			MMsg.DisEndPtr++;
		}
		else
		{
			/**< 显示“空” */
			MENU_ShowString(KONG_MSG,2,i,0,0);
			break;
		}
	}
}
/********************************************************************************************************

********************************************************************************************************/
uint8 MENU_DisOpenKeyMsg(void)
{
	uint8 len;
	uint8 *pbuf;
	uint8 reBack = FALSE;

    if((MMsg.DisStartPtr > MENU_MSG_SIZE)||(MMsg.DisEndPtr > MENU_MSG_SIZE))
    {
        MMsg.DisStartPtr = 0;  
        MMsg.DisEndPtr = 0;
    }
    
    if(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2D_NONE_MSG)
	{
		/**< 短消息 */
		if(1)//(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2W_CALL_MSG)
		{
			/**< Page2---Page7清零 */
			MENU_ClearScreen(2,7);
			
	        /**< 获取数据长度和数据指针 */
	        len   	= MMsg.Buf[MMsg.DisStartPtr].MsgLen;
	        pbuf 	= MMsg.Buf[MMsg.DisStartPtr].Msg;
	        
            /**< 显示数据队列 */
			MENU_ShowString(pbuf,len,0,0,0); 

			reBack = TRUE;
		}
		else
		{
#if 0
			/**< Page2---Page7清零 */
			MENU_ClearScreen(2,7);
			
	        /**< 获取数据长度和数据指针 */
	        len   	= MMsg.Buf[MMsg.DisStartPtr].MsgLen;
	        pbuf 	= MMsg.Buf[MMsg.DisStartPtr].Msg;
            
            /**< 显示数据队列 */
			MENU_ShowString(pbuf,len,0,0,0); 
			
			reBack = TRUE;
#else
			/**< 呼叫信息不能打开 */
			reBack = FALSE;
#endif
		}
	}
#if 0
	else
	{
		/**< Page2---Page7清零 */
		MENU_ClearScreen(2,7);
		
		/**< 显示“空” */
		MENU_ShowString(KONG_MSG,2,1,0,0);
		/**< 显示“空” */
		MENU_ShowString(KONG_MSG,2,1,1*16,0);
		/**< 显示“空” */
		MENU_ShowString(KONG_MSG,2,1,4*16,0);
        /**< 显示“空” */
		MENU_ShowString(KONG_MSG,2,1,5*16,0);
				
		reBack = TRUE;
	}
#endif
	return reBack;
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSendKeyMsg(void)
{
	uint8 n;
    
    n = MMsg.Size;

    /**< 删除当前打开信息 */
    MENU_ReSortMsg(MMsg.DisStartPtr);
    
    if((MMsg.DisStartPtr+1) >= n)
    {
        MMsg.DisStartPtr = 0;
        MMsg.DisEndPtr = 0;
    }

    MENU_DisPgdnKeyMsg(FALSE);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ResetWriterMsg(void)
{
#if 0
	uint8 i,x,n;

	ENTER_CRITICAL(x);

	n = MMsg.Size;

	//osal_nv_item_init(WRITER_NUM_MSG_ADDR,1,NULL);
	//osal_nv_write(WRITER_NUM_MSG_ADDR,0,1,&n);

	for(i=0;i<n;i++)
	{
        //osal_nv_item_init(WRITER_RESET_MSG_ADDR+(i*ONE_MSG_LENGTH),ONE_MSG_LENGTH,NULL);
        //osal_nv_write(WRITER_RESET_MSG_ADDR+(i*ONE_MSG_LENGTH),0,ONE_MSG_LENGTH, pBuffer->Body);
	}
	
	LEAVE_CRITICAL(x);
#endif
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ResetReadMsg(void)
{
#if 0
	uint8 i,n;
    
	/**< 读出最后操作信息的位置 */
	//osal_nv_item_init(WRITER_NUM_MSG_ADDR,1,NULL);
	//osal_nv_read(WRITER_NUM_MSG_ADDR,0,1,&n);

    if(n == 0xFF)
    {
        n = 0;
    }
	for(i=0;i<n;i++)
	{
        //osal_nv_item_init(0x1000+(i*ONE_MSG_LENGTH),ONE_MSG_LENGTH,NULL);
        //osal_nv_read(0x1000+(i*ONE_MSG_LENGTH),0,ONE_MSG_LENGTH,pBuffer->Body);
	}
	
	if(n > 0)
	{
		n = 0;
		//osal_nv_item_init(WRITER_NUM_MSG_ADDR,1,NULL);
		//osal_nv_write(WRITER_NUM_MSG_ADDR,0,1,&n);

	    MENU_DisRxMsg();
	}
	else
#endif
	{
		MENU_DisPgdnKeyMsg(FALSE);
	}

}
/********************************************************************************************************
                                    界面显示函数
********************************************************************************************************/
void LCD_DisOffMsg(void)
{
    if(!IsLcdSleep())
    {
        LCD_SetWork(FALSE);
    }
}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisListMsg(void)
{
    if(IsLcdSleep())
    {
        LCD_SetWork(TRUE);
        return;
    }
    
    MENU_DisPgdnKeyMsg(TRUE);
}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisOpenMsg(void)
{
	if(MENU_DisOpenKeyMsg())
	{

	}  
}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisLowPower(void)
{
	uint8 n = 12;
	
	MENU_ClearScreen(2,7);					           		/**< Page2---Page7清零 */
	MENU_ShowString(lOW_POWER_MSG,n,1,0,0); 				/**< 显示电池电量低 */
	osal_set_event(HBee_TaskID,HBEEAPP_MOTOR_ON_EVT);
	osal_set_event(HBee_TaskID,HBEEAPP_OLED_ON_EVT);
}
/********************************************************************************************************

********************************************************************************************************/
void LCD_DisIncPower(void)
{
	uint8 n = 10;
	
	MENU_ClearScreen(2,7);						           	/**< Page2---Page7清零 */
	MENU_ShowString(INC_POWER_MSG,n,1,0,0); 				/**< 显示电池电量低 */
	osal_stop_timerEx(HBee_TaskID,HBEEAPP_LOW_BATTERY_EVT);
}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisUserPassword(void)
{
	uint8 n;
	uint8 *p;
	MENU_ClearScreen(2,7);						                /**< Page2---Page7清零 */
	n =8;
    p = USER_SET_MSG;
	MENU_ShowString(p,n,0,32,0);
	n =11;
    p = ENTER_PASSWORD_MSG;
	MENU_ShowString(p,n,1,0,0); 
	n = 4;
    p = USER_PASSWORD_MSG;
	MENU_ShowString(p,n,2,96,0);
}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisUserSetDisplyDly(void)
{

}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisUserSetMsgTipDly(void)
{

}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisUserSetKeyMotor(void)
{

}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisEnterPassword(void)
{

}
/********************************************************************************************************

********************************************************************************************************/
void LCD_DisUserSetPassword(void)
{

}
/********************************************************************************************************

********************************************************************************************************/
void  LCD_DisSysPassword(void)
{
  
}
/********************************************************************************************************

********************************************************************************************************/
#endif

