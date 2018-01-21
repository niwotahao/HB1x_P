/********************************************************************************************************
*FileName:	SW_RTC.h
*Processor: PIC18F67J11+AT86RF212
*Complier:	MCC18 | HI-TECH18
*Company:   WWW.OURZIGBEE.COM
*********************************************************************************************************
*Author			Date				Comment
*NIWOTA			2010/8/28			Original
*NIWOTA			2010/9/13			 丰利源
********************************************************************************************************/
#ifndef _SRtc_H_
#define _SRtc_H_

/********************************************************************************************************
*										秒寄存器位信息联合结构体
********************************************************************************************************/
typedef struct 
{
	uint8   Second;
	uint8	Minute;
	uint8	Hour;
	uint8	Day;
	uint8	Weak;
	uint8	Month;
	uint8	Year;	
}SystemTime_t;
/********************************************************************************************************
*										
********************************************************************************************************/
#define SYS_INIT_SECOND         (33)
#define SYS_INIT_MINUTE         (38)
#define SYS_INIT_HOUR           (17)
#define SYS_INIT_DAY            (24)
#define SYS_INIT_WEAK           (7)
#define SYS_INIT_MONTH          (3)
#define SYS_INIT_YEAR           (13)
/********************************************************************************************************
*										
********************************************************************************************************/
extern SystemTime_t	SystemTime;                 //hex形式
/********************************************************************************************************
*										
********************************************************************************************************/
extern void SRTC_Init(void);
extern void SRTC_ShowTime(void);
extern void SRTC_ProcessEvent(void);
/********************************************************************************************************
*										
********************************************************************************************************/




/********************************************************************************************************
*										
********************************************************************************************************/
#endif
