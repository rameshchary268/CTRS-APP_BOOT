/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_filex.c
  * @author  MCD Application Team
  * @brief   FileX applicative file
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
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <FileX.h>
#include "ux_device_cdc_acm.h"
#include "app_threadx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* Main thread stack size */
#define FX_APP_THREAD_STACK_SIZE         1024*20
/* Main thread priority */
#define FX_APP_THREAD_PRIO               10
/* USER CODE BEGIN PD */
extern  uint8_t uart_flag_cdc;
extern TX_TIMER usb_first_timeout_timer;
extern uint8_t timer_deactivate_10;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Main thread global data structures.  */
TX_THREAD       fx_app_thread;

/* Buffer for FileX FX_MEDIA sector cache. */
ALIGN_32BYTES (uint32_t fx_mmc_media_memory[FX_STM32_MMC_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);
/* Define FileX global data structures.  */
FX_MEDIA        mmc_disk;

/* USER CODE BEGIN PV */
uint8_t uart_flag_cdc=0;
TX_MUTEX file_mutex;
TX_EVENT_FLAGS_GROUP app_event_flags;
extern ULONG  cdc_data_length;
extern UCHAR cdc_rx_buffer[RX_BUFFER_SIZE];
FX_FILE fx_file;
extern volatile UINT my_main_flag;
extern TX_TIMER usb_first_timeout_timer;
extern TX_TIMER usb_data_timeout_timer;
extern TX_THREAD my_thread;
extern uint8_t no_data_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Main thread entry function.  */
void fx_app_thread_entry(ULONG thread_input);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application FileX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
*/
UINT MX_FileX_Init(VOID *memory_ptr)
{
  UINT ret = FX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  VOID *pointer;

/* USER CODE BEGIN MX_FileX_MEM_POOL */

/* USER CODE END MX_FileX_MEM_POOL */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*Allocate memory for the main thread's stack*/
  ret = tx_byte_allocate(byte_pool, &pointer, FX_APP_THREAD_STACK_SIZE, TX_NO_WAIT);

/* Check FX_APP_THREAD_STACK_SIZE allocation*/
  if (ret != FX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

/* Create the main thread.  */
  ret = tx_thread_create(&fx_app_thread, FX_APP_THREAD_NAME, fx_app_thread_entry, 0, pointer, FX_APP_THREAD_STACK_SIZE,
                         FX_APP_THREAD_PRIO, FX_APP_PREEMPTION_THRESHOLD, FX_APP_THREAD_TIME_SLICE, FX_APP_THREAD_AUTO_START);

/* Check main thread creation */
  if (ret != FX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

/* USER CODE BEGIN MX_FileX_Init */

/* USER CODE END MX_FileX_Init */

/* Initialize FileX.  */
  fx_system_initialize();

/* USER CODE BEGIN MX_FileX_Init 1*/

/* USER CODE END MX_FileX_Init 1*/

  return ret;
}

/**
 * @brief  Main thread entry.
 * @param thread_input: ULONG user argument used by the thread entry
 * @retval none
*/
uint8_t 	  EMMC_OK;
 void fx_app_thread_entry(ULONG thread_input)
 {

  UINT mmc_status = FX_SUCCESS;

/* USER CODE BEGIN fx_app_thread_entry 0*/
  ULONG actual_flags;

/* USER CODE END fx_app_thread_entry 0*/

/* Open the disk driver */
  mmc_status =  fx_media_open(&mmc_disk, FX_MMC_VOLUME_NAME, fx_stm32_mmc_driver, (VOID *)FX_NULL, (VOID *) fx_mmc_media_memory, sizeof(fx_mmc_media_memory));

/* Check the media open mmc_status */
  if (mmc_status != FX_SUCCESS)
  {
    /* USER CODE BEGIN MMC open error */
    Error_Handler();

    /* USER CODE END MMC open error */
  }

/* USER CODE BEGIN fx_app_thread_entry 1*/
  EMMC_OK=1;
  //creating_file("CTRS_CFC.bin");
  tx_event_flags_get(&app_event_flags, 0x03, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

  tx_mutex_create(&file_mutex, "file_mutex", TX_NO_INHERIT);

  tx_event_flags_set(&app_event_flags, 0x01, TX_OR);


  while (1)
  {
	  ULONG actual_flags = 0;

		     // wait for either data (0x02) or timeout (0x04), check periodically if desired
		     tx_event_flags_get(&app_event_flags, 0x04, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

		     		     if (actual_flags & 0x04)
		     		     {
		     		         // close + flush file safely
		     		         fx_media_flush(&mmc_disk);
		     		         fx_file_close(&fx_file);
//		     		        timer_deactivate_10=0;
		     		      //  my_main_flag = 1;
		     		        no_data_flag=0;
		     		       File_validation();

		     		       tx_thread_resume(&my_thread);

		     		         // Optionally set state so subsequent writes re-open a new file
		     		     }
tx_thread_sleep(1);

  }
/* USER CODE END fx_app_thread_entry 1*/
  }

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
