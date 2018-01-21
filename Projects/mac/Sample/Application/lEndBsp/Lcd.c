
/********************************************************************************************************

********************************************************************************************************/
#include "Lcd.h"
#include "hal_SPI1.h"
/********************************************************************************************************

********************************************************************************************************/
#define M_SPI
//#define ILI9163C
#define ST7735S
#define BCOLOR           BLACK
/********************************************************************************************************

********************************************************************************************************/
uint8 LcdWorkFlag = TRUE;
/********************************************************************************************************

********************************************************************************************************/
void uDelay(uint8 l)
{
	while(l--);
}
/********************************************************************************************************
                                        �����ʱ
********************************************************************************************************/
void Delay(uint8 n)
{
	uint8 i,j,k;

	for(k=0;k<n;k++)
	{
		for(i=0;i<131;i++)
		{
			for(j=0;j<15;j++)
			{
				uDelay(203);	
			}
		}
	}
}
/********************************************************************************************************
                                        �����ʱ
********************************************************************************************************/
void DelayForUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}
/********************************************************************************************************
                                          Ӳ��SPIд����
Data:����
                                          
********************************************************************************************************/
void LCD_WriteCommand(uint8 Data)
{
    OLED_CS=0;
    OLED_DC=0;
    
    SPI_TX(Data);
    SPI_WAIT_RXRDY();
    
    OLED_DC=1;
    OLED_CS=1;
}
/********************************************************************************************************
                                          Ӳ��SPIд����
Data:����
********************************************************************************************************/
void LCD_WriteData(uint8 Data)
{
    OLED_CS=0;
    OLED_DC=1;
    
    SPI_TX(Data);
    SPI_WAIT_RXRDY();
    
    OLED_DC=1;
    OLED_CS=1;
}
/*------------------------------------------------------------------------------
                                    д2���ֽ�����
Data:2���ֽ�����
------------------------------------------------------------------------------*/
void LCD_Write_TwoData(uint16 Data)
{
  LCD_WriteData(Data>>8);
  LCD_WriteData(Data);
}
/********************************************************************************************************
								���ÿ�ʼ��
ys:��ʼ��
ye:������
********************************************************************************************************/
void LCD_SetStartColumn(uint8 ys, uint8 ye)
{
    #ifdef ILI9163C
	LCD_WriteCommand(0x2A);  //Colulm Address Set
	LCD_WriteData(0x00);			
	LCD_WriteData(ys);     			
	LCD_WriteData(0x00);		
	LCD_WriteData(ye); 
	#endif
	
	#ifdef ST7735S
	LCD_WriteCommand(0x2A);
    LCD_WriteData(0x00);
    LCD_WriteData(ys+2);  //��2����ΪST7735S������Һ�����е�ַ��Ҫƫ��2��ILI9163C����ƫ��
    LCD_WriteData(0x00);
    LCD_WriteData(ye+2);  //��2����ΪST7735S������Һ�����е�ַ��Ҫƫ��2��ILI9163C����ƫ�� 
    #endif
}
/********************************************************************************************************
								���ÿ�ʼҳ(��)
xs:��ʼ��
xe:������
********************************************************************************************************/
void LCD_SetStartPage(uint8 xs, uint8 xe)
{
	#ifdef ILI9163C
	LCD_WriteCommand(0x2B);  //Page Address Set
	LCD_WriteData(0x00);			
	LCD_WriteData(xs);     			
	LCD_WriteData(0x00);		
	LCD_WriteData(xe);
	#endif

	#ifdef ST7735S
    LCD_WriteCommand(0x2B);
    LCD_WriteData(0x00);
    LCD_WriteData(xs+3);   //��3����ΪST7735S������Һ�����е�ַ��Ҫƫ��3��ILI9163C����ƫ��
    LCD_WriteData(0x00);
    LCD_WriteData(xe+3);   //��3����ΪST7735S������Һ�����е�ַ��Ҫƫ��3��ILI9163C����ƫ��
    #endif
}
/********************************************************************************************************
                                    ������������ɫ
xs:����ʼ��ַ
ys:����ʼ��ַ
xe:�н�����ַ
ye:�н�����ַ
color:��ɫ
********************************************************************************************************/
void Fill_Area_Color(uint8 xs, uint8 ys, uint8 xe, uint8 ye, uint16 color) 
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
	LCD_WriteData(xs+2);     //��2����ΪST7735S������Һ�����е�ַ��Ҫƫ��2��ILI9163C����ƫ��			
	LCD_WriteData(0x00);		
	LCD_WriteData(xe+2);     //��2����ΪST7735S������Һ�����е�ַ��Ҫƫ��2��ILI9163C����ƫ��			

	LCD_WriteCommand(0x2B);  //Page Address Set
	LCD_WriteData(0x00);		
	LCD_WriteData(ys+3);     //��3����ΪST7735S������Һ�����е�ַ��Ҫƫ��3��ILI9163C����ƫ��			
	LCD_WriteData(0x00);		
	LCD_WriteData(ye+3);     //��3����ΪST7735S������Һ�����е�ַ��Ҫƫ��3��ILI9163C����ƫ��			
    #endif
	
	LCD_WriteCommand(0x2C); 

    for(i=0;i<=(xe-xs);i++)
    {
        for(j=0;j<=(ye-ys);j++)
        LCD_Write_TwoData(color);
    }
}
/********************************************************************************************************
                                           LCD��ʼ��
 �����ILI9163C�������е�ַ����Ҫƫ��
 �����ST7735S�����е�ַƫ��3���е�ַƫ��2
********************************************************************************************************/
void LCD_Init(void)
{
	#ifdef ILI9163C 
    OLED_RES=0;
	DelayForUs(50000);
	OLED_RES=1;
	DelayForUs(50000);

	LCD_WriteCommand(0x11); //�˳�˯��
    DelayForUs(50000);
    DelayForUs(50000);
    DelayForUs(50000);

	LCD_WriteCommand(0x26); //Set Default Gamma٤������
	LCD_WriteData(0x04);
	LCD_WriteCommand(0xB1);//Set Frame Rate֡������
	LCD_WriteData(0x08);
	LCD_WriteData(0x14);
	LCD_WriteCommand(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD ��ѹ����
	LCD_WriteData(0x08);    //�˴��Ƿ�Ҫ�޸�ΪLCD_WriteData(0x1F),GVDDΪ3.0V
	LCD_WriteData(0x00);
	LCD_WriteCommand(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL��ѹ����
	LCD_WriteData(0x05);
	LCD_WriteCommand(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML��ѹ����
	LCD_WriteData(0x46);	//�˴��Ƿ�Ҫ�޸�ΪLCD_WriteData(0x14),VOMHΪ3.0V
	LCD_WriteData(0x40);
	LCD_WriteCommand(0xC7);// Set VMF��ѹ����
	LCD_WriteData(0xC2);   //�ֲ���Ϊ0x40
	LCD_WriteCommand(0x3a); //Set Color Format�������ظ�ʽ
	LCD_WriteData(0x05);   //�ֲ���Ϊ0x66
	LCD_WriteCommand(0x2A); //Set Column Address�е�ַ����
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x7F);	//128
	LCD_WriteCommand(0x2B); //Set Page Addressҳ��ַ����
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x7F);	//128
	LCD_WriteCommand(0xB4);	//��ʾ�������
	LCD_WriteData(0x00);	//�ֲ���Ϊ0x02
	LCD_WriteCommand(0xEC); //Set pumping clock frequence(�����ֲ��Ҳ��������������)
	LCD_WriteData(0x0C);
	LCD_WriteCommand(0xf2); //Enable Gamma bit٤���������
	LCD_WriteData(0x01);
	LCD_WriteCommand(0x36);	//�ڴ���ʿ���
    
    LCD_WriteData(0x88);	//��ֱˢ�������ϵ���,BGR,�����Ǵ����ң����ϵ������У����Ͻ�Ϊ��0,0�����½�Ϊ��127,127����,Ҫ��ΪBGR������ʾ��ɫ
    //LCD_WriteData(0x00);	//��ֱˢ�������µ���,RGB,�����Ǵ����ң����µ������У����½�Ϊ��0,0�����Ͻ�Ϊ��127,127����
    //LCD_WriteData(0x20);	//ˮƽˢ����������,RGB,�����Ǵ����ң����µ������У����½�Ϊ��0,0�����Ͻ�Ϊ��127,127����
    //LCD_WriteData(0x60);  //ˮƽˢ�������ҵ���,RGB,�����Ǵ��ҵ��󣬴��µ������У����½�Ϊ��0,0�����Ͻ�Ϊ��127,127����
    
    //LCD_WriteData(0x40);  //��ֱˢ�������µ���,RGB,�����Ǵ��ҵ��󣬴��µ������У����½�Ϊ��0,0�����Ͻ�Ϊ��127,127����
    //LCD_WriteData(0xC0);  //��ֱˢ�������ϵ���,RGB,�����Ǵ��ҵ��󣬴��ϵ������У����Ͻ�Ϊ��0,0�����½�Ϊ��127,127����
    
    //LCD_WriteData(0xA0);  //ˮƽˢ����������,RGB,�����Ǵ�����,���ϵ������У����Ͻ�Ϊ��0,0�����½�Ϊ��127,127����
    //LCD_WriteData(0xE0);  //ˮƽˢ�������ҵ���,RGB,�����Ǵ��ҵ���,���ϵ������У����Ͻ�Ϊ��0,0�����½�Ϊ��127,127����
    
	LCD_WriteCommand(0xE0);	//����٤����������
	LCD_WriteData(0x3F);//p1
	LCD_WriteData(0x26);//p2
	LCD_WriteData(0x23);//p3
	LCD_WriteData(0x30);//p4
	LCD_WriteData(0x28);//p5
	LCD_WriteData(0x10);//p6
	LCD_WriteData(0x55);//p7
	LCD_WriteData(0xB7);//p8
	LCD_WriteData(0x40);//p9
	LCD_WriteData(0x19);//p10
	LCD_WriteData(0x10);//p11
	LCD_WriteData(0x1E);//p12
	LCD_WriteData(0x02);//p13
	LCD_WriteData(0x01);//p14
	LCD_WriteData(0x00);//p15
	LCD_WriteCommand(0xE1);	//����٤����������
	LCD_WriteData(0x00);//p1
	LCD_WriteData(0x19);//p2
	LCD_WriteData(0x1C);//p3
	LCD_WriteData(0x0F);//p4
	LCD_WriteData(0x14);//p5
	LCD_WriteData(0x0F);//p6
	LCD_WriteData(0x2A);//p7
	LCD_WriteData(0x48);//p8
	LCD_WriteData(0x3F);//p9
	LCD_WriteData(0x06);//p10
	LCD_WriteData(0x1D);//p11
	LCD_WriteData(0x21);//p12
	LCD_WriteData(0x3d);//p13
	LCD_WriteData(0x3e);//p14
	LCD_WriteData(0x3f);//p15
	LCD_WriteCommand(0x29); // Display On����ʾ
	#endif

	#ifdef ST7735S
    OLED_RES=0;
	DelayForUs(50000);
	DelayForUs(50000);
	OLED_RES=1;
	DelayForUs(50000);

	LCD_WriteCommand(0x11); //�˳�˯��
    DelayForUs(50000);
    DelayForUs(50000);
    DelayForUs(50000);

	//ST7735R Frame Rate
	LCD_WriteCommand(0xB1); 
	LCD_WriteData(0x01); 
	LCD_WriteData(0x2C); 
	LCD_WriteData(0x2D); 

	LCD_WriteCommand(0xB2); 
	LCD_WriteData(0x01); 
	LCD_WriteData(0x2C); 
	LCD_WriteData(0x2D); 

	LCD_WriteCommand(0xB3); 
	LCD_WriteData(0x01); 
	LCD_WriteData(0x2C); 
	LCD_WriteData(0x2D); 
	LCD_WriteData(0x01); 
	LCD_WriteData(0x2C); 
	LCD_WriteData(0x2D); 
	
	LCD_WriteCommand(0xB4); //Column inversion 
	LCD_WriteData(0x07); 
	
	//ST7735R Power Sequence
	LCD_WriteCommand(0xC0); 
	LCD_WriteData(0xA2); 
	LCD_WriteData(0x02); 
	LCD_WriteData(0x84); 
	LCD_WriteCommand(0xC1); 
	LCD_WriteData(0xC5); 

	LCD_WriteCommand(0xC2); 
	LCD_WriteData(0x0A); 
	LCD_WriteData(0x00); 

	LCD_WriteCommand(0xC3); 
	LCD_WriteData(0x8A); 
	LCD_WriteData(0x2A); 
	LCD_WriteCommand(0xC4); 
	LCD_WriteData(0x8A); 
	LCD_WriteData(0xEE); 
	
	LCD_WriteCommand(0xC5); //VCOM 
	LCD_WriteData(0x0E); 
	
	LCD_WriteCommand(0x36); //MX, MY, RGB mode 
	LCD_WriteData(0xC8); 
	
	//ST7735R Gamma Sequence
	LCD_WriteCommand(0xe0); 
	LCD_WriteData(0x0f); 
	LCD_WriteData(0x1a); 
	LCD_WriteData(0x0f); 
	LCD_WriteData(0x18); 
	LCD_WriteData(0x2f); 
	LCD_WriteData(0x28); 
	LCD_WriteData(0x20); 
	LCD_WriteData(0x22); 
	LCD_WriteData(0x1f); 
	LCD_WriteData(0x1b); 
	LCD_WriteData(0x23); 
	LCD_WriteData(0x37); 
	LCD_WriteData(0x00); 	
	LCD_WriteData(0x07); 
	LCD_WriteData(0x02); 
	LCD_WriteData(0x10); 

	LCD_WriteCommand(0xe1); 
	LCD_WriteData(0x0f); 
	LCD_WriteData(0x1b); 
	LCD_WriteData(0x0f); 
	LCD_WriteData(0x17); 
	LCD_WriteData(0x33); 
	LCD_WriteData(0x2c); 
	LCD_WriteData(0x29); 
	LCD_WriteData(0x2e); 
	LCD_WriteData(0x30); 
	LCD_WriteData(0x30); 
	LCD_WriteData(0x39); 
	LCD_WriteData(0x3f); 
	LCD_WriteData(0x00); 
	LCD_WriteData(0x07); 
	LCD_WriteData(0x03); 
	LCD_WriteData(0x10); 
	
	LCD_WriteCommand(0x2a);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x7f);
	//LCD_WriteData(0x83);

	LCD_WriteCommand(0x2b);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x7f);
	//LCD_WriteData(0x83);
	//LCD_WriteData(0x9f);

	/*
	LCD_WriteCommand(0xF0); //Enable test command  
	LCD_WriteData(0x01); 
	LCD_WriteCommand(0xF6); //Disable ram power save mode 
	LCD_WriteData(0x00); 
	*/
    
	LCD_WriteCommand(0x3A); //65k mode 
	LCD_WriteData(0x05); 
	
	LCD_WriteCommand(0x29);//Display on	

	#endif

    Fill_Area_Color(0,0,127,127,BCOLOR);
    
    P0_7 = 1;  //�������
}
/********************************************************************************************************
							
********************************************************************************************************/
void LCD_SetWork(BOOL on)
{
    if(on == TRUE)
    {
      if(LcdWorkFlag == FALSE)
      {
        LCD_WriteCommand(0x11); //�˳�˯��
		DelayForUs(50000);	//��Ҫ����ʱ120ms
		DelayForUs(50000);
		DelayForUs(50000);
        P0_7 = 1;   //P0.7Ϊ����ƿ�
        LcdWorkFlag = TRUE;
      }
    }
    else
    {
        LCD_WriteCommand(0x10); //����˯��
        DelayForUs(50000);
        P0_7 = 0;   //P0.7Ϊ����ƹ�
        LcdWorkFlag = FALSE;
    }
}
/********************************************************************************************************

********************************************************************************************************/

