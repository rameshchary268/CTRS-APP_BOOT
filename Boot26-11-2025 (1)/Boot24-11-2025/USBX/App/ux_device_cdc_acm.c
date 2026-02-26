/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "Filex.h"
#include "app_threadx.h"
#include <BOOT.h>
#include "mymain.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIMEOUT_MS          5000  // 5 seconds
#define FILE_NAME           "usb_data.bin"
#define USER_RX_BUFFER_SIZE 512

TX_TIMER usb_first_timeout_timer;




/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

UX_SLAVE_CLASS_CDC_ACM *cdc_acm;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


volatile UINT my_main_flag = 0;

UCHAR cdc_rx_buffer[RX_BUFFER_SIZE];
extern FX_MEDIA        mmc_disk;
extern TX_MUTEX file_mutex;
extern  FX_FILE fx_file;
extern TX_EVENT_FLAGS_GROUP app_event_flags;
extern EMMC_Info_t emmc_info;
static UCHAR UserRxBuffer[USER_RX_BUFFER_SIZE];
static UINT file_opened = 0;
static UINT mmc_status;
extern BOOT_INFO BOOT_info;
 ULONG last_data_tick = 0;
ULONG TIM=0;
ULONG  cdc_data_length;
extern uint8_t uart_flag_cdc;
ULONG total_bytes_written =0 ;
extern TX_THREAD my_thread;
extern uint8_t no_data_flag;
uint8_t timer_deactivate_10=0;
uint8_t close_flag=0;
LED_INFO LED_info = LED_INFO_DEFAULT;

extern uint8_t 	  EMMC_OK;
//UINT  no_data_flag = 0;
/* USER CODE END 0 */

/**
  * @brief  USBD_CDC_ACM_Activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Activate */
 // UX_PARAMETER_NOT_USED(cdc_acm_instance);
  cdc_acm = (UX_SLAVE_CLASS_CDC_ACM *)cdc_acm_instance;
  tx_event_flags_set(&app_event_flags, 0x03, TX_OR);
  Set_delay(BOOT_DELAY, DLY_CDC_START, &BOOT_info.app_flag);
  /* USER CODE END USBD_CDC_ACM_Activate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_Deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  /* USER CODE END USBD_CDC_ACM_Deactivate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_ParameterChange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  /* USER CODE END USBD_CDC_ACM_ParameterChange */

  return;
}

/* USER CODE BEGIN 1 */
VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input)
{
      /* Private Variables */
//      ULONG tx_actual_length;
      Set_delay(SW_SD_CARD_DLY, DLY_LED_START, &LED_info.poll_flag);
      while(1)
      {


    		if(LED_info.poll_flag)
    		{
    			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_7);
//    			cnt++;
    			Set_delay(SW_SD_CARD_DLY, DLY_LED_START, &LED_info.poll_flag);
    		}

		tx_thread_sleep(1);

	}
}
VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input)
{
    ULONG rx_actual_length;
    ULONG actual_flags;

    tx_event_flags_get(&app_event_flags, 0x01, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    uart_flag_cdc =1;

    printf("USB CDC read thread started.\n");


    while (1)
    {
        ULONG rx_len = 0;
        UINT status = ux_device_class_cdc_acm_read(
                        cdc_acm,
                        cdc_rx_buffer,
                        sizeof(cdc_rx_buffer),
                        &rx_len);
        if(EMMC_OK)
        {
			if(timer_deactivate_10==0)
			{
				  tx_thread_suspend(&my_thread);
				  creating_file("CTRS_CFC.bin");
//				tx_timer_deactivate(&usb_first_timeout_timer);

				my_main_flag=0;
				timer_deactivate_10=1;
			}
			if (status == UX_SUCCESS && rx_len > 0)
			{
				 Set_delay(BOOT_DELAY, DLY_CDC_START, &BOOT_info.app_flag);

				// Notify FileX thread
				no_data_flag=1;

				cdc_data_length = rx_len;
				fx_file_write(&fx_file, cdc_rx_buffer, cdc_data_length);
				total_bytes_written += rx_len;

				last_data_tick = tx_time_get();      // ?? Update last activity

			}
        }

        tx_thread_sleep(1); // avoid CPU hog
    }
}


VOID usb_first_timeout_cb(ULONG input)
{
    // No CDC data within 20 sec
	my_main_flag = 1;
}


/* USER CODE END 1 */
