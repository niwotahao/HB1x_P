
/********************************************************************************************************

********************************************************************************************************/
#include "bPower.h"
#include "hal_adc.h"
/********************************************************************************************************

********************************************************************************************************/
void POWER_Init(void)
{
	MyBat = POWER_ReadBattery();
}
#if 0
/********************************************************************************************************

********************************************************************************************************/
uint8 POWER_BatteryCheck(void)
{
	uint8 i;
	uint32 mybatt=0;

    mybatt = mV;//>>2;
    
    /** 10位AD,基准电压1.65V接电源33k+10k接地
    3.6----> (10/43 =0.2326)*x*(1023/1.65=620)= 2232*(10/43) = 519 = 0x207
    3.7----> (10/43)*x*620 = 2294*(10/43) = 534 = 0x216
    3.8----> (10/43)*x*620 = 2356*(10/43) = 548 = 0x224
    3.9----> (10/43)*x*620 = 2418*(10/43) = 562 = 0x232
    4.0----> (10/43)*x*620 = 2480*(10/43) = 577 = 0x241
    4.1----> (10/43)*x*620 = 2542*(10/43) = 591 = 0x24F
    4.2----> (10/43)*x*620 = 2604*(10/43) = 606 = 0x25E
    */

	if( mybatt >= 0x24F )
	{
        i = BATT_HIG;
	}
	else if (  mybatt >= 0x232)
	{
        i = BATT_MID2;
	}  
	else if ( mybatt >= 0x207 )
	{
        i = BATT_MID1;
	}
	else
	{
        i = BATT_LOW;
	}
    return i;
}
#endif
/********************************************************************************************************

********************************************************************************************************/
#define HAL_ADC_REF_125V    0x00    /* Internal 1.25V Reference */
#define HAL_ADC_DEC_128     0x10    /* Decimate by 128 : 10-bit resolution */
#define HAL_ADC_DEC_256     0x20    /* Decimate by 256 : 12-bit resolution */
#define HAL_ADC_DEC_512     0x30    /* Decimate by 512 : 14-bit resolution */
/********************************************************************************************************

********************************************************************************************************/
uint16 POWER_ReadBattery(void)
{
    uint16 value;

    /* Clear ADC interrupt flag */
    ADCIF = 0;

    ADCCON3 = (HAL_ADC_REF_125V | HAL_ADC_DEC_512 | HAL_ADC_CHN_AIN5);//HAL_ADC_CHN_VDD3 HAL_ADC_CHN_AIN5

    /* Wait for the conversion to finish */
    while ( !ADCIF );

    /* Get the result */
    value = ADCL;
    value |= ((uint16) ADCH) << 8;
    value = value >> 2;

    return value;
}
/********************************************************************************************************

********************************************************************************************************/
uint32 MyBat=0;

uint8 POWER_BatteryCheck(void)
{
    uint8 n;

    /** 12位AD,基准电压1.25V ,接电源33k+10k接地
    3.6----> (10/43 =0.2326)*x*(4095/1.25=3276)= 11793.6*(10/43) = 2742 = 0x0AB6
    3.7----> (10/43)*x*3276 = 12121.2*(10/43) = 2818 = 0x0B02
    3.8----> (10/43)*x*3276 = 12448.8*(10/43) = 2895 = 0x0B4F
    3.9----> (10/43)*x*3276 = 12776.4*(10/43) = 2971 = 0x0B9B
    4.0----> (10/43)*x*3276 = 13104  *(10/43) = 3047 = 0x0BE7
    4.1----> (10/43)*x*3276 = 13431.6*(10/43) = 3123 = 0x0C33
    4.2----> (10/43)*x*3276 = 13759.2*(10/43) = 3199 = 0x0C7F
    */
    
    /** 11位AD,基准电压1.15V ,接电源33k+10k接地
    3.6----> (10/43 =0.2326)*x*(2047/1.15=1780)= 414*x = 1490 = 0x05D2
    3.7----> (10/43)*x*1780 = 414*x = 1532 = 0x05FC
    3.8----> (10/43)*x*1780 = 414*x = 1573 = 0x0625
    3.9----> (10/43)*x*1780 = 414*x = 1615 = 0x064F
    4.0----> (10/43)*x*1780 = 414*x = 1656 = 0x0678
    4.1----> (10/43)*x*1780 = 414*x = 1697 = 0x06A1
    4.2----> (10/43)*x*1780 = 414*x = 1739 = 0x06CB
    */
    
    /** 13位AD,基准电压1.15V ,接电源33k+10k接地
    3.5----> (10/43)*x*7123 = 1657*x = 5800 = 0x16A8
    3.6----> (10/43 =0.2326)*x*(8191/1.15=7123)= 1657*x = 5964 = 0x174C
    3.7----> (10/43)*x*7123 = 1657*x = 6131 = 0x17F2
    3.8----> (10/43)*x*7123 = 1657*x = 6297 = 0x1899
    3.9----> (10/43)*x*7123 = 1657*x = 6462 = 0x193E
    4.0----> (10/43)*x*7123 = 1657*x = 6628 = 0x19E4
    4.1----> (10/43)*x*7123 = 1657*x = 6794 = 0x1A90
    4.2----> (10/43)*x*7123 = 1657*x = 6959 = 0x1B2F
    */
    

    MyBat+=POWER_ReadBattery();
    MyBat+=POWER_ReadBattery();
    MyBat+=POWER_ReadBattery();
    
    MyBat >>=2;

    if(MyBat >= 0x19E4)//0x1A90
    {
        n=BAT_FULL;
    }
    else if(MyBat >= 0x193E)
    {
        n=BAT_HIG;
    }
    else if(MyBat >= 0x1899)
    {
        n=BAT_MID3;
    }
    else if(MyBat >= 0x17F2)
    {
        n=BAT_MID2;
    }
    else if(MyBat >= 0x174C)
    {
        n=BAT_MID1;
    }
    else if(MyBat >= 0x16A8)
    {
        n=BAT_LOW;
    }
    else
    {
        n = BAT_LIT;
    }
    return n;
}
/********************************************************************************************************

********************************************************************************************************/




