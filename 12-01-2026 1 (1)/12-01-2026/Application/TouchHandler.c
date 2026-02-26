/*
 * TouchHandler.c
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#include "MyMain.h"
#include "DisplayCmdHandlers.h"
#include "TouchHandler.h"


uint8_t test_alarm_flg;
uint8_t LED_disconnect=0;

/*
 * *****************************************************************************
 * Function name: TemperatureLimitCheck
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TemperatureLimitCheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t TempLimitValue = *(DataInPtr) ;
	if((VPAddr == T1_HIGH_LIMIT_VALUE_RD)||(VPAddr == T2_HIGH_LIMIT_VALUE_RD))
	{
		if(UITempeSettingsData.TempUnitSelection==0)
		{
			if((TempLimitValue <= 000 && TempLimitValue >= -1900))
			{
				if(VPAddr == T1_HIGH_LIMIT_VALUE_RD)
				{
					if(TempLimitValue<=UITempeSettingsData.T1LowLimit)
					{
						TempSettingsConv.T1HighLimitFl = (vp_param_rw_str.high_temp1.value*10);
						UITempeSettingsData.T1HighLimit = (int16_t)TempSettingsConv.T1HighLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1HighLimit);
					}
					else
					{
						TempSettingsConv.T1HighLimitFl = UITempeSettingsData.T1HighLimit;
					}
				}
				else if(VPAddr == T2_HIGH_LIMIT_VALUE_RD)
				{
					if(TempLimitValue<=UITempeSettingsData.T2LowLimit)
					{
						TempSettingsConv.T2HighLimitFl = (vp_param_rw_str.high_temp2.value*10);
						UITempeSettingsData.T2HighLimit = (int16_t)TempSettingsConv.T2HighLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2HighLimit);
					}
					else
					{
						TempSettingsConv.T2HighLimitFl = UITempeSettingsData.T2HighLimit;
					}
				}
			}
			else
			{
				if(VPAddr == T1_HIGH_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T1HighLimitFl = (vp_param_rw_str.high_temp1.value*10);
					UITempeSettingsData.T1HighLimit = (int16_t)TempSettingsConv.T1HighLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1HighLimit);
				}
				else if(VPAddr == T2_HIGH_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T2HighLimitFl = (vp_param_rw_str.high_temp2.value*10);
					UITempeSettingsData.T2HighLimit = (int16_t)TempSettingsConv.T2HighLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2HighLimit);
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
		}
		else
		{
			if((TempLimitValue <= 320 && TempLimitValue >= -3100))
			{
				if(VPAddr == T1_HIGH_LIMIT_VALUE_RD)
				{
					if(TempLimitValue<=UITempeSettingsData.T1LowLimit)
					{
						TempSettingsConv.T1HighLimitFl = (((vp_param_rw_str.high_temp1.value*1.8)+32)*10);
						UITempeSettingsData.T1HighLimit = (int16_t)TempSettingsConv.T1HighLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1HighLimit);
					}
					else
					{
						TempSettingsConv.T1HighLimitFl = UITempeSettingsData.T1HighLimit;
					}
				}
				else if(VPAddr == T2_HIGH_LIMIT_VALUE_RD)
				{
					if(TempLimitValue<=UITempeSettingsData.T2LowLimit)
					{
						TempSettingsConv.T2HighLimitFl = (((vp_param_rw_str.high_temp2.value*1.8)+32)*10);
						UITempeSettingsData.T2HighLimit = (int16_t)TempSettingsConv.T2HighLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2HighLimit);
					}
					else
					{
						TempSettingsConv.T2HighLimitFl = UITempeSettingsData.T2HighLimit;
					}
				}
			}
			else
			{
				if(VPAddr == T1_HIGH_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T1HighLimitFl = (((vp_param_rw_str.high_temp1.value*1.8)+32)*10);
					UITempeSettingsData.T1HighLimit = (int16_t)TempSettingsConv.T1HighLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1HighLimit);
				}
				else if(VPAddr == T2_HIGH_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T2HighLimitFl = (((vp_param_rw_str.high_temp2.value*1.8)+32)*10);
					UITempeSettingsData.T2HighLimit = (int16_t)TempSettingsConv.T2HighLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2HighLimit);
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
		}
	}
	else if((VPAddr == T1_LOW_LIMIT_VALUE_RD)||(VPAddr == T2_LOW_LIMIT_VALUE_RD))
	{
		if(UITempeSettingsData.TempUnitSelection==0)
		{
			if((TempLimitValue <= -1500 && TempLimitValue >= -2200))
			{
				if(VPAddr == T1_LOW_LIMIT_VALUE_RD)
				{
					if(TempLimitValue>=UITempeSettingsData.T1HighLimit)
					{
						TempSettingsConv.T1LowLimitFl = (vp_param_rw_str.low_temp1.value*10);
						UITempeSettingsData.T1LowLimit = (int16_t)TempSettingsConv.T1LowLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1LowLimit);
					}
					else
					{
						TempSettingsConv.T1LowLimitFl = UITempeSettingsData.T1LowLimit;
					}
				}
				else if(VPAddr == T2_LOW_LIMIT_VALUE_RD)
				{
					if(TempLimitValue>=UITempeSettingsData.T2HighLimit)
					{
						TempSettingsConv.T2LowLimitFl = (vp_param_rw_str.low_temp2.value*10);
						UITempeSettingsData.T2LowLimit = (int16_t)TempSettingsConv.T2LowLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2LowLimit);
					}
					else
					{
						TempSettingsConv.T2LowLimitFl = UITempeSettingsData.T2LowLimit;
					}
				}
			}
			else
			{
				if(VPAddr == T1_LOW_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T1LowLimitFl = (vp_param_rw_str.low_temp1.value*10);
					UITempeSettingsData.T1LowLimit = (int16_t)TempSettingsConv.T1LowLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1LowLimit);
				}
				else if(VPAddr == T2_LOW_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T2LowLimitFl = (vp_param_rw_str.low_temp2.value*10);
					UITempeSettingsData.T2LowLimit = (int16_t)TempSettingsConv.T2LowLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2LowLimit);
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
		}
		else
		{
			if((TempLimitValue <= -2380 && TempLimitValue >= -3640))
			{
				if(VPAddr == T1_LOW_LIMIT_VALUE_RD)
				{
					if(TempLimitValue>=UITempeSettingsData.T1HighLimit)
					{
						TempSettingsConv.T1LowLimitFl = (((vp_param_rw_str.low_temp1.value*1.8)+32)*10);
						UITempeSettingsData.T1LowLimit = (int16_t)TempSettingsConv.T1LowLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1LowLimit);
					}
					else
					{
						TempSettingsConv.T1LowLimitFl = UITempeSettingsData.T1LowLimit;
					}
				}
				else if(VPAddr == T2_LOW_LIMIT_VALUE_RD)
				{
					if(TempLimitValue>=UITempeSettingsData.T2HighLimit)
					{
						TempSettingsConv.T2LowLimitFl = (((vp_param_rw_str.low_temp2.value*1.8)+32)*10);
						UITempeSettingsData.T2LowLimit = (int16_t)TempSettingsConv.T2LowLimitFl;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
						SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2LowLimit);
					}
					else
					{
						TempSettingsConv.T2LowLimitFl = UITempeSettingsData.T2LowLimit;
					}
				}
			}
			else
			{
				if(VPAddr == T1_LOW_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T1LowLimitFl = (((vp_param_rw_str.low_temp1.value*1.8)+32)*10);
					UITempeSettingsData.T1LowLimit = (int16_t)TempSettingsConv.T1LowLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T1LowLimit);
				}
				else if(VPAddr == T2_LOW_LIMIT_VALUE_RD)
				{
					TempSettingsConv.T2LowLimitFl = (((vp_param_rw_str.low_temp2.value*1.8)+32)*10);
					UITempeSettingsData.T2LowLimit = (int16_t)TempSettingsConv.T2LowLimitFl;
					SendN16DataToDisplay(VPAddr-0x500, UITempeSettingsData.T2LowLimit);
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
		}
	}

}
/*
 * *****************************************************************************
 * Function name: TemperatureUnitConvHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TemperatureUnitConvHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(PrevTempUnit == UITempeSettingsData.TempUnitSelection)
		return;
	else if(UITempeSettingsData.TempUnitSelection)
	{
		TempSettingsConv.T1HighLimitFl = (TempSettingsConv.T1HighLimitFl * 1.8 ) + 320;
		TempSettingsConv.T1LowLimitFl = (TempSettingsConv.T1LowLimitFl * 1.8 ) + 320;
		TempSettingsConv.T2HighLimitFl = (TempSettingsConv.T2HighLimitFl * 1.8 ) + 320;
		TempSettingsConv.T2LowLimitFl = (TempSettingsConv.T2LowLimitFl * 1.8 ) + 320;
	}
	else
	{
		TempSettingsConv.T1HighLimitFl = (TempSettingsConv.T1HighLimitFl -320) *0.5555556;
		TempSettingsConv.T1LowLimitFl = (TempSettingsConv.T1LowLimitFl -320) *0.5555556;
		TempSettingsConv.T2HighLimitFl = (TempSettingsConv.T2HighLimitFl -320) *0.5555556;
		TempSettingsConv.T2LowLimitFl = (TempSettingsConv.T2LowLimitFl -320) *0.5555556;

	}

	UITempeSettingsData.T1HighLimit = (int16_t)TempSettingsConv.T1HighLimitFl;  // Fahrenheit
	UITempeSettingsData.T1LowLimit = (int16_t)TempSettingsConv.T1LowLimitFl;
	UITempeSettingsData.T2HighLimit = (int16_t)TempSettingsConv.T2HighLimitFl;
	UITempeSettingsData.T2LowLimit = (int16_t)TempSettingsConv.T2LowLimitFl;

	PrevTempUnit = UITempeSettingsData.TempUnitSelection;
	SendN16DataToDisplay(T1_HIGH_LIMIT_VALUE_WR,(int16_t)UITempeSettingsData.T1HighLimit);
	SendN16DataToDisplay(T1_LOW_LIMIT_VALUE_WR,(int16_t)UITempeSettingsData.T1LowLimit);
	SendN16DataToDisplay(T2_HIGH_LIMIT_VALUE_WR,(int16_t)UITempeSettingsData.T2HighLimit);
	SendN16DataToDisplay(T2_LOW_LIMIT_VALUE_WR,(int16_t)UITempeSettingsData.T2LowLimit);
}

/*
 * *****************************************************************************
 * Function name: TestAlarmCtrl
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TestAlarmCtrl(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t SwitchValue = *(DataInPtr) ;
	test_alarm_flg=(SwitchValue&0x01);
	if(SwitchValue)
	{
		HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	}
	else
	{
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	}
}

void Led_britenessCtrl(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t SwitchValue = *(DataInPtr) ;
	if(LED_disconnect)
	{
		if (PCA9955B_Init(PCA9955B_ADDR) != HAL_OK)
		{
			return ;
		}
		if (PCA9955B_Init(PCA9955B_ADDR2) != HAL_OK)
		{
            return;
		}
		PCA9955B_SetHighFrequencyMode();
		PCA9955B_SetGlobalBrightness(SwitchValue);
	}
	else
	{
		PCA9955B_SetGlobalBrightness(SwitchValue);
	}

}
/*
 * *****************************************************************************
 * Function name: CalibProcessControlHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void CalibProcessControlHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(VPAddr == DP_2POINT_CAL1_VALUE_RD)
	{
		if(vp_cal_pt_rd_str.IsSinglePointCalibration)
		{
			if((vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD1))
			{
				if(UITempeSettingsData.TempUnitSelection)
				UISensorCalibData.DP2PointValue2 = (int16_t)((((float)UISensorCalibData.DP2PointValue1-320.0)/18.0)*10.0);
				else
				UISensorCalibData.DP2PointValue2=UISensorCalibData.DP2PointValue1;
				UISensorCalibData.DP2PointValue1 = 0;
				cal_pt_str[CALIB_PARAM_RTD1].y[0] = 8192.0;
				if((UISensorCalibData.DP2PointValue2 <= 50 && UISensorCalibData.DP2PointValue2 >= -2200))
				{
					vp_cal_pt_rd_str.rtd1_param[0] = (float)UISensorCalibData.DP2PointValue1/10.0;
					vp_cal_pt_rd_str.rtd1_param[1] = (float)UISensorCalibData.DP2PointValue2/10.0;
					return;
				}
				else
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
				}
			}
			else if((vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD2))
			{
				if(UITempeSettingsData.TempUnitSelection)
				UISensorCalibData.DP2PointValue2 = (int16_t)((((float)UISensorCalibData.DP2PointValue1-320.0)/18.0)*10.0);
				else
				UISensorCalibData.DP2PointValue2=UISensorCalibData.DP2PointValue1;
				UISensorCalibData.DP2PointValue1 = 0;
				cal_pt_str[CALIB_PARAM_RTD2].y[0] = 8192.0;
				if((UISensorCalibData.DP2PointValue2 <= 50 && UISensorCalibData.DP2PointValue2 >= -2200))
				{
					vp_cal_pt_rd_str.rtd2_param[0] = (float)UISensorCalibData.DP2PointValue1/10.0;
					vp_cal_pt_rd_str.rtd2_param[1] = (float)UISensorCalibData.DP2PointValue2/10.0;
					return;
				}
				else
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
				}
			}
		}
		else if((vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD1)||(vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD2))
		{
			if(UITempeSettingsData.TempUnitSelection)
			UISensorCalibData.DP2PointValue1 = (int16_t)((((float)UISensorCalibData.DP2PointValue1-320.0)/18.0)*10.0);

			if((UISensorCalibData.DP2PointValue1 <= 50 && UISensorCalibData.DP2PointValue1 >= -2200))
			{
				//vp_cal_pt_rd_str.step_num = 1;
				vp_cal_pt_rd_str.WaitForInput = 2;
			}
			else
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
			}
		}
	}
	else if(VPAddr == DP_2POINT_CAL2_VALUE_RD)
	{
		if((vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD1))
		{
			if(UITempeSettingsData.TempUnitSelection)
			UISensorCalibData.DP2PointValue2 = (int16_t)((((float)UISensorCalibData.DP2PointValue2-320.0)/18.0)*10.0);

			if((UISensorCalibData.DP2PointValue2 <= 250 && UISensorCalibData.DP2PointValue2 >= -2200))
			{
				vp_cal_pt_rd_str.rtd1_param[0] = (float)UISensorCalibData.DP2PointValue1/10.0;
				vp_cal_pt_rd_str.rtd1_param[1] = (float)UISensorCalibData.DP2PointValue2/10.0;
				return;
			}
			else
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
			}
		}
		else if((vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD2))
		{
			if(UITempeSettingsData.TempUnitSelection)
			UISensorCalibData.DP2PointValue2 = (int16_t)((((float)UISensorCalibData.DP2PointValue2-320.0)/18.0)*10.0);

			if((UISensorCalibData.DP2PointValue2 <= 250 && UISensorCalibData.DP2PointValue2 >= -2200))
			{
				vp_cal_pt_rd_str.rtd2_param[0] = (float)UISensorCalibData.DP2PointValue1/10.0;
				vp_cal_pt_rd_str.rtd2_param[1] = (float)UISensorCalibData.DP2PointValue2/10.0;
				return;
			}
			else
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
			}
		}
	}
}

//DAY selection
/* Function Declarations start here */
/*
 * *****************************************************************************
 * Function name: daily
 * Created by : Arikanti venkatesh
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the data entry events
 * Parameters:
 *  Input  : RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void daily(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t selection = (*(DataPtr+8) << 8) | *(DataPtr+9);
	if(VPAddr==SCHEDULED_FILL_DAILY_RD)
	{
		if(selection==0)
		{
			UIScheduledFillData.DaySettings &= ~Daily;
			SendN16DataToDisplay(SCHEDULED_FILL_EN_DIS_WEEKDAYS, 1);
			SendN16DataToDisplay(VPAddr-500, selection);
		}
		else
		{

			UIScheduledFillData.DaySettings =0x00;
			UIScheduledFillData.DaySettings |=Daily;
			SendN16DataToDisplay(SCHEDULED_FILL_EN_DIS_WEEKDAYS, 0);
			SendVPDataCmdToDisplay(Day_shed);
			SendVPDataCmdToDisplay(Day_shed1);
		}
	}
	else if(VPAddr==SCHEDULED_FILL_MONDAY_RD)
	{
		if(selection==0)
		{
			//day_sel[1]=0;
			UIScheduledFillData.DaySettings &= ~Monday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
		else
		{
			//day_sel[1]=1;
			UIScheduledFillData.DaySettings |= Monday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
	}
	else if(VPAddr==SCHEDULED_FILL_TUESDAY_RD)
	{
		if(selection==0)
		{

			UIScheduledFillData.DaySettings &= ~Tuesday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
		else
		{

			UIScheduledFillData.DaySettings |= Tuesday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
	}
	else if(VPAddr==SCHEDULED_FILL_WEDNESDAY_RD)
	{
		if(selection==0)
		{

			UIScheduledFillData.DaySettings &= ~Wednesday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
		else
		{
			UIScheduledFillData.DaySettings |= Wednesday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
	}
	else if(VPAddr==SCHEDULED_FILL_THURSDAY_RD)
	{
		if(selection==1)
		{
			UIScheduledFillData.DaySettings |= Thursday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
		else
		{
			UIScheduledFillData.DaySettings &= ~Thursday;
			SendN16DataToDisplay(VPAddr-500, selection);

		}
	}
	else if(VPAddr==SCHEDULED_FILL_FRIDAY_RD)
	{
		if(selection==1)
		{
			UIScheduledFillData.DaySettings |= Friday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
		else
		{
			SendN16DataToDisplay(VPAddr-500, selection);
			UIScheduledFillData.DaySettings &= ~Friday;
		}
	}
	else if(VPAddr==SCHEDULED_FILL_SATURDAY_RD)
	{
		if(selection==1)
		{
			UIScheduledFillData.DaySettings |= Saturday;
			SendN16DataToDisplay(VPAddr-500, selection);

		}
		else
		{
			UIScheduledFillData.DaySettings &= ~Saturday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
	}
	else if(VPAddr==SCHEDULED_FILL_SUNDAY_RD)
	{
		if(selection==1)
		{
			UIScheduledFillData.DaySettings |= Sunday;
			SendN16DataToDisplay(VPAddr-500, selection);

		}
		else
		{
			UIScheduledFillData.DaySettings &= ~Sunday;
			SendN16DataToDisplay(VPAddr-500, selection);
		}
	}
}

/*
 * *****************************************************************************
 * Function name: TimeFormatHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TimeFormatHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{

	if(PrevTimeFormat == UIDateTimeSettings.TimeFormat)
		return;
	if(UIDateTimeSettings.TimeFormat==0)
	{
		//Set Date and Time for RTC Sync
		if(UIDateTimeSettings.HoursSettings==0)
		UIDateTimeSettings.Hours12Settings 		= (UIDateTimeSettings.HoursSettings+12);
		else if(UIDateTimeSettings.HoursSettings>12)
		UIDateTimeSettings.Hours12Settings 		= (UIDateTimeSettings.HoursSettings-12);
		else
		UIDateTimeSettings.Hours12Settings 		= (UIDateTimeSettings.HoursSettings);

	//	UIDateTimeSettings.Minutes12Settings	= UIDateTimeSettings.MinutesSettings;
		if(UIDateTimeSettings.HoursSettings>=12)
		UIDateTimeSettings.AMPMSettings			= 1;
		else
		UIDateTimeSettings.AMPMSettings			= 0;
	}
	else
	{
		//Set Date and Time for RTC Sync
		if(UIDateTimeSettings.AMPMSettings==1)
		{
			if(UIDateTimeSettings.Hours12Settings<12)
			UIDateTimeSettings.HoursSettings	= (UIDateTimeSettings.Hours12Settings+12);
			else
			UIDateTimeSettings.HoursSettings	= (UIDateTimeSettings.Hours12Settings);
		}
		else
		{
			if(UIDateTimeSettings.Hours12Settings>=12)
			UIDateTimeSettings.HoursSettings	= (UIDateTimeSettings.Hours12Settings-12);
			else
			UIDateTimeSettings.HoursSettings	= (UIDateTimeSettings.Hours12Settings);
		}

	}
	PrevTimeFormat = UIDateTimeSettings.TimeFormat;

	SendN16DataToDisplay(RTC_12HOURS_VALUE_WR,(int16_t)UIDateTimeSettings.Hours12Settings);
	SendN16DataToDisplay(RTC_AMPM_VALUE_WR,(int16_t)UIDateTimeSettings.AMPMSettings);
	SendN16DataToDisplay(RTC_HOURS_VALUE_WR,(int16_t)UIDateTimeSettings.HoursSettings);
	SendN16DataToDisplay(RTC_MIN_VALUE_WR,(int16_t)UIDateTimeSettings.MinutesSettings);
}

/*
 * *****************************************************************************
 * Function name: MonthDateLimitHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void MonthDateLimitHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t MonthValue = (*(DataPtr+8) << 8) | *(DataPtr+9);
	uint8_t Lp;

	if(VPAddr == RTC_MONTH_VALUE_RD)
	{
		if((MonthValue==4)||(MonthValue==6)||(MonthValue==9)||(MonthValue==11))
		{
			if(UIDateTimeSettings.DateSettings>30)
			{
				UIDateTimeSettings.DateSettings=30;
				SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
			}
		}
		else if(MonthValue==2)
		{
			Lp = (UIDateTimeSettings.YearSettings%4);

			if(Lp==0)
			{
				if(UIDateTimeSettings.DateSettings>29)
				{
					UIDateTimeSettings.DateSettings=29;
					SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
				}
			}
			else
			{
				if(UIDateTimeSettings.DateSettings>28)
				{
					UIDateTimeSettings.DateSettings=28;
					SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
				}
			}
		}
	}
	else if(VPAddr == MAINT_MONTH_VALUE_RD)
	{
		if((MonthValue==4)||(MonthValue==6)||(MonthValue==9)||(MonthValue==11))
		{
			if(UIMaintenanceRemainderSettings.DateSettings>30)
			{
				UIMaintenanceRemainderSettings.DateSettings=30;
				SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
			}
		}
		else if(MonthValue==2)
		{
			Lp = (UIMaintenanceRemainderSettings.YearSettings%4);

			if(Lp==0)
			{
				if(UIMaintenanceRemainderSettings.DateSettings>29)
				{
					UIMaintenanceRemainderSettings.DateSettings=29;
					SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
				}
			}
			else
			{
				if(UIMaintenanceRemainderSettings.DateSettings>28)
				{
					UIMaintenanceRemainderSettings.DateSettings=28;
					SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
				}
			}
		}
	}
	else if(VPAddr == UPDATE_MONTH_VALUE_RD)
	{
		if((MonthValue==4)||(MonthValue==6)||(MonthValue==9)||(MonthValue==11))
		{
			if(UIUpdateRemaindersettings.DateSettings>30)
			{
				UIUpdateRemaindersettings.DateSettings=30;
				SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
			}
		}
		else if(MonthValue==2)
		{
			Lp = (UIUpdateRemaindersettings.YearSettings%4);

			if(Lp==0)
			{
				if(UIUpdateRemaindersettings.DateSettings>29)
				{
					UIUpdateRemaindersettings.DateSettings=29;
					SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
				}
			}
			else
			{
				if(UIUpdateRemaindersettings.DateSettings>28)
				{
					UIUpdateRemaindersettings.DateSettings=28;
					SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
				}
			}
		}
	}
}

/*
 * *****************************************************************************
 * Function name: DateLimitHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void DateLimitHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t DateValue = (*(DataPtr+8) << 8) | *(DataPtr+9);
	uint8_t Lp;

	if(VPAddr == RTC_DATE_VALUE_RD)
	{
		if((UIDateTimeSettings.MonthSettings==4)||(UIDateTimeSettings.MonthSettings==6)||(UIDateTimeSettings.MonthSettings==9)||(UIDateTimeSettings.MonthSettings==11))
		{
			if(DateValue>30)
			{
				UIDateTimeSettings.DateSettings=30;
				SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
			}
		}
		else if(UIDateTimeSettings.MonthSettings==2)
		{
			Lp = (UIDateTimeSettings.YearSettings%4);

			if(Lp==0)
			{
				if(DateValue>29)
				{
					UIDateTimeSettings.DateSettings=29;
					SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
				}
			}
			else
			{
				if(DateValue>28)
				{
					UIDateTimeSettings.DateSettings=28;
					SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
				}
			}
		}
	}
	else if(VPAddr == MAINT_DATE_VALUE_RD)
	{
		if((UIMaintenanceRemainderSettings.MonthSettings==4)||(UIMaintenanceRemainderSettings.MonthSettings==6)||(UIMaintenanceRemainderSettings.MonthSettings==9)||(UIMaintenanceRemainderSettings.MonthSettings==11))
		{
			if(DateValue>30)
			{
				UIMaintenanceRemainderSettings.DateSettings=30;
				SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
			}
		}
		else if(UIMaintenanceRemainderSettings.MonthSettings==2)
		{
			Lp = (UIMaintenanceRemainderSettings.YearSettings%4);

			if(Lp==0)
			{
				if(DateValue>29)
				{
					UIMaintenanceRemainderSettings.DateSettings=29;
					SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
				}
			}
			else
			{
				if(DateValue>28)
				{
					UIMaintenanceRemainderSettings.DateSettings=28;
					SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
				}
			}
		}
	}
	else if(VPAddr == UPDATE_DATE_VALUE_WD)
	{
		if((UIUpdateRemaindersettings.MonthSettings==4)||(UIUpdateRemaindersettings.MonthSettings==6)||(UIMaintenanceRemainderSettings.MonthSettings==9)||(UIMaintenanceRemainderSettings.MonthSettings==11))
		{
			if(DateValue>30)
			{
				UIUpdateRemaindersettings.DateSettings=30;
				SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
			}
		}
		else if(UIUpdateRemaindersettings.MonthSettings==2)
		{
			Lp = (UIUpdateRemaindersettings.YearSettings%4);

			if(Lp==0)
			{
				if(DateValue>29)
				{
					UIUpdateRemaindersettings.DateSettings=29;
					SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
				}
			}
			else
			{
				if(DateValue>28)
				{
					UIUpdateRemaindersettings.DateSettings=28;
					SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Date!!!");
				}
			}
		}
	}
}

/*
 * *****************************************************************************
 * Function name: YearDateLimitHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void YearDateLimitHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t YearValue = (*(DataPtr+8) << 8) | *(DataPtr+9);
	uint8_t Lp;

	if(VPAddr == RTC_YEAR_VALUE_RD)
	{
		if((UIDateTimeSettings.MonthSettings==4)||(UIDateTimeSettings.MonthSettings==6)||(UIDateTimeSettings.MonthSettings==9)||(UIDateTimeSettings.MonthSettings==11))
		{
			if(UIDateTimeSettings.DateSettings>30)
			{
				UIDateTimeSettings.DateSettings=30;
				SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
			}
		}
		else if(UIDateTimeSettings.MonthSettings==2)
		{
			Lp = (YearValue%4);

			if(Lp==0)
			{
				if(UIDateTimeSettings.DateSettings>29)
				{
					UIDateTimeSettings.DateSettings=29;
					SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
				}
			}
			else
			{
				if(UIDateTimeSettings.DateSettings>28)
				{
					UIDateTimeSettings.DateSettings=28;
					SendN16DataToDisplay(RTC_DATE_VALUE_WR, UIDateTimeSettings.DateSettings);
				}
			}
		}
	}
	else if(VPAddr == MAINT_YEAR_VALUE_RD)
	{
		if((UIMaintenanceRemainderSettings.MonthSettings==4)||(UIMaintenanceRemainderSettings.MonthSettings==6)||(UIMaintenanceRemainderSettings.MonthSettings==9)||(UIMaintenanceRemainderSettings.MonthSettings==11))
		{
			if(UIMaintenanceRemainderSettings.DateSettings>30)
			{
				UIMaintenanceRemainderSettings.DateSettings=30;
				SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
			}
		}
		else if(UIMaintenanceRemainderSettings.MonthSettings==2)
		{
			Lp = (YearValue%4);

			if(Lp==0)
			{
				if(UIMaintenanceRemainderSettings.DateSettings>29)
				{
					UIMaintenanceRemainderSettings.DateSettings=29;
					SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);
				}
			}
			else
			{
				if(UIMaintenanceRemainderSettings.DateSettings>28)
				{
					UIMaintenanceRemainderSettings.DateSettings=28;
					SendN16DataToDisplay(0x800F4, UIMaintenanceRemainderSettings.DateSettings);		//80322
				}
			}
		}
	}
	else if(VPAddr == UPDATE_YEAR_VALUE_RD)
	{
		if((UIUpdateRemaindersettings.MonthSettings==4)||(UIUpdateRemaindersettings.MonthSettings==6)||(UIUpdateRemaindersettings.MonthSettings==9)||(UIUpdateRemaindersettings.MonthSettings==11))
		{
			if(UIUpdateRemaindersettings.DateSettings>30)
			{
				UIUpdateRemaindersettings.DateSettings=30;
				SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);		//8034E
			}
		}
		else if(UIUpdateRemaindersettings.MonthSettings==2)
		{
			Lp = (YearValue%4);

			if(Lp==0)
			{
				if(UIUpdateRemaindersettings.DateSettings>29)
				{
					UIUpdateRemaindersettings.DateSettings=29;
					SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
				}
			}
			else
			{
				if(UIUpdateRemaindersettings.DateSettings>28)
				{
					UIUpdateRemaindersettings.DateSettings=28;
					SendN16DataToDisplay(UPDATE_DATE_VALUE_WR, UIUpdateRemaindersettings.DateSettings);
				}
			}
		}
	}
}

/*
 * *****************************************************************************
 * Function name: FreezerIdHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void FreezerIdHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(FrezzID, 0, sizeof(FrezzID));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				FrezzID[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				SendSTRDataToDisplay(FREEZER_ID_VALUE_RD,(uint8_t*)"\0");
				memcpy((uint8_t*)&FrezzID, (uint8_t*)&UIFreezerStruct.FreezerID, sizeof(FrezzID));
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(FREEZER_ID_VALUE_WR,(uint8_t*)&FrezzID);
		SendSTRDataToDisplay(FREEZER_ID_VALUE_RD,(uint8_t*)&FrezzID);
	}
}

/*
 * *****************************************************************************
 * Function name: RefreshGraph
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void RefreshGraph(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	//SendGraphData();
}
/*
 * *****************************************************************************
 * Function name: IOT_Connectivity
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void IOT_Connectivity(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	 if(send_email_iot==1)
	 {
	    //wifi_info.state = WIFI_IOT_TEST;
	    send_test_mail=1;
	    SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
	 }
}

/*
 * *****************************************************************************
 * Function name: Supply_con_Handler
 * Created by : Arikanti Venkatesh
 * Date:        14-10-2025
 * Description:
 *  Main function to Load Groupsettings
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Supply_con_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIUpdateSystemSettings.Supply_con, 0, sizeof(UIUpdateSystemSettings.Supply_con));   // Erase
	for(i=0;i<20;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]==0x2D))||(DataPtr[8+i]==0x2B)||(DataPtr[8+i]==0x20))
			{
				UIUpdateSystemSettings.Supply_con[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (-)( )(+)(0-9) Only !!!");
				SendSTRDataToDisplay(SUPLY_CONTACT_RD,(uint8_t*)"\0");
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(SUPLY_CONTACT_WR,(uint8_t*)&UIUpdateSystemSettings.Supply_con);
		SendSTRDataToDisplay(SUPLY_CONTACT_RD,(uint8_t*)"\0");
	}
}
/*
 * *****************************************************************************
 * Function name: Service_con_Handler
 * Created by : Arikanti Venkatesh
 * Date:        14-10-2025
 * Description:
 *  Main function to Load Groupsettings
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Service_con_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIUpdateSystemSettings.Service_con, 0, sizeof(UIUpdateSystemSettings.Service_con));   // Erase
	for(i=0;i<20;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]==0x2D))||(DataPtr[8+i]==0x2B)||(DataPtr[8+i]==0x20))
			{
				UIUpdateSystemSettings.Service_con[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (-)( )(+)(0-9) Only !!!");
				SendSTRDataToDisplay(SERVICE_CONTACT_RD,(uint8_t*)"\0");
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(SERVICE_CONTACT_WR,(uint8_t*)&UIUpdateSystemSettings.Service_con);
		SendSTRDataToDisplay(SERVICE_CONTACT_RD,(uint8_t*)"\0");
	}
}





