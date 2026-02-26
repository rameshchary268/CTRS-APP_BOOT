/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: MyMain.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains definitions for main and other local functions
 *
 * Note:
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "main.h"
#include "MyMain.h"
#include "DisplayCmdHandlers.h"
#include "DispUIkeyhandler.h"

/* Defines */
/* Declaration of external global variables */


/* Declaration of global variables */
EXTI_HandleTypeDef hext0;
EXTI_ConfigTypeDef Exti0_Config;

uint8_t Mains_fail=1;
uint32_t hb_flag=1;
uint8_t  MODE_SEL = 1;       // 0-Test mode, 1-Run mode
uint8_t  active_bank[4];     // Byte1 (current bank ID) => All except 2 - Bank1, 2 - Bank2, Bytes[3:1] => FWDL Count
uint8_t  boot_delay=0;
char fw_ver[6] = "     ";
char hw_ver[6] = HW_REV_STR;

const char *verStr = APP_FW_VER;

/* Declaration of static local variables */
uint32_t sw_tmr_active[NUM_SW_TMRS];
uint32_t *sw_tmr_flag[NUM_SW_TMRS];
uint32_t sw_tmr_count[NUM_SW_TMRS];
uint8_t  mute_seq_start_flg, mute_rem_start_flag;
uint16_t mute_seq_time, mute_rem_time, ui_one_sec_timer;
uint32_t hb_blink_dly;
uint8_t  boot_sw_dly = 10;
int      dly_flag;
int      buz_flag;
extern TX_EVENT_FLAGS_GROUP LOG_ENTRY_FLAG;
extern uint8_t Green_flag, LED_off;
//int16_t  cnt1 = 10;
//uint32_t cnt2 = 0x00ff0000;
/* Declaration of local functions */

/*
 * ****************************
 * my_init Function definition
 * ****************************
 */
void my_init(void)
{
   // Read unique serial number and unique mac address from OTP memory (8-bytes/64-bit wide read/write)
	GPIO_init();
	LRTC_init();
	ulog_if_init();
	ADC_init();
	UI_init();
	Err_msg_init();
	EEPROM_init();
	Calib_init();
	Settings_Init();
	Wifi_init();
	//BLE_init();
	Get_TimeStamp();

	if (PCA9955B_Init(PCA9955B_ADDR) != HAL_OK) {

	}
	if (PCA9955B_Init(PCA9955B_ADDR2) != HAL_OK) {
		}
	// Configure for high frequency operation
	PCA9955B_SetHighFrequencyMode();
	PCA9955B_SetAllLEDsOff();
	PCA9955B_SetGlobalBrightness(vp_param_rw_str.STM_LED_Brightness);
	PCA9955B_SetAllLEDsOff();
	PCA9955B_SetColorOn(1);
#if 1
	// Get the cause of last reset. This can be sent over Modbus for analysis
	RCC->CSR |= RCC_CSR_RMVF;             // Clear the causes bits
#endif
	//HAL_TIM_Base_Start_IT(&htim2);          // Start Timer6
	// Timer7 post-init setup
	if (HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID,
			TimeBase_Gen) == HAL_OK) {
		// Successfully added the call-back function
		HAL_TIM_Base_Start_IT(&htim6);        // Start Timer7
		HAL_NVIC_EnableIRQ(TIM7_IRQn);        // Enable Timer7 interrupt
	}
}

/*
 * ****************************
 * My Main Function definition
 * ****************************
 */

void Drv_LED(uint8_t id, DRV_STATE state)
{
  GPIO_TypeDef *pGPIO;
  uint16_t pinNum;

  switch (id)
  {
    case (LED_HB):
      pGPIO = GPIOE;
	  pinNum = GPIO_ODR_OD7;
	  uint8_t pwm0;
	  PCA9955B_ReadPWM0(&hi2c1, &pwm0);
	  if (pwm0 == 0x00)
	  {
	      // PCA9955B likely reset → reinitialize all registers
		if (PCA9955B_Init(PCA9955B_ADDR) != HAL_OK) {

		}
		if (PCA9955B_Init(PCA9955B_ADDR2) != HAL_OK) {
			}
		// Configure for high frequency operation
		PCA9955B_SetHighFrequencyMode();
		PCA9955B_SetAllLEDsOff();
		Green_flag=0;
		LED_off=0;
		PCA9955B_SetGlobalBrightness(vp_param_rw_str.STM_LED_Brightness);
	  }
	  break;
    case (Power_status_LED):
	  pGPIO = GPIOB;
	  pinNum = GPIO_ODR_OD0;
	  break;
    default:
	  break;
  }
  switch (state)
  {
    case DRV_OFF:
      HAL_GPIO_WritePin(pGPIO, pinNum, GPIO_PIN_RESET);
 	  break;

    case DRV_ON:
      HAL_GPIO_WritePin(pGPIO, pinNum, GPIO_PIN_SET);
	  break;

    case DRV_TOGGLE:
      HAL_GPIO_TogglePin(pGPIO, pinNum);
	  break;

    default:
	  // Do nothing
	  break;
  }
}



void my_main(void)
{

	  if (Update.firm_page  && (Update.update_firmware==1) && Update.update_display==0 && vp_rt_wr_str.PowerStatus)
	  {
		TargetPage =44;
		SendCmdToDisplay(LOAD_PAGE_CMD);
		boot_delay++;
		// Jump to Bank2 as image file is available to be flashed at Bank1
		if(boot_delay>1)
		{
			while(1)
			{
				NVIC_SystemReset();
				//reset_controller(APPLICATION_STRT_ADDR);
			}
		}
	  }
	  else
	  {
		 // Continue in Application RUN mode...!!!
	  }  // End of "else" of "if (SD_info.bin_present == 1)"
    Get_TimeStamp();

#ifdef EN_IWDG
  HAL_IWDG_Refresh(&hiwdg);
#endif
  vp_rt_wr_str.PowerStatus=HAL_GPIO_ReadPin(GPIOB, GPIO_IDR_ID1);
  vp_rt_wr_str.Lid_status = HAL_GPIO_ReadPin(GPIOE, GPIO_IDR_ID9);
  UI_proc();
  Track_schedule();
//  RS485_master_proc();
//  RS485_tx_proc();
//  RS485_rx_proc();

}

void reset_controller(uint32_t app_vector_addr)
{
	 typedef void (*pFunction)(void);

	 uint32_t app_msp = *((uint32_t *)app_vector_addr);
	 uint32_t reset_word = *((uint32_t *)(app_vector_addr + 4));
	 uint32_t reset_addr = (reset_word & (~(uint32_t)1u)); // mask thumb bit for sanity
	 pFunction app_entry = (pFunction)reset_word;

	 /* Basic sanity checks */
//	 if ((app_msp < 0x20000000u) || (app_msp > 0x3FFFFFFFu) || (reset_addr < 0x08000000u))
//	 {
//		 /* invalid vector table — stay here */
//		 while (1) { __WFE(); }
//	 }

	 HAL_RCC_DeInit();

	   /* De-initialize all peripherals */
	   HAL_DeInit();

	 __disable_irq();

	 /* Force thread mode to use MSP (important when jumping from a thread that used PSP) */
	 __set_CONTROL(0);
	 __ISB();

	 /* Stop SysTick (prevent stray ticks) */
	 SysTick->CTRL = 0;
	 SysTick->VAL  = 0;

	 /* Disable all interrupts and clear pending flags */
	 for (uint32_t i = 0; i < ((MCU_IRQS + 31u) / 32u); ++i)
	 {
		 NVIC->ICER[i] = 0xFFFFFFFFu;
		 NVIC->ICPR[i] = 0xFFFFFFFFu;
	 }

	 /* Re-enable all interrupts */
	 __enable_irq();

	 /* Point vector table to application (so interrupts go to app handlers) */
	 SCB->VTOR = app_vector_addr;
	 __DSB();
	 __ISB();

	 /* Set MSP from application vector table */
	 __set_MSP(app_msp);
	 __DSB();
	 __ISB();

	 /* Jump to application's Reset_Handler (Thumb bit may be set in pointer) */
	 app_entry();

	 /* Never returns */
	 while (1) { __WFE(); }

//	  typedef void (*pFunction)(void);
//
//	  pFunction JumpToApplication;
//	  uint32_t JumpAddress = APPLICATION_STRT_ADDR;
//	  __disable_irq();
//
//	    SysTick->CTRL = 0;                   // Disable SysTick timer
//	    HAL_RCC_DeInit();                    // Set the clock to the default state
//	    HAL_DeInit();
//	    for (uint8_t i = 0; i < (MCU_IRQS + 31u) / 32; i++)
//	    {
//	  	// Clear Interrupt Enable Register & Interrupt Pending Register
//	  	NVIC->ICER[i]=0xFFFFFFFF;
//	  	NVIC->ICPR[i]=0xFFFFFFFF;
//	    }
//	    __enable_irq();
//	  // Flash Bank 1 mapped at 0x0800 0000 (and aliased @0x0000 0000)
//	  // and Flash Bank 2 mapped at 0x0808 0000 (and aliased at 0x0008 0000)
//	  JumpToApplication = (void (*)(void))(*((uint32_t *)(JumpAddress + 4)));
//	  __set_MSP(*(__IO uint32_t*)JumpAddress );
//	  JumpToApplication();
//	  while(1);
}
/*
 * **************************************
 * **************************************
 * Function Declarations start here
 * **************************************
 * **************************************
 */
/*
 * *****************************************************************************
 * Function name: GPIO_init()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to initialize the GPIOs.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void GPIO_init(void)
{
  // EXTI interrupt via GPIOs (Mains power failure interrupt and mains restoration wake-up event)

}

/*
 * *****************************************************************************
 * Function name: HAL_GPIO_EXTI_Rising_Callback()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the common call-back routine for all the EXTI interrupts.
 * Parameters:
 *  Input  : GPIO pin
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  if (GPIO_Pin == GPIO_PIN_1)
  {
	//  Usb_vbus(DRV_OFF);
	  HAL_GPIO_WritePin(GPIOC, GPIO_ODR_OD1, GPIO_PIN_RESET);
	  System_Status[ERROR_FLAG_0] &= ~ERR_MAIN_PWR_FAILED;
	  System_Status[ERROR_FLAG_0] &= ~ERR_BATTERY_BACKUP;
	  spm_info.shutdown = 0;
	  vp_evnt_wr_str.mains_info.status = 1;
  }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Rising_Callback could be implemented in the user file
   */
}

/*
 * *****************************************************************************
 * Function name: HAL_GPIO_EXTI_Falling_Callback()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the common call-back routine for all the EXTI interrupts.
 * Parameters:
 *  Input  : GPIO pin
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  if (GPIO_Pin == GPIO_PIN_1)
  {
	  tx_event_flags_set(&LOG_ENTRY_FLAG, 0x01, TX_OR);
//	  if ((((System_Status[WARNING_FLAG] & WRN_BATT_VOLTAGE_LOW) ||
//	  		   (System_Status[WARNING_FLAG] & WRN_BATT_NOT_CONNECTED)) != 0) ||
//	  		  flag_no_bat)  // Revert back after battery ok
//		{
//		 // Usb_vbus(DRV_ON);
//		  HAL_GPIO_WritePin(GPIOC, GPIO_ODR_OD1, GPIO_PIN_SET);
//		  bat_down=1;
//		  System_Status[ERROR_FLAG_0] |= ERR_MAIN_PWR_FAILED;
//		  UpdateUIErrorLog();
//		}
//		else
//		{
//			  // The unit is operating with Battery power
//			  vp_evnt_wr_str.mains_info.status = 0;
//			  bat_down=1;
//			System_Status[ERROR_FLAG_0] |= ERR_MAIN_PWR_FAILED;
//			if(vp_param_rw_str.Battery_mode_sw_over)
//			{
//				System_Status[ERROR_FLAG_0] |= ERR_BATTERY_BACKUP;
//			}
//			UpdateUIErrorLog();
//		}
//	  bat_down=0;
  }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Falling_Callback could be implemented in the user file
   */
}


/*
 * *****************************************************************************
 * Function name: GetSysTick()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to get the system tick value.
 *  Interrupt duration is 1ms.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
uint32_t GetSysTick(void)
{
  return ((uint32_t)uwTick);
}

/*
 * *****************************************************************************
 * Function name: ChkSysTickDly()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to check for the system tick based delay.
 *  Interrupt duration is 1ms.
 * Parameters:
 *  Input  : Time delay in ms
 *  Return : 0 - Elapsed, 1 - Not elapsed
 * Note:
 *
 * *****************************************************************************
 */
uint8_t ChkSysTickDly(uint32_t dly)
{
  static uint32_t final_value;
  static uint8_t first_time = 1;

  if (first_time)
  {
    first_time = 0;
    final_value = GetSysTick() + dly;
  }
  else
  {
    if (final_value == GetSysTick())
    {
      first_time = 1;
      return 0;
    }
  }
  return 1;
}


/*
 * *****************************************************************************
 * Function name: TimeBase_Gen()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the callback function to the ISR for Timer7 interrupt vector.
 *  Interrupt duration is 1ms.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TimeBase_Gen(TIM_HandleTypeDef *htim)
{
  uint8_t i;

  if (++ui_one_sec_timer >= 667)
  {
	ui_one_sec_timer = 0;
	if (mute_seq_start_flg)
	{
	  if (mute_seq_time > 0)
	  {
	    mute_seq_time--;
	    if (mute_seq_time == 0)
		{
		  if (vp_param_rw_str.AudiableRepeatDelaySwitch)
		    mute_seq_start_flg=0;
	    }
	  }
	  else
	  {
	    if (vp_param_rw_str.AudiableRepeatDelaySwitch)
		  mute_seq_start_flg = 0;
	  }
	}

	if (mute_rem_start_flag)
	{
	  if(mute_rem_time > 0)
	  {
	    mute_rem_time--;
		if(mute_rem_time == 0)
		{
		  if(vp_param_rw_str.RemoteAlarmDelaySwitch)
		  mute_rem_start_flag = 0;
	    }
	  }
	  else
	  {
	  }
    }
  }

  if (factory_msg1_display_delay > 0)
	factory_msg1_display_delay--;
  if (factory_msg2_display_delay > 0)
	factory_msg2_display_delay--;

  for (i=0; i<NUM_SW_TMRS; i++)
  {
    if (sw_tmr_active[i] == 1)
    {
      if (--sw_tmr_count[i] == 0)
      {
        *sw_tmr_flag[i] = 1;
        sw_tmr_active[i] = 0;
      }
    }
  }

 // httpServer_time_handler();
}

/*
 * *****************************************************************************
 * Function name: Set_delay()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This function initializes timer modules and parameters.
 * Parameters:
 *  Input  : Timer ID, Delay in ms, address of flag
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Set_delay(uint8_t tmr_id, uint32_t delay, uint32_t *flag)
{
  sw_tmr_active[tmr_id] = 1;
  sw_tmr_flag[tmr_id] = flag;
  sw_tmr_count[tmr_id] = delay;
  *sw_tmr_flag[tmr_id] = 0;
}

/*
 * *****************************************************************************
 * Function name: Dis_sw_tmr()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This function disables the software timer.
 * Parameters:
 *  Input  : Software timer ID
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Dis_sw_tmr(uint8_t tmr_id)
{
  sw_tmr_active[tmr_id] = 0;
}



/*
 * *********************************
 * *********************************
 * End of the module: MyMain.c
 * *********************************
 * *********************************
 */

