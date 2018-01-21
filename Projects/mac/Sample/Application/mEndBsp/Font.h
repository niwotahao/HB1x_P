
#ifndef _FONT_H_
#define _FONT_H_
/********************************************************************************************************

********************************************************************************************************/
#include "eBsp.h"

/********************************************************************************************************

********************************************************************************************************/
//#define CS1_PIN  3  //p1_3
#define CS1_PIN  2  //p2_2
#define CLK_PIN  7  //P1_7
#define SI_PIN   0  //P1_0
#define SO_PIN   6  //P1_6

#define FONT_SCLK   BNAME(PORT1, CLK_PIN)
#define FONT_MISO   BNAME(PORT1, SO_PIN)
#define FONT_MOSI   BNAME(PORT1, SI_PIN)
//#define FONT_CS     BNAME(PORT1, CS1_PIN)
#define FONT_CS     BNAME(PORT2, CS1_PIN)
/********************************************************************************************************

********************************************************************************************************/
//+++++++++++++++++++++++++++++++�ַ���ʾ����+++++++++++++++++++++++++++//
#define   TYPE_8  	0 //8  ���ַ�
#define   TYPE_12  	1 //12 �㺺���ַ�
#define   TYPE_16  	2 //16 �㺺���ַ�
//+++++++++++++++++++++++++��GT21L16S2WоƬ�����ݵĵ�ַ++++++++++++++++//
#define ASC0808D2HZ_ADDR  	( 0x66c0 ) 		        //7*8 ascii code
#define ASC0812M2ZF_ADDR    ( 0x66d40 )   		    //6*12 ascii code
#define GBEX0816ZF_ADDR     ( 0x3c2c0 )             // 243648   //8*16 ascii code

#define ZF1112B2ZF_ADDR     ( 0x3cf80 )	   		    //12*12 12���ַ�
#define HZ1112B2HZ_ADDR     ( 0x3cf80+376*24 )	    //12*12 12�㺺��

#define CUTS1516ZF_ADDR     0x00  				    //16*16 16���ַ�
#define JFLS1516HZ_ADDR     27072  			        //16*16 16�㺺��
#define xx                  0x1dd780
//+++++++++++++++++++++++++++++����+++++++++++++++++++++++++++++++++++//
#define ASCII0507ZF_ADDR        245696
#define ARIAL_16B0_ADDR         246464
#define ARIAL_12B0_ADDR         422720
#define SPAC1616_ADDR           425264
#define GB2311ToUnicode_addr    (12032)
#define UnicodeToGB2311_addr    (425328)
/********************************************************************************************************

********************************************************************************************************/
extern const __code uint8 DzkCode[][16];

/********************************************************************************************************

********************************************************************************************************/
extern void FONT_Init(void);
extern uint32 FONT_GB2312Addr(uint8 *ss,uint8 type);
extern void FONT_ShowHanZi(uint32 addr, uint8 hanzipage, uint8 hanzifont,uint8 of,uint16 fcolor,uint16 bcolor);
extern void FONT_ShowAscii(uint32 addr, uint8 hanzipage, uint8 hanzifont,uint8 of,uint16 fcolor,uint16 bcolor);
extern void FONT_ShowAscii2(uint32 addr, uint8 hanzipage, uint8 hanzifont,uint8 of,uint16 fcolor,uint16 bcolor);
/********************************************************************************************************

********************************************************************************************************/
#endif

