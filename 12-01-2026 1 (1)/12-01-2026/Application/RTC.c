/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: RTC.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains definitions of RTC control & monitoring functions.
 * Interface IC is NXP PCF85063A
 *
 * Note:
 *  MCU supported: STM32L4P5VGT6
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"

/* Declaration of external global variables */
/* Declaration of global variables */
RTC_TimeTypeDef  Time = DEF_TIME_SETTING;
RTC_DateTypeDef  Date = DEF_DATE_SETTING;


uint8_t UI_RTC_set_flg;

const uint8_t ERTC_dat_reg[] = {
  REG_SECONDS,
  REG_MINUTES,
  REG_HOURS,
  REG_DAY_OF_MONTH,
  REG_DAY_OF_WEEK,
  REG_MONTH,
  REG_YEAR,
};

/* Declaration of static local variables */

/* Declaration of local functions */

/*
 * **************************************
 * **************************************
 * Function Declarations start here
 * **************************************
 * **************************************
 */

/*
 * *****************************************************************************
 * Function name: LRTC_init()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to initialize the Local RTC module.
 * Parameters:
 *  Input :  None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void  LRTC_init(void)
{
  // Initialization of the calendar time
  if ((HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN) == HAL_OK) &&
  	  (HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN) == HAL_OK))
  {
    // Time & Date initialized
  }
  else
  {
    // Not able to get date and time
    HAL_RTC_Init(&hrtc);
  }
}

/*
 * *****************************************************************************
 * Function name: Set_TimeDate()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to set the default date and time to the RTC module
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *  Global structures "Time" and "Date" are updated
 * *****************************************************************************
 */
void Set_TimeDate(void)
{
  if(UI_RTC_set_flg)
  {

	  if(vp_param_rw_str.RTC_TimeFormat==0)
	  {
		  Time.Hours 		= vp_param_rw_str.RTC_Hours12Settings;
		  Time.Minutes 		= vp_param_rw_str.RTC_MinutesSettings;
		  Time.TimeFormat 	= vp_param_rw_str.RTC_AMPMSettings;
	  }
	  else
	  {
		  if(vp_param_rw_str.RTC_HoursSettings==0)
		  Time.Hours 		= (vp_param_rw_str.RTC_HoursSettings+12);
		  else if(vp_param_rw_str.RTC_HoursSettings>12)
		  Time.Hours 		= (vp_param_rw_str.RTC_HoursSettings-12);
		  else
		  Time.Hours 		= (vp_param_rw_str.RTC_HoursSettings);
		  Time.Minutes 		= vp_param_rw_str.RTC_MinutesSettings;

		  Time.TimeFormat 	= vp_param_rw_str.RTC_AMPMSettings;
		  /*if((vp_param_rw_str.RTC_HoursSettings>=12)&&(vp_param_rw_str.RTC_HoursSettings!=0))
		  Time.TimeFormat 	= 1;
		  else
		  Time.TimeFormat 	= 0;*/
	  }
	  Time.Seconds 		= 20;
	  Date.Year 	= (vp_param_rw_str.RTC_YearSettings);//-2000
	  Date.Date 	= vp_param_rw_str.RTC_DateSettings;
	  Date.Month 	= vp_param_rw_str.RTC_MonthSettings;
  }
  //PWR->CR1 |= PWR_CR1_DBP;    // Access to RTC registers enabled
  if ((HAL_RTC_SetTime(&hrtc, &Time, RTC_FORMAT_BIN) == HAL_OK) &&
	  (HAL_RTC_SetDate(&hrtc, &Date, RTC_FORMAT_BIN) == HAL_OK))
  {
    // Default time is set successfully
  }
  //PWR->CR1 &= ~PWR_CR1_DBP;    // Access to RTC registers disabled
}

/*
 * *****************************************************************************
 * Function name: Get_TimeStamp()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to get the time stamp (Date and Time)
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *  Global structures "Time" and "Date" are updated
 * *****************************************************************************
 */
void Get_TimeStamp(void)
{
  if ((HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN) == HAL_OK) &&
      (HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN) == HAL_OK))
  {
    // Real time/date is read successfully
  }

}

/*
 * *********************************
 * *********************************
 * End of the module: RTC.c
 * *********************************
 * *********************************
 */

