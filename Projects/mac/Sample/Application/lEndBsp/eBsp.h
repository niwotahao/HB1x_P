
#ifndef _eBSP_H_
#define _eBSP_H_
/********************************************************************************************************

********************************************************************************************************/
#include "comdef.h"
/********************************************************************************************************

********************************************************************************************************/
#define UINT8BIT7(a)  ((a) & 0x80)
#define UINT8BIT6(a)  ((a) & 0x40)
#define UINT8BIT5(a)  ((a) & 0x20)
#define UINT8BIT4(a)  ((a) & 0x10)
#define UINT8BIT3(a)  ((a) & 0x08)
#define UINT8BIT2(a)  ((a) & 0x04)
#define UINT8BIT1(a)  ((a) & 0x02)
#define UINT8BIT0(a)  ((a) & 0x01)

#define BM(a,b,c) (((a)|(b) >> (c)))
#define BB(a)     (((a)==0) ? 0 : 1)
/********************************************************************************************************

********************************************************************************************************/
//#define  M_SPI            /**< LCD模拟SPI选项 */
/********************************************************************************************************

********************************************************************************************************/
typedef enum
{
	BSP_NONE = 0,
	BSP_KEY_OPEN,
	BSP_KEY_REPLY,
	BSP_KEY_NEXT,
    
	BSP_KEY_LONG_OPEN,
	BSP_KEY_LONG_REPLY,
	BSP_KEY_LONG_NEXT,
	BSP_KEY_LONG_OPEN_REPLY,
	BSP_KEY_LONG_REPLY_NEXT,
	BSP_KEY_LONG_OPEN_NEXT, 
	BSP_KEY_LONG_OPEN_REPLY_NEXT,
    BSP_LOW_POWER,
    BSP_INC_POWER,
    BSP_TIME_FLASH,
        
    BSP_MAX_EVENT = 0xF0
}BspEvent_t;
/********************************************************************************************************

********************************************************************************************************/
/**
用户模式
1.设置提示次数
2.设置按键震动
3.设置用户密码
管理模式
1.自动获取射频参数
2.设置频道
3.设置局域网号
4.设置地址
5.设置界面模式
6.设置充电接收
7.恢复出厂设置
*/
typedef enum
{
    DIS_OFF = 0,
    DIS_LIST_MSG,
    DIS_OPEN_MSG,
    
    DIS_LOW_POWER,
    DIS_INC_POWER,
    
    DIS_USER_PASSWORD,
	
    DIS_USER_SET_TIP_NUMBER,
    DIS_USER_SET_KEY_MOTOR,
    DIS_USER_SET_PASSWORD,
    
    DIS_SYS_PASSWORD,

    DIS_SYS_GET_RF_PARAM,
    DIS_SYS_SET_CHANNEL,
    DIS_SYS_SET_PANID,
    DIS_SYS_SET_ADDR,
    DIS_SYS_SET_SHOW_MODE,
    DIS_SYS_EN_SHUT_DOWN,
    DIS_SYS_RESET_TO_DEFAULT,
        
    DIS_NONE = 0xFF
}DisMenuIndex_t;
/********************************************************************************************************

********************************************************************************************************/
#include "hal_SPI1.h"
#include "Font.h"
#include "hal_Rkey.h"
#include "Lcd.h"
#include "Menu.h"
#include "Motor.h"
#include "bPower.h"
#include "MFRC522.h"
/********************************************************************************************************

********************************************************************************************************/
// General I/O definitions
#define IO_GIO  0  // General purpose I/O
#define IO_PER  1  // Peripheral function
#define IO_IN   0  // Input pin
#define IO_OUT  1  // Output pin
#define IO_PUD  0  // Pullup/pulldn input
#define IO_TRI  1  // Tri-state input
#define IO_PUP  0  // Pull-up input pin
#define IO_PDN  1  // Pull-down input pin

#define PORT0   0
#define PORT1   1
#define PORT2   2


/* I/O PORT CONFIGURATION */
#define CAT1(x,y) x##y  // Concatenates 2 strings
#define CAT2(x,y) CAT1(x,y)  // Forces evaluation of CAT1

// LCD port I/O defintions
// Builds I/O port name: PNAME(1,INP) ==> P1INP
#define PNAME(y,z) CAT2(P,CAT2(y,z))
// Builds I/O bit name: BNAME(1,2) ==> P1_2

#define BNAME(port,pin) CAT2(CAT2(P,port),CAT2(_,pin))

#define IO_DIR_PORT_PIN(port, pin, dir) 	\
{											\
	if ( dir == IO_OUT ) 					\
		PNAME(port,DIR) |= (1<<(pin)); 		\
	else 									\
		PNAME(port,DIR) &= ~(1<<(pin)); 	\
}


#define IO_FUNC_PORT_PIN(port, pin, func) 	\
{ 											\
	if( port < 2 ) 							\
	{ 										\
		if ( func == IO_PER ) 				\
			PNAME(port,SEL) |= (1<<(pin)); 	\
		else 								\
			PNAME(port,SEL) &= ~(1<<(pin)); \
	} 										\
	else 									\
	{ 										\
		if ( func == IO_PER )				\
			P2SEL |= (1<<(pin>>1)); 		\
		else 								\
			P2SEL &= ~(1<<(pin>>1)); 		\
	} 										\
}

#define IO_IMODE_PORT_PIN(port, pin, mode) 	\
{ 											\
	if ( mode == IO_TRI ) 					\
		PNAME(port,INP) |= (1<<(pin)); 		\
	else 									\
		PNAME(port,INP) &= ~(1<<(pin)); 	\
}

#define IO_PUD_PORT(port, dir) 				\
{ 											\
	if ( dir == IO_PDN ) 					\
		P2INP |= (1<<(port+5)); 			\
	else 									\
		P2INP &= ~(1<<(port+5)); 			\
}
/********************************************************************************************************

********************************************************************************************************/
extern uint8  MenuFuncIndex;
/********************************************************************************************************

********************************************************************************************************/
extern void BSP_Init(void);
extern void BSP_EventTask(BspEvent_t event);
/********************************************************************************************************

********************************************************************************************************/
#endif




