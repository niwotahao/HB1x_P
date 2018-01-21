
#ifndef _LCD_H_
#define _LCD_H_
/********************************************************************************************************

********************************************************************************************************/
#include "eBsp.h"
/********************************************************************************************************

********************************************************************************************************/
#define CS_PIN      2  //P1_2
#define DC_PIN      0  //P0_0
#define SCLK_PIN    5  //P1_5
#define SDIN_PIN    6  //P1_6
#define RES_PIN     3  //P2_3
#define LIGHT_PIN   7  //P0_7


#define OLED_CS     BNAME(PORT1, CS_PIN)
#define OLED_DC     BNAME(PORT0, DC_PIN)
#define OLED_SCLK   BNAME(PORT1, SCLK_PIN)
#define OLED_MOSI   BNAME(PORT1, SDIN_PIN)
#define OLED_RES    BNAME(PORT2, RES_PIN)
#define OLED_LIGHT  BNAME(PORT0, LIGHT_PIN)
//+++++++++++++++++++++++++++++++OLED点阵位置定义++++++++++++++++++//
#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel			((XLevelH&0x0F)*16+XLevelL)
#define Max_Column		128
#define Max_Row			128
//#define	Brightness		0xCF
//+++++++++++++++++++++++++++++++OLED颜色定义++++++++++++++++++//
#define WHITE         	 0xFFFF     //白色
#define BLACK         	 0x0000	    //黑色
#define BLUE         	 0x001F     //蓝色     
#define RED              0xF800     //红色
#define GREEN         	 0x07E0     //绿色
#define YELLOW        	 0xFFE0     //黄色
#define GRAY  			 0x8430     //灰色
#define PURPLE           0xF81F     //紫色

/*******************************************************************************************************/
#define IsLcdSleep()    (LcdWorkFlag == FALSE)
/********************************************************************************************************

********************************************************************************************************/
extern uint8 LcdWorkFlag;
/********************************************************************************************************

********************************************************************************************************/
void DelayForUs(uint16 microSecs);
void LCD_ConfigSPI(void);
void LCD_Init(void);
void LCD_WriteCommand(uint8 Data);
void LCD_WriteData(uint8 Data);
void LCD_Write_TwoData(uint16 Data);
void LCD_SetStartColumn(uint8 ys, uint8 ye);
void LCD_SetStartPage(uint8 xs, uint8 xe);
void Fill_Area_Color(uint8 xs, uint8 ys, uint8 xe, uint8 ye, uint16 color);
void LCD_SetWork(BOOL on);

/********************************************************************************************************

********************************************************************************************************/
#endif









