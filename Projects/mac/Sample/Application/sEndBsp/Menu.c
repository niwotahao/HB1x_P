
/********************************************************************************************************

********************************************************************************************************/
#include "string.h"
#include "Font.h"
#include "Menu.h"
#include "Rf.h"

#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OSAL_PwrMgr.h"
//#include "AF.h"
//#include "ZDApp.h"
#include "msa.h"
/********************************************************************************************************

********************************************************************************************************/
MenuFlg_t MenuFlg;
__no_init MenuMsg_t MMsg;
RfPc2EndAreaMsg_t RfPc2EndAreaMsg;
SysSetStoreValue_t SysSetValue;         /**< �û�����ϵͳ���û��� */
SysSetStoreValue_t SysSetStoreValue;    /**< ϵͳ����ֵ */
const __code uint8 DisNumHexTab[17] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,0x00};

extern bool msa_IsStarted;
/********************************************************************************************************

********************************************************************************************************/
void Fill_RAM(uint8 Data);
void Show_ShuZi(uint8 data, uint8 shuzipage, uint8 shuzifont);
/********************************************************************************************************

********************************************************************************************************/
const __code uint8 TX_POWER[] = //������
{
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,  //0  û���ź�

	0x00,0x60,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,  //1  1���ź�

	0x00,0x60,0x00,0x70,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,  //2

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x00,0x00,0x00,0x00,0x00,  //3

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x7C,0x00,0x00,0x00,0x00,  //4

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x7C,0x00,0x7E,0x00,0x00,  //5  5���ź�

	0x00,0x60,0x00,0x70,0x00,0x78,
	0x00,0x7C,0x00,0x7E,0x00,0x7F,   //6  ȫ�ź�*/
      
      
    0x00,0x50,0x20,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //7 x����
    //0x88,0x50,0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    
    //0x00,0x44,0x28,0x10,0x28,0x44,  //7 x����
    //0x00,0x00,0x00,0x00,0x00,0x00
};
/********************************************************************************************************

********************************************************************************************************/
const __code uint8 TX_ICO[] =               //TX Icon
{
	0x00,0x03,0x05,0x7F,0x05,0x03,      
};
/********************************************************************************************************

********************************************************************************************************/
const __code  uint8 DC_ICO[] =              //Batt Icon
{
    0x00,0x00,0x00,0x00,0x00,0x00,
    0xFE,0x83,0x83,0x83,0x83,0xFE,
    0xFE,0xC3,0xC3,0xC3,0xC3,0xFE,
    0xFE,0xE3,0xE3,0xE3,0xE3,0xFE,
    0xFE,0xF3,0xF3,0xF3,0xF3,0xFE,
    0xFE,0xFB,0xFB,0xFB,0xFB,0xFE,
    0xFE,0xFF,0xFF,0xFF,0xFF,0xFE
    /*
    0xFE,0x83,0x83,0x83,0x83,0x83,0xFE,
    0xFE,0x83,0xE3,0xE3,0xE3,0x83,0xFE,
    0xFE,0x83,0xFB,0xFB,0xFB,0x83,0xFE,
    0xFE,0x83,0xFF,0xFF,0xFF,0x83,0xFE,
    */   
};
/********************************************************************************************************

********************************************************************************************************/
void MENU_ShowBatt( uint8 j)
{
	uint8 i;

    
	LCD_SetStartPage(0x00);
	LCD_SetStartColumn(XLevel+120);
	for(i=0;i<6;i++)
	{
		LCD_WriteData(DC_ICO[j*6+i]);
	}
    
    /*
    LCD_SetStartPage(0x00);
	LCD_SetStartColumn(XLevel+120);
	for(i=0;i<7;i++)
	{
		LCD_WriteData(DC_ICO[j*7+i]);
	}
    */
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ShowTx(void)
{
	uint8 i;

	LCD_SetStartPage(0x00);
	LCD_SetStartColumn(XLevel);
	for(i=0;i<6;i++)
	{
		LCD_WriteData(TX_ICO[i]);
	}
	LCD_WriteData(0x00);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ShowTxPOWER(uint8 i)
{
	uint8 j;

	LCD_SetStartPage(0x00);
	LCD_SetStartColumn(XLevel+6);

	for(j=0;j<12;j++)
	{
		LCD_WriteData(TX_POWER[i*12+j]);
	}
	LCD_WriteData(0x00); 
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_FillRAM(uint8 Data)
{
	uint8 i,j;
    
	for(i=0;i<8;i++)
	{
		LCD_SetStartPage(i);
		LCD_SetStartColumn(0x00);

		for(j=0;j<128;j++)
		{
			LCD_WriteData(Data);
		}
	}
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ClearScreen(uint8 start, uint8 end)
{
	uint8 i,j;

	for(i=start;i<(end+1);i++)  //i=0
	{
		LCD_SetStartPage(i);
		LCD_SetStartColumn(0x00);

		for(j=0;j<128;j++)
		{
			LCD_WriteData(0);
		}
	}
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ShowString(uint8 * data, uint8 datalen, uint8 page,uint8 font,uint8 of)
{
    uint8 i;
    for( i = 0; i <datalen;)
    {
        if(SysSetStoreValue.DisMode == 3)
        {
            /**< ��ʾ�ַ�ȫ������������ */
            of &= 0x01;
                
            if(of)
            {
                if(i==0)
                {
                    /**< ��һ���� */
                    of |= 0x80;
                }
                else 
                {
                    if( ( (data[datalen-2] >= 0x80) && ( i== datalen-2) )  ||  (  (data[datalen-2] < 0x80) && (i== datalen-1) )  )
                    {
                        /**< ���һ���� */
                        //of |= 0x40;
                    }
                }
            }
        }
        
        if(data[i]==0) //�ж��Ƿ��о��ӽ�����ʶ�������������ʾ������
        {
            break;
        }
        else if (data[i] < 0x80)///�Զ���font����
        {
            if( font > 120  ) //�����ʾ��һ��
            {
                font = 0;
                page++;
            }
            if( page > 2) //�����ʾ��һҳ
            {
                page = 0;     ///////////////////////////δ��ӷ�ҳ��ʾ��
            }
            MENU_ShowShuZi(data[i], page, font,of);
            //FONT_ShowAscii(FONT_GB2312Addr(data+i,TYPE_16),page,font,of);
            font +=8;
            i++;
        }
        else
        {
            /**< ��ֹһҳ��ʾ����������ʾ����һ�� */
            if((i+1)!=datalen)
            {
                if( font > 112 ) //�����ʾ��һ��
                {
                    font = 0;
                    page++;
                }
                if( page > 2) //�����ʾ��һҳ
                {
                    page = 0;     ///////////////////////////δ��ӷ�ҳ��ʾ��
                }
                FONT_ShowHanZi(FONT_GB2312Addr(data+i,TYPE_16),page,font,of);
                font +=16;
                i = i+2;    //ÿ�����ֽڴ���һ������
            }
            else
            {
                break;
            }
        }
    }
    /**< ���һ����ʾ */
	if(of)
	{
        
        if(SysSetStoreValue.DisMode == 0)
        {
            return;
        }
		for( i = datalen; i <16;)
		{
			uint8 kong= 0x20;
            
			if( font > 120	) //�����ʾ��һ��
			{
				font = 0;
				page++;
			}
			if( page > 2) //�����ʾ��һҳ
			{
				page = 0;	  ///////////////////////////δ��ӷ�ҳ��ʾ��
			}
            
            if(SysSetStoreValue.DisMode == 3)
            {
                of &= 0x01;
                if(i == 15)
                {
                    of |= 0x40;
                }
            }
			MENU_ShowShuZi(kong, page, font,of);
			//FONT_ShowAscii(FONT_GB2312Addr(&kong,TYPE_16),page,font,of);
			font +=8;
			i++;
		}
	}
}
/********************************************************************************************************

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
        FONT_ShowAscii2(FONT_GB2312Addr(&n[k],TYPE_16),0,46+(k*8),0);
    }
    return;
    
    /*
    uint8 i;
    
    n[0] -= 0x20;
    n[1] -= 0x20;
    n[2] -= 0x20;
    n[3] -= 0x20;
    n[4] -= 0x20;
    
	LCD_SetStartPage(0x00);
	LCD_SetStartColumn(XLevel+46);

    for(k=0;k<5;k++)
    {
        for(i=0;i<8;i++)
        {
            LCD_WriteData(*(DzkCode[n[k]]+i));
        }
    }

	LCD_SetStartPage(0x01);
	LCD_SetStartColumn(XLevel+46); 
    
    for(k=0;k<5;k++)
    {
        for(i=8;i<16;i++)
        {
            LCD_WriteData(*(DzkCode[n[k]]+i));
        }
    }
    */
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_ShowShuZi(uint8 data, uint8 shuzipage, uint8 shuzifont,uint8 of)
{
	uint8 i,n;

	LCD_SetStartPage(0x02+shuzipage*2);
	LCD_SetStartColumn(XLevel+shuzifont);

    if(data >= 0x20)
    {
        data-=0x20;
    }

    for(i=0;i<8;i++)
	{
        n = *(DzkCode[data]+i);

        if(of)
        {
            if(SysSetStoreValue.DisMode == 0)
            {
                
                /**< ���·ֱ�һ�� */
                //n|= 0x01;
            }
            else if(SysSetStoreValue.DisMode == 1)
            {
                /**< �������� */
            }
            else if(SysSetStoreValue.DisMode == 2)
            {
                /**< ���� */
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
        LCD_WriteData(n);
	}
    
	LCD_SetStartPage(0x03+shuzipage*2);
	LCD_SetStartColumn(XLevel+shuzifont); 

    for(i=8;i<16;i++)
	{
        n = *(DzkCode[data]+i);
        if(of)
        {
            if(SysSetStoreValue.DisMode == 0)
            {

                /**< ���·ֱ�һ�� */
                //n|= 0x80;
            }
            else if(SysSetStoreValue.DisMode == 1)
            {
                /**< �������� */
                //n|= 0xC0;
                n|= 0x80;
            }
            else if(SysSetStoreValue.DisMode == 2)
            {
                /**< ���� */
                n =~n;
            }
            else
            {
                n|= 0x80;
                if(of&0x80)
                {
                    if(i==8)
                    {
                        n = 0xFF;
                    }
                }
                else if(of&0x40)
                {
                    if(i==15)
                    {
                        n = 0xFF;
                    }
                }
            }
        }
        LCD_WriteData(n);
	}
}
/********************************************************************************************************

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
    SysSetStoreValue.DisMode                = 0;            /**< 0~3 */ 
    SysSetStoreValue.DisSort                = 0;            /**< 0~1 */ 
    SysSetStoreValue.EnShutdown             = 1;            /**< 0~1 */  
    SysSetStoreValue.ResetToDefault         = 0;            /**< 0~1 */ 
    osal_nv_item_init(WRITER_SET_STORE_ID,sizeof(SysSetStoreValue_t),NULL);
    osal_nv_write(WRITER_SET_STORE_ID,0,sizeof(SysSetStoreValue_t),(uint8*)&SysSetStoreValue);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_Init(void)				
{
    /** oled �ܹ�8ҳ��1ҳ==8�� */
	LCD_Init();
    
#if 1
    MENU_MsgBufInit();
    
	MENU_FillRAM(0x00);		

    MENU_ShowTx();
    MENU_ShowTxPOWER(TX_POWER_X);
    MENU_ShowTime(12,0);
    MENU_ShowBatt(BAT_FULL);
	
	LCD_SetWork(TRUE);
    //LCD_SetWork(FALSE);
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
                                        ���ؿ��Դ洢���ݵĵ�ַ����
********************************************************************************************************/
uint8 MENU_ReSortMsg(uint8 s)
{
    uint8 i,n;
    
    if(MMsg.Size > 0)
    {
        n = MMsg.Size-1;
        
        /**< �����������������Ƚ��յ������ */
        for(i=s;i<n;i++)
        {
            MMsg.Buf[i] = MMsg.Buf[i+1];
        }
        if(s <= n)
        {
            MMsg.Size--;
        }
        MMsg.Buf[i].MsgType = RF_P2D_NONE_MSG;
        
        
        /**< Ϊ�����ɾ����ʾ���治���� */
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
    
    /**< �жϻ������Ƿ��� */
    if(MMsg.Size >= MENU_MSG_SIZE)
    {
        /**< �����������������Ƚ��յ������ */
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
        /**< ���յ����кͶ���Ϣ */
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
        /**< ���յ�ȡ�����кͶ���Ϣ */
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

********************************************************************************************************/
void MENU_DisPgdnKeyMsg(BOOL nAdd)
{
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
    
    MMsg.DisEndPtr = MMsg.DisStartPtr - (MMsg.DisStartPtr%3);

    MENU_ClearScreen(2,7);                                      /**< Page2---Page7���� */
    
    if(SysSetStoreValue.DisMode == 0)
    {
#if 1
        if(MMsg.Size)
        {
            p = "��";
            /**< ��ʾ��->�� */
            MENU_ShowString(p,2,(MMsg.DisStartPtr%3),0,0);
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
    for(i=0;i<3;i++)
    {
        if(MMsg.Buf[MMsg.DisEndPtr].MsgType != RF_P2D_NONE_MSG)      /**<  ���Ϊ12��15���ָʾΪ�� */
        {
            if(MMsg.Buf[MMsg.DisEndPtr].MsgLen > max)
            {
                len = max;
            }
            else
            {
                len = MMsg.Buf[MMsg.DisEndPtr].MsgLen;
            }

            if((MMsg.DisStartPtr%3) == i)
            {
                of = 1;
            }
            else
            {
                of = 0;
            }
            /**< ��ʾ���ݶ��� */
            MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,i,start,of); 
            MMsg.DisEndPtr++;
        }
        else
        {
            if(i == 0)
            {
                /**< ��ʾ���ա� */
                p = &RfPc2EndAreaMsg.Msg[2];
                n = (128-((RfPc2EndAreaMsg.MsgLen-2)*8))>>1;
                
                MENU_ShowString(p,RfPc2EndAreaMsg.MsgLen-2,1,n,0);
            }
            else
            {
                p = "��";
                /**< ��ʾ���ա� */
                MENU_ShowString(p,2,i,start,0);
            }
            break;
        }
    }
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisRefreshMsg(void)
{
    uint8 *p;
    uint8 i,len,m=0,of=0,n=0;
    uint8 max = 0,start = 0;
    
    m = MMsg.DisEndPtr%3;
    if((m == 0)&&(MMsg.DisEndPtr > 0))
    {
        /**< ��ʾҳ�� */
        return;
    }
    MMsg.DisEndPtr = MMsg.DisEndPtr - (MMsg.DisEndPtr%3);
    
    /**< Page2---Page7���� */
    MENU_ClearScreen(2,7);
    
    if(SysSetStoreValue.DisMode == 0)
    {
        if(MMsg.Size)
        {
            p = "��";
            /**< ��ʾ��->�� */
            MENU_ShowString(p,2,(MMsg.DisStartPtr%3),0,0);
        }
        max = 14;
        start = 16;
    }
    else
    {
        max = 16;
        start = 0;
    }
    
    for(i= 0;i< 3;i++)
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

            if((MMsg.DisStartPtr%3) == i)
            {
                of = 1;
            }
            else
            {
                of = 0;
            }

            /**< ��ʾ���ݶ��� */
            MENU_ShowString(MMsg.Buf[MMsg.DisEndPtr].Msg,len,i,start,of); 
            MMsg.DisEndPtr++;
        }
        else
        {
            if(i == 0)
            {
                /**< ��ʾ���ա� */
                p = &RfPc2EndAreaMsg.Msg[2];
                n = (128-((RfPc2EndAreaMsg.MsgLen-2)*8))>>1;
                
                MENU_ShowString(p,RfPc2EndAreaMsg.MsgLen-2,1,n,0);
            }
            else
            {
                p = "��";
                /**< ��ʾ���ա� */
                MENU_ShowString(p,2,i,start,0);
            }

            break;
        }
    }
}
/********************************************************************************************************

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
		/**< ����Ϣ */
		if(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2W_CALL_MSG)
		{
			/**< Page2---Page7���� */
			MENU_ClearScreen(2,7);
			
	        /**< ��ȡ���ݳ��Ⱥ�����ָ�� */
	        len   	= MMsg.Buf[MMsg.DisStartPtr].MsgLen;
	        pbuf 	= MMsg.Buf[MMsg.DisStartPtr].Msg;
	        
            /**< ��ʾ���ݶ��� */
			MENU_ShowString(pbuf,len,0,0,0); 

            reBack = TRUE;
		}
		else
		{
#if 1
			/**< Page2---Page7���� */
			MENU_ClearScreen(2,7);
			
	        /**< ��ȡ���ݳ��Ⱥ�����ָ�� */
	        len   	= MMsg.Buf[MMsg.DisStartPtr].MsgLen;
	        pbuf 	= MMsg.Buf[MMsg.DisStartPtr].Msg;
            
            /**< ��ʾ���ݶ��� */
			MENU_ShowString(pbuf,len,0,0,0); 

            /*********************************/
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
            
            /**< ��ȡ���ݳ��Ⱥ�����ָ�� */
	        len   	= 14;
	        pbuf 	= &password[0];
            
            
            /**< ��ʾ���ݶ��� */
			MENU_ShowString(pbuf,len,2,0,0); 
			
			reBack = TRUE;
#endif
		}
	}
#if 0
	else
	{
		/**< Page2---Page7���� */
		MENU_ClearScreen(2,7);
		
		/**< ��ʾ���ա� */
		MENU_ShowString(KONG_MSG,4,1,0,0);
		/**< ��ʾ���ա� */
		MENU_ShowString(KONG_MSG,4,1,1*16,0);
		/**< ��ʾ���ա� */
		MENU_ShowString(KONG_MSG,4,1,4*16,0);
        /**< ��ʾ���ա� */
		MENU_ShowString(KONG_MSG,4,1,5*16,0);
				
		reBack = TRUE;
	}
#endif
	return reBack;
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisDeleteShortMsg(BOOL tf)
{
#if 0
    if((MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF0)&&(MMsg.Buf[MMsg.DisStartPtr].MsgType != RF_P2W_CALL_MSG))
    {
        /**< ɾ����ǰ����Ϣ */
        MENU_ReSortMsg(MMsg.DisStartPtr);
    }
#else
    RfW2PCall_t RfW2PCall;
    
    /**< ��������� */
    if( (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF7)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF8)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF4)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xEF)||
        (MMsg.Buf[MMsg.DisStartPtr].Msg[1] == 0xF0))   
    {
        if(MMsg.Buf[MMsg.DisStartPtr].Msg[1] != 0xF0)
        {
            /**< ���ͷ��������Ϣ���� */
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
        /**< ɾ����ǰ����Ϣ */
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
                                    ������ʾ����
********************************************************************************************************/
void MENU_DisOffMsg(void)
{
    uint8 *p;
    
    if(!IsLcdSleep())
    {
        MENU_ClearScreen(2,7);
        p = "�ػ���......";
        MENU_ShowString(p,12,1,8,0); 
    }
    osal_start_timerEx(MSA_TaskId,HBEEAPP_SYS_OFF_EVT,1000);
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisListMsg(void)
{ 
    if(IsLcdSleep())
    {
        if(MenuFlg.ShutDown == 1)
        {
            /**< �ػ���ǲ�ʹ�� */
            MenuFlg.ShutDown = 0;
            /*******************************************/
            //ZDApp_StartJoiningCycle();
            //NLME_SetPollRate(ZDApp_SavedPollRate);                      /**< ������������ */
            
            if(msa_IsStarted == TRUE)
            {
                osal_start_timerEx( MSA_TaskId,MSA_POLL_EVENT,1000);
            }
            else
            {
                 osal_start_timerEx(MSA_TaskId,MSA_SCAN_EVENT,1000);
            }
            
            osal_set_event(MSA_TaskId,HBEEAPP_ONLINE_EVT);             /**< �������� */
            osal_set_event(MSA_TaskId,HBEEAPP_TIME_UPDATEMSG_EVT);     /**< ������ȡʱ�� */
            /*******************************************/
            HBee_SetUserEvent(HB_SET_MOTOR_EVENT,0x1);
        }
        osal_set_event(MSA_TaskId,HBEEAPP_OLED_ON_EVT);
    }
    
    MENU_DisPgdnKeyMsg(FALSE);  
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisDeceiveMsg(void)
{
    uint8 i,j,n;
	uint8 *p;
    uint16 addr;
    uint8 password[16];
    
    MENU_ClearScreen(2,7);
    password[0] = DisNumHexTab[SysSetStoreValue.Channel/16];
    password[1] = DisNumHexTab[SysSetStoreValue.Channel%16];
    MENU_ShowString(password,2,0,0,0);
    
    n = sizeof(RF_SOFTWARE_VER);
    p = RF_SOFTWARE_VER;
	MENU_ShowString(p,n,0,24,0); 
    n = sizeof("IEEE:");
    p = "IEEE:";
    MENU_ShowString(p,n,0,80,0);
    
    
    p = NLME_GetExtAddr();
    for(i=8,j=0;i>0;i--,j++)
    {
        password[(j<<1)]   = DisNumHexTab[p[i-1]/16];
        password[(j<<1)+1] = DisNumHexTab[p[i-1]%16];
    }
    MENU_ShowString(password,16,1,0,0);

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
    
    MENU_ShowString(password,14,2,0,0);
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisResetMsg(void)
{
	uint8 *p;

    MENU_ClearScreen(2,7);
    p = "��λ��......";
    MENU_ShowString(p,12,1,8,0);
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

********************************************************************************************************/
void  MENU_DisLowPower(void)
{
	uint8 *p;

	MENU_ClearScreen(2,7);
    
    p = "������,����";
	MENU_ShowString(p,13,1,12,0); 				            /**< ��ʾ��ص����� */
    HBee_SetUserEvent(HB_SET_MOTOR_EVENT,0x01);
	osal_set_event(MSA_TaskId,HBEEAPP_OLED_ON_EVT);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisIncPower(void)
{
	uint8 *p;
    if(MAIN_POWER == 0)
    {
        MENU_ClearScreen(2,7);
        p = "�����......";
        MENU_ShowString(p,12,1,8,0);
    }
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisUserPassword(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4]={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =8;
        p = "�û�����";
        MENU_ShowString(p,n,0,32,0);
        n =11;
        p = "����������:";
        MENU_ShowString(p,n,1,0,0); 
        
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

	MENU_ShowString(password,4,2,96,0);
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
	uint8 i,n,len,of=0;
	
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

	pMenu->DisEndPtr = pMenu->DisStartPtr - (pMenu->DisStartPtr%3);

	/**< Page2---Page7���� */
	MENU_ClearScreen(2,7);

	for(i=0;i<3;i++)
	{
		if(pMenu->Buf[pMenu->DisEndPtr].MsgType != 0) 	 /**<  ���Ϊ12��15���ָʾΪ�� */
		{
			if(pMenu->Buf[pMenu->DisEndPtr].MsgLen > 16)
			{
				len = 16;
			}
			else
			{
				len = pMenu->Buf[pMenu->DisEndPtr].MsgLen;
			}

			if((pMenu->DisStartPtr%3) == i)
			{
				of = 1;
			}
			else
			{
				of = 0;
			}
			/**< ��ʾ���ݶ��� */
			MENU_ShowString(pMenu->Buf[pMenu->DisEndPtr].pMsg,len,i,0,of); 
			pMenu->DisEndPtr++;
		}
		else
		{
            
            p = "����";
			/**< ��ʾ���ա� */
			MENU_ShowString(p,4,i,0,0);
			break;
		}
	}
}
void MENU_DisUserSetList(void)
{
	/**
	�û�ģʽ
	1.��ʾ����
	2.������
	3.�û�����
	4.�������ģʽ
	����ģʽ
	1.�Զ���ȡ��Ƶ����
	2.����Ƶ��
	3.���þ�������
	4.���õ�ַ
	5.���ó�����
	6.�ָ���������
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
		uSetMenu.Buf[0].pMsg    =  "1-��ʾ����";

		uSetMenu.Buf[1].MsgType =  1;
		uSetMenu.Buf[1].MsgLen	=  10;
		uSetMenu.Buf[1].pMsg    =  "2-������";

		uSetMenu.Buf[2].MsgType =  1;
		uSetMenu.Buf[2].MsgLen	=  10;
		uSetMenu.Buf[2].pMsg    =  "3-�û�����";

		uSetMenu.Buf[3].MsgType =  1;
		uSetMenu.Buf[3].MsgLen	=  14;
		uSetMenu.Buf[3].pMsg    =  "4-�������ģʽ";
	}

	MENU_DisSetPgdnKeyMsg(&uSetMenu,TRUE);
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisUserSetTipNumber(void)
{
    uint8 n;
    uint8 *p;
    uint8 disBuf[4] = "2 ��";
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =14;
        p = "1-������ʾ����";
        MENU_ShowString(p,n,0,8,0);
        n =9;
        p = "��ʾ����:";
        MENU_ShowString(p,n,1,0,0); 
        
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
    MENU_ShowString(disBuf,4,2,96,0);   
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisUserSetKeyMotor(void)
{
	uint8 n;
    uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =14;
        p = "2-���ð�����";
        MENU_ShowString(p,n,0,8,0);

        if(SysSetValue.KeyMotor == 0x01)
        {
            p = "����    ���";
        }
        else
        {
            p = "����    ���";
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
                    p = "����    ���";
                }
                else
                {
                    p = "����    ���";
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
                    p = "  ��    ���";
                }
                else
                {
                    p = "����      ��";
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,12,2,16,0);   
    
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisUserSetPassword(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4]={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =14;
        p = "3-�����û�����";
        MENU_ShowString(p,n,0,8,0);
        n =8;
        p = "������:";
        MENU_ShowString(p,n,1,0,0); 
        
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

	MENU_ShowString(password,4,2,96,0);
}
/********************************************************************************************************

********************************************************************************************************/
void  MENU_DisSysPassword(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4] ={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =8;
        p = "ϵͳ����";;
        MENU_ShowString(p,n,0,32,0);
        n =11;
        p = "����������:";;
        MENU_ShowString(p,n,1,0,0); 
        
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

	MENU_ShowString(password,4,2,96,0);    
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSysGetRfParam(void)
{
    uint8 n;
	uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =16;
        p = "1-�Զ���ȡRF����";;
        MENU_ShowString(p,n,0,0,0);

        if(SysSetValue.AutoGetRf == 0x00)
        {
            p = "������  ��ر�";
        }
        else
        {
            p = "������  ��ر�";     
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
                    p = "������  ��ر�";
                }
                else
                {
                    p = "������  ��ر�";     
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
                    p = "������    �ر�";
                }
                else
                {
                    p = "  ����  ��ر�";     
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,16,2,0,0);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSysSetChannel(void)
{
    uint8 n;
    uint8 *p;
    uint8 disBuf[6] = "11�ŵ�";
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =10;
        p = "2-�ŵ�����";;
        MENU_ShowString(p,n,0,24,0);
        
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
    MENU_ShowString(disBuf,6,2,80,0);   
    
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSysSetPanid(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[4]={0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =12;
        p = "3-PANID ����";;
        MENU_ShowString(p,n,0,16,0);
        
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
    MENU_ShowString(password,4,2,96,0);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSysSetAddr(void)
{
	uint8 n;
	uint8 *p;
    uint8 password[16]={0x32,0x30,0x31,0x34,0x30,0x35,0x30,0x31, 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
    
    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =10;
        p = "4-��ַ����";;
        MENU_ShowString(p,n,0,24,0);
        n =5;
        p = "IEEE:";;
        MENU_ShowString(p,n,1,0,0);
        
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
	MENU_ShowString(password,16,2,0,0);  
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSysSetShowMode(void)
{
    uint8 n,*p;
    uint8 disBuf[5] = "ģʽ1";

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =14;
        p = "5-��ʾģʽ����";;
        MENU_ShowString(p,n,0,8,0);

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
    
	MENU_ShowString(disBuf,5,2,80,0);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisEnShutDown(void)
{
	uint8 n;
	uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =12;
        p = "6-�ػ�����";;
        MENU_ShowString(p,n,0,16,0);

        if(SysSetValue.EnShutdown == 0x01)
        {
            p = "������  ������";
        }
        else
        {
            p = "������  ������";     
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
                    p = "������  ������";
                }
                else
                {
                    p = "������  ������";     
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
                    p = "  ����  ������";
                }
                else
                {
                    p = "������    ������";     
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,16,2,0,0);
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSysResetToDefault(void)
{
    uint8 n;
	uint8 *p;

    if(MenuFlg.SelectBit == 0)
    {
        MENU_ClearScreen(2,7);						                /**< Page2---Page7���� */
        n =14;
        p = "7-�ָ���������";;
        MENU_ShowString(p,n,0,8,0);

        if(SysSetValue.ResetToDefault == 0x01)
        {
            p = "������  ������";
        }
        else
        {
            p = "������  ������";     
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
                    p = "������  ������";
                }
                else
                {
                    p = "������  ������";     
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
                    p = "  ����  ������";
                }
                else
                {
                    p = "������    ������";     
                }
            }
        }
        MenuFlg.ShowFlash = !MenuFlg.ShowFlash;
    }    
    MENU_ShowString(p,16,2,0,0);
}
/********************************************************************************************************

********************************************************************************************************/
