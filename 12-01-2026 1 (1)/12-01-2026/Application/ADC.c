/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: ADC.c
 * Author: Ilensys
 *
 * Created on 29 Aug, 2022, 2:23 PM
 *
 * Description:
 * This module contains definitions for ADC module and relevant data processing
 * functions
 *
 * Note:
 *  MCU supported: STM32L4P5VGT6
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DispVPAddrList.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
#include "DisplayTxHandlers.h"
#include "ErrorLog.h"

/* Declaration of external global variables */

/* Declaration of global variables */
//ADC_Common_TypeDef ADC123_COMMON;

ADC_INFO   ADC_info;
BATT_INFO  Batt_info;
uint16_t   avg_bat_val;
uint16_t   avg_dp_val = 0, prv_avg_dp_val = 0;
uint16_t   DPAvg_Value = 0;
uint8_t    Shut_Load=0, shutdown_flag=0, display_off_flg, bat_down=0;
uint8_t    flag_no_bat=0;


//volatile uint8_t ADCDataReady = 0;

/* Declaration of local functions */

/*
 * **************************************
 * **************************************
 * Function Definitions start here
 * **************************************
 * **************************************
 */

/*
 * *****************************************************************************
 * Function name: ADC_init()
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  This function initializes ADC module, parameters and its state machine.
 *  ADC clock frequency is 24MHz
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void ADC_init(void)
{
  ADC_info = ADC_INFO_DEFAULT;
  Batt_info = BATT_INFO_DEFAULT;
  ADC_Disable(&hadc1);
}

/*
 * *****************************************************************************
 * Function name: ADC_ISR_EOC_CallbackFunction()
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  This is the callback function to the ISR for ADC interrupts (EOC, EOS, EOSMP & OVR).
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void ADC_ISR_EOC_CallbackFunction(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance->ISR & ADC_ISR_EOC)
  {
    // End of Conversion
    ADC_info.Temp_buffer[ADC_info.ch_id++] = hadc1.Instance->DR;
    if (ADC_info.ch_id == MAX_ADC_CHANNELS)
    {
      ADC_info.ch_id = 0;
      ADC_info.ADC_Data_rdy = 1;
    }
	SET_BIT(hadc1.Instance->ISR, ADC_ISR_EOC);
  }
  if (hadc->Instance->ISR & ADC_ISR_EOS)
  {
    // End of Sequence.
	SET_BIT(hadc1.Instance->ISR, ADC_ISR_EOS);
  }
  if (hadc->Instance->ISR & ADC_ISR_OVR)
  {
    // Overrun error
    SET_BIT(hadc->Instance->ISR, ADC_ISR_OVR);
  }
}

/*
 * *****************************************************************************
 * Function name: ADC_proc()
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  This is the State Machine to handle various ADC related activities
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void ADC_proc(void)
{
  LevelMon_proc();
  static uint8_t timer_flag=0;

  //uint16_t avg_value;
  if(ADC_info.ADC_Fail>10)
  {
	  ADC_info.ch_id=0;
	  ADC_info.ADC_Fail=0;
	  HAL_ADC_Stop_IT(&hadc1);
	  ADC_Disable(&hadc1);
	  ADC_info.state=ADC_CALIBRATE;
  }
  switch (ADC_info.state)
  {
    case ADC_CALIBRATE:
      CLEAR_BIT(hadc1.Instance->CR,ADC_CR_DEEPPWD|ADC_CR_ADEN);
      SET_BIT(hadc1.Instance->CR,ADC_CR_ADVREGEN);
      SET_BIT(hadc1.Instance->CR,ADC_CR_ADCAL);
	  if (HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, ADC_ISR_EOC_CallbackFunction) != HAL_OK)
	  {
		  ADC_info.state = ADC_CALIBRATE;
		  break;
		// Error in setting the call-back function/interrupts for the ADC
	  }
	  ADC_info.state = ADC_START;
	  break;

    case ADC_START:
	  if ((HAL_ADC_Start_IT(&hadc1) != HAL_OK))
	  {
		  ADC_info.state = ADC_START;
		  break;
		// Error in setting the call-back function/interrupts for the ADC
	  }
	  ADC_Enable(&hadc1);
	  ADC_info.state = ADC_SOC;
	  Set_delay(SW_TMR_ADC, DLY_ADC_STRT, &ADC_info.poll_flag);
	  break;

    case ADC_SOC:
      if (ADC_info.poll_flag)
      {
        Set_delay(SW_TMR_ADC, DLY_ADC_POLL, &ADC_info.poll_flag);
        SET_BIT(hadc1.Instance->IER, ADC_IER_EOSIE | ADC_IER_EOCIE | ADC_IER_EOSMPIE);  // Enable interrupts
        SET_BIT(hadc1.Instance->CR, ADC_CR_ADSTART);    // Start a new conversion of single channel
        ADC_info.state = ADC_EOC;
      }
      break;

    case ADC_EOC:
      if(ADC_info.ADC_Data_rdy)
      {
    	 // calib3wayout();
    	  ADC_info.ADC_Data_rdy = 0;
    	  if(ADC_info.Temp_buffer[ADC_CH_REF]>=900 &&  ADC_info.Temp_buffer[ADC_CH_REF] <=1610)
		  {
    		  ADC_info.ADC_Fail=0;
			  ADC_info.ADC_Data_rdy = 0;
			  memcpy(&ADC_info.buffer,&ADC_info.Temp_buffer,sizeof(ADC_info.buffer));
			  ADC_info.data_rdy = 1;
			  ADC_info.ADCDataReady = 1;
		  }
    	  else
    	  {
    		  ADC_info.ADC_Fail++;
    	  }
      }

      if (ADC_info.data_rdy)
      {
        CLEAR_BIT(hadc1.Instance->IER, ADC_IER_EOSIE | ADC_IER_EOCIE | ADC_IER_EOSMPIE);  // Disable interrupts
        ADC_info.data_rdy = 0;
        //////////////////////////////
        // Battery voltage at ADC1_IN17
        //////////////////////////////
        avg_bat_val = IIRLPF_THERM((float)ADC_info.buffer[ADC_CH_BATT_VOLT], (float)avg_bat_val,0.333333,0.666667);;

        Batt_info.volt = ((float)avg_bat_val * 26.9) / MAX_ADC_VALUE;   // mul_fact changes from 13.5  to 1.7609 //
        vp_rt_wr_str.batt_volt = Batt_info.volt;
        if (((vp_param_rw_str.plumbing_options) & (BATTERY_SENS_SEL)))
        {
          if (ADC_info.cal_flag)
		  {
			if ((vp_rt_wr_str.batt_volt<=11.2) && (vp_rt_wr_str.PowerStatus == 0))
			{
			  System_Status[WARNING_FLAG] |= WRN_BATT_VOLTAGE_LOW;
			}
			else if ((vp_rt_wr_str.batt_volt>=11.28))
			{
			  System_Status[WARNING_FLAG] &= ~WRN_BATT_VOLTAGE_LOW;
			  System_Status[WARNING_FLAG] &= ~WRN_BATT_NOT_CONNECTED;
			}
			else
			{
			  // Battery voltage is within limits. Do nothing
			}
			if (vp_rt_wr_str.batt_volt<=0.5)
			{
			  System_Status[WARNING_FLAG] |= WRN_BATT_NOT_CONNECTED;
			}
			// Shutdown message should be popped up
			if ((vp_rt_wr_str.batt_volt!=0)&&(vp_rt_wr_str.batt_volt<=11.0) && (vp_rt_wr_str.PowerStatus == 0)&&(!spm_info.shutdown))//11
			{
			  spm_info.shutdown = 1;
			  Shut_Load=0;
			}
			else
			{
              // Do nothing
			}
		  }
		  else if (timer_flag==0)
		  {
			Set_delay(SW_TMR_batt_dly, DLY_MOV_AVG_CAL, &ADC_info.cal_flag);
			timer_flag =1;
		  }
        }
        else
        {
          flag_no_bat=1;
        }


        // Check for shut-down request
          // Restart a new conversion cycle
          ADC_info.state = ADC_SOC;
      }        // End of "if (ADC_info.data_rdy)"
      else if (ADC_info.poll_flag)
      {
        // Timed-out. Log error
        ADC_info.state = ADC_ERR;
      }
      else
      {
        // Do nothing. Wait for data to be ready
      }
      break;

    case ADC_HALT:
      // Do nothing. Wait for system shutdown
      break;

    case ADC_ERR:
      Set_delay(SW_TMR_ADC, DLY_ADC_POLL, &ADC_info.poll_flag);
      ADC_info.state = ADC_SOC;
      break;

    default:
      Set_delay(SW_TMR_ADC, DLY_ADC_POLL, &ADC_info.poll_flag);
      ADC_info.state = ADC_SOC;
      break;
  }
}
/*
 * *****************************************************************************
 * Function name: Before_update(void)
 * Created by: Arikanti venkatesh
 * Date:        14-10-2025
 * Description:
 * this function used to turn off fill valves before updating.
 *
 * Parameters:
 *  Input  :None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Before_update(void)
{

	vp_rt_wr_str.MBStatus=0;
	bat_down=1;
	System_Status[EVENT_FLAG] |= EVT_FIRMWARE_UPDATE;
//	UpdateUIErrorLog();
//	if(iot_info.file_open)
//	{
//		 f_close(&IOT_SD_File);
//		 iot_info.file_open=0;
//	}
//	IOT_update_SDCARD();
	bat_down=0;
}

/*
 * *****************************************************************************
 * Function name: ShutdownDisplay(void)
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 * this function used to shut down display and pop up shutting down message in display
 *
 * Parameters:
 *  Input  :set the flag
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void ShutdownDisplay(void)
{
  if (!Shut_Load)
  {
	Shut_Load=1;
	TargetPage = 29;
	SendCmdToDisplay(LOAD_PAGE_CMD);
	Set_delay(SW_TMR_SHUTDOWN, DLY_SHUTDOWN,(uint32_t*)&shutdown_flag);
	//Save messages to SD card
	bat_down=1;
	System_Status[EVENT_FLAG] |= EVT_DISPLAY_CLOSE;
	UpdateUIErrorLog();
//	if(iot_info.file_open)
//	{
//		 f_close(&IOT_SD_File);
//		 iot_info.file_open=0;
//	}
//	IOT_update_SDCARD();
	bat_down=0;
  }
  if (shutdown_flag)
  {
	shutdown_flag=0;
//	wifi_info.state = WIFI_STOP;
	//Drv_LED(LED_HB, DRV_OFF);
//	GPIOA->MODER &= ~(GPIO_MODER_MODE8_1 | GPIO_MODER_MODE8_0);    // PA8 Set as input port
//	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD8_1 | GPIO_PUPDR_PUPD8_0);    // No pull-up / no pull-down
//	GPIOA->PUPDR |=  (GPIO_PUPDR_PUPD8_0);    					   // Pull-up added
//	SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));          // MCU uses deep sleep as its low power mode
	// Shut-down the core -  LPMS=?000? + SLEEPDEEP bit + WFI
//	PWR->CR1 &= PWR_CR1_LPMS_STOP0;
//	while (1)
//	{
//	  HAL_IWDG_Refresh(&hiwdg);
//	  Wifi_shutdown();
//	}  // End of "while (1)"
  }    // End of "if (shutdown_flag)"
}      // End of function definition

/*
 * *********************************
 * *********************************
 * End of the module: ADC.c
 * *********************************
 * *********************************
 */

