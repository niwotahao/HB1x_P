#ifndef MFRC522_H
#define MFRC522_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************
* INCLUDES
*/
#include "hal_types.h"

  
  
//MFRC522
#define MF522_RST   P0_6                    //RC500Ƭѡ
#define MF522_NSS   P0_7 
  
#define MF522_SCK   P1_5  
#define MF522_SI    P1_6   
#define MF522_SO    P1_7  
  
#define PcbEnterSleep()     (MF522_RST = 0)
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////
//����ԭ��
/////////////////////////////////////////////////////////////////////
void SPI_Init(void);
uint8 SPI_WriteReadData(uint8 Data);

int8 PcdReset(void);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
void PcdWakeUp(void);
void PcdPowerDown(void);



int8 PcdRequest(uint8 req_code,uint8 *pTagType);
int8 PcdAnticollSecond(uint8 *pSnr);
int8 PcdAnticoll(uint8 *pSnr);
int8 PcdSelect(uint8 *pSnr);         
int8 PcdAuthState(uint8 auth_mode,uint8 addr,uint8 *pKey,uint8 *pSnr);     
int8 PcdRead(uint8 addr,uint8 *pData);     
int8 PcdWrite(uint8 addr,uint8 *pData);    
int8 PcdValue(uint8 dd_mode,uint8 addr,uint8 *pValue);   
int8 PcdBakValue(uint8 sourceaddr, uint8 goaladdr);                                 
int8 PcdHalt(void);
int8 PcdComMF522(uint8 Command, 
                 uint8 *pInData, 
                 uint8 InLenByte,
                 uint8 *pOutData, 
                 uint16  *pOutLenBit);
void CalulateCRC(uint8 *pIndata,uint8 len,uint8 *pOutData);
void WriteRawRC(uint8 Address,uint8 value);
uint8 ReadRawRC(uint8 Address); 
void SetBitMask(uint8 reg,uint8 mask); 
void ClearBitMask(uint8 reg,uint8 mask); 
int8 M500PcdConfigISOType(uint8 type);
//void delay_10ms(uint16 _10ms);
void iccardcode();
int8 PcdBakValue(uint8 sourceaddr, uint8 goaladdr);
int8 PcdValue(uint8 dd_mode,uint8 addr,uint8 *pValue);

void PcdRequest2(void);
int8 PcdComMF522Second(uint8 Command, 
                       uint8 *pInData, 
                       uint8 InLenByte,
                       uint8 *pOutData, 
                       uint16 *pOutLenBit);

void ReadDataFromBuf();
void PcdComMF522Fourth();
void PcdComMF522Fifth();
void ReadDataFirst();
void PcdComMF522Sixth();
void PcdComMF522Seventh();
/*******************************************************************************************************/
typedef struct bs     //����λ��
{
    uint8 firstbit:1;    //��һλ
}bit_data_t;
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_OK                          0
#define MI_NOTAGERR                    (-1)
#define MI_ERR                         (-2)


#ifdef  __cplusplus
}
#endif

/**********************************************************************************/
#endif
