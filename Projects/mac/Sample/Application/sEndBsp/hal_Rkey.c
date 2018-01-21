/********************************************************************************************************
*FileName:	main.c
*Processor: PIC18F67J11+AT86RF212
*Complier:	MCC18 | HI-TECH18
*Company:   WWW.OURZIGBEE.COM
*********************************************************************************************************
*Author			Date				Comment
*NIWOTA			2010/8/28			Original
*NIWOTA			2010/9/13			 丰利源
********************************************************************************************************/
#include "msa.h"
#include "hal_Rkey.h"
#include "OSAL.h"
/********************************************************************************************************
*                                      界面显示和界面切换使用变量
********************************************************************************************************/ 

/********************************************************************************************************
*                                           读取按键所用变量
********************************************************************************************************/ 
uint8 KeyCut = 0;
uint8 KeyFlg = 0;
uint8 KeyBuf = KEY_NO;
uint8 KeyShortBuf = KEY_NO;
uint8 KeyLongBuf = KEY_NO;
uint8 KeyLongTime = FIRST_LONG_KEY_CNT;
/********************************************************************************************************
*
********************************************************************************************************/  
void KEY_Init(void)
{
	//KEY_PORT_INIT();
}
/********************************************************************************************************
*
********************************************************************************************************/
BspEvent_t KEY_Read(void)
{
	uint8 key = KEY_NO;
    BspEvent_t event = BSP_NONE;
	   
	KeyBuf 	= KEY_PORT();
	
	if(KeyBuf == KEY_NO)
	{
		KeyLongBuf 	= KeyBuf;
		KeyLongTime = FIRST_LONG_KEY_CNT;				/**< 长按响应时间 */				
	}
    else
    {
       // MENU_ShowTime(KeyBuf,0x00);
    }  
	if(KeyLongBuf !=KeyBuf)
	{
		KeyCut++;
		if(KeyCut > SHORT_KEY_CNT && KeyFlg == 0 && KeyLongTime == FIRST_LONG_KEY_CNT)
		{
            KeyShortBuf = KeyBuf;
			KeyFlg = 1; 
		}
		if(KeyCut >= KeyLongTime)
		{	
			KeyCut = 0;
			KeyFlg = 0;	
			
            KeyLongTime = NEXT_LONG_KEY_CNT;		    /**< 连按间隔时间 */
			KeyLongBuf 	= KeyBuf;
			key         = (KeyLongBuf|LONG_KEY_MARK);
		}
	}
	else
	{
		KeyCut = 0;
		if(KeyFlg==1)				 					/**< 短按键在释放按键时响应 */		
		{
			KeyFlg=0;
			KeyLongBuf = KeyBuf;	  			        /**< 将当前值给长按键值用于按键限定 */
			key = KeyShortBuf;
		}
	}
/***********************************************************************************************/
	/**<-----------------------------------------------*/
	if(key&LONG_KEY_MARK)
	{
		//KeyLongBuf = KEY_NO;							/**< 恢复长按键的连发功能 */
	}
	/**<-----------------------------------------------*/
    switch(key)
    {
		case KEY_OPEN:
			
			event = BSP_KEY_OPEN;
			break;
		case KEY_REPLY:
			
			event = BSP_KEY_REPLY;
			break;
		case KEY_NEXT:
			
			event = BSP_KEY_NEXT;
			break;
		case KEY_LONG_OPEN:

			event = BSP_KEY_LONG_OPEN;
			break;
		case KEY_LONG_REPLY:

			event = BSP_KEY_LONG_REPLY;
			break;
		case KEY_LONG_NEXT:
			
            //KeyLongBuf = KEY_NO;
			event = BSP_KEY_LONG_NEXT;	
			break;
		case KEY_LONG_OPEN_REPLY:

			event = BSP_KEY_LONG_OPEN_REPLY;
			break;
		case KEY_LONG_REPLY_NEXT:
        
			event = BSP_KEY_LONG_REPLY_NEXT;
            break;
		case KEY_LONG_OPEN_NEXT:
			
			event = BSP_KEY_LONG_OPEN_NEXT;
			break;
		case KEY_LONG_OPEN_REPLY_NEXT:

			event = BSP_KEY_LONG_OPEN_REPLY_NEXT;
			break;
		default:

			break;
	}
	return event;
}
/********************************************************************************************************
*											    结束
********************************************************************************************************/
