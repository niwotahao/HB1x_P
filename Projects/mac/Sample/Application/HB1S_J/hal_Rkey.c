/********************************************************************************************************

********************************************************************************************************/
#include "hal_Rkey.h"


/********************************************************************************************************
*                                           ��ȡ�������ñ���
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
	KEY_PORT_INIT();
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
		KeyLongTime = FIRST_LONG_KEY_CNT;				/**< ������Ӧʱ�� */				
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
			
            KeyLongTime = NEXT_LONG_KEY_CNT;		    /**< �������ʱ�� */
			KeyLongBuf 	= KeyBuf;
			key         = (KeyLongBuf|LONG_KEY_MARK);
		}
	}
	else
	{
		KeyCut = 0;
		if(KeyFlg==1)				 					/**< �̰������ͷŰ���ʱ��Ӧ */		
		{
			KeyFlg=0;
			KeyLongBuf = KeyBuf;	  			        /**< ����ǰֵ��������ֵ���ڰ����޶� */
			key = KeyShortBuf;
		}
	}
/***********************************************************************************************/
	/**<-----------------------------------------------*/
	if(key&LONG_KEY_MARK)
	{
		//KeyLongBuf = KEY_NO;							/**< �ָ����������������� */
	}
	/**<-----------------------------------------------*/
    switch(key)
    {
		case KEY_CLEAR:
			
			event = BSP_KEY_CLEAR;
			break;
		case KEY_SET:
			
			event = BSP_KEY_SET;
			break;
		case KEY_DEC:
			
			event = BSP_KEY_DEC;
			break;
		case KEY_ADD:
			
			event = BSP_KEY_ADD;
			break;
        case KEY_AUTO_ADD:
            event = BSP_KEY_AUTO_ADD;
            break;
		case KEY_LONG_SET:

			event = BSP_KEY_LONG_SET;
			break;
		default:

			break;
	}
	return event;
}
/********************************************************************************************************
*											    ����
********************************************************************************************************/
