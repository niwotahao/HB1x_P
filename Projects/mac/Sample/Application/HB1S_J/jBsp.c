/********************************************************************************************************

********************************************************************************************************/
#include "OnBoard.h"
#include "msa.h"
#include "OSAL_Nv.h"
#include "jBsp.h"
#include "Menu.h"
#include "mac_api.h"
/********************************************************************************************************

********************************************************************************************************/
typedef BspEvent_t(*BspFuncPtr)(BspEvent_t event);

typedef struct
{
	uint8 CurrentIndex; 			    /**< ��ǰ�˵����� */
	uint8 ClearIndex;					/**< ����Clear���˵����� */
	uint8 SetIndex;				        /**< ����Set���˵����� */
	uint8 DecIndex;					    /**< ����Dec���˵����� */ 
    uint8 AddIndex;					    /**< ����Add���˵����� */
    uint8 AutoAddIndex;                 /**< �����Զ��Ӽ��˵����� */
    
	uint8 SetLongIndex;			        /**< ����Set�������˵����� */

    //uint8 LowPowerIndex;                /**< �͵�ѹָʾ���� */
    //uint8 IncPowerIndex;                /**< �������ָʾ���� */
    uint8 FlashIndex;                   /**< ��˸���� */
	BspFuncPtr CurrentOperate;          /**< ��ǰ�˵��������� */
}MenuOperate_t;
/********************************************************************************************************
*																	
********************************************************************************************************/
BspFlg_t BspFlg;
BspFlg_t StoreBspFlg;
uint8  MenuFuncIndex = 0;
BspFuncPtr 	MenuFuncPtr = NULL;
/********************************************************************************************************/

BspEvent_t BSP_DisCountValueOperate(BspEvent_t event);
BspEvent_t BSP_DisAutoManualOperate(BspEvent_t event);
BspEvent_t BSP_DisRfWorkMOdeOperate(BspEvent_t event);
BspEvent_t BSP_DisChannelOperate(BspEvent_t event);
BspEvent_t BSP_DisSetAutoManualOperate(BspEvent_t event);
BspEvent_t BSP_DisSetRfWorkMOdeOperate(BspEvent_t event);
BspEvent_t BSP_DisSetChannelOperate(BspEvent_t event);
/********************************************************************************************************/
const __code MenuOperate_t MenuTotalTab[]=
{
    {	/**< 00 */
        DIS_COUNT_VALUE,                /**< ��ǰ�˵����� */
        DIS_NONE,					    /**< ����Clear���˵����� */
        DIS_NONE,				        /**< ����Set���˵����� */
        DIS_COUNT_VALUE,			    /**< ����Dec���˵����� */ 
        DIS_COUNT_VALUE,			    /**< ����Add���˵����� */ 
        DIS_COUNT_VALUE,                /**< �����Զ��Ӽ��˵����� */

        DIS_AUTO_MANUAL,			    /**< ����Set�������˵����� */

        DIS_COUNT_VALUE,                /**< ��˸���� */
        BSP_DisCountValueOperate,       /**< ��ǰ�˵��������� */
    },
    {	/**< 10 */
        DIS_AUTO_MANUAL,                /**< ��ǰ�˵����� */
        DIS_NONE,			            /**< ����Clear���˵����� */
        DIS_SET_AUTO_MANUAL,		    /**< ����Set���˵����� */
        DIS_CHANNEL,				    /**< ����Dec���˵����� */ 
        DIS_WORK_MODE,			        /**< ����Add���˵����� */ 
        DIS_NONE,                       /**< �����Զ��Ӽ��˵����� */
        
        DIS_COUNT_VALUE,			    /**< ����Set�������˵����� */

        DIS_AUTO_MANUAL,                /**< ��˸���� */
        BSP_DisAutoManualOperate,       /**< ��ǰ�˵��������� */
    },
    {	/**< 11 */
        DIS_WORK_MODE,                  /**< ��ǰ�˵����� */
        DIS_NONE,					    /**< ����Clear���˵����� */
        DIS_SET_WORK_MODE,			    /**< ����Set���˵����� */
        DIS_AUTO_MANUAL,			    /**< ����Dec���˵����� */ 
        DIS_CHANNEL,			        /**< ����Add���˵����� */ 
        DIS_NONE,                       /**< �����Զ��Ӽ��˵����� */
        
        DIS_COUNT_VALUE,			    /**< ����Set�������˵����� */

        DIS_WORK_MODE,                  /**< ��˸���� */
        BSP_DisRfWorkMOdeOperate,       /**< ��ǰ�˵��������� */
    },
    {	/**< 12 */
        DIS_CHANNEL,                    /**< ��ǰ�˵����� */
        DIS_NONE,			            /**< ����Clear���˵����� */
        DIS_SET_CHANNEL,			    /**< ����Set���˵����� */
        DIS_WORK_MODE,	                /**< ����Dec���˵����� */ 
        DIS_AUTO_MANUAL,			    /**< ����Add���˵����� */ 
        DIS_NONE,                       /**< �����Զ��Ӽ��˵����� */
        
        DIS_COUNT_VALUE,			    /**< ����Set�������˵����� */

        DIS_CHANNEL,                    /**< ��˸���� */
        BSP_DisChannelOperate,          /**< ��ǰ�˵��������� */
    },
    {	/**< 100 */
        DIS_SET_AUTO_MANUAL,            /**< ��ǰ�˵����� */
        DIS_AUTO_MANUAL,			    /**< ����Clear���˵����� */
        DIS_AUTO_MANUAL,			    /**< ����Set���˵����� */
        DIS_SET_AUTO_MANUAL,		    /**< ����Dec���˵����� */ 
        DIS_SET_AUTO_MANUAL,		    /**< ����Add���˵����� */ 
        DIS_NONE,                       /**< �����Զ��Ӽ��˵����� */
        
        DIS_NONE,			            /**< ����Set�������˵����� */

        DIS_SET_AUTO_MANUAL,            /**< ��˸���� */
        BSP_DisSetAutoManualOperate,    /**< ��ǰ�˵��������� */
    },
    {	/**< 110 */
        DIS_SET_WORK_MODE,              /**< ��ǰ�˵����� */
        DIS_WORK_MODE,				    /**< ����Clear���˵����� */
        DIS_WORK_MODE,			        /**< ����Set���˵����� */
        DIS_SET_WORK_MODE,			    /**< ����Dec���˵����� */ 
        DIS_SET_WORK_MODE,			    /**< ����Add���˵����� */ 
        DIS_NONE,                       /**< �����Զ��Ӽ��˵����� */
        
        DIS_NONE,			            /**< ����Set�������˵����� */

        DIS_SET_WORK_MODE,              /**< ��˸���� */
        BSP_DisSetRfWorkMOdeOperate,    /**< ��ǰ�˵��������� */
    },
    {	/**< 120 */
        DIS_SET_CHANNEL,                /**< ��ǰ�˵����� */
        DIS_CHANNEL,			        /**< ����Clear���˵����� */
        DIS_CHANNEL,			        /**< ����Set���˵����� */
        DIS_SET_CHANNEL,	            /**< ����Dec���˵����� */ 
        DIS_SET_CHANNEL,			    /**< ����Add���˵����� */ 
        DIS_NONE,                       /**< �����Զ��Ӽ��˵����� */
        
        DIS_NONE,			            /**< ����Set�������˵����� */

        DIS_SET_CHANNEL,                /**< ��˸���� */
        BSP_DisSetChannelOperate,       /**< ��ǰ�˵��������� */
    },
};
/********************************************************************************************************/
void BSP_Init(void)
{
    SN74LS164SegInit();
    MENU_Init();
    
    osal_nv_item_init(WRITER_SET_FLG_ID,sizeof(uint8),NULL);
    osal_nv_read(WRITER_SET_FLG_ID,0,sizeof(uint8),(uint8*)&StoreBspFlg);
    if(StoreBspFlg.Reserve == 1)
    {
        StoreBspFlg.AutoManual = 1;     /**< 1���ֶ� 0���Զ� */
        StoreBspFlg.Channel = 1;        /**< Ĭ��1�ŵ� */
        StoreBspFlg.TxMode = 1;         /**< 1�㲥 0�ǵ��� */
        StoreBspFlg.Reserve = 0;        /**< ���Ϊ�ǵ�һ�β��� */
        
        osal_nv_item_init(WRITER_SET_FLG_ID,sizeof(uint8),NULL);
        osal_nv_write(WRITER_SET_FLG_ID,0,sizeof(uint8),(uint8*)&StoreBspFlg);
    }
    BspFlg = StoreBspFlg;
}
/********************************************************************************************************/
BspEvent_t BSP_DisCountValueOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
            
            SN74LS164SegNum = 0;//1 = �ֶ�, 0 = �Զ�
            HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            break;
		case BSP_KEY_SET:

            break;
		case BSP_KEY_DEC:

            if(StoreBspFlg.AutoManual == 1)
            {
                if(SN74LS164SegNum > 0)
                {
                    SN74LS164SegNum--;
                }
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }
            break;
		case BSP_KEY_ADD:
            if(StoreBspFlg.AutoManual == 1)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }
            break;
        case BSP_KEY_AUTO_ADD:
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }
            break;
        case BSP_KEY_LONG_SET:
            BspFlg = StoreBspFlg;
            break;
        case BSP_TIME_FLASH:
        
            BspFlg.Flash ^=1;
            break;
		default:
			MENU_DisCountValue();
			break;
	}
	return event;
}
/********************************************************************************************************
*						
********************************************************************************************************/
BspEvent_t BSP_DisAutoManualOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
        
            break;
		case BSP_KEY_SET:
        
            break;
		case BSP_KEY_DEC:

            break;
		case BSP_KEY_ADD:

            break;
        case BSP_KEY_AUTO_ADD:
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }  
            break;
        case BSP_KEY_LONG_SET:
        
            break;
        case BSP_TIME_FLASH:

            break;
		default:
			MENU_DisAutoManual();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*						
********************************************************************************************************/
BspEvent_t BSP_DisRfWorkMOdeOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
        
            break;
		case BSP_KEY_SET:
        
            break;
		case BSP_KEY_DEC:
        
            break;
		case BSP_KEY_ADD:

            break;
        case BSP_KEY_AUTO_ADD:
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            } 
            break;
        case BSP_KEY_LONG_SET:
        
            break;
        case BSP_TIME_FLASH:

            break;
		default:
			MENU_DisRfWorkMOde();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*						
********************************************************************************************************/
BspEvent_t BSP_DisChannelOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
        
            break;
		case BSP_KEY_SET:
        
            break;
		case BSP_KEY_DEC:

            break;
		case BSP_KEY_ADD:

            break;
        case BSP_KEY_AUTO_ADD:
        
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }   
            break;
        case BSP_KEY_LONG_SET:
        
            break;
        case BSP_TIME_FLASH:

            break;
		default:
			MENU_DisChannel();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*						
********************************************************************************************************/
BspEvent_t BSP_DisSetAutoManualOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
        
            break;
		case BSP_KEY_SET:
        
            StoreBspFlg = BspFlg;
            osal_nv_item_init(WRITER_SET_FLG_ID,sizeof(uint8),NULL);
            osal_nv_write(WRITER_SET_FLG_ID,0,sizeof(uint8),(uint8*)&StoreBspFlg);
            break;
		case BSP_KEY_DEC:
        
            BspFlg.AutoManual = 0;
            break;
		case BSP_KEY_ADD:
        
            BspFlg.AutoManual = 1;
            break;
        case BSP_KEY_AUTO_ADD:
        
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }   
            break;
        case BSP_KEY_LONG_SET:
        
            break;
        case BSP_TIME_FLASH:
        
            BspFlg.Flash ^=1;
            break;
		default:
			MENU_DisSetAutoManual();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSetRfWorkMOdeOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
        
            break;
		case BSP_KEY_SET:
        
            StoreBspFlg = BspFlg;
            osal_nv_item_init(WRITER_SET_FLG_ID,sizeof(uint8),NULL);
            osal_nv_write(WRITER_SET_FLG_ID,0,sizeof(uint8),(uint8*)&StoreBspFlg);
            break;
		case BSP_KEY_DEC:
            
            BspFlg.TxMode = 0;
            break;
		case BSP_KEY_ADD:
        
            BspFlg.TxMode = 1;
            break;
        case BSP_KEY_AUTO_ADD:
        
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }   
            break;
        case BSP_KEY_LONG_SET:
        
            break;
        case BSP_TIME_FLASH:
        
            BspFlg.Flash ^=1;
            break;
		default:
			MENU_DisSetRfWorkMOde();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*			
********************************************************************************************************/
BspEvent_t BSP_DisSetChannelOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_CLEAR:
        
            break;
		case BSP_KEY_SET:
        
            StoreBspFlg = BspFlg;
            osal_nv_item_init(WRITER_SET_FLG_ID,sizeof(uint8),NULL);
            osal_nv_write(WRITER_SET_FLG_ID,0,sizeof(uint8),(uint8*)&StoreBspFlg);
            
            RfChannle = 11+StoreBspFlg.Channel;
            MAC_MlmeSetReq(MAC_LOGICAL_CHANNEL,&RfChannle);
            break;
		case BSP_KEY_DEC:
        
            if(BspFlg.Channel > 0)
            {
                BspFlg.Channel--;
            }
            break;
		case BSP_KEY_ADD:
        
            if(BspFlg.Channel < 6)
            {
                BspFlg.Channel++;
            }
            break;
        case BSP_KEY_AUTO_ADD:
        
            if(StoreBspFlg.AutoManual == 0)
            {
                SN74LS164SegNum++;
                HBee_JsqSendData(StoreBspFlg.AutoManual,SN74LS164SegNum);
            }   
            break;
        case BSP_KEY_LONG_SET:
            
            break;
        case BSP_TIME_FLASH:
        
            BspFlg.Flash ^=1;
            break;
		default:
			MENU_DisSetChannel();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
void BSP_EventTask(BspEvent_t event)
{
    uint8 funcIndex;
        
	if((event <= BSP_NONE)||(event >= BSP_MAX_EVENT))
	{
		return;
	}
	/** ������ǰ����������� */
	MenuFuncPtr = MenuTotalTab[MenuFuncIndex].CurrentOperate;
	event =(*MenuFuncPtr)(event);

    switch(event)
    {
		case BSP_KEY_CLEAR:
			funcIndex = MenuTotalTab[MenuFuncIndex].ClearIndex;
			break;
		case BSP_KEY_SET:
			funcIndex = MenuTotalTab[MenuFuncIndex].SetIndex;
			break;
		case BSP_KEY_DEC:
			funcIndex = MenuTotalTab[MenuFuncIndex].DecIndex;
			break;
		case BSP_KEY_ADD:
			funcIndex = MenuTotalTab[MenuFuncIndex].AddIndex;
			break;
		case BSP_KEY_LONG_SET:
			funcIndex = MenuTotalTab[MenuFuncIndex].SetLongIndex;
			break;
	    case BSP_TIME_FLASH:
	        funcIndex = MenuTotalTab[MenuFuncIndex].FlashIndex;
	        break;
        default:
            funcIndex = MenuTotalTab[MenuFuncIndex].CurrentIndex;  
            break;
	}
    
    if(funcIndex != DIS_NONE)
    {
        MenuFuncIndex = funcIndex;
        /** ת���������� */
        MenuFuncPtr = MenuTotalTab[MenuFuncIndex].CurrentOperate;
        (*MenuFuncPtr)(BSP_NONE);
    }
}
/********************************************************************************************************
*										
********************************************************************************************************/


