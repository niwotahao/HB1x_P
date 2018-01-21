
#ifndef HAL_SPI1_H
#define HAL_SPI1_H

/*******************************************************************************************************/
#include "hal_defs.h"
#include "hal_mcu.h"
#include "ioCC2530.h"
/*******************************************************************************************************/
#define HAL_BOARD_IO_SPI_MISO_PORT     1
#define HAL_BOARD_IO_SPI_MISO_PIN      7
#define HAL_BOARD_IO_SPI_MOSI_PORT     1
#define HAL_BOARD_IO_SPI_MOSI_PIN      6
#define HAL_BOARD_IO_SPI_CLK_PORT      1
#define HAL_BOARD_IO_SPI_CLK_PIN       5
#define HAL_BOARD_IO_EM_CS_PORT        0
#define HAL_BOARD_IO_EM_CS_PIN         7




#define SPI_CLOCK_POL_LO       0x00
#define SPI_CLOCK_POL_HI       0x80
#define SPI_CLOCK_PHA_0        0x00
#define SPI_CLOCK_PHA_1        0x40
#define SPI_TRANSFER_MSB_FIRST 0x20
#define SPI_TRANSFER_MSB_LAST  0x00
/*******************************************************************************************************/

#define SPI_BEGIN()            MCU_IO_SET_LOW(HAL_BOARD_IO_EM_CS_PORT, HAL_BOARD_IO_EM_CS_PIN)
#define SPI_TX(x)              st(U1CSR &= ~(BV(2) | BV(1)); U1DBUF = x; while( !(U1CSR & BV(1)) );)
#define SPI_RX()               U1DBUF
#define SPI_WAIT_RXRDY()       st( { while(!(U1CSR & BV(1))); } )
#define SPI_RXRDY()            st( while((U1CSR & 0x04) != 0x04); )
/*******************************************************************************************************/






/*******************************************************************************************************/
#endif