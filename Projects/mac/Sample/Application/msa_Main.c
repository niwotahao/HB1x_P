/**************************************************************************************************
  Filename:       msa_Main.c
  Revised:        $Date: 2012-10-09 13:50:35 -0700 (Tue, 09 Oct 2012) $
  Revision:       $Revision: 31762 $

  Description:    This file contains the main and callback functions


  Copyright 2006-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/
/* Hal Drivers */
#include "hal_types.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_drivers.h"
#include "hal_led.h"
#include "hal_adc.h"

#ifdef HAL_BOARD_CC2538
  #include "hal_systick.h"
#endif

/* MAC Application Interface */
#include "mac_api.h"

/* Application */
#include "msa.h"

/* OSAL */
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "OnBoard.h"
#include "OSAL_PwrMgr.h"
#include "OSAL_Nv.h"

/**************************************************************************************************
 * FUNCTIONS
 **************************************************************************************************/
/* This callback is triggered when a key is pressed */
void MSA_Main_KeyCallback(uint8 keys, uint8 state);

/* This function invokes osal_start_system */
void msaOSTask(void *task_parameter);
static void msaVddCheck( void );

extern void RfTestProcessEvent(void);

#ifdef IAR_ARMCM3_LM
#include "osal_task.h"
#define OSAL_START_SYSTEM() st(                                         \
  static uint8 param_to_pass;                                           \
  void * task_handle;                                                   \
  uint32 osal_task_priority;                                            \
                                                                        \
  /* create OSAL task */                                                \
  osal_task_priority = OSAL_TASK_PRIORITY_ONE;                          \
  osal_task_create(msaOSTask, "osal_task", OSAL_MIN_STACK_SIZE,         \
                   &param_to_pass, osal_task_priority, &task_handle);   \
  osal_task_start_scheduler();                                          \
)
#else
#define OSAL_START_SYSTEM() st(                                         \
  osal_start_system();                                                  \
)
#endif /* IAR_ARMCM3_LM */

/**************************************************************************************************
 * @fn          main
 *
 * @brief       Start of application.
 *
 * @param       none
 * * @return      none
 **************************************************************************************************
 */
int main(void)
{
#if defined(RF_CARRIER_TESTING)
  HAL_BOARD_INIT();
  RfTestProcessEvent();
#endif
  
  // Turn off interrupts
  osal_int_disable(INTS_ALL);

  /* Initialize hardware */
  HAL_BOARD_INIT();

#if defined(WATCH_DISPLY)

  for(int i =0;i<1000;i++)
  {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
  }
#else
  // Make sure supply voltage is high enough to run
  msaVddCheck();
  
#endif

  /* Initialze the HAL driver */
  HalDriverInit();

  // Initialize NV System  
  osal_nv_init( NULL );
  
  /* Initialize MAC */
  MAC_Init();

  /* Initialize the operating system */
  osal_init_system();

    // Allow interrupts
  osal_int_enable( INTS_ALL );
  
#ifdef HAL_BOARD_CC2538  
  /* Master enable interrupts */
  IntMasterEnable();

  /* Setup SysTick to generate interrupt every 1 ms */
  SysTickSetup(); 
#endif /* HAL_BOARD_CC2538 */
  
  /* Enable interrupts */
  HAL_ENABLE_INTERRUPTS();
 
  /* Setup Keyboard callback */
  HalKeyConfig(MSA_KEY_INT_ENABLED, MSA_Main_KeyCallback);

  /* Blink LED on startup */
  HalLedBlink (HAL_LED_4, 0, 40, 200);

  /* Start OSAL */
  OSAL_START_SYSTEM();

  return 0;
}
/*********************************************************************
 * @fn      msaVddCheck
 * @brief   Check if the Vdd is OK to run the processor.
 * @return  Return if Vdd is ok; otherwise, flash LED, then reset
 *********************************************************************/
static void msaVddCheck( void )
{
  uint8 cnt = 16;
  
  do 
  {
    while (!HalAdcCheckVdd(VDD_MIN_RUN));
  } while (--cnt);
}

/*********************************************************************
 * @fn      msaOSTask
 *
 * @brief   This function invokes osal_start_system(The main event loop)
 *
 * @param   task_parameter(Not being used in the present fucntion)
 *
 * @return  none
 */
void msaOSTask(void *task_parameter)
{
  osal_start_system();
}

/**************************************************************************************************
                                           CALL-BACKS
**************************************************************************************************/


/**************************************************************************************************
 * @fn      MSA_KeyCallback
 *
 * @brief   Callback service for keys
 *
 * @param   keys  - keys that were pressed
 *          state - shifted
 *
 * @return  void
 **************************************************************************************************/
void MSA_Main_KeyCallback(uint8 keys, uint8 state)
{
  if ( MSA_TaskId != TASK_NO_TASK )
  {
    MSA_HandleKeys (keys, state);
  }
}

/**************************************************************************************************
 *
 * @fn      MSA_PowerMgr
 *
 * @brief   Enable/Disable and setup power saving related stuff
 *
 * @param   mode - PWRMGR_ALWAYS_ON or PWRMGR_BATTERY
 *
 * @return  void
 *
 **************************************************************************************************/
void MSA_PowerMgr(uint8 enable)
{
  /* enable OSAL power management */
  if (enable)
   osal_pwrmgr_device(PWRMGR_BATTERY);
  else
   osal_pwrmgr_device(PWRMGR_ALWAYS_ON);
}

/*************************************************************************************************
**************************************************************************************************/
