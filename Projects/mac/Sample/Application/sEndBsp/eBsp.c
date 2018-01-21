
/********************************************************************************************************

********************************************************************************************************/
#include "eBsp.h"
#include "OnBoard.h"
#include "msa.h"
#include "OSAL_Nv.h"
/********************************************************************************************************

********************************************************************************************************/
typedef BspEvent_t(*BspFuncPtr)(BspEvent_t event);

typedef struct
{
	uint8 CurrentIndex; 			    /**< ��ǰ�˵����� */
	uint8 OpenIndex;					/**< ����OPEN���˵����� */
	uint8 ReplyIndex;				    /**< ����REPLY���˵����� */
	uint8 NextIndex;					/**< ����NEXT���˵����� */ 
    
	uint8 OpenLongIndex;			    /**< ����OPEN�������˵����� */
	uint8 ReplyLongIndex;			    /**< ����REPLY�������˵����� */
	uint8 NextLongIndex;			    /**< ����NEXT�������˵����� */ 
    
	uint8 OpenReplyLongIndex;		    /**< ����OPEN_REPLY�������˵����� */
	uint8 ReplyNextLongIndex;		    /**< ����REPLY_NEXT�������˵����� */
	uint8 OpenNextLongIndex;            /**< ����OPEN_NEXT�������˵����� */
	uint8 OpenReplyNextLongIndex;       /**< ����OPEN_REPLY_NEXT�������˵����� */
    
    uint8 LowPowerIndex;                /**< �͵�ѹָʾ���� */
    uint8 IncPowerIndex;                /**< �������ָʾ���� */
    uint8 FlashIndex;                   /**< ��˸���� */
	BspFuncPtr CurrentOperate;          /**< ��ǰ�˵��������� */
}MenuOperate_t;
/********************************************************************************************************
*																	
********************************************************************************************************/
uint8  MenuFuncIndex = 0;
BspFuncPtr 	MenuFuncPtr = NULL;
/********************************************************************************************************

********************************************************************************************************/
BspEvent_t BSP_DisOffOperate(BspEvent_t event);
BspEvent_t  BSP_DisListMsgOperate(BspEvent_t event);
BspEvent_t  BSP_DisOpenMsgOperate(BspEvent_t event);
BspEvent_t BSP_DisIncPowerOperate(BspEvent_t event);
BspEvent_t  BSP_DisLowPowerOperate(BspEvent_t event);

BspEvent_t BSP_DisUserPasswordOperate(BspEvent_t event);
BspEvent_t  BSP_DisUserPasswordOperate(BspEvent_t event);
BspEvent_t BSP_DisUserSetTipNumberOperate(BspEvent_t event);
BspEvent_t  BSP_DisEnterPasswordOperate(BspEvent_t event);
BspEvent_t  BSP_DisUserSetPasswordOperate(BspEvent_t event);
BspEvent_t  BSP_DisSysPasswordOperate(BspEvent_t event);
BspEvent_t BSP_DisUserSetKeyMotorOperate(BspEvent_t event);
BspEvent_t BSP_DisSysGetRfParamOperate(BspEvent_t event);


BspEvent_t BSP_DisSysSetChannelOperate(BspEvent_t event);
BspEvent_t BSP_DisSysSetPanidOperate(BspEvent_t event);
BspEvent_t BSP_DisSysSetAddrOperate(BspEvent_t event);
BspEvent_t BSP_DisSysSetShowModeOperate(BspEvent_t event);
BspEvent_t BSP_DisSysEnShutDownOperate(BspEvent_t event);
BspEvent_t BSP_DisSysResetToDefaultOperate(BspEvent_t event);
/********************************************************************************************************/
const __code MenuOperate_t MenuTotalTab[]=
{
    {	/**< 00 */
        DIS_OFF, 			                /**< ��ǰ�˵����� */
        
        DIS_NONE,					        /**< ����OPEN���˵����� */
        DIS_NONE,				            /**< ����REPLY���˵����� ? */
        DIS_NONE,					        /**< ����NEXT���˵����� */ 
        
        DIS_LIST_MSG,			            /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,			                /**< ����NEXT�������˵����� */ 
        
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */
        DIS_INC_POWER,                      /**< �������ָʾ���� */
        DIS_NONE,                           /**< ��˸���� */

        BSP_DisOffOperate,                  /**< ��ǰ�˵��������� */
    },
    {	/**< 01 */
        DIS_LIST_MSG, 			            /**< ��ǰ�˵����� */
        
        DIS_OPEN_MSG,					    /**< ����OPEN���˵����� */
        DIS_NONE,//DIS_LIST_MSG,		    /**< ����REPLY���˵����� ? */
        DIS_NONE,//DIS_LIST_MSG,		    /**< ����NEXT���˵����� */ 
        
        DIS_OFF,			                /**< ����OPEN�������˵����� */
        DIS_USER_PASSWORD,			        /**< ����REPLY�������˵����� */
        DIS_LIST_MSG,		                /**< ����NEXT�������˵����� */ 
        
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_LOW_POWER,                      /**< �͵�ѹָʾ���� */
        DIS_INC_POWER,                      /**< �������ָʾ���� */
        DIS_NONE,                           /**< ��˸���� */

        BSP_DisListMsgOperate,              /**< ��ǰ�˵��������� */
    },
    {	/**< 02 */
        DIS_OPEN_MSG, 			            /**< ��ǰ�˵����� */
        
        DIS_LIST_MSG,					    /**< ����OPEN���˵����� */
        DIS_OPEN_MSG,				        /**< ����REPLY���˵����� */
        DIS_OPEN_MSG,					    /**< ����NEXT���˵����� */ 
        
        DIS_OFF,			                /**< ����OPEN�������˵����� */
        DIS_LIST_MSG,			            /**< ����REPLY�������˵����� */
        DIS_NONE,			                /**< ����NEXT�������˵����� */ 
        
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_NONE,                           /**< ��˸���� */
        BSP_DisOpenMsgOperate,              /**< ��ǰ�˵��������� */
    },
    {	/**< 03 */
        DIS_LOW_POWER, 			            /**< ��ǰ�˵����� */
        
        DIS_LIST_MSG,					    /**< ����OPEN���˵����� */
        DIS_LIST_MSG,				        /**< ����REPLY���˵����� */
        DIS_LIST_MSG,					    /**< ����NEXT���˵����� */ 
        
        DIS_OFF,			                /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,			                /**< ����NEXT�������˵����� */ 
        
        DIS_LIST_MSG,		                /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_INC_POWER,                      /**< �������ָʾ���� */
        DIS_NONE,                           /**< ��˸���� */

        BSP_DisLowPowerOperate,             /**< ��ǰ�˵��������� */
    },  
    {	/**< 04 */
        DIS_INC_POWER, 			            /**< ��ǰ�˵����� */
        
        DIS_LIST_MSG,					    /**< ����OPEN���˵����� */
        DIS_LIST_MSG,		                /**< ����REPLY���˵����� */
        DIS_LIST_MSG,					    /**< ����NEXT���˵����� */ 
        
        DIS_NONE,			                /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,			                /**< ����NEXT�������˵����� */ 
        
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_LIST_MSG,                       /**< �������ָʾ���� */
        DIS_NONE,                           /**< ��˸���� */

        BSP_DisIncPowerOperate,             /**< ��ǰ�˵��������� */
    },
    {	/**< 10 -- 5 */  
        DIS_USER_PASSWORD, 			        /**< ��ǰ�˵����� */
            
        DIS_USER_PASSWORD,	                /**< ����OPEN���˵����� */
        DIS_USER_PASSWORD,		            /**< ����REPLY���˵����� */
        DIS_LIST_MSG ,				        /**< ����NEXT���˵����� */ 
            
        DIS_USER_SET_TIP_NUMBER,		    /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,			                /**< ����NEXT�������˵����� */ 
            
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_SYS_GET_RF_PARAM,               /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_USER_PASSWORD,                  /**< ��˸���� */
		
        BSP_DisUserPasswordOperate,         /**< ��ǰ�˵��������� */
    },
 	{	/**< 11 -- 6 */
        DIS_USER_SET_TIP_NUMBER, 		    /**< ��ǰ�˵����� */
        
        DIS_USER_SET_TIP_NUMBER,		    /**< ����OPEN���˵����� */
        DIS_USER_SET_TIP_NUMBER,		    /**< ����REPLY���˵����� */
        DIS_USER_SET_KEY_MOTOR,		        /**< ����NEXT���˵����� */ 
            
        DIS_USER_SET_TIP_NUMBER,		    /**< ����OPEN�������˵����� */
        DIS_LIST_MSG,			            /**< ����REPLY�������˵����� */
        DIS_NONE,		                    /**< ����NEXT�������˵����� */ 
            
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_SYS_PASSWORD,            	    /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,                           /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_USER_SET_TIP_NUMBER,            /**< ��˸���� */

        BSP_DisUserSetTipNumberOperate,     /**< ��ǰ�˵��������� */
    },  
    {	/**< 12 -- 7 */ 
        DIS_USER_SET_KEY_MOTOR, 		    /**< ��ǰ�˵����� */
            
        DIS_USER_SET_KEY_MOTOR,		        /**< ����OPEN���˵����� */
        DIS_USER_SET_KEY_MOTOR,		        /**< ����REPLY���˵����� */
        DIS_USER_SET_PASSWORD,              /**< ����NEXT���˵����� */ 
            
        DIS_USER_SET_KEY_MOTOR,		        /**< ����OPEN�������˵����� */
        DIS_LIST_MSG,                       /**< ����REPLY�������˵����� */
        DIS_NONE,				            /**< ����NEXT�������˵����� */ 
            
        DIS_NONE,		    	            /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    	            /**< ����REPLY_NEXT�������˵����� */
        DIS_SYS_PASSWORD,       		    /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,        		            /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_USER_SET_KEY_MOTOR,             /**< ��˸���� */
    
        BSP_DisUserSetKeyMotorOperate,      /**< ��ǰ�˵��������� */
    },
	{	/**< 13 -- 8 */
		DIS_USER_SET_PASSWORD,			    /**< ��ǰ�˵����� */
            
		DIS_USER_SET_PASSWORD,			    /**< ����OPEN���˵����� */
		DIS_USER_SET_PASSWORD,		        /**< ����REPLY���˵����� */
		DIS_USER_SET_TIP_NUMBER,	        /**< ����NEXT���˵����� */ 
            
		DIS_USER_SET_PASSWORD,			    /**< ����OPEN�������˵����� */
		DIS_LIST_MSG,					    /**< ����REPLY�������˵����� */
		DIS_NONE,			                /**< ����NEXT�������˵����� */ 
            
		DIS_NONE,				            /**< ����OPEN_REPLY�������˵����� */
		DIS_NONE,				            /**< ����REPLY_NEXT�������˵����� */
		DIS_SYS_PASSWORD,				    /**< ����OPEN_NEXT�������˵����� */
		DIS_NONE,				            /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_USER_SET_PASSWORD,              /**< ��˸���� */

		BSP_DisUserSetPasswordOperate,	    /**< ��ǰ�˵��������� */
	},
    {	/**< 20 -- 9 */
        DIS_SYS_PASSWORD, 		            /**< ��ǰ�˵����� */
            
        DIS_SYS_PASSWORD,		 	        /**< ����OPEN���˵����� */
        DIS_SYS_PASSWORD,		            /**< ����REPLY���˵����� */
        DIS_USER_SET_TIP_NUMBER,	        /**< ����NEXT���˵����� */ 
            
        DIS_SYS_GET_RF_PARAM,		        /**< ����OPEN�������˵����� */
        DIS_SYS_PASSWORD,			  	    /**< ����REPLY�������˵����� */
        DIS_SYS_PASSWORD,	                /**< ����NEXT�������˵����� */ 
            
        DIS_NONE,		                    /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		                    /**< ����REPLY_NEXT�������˵����� */
        DIS_NONE,              	            /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,              	            /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_PASSWORD,                   /**< ��˸���� */

        BSP_DisSysPasswordOperate,   	    /**< ��ǰ�˵��������� */
    },
    {	/**< 21 -- 10 */
        DIS_SYS_GET_RF_PARAM, 		        /**< ��ǰ�˵����� */
                
        DIS_SYS_GET_RF_PARAM,		        /**< ����OPEN���˵����� */
        DIS_SYS_GET_RF_PARAM,		        /**< ����REPLY���˵����� */
        DIS_SYS_SET_CHANNEL ,		        /**< ����NEXT���˵����� */ 
                
        DIS_SYS_GET_RF_PARAM,		        /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,		                    /**< ����NEXT�������˵����� */ 
        
        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,            		        /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_GET_RF_PARAM,               /**< ��˸���� */
     
        BSP_DisSysGetRfParamOperate,        /**< ��ǰ�˵��������� */
    },


    {	/**< 22 -- 11 */
        DIS_SYS_SET_CHANNEL, 		        /**< ��ǰ�˵����� */
            
        DIS_SYS_SET_CHANNEL,		        /**< ����OPEN���˵����� */
        DIS_SYS_SET_CHANNEL,		        /**< ����REPLY���˵����� */
        DIS_SYS_SET_PANID,		            /**< ����NEXT���˵����� */ 
            
        DIS_SYS_SET_CHANNEL,		        /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,		    	            /**< ����NEXT�������˵����� */ 
            
    
        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,            		        /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_SET_CHANNEL,                /**< ��˸���� */

        BSP_DisSysSetChannelOperate,        /**< ��ǰ�˵��������� */
    },
    {	/**< 23 -- 12 */
        DIS_SYS_SET_PANID, 		            /**< ��ǰ�˵����� */
            
        DIS_SYS_SET_PANID,		    	    /**< ����OPEN���˵����� */
        DIS_SYS_SET_PANID,		    	    /**< ����REPLY���˵����� */
        DIS_SYS_SET_ADDR,		    	    /**< ����NEXT���˵����� */ 
            
        DIS_SYS_SET_PANID,		    	    /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,		    	            /**< ����NEXT�������˵����� */ 
            
    
        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,            		        /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_SET_PANID,                  /**< ��˸���� */

        BSP_DisSysSetPanidOperate,          /**< ��ǰ�˵��������� */
    },
    {	/**< 24 -- 13 */
        DIS_SYS_SET_ADDR, 		            /**< ��ǰ�˵����� */
        
        DIS_SYS_SET_ADDR,		    		/**< ����OPEN���˵����� */
        DIS_SYS_SET_ADDR,		            /**< ����REPLY���˵����� */
        DIS_SYS_SET_SHOW_MODE,		        /**< ����NEXT���˵����� */ 
        
        DIS_SYS_SET_ADDR,		    		/**< ����OPEN�������˵����� */
        DIS_NONE,			    	        /**< ����REPLY�������˵����� */
        DIS_NONE,		    	            /**< ����NEXT�������˵����� */ 
        

        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,            		        /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_SET_ADDR,                   /**< ��˸���� */

        BSP_DisSysSetAddrOperate,           /**< ��ǰ�˵��������� */
    },
    {	/**< 25 -- 14 */
        DIS_SYS_SET_SHOW_MODE, 		        /**< ��ǰ�˵����� */
            
        DIS_SYS_SET_SHOW_MODE,		        /**< ����OPEN���˵����� */
        DIS_SYS_SET_SHOW_MODE,		        /**< ����REPLY���˵����� */
        DIS_SYS_EN_SHUT_DOWN,		        /**< ����NEXT���˵����� */ 
            
        DIS_SYS_SET_SHOW_MODE,		        /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,		                    /**< ����NEXT�������˵����� */ 
            
    
        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_LIST_MSG,            		    /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_SET_SHOW_MODE,              /**< ��˸���� */

        BSP_DisSysSetShowModeOperate,       /**< ��ǰ�˵��������� */
    },
    {	/**< 26 -- 15 */
        DIS_SYS_EN_SHUT_DOWN, 		        /**< ��ǰ�˵����� */
            
        DIS_SYS_EN_SHUT_DOWN,		        /**< ����OPEN���˵����� */
        DIS_SYS_EN_SHUT_DOWN,		        /**< ����REPLY���˵����� */
        DIS_SYS_RESET_TO_DEFAULT,		    /**< ����NEXT���˵����� */ 
            
        DIS_SYS_EN_SHUT_DOWN,		        /**< ����OPEN�������˵����� */
        DIS_NONE,			                /**< ����REPLY�������˵����� */
        DIS_NONE,		                    /**< ����NEXT�������˵����� */ 
            
    
        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_LIST_MSG,            		    /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_EN_SHUT_DOWN,               /**< ��˸���� */
 
        BSP_DisSysEnShutDownOperate,        /**< ��ǰ�˵��������� */
    }, 
    {	/**< 27 -- 16 */
        DIS_SYS_RESET_TO_DEFAULT, 		    /**< ��ǰ�˵����� */
        
        DIS_SYS_RESET_TO_DEFAULT,		    /**< ����OPEN���˵����� */
        DIS_SYS_RESET_TO_DEFAULT,		    /**< ����REPLY���˵����� */
        DIS_SYS_GET_RF_PARAM,		        /**< ����NEXT���˵����� */ 
            
        DIS_SYS_RESET_TO_DEFAULT,		    /**< ����OPEN�������˵����� */
        DIS_NONE,		                    /**< ����REPLY�������˵����� */
        DIS_NONE,		                    /**< ����NEXT�������˵����� */ 
            

        DIS_NONE,		    		        /**< ����OPEN_REPLY�������˵����� */
        DIS_NONE,		    		        /**< ����REPLY_NEXT�������˵����� */
        DIS_USER_SET_TIP_NUMBER,            /**< ����OPEN_NEXT�������˵����� */
        DIS_NONE,            		        /**< ����OPEN_REPLY_NEXT�������˵����� */
        DIS_NONE,                           /**< �͵�ѹָʾ���� */        
        DIS_NONE,                           /**< �������ָʾ���� */
        DIS_SYS_RESET_TO_DEFAULT,           /**< ��˸���� */

        BSP_DisSysResetToDefaultOperate,    /**< ��ǰ�˵��������� */
    }, 
};
/********************************************************************************************************
*
********************************************************************************************************/

/********************************************************************************************************
*
********************************************************************************************************/
void BSP_InitIOPortFun( void )
{
    //++++++++++++++++++++++GT21L16S2W IO�ڶ���++++++++++++++++++//
    IO_FUNC_PORT_PIN( PORT0, CS1_PIN, IO_GIO);
    IO_FUNC_PORT_PIN( PORT1, CLK_PIN, IO_GIO);
    IO_FUNC_PORT_PIN( PORT1, SI_PIN, IO_GIO);
    IO_FUNC_PORT_PIN( PORT1, SO_PIN, IO_GIO);

    IO_DIR_PORT_PIN(PORT0, CS1_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT1, CLK_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT1, SI_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT1, SO_PIN, IO_IN);

    FONT_CS = 1; //�ֿ�оƬƬѡ����
//++++++++++++++++++++++OLED IO�ڶ���++++++++++++++++++//
    IO_FUNC_PORT_PIN( PORT1, CS_PIN, IO_GIO );
    IO_FUNC_PORT_PIN( PORT0, DC_PIN, IO_GIO );
    IO_FUNC_PORT_PIN( PORT1, SCLK_PIN, IO_GIO);
    IO_FUNC_PORT_PIN( PORT1, SDIN_PIN, IO_GIO);
    IO_FUNC_PORT_PIN( PORT0, RES_PIN, IO_GIO);

    IO_DIR_PORT_PIN(PORT1, CS_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT0, DC_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT1, SCLK_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT1, SDIN_PIN, IO_OUT);
    IO_DIR_PORT_PIN(PORT0, RES_PIN, IO_OUT);

//++++++++++++++++++++++MOTOR IO�ڶ���++++++++++++++++++//
    IO_FUNC_PORT_PIN( PORT2, MOTOR_PIN, IO_GIO );
    IO_DIR_PORT_PIN( PORT2, MOTOR_PIN, IO_OUT);
}
void BSP_InitInterrupt( void )
{
    /***************Key Interrupt Init****************/
    IO_FUNC_PORT_PIN(0,1, IO_GIO );
    IO_FUNC_PORT_PIN(0,2, IO_GIO );
    IO_FUNC_PORT_PIN(0,3, IO_GIO );
    IO_FUNC_PORT_PIN(0,6, IO_GIO );
    
    IO_DIR_PORT_PIN(0,1, IO_IN);
    IO_DIR_PORT_PIN(0,2, IO_IN);
    IO_DIR_PORT_PIN(0,3, IO_IN);
    IO_DIR_PORT_PIN(0,6, IO_IN);
    
    IO_IMODE_PORT_PIN(0,1,IO_PUD);
    IO_IMODE_PORT_PIN(0,2,IO_PUD);
    IO_IMODE_PORT_PIN(0,3,IO_PUD);
    IO_IMODE_PORT_PIN(0,6,IO_PUD);
    
    IO_PUD_PORT(0,IO_PUP);

    IO_FUNC_PORT_PIN(0,5, IO_PER );
    IO_DIR_PORT_PIN(0,5, IO_IN);
    IO_IMODE_PORT_PIN(0,5,IO_TRI);
    APCFG |= 0x20;  /**< P0.5 ΪADC�ڲ��ص��� */
    
    PICTL |= 0x01;  /**< Falling edge �½��� */
    P0IEN |= 0x4E;  /**< 0100 1110 P0.1236 Enable interrupt Mask */
    IEN1  |= 0x20;  /**< BV(5)P0 Interrupt enable */
    P0IFG  = 0x00;  /**< Clear any pending interrupt */
}
/********************************************************************************************************

********************************************************************************************************/

/********************************************************************************************************
*
********************************************************************************************************/

/********************************************************************************************************

********************************************************************************************************/
void BSP_Init(void)
{
    BSP_InitIOPortFun();
    BSP_InitInterrupt();
    KEY_Init();
    FONT_Init(); 
    MOTOR_Init();
    POWER_Init();
    MENU_Init();
}
/********************************************************************************************************

********************************************************************************************************/
void BSP_ProcessEvent(void)
{
    
}
/********************************************************************************************************
*									��ʾ��������								
********************************************************************************************************/
BspEvent_t BSP_DisOffOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_OPEN:
		case BSP_KEY_REPLY:
		case BSP_KEY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN:
        
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
		case BSP_KEY_LONG_OPEN_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:

			break;
		default:
			MENU_DisOffMsg();
			break;
	}
	
	return event;

}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisListMsgOperate(BspEvent_t event)
{
    switch(event)
    {
		case BSP_KEY_OPEN:
            if(MENU_MsgSize() == 0)
            {
                /**< �޲��� */
                event = BSP_KEY_LONG_OPEN_REPLY_NEXT;
            }
            break;
		case BSP_KEY_REPLY:
            break;
		case BSP_KEY_NEXT:
            MENU_DisPgdnKeyMsg(TRUE);    
            break;
		case BSP_KEY_LONG_OPEN:
            if(SysSetStoreValue.EnShutdown == 0x00)
            {
                /**< �޲��� */
                event = BSP_KEY_LONG_OPEN_REPLY_NEXT;
            }
            break;
		case BSP_KEY_LONG_REPLY:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_read(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            
            SysSetValue =  SysSetStoreValue;
            SysSetValue.UserPassword.Data = 0x0000;
            SysSetValue.SysPassword.Data  = 0x0000;
            break;
        case BSP_KEY_LONG_NEXT:
            MENU_DisDeleteShortMsg(FALSE);
            break;
        case BSP_KEY_LONG_OPEN_REPLY:
            MENU_DisRefreshMsg();
            MENU_DisPgdnKeyMsg(FALSE);  /**<  �������������и���ͼƬ */
            break;
		case BSP_KEY_LONG_REPLY_NEXT:
            MENU_DisRefreshMsg();
            MENU_DisPgdnKeyMsg(FALSE);
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
            
            MENU_DisDeceiveMsg();
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            MENU_DisResetMsg();
            MENU_ResetWriterMsg();
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER: 
            break;
        case BSP_INC_POWER:
            if(MAIN_POWER)
            {
                event = BSP_KEY_LONG_OPEN_REPLY_NEXT;
            }
            break;
        case BSP_TIME_FLASH:
			break;
		default:
			MENU_DisListMsg();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisOpenMsgOperate(BspEvent_t event)
{
    Rf2PcMsg_t   Rf2PcMsg;
    
    switch(event)
    {
		case BSP_KEY_OPEN:
        
            break;
		case BSP_KEY_REPLY:
            /**< ��--0xA1F1 / ��--0xA1F0 */
            if(MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF1)
            {
                MMsg.Buf[MMsg.DisStartPtr].Msg[1] = 0xF0;
                
                Rf2PcMsg.MsgType    = RF_W2P_OPEN_SHORT_MSG;
                Rf2PcMsg.TaskID     = MMsg.Buf[MMsg.DisStartPtr].TaskID;
                Rf2PcMsg.MsgLen     = 0;        
                HBee_EndRelpyMsg((RfPc2EndMsg_t*)&Rf2PcMsg);
            }
            break;
		case BSP_KEY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN:
            if(SysSetStoreValue.EnShutdown == 0x00)
            {
                /**< �޲��� */
                event = BSP_KEY_LONG_OPEN_REPLY_NEXT;
            }
            break;
		case BSP_KEY_LONG_REPLY:
            MENU_DisDeleteShortMsg(TRUE);
            break;
        case BSP_KEY_LONG_NEXT:
            break;
        case BSP_KEY_LONG_OPEN_REPLY:
            
            break;
		case BSP_KEY_LONG_REPLY_NEXT:
            
            /**< MENU_ReSortMsg ������������� */
            if(MMsg.Buf[MMsg.DisStartPtr].MsgType == RF_P2D_NONE_MSG)
            {
                /**< ����򿪵���Ϣ�Ǿͷ��������� */
                event = BSP_KEY_OPEN;
            }
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisOpenMsg();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisLowPowerOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
		case BSP_KEY_REPLY:
		case BSP_KEY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN:
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
		case BSP_KEY_LONG_OPEN_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
                MENU_DisLowPower();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisIncPowerOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
		case BSP_KEY_REPLY:
		case BSP_KEY_NEXT:
		case BSP_KEY_LONG_OPEN:
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
		case BSP_KEY_LONG_OPEN_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:
            break;
        case BSP_INC_POWER:
            break;
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisIncPower();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisUserPasswordOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 5)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.UserPassword.Bit.b0++;
                if(SysSetValue.UserPassword.Bit.b0 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b0 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 3)
            {
                SysSetValue.UserPassword.Bit.b1++;
                if(SysSetValue.UserPassword.Bit.b1 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b1 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 4)
            {
                SysSetValue.UserPassword.Bit.b2++;
                if(SysSetValue.UserPassword.Bit.b2 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b2 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 5)
            {
                SysSetValue.UserPassword.Bit.b3++;
                if(SysSetValue.UserPassword.Bit.b3 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b3 = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue.UserPassword.Data = 0x0000;
            SysSetValue.SysPassword.Data  = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            if((SysSetValue.UserPassword.Data != SysSetStoreValue.UserPassword.Data)&&(SysSetValue.UserPassword.Data != 0x1127))
            {
                /**< ����������� �˳������������ */
                event = BSP_KEY_NEXT;
            }
            else
            {
                SysSetValue =  SysSetStoreValue;
                SysSetValue.SysPassword.Data = 0x0000;
            }
            break;
		case BSP_KEY_LONG_REPLY:
            break;
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            if(SysSetValue.UserPassword.Data != SysSetStoreValue.SysPassword.Data)
            {
                /**< ����������� �˳������������ */
                event = BSP_KEY_NEXT;
            }
            else
            {
                SysSetValue = SysSetStoreValue;
                SysSetValue.SysPassword.Data = 0x0000;
            }
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisUserPassword();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisUserSetListOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
		case BSP_KEY_REPLY:
		case BSP_KEY_NEXT:
		case BSP_KEY_LONG_OPEN:
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
		case BSP_KEY_LONG_OPEN_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisUserSetList();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisUserSetTipNumberOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:

            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.TipNumber++;
                if(SysSetValue.TipNumber > 5)
                {
                    SysSetValue.TipNumber = 1;    
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;

            SysSetStoreValue.TipNumber = SysSetValue.TipNumber;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            MENU_DisResetMsg();
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisUserSetTipNumber();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisUserSetKeyMotorOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.KeyMotor++;
                if(SysSetValue.KeyMotor > 1)
                {
                    SysSetValue.KeyMotor = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            SysSetStoreValue.KeyMotor = SysSetValue.KeyMotor;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            MENU_DisResetMsg();
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisUserSetKeyMotor();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisUserSetPasswordOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 5)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.UserPassword.Bit.b0++;
                if(SysSetValue.UserPassword.Bit.b0 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b0 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 3)
            {
                SysSetValue.UserPassword.Bit.b1++;
                if(SysSetValue.UserPassword.Bit.b1 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b1 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 4)
            {
                SysSetValue.UserPassword.Bit.b2++;
                if(SysSetValue.UserPassword.Bit.b2 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b2 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 5)
            {
                SysSetValue.UserPassword.Bit.b3++;
                if(SysSetValue.UserPassword.Bit.b3 >= 10)
                {
                    SysSetValue.UserPassword.Bit.b3 = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            SysSetStoreValue.UserPassword.Data  = SysSetValue.UserPassword.Data;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            MENU_DisResetMsg();
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisUserSetPassword();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysPasswordOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 5)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.SysPassword.Bit.b0++;
                if(SysSetValue.SysPassword.Bit.b0 >= 10)
                {
                    SysSetValue.SysPassword.Bit.b0 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 3)
            {
                SysSetValue.SysPassword.Bit.b1++;
                if(SysSetValue.SysPassword.Bit.b1 >= 10)
                {
                    SysSetValue.SysPassword.Bit.b1 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 4)
            {
                SysSetValue.SysPassword.Bit.b2++;
                if(SysSetValue.SysPassword.Bit.b2 >= 10)
                {
                    SysSetValue.SysPassword.Bit.b2 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 5)
            {
                SysSetValue.SysPassword.Bit.b3++;
                if(SysSetValue.SysPassword.Bit.b3 >= 10)
                {
                    SysSetValue.SysPassword.Bit.b3 = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            if(SysSetValue.SysPassword.Data != SysSetStoreValue.SysPassword.Data)
            {
                /**< ����������� �˳������������ */
                event = BSP_KEY_NEXT;
            }
            else
            {
                SysSetValue = SysSetStoreValue;
                SysSetValue.SysPassword.Data = 0x0000;
            }
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysPassword();
			break;
	}
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysGetRfParamOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            /*
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }*/
            MenuFlg.SelectBit = 1;
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.AutoGetRf++;
                if(SysSetValue.AutoGetRf > 1)
                {
                    SysSetValue.AutoGetRf = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            SysSetStoreValue.AutoGetRf = SysSetValue.AutoGetRf;
            
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            
            if(SysSetStoreValue.AutoGetRf == 1)
            {
                osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            }
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:  
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysGetRfParam();
			break;
	}
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysSetChannelOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.Channel++;
                if(SysSetValue.Channel > 6)
                {
                    SysSetValue.Channel = 0;    
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            /**< �ر�֮ǰ���Զ���ȡ�ŵ� */
            SysSetStoreValue.AutoGetRf = 0;
            
            SysSetStoreValue.Channel = SysSetValue.Channel;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysSetChannel();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysSetPanidOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 5)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.PANID.Bit.b0++;
                if(SysSetValue.PANID.Bit.b0 >= 16)
                {
                    SysSetValue.PANID.Bit.b0 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 3)
            {
                SysSetValue.PANID.Bit.b1++;
                if(SysSetValue.PANID.Bit.b1 >= 16)
                {
                    SysSetValue.PANID.Bit.b1 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 4)
            {
                SysSetValue.PANID.Bit.b2++;
                if(SysSetValue.PANID.Bit.b2 >= 16)
                {
                    SysSetValue.PANID.Bit.b2 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 5)
            {
                SysSetValue.PANID.Bit.b3++;
                if(SysSetValue.PANID.Bit.b3 >= 16)
                {
                    SysSetValue.PANID.Bit.b3 = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;

            SysSetStoreValue.PANID.Data  = SysSetValue.PANID.Data;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysSetPanid();
			break;
	}
	
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysSetAddrOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
        
            /**< 2530�������õ�ַ 
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 9)
            {
                MenuFlg.SelectBit = 2;
            }
            */
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.Addr.Bit.b0++;
                if(SysSetValue.Addr.Bit.b0 >= 16)
                {
                    SysSetValue.Addr.Bit.b0 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 3)
            {
                SysSetValue.Addr.Bit.b1++;
                if(SysSetValue.Addr.Bit.b1 >= 16)
                {
                    SysSetValue.Addr.Bit.b1 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 4)
            {
                SysSetValue.Addr.Bit.b2++;
                if(SysSetValue.Addr.Bit.b2 >= 16)
                {
                    SysSetValue.Addr.Bit.b2 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 5)
            {
                SysSetValue.Addr.Bit.b3++;
                if(SysSetValue.Addr.Bit.b3 >= 16)
                {
                    SysSetValue.Addr.Bit.b3 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 6)
            {
                SysSetValue.Addr.Bit.b4++;
                if(SysSetValue.Addr.Bit.b4 >= 16)
                {
                    SysSetValue.Addr.Bit.b4 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 7)
            {
                SysSetValue.Addr.Bit.b5++;
                if(SysSetValue.Addr.Bit.b5 >= 16)
                {
                    SysSetValue.Addr.Bit.b5 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 8)
            {
                SysSetValue.Addr.Bit.b6++;
                if(SysSetValue.Addr.Bit.b6 >= 16)
                {
                    SysSetValue.Addr.Bit.b6 = 0;
                }
            }
            else if(MenuFlg.SelectBit == 9)
            {
                SysSetValue.Addr.Bit.b7++;
                if(SysSetValue.Addr.Bit.b7 >= 16)
                {
                    SysSetValue.Addr.Bit.b7 = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            SysSetStoreValue.Addr.Data  = SysSetValue.Addr.Data;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            break;
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysSetAddr();
			break;
	}
	
	return event;

}


/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysSetShowModeOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.DisMode++;
                if(SysSetValue.DisMode >= 4)
                {
                    SysSetValue.DisMode = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:

            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            SysSetStoreValue.DisMode = SysSetValue.DisMode;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysSetShowMode();
			break;
	}
	
	return event;
}

/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysEnShutDownOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.EnShutdown++;
                if(SysSetValue.EnShutdown >1)
                {
                    SysSetValue.EnShutdown = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            
            SysSetStoreValue.EnShutdown = SysSetValue.EnShutdown;
            osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
            osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisEnShutDown();
			break;
	}
	return event;
}
/********************************************************************************************************
*										
********************************************************************************************************/
BspEvent_t BSP_DisSysResetToDefaultOperate(BspEvent_t event)
{
    switch(event)
    {    
		case BSP_KEY_OPEN:
            MenuFlg.SelectBit++;
            if(MenuFlg.SelectBit > 2)
            {
                MenuFlg.SelectBit = 2;
            }
            break;
		case BSP_KEY_REPLY:
            if(MenuFlg.SelectBit == 2)
            {
                SysSetValue.ResetToDefault++;
                if(SysSetValue.ResetToDefault >= 4)
                {
                    SysSetValue.ResetToDefault = 0;
                }
            }
            break;
		case BSP_KEY_NEXT:

            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetValue = SysSetStoreValue;
            SysSetValue.SysPassword.Data = 0x0000;
            break;
		case BSP_KEY_LONG_OPEN:
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            SysSetStoreValue.ResetToDefault = SysSetValue.ResetToDefault;
            if(SysSetStoreValue.ResetToDefault == 1)
            {
                MENU_RestetDefault();
                osal_start_timerEx(MSA_TaskId,HBEEAPP_MCU_REST_EVT,1000);
            }
            break;
		case BSP_KEY_LONG_REPLY:
        case BSP_KEY_LONG_NEXT:
        case BSP_KEY_LONG_OPEN_REPLY:
		case BSP_KEY_LONG_REPLY_NEXT:
            break;
		case BSP_KEY_LONG_OPEN_NEXT:
        
            MenuFlg.ShowFlash = 0;
            MenuFlg.SelectBit = 0;
            break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
        case BSP_LOW_POWER:        
        case BSP_INC_POWER:
        case BSP_TIME_FLASH:
			break;
		default:
            MENU_DisSysResetToDefault();
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
		case BSP_KEY_OPEN:
			funcIndex = MenuTotalTab[MenuFuncIndex].OpenIndex;
			break;
		case BSP_KEY_REPLY:
			funcIndex = MenuTotalTab[MenuFuncIndex].ReplyIndex;
			break;
		case BSP_KEY_NEXT:
			funcIndex = MenuTotalTab[MenuFuncIndex].NextIndex;
			break;
		case BSP_KEY_LONG_OPEN:
			funcIndex = MenuTotalTab[MenuFuncIndex].OpenLongIndex;
			break;
		case BSP_KEY_LONG_REPLY:
			funcIndex = MenuTotalTab[MenuFuncIndex].ReplyLongIndex;
			break;
	    case BSP_KEY_LONG_NEXT:
	        funcIndex = MenuTotalTab[MenuFuncIndex].NextLongIndex;
	        break;
        case BSP_KEY_LONG_OPEN_REPLY:
            funcIndex = MenuTotalTab[MenuFuncIndex].OpenReplyLongIndex;
            break;
		case BSP_KEY_LONG_REPLY_NEXT:
			funcIndex = MenuTotalTab[MenuFuncIndex].ReplyNextLongIndex;
			break;
		case BSP_KEY_LONG_OPEN_NEXT:
			funcIndex = MenuTotalTab[MenuFuncIndex].OpenNextLongIndex;
			break;
        case BSP_KEY_LONG_OPEN_REPLY_NEXT:
            funcIndex = MenuTotalTab[MenuFuncIndex].OpenReplyNextLongIndex;
            break;
        case BSP_LOW_POWER:
            funcIndex = MenuTotalTab[MenuFuncIndex].LowPowerIndex;
            break;
        case BSP_INC_POWER:
            funcIndex = MenuTotalTab[MenuFuncIndex].IncPowerIndex;
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

********************************************************************************************************/

