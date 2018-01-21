/*********************************************************************************************************
*FileName:	SHal.c
*Processor: zigbee RF MCU
*Complier:	IAR |KEIL |AVR GCC|HT-TECH
*Company:   
**********************************************************************************************************
*Author			Date				Comment
*LXP			2014/3/11			Original
*********************************************************************************************************/
#include "SHal.h"
#include "hal_uart.h"
#include "OSAL.h"
/*********************************************************************************************************

*********************************************************************************************************/
#define MASTER_UART HAL_UART_PORT_0
#define SLAVE_UART  HAL_UART_PORT_1
/*********************************************************************************************************

*********************************************************************************************************/
void SerialApp_Init(void)
{
    halUARTCfg_t uartConfig;

    if(USE_MAX_UART > 0)
    {
        uartConfig.configured           = TRUE;                 // 2x30 don't care - see uart driver.
        uartConfig.baudRate             = HAL_UART_BR_38400;
        uartConfig.flowControl          = FALSE;
        uartConfig.flowControlThreshold = 64;                   // 2x30 don't care - see uart driver.
        uartConfig.rx.maxBufSize        = 64;                   // 2x30 don't care - see uart driver.
        uartConfig.tx.maxBufSize        = 64;                   // 2x30 don't care - see uart driver.
        uartConfig.idleTimeout          = 6;                    // 2x30 don't care - see uart driver.
        uartConfig.intEnable            = TRUE;                 // 2x30 don't care - see uart driver.
        uartConfig.callBackFunc         = SerialApp_CallBack;
        HalUARTOpen (MASTER_UART, &uartConfig);
        
        if(USE_MAX_UART > 1)
        {
            uartConfig.configured           = TRUE;                 // 2x30 don't care - see uart driver.
            uartConfig.baudRate             = HAL_UART_BR_38400;
            uartConfig.flowControl          = FALSE;
            uartConfig.flowControlThreshold = 64;                   // 2x30 don't care - see uart driver.
            uartConfig.rx.maxBufSize        = 64;                   // 2x30 don't care - see uart driver.
            uartConfig.tx.maxBufSize        = 64;                   // 2x30 don't care - see uart driver.
            uartConfig.idleTimeout          = 6;                    // 2x30 don't care - see uart driver.
            uartConfig.intEnable            = TRUE;                 // 2x30 don't care - see uart driver.
            uartConfig.callBackFunc         = SerialApp_CallBack;
            HalUARTOpen (SLAVE_UART, &uartConfig);
        }
    }
}
/*********************************************************************************************************

*********************************************************************************************************/
void SerialApp_CallBack(uint8 port, uint8 event)
{
    uint8 *pBuf,len;
    
    if(event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
    {
        len = Hal_UART_RxBufLen(port);
        pBuf= (uint8*)osal_msg_allocate(len);

        if(pBuf != NULL)
        {   
            HalUARTRead(port,pBuf,len);
            if(port == SLAVE_UART)
            {
                port = 1;
            }
            else
            {
                port = 0;
            }
            Serial_vRx(port,len,pBuf);
            osal_msg_deallocate((uint8*)pBuf);
        }
    }
}
/*********************************************************************************************************

*********************************************************************************************************/




