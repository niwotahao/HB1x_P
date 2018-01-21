/*******************************************************************************************************


********************************************************************************************************/
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_SPI1.h"
#include "usb_board_cfg.h"
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
    uint8 baud_exponent;
    uint8 baud_mantissa;
    
    // Set SPI on UART 1 alternative 2
    PERCFG |= 0x02;
    // Use SPI on USART 1 alternative 2
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_SPI_MISO_PORT, HAL_BOARD_IO_SPI_MISO_PIN);
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_SPI_MOSI_PORT, HAL_BOARD_IO_SPI_MOSI_PIN);
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_SPI_CLK_PORT,  HAL_BOARD_IO_SPI_CLK_PIN);
    MCU_IO_OUTPUT(HAL_BOARD_IO_EM_CS_PORT, HAL_BOARD_IO_EM_CS_PIN, 1);

    //baud_exponent = 18;            /**< 8MHZ */    
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


********************************************************************************************************/