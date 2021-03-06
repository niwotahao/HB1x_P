#ifndef _HAL_RKEY_H_
#define _HAL_RKEY_H_
/********************************************************************************************************
*											
********************************************************************************************************/ 
#include <ioCC2530.h>
#include "hal_types.h"
#include "jBsp.h"
/********************************************************************************************************
*											常量宏定义
********************************************************************************************************/  
    /**< P0.0 P0.1 P0.2 P0.3*/
    #define KEY_PORT_INIT()             {   P0SEL &= ~(0x0F); P0DIR &= ~(0x0F);P0INP &= ~(0x0F);P2INP &= ~(0x20);\
                                            P1SEL &= ~(0x01); P1DIR &= ~(0x01);P1INP &= ~(0x01);P2INP &= ~(0x40);}

	#define KEY_PORT()					(((P0<<1)&0x1E)|(P1&0x01))

	#define KEY_INT_DISABLE()			{P0IEN &= ~0x0F;}
	#define KEY_INT_ENABLE()			{IEN1 |= 0x20;P0IEN |= 0x0F;PICTL |= 0x01;}/**< 下降沿触发中断 */
/********************************************************************************************************
*											常量宏定义
********************************************************************************************************/  
    #define LONG_KEY_MARK               (0x80)

    #define KEY_NO                      (0x1F)

    #define KEY_CLEAR                   (0x1D)
    #define KEY_SET                	    (0x17)
    #define KEY_DEC                     (0x0F)
    #define KEY_ADD                     (0x1B)
    #define KEY_AUTO_ADD                (0x1E)

    #define KEY_LONG_SET                (LONG_KEY_MARK|KEY_SET)
/********************************************************************************************************
*											常量宏定义
********************************************************************************************************/  

#define FIRST_LONG_KEY_CNT		200
#define NEXT_LONG_KEY_CNT		30
#define SHORT_KEY_CNT			4
/********************************************************************************************************
*											外部使用变量
********************************************************************************************************/  

/********************************************************************************************************
*											按键函数申明
********************************************************************************************************/  
extern void KEY_Init(void);
extern BspEvent_t KEY_Read(void);
/********************************************************************************************************
*											    结束
********************************************************************************************************/
#endif