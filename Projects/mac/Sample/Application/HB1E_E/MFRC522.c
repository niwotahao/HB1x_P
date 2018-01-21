#include "hal_types.h"
#include "hal_defs.h"
#include "MFRC522.h"
#include "hal_board.h"
#include "hal_SPI1.h"
/*******************************************************************************************************/
#define MAXRLEN 18
/*******************************************************************************************************/
uint8 ReciveFlag;
bit_data_t bit_data;


/*******************************************************************************************************/
void PcdComMF522Seventh(void);
void PcdComMF522Sixth(void);
void ReadDataFirst(void);
void PcdComMF522Fifth(void);
void PcdComMF522Fourth(void);
void ReadDataFromBuf(void);
void ReadBuffer(void);


void halMcuWaitUs(uint16 usec);
void halMcuWaitMs(uint16 msec);
/*******************************************************************************************************


********************************************************************************************************/
#pragma optimize=none
void halMcuWaitUs(uint16 usec)
{
    usec>>= 1;
    while(usec--)
    {
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
    }
}
/*******************************************************************************************************


********************************************************************************************************/
#pragma optimize=none
void halMcuWaitMs(uint16 msec)
{
    while(msec--)
        halMcuWaitUs(1000);
}
/*******************************************************************************************************


********************************************************************************************************/
void SPI_Init(void)
{   
#ifdef H_SPI
  
    uint8 baud_exponent;
    uint8 baud_mantissa;
    
    // Set SPI on UART 1 alternative 2
    PERCFG |= 0x02;
    // Use SPI on USART 1 alternative 2
    //1 7
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_SPI_MISO_PORT, HAL_BOARD_IO_SPI_MISO_PIN);
    //1 6
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_SPI_MOSI_PORT, HAL_BOARD_IO_SPI_MOSI_PIN);
    //1 5
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_SPI_CLK_PORT,  HAL_BOARD_IO_SPI_CLK_PIN);
    //0 7
    MCU_IO_OUTPUT(HAL_BOARD_IO_EM_CS_PORT, HAL_BOARD_IO_EM_CS_PIN, 1);

    //baud_exponent = 18;            /**< 4MHZ */    
    //baud_mantissa = 0;

    //baud_exponent = 15;           /**< 1MHZ */
    //baud_mantissa = 0;

    //baud_exponent = 16;           /**< 2MHZ */
    //baud_mantissa = 0;
    
    baud_exponent = 17;         /**< 4MHZ */
    baud_mantissa = 0;
    
    // Configure peripheral
    U1UCR  = 0x80; 
    U1CSR  = 0x40;               // SPI mode, master.
    U1GCR  = SPI_TRANSFER_MSB_FIRST | SPI_CLOCK_PHA_0 | SPI_CLOCK_POL_LO | baud_exponent;  
    U1BAUD = baud_mantissa;
    
#else
    //模拟SPI
    P1SEL &= ~0xE0; //P1.5,P1.6,P1.7
    P1DIR |= 0x60;
    P1DIR &= ~0x80;
#endif
    
    P0SEL &= ~0xC0;  //P0.6,P0.7
    P0DIR |= 0xC0;
    
    P2SEL &= ~0x01;  /**< P2.0通用I/O */
    P2DIR &= ~0x01;  /**< P2.0输入 */
    P2INP &= ~0X81;  /**< P2.0 上拉 支持上下拉  */  
  
  
    PICTL |= 0x08;  //Falling edge
    P2IEN |= 0x01; //P2.0 Enable interrupt Mask
    IEN2 |= 0x02;  //BV(5)P2 Interrupt enable
    P2IFG = 0x00; //Clear any pending interrupt
}
/*******************************************************************************************************


********************************************************************************************************/
uint8 SPI_WriteReadData(uint8 Data)
{
    uint8 rxData;

    SPI_TX(Data);
    SPI_WAIT_RXRDY();
    rxData = SPI_RX();
    
    return rxData;
}
/*******************************************************************************************************
功    能：读RC632寄存器
参数说明：Address[IN]:寄存器地址
返    回：读出的值
********************************************************************************************************/
uint8 ReadRawRC(uint8 Address)
{ 
    uint8 ucAddr;
    uint8 ucResult=0;
    ucAddr = (((Address<<1)&0x7E)|0x80);
    
#ifdef H_SPI
    
    //硬件SPI
    MF522_NSS = 0;
    SPI_WriteReadData(ucAddr);
    ucResult = SPI_WriteReadData(0x00);
    MF522_NSS = 1;
    
#else
     //模拟SPI
     uint8 i;
     
     MF522_SCK = 0;
     MF522_NSS = 0;
     ucAddr = ((Address<<1)&0x7E)|0x80;

     for(i=8;i>0;i--)
     {
         MF522_SI = ((ucAddr&0x80)==0x80);
         MF522_SCK = 1;
         ucAddr <<= 1;
         MF522_SCK = 0;
     }

     for(i=8;i>0;i--)
     {
         MF522_SCK = 1;
         ucResult <<= 1;
         //ucResult|=(bit)MF522_SO;
         //ucResult|=(bit_data.firstbit)MF522_SO;
         bit_data.firstbit = MF522_SO;  //添加修改
         ucResult|= bit_data.firstbit;  //添加修改
         MF522_SCK = 0;
     }
     MF522_NSS = 1;
     MF522_SCK = 1;
     
#endif
     return ucResult;
}
/*******************************************************************************************************
功    能：写RC632寄存器
参数说明：Address[IN]:寄存器地址
          value[IN]:写入的值
********************************************************************************************************/
void WriteRawRC(uint8 Address, uint8 value)
{  
    uint8 ucAddr;
    ucAddr = ((Address<<1)&0x7E);

#ifdef H_SPI
    
    //硬件SPI
    MF522_NSS = 0;
    SPI_WriteReadData(ucAddr);
    SPI_WriteReadData(value);
    MF522_NSS = 1;
    
#else
    
    //模拟SPI
    uint8 i;   

    MF522_SCK = 0;
    MF522_NSS = 0;
    ucAddr = ((Address<<1)&0x7E);

    for(i=8;i>0;i--)
    {
        MF522_SI = ((ucAddr&0x80)==0x80);
        MF522_SCK = 1;
        ucAddr <<= 1;
        MF522_SCK = 0;
    }

    for(i=8;i>0;i--)
    {
        MF522_SI = ((value&0x80)==0x80);
        MF522_SCK = 1;
        value <<= 1;
        MF522_SCK = 0;
    }
    MF522_NSS = 1;
    MF522_SCK = 1;
#endif
}
/*******************************************************************************************************
功    能：复位RC522
返    回: 成功返回MI_OK
********************************************************************************************************/
int8 PcdReset(void)
{
    MF522_RST=1;
    halMcuWaitUs(100);
    MF522_RST=0;
    halMcuWaitUs(100);
    MF522_RST=1;
    halMcuWaitUs(100);
    WriteRawRC(CommandReg,PCD_RESETPHASE); //软件复位RC522
    halMcuWaitUs(200);
    
    WriteRawRC(ModeReg,0x3D);            	//和Mifare卡通讯，CRC初始值0x6363,传输开始
    /*
    WriteRawRC(TReloadRegL,0xFF);     		//为16位定时器加载计数数值(最大值65535)48      
    WriteRawRC(TReloadRegH,0xFF);
    WriteRawRC(TModeReg,0x8F);           	//定时器自动运行，定时器不自动重载，定时器从初始值倒计时到0后触发一个定时中断,f(timer频率)=6.78MHz/TPrescaler
    //WriteRawRC(TModeReg,0x9F);
	WriteRawRC(RxModeReg,0x04);
    WriteRawRC(TPrescalerReg,0xFF);      	//定时器时钟频率6.78MHz由27.12MHz分频得来，12位预定标器数值(最大值4095)，最大时间=65535*4095/6.78MHz=39.59s，TPrescaler=0xD3E=3390,定时器的时间为T=48*3390/6.78MHz=24ms
    */
    
    
    /*
    WriteRawRC(TReloadRegL,30);          	//为16位定时器加载计数数值(最大值65535)48      
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);           	//定时器自动运行，定时器不自动重载，定时器从初始值倒计时到0后触发一个定时中断,f(timer频率)=6.78MHz/TPrescaler
    WriteRawRC(TPrescalerReg,0x3E);      	//定时器时钟频率6.78MHz由27.12MHz分频得来，12位预定标器数值(最大值4095)，最大时间=65535*4095/6.78MHz=39.59s，TPrescaler=0xD3E=3390,定时器的时间为T=48*3390/6.78MHz=24ms
    */
    
    
    WriteRawRC(TxModeReg,0x0);
    WriteRawRC(RxModeReg,0x0);
    //WriteRawRC(TxControlReg,0x80);
    //WriteRawRC(TxSelReg,0x10);

    
    WriteRawRC(TxAutoReg,0x40);          	//100%ASK调制
    
    return MI_OK;
}
/*******************************************************************************************************
开启天线  
每次启动或关闭天险发射之间应至少有1ms的间隔
********************************************************************************************************/
void PcdAntennaOn(void)
{
    uint8 i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);  //使能天线TX1和TX2,传送13.56MHz的调制载波传输数据
    }
    halMcuWaitMs(1);
}
/*******************************************************************************************************
关闭天线
********************************************************************************************************/
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}
/*******************************************************************************************************
进入软件低功耗 
********************************************************************************************************/
void PcdPowerDown(void)
{
    //WriteRawRC(CommandReg,0x10);
    SetBitMask(CommandReg,0x10);
}
/*******************************************************************************************************
唤醒
********************************************************************************************************/
void PcdWakeUp(void)
{
    SetBitMask(CommandReg,0x00);
    halMcuWaitMs(1);
}
/*******************************************************************************************************
功    能：寻卡
参数说明: req_code[IN]:寻卡方式
                0x52 = 寻感应区内所有符合14443A标准的卡
                0x26 = 寻未进入休眠状态的卡
          pTagType[OUT]：卡片类型代码
                0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
返    回: 成功返回MI_OK
********************************************************************************************************/
int8 PcdRequest(uint8 req_code,uint8 *pTagType)
{
   int8 status;  
   uint16 unLen;
   uint8 ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08);       //不加密传输
   WriteRawRC(BitFramingReg,0x07);      //定义最后一个字节的多少位要被传输(如果为000b那么最后一个字节的全部位都被传输)，7位被传输

   SetBitMask(TxControlReg,0x03);       //使能天线TX1和TX2,传送13.56MHz的调制载波传输数据
   
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   
		status = MI_ERR;   
   }
   
   return status;
}
/*******************************************************************************************************
功    能：防冲撞
参数说明: pSnr[OUT]:卡片序列号，4字节
返    回: 成功返回MI_OK
********************************************************************************************************/
int8 PcdAnticoll(uint8 *pSnr)
{
    int8    status;
    uint8   i,snr_check=0;
    uint16  unLen;
    uint8   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}
/*******************************************************************************************************


********************************************************************************************************/
void PcdRequest2(void)
{
	ClearBitMask(Status2Reg,0x08);  //不加密传输
	WriteRawRC(BitFramingReg,0x07);  //定义最后一个字节的多少位要被传输(如果为000b那么最后一个字节的全部位都被传输)，7位被传输
	SetBitMask(TxControlReg,0x03);  //使能天线TX1和TX2,传送13.56MHz的调制载波传输数据

	WriteRawRC(FIFODataReg, 0x52);         //写进64字节的FIFO缓存区
	WriteRawRC(CommandReg, 0x04);          //启动发送
	//WriteRawRC(CommandReg, 0x0C);          //启动发送
	SetBitMask(BitFramingReg,0x80);        //开始传送
	SetBitMask(ControlReg,0x80);           //停止定时器
	WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令
}
/*******************************************************************************************************


********************************************************************************************************/
int8 PcdAnticollSecond(uint8 *pSnr)
{
    int8 status;
    uint8 i,snr_check=0;
    uint16  unLen;
    uint8 ucComMF522Buf[MAXRLEN]; 
	uint8 *ReciveData;
    
    ClearBitMask(Status2Reg,0x08);   //不加密传输
    WriteRawRC(BitFramingReg,0x00);  //最后一个字节的全部位都被传送
    ClearBitMask(CollReg,0x80);      //如果有一个冲撞则全部接收的位都清0(此情况只在106kbit/s传输时的按位防冲撞中有效，其他情况则设为1)
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;
    //status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);    //原始代码
	status = PcdComMF522Second(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);  //修改的代码

    if (status == MI_OK)
    {
         //修改的代码
         for (i=0; i<MAXRLEN; i++)
         {   
           ReciveData[i] = ReadRawRC(FIFODataReg);    //从FIFO缓存区读取n个字节数据
         }  
         
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ReciveData[i];
             snr_check ^= ReciveData[i];
         }
         if (snr_check != ReciveData[i])
         {   
           status = MI_ERR;    
         }
         /*
         //原始代码
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
		 */
    }
    SetBitMask(CollReg,0x80); //将按位防冲撞设置为1(只有106kbit/s需要按位防冲撞，其他的不需要按位防冲撞则设置为1)
    return status;
}
/*******************************************************************************************************
功    能：选定卡片
参数说明: pSnr[IN]:卡片序列号，4字节
返    回: 成功返回MI_OK
********************************************************************************************************/
int8 PcdSelect(uint8 *pSnr)
{
    int8 status;
    uint8 i;
    uint16  unLen;
    uint8 ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   
      status = MI_OK;  
    }
    else
    {   
      status = MI_ERR;    
    }

    return status;
}
/*******************************************************************************************************
功    能：验证卡片密码
参数说明: auth_mode[IN]: 密码验证模式
                 0x60 = 验证A密钥
                 0x61 = 验证B密钥 
          addr[IN]：块地址
          pKey[IN]：密码
          pSnr[IN]：卡片序列号，4字节
返    回: 成功返回MI_OK
********************************************************************************************************/           
int8 PcdAuthState(uint8 auth_mode,uint8 addr,uint8 *pKey,uint8 *pSnr)
{
    int8 status;
    uint16  unLen;
    uint8 i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    
      ucComMF522Buf[i+2] = *(pKey+i);   
    }
    for (i=0; i<6; i++)
    {    
      ucComMF522Buf[i+8] = *(pSnr+i);   
    }
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   
      status = MI_ERR;   
    }
    
    return status;
}
/*******************************************************************************************************
功    能：读取M1卡一块数据
参数说明: addr[IN]：块地址
          pData[OUT]：读出的数据，16字节
返    回: 成功返回MI_OK
********************************************************************************************************/
int8 PcdRead(uint8 addr,uint8 *pData)
{
    int8 status;
    uint16 unLen;
    uint8 i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {    
          *(pData+i) = ucComMF522Buf[i];   
        }
    }
    else
    {   
      status = MI_ERR;   
    }
    
    return status;
}
/*******************************************************************************************************
功    能：写数据到M1卡一块
参数说明: addr[IN]：块地址
          pData[IN]：写入的数据，16字节
返    回: 成功返回MI_OK
********************************************************************************************************/             
int8 PcdWrite(uint8 addr,uint8 *pData)
{
    int8 status;
    uint16 unLen;
    uint8 i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
      status = MI_ERR;   
    }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    
          ucComMF522Buf[i] = *(pData+i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   
          status = MI_ERR;   
        }
    }
    return status;
}
/*******************************************************************************************************
功    能：命令卡片进入休眠状态
返    回: 成功返回MI_OK
********************************************************************************************************/
int8 PcdHalt(void)
{
    int8 status;
    uint16 unLen;
    uint8 ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}
/*******************************************************************************************************
用MF522计算CRC16函数
********************************************************************************************************/
void CalulateCRC(uint8 *pIndata,uint8 len,uint8 *pOutData)
{
    uint8 i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   
      WriteRawRC(FIFODataReg, *(pIndata+i));   
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}
/*******************************************************************************************************
设置RC632的工作方式 
********************************************************************************************************/
int8 M500PcdConfigISOType(uint8 type)
{
   if (type == 'A')                     //ISO14443_A
   { 
        ClearBitMask(Status2Reg,0x08);
        /*
        WriteRawRC(CommandReg,0x20);        //as default   
        WriteRawRC(ComIEnReg,0x80);         //as default
        WriteRawRC(DivlEnReg,0x0);          //as default
        WriteRawRC(ComIrqReg,0x04);         //as default
        WriteRawRC(DivIrqReg,0x0);          //as default
        WriteRawRC(Status2Reg,0x0);//80     //trun off temperature sensor
        WriteRawRC(WaterLevelReg,0x08);     //as default
        WriteRawRC(ControlReg,0x20);        //as default
        WriteRawRC(CollReg,0x80);           //as default
        */
        WriteRawRC(ModeReg,0x3D);//3F
        /*	   
        WriteRawRC(TxModeReg,0x0);          //as default???
        WriteRawRC(RxModeReg,0x0);          //as default???
        WriteRawRC(TxControlReg,0x80);      //as default???
        WriteRawRC(TxSelReg,0x10);          //as default???
        */
        WriteRawRC(RxSelReg,0x86);//84
        //WriteRawRC(RxThresholdReg,0x84);  //as default
        //WriteRawRC(DemodReg,0x4D);        //as default
        
        //WriteRawRC(ModWidthReg,0x13);     //26
        WriteRawRC(RFCfgReg,0x7F);          //4F
        /*   
        WriteRawRC(GsNReg,0x88);            //as default???
        WriteRawRC(CWGsCfgReg,0x20);        //as default???
        WriteRawRC(ModGsCfgReg,0x20);       //as default???
        */
        WriteRawRC(TReloadRegL,30);         //tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
        WriteRawRC(TReloadRegH,0);
        WriteRawRC(TModeReg,0x8D);
        WriteRawRC(TPrescalerReg,0x3E);

        //PcdSetTmo(106);

        halMcuWaitMs(10);
        PcdAntennaOn();
   }
   else
   { 
     return -1; 
   }
   
   return MI_OK;
}
/*******************************************************************************************************
功    能：置RC522寄存器位
参数说明：reg[IN]:寄存器地址
          mask[IN]:置位值
********************************************************************************************************/
void SetBitMask(uint8 reg,uint8 mask)  
{
    int8 tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}
/*******************************************************************************************************
功    能：清RC522寄存器位
参数说明：reg[IN]:寄存器地址
          mask[IN]:清位值
********************************************************************************************************/
void ClearBitMask(uint8 reg,uint8 mask)  
{
    int8 tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 
/*******************************************************************************************************
功    能：通过RC522和ISO14443卡通讯
参数说明：Command[IN]:RC522命令字
          pInData[IN]:通过RC522发送到卡片的数据
          InLenByte[IN]:发送数据的字节长度
          pOutData[OUT]:接收到的卡片返回数据
          *pOutLenBit[OUT]:返回数据的位长度
********************************************************************************************************/
int8 PcdComMF522(uint8 Command, 
                 uint8 *pInData, 
                 uint8 InLenByte,
                 uint8 *pOutData, 
                 uint16 *pOutLenBit)
{
    int8 status = MI_ERR;
    uint8 irqEn   = 0x00;
    uint8 waitFor = 0x00;
    uint8 lastBits;
    uint8 n;
    uint16 i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
    WriteRawRC(ComIEnReg,irqEn|0x80);  //引脚IRQ跟Status1Reg的IRq位相反，使能发送，接受，空闲，超时，错误中断，在IRQ脚输出低电平  
    ClearBitMask(ComIrqReg,0x80);      //清除所有使能IRQ(发送，接受，空闲，超时，错误)标志位
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);     //清除FIFO缓存读写指示器和缓存溢出标志在错误寄存器
    
    for (i=0; i<InLenByte; i++)
    {   
    	WriteRawRC(FIFODataReg, pInData[i]);  		//写进64字节的FIFO缓存区  
    }  
    WriteRawRC(CommandReg, Command);               	//启动发送和接收
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
    	SetBitMask(BitFramingReg,0x80);  			//开始传送		
   	}      	
    
	//i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
	i = 300;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  				//查询是否有(发送，接收，空闲，超时，错误)中断
         i--;  
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));  	//i=0,或者超时中断，空闲中断，接收中断则退出循环
    
    ClearBitMask(BitFramingReg,0x80);   			//停止传送
	      
    if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) 			//判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
         {
             status = MI_OK;
             if (n & irqEn & 0x01)      			//判断是否是超时中断
             {   
             	status = MI_NOTAGERR;   
             }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);  		//读取在FIFO缓存区的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
                if (lastBits)
                {   
                	*pOutLenBit = (n-1)*8 + lastBits;   
                }
                else
                {
                	*pOutLenBit = n*8;   
                }
                if (n == 0)
                {   
                	n = 1;    
                }
                if (n > MAXRLEN)
                {   
                	n = MAXRLEN;   
                }
                for (i=0; i<n; i++)
                {   
                	pOutData[i] = ReadRawRC(FIFODataReg);   //从FIFO缓存区读取n个字节数据 
                }  
            }
         }
         else
         {   
         	status = MI_ERR;   
         }
   }
   SetBitMask(ControlReg,0x80);           //停止定时器
   WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令 
   return status;
}
/*******************************************************************************************************


********************************************************************************************************/
int8 PcdComMF522Second(uint8 Command, 
                       uint8 *pInData, 
                       uint8 InLenByte,
                       uint8 *pOutData, 
                       uint16 *pOutLenBit)
{
    int8 status = MI_ERR;
    uint8 irqEn   = 0x00;
    uint8 waitFor = 0x00;
    uint8 lastBits;
    uint8 n;
    uint16 i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
	//WriteRawRC(ComIEnReg,0xA0);  //接收中断
    WriteRawRC(ComIEnReg,irqEn|0x80);  //引脚IRQ跟Status1Reg的IRq位相反，使能发送，接收，空闲，超时，错误中断，在IRQ脚输出低电平  
    ClearBitMask(ComIrqReg,0x80);      //清除所有使能IRQ(发送，接受，空闲，超时，错误)标志位
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);     //清除FIFO缓存读写指示器和缓存溢出标志在错误寄存器
    
    for (i=0; i<InLenByte; i++)
    {   
      WriteRawRC(FIFODataReg, pInData[i]);          //写进64字节的FIFO缓存区
    }  
    WriteRawRC(CommandReg, Command);                //启动发送和接收
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
        SetBitMask(BitFramingReg,0x80);  
    }      //开始传送
    
//  i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;  
    do 
    {
         //n = ReadRawRCSecond(ComIrqReg);
         n = ReadRawRC(ComIrqReg);  //查询是否有(发送，接收，空闲，超时，错误)中断
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,或者超时中断，空闲中断，接收中断则退出循环
    //while ((!(n&0x20)) && (!(n&0x03)));
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //停止传送
	      
    //if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
         {
             status = MI_OK;
             if (n & irqEn & 0x01)      //判断是否是超时中断
             {   
               status = MI_NOTAGERR;   
             }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);  //读取在FIFO缓存区的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
                if (lastBits)
                {   
                  *pOutLenBit = (n-1)*8 + lastBits;   
                }
                else
                {   
                  *pOutLenBit = n*8;   
                }
                if (n == 0)
                {   
                  n = 1;    
                }
                if (n > MAXRLEN)
                {   
                  n = MAXRLEN;   
                }
                //for (i=0; i<n; i++)
                //{   pOutData[i] = ReadRawRC(FIFODataReg);    }  //从FIFO缓存区读取n个字节数据
            }
         }
         else
         {   
           status = MI_ERR;   
         }

   }
   SetBitMask(ControlReg,0x80);           //停止定时器
   WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令 
   return status;
}
/*******************************************************************************************************

********************************************************************************************************/
void ReadBuffer(void)
{
	uint8 n;
    uint8 i;
	uint8 lastBits;
	uint8 length;
	uint8 snr_check=0;
	uint8 *OutData;
	uint8 *pSnr;
	
	 if(!(ReadRawRC(ErrorReg)&0x1B))                //判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
	 {
        n = ReadRawRC(FIFOLevelReg);                //读取在FIFO缓存区的字节数
        lastBits = ReadRawRC(ControlReg) & 0x07;    //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
        if (lastBits)
        {   
          length = (n-1)*8 + lastBits;   
        }
        else
        {   
          length = n*8;   
        }
        if (n == 0)
        {   
          n = 1;    
        }
        if (n > MAXRLEN)
        {   
          n = MAXRLEN;   
        }
        for (i=0; i<n; i++)
        {   
            OutData[i] = ReadRawRC(FIFODataReg);    //从FIFO缓存区读取n个字节数据
        }  

        for (i=0; i<4; i++)
        {   
            *(pSnr+i)  = OutData[i];
            snr_check ^= OutData[i];
        }
        if (snr_check != OutData[i])
        {   
            /*halUartWrite(pSnr,4);*/
        }
	 }
}
/*******************************************************************************************************

********************************************************************************************************/
void ReadDataFromBuf(void)
{
    uint8 lastBits;
    uint16 *pOutLenBit;
    uint8 n;
    uint16 i;
	uint8 *pOutData;
	uint8 *pSnr;
	uint8 snr_check;
	
    if(!(ReadRawRC(ErrorReg)&0x1B)) //判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
     {
         //n = ReadRawRC(ComIrqReg);ReciveFlag
         //if (!(n & 0x01))      //判断是否是超时中断
         if (!(ReciveFlag & 0x01))      //判断是否是超时中断
         {    
               	n = ReadRawRC(FIFOLevelReg);  //读取在FIFO缓存区的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
                if (lastBits)
                {   
                  *pOutLenBit = (n-1)*8 + lastBits;   
                }
                else
                {   
                  *pOutLenBit = n*8;   
                }
                if (n == 0)
                {   
                  n = 1;    
                }
                if (n > MAXRLEN)
                {   
                  n = MAXRLEN;   
                }
				
                for (i=0; i<n; i++)
                {   
                  pOutData[i] = ReadRawRC(FIFODataReg);    //从FIFO缓存区读取n个字节数据
                }  
                
		    	for (i=0; i<4; i++)
		        {   
		            *(pSnr+i)  = pOutData[i];
		            snr_check ^= pOutData[i];
		        }
		        if (snr_check != pOutData[i])
		        {   /*halUartWrite(pSnr,4);*/ }
				
		 }

     }
}
/*******************************************************************************************************


********************************************************************************************************/
void PcdComMF522Fourth(void)
{
    uint8 n;
    uint16 i;

    ClearBitMask(Status2Reg,0x08);   //不加密传输
    WriteRawRC(BitFramingReg,0x00);  //最后一个字节的全部位都被传送
    ClearBitMask(CollReg,0x80);      //如果有一个冲撞则全部接收的位都清0(此情况只在106kbit/s传输时的按位防冲撞中有效，其他情况则设为1)

    WriteRawRC(ComIEnReg,0xE7);  //引脚IRQ跟Status1Reg的IRq位相反，使能发送，接收，空闲，超时，错误中断，在IRQ脚输出低电平  
    ClearBitMask(ComIrqReg,0x80);      //清除所有使能IRQ(发送，接受，空闲，超时，错误)标志位
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);     //清除FIFO缓存读写指示器和缓存溢出标志在错误寄存器
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //启动发送和接收
    
    SetBitMask(BitFramingReg,0x80);           //开始传送
   
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;  
    do 
    {
         //n = ReadRawRC(ComIrqReg);  //查询是否有(发送，接收，空闲，超时，错误)中断
         ReciveFlag = ReadRawRC(ComIrqReg);  //查询是否有(发送，接收，空闲，超时，错误)中断
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,或者超时中断，空闲中断，接收中断则退出循环
    //while ((!(n&0x20)) && (!(n&0x03)));
	//while ((!(n&0x20)) && (i!=0));
	while ((!(ReciveFlag&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //停止传送

	ReadDataFromBuf();

    SetBitMask(ControlReg,0x80);           //停止定时器
    WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令 
    SetBitMask(CollReg,0x80);             //将按位防冲撞设置为1(只有106kbit/s需要按位防冲撞，其他的不需要按位防冲撞则设置为1)
}
/*******************************************************************************************************

********************************************************************************************************/
void PcdComMF522Fifth(void)
{
    uint8 lastBits;
    uint8 n;
    uint16 i;
	uint16 *pOutLenBit;
	uint8 *pOutData;
	uint8 *pSnr;
	uint8 snr_check;

    ClearBitMask(Status2Reg,0x08);   //不加密传输
    WriteRawRC(BitFramingReg,0x00);  //最后一个字节的全部位都被传送
    ClearBitMask(CollReg,0x80);      //如果有一个冲撞则全部接收的位都清0(此情况只在106kbit/s传输时的按位防冲撞中有效，其他情况则设为1)

    WriteRawRC(ComIEnReg,0xE7);  //引脚IRQ跟Status1Reg的IRq位相反，使能发送，接收，空闲，超时，错误中断，在IRQ脚输出低电平  
    ClearBitMask(ComIrqReg,0x80);      //清除所有使能IRQ(发送，接受，空闲，超时，错误)标志位
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);     //清除FIFO缓存读写指示器和缓存溢出标志在错误寄存器
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //启动发送和接收
    
    SetBitMask(BitFramingReg,0x80);           //开始传送
   
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  //查询是否有(发送，接收，空闲，超时，错误)中断
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,或者超时中断，空闲中断，接收中断则退出循环
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //停止传送

    //if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
         {
             if (!(n & 0x01))      //判断是否是超时中断
             {    
	               	n = ReadRawRC(FIFOLevelReg);  //读取在FIFO缓存区的字节数
	              	lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
	                if (lastBits)
	                {   
                      *pOutLenBit = (n-1)*8 + lastBits;   
                    }
	                else
	                {   
                      *pOutLenBit = n*8;   
                    }
	                if (n == 0)
	                {   
                      n = 1;    
                    }
	                if (n > MAXRLEN)
	                {   
                      n = MAXRLEN;   
                    }
					
	                for (i=0; i<n; i++)
	                {   pOutData[i] = ReadRawRC(FIFODataReg);    }  //从FIFO缓存区读取n个字节数据
	                
			    	for (i=0; i<4; i++)
			        {   
			            *(pSnr+i)  = pOutData[i];
			            snr_check ^= pOutData[i];
			        }
			        if (snr_check != pOutData[i])
			        {   /*halUartWrite(pSnr,4);*/ }
					
			 }

         }

   }
   SetBitMask(ControlReg,0x80);           //停止定时器
   WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令 
   SetBitMask(CollReg,0x80);             //将按位防冲撞设置为1(只有106kbit/s需要按位防冲撞，其他的不需要按位防冲撞则设置为1)
}
/*******************************************************************************************************

********************************************************************************************************/
void ReadDataFirst(void)
{
    uint8 n;
	uint8 m;
    uint16 i;
	uint8 lastBits;
	uint16 *pOutLenBit;
	uint8 *pOutData;
	uint8 pSnr[4];
	uint8 snr_check;
	
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  //查询是否有(发送，接收，空闲，超时，错误)中断
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,或者超时中断，空闲中断，接收中断则退出循环
	//while ((!(n&0x20)) && (i!=0) && (!(n&0x01)) && (!(n&0x10)));
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //停止传送

    if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
         {
             if (!(n & 0x01))      //判断是否是超时中断
             {    
	               	m= ReadRawRC(FIFOLevelReg);  //读取在FIFO缓存区的字节数
	              	lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
	                if (lastBits)
	                {   *pOutLenBit = (m-1)*8 + lastBits;   }
	                else
	                {   *pOutLenBit = m*8;   }
	                if (m== 0)
	                {   m = 1;    }
	                if (m > MAXRLEN)
	                {   m= MAXRLEN;   }

	                for (i=0; i<m; i++)
	                {   pOutData[i] = ReadRawRC(FIFODataReg);    }  //从FIFO缓存区读取n个字节数据
	                
			    	for (i=0; i<4; i++)
			        {   
			            pSnr[i]  = pOutData[i];
			            snr_check ^= pOutData[i];
			        }
			        //if (snr_check != pOutData[i])
			        if (snr_check == pOutData[i])
			        {  /* halUartWrite(pSnr,4);*/ }
					
					//halMcuWaitUs(10);  //延时10us
                    
					for (i=0; i<4; i++)   //清空
					{
                        pSnr[i] = 0;
					}
					for (i=0; i<MAXRLEN; i++)  //清空
					{
                        pOutData[i] = 0;
					}
			 }
         }
   }
   SetBitMask(ControlReg,0x80);           //停止定时器
   WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令 
   SetBitMask(CollReg,0x80);             //将按位防冲撞设置为1(只有106kbit/s需要按位防冲撞，其他的不需要按位防冲撞则设置为1)
}
/*******************************************************************************************************

********************************************************************************************************/
void PcdComMF522Sixth(void)
{
    ClearBitMask(Status2Reg,0x08);   //不加密传输
    WriteRawRC(BitFramingReg,0x00);  //最后一个字节的全部位都被传送
    ClearBitMask(CollReg,0x80);      //如果有一个冲撞则全部接收的位都清0(此情况只在106kbit/s传输时的按位防冲撞中有效，其他情况则设为1)

    WriteRawRC(ComIEnReg,0xE7);  //引脚IRQ跟Status1Reg的IRq位相反，使能发送，接收，空闲，超时，错误中断，在IRQ脚输出低电平  
    ClearBitMask(ComIrqReg,0x80);      //清除所有使能IRQ(发送，接受，空闲，超时，错误)标志位
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);     //清除FIFO缓存读写指示器和缓存溢出标志在错误寄存器
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //启动发送和接收
    SetBitMask(BitFramingReg,0x80);           //开始传送
    //ReadDataFirst();
   
}
/*******************************************************************************************************

********************************************************************************************************/
void PcdComMF522Seventh(void)
{

    uint8 n;
	uint8 m;
    uint16 i;
	uint8 lastBits;
	uint16 *pOutLenBit;
	uint8 *pOutData;
	uint8 pSnr[4];
	uint8 snr_check;

    ClearBitMask(Status2Reg,0x08);   //不加密传输
    WriteRawRC(BitFramingReg,0x00);  //最后一个字节的全部位都被传送
    ClearBitMask(CollReg,0x80);      //如果有一个冲撞则全部接收的位都清0(此情况只在106kbit/s传输时的按位防冲撞中有效，其他情况则设为1)

    WriteRawRC(ComIEnReg,0xE7);  //引脚IRQ跟Status1Reg的IRq位相反，使能发送，接收，空闲，超时，错误中断，在IRQ脚输出低电平  
    ClearBitMask(ComIrqReg,0x80);      //清除所有使能IRQ(发送，接受，空闲，超时，错误)标志位
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);     //清除FIFO缓存读写指示器和缓存溢出标志在错误寄存器
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //启动发送和接收
    SetBitMask(BitFramingReg,0x80);           //开始传送

//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  //查询是否有(发送，接收，空闲，超时，错误)中断
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,或者超时中断，空闲中断，接收中断则退出循环
	//while ((!(n&0x20)) && (i!=0) && (!(n&0x01)) && (!(n&0x10)));
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //停止传送

    if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //判断错误是否是FIFO缓存区溢出错误,奇偶校验错误,协议错误,检测到位碰撞错误(只在106kbit/s按位防碰撞中有效)
         {
             if (!(n & 0x01))      //判断是否是超时中断
             {    
	               	m= ReadRawRC(FIFOLevelReg);  //读取在FIFO缓存区的字节数
	              	lastBits = ReadRawRC(ControlReg) & 0x07; //最后接收字节的有效位的个数，如果为0则整个最后接收字节有效
	                if (lastBits)
	                {   *pOutLenBit = (m-1)*8 + lastBits;   }
	                else
	                {   *pOutLenBit = m*8;   }
	                if (m== 0)
	                {   m = 1;    }
	                if (m > MAXRLEN)
	                {   m= MAXRLEN;   }

	                for (i=0; i<m; i++)
	                {   pOutData[i] = ReadRawRC(FIFODataReg);    }  //从FIFO缓存区读取n个字节数据
	                
			    	for (i=0; i<4; i++)
			        {   
			            pSnr[i]  = pOutData[i];
			            snr_check ^= pOutData[i];
			        }
			        //if (snr_check != pOutData[i])
			        if (snr_check == pOutData[i])
			        {   /*halUartWrite(pSnr,4);*/ }
					
					//halMcuWaitUs(10);  //延时10us
                    
					for (i=0; i<4; i++)   //清空
					{
                        pSnr[i] = 0;
					}
					for (i=0; i<MAXRLEN; i++)  //清空
					{
                        pOutData[i] = 0;
					}
					
			 }

         }

   }
   SetBitMask(ControlReg,0x80);           //停止定时器
   WriteRawRC(CommandReg,PCD_IDLE);       //取消当前指令 
   SetBitMask(CollReg,0x80);              //将按位防冲撞设置为1(只有106kbit/s需要按位防冲撞，其他的不需要按位防冲撞则设置为1)
}
/*******************************************************************************************************
功    能：扣款和充值
参数说明: dd_mode[IN]：命令字
               0xC0 = 扣款
               0xC1 = 充值
          addr[IN]：钱包地址
          pValue[IN]：4字节增(减)值，低位在前
返    回: 成功返回MI_OK
********************************************************************************************************/              
int8 PcdValue(uint8 dd_mode,uint8 addr,uint8 *pValue)
{
    uint8 i; //添加
    int8 status;
    uint16 unLen;
    uint8 ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pValue, 4);
        for(i=0;i<4;i++)  //添加修改
        {
            ucComMF522Buf[i] = pValue[i];    
        }
 //       for (i=0; i<16; i++)
 //       {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}
/*******************************************************************************************************
功    能：备份钱包
参数说明: sourceaddr[IN]：源地址
          goaladdr[IN]：目标地址
返    回: 成功返回MI_OK
********************************************************************************************************/   
int8 PcdBakValue(uint8 sourceaddr, uint8 goaladdr)
{
    int8 status;
    uint16 unLen;
    uint8 ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}
/*******************************************************************************************************


********************************************************************************************************/  

