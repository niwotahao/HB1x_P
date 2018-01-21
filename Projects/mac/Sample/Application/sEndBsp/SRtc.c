/********************************************************************************************************
*FileName:	SW_RTC.c
*Processor: PIC18F67J11+AT86RF212
*Complier:	MCC18 | HI-TECH18
*Company:   WWW.OURZIGBEE.COM
*********************************************************************************************************
*Author			Date				Comment
*NIWOTA			2010/8/28			Original
*NIWOTA			2010/9/13			 丰利源
********************************************************************************************************/
#include "eBsp.h"
/********************************************************************************************************
*										
********************************************************************************************************/
SystemTime_t	SystemTime;                 //hex形式

uint8 const DayTab[2][13]=
{
	{0,31,28,31,30,31,30,31,31,30,31,30,31},
	{0,31,29,31,30,31,30,31,31,30,31,30,31}
};
/********************************************************************************************************
*										
********************************************************************************************************/
void SRTC_Init(void)
{
	/*显示时间初始化*/
	SystemTime.Second	    = SYS_INIT_SECOND;	
	SystemTime.Minute		= SYS_INIT_MINUTE;
	SystemTime.Hour			= SYS_INIT_HOUR;
	SystemTime.Day			= SYS_INIT_DAY;	
	SystemTime.Weak			= SYS_INIT_WEAK; 
	SystemTime.Month		= SYS_INIT_MONTH;
	SystemTime.Year			= SYS_INIT_YEAR;
}
/********************************************************************************************************
*										
********************************************************************************************************/
void SRTC_ShowTime(void)
{
	MENU_ShowTime(SystemTime.Hour,SystemTime.Minute);
}
/********************************************************************************************************
*										
********************************************************************************************************/
void SRTC_ProcessEvent(void)
{
    SystemTime.Second++;
    if(SystemTime.Second > 59)
    {
        SystemTime.Second = 0;
        
        SystemTime.Minute++; 
        if(SystemTime.Minute >59)
        {
            SystemTime.Minute = 0;
            SystemTime.Hour++;
            if(SystemTime.Hour >23)
            {
                SystemTime.Hour = 0;
                SystemTime.Day++;
                if((SystemTime.Year%4)==0)     //润年
                {
                    if((SystemTime.Day) > (DayTab[1][SystemTime.Month]))
                    {
                        SystemTime.Day = 1;
                        SystemTime.Month++;
                    }
                }
                else
                {
                    if((SystemTime.Day) > (DayTab[0][SystemTime.Month]))
                    {
                        SystemTime.Day = 1;
                        SystemTime.Month++;
                    }
                }
                if(SystemTime.Month >12)
                {
                    SystemTime.Month = 1;
                    SystemTime.Year++;
                    if(SystemTime.Year > 99)
                    {
                        SystemTime.Year = 13;
                    }
                }
            }
        }
    }
}
/********************************************************************************************************
*										
********************************************************************************************************/


