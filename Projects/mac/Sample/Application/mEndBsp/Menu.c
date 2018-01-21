
/********************************************************************************************************

********************************************************************************************************/
#include "string.h"
#include "Font.h"
#include "Menu.h"
#include "Rf.h"

#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OSAL_PwrMgr.h"
#include "msa.h"

#include "Lcd.h"

/********************************************************************************************************

********************************************************************************************************/
MenuFlg_t MenuFlg;
__no_init MenuMsg_t MMsg;
RfPc2EndAreaMsg_t RfPc2EndAreaMsg;
SysSetStoreValue_t SysSetValue;         /**< 用户操作系统设置缓冲 */
SysSetStoreValue_t SysSetStoreValue;    /**< 系统设置值 */
const __code uint8 DisNumHexTab[17] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x00};

extern bool msa_IsStarted;

#define WHITE         	 0xFFFF     //白色
#define BLACK         	 0x0000	    //黑色
#define BLUE         	 0x001F     //蓝色     
#define RED              0xF800     //红色
#define GREEN         	 0x07E0     //绿色
#define YELLOW        	 0xFFE0     //黄色
#define GRAY  			 0x8430     //灰色
#define PURPLE           0xF81F     //紫色

#define ST7735S
#define FCOLOR           WHITE      //界面字体颜色
#define BCOLOR           BLACK      //界面背景颜色
#define FCOLOR2          WHITE      //手表信号，电量，时间的字体颜色
#define BCOLOR2          BLACK      //手表信号，电量，时间的背景颜色
#define WNFCOLOR         GREEN      //手表名称字体颜色
#define WNBCOLOR         BLACK      //手表名称背景颜色
#define FISRTCOLOR       GREEN      //一级呼叫颜色
#define SECONDCOLOR      YELLOW     //二级呼叫颜色
#define THIRDCOLOR       RED        //三级呼叫颜色

/********************************************************************************************************
                                       功率线-宽x高=12x8
********************************************************************************************************/
const __code uint8 TX_POWER[] =     
{
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,  //0  没有信号

	0x00,0x60,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,  //1  1个信号

	0x00,0x60,0x00,0x70,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,  //2

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x00,0x00,0x00,0x00,0x00,  //3

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x7C,0x00,0x00,0x00,0x00,  //4

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x7C,0x00,0x7E,0x00,0x00,  //5  5个信号

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x7C,0x00,0x7E,0x00,0x7F,  //6  全信号
        
    0x00,0x50,0x20,0x50,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00   //7 x接收

};
/********************************************************************************************************
                                      TX Icon信号图标-宽x高=6x8
********************************************************************************************************/
const __code uint8 TX_ICO[] =               
{
	0x00,0x03,0x05,0x7F,0x05,0x03,      
};
/********************************************************************************************************
                                      Batt Icon电量图标-宽x高=6x8
********************************************************************************************************/
const __code  uint8 DC_ICO[] =              
{
    0x00,0x00,0x00,0x00,0x00,0x00,
    0xFE,0x83,0x83,0x83,0x83,0xFE,
    0xFE,0xC3,0xC3,0xC3,0xC3,0xFE,
    0xFE,0xE3,0xE3,0xE3,0xE3,0xFE,
    0xFE,0xF3,0xF3,0xF3,0xF3,0xFE,
    0xFE,0xFB,0xFB,0xFB,0xFB,0xFE,
    0xFE,0xFF,0xFF,0xFF,0xFF,0xFE
  
};
/********************************************************************************************************
                                      "->"粗箭头图标-宽x高=16x16
********************************************************************************************************/
const __code uint8 ARROWS_ICO[] =              
{
	/*0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,
	0x00,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01
    
  0x00,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00,
0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00*/
  0x00,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x00,
0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00
};
/********************************************************************************************************
                                  电量图标显示
j:电量
********************************************************************************************************/
void MENU_ShowBatt( uint8 j)
{
    uint8 m,n,Font,Pick;

    //竖置横排-纵向取模-字节倒序
    for(m=0;m<6;m++)
    {
        Pick=DC_ICO[j*6+m];
        LCD_SetStartColumn(122+m,122+m);
        LCD_SetStartPage(0,0+8);

        LCD_WriteCommand(0x2C);
        
        for(n=0;n<8;n++)
        {
            Font=Pick; 
            if(((Font >> n) & 0x01) == 0x01)
            {
                LCD_Write_TwoData(FCOLOR2);    
            }
            else
            {
                LCD_Write_TwoData(BCOLOR2);
            }
        }
    }

}
/********************************************************************************************************
                                      信号图标显示                            
********************************************************************************************************/
void MENU_ShowTx(void)
{
    uint8 i,j,Font,Pick;
  
    //竖置横排-纵向取模-字节倒序
    for(i=0;i<6;i++)
    {
        Pick=TX_ICO[i];
        LCD_SetStartColumn(0+i,0+i);
        LCD_SetStartPage(0,0+8);

        LCD_WriteCommand(0x2C);
        
        for(j=0;j<8;j++)
        {
            Font=Pick; 
            if(((Font >> j) & 0x01) == 0x01)
            {
                LCD_Write_TwoData(FCOLOR2);    
            }
            else
            {
                LCD_Write_TwoData(BCOLOR2);
            }
        }
    }
}
/********************************************************************************************************
                                  信号线显示
i:信号线强度
********************************************************************************************************/
void MENU_ShowTxPOWER(uint8 i)
{
    uint8 m,n,Font,Pick;

    //竖置横排-纵向取模-字节倒序
    for(m=0;m<12;m++)
    {
        Pick=TX_POWER[i*12+m];
        LCD_SetStartColumn(6+m,6+m);
        LCD_SetStartPage(0,0+8);
		
        LCD_WriteCommand(0x2C);
        
        for(n=0;n<8;n++)
        {
            Font=Pick; 
            if(((Font >> n) & 0x01) == 0x01)
            {
                LCD_Write_TwoData(FCOLOR2);    
            }
            else
            {
                LCD_Write_TwoData(BCOLOR2);
            }
        }
    }

}
/********************************************************************************************************
                                      箭头图标
********************************************************************************************************/
void MENU_ShowArrows(uint8 page)
{
    uint8 i,j,Font,Pick;

    //竖置横排-纵向取模-字节倒序
    for(i=0;i<32;i++)
    {
        Pick=ARROWS_ICO[i];
        
        if(i < 16)
        {
            LCD_SetStartColumn(i,i);
            LCD_SetStartPage(page,page+8);
        }
        else
        {
            LCD_SetStartColumn((i-16),(i-16));
            LCD_SetStartPage(page+8,page+16);
        }
        
        //LCD_SetStartColumn(i,i);
        //LCD_SetStartPage(page,page+32);

        LCD_WriteCommand(0x2C);
        
        for(j=0;j<8;j++)
        {
            Font=Pick; 
            if(((Font >> j) & 0x01) == 0x01)
            {
                LCD_Write_TwoData(FCOLOR2);    
            }
            else
            {
                LCD_Write_TwoData(BCOLOR2);
            }
        }
    }
}
/********************************************************************************************************
                                  区域清屏
xs:列起始地址
ys:行起始地址
xe:列结束地址
ye:行结束地址
********************************************************************************************************/
void MENU_ClearScreen(uint8 xs, uint8 ys, uint8 xe, uint8 ye) 
{
    uint8 i,j;
	
	#ifdef ILI9163C
	LCD_WriteCommand(0x2A);  //Colulm Address Set
	LCD_WriteData(0x00);			
	LCD_WriteData(xs);     			
	LCD_WriteData(0x00);		
	LCD_WriteData(xe);     			

	LCD_WriteCommand(0x2B);  //Page Address Set
	LCD_WriteData(0x00);		
	LCD_WriteData(ys);     			
	LCD_WriteData(0x00);		
	LCD_WriteData(ye); 
	#endif
	
	#ifdef ST7735S
	LCD_WriteCommand(0x2A);  //Colulm Address Set
	LCD_WriteData(0x00);			
	LCD_WriteData(xs+2);     //加2是因为ST7735S驱动的液晶的列地址需要偏移2，ILI9163C则不用偏移			
	LCD_WriteData(0x00);		
	LCD_WriteData(xe+2);     //加2是因为ST7735S驱动的液晶的列地址需要偏移2，ILI9163C则不用偏移			

	LCD_WriteCommand(0x2B);  //Page Address Set
	LCD_WriteData(0x00);		
	LCD_WriteData(ys+3);     //加3是因为ST7735S驱动的液晶的行地址需要偏移3，ILI9163C则不用偏移			
	LCD_WriteData(0x00);		
	LCD_WriteData(ye+3);     //加3是因为ST7735S驱动的液晶的行地址需要偏移3，ILI9163C则不用偏移			
    #endif		

	LCD_WriteCommand(0x2C); 

    #define OLCD_SPI_TX(x)                   { U0CSR &= ~(BV(2) | BV(1)); U0DBUF = x; while( !(U0CSR & BV(1)) ); }
    
    OLED_CS=0;
    OLED_DC=1;    
    for(i=0;i<=(xe-xs);i++)
    {
        for(j=0;j<=(ye-ys);j++)
        {
            OLCD_SPI_TX(BCOLOR>>8);
            OLCD_SPI_TX(BCOLOR);

            //LCD_Write_TwoData(BCOLOR);
        }
    }
    OLED_DC=1;
    OLED_CS=1;
}

/********************************************************************************************************
                                    显示字符串
data:字符串
datalen：字符串的长度
page：在液晶的哪一行开始显示
font：在液晶的哪一列开始显示
of:显示模式
fcolor:字符颜色
bcolor:背景颜色
********************************************************************************************************/
void MENU_ShowString(uint8 * data, uint8 datalen, uint8 page,uint8 font,uint8 of,uint16 fcolor,uint16 bcolor)
{
    uint8 i;
    for( i = 0; i <datalen;)
    {
        if(SysSetStoreValue.DisMode == 3)
        {
            /**< 显示字符全部被框起来了 */
            of &= 0x01;
                
            if(of)
            {
                if(i==0)
                {
                    /**< 第一个字 */
                    of |= 0x80;
                }
                else 
                {
                    if( ( (data[datalen-2] >= 0x80) && ( i== datalen-2) )  ||  (  (data[datalen-2] < 0x80) && (i== datalen-1) )  )
                    {
                        /**< 最后一个字 */
                        //of |= 0x40;
                    }
                }
            }
        }
        
        if(data[i]==0)              //判断是否有句子结束标识符，如果有则显示结束。
        {
            break;
        }
        else if (data[i] < 0x80)    //自定义font个数
        {
            if( font > 120  )       //如果显示满一行
            {
                font = 0;
                page +=16;
            }
            if( page > 112) //如果显示满一页
            {
                
                page = 16;     //未添加翻页显示。
            }
            MENU_ShowShuZi(data[i], page, font,of,fcolor,bcolor);
            font +=8;
            i++;
        }
        else
        {
            /**< 防止一页显示不完中文显示到下一行 */
            if((i+1)!=datalen)
            {
                if( font > 112 ) //如果显示满一行
                {
                    font = 0; 
                    page +=16;
                }
                if( page > 112) //如果显示满一页 
                {
                     
                    page = 16;     //未添加翻页显示。
                }
                FONT_ShowHanZi(FONT_GB2312Addr(data+i,TYPE_16),page,font,of,fcolor,bcolor);
                font +=16;
                i = i+2;    //每两个字节代表一个汉字
            }
            else
            {
                break;
            }
        }
    }
    
    /**< 完成一行显示 */
	if(of)
	{
        
        if(SysSetStoreValue.DisMode == 0)
        {
            return;
        }
		for( i = datalen; i <16;)
		{
			uint8 kong= 0x20;
            
			if( font > 120	) //如果显示满一行
			{
				font = 0;
				page +=16;
			}
			if( page > 112) //如果显示满一页
			{
				page = 16;	  //未添加翻页显示。
			}
            
            if(SysSetStoreValue.DisMode == 3)
            {
                of &= 0x01;
                if(i == 15)
                {
                    of |= 0x40;
                }
            }
			MENU_ShowShuZi(kong, page, font,of,fcolor,bcolor);
			//FONT_ShowAscii(FONT_GB2312Addr(&kong,TYPE_16),page,font,of);
			font +=8;
			i++;
		}
	}  

}
/********************************************************************************************************
                                  字库中的ASCII码显示时间
hour：小时（10进制）
minute：分钟（10进制）
********************************************************************************************************/
void MENU_ShowTime(uint8 hour,uint8 minute)
{
	uint8 k;
    uint8 n[5];
    static uint8 onff=0;

    n[0] = DisNumHexTab[hour/10];
    n[1] = DisNumHexTab[hour%10];
    if(onff == 1)
    {
        n[2] = ':';
    }
    else
    {
        n[2] = ' ';
    }
    onff =!onff;
    
    n[3] = DisNumHexTab[minute/10];
    n[4] = DisNumHexTab[minute%10];
    
    
    for(k=0;k<5;k++)
    {
        FONT_ShowAscii2(FONT_GB2312Addr(&n[k],TYPE_16),0,46+(k*8),0,FCOLOR2,BCOLOR2);
    }
    return;
    
}
/********************************************************************************************************
                                  ASCII码显示(非字库)
data：ASCII码(竖置横排-纵向取模-字节倒序-宽x高=8x16)
shuzipage：在液晶的哪一行开始显示
shuzifont：在液晶的哪一列开始显示
of:显示模式
fcolor:字符颜色
bcolor:背景颜色
********************************************************************************************************/
void MENU_ShowShuZi(uint8 data, uint8 shuzipage, uint8 shuzifont,uint8 of,uint16 fcolor,uint16 bcolor)
{
    uint8 n,i,j,Font;

    if(data >= 0x20)
    {
        data-=0x20;
    }
	
    //ASCII码显示
    //竖置横排-纵向取模-字节倒序
    for(i=0;i<16;i++)
    {
        n=*(DzkCode[data]+i);
        
        if(of)
        {
            if(SysSetStoreValue.DisMode == 0)
            {
                
                /**< 上下分别一行 */
                //n|= 0x01;
            }
            else if(SysSetStoreValue.DisMode == 1)
            {
                /**< 下面两行 */
            }
            else if(SysSetStoreValue.DisMode == 2)
            {
                /**< 反白 */
                n = ~n;
            }
            else
            {
                n|= 0x01;
                if(of&0x80)
                {
                    if(i==0)
                    {
                        n = 0xFF;
                    }
                }
                else if(of&0x40)
                {
                    if(i==7)
                    {
                        n = 0xFF;
                    }
                }
            } 
        }

        if(i < 8)
        {
            LCD_SetStartColumn(shuzifont+i,shuzifont+i);
            LCD_SetStartPage(shuzipage,shuzipage+8);
        }
        else
        {
            LCD_SetStartColumn((shuzifont+(i-8)),(shuzifont+(i-8)));
            LCD_SetStartPage(shuzipage+8,shuzipage+16);
        }

        LCD_WriteCommand(0x2C);
        
        for(j=0;j<8;j++)
        {
            Font=n;
            if(((Font >> j) & 0x01) == 0x01)
            {
                LCD_Write_TwoData(fcolor);    
            }
            else
            {
                LCD_Write_TwoData(bcolor);
            }
        }
    }
}
/********************************************************************************************************
                                          默认值定义
********************************************************************************************************/
void MENU_RestetDefault(void)
{
    uint8 *p;

    p = NLME_GetExtAddr();

    SysSetStoreValue.TipNumber              = 2;            /**< 1~5 */
    SysSetStoreValue.KeyMotor               = 1;            /**< 0~1 */     
    SysSetStoreValue.UserPassword.Data      = 0x0000;       /**< 0~0x9999 */ 
    SysSetStoreValue.Channel                = RF_DEFAULT_CHANNLE;       /**< 25~31  ---  11~17 ---- 0~6 */ 
    SysSetStoreValue.PANID.Data             = RF_DEFAULT_PANID;         /**< 0~0x9999 */ 

    SysSetStoreValue.NetID.Data             = p[7];         /**< 0~0x99999999 */
    SysSetStoreValue.NetID.Data           <<= 8;
    SysSetStoreValue.NetID.Data            |= p[6];
    SysSetStoreValue.NetID.Data           <<= 8;
    SysSetStoreValue.NetID.Data            |= p[5];
    SysSetStoreValue.NetID.Data           <<= 8;
    SysSetStoreValue.NetID.Data            |= p[4];

    SysSetStoreValue.Addr.Data              = p[3];         /**< 0~0x99999999 */
    SysSetStoreValue.Addr.Data            <<= 8;
    SysSetStoreValue.Addr.Data             |= p[2];
    SysSetStoreValue.Addr.Data            <<= 8;
    SysSetStoreValue.Addr.Data             |= p[1];
    SysSetStoreValue.Addr.Data            <<= 8;
    SysSetStoreValue.Addr.Data             |= p[0];

    SysSetStoreValue.SysPassword.Data       = 0x1530;       /**< 0~0x9999 */ 
    SysSetStoreValue.AutoGetRf              = 0;            /**< 0~1 */ 
    SysSetStoreValue.DisMode                = 2;            /**< 0~3 */ 
    SysSetStoreValue.DisSort                = 0;            /**< 0~1 */ 
    SysSetStoreValue.EnShutdown             = 1;            /**< 0~1 */  
    SysSetStoreValue.ResetToDefault         = 0;            /**< 0~1 */ 
    osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
    osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
}
/********************************************************************************************************
                                         初始化
********************************************************************************************************/
void MENU_Init(void)				
{
    /** oled 总共128页，1页==1行 */
	LCD_Init();
    
#if 1
    MENU_MsgBufInit();

	MENU_ClearScreen(0,0,127,127);		

    MENU_ShowTx();
    MENU_ShowTxPOWER(TX_POWER_X);
    MENU_ShowTime(12,0);
    MENU_ShowBatt(BAT_FULL);
	
	//LCD_SetWork(TRUE);
    MenuFlg.SacnKey   = 1;
    MenuFlg.ShutDown  = 0;
    MenuFlg.ShowFlash = 0;
    MenuFlg.SelectBit = 0;
    MenuFlg.Reserve   = 0;
    
    osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
    osal_nv_read(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
    if(SysSetStoreValue.Channel == 0xFF)
    {
        MENU_RestetDefault();
    } 
    osal_nv_item_init(WRITER_AREA_NAME_MSG_ID,sizeof(RfPc2EndAreaMsg_t),NULL);
    osal_nv_read(WRITER_AREA_NAME_MSG_ID,0,sizeof(RfPc2EndAreaMsg_t),(uint8*)&RfPc2EndAreaMsg);
    if(RfPc2EndAreaMsg.MsgType == 0xFF)
    {
        RfPc2EndAreaMsg.MsgType = 0x00;
        RfPc2EndAreaMsg.TaskID  = 0x00;
        RfPc2EndAreaMsg.MsgLen  = 0x09;
        
        RfPc2EndAreaMsg.Msg[0]  = 0xA1;
        RfPc2EndAreaMsg.Msg[1]  = 0xF9;
        RfPc2EndAreaMsg.Msg[2]  = 'T';
        RfPc2EndAreaMsg.Msg[3]  = 'i';
        RfPc2EndAreaMsg.Msg[4]  = 'n';
        RfPc2EndAreaMsg.Msg[5]  = 'y';
        RfPc2EndAreaMsg.Msg[6]  = 'B';
        RfPc2EndAreaMsg.Msg[7]  = 'e';
        RfPc2EndAreaMsg.Msg[8]  = 'e';
    }
    
    
#endif
}
/********************************************************************************************************
                                          信息缓存初始化
********************************************************************************************************/
void MENU_MsgBufInit(void)
{
    memset(&MMsg,0,sizeof(MenuMsg_t));
    
    MENU_ResetReadMsg();
}
/********************************************************************************************************
                                        
********************************************************************************************************/
uint8 MENU_MsgSize(void)
{
    return MMsg.Size;
}
/********************************************************************************************************
                                        返回可以存储数据的地址索引
********************************************************************************************************/
uint8 MENU_ReSortMsg(uint8 s)
{
    uint8 i,n;
    
    if(MMsg.Size > 0)
    {
        n = MMsg.Size-1;
        
        /**< 缓冲区满将缓冲最先接收到的清除 */
        for(i=s;i<n;i++)
        {
            MMsg.Buf[i] = MMsg.Buf[i+1];
        }
        if(s <= n)
        {
            MMsg.Size--;
        }
        MMsg.Buf[i].MsgType = RF_P2D_NONE_MSG;
        
        
        /**< 为了配合删除显示界面不混乱 */
        if(s < MMsg.DisStartPtr)
        {
           MMsg.DisStartPtr--; 
        }
        else if(s == MMsg.DisStartPtr)
        {
            MMsg.DisStartPtr = i;
        }
        
        return i;
    }
    return 0xFF;
}
/********************************************************************************************************

********************************************************************************************************/
RfPc2EndMsg_t* MENU_AppendMsg(void)
{
    uint8 n;
    
    n = MMsg.Size;
    
    /**< 判断缓冲区是否满 */
    if(MMsg.Size >= MENU_MSG_SIZE)
    {
        /**< 缓冲区满将缓冲最先接收到的清除 */
        n = MENU_ReSortMsg(0);
    }
    MMsg.Size++;
 
    return &MMsg.Buf[n];
}
/********************************************************************************************************

********************************************************************************************************/
RfPc2EndMsg_t *MENU_SearchOrRemoveMsg(RfPc2EndMsg_t *pMsg)
{
    uint8 i;
    
    if((pMsg->MsgType == RF_P2W_CALL_MSG)||(pMsg->MsgType == RF_P2W_SHORT_MSG))
    {
        /**< 接收到呼叫和短消息 */
        for(i=0;i<MMsg.Size;i++)
        {
            if((pMsg->MsgType == MMsg.Buf[i].MsgType)&&(pMsg->TaskID == MMsg.Buf[i].TaskID))
            {
                return &MMsg.Buf[i];
            }
        }
    }
    else if((pMsg->MsgType == RF_P2W_CANCEL_CALL_MSG)||(pMsg->MsgType == RF_P2W_CANCEL_SHORT_MSG))
    {
        /**< 接收到取消呼叫和短消息 */
        for(i=0;i<MMsg.Size;i++)
        {
            if(((pMsg->MsgType == RF_P2W_CANCEL_CALL_MSG)&&(MMsg.Buf[i].MsgType == RF_P2W_CALL_MSG)
				&&(pMsg->TaskID == (MMsg.Buf[i].TaskID|0x80000000)))||
               ((pMsg->MsgType == RF_P2W_CANCEL_SHORT_MSG)&&(MMsg.Buf[i].MsgType == RF_P2W_SHORT_MSG)
               &&(pMsg->TaskID == MMsg.Buf[i].TaskID)))
            {
                MENU_ReSortMsg(i);
                return &MMsg.Buf[i];
            }
        }
    }
    return NULL;
}
/********************************************************************************************************
                                          显示下一条信息
********************************************************************************************************/
void MENU_DisPgdnKeyMsg(BOOL nAdd)
{
        uint8 j,k;//添加
    uint8 *p;
    uint8 i,n,len,of=0;
    uint8 max = 0,start = 0;
    
    if(nAdd == TRUE)
    {
        n = MMsg.Size;
        MMsg.DisStartPtr++;
        if(MMsg.DisStartPtr >= n)
        {
            MMsg.DisStartPtr = 0;
        }
    }
    else
    {
        n = MMsg.Size;
        if((MMsg.DisStartPtr+1) >= n)
        {
            MMsg.DisStartPtr = 0;
            MMsg.DisEndPtr = 0;
        }
    }
    
    MMsg.DisEndPtr = MMsg.DisStartPtr - (MMsg.DisStartPtr%5);

    MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
    
    if(SysSetStoreValue.DisMode == 0)
    {
#if 1
        if(MMsg.Size)
        {
            j = 24+((MMsg.DisStartPtr%5)*20);
			MENU_ShowArrows(j);
        }
        max = 14;
        start = 16;     
#endif
    }
    else
    {
        max = 16;
        start = 0;
    }

    for(i=0;i<5;i++)
    {
        if(MMsg.Buf[MMsg.DisEndPtr].MsgType != RF_P2D_NONE_MSG)      /**<  最大为12，15标记指示为空 */
        {
            if(MMsg.Buf[MMsg.DisEndPtr].MsgLen > max)
            {
                len = max;
            }
            else
            {
                len = MMsg.Buf[MMsg.DisEndPtr].MsgLen;
            }

            if((MMsg.DisStartPtr%5) == i)
            {
                of = 1;
            }
            else
            {
                of = 0;
            }
            /**< 显示数据队列 */
			//k=(16*(i+1))+(8*(i+1));
            k = 24+(i*20);
			if((MMsg.Buf[MMsg.DisEndPtr].Msg[0] == 0xA1)&&((MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xF7)||(MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xF8)))
			{
				//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,FISRTCOLOR,BCOLOR);
				MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,FISRTCOLOR,BCOLOR);
			}
			else if((MMsg.Buf[MMsg.DisEndPtr].Msg[0] == 0xA1)&&(MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xF4))
			{
				//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,SECONDCOLOR,BCOLOR);
				MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,SECONDCOLOR,BCOLOR);
			}
			else if((MMsg.Buf[MMsg.DisEndPtr].Msg[0] == 0xA1)&&(MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xEF))
			{
				//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,THIRDCOLOR,BCOLOR);
				MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,THIRDCOLOR,BCOLOR);
			}
            else
            {
                /**> 短消息 */
                if(len > 16)
                {
                   len = 16; 
                }
                MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,FCOLOR,BCOLOR);
            }
			//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,FCOLOR,BCOLOR);
            MMsg.DisEndPtr++;
        }
        else
        {
            if(i == 0)
            {
                /**< 显示“空” */
                p = &RfPc2EndAreaMsg.Msg[2];
                n = (128-((RfPc2EndAreaMsg.MsgLen-2)*8))>>1;
                //MENU_ShowString(p,RfPc2EndAreaMsg.MsgLen-2,64,n,0,FCOLOR,BCOLOR);  //此处可以修改手表名称的颜色
                MENU_ShowString(p,RfPc2EndAreaMsg.MsgLen-2,56,n,0,WNFCOLOR,WNBCOLOR);
                
                n = (128-((sizeof("TB-2WM-V2")-1)*8))>>1;
                MENU_ShowString("TB-2WM-V2",9,72,n,0,WHITE,WNBCOLOR);
            }
            else
            {
                p = "…";
                /**< 显示“空” */
				//k = (16*(i+1))+(8*(i+1));
                k = 24+(i*20);
				MENU_ShowString(p,2,k,start,0,FCOLOR,BCOLOR);
            }
            break;
        }
    }
}
/********************************************************************************************************
                                        显示更新信息
********************************************************************************************************/
void MENU_DisRefreshMsg(void)
{
   uint8 j,k;//添加
    uint8 *p;
    uint8 i,len,m=0,of=0,n=0;
    uint8 max = 0,start = 0;
    
    m = MMsg.DisEndPtr%5;
    if((m == 0)&&(MMsg.DisEndPtr > 0))
    {
        /**< 显示页满 */
        return;
    }

    MMsg.DisEndPtr = MMsg.DisEndPtr - (MMsg.DisEndPtr%5);
    
    //MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
    
    if(SysSetStoreValue.DisMode == 0)
    {
        if(MMsg.Size)
        {
            j = 24+((MMsg.DisStartPtr%5)*20);
			MENU_ShowArrows(j);
        }
        max = 14;
        start = 16;
    }
    else
    {
        max = 16;
        start = 0;
    }
    
    for(i= 0;i< 5;i++)
    {
        if(MMsg.Buf[MMsg.DisEndPtr].MsgType != RF_P2D_NONE_MSG)
        {
            if(MMsg.Buf[MMsg.DisEndPtr].MsgLen > max)
            {
                len = max;
            }
            else
            {
                len = MMsg.Buf[MMsg.DisEndPtr].MsgLen;
            }

            if((MMsg.DisStartPtr%5) == i)
            {
                of = 1;
            }
            else
            {
                of = 0;
            }

            /**< 显示数据队列 */
			//k=(16*(i+1))+(8*(i+1));
            k = 24+(i*20);
			if((MMsg.Buf[MMsg.DisEndPtr].Msg[0] == 0xA1)&&((MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xF7)||(MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xF8)))
			{
				//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,FISRTCOLOR,BCOLOR);
				MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,FISRTCOLOR,BCOLOR);
			}
            else if((MMsg.Buf[MMsg.DisEndPtr].Msg[0] == 0xA1)&&(MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xF4))
			{
				//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,SECONDCOLOR,BCOLOR);
				MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,SECONDCOLOR,BCOLOR);
			}
			else if((MMsg.Buf[MMsg.DisEndPtr].Msg[0] == 0xA1)&&(MMsg.Buf[MMsg.DisEndPtr].Msg[1] == 0xEF))
			{
				//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,THIRDCOLOR,BCOLOR);
				MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,THIRDCOLOR,BCOLOR);
			}
            else
            {
                /**> 短消息 */
                if(len > 16)
                {
                   len = 16; 
                }
                MENU_ShowString(&MMsg.Buf[MMsg.DisEndPtr].Msg[0],len,k,start,of,FCOLOR,BCOLOR);
            }
			//MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,k,start,of,FCOLOR,BCOLOR);
            MMsg.DisEndPtr++;
        }
        else
        {
            if(i == 0)
            {
                /**< 显示“空” */
                p = &RfPc2EndAreaMsg.Msg[2];
                n = (128-((RfPc2EndAreaMsg.MsgLen-2)*8))>>1;
                //MENU_ShowString(p,RfPc2EndAreaMsg.MsgLen-2,64,n,0,FCOLOR,BCOLOR);  //此处可以修改手表名称的颜色 
                MENU_ShowString(p,RfPc2EndAreaMsg.MsgLen-2,56,n,0,WNFCOLOR,WNBCOLOR);
                
                n = (128-((sizeof("TB-2WM-V2")-1)*8))>>1;
                MENU_ShowString("TB-2WM-V2",9,72,n,0,WHITE,WNBCOLOR);
            }
            else
            {
                p = "…";
                /**< 显示“空” */
				//k=(16*(i+1))+(8*(i+1));
                k = 24+(i*20);
				MENU_ShowString(p,2,k,start,0,FCOLOR,BCOLOR); 
            }

            break;
        }
    }
}
/********************************************************************************************************
                                     显示打开信息
********************************************************************************************************/
uint8 MENU_DisOpenKeyMsg(void)
{
	uint8 len;
	uint8 *pbuf;
	uint8 reBack = FALSE;

    if((MMsg.DisStartPtr > MENU_MSG_SIZE)||(MMsg.DisEndPtr > MENU_MSG_SIZE))
    {
        MMsg.DisStartPtr = 0;  
        MMsg.DisEndPtr = 0;
    }
    
    if(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2D_NONE_MSG)
	{
		/**< 短消息 */
		if(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2W_CALL_MSG)
		{
			MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
			
	        /**< 获取数据长度和数据指针 */
	        len   	= MMsg.Buf[MMsg.DisStartPtr].MsgLen;
	        pbuf 	= MMsg.Buf[MMsg.DisStartPtr].Msg;
	        
            /**< 显示数据队列 */
			//MENU_ShowString(pbuf,len,0,0,0); 
			MENU_ShowString(pbuf,len,48,0,0,FCOLOR,BCOLOR);

            reBack = TRUE;
		}
		else
		{
#if 1
			/**< Page2---Page7清零 */
		    MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
			
	        /**< 获取数据长度和数据指针 */
	        len   	= MMsg.Buf[MMsg.DisStartPtr].MsgLen;
	        pbuf 	= MMsg.Buf[MMsg.DisStartPtr].Msg;
            
            /**< 显示数据队列 */
            MENU_ShowString(pbuf,len,32,0,0,FCOLOR,BCOLOR);

#if 0
            uint8 i,password[14];
         
            i = MMsg.Buf[MMsg.DisStartPtr].MsgType;
            password[0] = i/16;
            password[1] = i%16;
            
            password[2] = 0;
            
            i = (uint8)(MMsg.Buf[MMsg.DisStartPtr].TaskID>>24);
            password[3] = i/16;
            password[4] = i%16;
            i = (uint8)(MMsg.Buf[MMsg.DisStartPtr].TaskID>>16);
            password[5] = i/16;
            password[6] = i%16;
            i = (uint8)(MMsg.Buf[MMsg.DisStartPtr].TaskID>>8);
            password[7] = i/16;
            password[8] = i%16;
            i = (uint8)(MMsg.Buf[MMsg.DisStartPtr].TaskID>>0);
            password[9] = i/16;
            password[10] = i%16;
            
            password[11] = 0;
            
            i = MMsg.Buf[MMsg.DisStartPtr].MsgLen;
            password[12] = i/16;
            password[13] = i%16;
            
            
            for(i=0;i<14;i++)
            {
                password[i]  = DisNumHexTab[password[i]];
            }
            password[2]  = ' ';
            password[11] = ' ';
            
            /**< 获取数据长度和数据指针 */
	        len   	= 14;
	        pbuf 	= &password[0];
            
            //MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
            
            /**< 显示数据队列 */
            MENU_ShowString(pbuf,len,102,0,0,FCOLOR,BCOLOR);
			
#endif
            
			reBack = TRUE;
#endif
		}
	}
#if 0
	else
	{
		/**< Page2---Page7清零 */
		//MENU_ClearScreen(2,7);
		MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
		
		/**< 显示“空” */
		//MENU_ShowString(KONG_MSG,4,1,0,0);
		MENU_ShowString(KONG_MSG,4,64,0,0);
		/**< 显示“空” */
		//MENU_ShowString(KONG_MSG,4,1,1*16,0);
		MENU_ShowString(KONG_MSG,4,64,1*16,0);
		/**< 显示“空” */
		//MENU_ShowString(KONG_MSG,4,1,4*16,0);
		MENU_ShowString(KONG_MSG,4,64,4*16,0);
        /**< 显示“空” */
		//MENU_ShowString(KONG_MSG,4,1,5*16,0);
		MENU_ShowString(KONG_MSG,4,64,5*16,0);
				
		reBack = TRUE;
	}
#endif
	return reBack;
}
/********************************************************************************************************
                                         删除短信息
********************************************************************************************************/
void MENU_DisDeleteShortMsg(BOOL tf)
{
#if 0
    if((MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF0)&&(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2W_CALL_MSG))
    {
        /**< 删除当前打开信息 */
        MENU_ReSortMsg(MMsg.DisStartPtr);
    }
#else
    RfW2PCall_t RfW2PCall;
    
    /**< △▲◆★○ */
     if((MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF7)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF8)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF4)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xEF)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF0))   
    {
        if(MMsg.Buf[MMsg.DisStartPtr].Msg[1] != 0xF0)
        {
            /**< 发送仿真呼叫信息数据 */
            RfW2PCall.MsgType    = RF_E2P_MSG;
            RfW2PCall.TaskID     = MMsg.Buf[MMsg.DisStartPtr].TaskID|0x80000000;
            RfW2PCall.MsgLen     = 0;
            
            HBee_EndSimMsg((RfPc2EndMsg_t*)&RfW2PCall);
        }
        else
        {
            if(tf == FALSE)
            {
                return;
            }
        }
        /**< 删除当前打开信息 */
        MENU_ReSortMsg(MMsg.DisStartPtr);
    }
#endif
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ResetWriterMsg(void)
{  
    uint8 n = 0;
    //EA = 0;
    
    n = MMsg.Size;
    osal_nv_item_init(WRITER_NUM_MSG_ID,1,NULL);
    osal_nv_write(WRITER_NUM_MSG_ID,0,1,&n);
    if(n > 0)
    {
        osal_nv_item_init(WRITER_RESET_MSG_ID,sizeof(MenuMsg_t),NULL);
        osal_nv_write(WRITER_RESET_MSG_ID,0,sizeof(MenuMsg_t),&MMsg);
    }

	//EA = 1;
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ResetReadMsg(void)
{
    uint8 n = 0;
    
    osal_nv_item_init(WRITER_NUM_MSG_ID,1,NULL);
    osal_nv_read(WRITER_NUM_MSG_ID,0,1,&n);
    if((n > 0)&&(n != 0xFF))
    {
        osal_nv_item_init(WRITER_RESET_MSG_ID,sizeof(MenuMsg_t),NULL);
        osal_nv_read(WRITER_RESET_MSG_ID,0,sizeof(MenuMsg_t),&MMsg);
        
        n = 0;
        osal_nv_item_init(WRITER_NUM_MSG_ID,1,NULL);
        osal_nv_write(WRITER_NUM_MSG_ID,0,1,&n);
    }
    else
    {
        memset(&MMsg,0,sizeof(MenuMsg_t));
    }
    MENU_DisRefreshMsg();
    MENU_DisPgdnKeyMsg(FALSE);
}
/********************************************************************************************************
                                       软件延时                            
********************************************************************************************************/
void MENU_Delay(uint16 n)
{
    uint16 i,k;
    
    for(i=0;i<n;i++)
    {
        for(k=0;k<10000;k++)
        {
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        }
    }
}
/********************************************************************************************************
                                    关机界面
********************************************************************************************************/
void MENU_DisOffMsg(void)
{
    uint8 *p;
    
    if(!IsLcdSleep())
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        p = "关机中......";
        MENU_ShowString(p,12,64,8,0,FCOLOR,BCOLOR);

        osal_start_timerEx(MSA_TaskId,HBEEAPP_SYS_OFF_EVT,1000);
    }
}
/********************************************************************************************************
                                  信息列表界面
********************************************************************************************************/
void  MENU_DisListMsg(void)
{ 
    if(IsLcdSleep())
    {
        if(MenuFlg.ShutDown == 1)
        {
            /**< 关机标记不使能 */
            MenuFlg.ShutDown = 0;
            /*******************************************/
            //ZDApp_StartJoiningCycle();
            //NLME_SetPollRate(ZDApp_SavedPollRate);                      /**< 开启数据请求 */
            
            if(msa_IsStarted == TRUE)
            {
                osal_start_timerEx( MSA_TaskId,MSA_POLL_EVENT,1000);
            }
            else
            {
                 osal_start_timerEx(MSA_TaskId,MSA_SCAN_EVENT,1000);
            }
            
            osal_set_event(MSA_TaskId,HBEEAPP_ONLINE_EVT);             /**< 开启心跳 */
            osal_set_event(MSA_TaskId,HBEEAPP_TIME_UPDATEMSG_EVT);     /**< 开启获取时间 */
            /*******************************************/
            HBee_SetUserEvent(HB_SET_MOTOR_EVENT,0x1);
        }
        osal_set_event(MSA_TaskId,HBEEAPP_OLED_ON_EVT);
    }
    
    MENU_DisPgdnKeyMsg(FALSE);  
}
/********************************************************************************************************
                                  显示设备信息界面
********************************************************************************************************/
void  MENU_DisDeceiveMsg(void)
{
    uint8 i,j,n;
	uint8 *p;
    uint16 addr;
    uint8 password[16];
    
    MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
    password[0] = DisNumHexTab[SysSetStoreValue.Channel/16];
    password[1] = DisNumHexTab[SysSetStoreValue.Channel%16];
    MENU_ShowString(password,2,48,0,0,FCOLOR,BCOLOR);
    
    n = sizeof(RF_SOFTWARE_VER);
    p = RF_SOFTWARE_VER;
	MENU_ShowString(p,n,48,24,0,FCOLOR,BCOLOR);
	
    n = sizeof("IEEE:");
    p = "IEEE:";
    MENU_ShowString(p,n,48,80,0,FCOLOR,BCOLOR);
    
    p = NLME_GetExtAddr();
    for(i=8,j=0;i>0;i--,j++)
    {
        password[(j<<1)]   = DisNumHexTab[p[i-1]/16];
        password[(j<<1)+1] = DisNumHexTab[p[i-1]%16];
    }
    MENU_ShowString(password,16,64,0,0,FCOLOR,BCOLOR);

    addr    = NLME_GetCoordShortAddr();
    n = addr>>8;
    password[0] = DisNumHexTab[n/16];
    password[1] = DisNumHexTab[n%16];
    n = addr;
    password[2] = DisNumHexTab[n/16];
    password[3] = DisNumHexTab[n%16];
    password[4] = ' ';
    addr    = NLME_GetShortAddr();
    n = addr>>8;
    password[5] = DisNumHexTab[n/16];
    password[6] = DisNumHexTab[n%16];
    n = addr;
    password[7] = DisNumHexTab[n/16];
    password[8] = DisNumHexTab[n%16];
    
    password[9] = ' ';
    n = SysSetStoreValue.PANID.Data>>8;
    password[10] = DisNumHexTab[n/16];
    password[11] = DisNumHexTab[n%16];
    n = SysSetStoreValue.PANID.Data;
    password[12] = DisNumHexTab[n/16];
    password[13] = DisNumHexTab[n%16];
    
    MENU_ShowString(password,14,80,0,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                            复位界面
********************************************************************************************************/
void  MENU_DisResetMsg(void)
{
	uint8 *p;

    MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
    p = "复位中......";
    MENU_ShowString(p,12,64,8,0,FCOLOR,BCOLOR);
	
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisOpenMsg(void)
{
    if(!MENU_DisOpenKeyMsg())
    {
        
    }
}
/********************************************************************************************************
                                          低电量界面
********************************************************************************************************/
void  MENU_DisLowPower(void)
{
	uint8 *p;

	MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
    
    p = "电量低,请充电";
	MENU_ShowString(p,13,64,12,0,FCOLOR,BCOLOR); 				            /**< 显示电池电量低 */
    HBee_SetUserEvent(HB_SET_MOTOR_EVENT,0x01);
	osal_set_event(MSA_TaskId,HBEEAPP_OLED_ON_EVT);

}
/********************************************************************************************************
                                      充电界面
********************************************************************************************************/
void MENU_DisIncPower(void)
{
	uint8 *p;
    if(MAIN_POWER == 0)
    {
        MENU_ClearScreen(0,16,127,127);						    /**< Page16---Page127清零 */
        p = "充电中......";
        MENU_ShowString(p,12,64,8,0,FCOLOR,BCOLOR);
    }
}
/********************************************************************************************************
                                        用户密码登陆界面
********************************************************************************************************/
void  MENU_DisUserPassword(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4]={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =8;
        p = "用户设置";
        MENU_ShowString(p,n,48,32,0,FCOLOR,BCOLOR);
		
        n =11;
        p = "请输入密码:";
        MENU_ShowString(p,n,64,0,0,FCOLOR,BCOLOR);
        
        password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
        password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
        password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
        password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 2;
    }
    else
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else if(MenuFlg.SelectBit == 2)
            {
                password[0] = ' ';
                password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 3)
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = ' ';
                password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 4)
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
                password[2] = ' ';
                password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
            }
            else
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
                password[3] = ' ';
            }
        }
        else
        {
            password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
            password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
            password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
            password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
        }
        
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }

	MENU_ShowString(password,4,80,96,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************

********************************************************************************************************/
typedef struct
{
    uint8   MsgType;
    uint8   MsgLen;
    uint8   *pMsg;
}MenuBufMsg_t;

typedef struct
{
    uint8 DisStartPtr  :4;
    uint8 DisEndPtr    :4;
    uint8 Size;

    MenuBufMsg_t        Buf[6];
}SetMenuMsg_t;

void MENU_DisSetPgdnKeyMsg(SetMenuMsg_t *pMenu,BOOL nAdd)
{
    uint8 *p;
	uint8 i,n,len,k,of=0;
	
	if(nAdd == TRUE)
	{
		n = pMenu->Size;
		pMenu->DisStartPtr++;
		if(pMenu->DisStartPtr >= n)
		{
			pMenu->DisStartPtr = 0;
		}
	}
	else
	{
		n = pMenu->Size;
		if((pMenu->DisStartPtr+1) >= n)
		{
			pMenu->DisStartPtr = 0;
			pMenu->DisEndPtr = 0;
		}
	}

	pMenu->DisEndPtr = pMenu->DisStartPtr - (pMenu->DisStartPtr%7);

    MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */

	for(i=0;i<7;i++)
	{
		if(pMenu->Buf[pMenu->DisEndPtr].MsgType != 0) 	 /**<  最大为12，15标记指示为空 */
		{
			if(pMenu->Buf[pMenu->DisEndPtr].MsgLen > 16)
			{
				len = 16;
			}
			else
			{
				len = pMenu->Buf[pMenu->DisEndPtr].MsgLen;
			}

			if((pMenu->DisStartPtr%7) == i)
			{
				of = 1;
			}
			else
			{
				of = 0;
			}

			k=(16*(i+1));
			MENU_ShowString(pMenu->Buf[pMenu->DisEndPtr].pMsg,len,k,0,of,FCOLOR,BCOLOR); 
			pMenu->DisEndPtr++;
		}
		else
		{
            
            p = "……";
			/**< 显示“空” */
			k=(16*(i+1));
			MENU_ShowString(p,4,k,0,0,FCOLOR,BCOLOR);
			break;
		}
	}
}
void MENU_DisUserSetList(void)
{
	/**
	用户模式
	1.提示次数
	2.按键震动
	3.用户密码
	4.进入管理模式
	管理模式
	1.自动获取射频参数
	2.设置频道
	3.设置局域网号
	4.设置地址
	5.设置充电接收
	6.恢复出厂设置
	*/
	static SetMenuMsg_t uSetMenu;

	if(uSetMenu.Size != 4)
	{
		memset(&uSetMenu,0,sizeof(SetMenuMsg_t));
		
		uSetMenu.Size = 4;
		uSetMenu.DisEndPtr = 0;
		uSetMenu.DisStartPtr = 0;
		uSetMenu.Buf[0].MsgType =  1;
		uSetMenu.Buf[0].MsgLen  =  10;
		uSetMenu.Buf[0].pMsg    =  "1-提示次数";

		uSetMenu.Buf[1].MsgType =  1;
		uSetMenu.Buf[1].MsgLen	=  10;
		uSetMenu.Buf[1].pMsg    =  "2-按键震动";

		uSetMenu.Buf[2].MsgType =  1;
		uSetMenu.Buf[2].MsgLen	=  10;
		uSetMenu.Buf[2].pMsg    =  "3-用户密码";

		uSetMenu.Buf[3].MsgType =  1;
		uSetMenu.Buf[3].MsgLen	=  14;
		uSetMenu.Buf[3].pMsg    =  "4-进入管理模式";
	}

	MENU_DisSetPgdnKeyMsg(&uSetMenu,TRUE);
}
/********************************************************************************************************
                                    设置提示次数界面
********************************************************************************************************/
void  MENU_DisUserSetTipNumber(void)
{
    uint8 n;
    uint8 *p;
    uint8 disBuf[4] = "2 次";
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =14;
        p = "1-设置提示次数";
        MENU_ShowString(p,n,48,8,0,FCOLOR,BCOLOR);
		
        n =9;
        p = "提示次数:";
        MENU_ShowString(p,n,64,0,0,FCOLOR,BCOLOR);
        
        disBuf[0] = DisNumHexTab[SysSetValue.TipNumber];
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                disBuf[0] = ' ';
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                disBuf[0] = DisNumHexTab[SysSetValue.TipNumber];
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }     
    MENU_ShowString(disBuf,4,80,96,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                          设置按键震动界面
********************************************************************************************************/
void  MENU_DisUserSetKeyMotor(void)
{
	uint8 n;
    uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =14;
        p = "2-设置按键震动";
        MENU_ShowString(p,n,48,8,0,FCOLOR,BCOLOR);

        if(SysSetValue.KeyMotor == 0x01)
        {
            p = "●是    ○否";
        }
        else
        {
            p = "○是    ●否";
        }

        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.KeyMotor == 0x01)
                {
                    p = "●是    ○否";
                }
                else
                {
                    p = "○是    ●否";
                }
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.KeyMotor == 0x01)
                {
                    p = "  是    ○否";
                }
                else
                {
                    p = "○是      否";
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
	
    MENU_ShowString(p,12,80,16,0,FCOLOR,BCOLOR);  
}
/********************************************************************************************************
                                          设置用户密码界面
********************************************************************************************************/
void MENU_DisUserSetPassword(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4]={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =14;
        p = "3-设置用户密码";
        MENU_ShowString(p,n,48,8,0,FCOLOR,BCOLOR);
		
        n =8;
        p = "新密码:";
        MENU_ShowString(p,n,64,0,0,FCOLOR,BCOLOR);
        
        password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
        password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
        password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
        password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else
    {
        if(MenuFlg.ShowFlash == 1)
        {

            if(MenuFlg.SelectBit == 1)
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 2)
            {
                password[0] = ' ';
                password[1] =DisNumHexTab[ SysSetValue.UserPassword.Bit.b1];
                password[2] =DisNumHexTab[ SysSetValue.UserPassword.Bit.b2];
                password[3] =DisNumHexTab[ SysSetValue.UserPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 3)
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = ' ';
                password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 4)
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
                password[2] = ' ';
                password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
            }
            else
            {
                password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
                password[3] = ' ';
            }
        }
        else
        {
            password[0] = DisNumHexTab[SysSetValue.UserPassword.Bit.b0];
            password[1] = DisNumHexTab[SysSetValue.UserPassword.Bit.b1];
            password[2] = DisNumHexTab[SysSetValue.UserPassword.Bit.b2];
            password[3] = DisNumHexTab[SysSetValue.UserPassword.Bit.b3];
        }
        
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }

	MENU_ShowString(password,4,80,96,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                系统密码登陆界面
********************************************************************************************************/
void  MENU_DisSysPassword(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4] ={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =8;
        p = "系统设置";;
        MENU_ShowString(p,n,48,32,0,FCOLOR,BCOLOR);
		
        n =11;
        p = "请输入密码:";;
        MENU_ShowString(p,n,64,0,0,FCOLOR,BCOLOR);
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 2;
    }
    else
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
            
            }
            else if(MenuFlg.SelectBit == 2)
            {
                password[0] = ' ';
                password[1] = DisNumHexTab[SysSetValue.SysPassword.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.SysPassword.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.SysPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 3)
            {
                password[0] = DisNumHexTab[SysSetValue.SysPassword.Bit.b0];
                password[1] = ' ';
                password[2] = DisNumHexTab[SysSetValue.SysPassword.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.SysPassword.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 4)
            {
                password[0] = DisNumHexTab[SysSetValue.SysPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.SysPassword.Bit.b1];
                password[2] = ' ';
                password[3] = DisNumHexTab[SysSetValue.SysPassword.Bit.b3];
            }
            else
            {
                password[0] = DisNumHexTab[SysSetValue.SysPassword.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.SysPassword.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.SysPassword.Bit.b2];
                password[3] = ' ';
            }
        }
        else
        {
            password[0] = DisNumHexTab[SysSetValue.SysPassword.Bit.b0];
            password[1] = DisNumHexTab[SysSetValue.SysPassword.Bit.b1];
            password[2] = DisNumHexTab[SysSetValue.SysPassword.Bit.b2];
            password[3] = DisNumHexTab[SysSetValue.SysPassword.Bit.b3];
        }
        
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }

	MENU_ShowString(password,4,80,96,0,FCOLOR,BCOLOR); 
}
/********************************************************************************************************
                                        自动获取RF参数界面
********************************************************************************************************/
void MENU_DisSysGetRfParam(void)
{
    uint8 n;
	uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =16;
        p = "1-自动获取RF参数";;
        MENU_ShowString(p,n,48,0,0,FCOLOR,BCOLOR);

        if(SysSetValue.AutoGetRf == 0x00)
        {
            p = "○启动  ●关闭";
        }
        else
        {
            p = "●启动  ○关闭";     
        }

        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.AutoGetRf == 0x00)
                {
                    p = "○启动  ●关闭";
                }
                else
                {
                    p = "●启动  ○关闭";     
                }
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.AutoGetRf == 0x00)
                {
                    p = "○启动    关闭";
                }
                else
                {
                    p = "  启动  ○关闭";     
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,16,80,0,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                         设置信道界面
********************************************************************************************************/
void MENU_DisSysSetChannel(void)
{
    uint8 n;
    uint8 *p;
    uint8 disBuf[6] = "11信道";
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =10;
        p = "2-信道设置";;
        MENU_ShowString(p,n,48,24,0,FCOLOR,BCOLOR);
        
        disBuf[0] = DisNumHexTab[SysSetValue.Channel/16];
        disBuf[1] = DisNumHexTab[SysSetValue.Channel%16];
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                disBuf[0] = ' ';
                disBuf[1] = ' ';
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                disBuf[0] = DisNumHexTab[SysSetValue.Channel/16];
                disBuf[1] = DisNumHexTab[SysSetValue.Channel%16];
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(disBuf,6,80,80,0,FCOLOR,BCOLOR);
    
}
/********************************************************************************************************
                                    设置PANID界面
********************************************************************************************************/
void MENU_DisSysSetPanid(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4]={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =12;
        p = "3-PANID 设置";;
        MENU_ShowString(p,n,48,16,0,FCOLOR,BCOLOR);
        
        password[0] = DisNumHexTab[SysSetValue.PANID.Bit.b0];
        password[1] = DisNumHexTab[SysSetValue.PANID.Bit.b1];
        password[2] = DisNumHexTab[SysSetValue.PANID.Bit.b2];
        password[3] = DisNumHexTab[SysSetValue.PANID.Bit.b3];
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                password[0] = DisNumHexTab[SysSetValue.PANID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.PANID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.PANID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.PANID.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 2)
            {
                password[0] = ' ';
                password[1] =DisNumHexTab[SysSetValue.PANID.Bit.b1];
                password[2] =DisNumHexTab[SysSetValue.PANID.Bit.b2];
                password[3] =DisNumHexTab[SysSetValue.PANID.Bit.b3];
            }                                                      
            else if(MenuFlg.SelectBit == 3)
            {
                password[0] = DisNumHexTab[SysSetValue.PANID.Bit.b0];
                password[1] =  ' ';                               
                password[2] = DisNumHexTab[SysSetValue.PANID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.PANID.Bit.b3];
            }
            else if(MenuFlg.SelectBit == 4)
            {
                password[0] = DisNumHexTab[SysSetValue.PANID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.PANID.Bit.b1];
                password[2] = ' ';                                
                password[3] = DisNumHexTab[SysSetValue.PANID.Bit.b3];
            }
            else
            {
                password[0] = DisNumHexTab[SysSetValue.PANID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.PANID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.PANID.Bit.b2];
                password[3] = ' ';                                
            }
        }
        else
        {
            password[0] = DisNumHexTab[SysSetValue.PANID.Bit.b0];
            password[1] = DisNumHexTab[SysSetValue.PANID.Bit.b1];
            password[2] = DisNumHexTab[SysSetValue.PANID.Bit.b2];
            password[3] = DisNumHexTab[SysSetValue.PANID.Bit.b3];
        }
        
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }
    MENU_ShowString(password,4,80,96,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                       设置IEEE地址(只显示不能设置)界面
********************************************************************************************************/
void MENU_DisSysSetAddr(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[16]={0x32,0x30,0x31,0x34,0x30,0x35,0x30,0x31, 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =10;
        p = "4-地址设置";;
        MENU_ShowString(p,n,48,24,0,FCOLOR,BCOLOR);
		
        n =5;
        p = "IEEE:";;
        MENU_ShowString(p,n,64,0,0,FCOLOR,BCOLOR);
        
        password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
        password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
        password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
        password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

        password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
        password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
        password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
        password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];

        password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
        password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
        password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
        password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

        password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
        password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
        password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
        password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
        
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }
            else if(MenuFlg.SelectBit == 2)
            {
                
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = ' ';
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];
                                                                   
                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }
            else if(MenuFlg.SelectBit == 3)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = ' ';                                
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }
            else if(MenuFlg.SelectBit == 4)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = ' ';                                
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }
            else if(MenuFlg.SelectBit == 5)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11]  = ' ';                               

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }
            else if(MenuFlg.SelectBit == 6)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12]  = ' ';
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }                                                      
            else if(MenuFlg.SelectBit == 7)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = ' ';                                
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];

            }
            else if(MenuFlg.SelectBit == 8)
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = ' ';                                
                password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
            }
            else
            {
                password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
                password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
                password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
                password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

                password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
                password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
                password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
                password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
                
                password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
                password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
                password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
                password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

                password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
                password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
                password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
                password[15] = ' ';                                
            }
        }
        else
        {
            password[0] = DisNumHexTab[SysSetValue.NetID.Bit.b0];
            password[1] = DisNumHexTab[SysSetValue.NetID.Bit.b1];
            password[2] = DisNumHexTab[SysSetValue.NetID.Bit.b2];
            password[3] = DisNumHexTab[SysSetValue.NetID.Bit.b3];

            password[4] = DisNumHexTab[SysSetValue.NetID.Bit.b4];
            password[5] = DisNumHexTab[SysSetValue.NetID.Bit.b5];
            password[6] = DisNumHexTab[SysSetValue.NetID.Bit.b6];
            password[7] = DisNumHexTab[SysSetValue.NetID.Bit.b7];
            
            password[8]  = DisNumHexTab[SysSetValue.Addr.Bit.b0];
            password[9]  = DisNumHexTab[SysSetValue.Addr.Bit.b1];
            password[10] = DisNumHexTab[SysSetValue.Addr.Bit.b2];
            password[11] = DisNumHexTab[SysSetValue.Addr.Bit.b3];

            password[12] = DisNumHexTab[SysSetValue.Addr.Bit.b4];
            password[13] = DisNumHexTab[SysSetValue.Addr.Bit.b5];
            password[14] = DisNumHexTab[SysSetValue.Addr.Bit.b6];
            password[15] = DisNumHexTab[SysSetValue.Addr.Bit.b7];
        }
        
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }
	MENU_ShowString(password,16,80,0,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                     设置显示模式界面
********************************************************************************************************/
void MENU_DisSysSetShowMode(void)
{
    uint8 n,*p;
    uint8 disBuf[5] = "模式1";

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =14;
        p = "5-显示模式设置";;
        MENU_ShowString(p,n,48,8,0,FCOLOR,BCOLOR);

        disBuf[4] = DisNumHexTab[SysSetValue.DisMode%16];
        
        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                disBuf[4] = ' ';
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                disBuf[4] = DisNumHexTab[SysSetValue.DisMode%16];
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    
	MENU_ShowString(disBuf,5,80,80,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                        设置关机界面
********************************************************************************************************/
void MENU_DisEnShutDown(void)
{
	uint8 n;
	uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =12;
        p = "6-关机设置";;
        MENU_ShowString(p,n,48,16,0,FCOLOR,BCOLOR);

        if(SysSetValue.EnShutdown == 0x01)
        {
            p = "●允许  ○不允许";
        }
        else
        {
            p = "○允许  ●不允许";     
        }

        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.EnShutdown == 0x01)
                {
                    p = "●允许  ○不允许";
                }
                else
                {
                    p = "○允许  ●不允许";     
                }
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.EnShutdown == 0x01)
                {
                    p = "  允许  ○不允许";
                }
                else
                {
                    p = "○允许    不允许";     
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,16,80,0,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************
                                      恢复出厂设置界面
********************************************************************************************************/
void MENU_DisSysResetToDefault(void)
{
    uint8 n;
	uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(0,16,127,127);						                /**< Page16---Page127清零 */
        n =14;
        p = "7-恢复出厂设置";;
        MENU_ShowString(p,n,48,8,0,FCOLOR,BCOLOR);

        if(SysSetValue.ResetToDefault == 0x01)
        {
            p = "●允许  ○不允许";
        }
        else
        {
            p = "○允许  ●不允许";     
        }

        MenuFlg.ShowFlash = 1;
        MenuFlg.SelectBit = 1;
    }
    else 
    {
        if(MenuFlg.ShowFlash == 1)
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.ResetToDefault == 0x01)
                {
                    p = "●允许  ○不允许";
                }
                else
                {
                    p = "○允许  ●不允许";     
                }
            }
        }
        else
        {
            if(MenuFlg.SelectBit == 1)
            {
                return;
            }
            else
            {
                if(SysSetValue.ResetToDefault == 0x01)
                {
                    p = "  允许  ○不允许";
                }
                else
                {
                    p = "○允许    不允许";     
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,16,80,0,0,FCOLOR,BCOLOR);
}
/********************************************************************************************************

********************************************************************************************************/
