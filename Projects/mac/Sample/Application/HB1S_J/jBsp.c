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
	uint8 CurrentIndex; 			    /**< 当前菜单索引 */
	uint8 ClearIndex;					/**< 操作Clear键菜单索引 */
	uint8 SetIndex;				        /**< 操作Set键菜单索引 */
	uint8 DecIndex;					    /**< 操作Dec键菜单索引 */ 
    uint8 AddIndex;					    /**< 操作Add键菜单索引 */
    uint8 AutoAddIndex;                 /**< 操作自动加键菜单索引 */
    
	uint8 SetLongIndex;			        /**< 操作Set长按键菜单索引 */

    //uint8 LowPowerIndex;                /**< 低电压指示索引 */
    //uint8 IncPowerIndex;                /**< 操作充电指示索引 */
    uint8 FlashIndex;                   /**< 闪烁索引 */
	BspFuncPtr CurrentOperate;          /**< 当前菜单操作函数 */
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
        DIS_COUNT_VALUE,                /**< 当前菜单索引 */
        DIS_NONE,					    /**< 操作Clear键菜单索引 */
        DIS_NONE,				        /**< 操作Set键菜单索引 */
        DIS_COUNT_VALUE,			    /**< 操作Dec键菜单索引 */ 
        DIS_COUNT_VALUE,			    /**< 操作Add键菜单索引 */ 
        DIS_COUNT_VALUE,                /**< 操作自动加键菜单索引 */

        DIS_AUTO_MANUAL,			    /**< 操作Set长按键菜单索引 */

        DIS_COUNT_VALUE,                /**< 闪烁索引 */
        BSP_DisCountValueOperate,       /**< 当前菜单操作函数 */
    },
    {	/**< 10 */
        DIS_AUTO_MANUAL,                /**< 当前菜单索引 */
        DIS_NONE,			            /**< 操作Clear键菜单索引 */
        DIS_SET_AUTO_MANUAL,		    /**< 操作Set键菜单索引 */
        DIS_CHANNEL,				    /**< 操作Dec键菜单索引 */ 
        DIS_WORK_MODE,			        /**< 操作Add键菜单索引 */ 
        DIS_NONE,                       /**< 操作自动加键菜单索引 */
        
        DIS_COUNT_VALUE,			    /**< 操作Set长按键菜单索引 */

        DIS_AUTO_MANUAL,                /**< 闪烁索引 */
        BSP_DisAutoManualOperate,       /**< 当前菜单操作函数 */
    },
    {	/**< 11 */
        DIS_WORK_MODE,                  /**< 当前菜单索引 */
        DIS_NONE,					    /**< 操作Clear键菜单索引 */
        DIS_SET_WORK_MODE,			    /**< 操作Set键菜单索引 */
        DIS_AUTO_MANUAL,			    /**< 操作Dec键菜单索引 */ 
        DIS_CHANNEL,			        /**< 操作Add键菜单索引 */ 
        DIS_NONE,                       /**< 操作自动加键菜单索引 */
        
        DIS_COUNT_VALUE,			    /**< 操作Set长按键菜单索引 */

        DIS_WORK_MODE,                  /**< 闪烁索引 */
        BSP_DisRfWorkMOdeOperate,       /**< 当前菜单操作函数 */
    },
    {	/**< 12 */
        DIS_CHANNEL,                    /**< 当前菜单索引 */
        DIS_NONE,			            /**< 操作Clear键菜单索引 */
        DIS_SET_CHANNEL,			    /**< 操作Set键菜单索引 */
        DIS_WORK_MODE,	                /**< 操作Dec键菜单索引 */ 
        DIS_AUTO_MANUAL,			    /**< 操作Add键菜单索引 */ 
        DIS_NONE,                       /**< 操作自动加键菜单索引 */
        
        DIS_COUNT_VALUE,			    /**< 操作Set长按键菜单索引 */

        DIS_CHANNEL,                    /**< 闪烁索引 */
        BSP_DisChannelOperate,          /**< 当前菜单操作函数 */
    },
    {	/**< 100 */
        DIS_SET_AUTO_MANUAL,            /**< 当前菜单索引 */
        DIS_AUTO_MANUAL,			    /**< 操作Clear键菜单索引 */
        DIS_AUTO_MANUAL,			    /**< 操作Set键菜单索引 */
        DIS_SET_AUTO_MANUAL,		    /**< 操作Dec键菜单索引 */ 
        DIS_SET_AUTO_MANUAL,		    /**< 操作Add键菜单索引 */ 
        DIS_NONE,                       /**< 操作自动加键菜单索引 */
        
        DIS_NONE,			            /**< 操作Set长按键菜单索引 */

        DIS_SET_AUTO_MANUAL,            /**< 闪烁索引 */
        BSP_DisSetAutoManualOperate,    /**< 当前菜单操作函数 */
    },
    {	/**< 110 */
        DIS_SET_WORK_MODE,              /**< 当前菜单索引 */
        DIS_WORK_MODE,				    /**< 操作Clear键菜单索引 */
        DIS_WORK_MODE,			        /**< 操作Set键菜单索引 */
        DIS_SET_WORK_MODE,			    /**< 操作Dec键菜单索引 */ 
        DIS_SET_WORK_MODE,			    /**< 操作Add键菜单索引 */ 
        DIS_NONE,                       /**< 操作自动加键菜单索引 */
        
        DIS_NONE,			            /**< 操作Set长按键菜单索引 */

        DIS_SET_WORK_MODE,              /**< 闪烁索引 */
        BSP_DisSetRfWorkMOdeOperate,    /**< 当前菜单操作函数 */
    },
    {	/**< 120 */
        DIS_SET_CHANNEL,                /**< 当前菜单索引 */
        DIS_CHANNEL,			        /**< 操作Clear键菜单索引 */
        DIS_CHANNEL,			        /**< 操作Set键菜单索引 */
        DIS_SET_CHANNEL,	            /**< 操作Dec键菜单索引 */ 
        DIS_SET_CHANNEL,			    /**< 操作Add键菜单索引 */ 
        DIS_NONE,                       /**< 操作自动加键菜单索引 */
        
        DIS_NONE,			            /**< 操作Set长按键菜单索引 */

        DIS_SET_CHANNEL,                /**< 闪烁索引 */
        BSP_DisSetChannelOperate,       /**< 当前菜单操作函数 */
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
        StoreBspFlg.AutoManual = 1;     /**< 1是手动 0是自动 */
        StoreBspFlg.Channel = 1;        /**< 默认1信道 */
        StoreBspFlg.TxMode = 1;         /**< 1广播 0是单播 */
        StoreBspFlg.Reserve = 0;        /**< 标记为非第一次操作 */
        
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
            
            SN74LS164SegNum = 0;//1 = 手动, 0 = 自动
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
	/** 操作当前界面参数或处理 */
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
        /** 转换操作界面 */
        MenuFuncPtr = MenuTotalTab[MenuFuncIndex].CurrentOperate;
        (*MenuFuncPtr)(BSP_NONE);
    }
}
/********************************************************************************************************
*										
********************************************************************************************************/


