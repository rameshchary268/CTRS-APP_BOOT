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
#include "MyMain.h"
#include "stdint.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define THREAD_STACK_SIZE 1024*4

static TX_THREAD Nora_thread;
uint8_t Nora_stack[THREAD_STACK_SIZE];

static TX_THREAD UI_Error_log;
uint8_t UI_ERROR_STACK[THREAD_STACK_SIZE];
static TX_THREAD UI_RECEIVE;
uint8_t UI_RECEIVE_STACK[THREAD_STACK_SIZE*8];

static TX_THREAD VACCUM_thread;
uint8_t VACCUM_stack[THREAD_STACK_SIZE];

static TX_THREAD MAX_SPI_thread;
uint8_t MAX_SPI_stack[THREAD_STACK_SIZE];

static TX_THREAD HB_LED_thread;
uint8_t HB_LED_stack[THREAD_STACK_SIZE];


static TX_THREAD ADC_thread;
uint8_t ADC_stack[THREAD_STACK_SIZE];

static TX_THREAD MAIN_thread;
uint8_t MAIN_stack[THREAD_STACK_SIZE];

TX_THREAD log_data_thread;
uint8_t LOG_DATA[THREAD_STACK_SIZE];

TX_EVENT_FLAGS_GROUP LOG_ENTRY_FLAG;


 int led_flag;
extern SPI_HandleTypeDef hspi2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
TX_SEMAPHORE emmc_semaphore;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
VOID Nora_thread_entry(ULONG arg);
VOID LOG_Entry(ULONG arg);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

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
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  VOID *pointer;

/* USER CODE BEGIN MX_FileX_MEM_POOL */
  (void)byte_pool;
/* USER CODE END MX_FileX_MEM_POOL */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*Allocate memory for the main thread's stack*/
//  ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
///* Check FX_APP_THREAD_STACK_SIZE allocation*/
//  if (ret != FX_SUCCESS)
//  {
//    return TX_POOL_ERROR;
//  }
  tx_event_flags_create(&LOG_ENTRY_FLAG, "Log_Entry_Flag");

  tx_semaphore_create(&emmc_semaphore, "EMMC Access", 1);

  tx_thread_create(&log_data_thread,"Log Entry THread",LOG_Entry,0,LOG_DATA,THREAD_STACK_SIZE,1,1,1,TX_AUTO_START);

  tx_thread_create(&Nora_thread,"NORA thread",Nora_thread_entry,0x1234,Nora_stack,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);

//  ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
//  /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//    if (ret != FX_SUCCESS)
//    {
//      return TX_POOL_ERROR;
//    }

  tx_thread_create(&MAX_SPI_thread,"MAX_RTD thread",MAX_RTD_entry,0x1234,MAX_SPI_stack,THREAD_STACK_SIZE,11,11,1,TX_AUTO_START);
//  ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
//  /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//    if (ret != FX_SUCCESS)
//    {
//      return TX_POOL_ERROR;
//    }


    tx_thread_create(&HB_LED_thread,"HB_LED thread",HB_LED_entry,0x1234,HB_LED_stack,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);

//    ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
//    /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//      if (ret != FX_SUCCESS)
//      {
//        return TX_POOL_ERROR;
//      }

    tx_thread_create(&ADC_thread,"ADC thread",ADC_entry,0x1234,ADC_stack,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);

//    ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
//    /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//      if (ret != FX_SUCCESS)
//      {
//        return TX_POOL_ERROR;
//      }
  //
    tx_thread_create(&VACCUM_thread,"MAX_VACCUM thread",VACCUM_entry,0x1234,VACCUM_stack,THREAD_STACK_SIZE,15,15,1,TX_AUTO_START);

//    ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
//    /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//      if (ret != FX_SUCCESS)
//      {
//        return TX_POOL_ERROR;
//      }

    tx_thread_create(&MAIN_thread,"MAIN thread",MAIN_entry,0x1234,MAIN_stack,THREAD_STACK_SIZE,8,8,1,TX_AUTO_START);

//    ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
//
//    /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//      if (ret != FX_SUCCESS)
//      {
//        return TX_POOL_ERROR;
//      }
    tx_thread_create(&UI_Error_log,"UI_ERROR LOG",UI_Error_Log_entry,0x1234,UI_ERROR_STACK,THREAD_STACK_SIZE,9,9,1,TX_AUTO_START);

//    ret = tx_byte_allocate(byte_pool, &pointer, THREAD_STACK_SIZE*8, TX_NO_WAIT);
//
//    /* Check FX_APP_THREAD_STACK_SIZE allocation*/
//      if (ret != FX_SUCCESS)
//      {
//        return TX_POOL_ERROR;
//      }
    tx_thread_create(&UI_RECEIVE,"UI_RECEIVE",UI_RECEIVE_entry,0x1234,UI_RECEIVE_STACK,THREAD_STACK_SIZE*8,9,9,1,TX_AUTO_START);
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

VOID LOG_Entry(ULONG arg)
{
	while(1)
	{
	ULONG flags =0;
	tx_event_flags_get(&LOG_ENTRY_FLAG,0x01,TX_AND_CLEAR,&flags,TX_WAIT_FOREVER);

	if ((((System_Status[WARNING_FLAG] & WRN_BATT_VOLTAGE_LOW) ||
		  		   (System_Status[WARNING_FLAG] & WRN_BATT_NOT_CONNECTED)) != 0) ||
		  		  flag_no_bat)  // Revert back after battery ok
			{
			 // Usb_vbus(DRV_ON);
			  HAL_GPIO_WritePin(GPIOC, GPIO_ODR_OD1, GPIO_PIN_SET);
			  bat_down=1;
			  System_Status[ERROR_FLAG_0] |= ERR_MAIN_PWR_FAILED;
			  UpdateUIErrorLog();
			}
			else
			{
				  // The unit is operating with Battery power
				  vp_evnt_wr_str.mains_info.status = 0;
				  bat_down=1;
				System_Status[ERROR_FLAG_0] |= ERR_MAIN_PWR_FAILED;
				if(vp_param_rw_str.Battery_mode_sw_over)
				{
					System_Status[ERROR_FLAG_0] |= ERR_BATTERY_BACKUP;
				}
				UpdateUIErrorLog();
			}
		  bat_down=0;
	}
}
VOID UI_Error_Log_entry(ULONG arg)
{
	while(1)
	{
		  if((vp_cal_pt_rd_str.status == CALIB_INACTIVE)&&(Init_Wait_Delay_for_UI_flg))
		  {
			  UpdateUIErrorLog();
		  }
		  Calib_proc();
		  tx_thread_sleep(4);
	}

}

VOID UI_RECEIVE_entry(ULONG arg)
{
	while(1)
	{
		  ProcessLCDData();
		  tx_thread_sleep(4);
	}
}


VOID Nora_thread_entry(ULONG arg)
{
	while(1)
	{
		WIFI_Process();
	   // BLE_Process();
	    tx_thread_sleep(5);
	}
}

VOID VACCUM_entry(ULONG arg)
 {
	int val_1 = 1;
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	MAX11206_Init(&hspi2, GAIN1); // change spi accourdingly

	HAL_Delay(100);

	while (1) {

		if (1) {
			val_1 = Read_MAX11206_ADC_Value();
		}
		led_flag = 0;
		tx_thread_sleep(10);
	}
}

VOID MAIN_entry(ULONG arg)
{
	while (1)
	{
		my_main();
		tx_thread_sleep(4);
	}
}

VOID MAX_RTD_entry(ULONG arg)
{
	RTD_init();
	while (1) {
		RTD_proc();
		TempMon1_proc();
		TempMon2_proc();
		  if((reinitialize_SPI)&&(Init_Wait_Delay_for_UI_flg))
		  {
			  re_initialize_SPI3();
		  }
		tx_thread_sleep(5);
	}
}

VOID HB_LED_entry(ULONG arg)
{

	while (1) {
		Set_delay(SW_TMR_HB, hb_blink_dly, &hb_flag);
		Drv_LED(LED_HB, DRV_TOGGLE);
		if(vp_rt_wr_str.PowerStatus)
		{
			Drv_LED(Power_status_LED, DRV_ON);

		}
		else{
			Drv_LED(Power_status_LED, DRV_TOGGLE);
		}

		Get_TimeStamp();
		tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);  // 1 second
	}
}

VOID ADC_entry(ULONG arg)
{
	while (1) {
		ADC_proc();
		tx_thread_sleep(5);
	}
}
/* USER CODE END 1 */
