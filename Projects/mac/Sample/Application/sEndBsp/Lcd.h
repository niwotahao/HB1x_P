
#ifndef _LCD_H_
#define _LCD_H_
/********************************************************************************************************

********************************************************************************************************/
#include "eBsp.h"
/********************************************************************************************************

********************************************************************************************************/
#define CS_PIN      2   //P1_2
#define DC_PIN      0   //P0_0
#define SCLK_PIN    5  //P1_5
#define SDIN_PIN    3  //P1_3
#define RES_PIN     4  //P0_4

#define OLED_CS     BNAME(PORT1, CS_PIN)
#define OLED_DC     BNAME(PORT0, DC_PIN)
#define OLED_SCLK   BNAME(PORT1, SCLK_PIN)
#define OLED_MOSI   BNAME(PORT1, SDIN_PIN)
#define OLED_RES    BNAME(PORT0, RES_PIN)
//+++++++++++++++++++++++++++++++OLED点阵位置定义++++++++++++++++++//
#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel			((XLevelH&0x0F)*16+XLevelL)
#define Max_Column		128
#define Max_Row			64
#define	Brightness		0xCF
/*******************************************************************************************************/
#define IsLcdSleep()    (LcdWorkFlag == FALSE)
/********************************************************************************************************

********************************************************************************************************/
extern uint8 LcdWorkFlag;
/********************************************************************************************************

********************************************************************************************************/
void LCD_Init(void);

void LCD_WriteCommand(uint8 Data);
void LCD_WriteData(uint8 Data);

void LCD_SetStartColumn(uint8 d);
void LCD_SetAddressingMode(uint8 d);
void LCD_SetColumnAddress(uint8 a, uint8 b);
void LCD_SetPageAddress(uint8 a, uint8 b);
void LCD_SetStartLine(uint8 d);
void LCD_SetContrastControl(uint8 d);
void LCD_SetChargePump(uint8 d);
void LCD_SetSegmentRemap(uint8 d);
void LCD_SetEntireDisplay(uint8 d);
void LCD_SetInverseDisplay(uint8 d);
void LCD_SetMultiplexRatio(uint8 d);
void LCD_SetDisplayOnOff(uint8 d);
void LCD_SetStartPage(uint8 d);
void LCD_SetCommonRemap(uint8 d);
void LCD_SetDisplayOffset(uint8 d);
void LCD_SetDisplayClock(uint8 d);
void LCD_SetPrechargePeriod(uint8 d);
void LCD_SetCommonConfig(uint8 d);
void LCD_SetVCOMH(uint8 d);
void LCD_SetNOP(void);

void LCD_SetWork(BOOL on);
/********************************************************************************************************

********************************************************************************************************/
#endif









