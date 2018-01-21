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
    //ģ��SPI
    P1SEL &= ~0xE0; //P1.5,P1.6,P1.7
    P1DIR |= 0x60;
    P1DIR &= ~0x80;
#endif
    
    P0SEL &= ~0xC0;  //P0.6,P0.7
    P0DIR |= 0xC0;
    
    P2SEL &= ~0x01;  /**< P2.0ͨ��I/O */
    P2DIR &= ~0x01;  /**< P2.0���� */
    P2INP &= ~0X81;  /**< P2.0 ���� ֧��������  */  
  
  
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
��    �ܣ���RC632�Ĵ���
����˵����Address[IN]:�Ĵ�����ַ
��    �أ�������ֵ
********************************************************************************************************/
uint8 ReadRawRC(uint8 Address)
{ 
    uint8 ucAddr;
    uint8 ucResult=0;
    ucAddr = (((Address<<1)&0x7E)|0x80);
    
#ifdef H_SPI
    
    //Ӳ��SPI
    MF522_NSS = 0;
    SPI_WriteReadData(ucAddr);
    ucResult = SPI_WriteReadData(0x00);
    MF522_NSS = 1;
    
#else
     //ģ��SPI
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
         bit_data.firstbit = MF522_SO;  //����޸�
         ucResult|= bit_data.firstbit;  //����޸�
         MF522_SCK = 0;
     }
     MF522_NSS = 1;
     MF522_SCK = 1;
     
#endif
     return ucResult;
}
/*******************************************************************************************************
��    �ܣ�дRC632�Ĵ���
����˵����Address[IN]:�Ĵ�����ַ
          value[IN]:д���ֵ
********************************************************************************************************/
void WriteRawRC(uint8 Address, uint8 value)
{  
    uint8 ucAddr;
    ucAddr = ((Address<<1)&0x7E);

#ifdef H_SPI
    
    //Ӳ��SPI
    MF522_NSS = 0;
    SPI_WriteReadData(ucAddr);
    SPI_WriteReadData(value);
    MF522_NSS = 1;
    
#else
    
    //ģ��SPI
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
��    �ܣ���λRC522
��    ��: �ɹ�����MI_OK
********************************************************************************************************/
int8 PcdReset(void)
{
    MF522_RST=1;
    halMcuWaitUs(100);
    MF522_RST=0;
    halMcuWaitUs(100);
    MF522_RST=1;
    halMcuWaitUs(100);
    WriteRawRC(CommandReg,PCD_RESETPHASE); //�����λRC522
    halMcuWaitUs(200);
    
    WriteRawRC(ModeReg,0x3D);            	//��Mifare��ͨѶ��CRC��ʼֵ0x6363,���俪ʼ
    /*
    WriteRawRC(TReloadRegL,0xFF);     		//Ϊ16λ��ʱ�����ؼ�����ֵ(���ֵ65535)48      
    WriteRawRC(TReloadRegH,0xFF);
    WriteRawRC(TModeReg,0x8F);           	//��ʱ���Զ����У���ʱ�����Զ����أ���ʱ���ӳ�ʼֵ����ʱ��0�󴥷�һ����ʱ�ж�,f(timerƵ��)=6.78MHz/TPrescaler
    //WriteRawRC(TModeReg,0x9F);
	WriteRawRC(RxModeReg,0x04);
    WriteRawRC(TPrescalerReg,0xFF);      	//��ʱ��ʱ��Ƶ��6.78MHz��27.12MHz��Ƶ������12λԤ��������ֵ(���ֵ4095)�����ʱ��=65535*4095/6.78MHz=39.59s��TPrescaler=0xD3E=3390,��ʱ����ʱ��ΪT=48*3390/6.78MHz=24ms
    */
    
    
    /*
    WriteRawRC(TReloadRegL,30);          	//Ϊ16λ��ʱ�����ؼ�����ֵ(���ֵ65535)48      
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);           	//��ʱ���Զ����У���ʱ�����Զ����أ���ʱ���ӳ�ʼֵ����ʱ��0�󴥷�һ����ʱ�ж�,f(timerƵ��)=6.78MHz/TPrescaler
    WriteRawRC(TPrescalerReg,0x3E);      	//��ʱ��ʱ��Ƶ��6.78MHz��27.12MHz��Ƶ������12λԤ��������ֵ(���ֵ4095)�����ʱ��=65535*4095/6.78MHz=39.59s��TPrescaler=0xD3E=3390,��ʱ����ʱ��ΪT=48*3390/6.78MHz=24ms
    */
    
    
    WriteRawRC(TxModeReg,0x0);
    WriteRawRC(RxModeReg,0x0);
    //WriteRawRC(TxControlReg,0x80);
    //WriteRawRC(TxSelReg,0x10);

    
    WriteRawRC(TxAutoReg,0x40);          	//100%ASK����
    
    return MI_OK;
}
/*******************************************************************************************************
��������  
ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
********************************************************************************************************/
void PcdAntennaOn(void)
{
    uint8 i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);  //ʹ������TX1��TX2,����13.56MHz�ĵ����ز���������
    }
    halMcuWaitMs(1);
}
/*******************************************************************************************************
�ر�����
********************************************************************************************************/
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}
/*******************************************************************************************************
��������͹��� 
********************************************************************************************************/
void PcdPowerDown(void)
{
    //WriteRawRC(CommandReg,0x10);
    SetBitMask(CommandReg,0x10);
}
/*******************************************************************************************************
����
********************************************************************************************************/
void PcdWakeUp(void)
{
    SetBitMask(CommandReg,0x00);
    halMcuWaitMs(1);
}
/*******************************************************************************************************
��    �ܣ�Ѱ��
����˵��: req_code[IN]:Ѱ����ʽ
                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
                0x26 = Ѱδ��������״̬�Ŀ�
          pTagType[OUT]����Ƭ���ʹ���
                0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
��    ��: �ɹ�����MI_OK
********************************************************************************************************/
int8 PcdRequest(uint8 req_code,uint8 *pTagType)
{
   int8 status;  
   uint16 unLen;
   uint8 ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08);       //�����ܴ���
   WriteRawRC(BitFramingReg,0x07);      //�������һ���ֽڵĶ���λҪ������(���Ϊ000b��ô���һ���ֽڵ�ȫ��λ��������)��7λ������

   SetBitMask(TxControlReg,0x03);       //ʹ������TX1��TX2,����13.56MHz�ĵ����ز���������
   
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
��    �ܣ�����ײ
����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
��    ��: �ɹ�����MI_OK
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
	ClearBitMask(Status2Reg,0x08);  //�����ܴ���
	WriteRawRC(BitFramingReg,0x07);  //�������һ���ֽڵĶ���λҪ������(���Ϊ000b��ô���һ���ֽڵ�ȫ��λ��������)��7λ������
	SetBitMask(TxControlReg,0x03);  //ʹ������TX1��TX2,����13.56MHz�ĵ����ز���������

	WriteRawRC(FIFODataReg, 0x52);         //д��64�ֽڵ�FIFO������
	WriteRawRC(CommandReg, 0x04);          //��������
	//WriteRawRC(CommandReg, 0x0C);          //��������
	SetBitMask(BitFramingReg,0x80);        //��ʼ����
	SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
	WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ��
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
    
    ClearBitMask(Status2Reg,0x08);   //�����ܴ���
    WriteRawRC(BitFramingReg,0x00);  //���һ���ֽڵ�ȫ��λ��������
    ClearBitMask(CollReg,0x80);      //�����һ����ײ��ȫ�����յ�λ����0(�����ֻ��106kbit/s����ʱ�İ�λ����ײ����Ч�������������Ϊ1)
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;
    //status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);    //ԭʼ����
	status = PcdComMF522Second(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);  //�޸ĵĴ���

    if (status == MI_OK)
    {
         //�޸ĵĴ���
         for (i=0; i<MAXRLEN; i++)
         {   
           ReciveData[i] = ReadRawRC(FIFODataReg);    //��FIFO��������ȡn���ֽ�����
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
         //ԭʼ����
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
		 */
    }
    SetBitMask(CollReg,0x80); //����λ����ײ����Ϊ1(ֻ��106kbit/s��Ҫ��λ����ײ�������Ĳ���Ҫ��λ����ײ������Ϊ1)
    return status;
}
/*******************************************************************************************************
��    �ܣ�ѡ����Ƭ
����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
��    ��: �ɹ�����MI_OK
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
��    �ܣ���֤��Ƭ����
����˵��: auth_mode[IN]: ������֤ģʽ
                 0x60 = ��֤A��Կ
                 0x61 = ��֤B��Կ 
          addr[IN]�����ַ
          pKey[IN]������
          pSnr[IN]����Ƭ���кţ�4�ֽ�
��    ��: �ɹ�����MI_OK
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
��    �ܣ���ȡM1��һ������
����˵��: addr[IN]�����ַ
          pData[OUT]�����������ݣ�16�ֽ�
��    ��: �ɹ�����MI_OK
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
��    �ܣ�д���ݵ�M1��һ��
����˵��: addr[IN]�����ַ
          pData[IN]��д������ݣ�16�ֽ�
��    ��: �ɹ�����MI_OK
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
��    �ܣ����Ƭ��������״̬
��    ��: �ɹ�����MI_OK
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
��MF522����CRC16����
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
����RC632�Ĺ�����ʽ 
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
��    �ܣ���RC522�Ĵ���λ
����˵����reg[IN]:�Ĵ�����ַ
          mask[IN]:��λֵ
********************************************************************************************************/
void SetBitMask(uint8 reg,uint8 mask)  
{
    int8 tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}
/*******************************************************************************************************
��    �ܣ���RC522�Ĵ���λ
����˵����reg[IN]:�Ĵ�����ַ
          mask[IN]:��λֵ
********************************************************************************************************/
void ClearBitMask(uint8 reg,uint8 mask)  
{
    int8 tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 
/*******************************************************************************************************
��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
����˵����Command[IN]:RC522������
          pInData[IN]:ͨ��RC522���͵���Ƭ������
          InLenByte[IN]:�������ݵ��ֽڳ���
          pOutData[OUT]:���յ��Ŀ�Ƭ��������
          *pOutLenBit[OUT]:�������ݵ�λ����
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
    WriteRawRC(ComIEnReg,irqEn|0x80);  //����IRQ��Status1Reg��IRqλ�෴��ʹ�ܷ��ͣ����ܣ����У���ʱ�������жϣ���IRQ������͵�ƽ  
    ClearBitMask(ComIrqReg,0x80);      //�������ʹ��IRQ(���ͣ����ܣ����У���ʱ������)��־λ
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);     //���FIFO�����дָʾ���ͻ��������־�ڴ���Ĵ���
    
    for (i=0; i<InLenByte; i++)
    {   
    	WriteRawRC(FIFODataReg, pInData[i]);  		//д��64�ֽڵ�FIFO������  
    }  
    WriteRawRC(CommandReg, Command);               	//�������ͺͽ���
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
    	SetBitMask(BitFramingReg,0x80);  			//��ʼ����		
   	}      	
    
	//i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
	i = 300;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  				//��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         i--;  
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));  	//i=0,���߳�ʱ�жϣ������жϣ������ж����˳�ѭ��
    
    ClearBitMask(BitFramingReg,0x80);   			//ֹͣ����
	      
    if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) 			//�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
         {
             status = MI_OK;
             if (n & irqEn & 0x01)      			//�ж��Ƿ��ǳ�ʱ�ж�
             {   
             	status = MI_NOTAGERR;   
             }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);  		//��ȡ��FIFO���������ֽ���
              	lastBits = ReadRawRC(ControlReg) & 0x07; //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
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
                	pOutData[i] = ReadRawRC(FIFODataReg);   //��FIFO��������ȡn���ֽ����� 
                }  
            }
         }
         else
         {   
         	status = MI_ERR;   
         }
   }
   SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
   WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ�� 
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
	//WriteRawRC(ComIEnReg,0xA0);  //�����ж�
    WriteRawRC(ComIEnReg,irqEn|0x80);  //����IRQ��Status1Reg��IRqλ�෴��ʹ�ܷ��ͣ����գ����У���ʱ�������жϣ���IRQ������͵�ƽ  
    ClearBitMask(ComIrqReg,0x80);      //�������ʹ��IRQ(���ͣ����ܣ����У���ʱ������)��־λ
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);     //���FIFO�����дָʾ���ͻ��������־�ڴ���Ĵ���
    
    for (i=0; i<InLenByte; i++)
    {   
      WriteRawRC(FIFODataReg, pInData[i]);          //д��64�ֽڵ�FIFO������
    }  
    WriteRawRC(CommandReg, Command);                //�������ͺͽ���
   
    
    if (Command == PCD_TRANSCEIVE)
    {    
        SetBitMask(BitFramingReg,0x80);  
    }      //��ʼ����
    
//  i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    i = 2000;  
    do 
    {
         //n = ReadRawRCSecond(ComIrqReg);
         n = ReadRawRC(ComIrqReg);  //��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,���߳�ʱ�жϣ������жϣ������ж����˳�ѭ��
    //while ((!(n&0x20)) && (!(n&0x03)));
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //ֹͣ����
	      
    //if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
         {
             status = MI_OK;
             if (n & irqEn & 0x01)      //�ж��Ƿ��ǳ�ʱ�ж�
             {   
               status = MI_NOTAGERR;   
             }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);  //��ȡ��FIFO���������ֽ���
              	lastBits = ReadRawRC(ControlReg) & 0x07; //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
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
                //{   pOutData[i] = ReadRawRC(FIFODataReg);    }  //��FIFO��������ȡn���ֽ�����
            }
         }
         else
         {   
           status = MI_ERR;   
         }

   }
   SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
   WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ�� 
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
	
	 if(!(ReadRawRC(ErrorReg)&0x1B))                //�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
	 {
        n = ReadRawRC(FIFOLevelReg);                //��ȡ��FIFO���������ֽ���
        lastBits = ReadRawRC(ControlReg) & 0x07;    //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
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
            OutData[i] = ReadRawRC(FIFODataReg);    //��FIFO��������ȡn���ֽ�����
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
	
    if(!(ReadRawRC(ErrorReg)&0x1B)) //�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
     {
         //n = ReadRawRC(ComIrqReg);ReciveFlag
         //if (!(n & 0x01))      //�ж��Ƿ��ǳ�ʱ�ж�
         if (!(ReciveFlag & 0x01))      //�ж��Ƿ��ǳ�ʱ�ж�
         {    
               	n = ReadRawRC(FIFOLevelReg);  //��ȡ��FIFO���������ֽ���
              	lastBits = ReadRawRC(ControlReg) & 0x07; //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
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
                  pOutData[i] = ReadRawRC(FIFODataReg);    //��FIFO��������ȡn���ֽ�����
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

    ClearBitMask(Status2Reg,0x08);   //�����ܴ���
    WriteRawRC(BitFramingReg,0x00);  //���һ���ֽڵ�ȫ��λ��������
    ClearBitMask(CollReg,0x80);      //�����һ����ײ��ȫ�����յ�λ����0(�����ֻ��106kbit/s����ʱ�İ�λ����ײ����Ч�������������Ϊ1)

    WriteRawRC(ComIEnReg,0xE7);  //����IRQ��Status1Reg��IRqλ�෴��ʹ�ܷ��ͣ����գ����У���ʱ�������жϣ���IRQ������͵�ƽ  
    ClearBitMask(ComIrqReg,0x80);      //�������ʹ��IRQ(���ͣ����ܣ����У���ʱ������)��־λ
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);     //���FIFO�����дָʾ���ͻ��������־�ڴ���Ĵ���
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //�������ͺͽ���
    
    SetBitMask(BitFramingReg,0x80);           //��ʼ����
   
//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    i = 2000;  
    do 
    {
         //n = ReadRawRC(ComIrqReg);  //��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         ReciveFlag = ReadRawRC(ComIrqReg);  //��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,���߳�ʱ�жϣ������жϣ������ж����˳�ѭ��
    //while ((!(n&0x20)) && (!(n&0x03)));
	//while ((!(n&0x20)) && (i!=0));
	while ((!(ReciveFlag&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //ֹͣ����

	ReadDataFromBuf();

    SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
    WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ�� 
    SetBitMask(CollReg,0x80);             //����λ����ײ����Ϊ1(ֻ��106kbit/s��Ҫ��λ����ײ�������Ĳ���Ҫ��λ����ײ������Ϊ1)
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

    ClearBitMask(Status2Reg,0x08);   //�����ܴ���
    WriteRawRC(BitFramingReg,0x00);  //���һ���ֽڵ�ȫ��λ��������
    ClearBitMask(CollReg,0x80);      //�����һ����ײ��ȫ�����յ�λ����0(�����ֻ��106kbit/s����ʱ�İ�λ����ײ����Ч�������������Ϊ1)

    WriteRawRC(ComIEnReg,0xE7);  //����IRQ��Status1Reg��IRqλ�෴��ʹ�ܷ��ͣ����գ����У���ʱ�������жϣ���IRQ������͵�ƽ  
    ClearBitMask(ComIrqReg,0x80);      //�������ʹ��IRQ(���ͣ����ܣ����У���ʱ������)��־λ
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);     //���FIFO�����дָʾ���ͻ��������־�ڴ���Ĵ���
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //�������ͺͽ���
    
    SetBitMask(BitFramingReg,0x80);           //��ʼ����
   
//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    i = 2000;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  //��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,���߳�ʱ�жϣ������жϣ������ж����˳�ѭ��
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //ֹͣ����

    //if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
         {
             if (!(n & 0x01))      //�ж��Ƿ��ǳ�ʱ�ж�
             {    
	               	n = ReadRawRC(FIFOLevelReg);  //��ȡ��FIFO���������ֽ���
	              	lastBits = ReadRawRC(ControlReg) & 0x07; //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
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
	                {   pOutData[i] = ReadRawRC(FIFODataReg);    }  //��FIFO��������ȡn���ֽ�����
	                
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
   SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
   WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ�� 
   SetBitMask(CollReg,0x80);             //����λ����ײ����Ϊ1(ֻ��106kbit/s��Ҫ��λ����ײ�������Ĳ���Ҫ��λ����ײ������Ϊ1)
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
	
//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    i = 2000;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  //��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,���߳�ʱ�жϣ������жϣ������ж����˳�ѭ��
	//while ((!(n&0x20)) && (i!=0) && (!(n&0x01)) && (!(n&0x10)));
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //ֹͣ����

    if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
         {
             if (!(n & 0x01))      //�ж��Ƿ��ǳ�ʱ�ж�
             {    
	               	m= ReadRawRC(FIFOLevelReg);  //��ȡ��FIFO���������ֽ���
	              	lastBits = ReadRawRC(ControlReg) & 0x07; //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
	                if (lastBits)
	                {   *pOutLenBit = (m-1)*8 + lastBits;   }
	                else
	                {   *pOutLenBit = m*8;   }
	                if (m== 0)
	                {   m = 1;    }
	                if (m > MAXRLEN)
	                {   m= MAXRLEN;   }

	                for (i=0; i<m; i++)
	                {   pOutData[i] = ReadRawRC(FIFODataReg);    }  //��FIFO��������ȡn���ֽ�����
	                
			    	for (i=0; i<4; i++)
			        {   
			            pSnr[i]  = pOutData[i];
			            snr_check ^= pOutData[i];
			        }
			        //if (snr_check != pOutData[i])
			        if (snr_check == pOutData[i])
			        {  /* halUartWrite(pSnr,4);*/ }
					
					//halMcuWaitUs(10);  //��ʱ10us
                    
					for (i=0; i<4; i++)   //���
					{
                        pSnr[i] = 0;
					}
					for (i=0; i<MAXRLEN; i++)  //���
					{
                        pOutData[i] = 0;
					}
			 }
         }
   }
   SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
   WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ�� 
   SetBitMask(CollReg,0x80);             //����λ����ײ����Ϊ1(ֻ��106kbit/s��Ҫ��λ����ײ�������Ĳ���Ҫ��λ����ײ������Ϊ1)
}
/*******************************************************************************************************

********************************************************************************************************/
void PcdComMF522Sixth(void)
{
    ClearBitMask(Status2Reg,0x08);   //�����ܴ���
    WriteRawRC(BitFramingReg,0x00);  //���һ���ֽڵ�ȫ��λ��������
    ClearBitMask(CollReg,0x80);      //�����һ����ײ��ȫ�����յ�λ����0(�����ֻ��106kbit/s����ʱ�İ�λ����ײ����Ч�������������Ϊ1)

    WriteRawRC(ComIEnReg,0xE7);  //����IRQ��Status1Reg��IRqλ�෴��ʹ�ܷ��ͣ����գ����У���ʱ�������жϣ���IRQ������͵�ƽ  
    ClearBitMask(ComIrqReg,0x80);      //�������ʹ��IRQ(���ͣ����ܣ����У���ʱ������)��־λ
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);     //���FIFO�����дָʾ���ͻ��������־�ڴ���Ĵ���
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //�������ͺͽ���
    SetBitMask(BitFramingReg,0x80);           //��ʼ����
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

    ClearBitMask(Status2Reg,0x08);   //�����ܴ���
    WriteRawRC(BitFramingReg,0x00);  //���һ���ֽڵ�ȫ��λ��������
    ClearBitMask(CollReg,0x80);      //�����һ����ײ��ȫ�����յ�λ����0(�����ֻ��106kbit/s����ʱ�İ�λ����ײ����Ч�������������Ϊ1)

    WriteRawRC(ComIEnReg,0xE7);  //����IRQ��Status1Reg��IRqλ�෴��ʹ�ܷ��ͣ����գ����У���ʱ�������жϣ���IRQ������͵�ƽ  
    ClearBitMask(ComIrqReg,0x80);      //�������ʹ��IRQ(���ͣ����ܣ����У���ʱ������)��־λ
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);     //���FIFO�����дָʾ���ͻ��������־�ڴ���Ĵ���
    
    WriteRawRC(FIFODataReg, 0x93);
	WriteRawRC(FIFODataReg, 0x20);
	
    WriteRawRC(CommandReg, PCD_TRANSCEIVE);   //�������ͺͽ���
    SetBitMask(BitFramingReg,0x80);           //��ʼ����

//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    i = 2000;  
    do 
    {
         n = ReadRawRC(ComIrqReg);  //��ѯ�Ƿ���(���ͣ����գ����У���ʱ������)�ж�
         i--;  
    }
    //while ((i!=0) && !(n&0x01) && !(n&waitFor));  //i=0,���߳�ʱ�жϣ������жϣ������ж����˳�ѭ��
	//while ((!(n&0x20)) && (i!=0) && (!(n&0x01)) && (!(n&0x10)));
	while ((!(n&0x20)) && (i!=0));
    ClearBitMask(BitFramingReg,0x80);   //ֹͣ����

    if (i!=0)  
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B)) //�жϴ����Ƿ���FIFO�������������,��żУ�����,Э�����,��⵽λ��ײ����(ֻ��106kbit/s��λ����ײ����Ч)
         {
             if (!(n & 0x01))      //�ж��Ƿ��ǳ�ʱ�ж�
             {    
	               	m= ReadRawRC(FIFOLevelReg);  //��ȡ��FIFO���������ֽ���
	              	lastBits = ReadRawRC(ControlReg) & 0x07; //�������ֽڵ���Чλ�ĸ��������Ϊ0�������������ֽ���Ч
	                if (lastBits)
	                {   *pOutLenBit = (m-1)*8 + lastBits;   }
	                else
	                {   *pOutLenBit = m*8;   }
	                if (m== 0)
	                {   m = 1;    }
	                if (m > MAXRLEN)
	                {   m= MAXRLEN;   }

	                for (i=0; i<m; i++)
	                {   pOutData[i] = ReadRawRC(FIFODataReg);    }  //��FIFO��������ȡn���ֽ�����
	                
			    	for (i=0; i<4; i++)
			        {   
			            pSnr[i]  = pOutData[i];
			            snr_check ^= pOutData[i];
			        }
			        //if (snr_check != pOutData[i])
			        if (snr_check == pOutData[i])
			        {   /*halUartWrite(pSnr,4);*/ }
					
					//halMcuWaitUs(10);  //��ʱ10us
                    
					for (i=0; i<4; i++)   //���
					{
                        pSnr[i] = 0;
					}
					for (i=0; i<MAXRLEN; i++)  //���
					{
                        pOutData[i] = 0;
					}
					
			 }

         }

   }
   SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
   WriteRawRC(CommandReg,PCD_IDLE);       //ȡ����ǰָ�� 
   SetBitMask(CollReg,0x80);              //����λ����ײ����Ϊ1(ֻ��106kbit/s��Ҫ��λ����ײ�������Ĳ���Ҫ��λ����ײ������Ϊ1)
}
/*******************************************************************************************************
��    �ܣ��ۿ�ͳ�ֵ
����˵��: dd_mode[IN]��������
               0xC0 = �ۿ�
               0xC1 = ��ֵ
          addr[IN]��Ǯ����ַ
          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
��    ��: �ɹ�����MI_OK
********************************************************************************************************/              
int8 PcdValue(uint8 dd_mode,uint8 addr,uint8 *pValue)
{
    uint8 i; //���
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
        for(i=0;i<4;i++)  //����޸�
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
��    �ܣ�����Ǯ��
����˵��: sourceaddr[IN]��Դ��ַ
          goaladdr[IN]��Ŀ���ַ
��    ��: �ɹ�����MI_OK
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

