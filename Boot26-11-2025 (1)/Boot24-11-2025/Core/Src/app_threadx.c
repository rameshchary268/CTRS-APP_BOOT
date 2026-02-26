/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <main.h>
#include "fx_api.h"
#include <mymain.h>
#include "BOOT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern TX_EVENT_FLAGS_GROUP app_event_flags;
extern  ULONG last_data_tick;
uint8_t no_data_flag = 0;
extern uint8_t 	  EMMC_OK;

uint8_t flash_flag=0;
TX_THREAD my_thread;

extern uint8_t close_flag;
UINT mmc_status1;
ULONG my_main_thread_stack[MY_MAIN_THREAD_STACK_SIZE / sizeof(ULONG)];
BOOT_INFO BOOT_info = BOOT_INFO_DEFAULT;
TX_TIMER             no_data_timer;
extern uint8_t mythread_flag;
extern uint8_t uart_flag_cdc;
extern volatile UINT my_main_flag;
extern TX_TIMER usb_first_timeout_timer;
extern FX_FILE fx_file;
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  tx_event_flags_create(&app_event_flags, "app_event_flags");

   ret = tx_thread_create(
    	            &my_thread,                // Pointer to thread control block
    	            "MyThread",                // Thread name (for debug)
    	            my_main_thread,           // Entry function
    	            0,                         // Entry input parameter
    	            my_main_thread_stack,           // Pointer to stack base
    	            MY_MAIN_THREAD_STACK_SIZE,      // Stack size
    	            1,                         // Priority (1 = higher, larger = lower priority)
    	            1,                         // Preemption threshold
    	            TX_NO_TIME_SLICE,          // Time slice (not used here)
    	            TX_AUTO_START              // Auto start thread
    	        );
   tx_timer_create(&no_data_timer, "no_data_timer",
                   no_data_timer_callback,
                   0,
                   TX_TIMER_TICKS_PER_SECOND / 2,   // periodic every 0.5s
                   TX_TIMER_TICKS_PER_SECOND / 2,
                   TX_AUTO_ACTIVATE);
  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

void my_main_thread(ULONG thread_input)
{
  /* USER CODE BEGIN my_thread_entry */

	  //tx_timer_activate(&usb_first_timeout_timer);
	Set_delay(BOOT_DELAY, DLY_BOOT_START, &BOOT_info.app_flag);

  while (1)
  {

	  if(EMMC_OK)
	  {
		  my_func();
	  }
//	  my_func();
             // your jump function or handler
	  if(flash_flag!=1) //this thread should not go to sleep while writing flash data.
	  {
	  tx_thread_sleep(1);
	  }
  }
  /* USER CODE END my_thread_entry */
}
VOID no_data_timer_callback(ULONG id)
{
    ULONG now = tx_time_get();

    // If last_data_tick is zero treat as no activity yet
    if ((now - last_data_tick) > NO_DATA_TIMEOUT_TICKS)
    {
        if (no_data_flag)   // only act on rising edge
        {
//            no_data_flag = 1;
            // signal FileX thread that timeout happened (bit 0x04)
            tx_event_flags_set(&app_event_flags, 0x04, TX_OR);
        }
    }
    else
    {
    }
}


/* USER CODE END 1 */
