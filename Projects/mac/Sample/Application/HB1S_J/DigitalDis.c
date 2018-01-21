/********************************************************************************************************

********************************************************************************************************/
#include "msa.h"
#include "DigitalDis.h"
/********************************************************************************************************

********************************************************************************************************/
#define SN_DATA     P1_2           //P1.2
#define SN_CLK      P2_0           //P2.0
#define SEG_COM0    P1_3           //P1.3
#define SEG_COM1    P1_5           //P1.5
#define SEG_COM2    P1_6           //P1.6
#define SEG_COM3    P1_7           //P1.7
/********************************************************************************************************

********************************************************************************************************/
/**< 8,7,6,5,4,3,2,1 = A,F,B,G,E,D,H,C */
//�߶�������                0     1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   ȫ��
uint8   SN74LS164SegTab[] = {0x12,0xDE,0x43,0x4A,0x8E,0x2A,0x22,0x5E,0x02,0x0A,0x06,0xA2,0x33,0xC2,0x23,0x27,0xFF};
uint8   SN74LS164SegSelect = 0;     //�߶���Ƭѡѡ���ź�,1ѡ���λ,2ѡ��ʮλ,3ѡ���λ,4ѡ��ǧλ
uint16  SN74LS164SegNum = 0;        //�߶���Ҫ��ʾ������
/********************************************************************************************************

********************************************************************************************************/
void SN74LS164SegInit(void)
{
  /*
    Seg
    SnData P1_2
    SnClk  P2_0
    
    SegCom1     P1_3
    SegCom2     P1_5
    SegCom3     P1_6
    SegCom4     P1_7
  */
  
#define SN_DATA_PIN     2           //P1.2
#define SN_CLK_PIN      0           //P2.0
#define SEG_COM0_PIN    3           //P1.3
#define SEG_COM1_PIN    5           //P1.5
#define SEG_COM2_PIN    6           //P1.6
#define SEG_COM3_PIN    7           //P1.7
    IO_FUNC_PORT_PIN(PORT1,SN_DATA_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT1,SN_DATA_PIN,IO_OUT);

    IO_FUNC_PORT_PIN(PORT2,SN_CLK_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT2,SN_CLK_PIN,IO_OUT);

    IO_FUNC_PORT_PIN(PORT1,SEG_COM0_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT1,SEG_COM0_PIN,IO_OUT);
    
    IO_FUNC_PORT_PIN(PORT1,SEG_COM1_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT1,SEG_COM1_PIN,IO_OUT);

    IO_FUNC_PORT_PIN(PORT1,SEG_COM2_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT1,SEG_COM2_PIN,IO_OUT);
    
    IO_FUNC_PORT_PIN(PORT1,SEG_COM3_PIN,IO_GIO);
    IO_DIR_PORT_PIN(PORT1,SEG_COM3_PIN,IO_OUT);

  
    //�߶����ʼ��:�ر�4λ�߶������ʾ
    SEG_COM0 = 0;
    SEG_COM1 = 0;
    SEG_COM2 = 0;
    SEG_COM3 = 0;
    
    //SN74LS164��ʼ��:Q0~Q7����ʼ��Ϊ1
    SN74LS164SendData(0xFF);
    
    SN_CLK = 0;     //�ָ�ʱ���ź�Ϊ��
}
/********************************************************************************************************

********************************************************************************************************/
void SN74LS164SendData(uint8 data)
{
    uint8 i = 0;
    
    //��8λ������λ�͵�SN74LS164��Q0~Q7
    for(i=0; i<8; i++)
    {
        SN_CLK = 0;
        if(data&0x80)
        {
            SN_DATA = 1;
        }
        else
        {
            SN_DATA = 0;
        }
        data <<=1;
        SN_CLK = 1;
    }
    
    SN_CLK = 0;       //�ָ�ʱ���ź�Ϊ��
}
/********************************************************************************************************

********************************************************************************************************/
void SN74LS164SegDisplay(void)
{
    //��Ƭѡ
    SEG_COM0 = 0;
    SEG_COM1 = 0;
    SEG_COM2 = 0;
    SEG_COM3 = 0;
    
    SN74LS164SendData(DisData[SN74LS164SegSelect]);//�Ͷ���,ʮ������ʾ
    
    if(SN74LS164SegSelect == 0)
    {
        SEG_COM3 = 1;
    }
    else if(SN74LS164SegSelect == 1)
    {
        SEG_COM2 = 1;
    }
    else if(SN74LS164SegSelect == 2)
    {
        SEG_COM1 = 1;
    }
    else
    {
        SEG_COM0 = 1;
    }
    
    SN74LS164SegSelect++;
    if(SN74LS164SegSelect > 3)
    {
        SN74LS164SegSelect = 0;
    }
}
/********************************************************************************************************

********************************************************************************************************/

