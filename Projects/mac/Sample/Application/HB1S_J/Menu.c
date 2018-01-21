/********************************************************************************************************

********************************************************************************************************/

#include "Menu.h"
/********************************************************************************************************

********************************************************************************************************/
uint8 DisData[4];
/********************************************************************************************************

********************************************************************************************************/
void MENU_Init(void)
{
    DisData[0] = (SN74LS164SegNum%10000)/1000;
    DisData[1] = (SN74LS164SegNum%1000)/100;
    DisData[2] = (SN74LS164SegNum%100)/10;
    DisData[3] = (SN74LS164SegNum%10);
    
    DisData[0] = SN74LS164SegTab[DisData[0]];
    DisData[1] = SN74LS164SegTab[DisData[1]];
    DisData[2] = SN74LS164SegTab[DisData[2]];
    DisData[3] = SN74LS164SegTab[DisData[3]];
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisCountValue(void)
{
    DisData[0] = (SN74LS164SegNum%10000)/1000;
    DisData[1] = (SN74LS164SegNum%1000)/100;
    DisData[2] = (SN74LS164SegNum%100)/10;
    DisData[3] = (SN74LS164SegNum%10);
    if(DisData[0] == 0x00)
    {
        DisData[0] = 16;
        if(DisData[1] == 0x00)
        {
            DisData[1] = 16;
            if(DisData[2] == 0x00)
            {
                DisData[2] = 16;
            }
        }
    }
    DisData[0] = SN74LS164SegTab[DisData[0]];
    DisData[1] = SN74LS164SegTab[DisData[1]];
    DisData[2] = SN74LS164SegTab[DisData[2]];
    DisData[3] = SN74LS164SegTab[DisData[3]];
    if(StoreBspFlg.AutoManual == 0)
    {
        DisData[0] &= 0xFD;
        DisData[1] &= 0xFD;
        DisData[2] &= 0xFD;
        DisData[3] &= 0xFD;
    }
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisAutoManual(void)
{
    DisData[0] = 0x6B; //"=-"
    DisData[1] = 0xEF; //"-"

    if(StoreBspFlg.AutoManual == 0)
    {
        DisData[2] = 0xFF; //" "
        DisData[3] = 0x06; //"A"
    }
    else
    {   
        DisData[2] = 0x1F; //"n"
        DisData[3] = 0x06; //"A"   
    }
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisRfWorkMOde(void)
{
    DisData[0] = 0x2A; //"S"
    DisData[1] = 0xEF; //"-"
    if(StoreBspFlg.TxMode == 1)
    {
        DisData[2] = 0x27; //"F"
        DisData[3] = 0x27; //"F"
    }
    else
    {
        DisData[2] = 0x12; //"0"
        DisData[3] = 0x12; //"0"
    }
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisChannel(void)
{
    DisData[0] = 0x33; //"C"
    DisData[1] = 0xEF; //"-"
    DisData[2] = 0xFF; //" "
    DisData[3] = SN74LS164SegTab[StoreBspFlg.Channel]; //"0~F"
}
/********************************************************************************************************

********************************************************************************************************/


/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSetAutoManual(void)
{
    DisData[0] = 0x6B; //"=-"
    DisData[1] = 0xEF; //"-"

    if(BspFlg.AutoManual == 0)
    {
        DisData[2] = 0xFF; //" "
        DisData[3] = 0x06; //"A"
    }
    else
    {   
        DisData[2] = 0x1F; //"n"
        DisData[3] = 0x06; //"A"   
    }
    
    if(BspFlg.Flash == 0)
    {
        DisData[2] = 0xFF; //" "
        DisData[3] = 0xFF; //" " 
    }
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSetRfWorkMOde(void)
{
    DisData[0] = 0x2A; //"S"
    DisData[1] = 0xEF; //"-"
    if(BspFlg.TxMode == 1)
    {
        DisData[2] = 0x27; //"F"
        DisData[3] = 0x27; //"F"
    }
    else
    {
        DisData[2] = 0x12; //"0"
        DisData[3] = 0x12; //"0"
    }
    
    if(BspFlg.Flash == 0)
    {
        DisData[2] = 0xFF; //" "
        DisData[3] = 0xFF; //" " 
    }
}
/********************************************************************************************************

********************************************************************************************************/
void MENU_DisSetChannel(void)
{
    DisData[0] = 0x33; //"C"
    DisData[1] = 0xEF; //"-"
    DisData[2] = 0xFF; //" "
    DisData[3] = SN74LS164SegTab[BspFlg.Channel]; //"0~F"
    
    if(BspFlg.Flash == 0)
    {
        DisData[3] = 0xFF; //" " 
    }
}
/********************************************************************************************************

********************************************************************************************************/






