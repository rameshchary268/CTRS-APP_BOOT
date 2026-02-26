/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: DisplayTxHandlers.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains funtions to handle the transmit data to LCD module
 *
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DisplayTxHandlers.h"
#include "DispVPAddrList.h"
#include "TopwayCmdList.h"
#include "DisplayCommDriver.h"

/* Function Declarations */

/* Declaration of external global variables */
//extern NODE_INFO  node_info[MB_MAX_NODES];

uint8_t CalibDataReqSent = 0;
uint8_t Rtc_Toggle;
uint8_t RunTimeDataReqSent = 0;
uint8_t UserActionEventDataReqSent = 0;
uint8_t LoginDataReqSent = 0;
SensorReadingStruct SensorReading;
CTRSStruct    Ctrsreading;
TemperatureSettingsStruct TemperatureSettings;
LEVELSettingsStruct   LevelSettings;
LEDSettingsStruct LedSettings;
AlarmSettingsStruct AlarmSettings;
LidFunctionsStruct LidFunctions;
DisplayDataStruct DispSettings;
ModbusDataStruct ModbusSettings;
ScheduledEventStruct ScheduledSettings;
Daily_sel Daily_data;

RemainderSettingsStruct ScheduledFillSettings;
SCHRemainderSettingsStruct ScheduledRLevelSettings;
RemainderSettingsStruct ScheduledLN2SupplySettings;

MaintenanceRemainderSettingsStruct MaintenanceRemainderSettings;


DateTimeSettingsStruct DateTimeSettings;
UpdateGroupSettingsStruct  UpdateGroupsettings;
UIVPStruct	UIInitVps;
SystemInfoStruct SystemInfo;
uint16_t MUTE_TIME_DELAY;
uint16_t MUTE_REM_TIME_DELAY;
int16_t Group_inactive_nodes[32];

UpdateWifiEn updatewifien;
UPDATEFILEEn updatefileen;
UPDATEFILETYPE updatefiletypeen;
FILE_SELECTION File_read_write;
FILE_SIZE_SELECTION File_Size;

uint8_t TargetPage = 0;
extern double mTorr;
extern SSID_Details Wifi_ssid[11];
/* Function Declarations start here */

/*
 * *****************************************************************************
 * Function name: SwapBytes
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to swap the bytes
 * Parameters:
 *  Input  : date to swap
 *  Return : swapped data
 * Note:
 *
 * *****************************************************************************
 */
int16_t SwapBytes(int16_t Data)
{
	uint16_t SwapData = (uint16_t)Data;
	uint16_t Swapped = SwapData>>8;
	Swapped |= (SwapData & 0x00FF) << 8;
	return (int16_t)Swapped;
}
int32_t Swap32Bytes(int32_t Data)
{
	uint32_t SwapData = (uint32_t)Data;
	uint32_t Swapped = SwapData>>24;
	Swapped |= ((SwapData>>16) & 0x00ff) << 8;
	Swapped |= ((SwapData>>8) & 0x00FF) << 16;
	Swapped |= (SwapData & 0x00FF) << 24;
	return (int32_t)Swapped;
}
/*
 * *****************************************************************************
 * Function name: DispHandshakeCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Handshake command, will be called during Init
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispHandshakeCmd( uint8_t *DataPtr)
{
	//No data to be added
	return 0;
}
/*
 * *****************************************************************************
 * Function name: DispSetSysConfigCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame system config command, will be called during Init
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispSetSysConfigCmd( uint8_t *DataPtr)
{
	const uint8_t TFT_EN_IF_PROC_FN[] = {0x55, 0xAA, 0x5A, 0xA5, BAUD_SET, TP_PARAM, 0x00}; // Config

	memcpy(DataPtr, TFT_EN_IF_PROC_FN, sizeof(TFT_EN_IF_PROC_FN));

	return sizeof(TFT_EN_IF_PROC_FN);
}

/*
 * *****************************************************************************
 * Function name: DispSetScreenSaverCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame screen saver command, will be called during Init
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispSetScreenSaverCmd( uint8_t *DataPtr)
{
	/*const uint8_t TFT_SCREEN_SAVER[] = {TFT_BL_TIMEOUT_MSB, TFT_BL_TIMEOUT_LSB, TFT_BL_MIN_VAL};

	memcpy(DataPtr, TFT_SCREEN_SAVER, sizeof(TFT_SCREEN_SAVER));

	return sizeof(TFT_SCREEN_SAVER);*/
	uint8_t TFT_SCREEN_SAVER_SETT[3];
	if(vp_param_rw_str.STM_ScreenSaverTime==0)//10Min
	{
		TFT_SCREEN_SAVER_SETT[0]=0x02;
		TFT_SCREEN_SAVER_SETT[1]=0x58;
	}
	else if(vp_param_rw_str.STM_ScreenSaverTime==1)//20Min
	{
		TFT_SCREEN_SAVER_SETT[0]=0x04;
		TFT_SCREEN_SAVER_SETT[1]=0xb0;
	}
	else if(vp_param_rw_str.STM_ScreenSaverTime==2)//30Min
	{
		TFT_SCREEN_SAVER_SETT[0]=0x07;
		TFT_SCREEN_SAVER_SETT[1]=0x08;
	}
	else//Never
	{
		TFT_SCREEN_SAVER_SETT[0]= 0x00; //0xFF;
		TFT_SCREEN_SAVER_SETT[1]= 0x00; //0xFF;
	}
	TFT_SCREEN_SAVER_SETT[2]=0x01;

	//uint8_t TFT_RTC_SET_DATE[]        = {HEADER, 0x9c, RTC_Year, RTC_Month, RTC_Hour,RTC_Min,RTC_Sec, FOOTER1, FOOTER2, FOOTER3, FOOTER4};
	//const uint8_t TFT_RTC_SET_DATE[] = {RTC_Year,RTC_Month,RTC_Date,RTC_Hour,RTC_Min,RTC_Sec};

	memcpy(DataPtr, TFT_SCREEN_SAVER_SETT, sizeof(TFT_SCREEN_SAVER_SETT));

	return sizeof(TFT_SCREEN_SAVER_SETT);
}

/*
 * *****************************************************************************
 * Function name: DispLoadPageCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to load new page
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispLoadPageCmd( uint8_t *DataPtr)
{
	*DataPtr++ = 0;
	*DataPtr = TargetPage;
//	if(ref_page_flg)
	{

	}
	return 2;
}
/*
 * *****************************************************************************
 * Function name: DispSetRTCCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame RTC Set command, will be called during Init
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispSetRTCCmd( uint8_t *DataPtr)
{
	uint8_t RTC_SET_DATE[6];
	RTC_SET_DATE[0]=RTC_Year;
	RTC_SET_DATE[1]=RTC_Month;
	RTC_SET_DATE[2]=RTC_Date;
	RTC_SET_DATE[3]=RTC_Hour;
	RTC_SET_DATE[4]=RTC_Min;
	RTC_SET_DATE[5]=RTC_Sec;
	//uint8_t TFT_RTC_SET_DATE[]        = {HEADER, 0x9c, RTC_Year, RTC_Month, RTC_Hour,RTC_Min,RTC_Sec, FOOTER1, FOOTER2, FOOTER3, FOOTER4};
	//const uint8_t TFT_RTC_SET_DATE[] = {RTC_Year,RTC_Month,RTC_Date,RTC_Hour,RTC_Min,RTC_Sec};

	memcpy(DataPtr, RTC_SET_DATE, sizeof(RTC_SET_DATE));

	return sizeof(RTC_SET_DATE);
}

/*
 * *****************************************************************************
 * Function name: DispSetTouchConfigCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame to set touch config command, will be called during Init
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispSetTouchConfigCmd( uint8_t *DataPtr)
{
	//No data to be added
	return 0;
}

/*
 * *****************************************************************************
 * Function name: DispSetInitPageIdCmd
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame set page command, will be called during Init
 * Parameters:
 *  Input  : Buffer pointer to write command data
 *  Return : Size of the data
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DispSetInitPageIdCmd( uint8_t *DataPtr)
{
	const uint8_t TFT_DISP_PAGE[] = {DISP_PAGE_IDH, DISP_PAGE_IDL}; // Display a pre-stored TML file

	memcpy(DataPtr, TFT_DISP_PAGE, sizeof(TFT_DISP_PAGE));

	return sizeof(TFT_DISP_PAGE);
}

uint8_t SendLevelParamSettings(uint8_t *DataPtr)
{
	LevelSettings.EMPTY_EN = SwapBytes((int16_t)UILevelSettingsData.EMPTY_EN);  //Not defined anywhere
	LevelSettings.REFILLSOON = SwapBytes((int16_t)(UILevelSettingsData.REFILLSOON));
	LevelSettings.FILL = SwapBytes((int16_t)UILevelSettingsData.FILL);  //Not defined anywhere
	memcpy(DataPtr, (uint8_t*)&LevelSettings, sizeof(LevelSettings));
	return sizeof(LevelSettings);
}
uint8_t SendLedParamSettings(uint8_t *DataPtr)
{
	LedSettings.ERRORS_ONLY = SwapBytes((int16_t)UILEDSettingsData.ERRORS_ONLY);
	LedSettings.TEMP = SwapBytes((int16_t)UILEDSettingsData.TEMP);
	LedSettings.LEVEL = SwapBytes((int16_t)UILEDSettingsData.LEVEL);
	LedSettings.VACUUM = SwapBytes((int16_t)UILEDSettingsData.VACUUM);
	LedSettings.STORAGE_FAIL = SwapBytes((int16_t)UILEDSettingsData.STORAGE_FAIL);
	LedSettings.MAINS_BATT = SwapBytes((int16_t)UILEDSettingsData.MAINS_BATT);
	LedSettings.NETWORK_ISSUES = SwapBytes((int16_t)UILEDSettingsData.NETWORK_ISSUES);
	LedSettings.BLINK_CTRL = SwapBytes((int16_t)UILEDSettingsData.BLINK_CTRL);
	LedSettings.NO_ERROR = SwapBytes((int16_t)UILEDSettingsData.NO_ERROR);
	LedSettings.ERRORS = SwapBytes((int16_t)UILEDSettingsData.ERRORS);
	LedSettings.WARNINGS = SwapBytes((int16_t)UILEDSettingsData.WARNINGS);
	LedSettings.UPDATES = SwapBytes((int16_t)UILEDSettingsData.UPDATES);
	LedSettings.NEW_COLOR = SwapBytes((int16_t)UILEDSettingsData.NEW_COLOR);
	LedSettings.OLD_CLR = SwapBytes((int16_t)UILEDSettingsData.OLD_CLR);

	memcpy(DataPtr, (uint8_t *)&LedSettings, sizeof(LedSettings));
	return(sizeof(LedSettings));
}
/*
 * *****************************************************************************
 * Function name: SendTemperatureParamSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame temperature Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendTemperatureParamSettings(uint8_t *DataPtr)
{
	TemperatureSettings.T1HighLimitSwitch = SwapBytes((int16_t)UITempeSettingsData.T1HighLimitSwitch);  //Not defined anywhere
	TemperatureSettings.T1HighLimit = SwapBytes((int16_t)(UITempeSettingsData.T1HighLimit));
	TemperatureSettings.T1LowLimitSwitch = SwapBytes((int16_t)UITempeSettingsData.T1LowLimitSwitch);  //Not defined anywhere
	TemperatureSettings.T1LowLimit = SwapBytes((int16_t)(UITempeSettingsData.T1LowLimit));
	TemperatureSettings.T2HighLimitSwitch = SwapBytes((int16_t)UITempeSettingsData.T2HighLimitSwitch);  //Not defined anywhere
	TemperatureSettings.T2HighLimit = SwapBytes((int16_t)(UITempeSettingsData.T2HighLimit));
	TemperatureSettings.T2LowLimitSwitch = SwapBytes((int16_t)UITempeSettingsData.T2LowLimitSwitch);  //Not defined anywhere
	TemperatureSettings.T2LowLimit = SwapBytes((int16_t)(UITempeSettingsData.T2LowLimit));
	TemperatureSettings.TempUnitSelection = SwapBytes((int16_t)UITempeSettingsData.TempUnitSelection);
	TemperatureSettings.T2Enable = SwapBytes((int16_t)UITempeSettingsData.T2Enable);

	memcpy(DataPtr, (uint8_t*)&TemperatureSettings, sizeof(TemperatureSettings));
	return sizeof(TemperatureSettings);
}


uint8_t SendAlarmParamSettings(uint8_t *DataPtr)
{
	AlarmSettings.RemoteAlarmDelaySwitch = SwapBytes((int16_t)UIAlarmSettingsData.RemoteAlarmDelaySwitch);
	AlarmSettings.RemoteAlarmDelay = SwapBytes((int16_t)UIAlarmSettingsData.RemoteAlarmDelay);
	AlarmSettings.AudiableRepeatDelaySwitch = SwapBytes((int16_t)UIAlarmSettingsData.AudiableRepeatDelaySwitch);
	AlarmSettings.AudiableRepeatDelay = SwapBytes((int16_t)UIAlarmSettingsData.AudiableRepeatDelay);
	AlarmSettings.LN2UsageDelaySwitch = SwapBytes((int16_t)UIAlarmSettingsData.LN2UsageDelaySwitch);
	AlarmSettings.LN2UsageAlarm_value = SwapBytes((int16_t)UIAlarmSettingsData.LN2UsageAlarm_value);
	AlarmSettings.TestAlarmControl = SwapBytes((int16_t)UIAlarmSettingsData.TestAlarmControl);
	AlarmSettings.Battery_sw_over       =  SwapBytes((int16_t)UIAlarmSettingsData.Battery_sw_over);
	memcpy(DataPtr, (uint8_t*)&AlarmSettings, sizeof(AlarmSettings));
	return sizeof(AlarmSettings);
}

/*
 * *****************************************************************************
 * Function name: SendLidFunctionParameters
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Lid Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendLidFunctionParameters(uint8_t *DataPtr)
{
	LidFunctions.LidSwitchSense = SwapBytes((int16_t)UILidFunctionSettingsData.LidSwitchSense);
	LidFunctions.LidOpenTime = SwapBytes((int16_t)UILidFunctionSettingsData.LidOpenTime);
	LidFunctions.LidOpenTime = SwapBytes((int16_t)UILidFunctionSettingsData.LidOpenTime);
	LidFunctions.LidOpenTime = SwapBytes((int16_t)UILidFunctionSettingsData.LidOpenTime);

	memcpy(DataPtr, (uint8_t*)&LidFunctions, sizeof(LidFunctions));
	return sizeof(LidFunctions);
}

/*
 * *****************************************************************************
 * Function name: SendDisplaySettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Display Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendDisplaySettings(uint8_t *DataPtr)
{
	DispSettings.Brightness 		= SwapBytes((int16_t)UIDisplayData.Brightness);
	DispSettings.TouchSound 		= SwapBytes((int16_t)UIDisplayData.TouchSound);
	DispSettings.ScreenSaverTime 	= SwapBytes((int16_t)UIDisplayData.ScreenSaverTime);
	DispSettings.LED_Brightness        = SwapBytes((int16_t)UIDisplayData.LED_Brightness);
	DispSettings.StatusLEDEn        = SwapBytes((int16_t)UIDisplayData.StatusLEDEn);
//	DispSettings.BatterSaver 		= SwapBytes((int16_t)UIDisplayData.BatterSaver);
//	DispSettings.Darkmode 			= SwapBytes((int16_t)UIDisplayData.Darkmode);
//	DispSettings.Language 			= SwapBytes((int16_t)UIDisplayData.Language);

	memcpy(DataPtr, (uint8_t*)&DispSettings, sizeof(DispSettings));
	return sizeof(DispSettings);
}

/*
 * *****************************************************************************
 * Function name: SendModbusSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Modbus Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendModbusSettings(uint8_t *DataPtr)
{
//	ModbusSettings.DeviceIDCryolink 		= SwapBytes((int16_t)UIModbusData.DeviceIDCryolink);
//	ModbusSettings.BaudRateCryolink 		= SwapBytes((int16_t)UIModbusData.BaudRateCryolink);
//	ModbusSettings.FPByteOrderCryolink 		= SwapBytes((int16_t)UIModbusData.FPByteOrderCryolink);
	ModbusSettings.DeviceIDPhasetwo 		= SwapBytes((int16_t)UIModbusData.DeviceIDPhasetwo);
	ModbusSettings.BaudRatePhasetwo 		= SwapBytes((int16_t)UIModbusData.BaudRatePhasetwo);
	ModbusSettings.DataRead_En 		        = SwapBytes((int16_t)UIModbusData.DataRead_En);
//	ModbusSettings.FillMethodPhasetwo 		= SwapBytes((int16_t)UIModbusData.FillMethodPhasetwo);
//	ModbusSettings.Fill_Cmd                 = SwapBytes((int16_t)UIModbusData.Fill_Cmd);
//	ModbusSettings.EN_4_20_mA                 = SwapBytes((int16_t)UIModbusData.EN_4_20_mA);

	memcpy(DataPtr, (uint8_t*)&ModbusSettings, sizeof(ModbusSettings));
	return sizeof(ModbusSettings);
}

/*
 * *****************************************************************************
 * Function name: SendScheduledFillSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame scheduled fill remainder Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendScheduledFillSettings(uint8_t *DataPtr)
{
	ScheduledFillSettings.HoursSettings 		= SwapBytes(UIScheduledFillData.HoursSettings);
	ScheduledFillSettings.MinutesSettings 		= SwapBytes(UIScheduledFillData.MinutesSettings);
	ScheduledFillSettings.AMPMSettings 			= SwapBytes(UIScheduledFillData.AMPMSettings);
	//ScheduledFillSettings.DaySettings 			= SwapBytes(UIScheduledFillData.DaySettings);
	ScheduledFillSettings.Hours12Settings 		= SwapBytes(UIScheduledFillData.Hours12Settings);

	memcpy(DataPtr, (uint8_t*)&ScheduledFillSettings, sizeof(ScheduledFillSettings));
	return sizeof(ScheduledFillSettings);
}

/*
 * *****************************************************************************
 * Function name: SendCheckLevelRemainderSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame check level remainder Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendFillRemainderSettings(uint8_t *DataPtr)
{
	ScheduledRLevelSettings.DateSettings 		= SwapBytes(UIFillRemainderData.DateSettings);
	ScheduledRLevelSettings.MonthSettings 		= SwapBytes(UIFillRemainderData.MonthSettings);
	ScheduledRLevelSettings.YearSettings 		= SwapBytes(UIFillRemainderData.YearSettings);
	ScheduledRLevelSettings.Hours24Settings 			= SwapBytes(UIFillRemainderData.Hours24Settings);
	ScheduledRLevelSettings.MinutesSettings 		= SwapBytes(UIFillRemainderData.MinutesSettings);
	ScheduledRLevelSettings.AMPMSettings 			= SwapBytes(UIFillRemainderData.AMPMSettings);
	ScheduledRLevelSettings.Hours12Settings 		= SwapBytes(UIFillRemainderData.Hours12Settings);

	memcpy(DataPtr, (uint8_t*)&ScheduledRLevelSettings, sizeof(ScheduledRLevelSettings));
	return sizeof(ScheduledRLevelSettings);
}

/*
 * *****************************************************************************
 * Function name: SendLN2SupplyRemainderSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame LN2 Supply remainder Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
//uint8_t SendLN2SupplyRemainderSettings(uint8_t *DataPtr)
//{
//	ScheduledLN2SupplySettings.HoursSettings 		= SwapBytes(UILN2SupplyRemainderData.HoursSettings);
//	ScheduledLN2SupplySettings.MinutesSettings 		= SwapBytes(UILN2SupplyRemainderData.MinutesSettings);
//	ScheduledLN2SupplySettings.AMPMSettings 		= SwapBytes(UILN2SupplyRemainderData.AMPMSettings);
//	ScheduledLN2SupplySettings.DaySettings 			= SwapBytes(UILN2SupplyRemainderData.DaySettings);
//	ScheduledLN2SupplySettings.Hours12Settings 		= SwapBytes(UILN2SupplyRemainderData.Hours12Settings);
////	ScheduledLN2SupplySettings.Minutes12Settings 	= SwapBytes(UILN2SupplyRemainderData.Minutes12Settings);
//
//	memcpy(DataPtr, (uint8_t*)&ScheduledLN2SupplySettings, sizeof(ScheduledLN2SupplySettings));
//	return sizeof(ScheduledLN2SupplySettings);
//}

/*
 * *****************************************************************************
 * Function name: SendMaintenanceRemainderSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Maintenance remainder Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendMaintenanceRemainderSettings(uint8_t *DataPtr)
{
	MaintenanceRemainderSettings.Hours12Settings 	= SwapBytes(UIMaintenanceRemainderSettings.Hours12Settings);
	MaintenanceRemainderSettings.DateSettings 		= SwapBytes(UIMaintenanceRemainderSettings.DateSettings);
	MaintenanceRemainderSettings.MonthSettings 		= SwapBytes(UIMaintenanceRemainderSettings.MonthSettings);
	MaintenanceRemainderSettings.YearSettings 		= SwapBytes(UIMaintenanceRemainderSettings.YearSettings);
	MaintenanceRemainderSettings.HoursSettings 		= SwapBytes(UIMaintenanceRemainderSettings.HoursSettings);
	MaintenanceRemainderSettings.MinutesSettings 	= SwapBytes(UIMaintenanceRemainderSettings.MinutesSettings);
	MaintenanceRemainderSettings.AMPMSettings 		= SwapBytes(UIMaintenanceRemainderSettings.AMPMSettings);

	memcpy(DataPtr, (uint8_t*)&MaintenanceRemainderSettings, sizeof(MaintenanceRemainderSettings));
	return sizeof(MaintenanceRemainderSettings);
}

/*
 * *****************************************************************************
 * Function name: SendDateTimeSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Date and time Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendDateTimeSettings(uint8_t *DataPtr)
{
	Get_TimeStamp();
	DateTimeSettings.Hours12Settings 		= SwapBytes(Time.Hours);
	UIDateTimeSettings.Hours12Settings		= Time.Hours;
	DateTimeSettings.DateFormat 			= SwapBytes(UIDateTimeSettings.DateFormat);
	DateTimeSettings.TimeFormat 			= SwapBytes(UIDateTimeSettings.TimeFormat);
	DateTimeSettings.DateSettings 			= SwapBytes(Date.Date);
	UIDateTimeSettings.DateSettings 		= Date.Date;
	DateTimeSettings.MonthSettings 			= SwapBytes(Date.Month);
	UIDateTimeSettings.MonthSettings 		= Date.Month;
	DateTimeSettings.YearSettings 			= SwapBytes(Date.Year);
	UIDateTimeSettings.YearSettings 		= Date.Year;
	//___________________________________________________________________//
	if(UIDateTimeSettings.TimeFormat==1)
	{
		if(Time.Hours==12)
		{
			if((Time.TimeFormat==0))
			UIDateTimeSettings.HoursSettings=(Time.Hours-12);
			else
			UIDateTimeSettings.HoursSettings=(Time.Hours);
		}
		else if((Time.Hours<12)&&(Time.TimeFormat==1))
		UIDateTimeSettings.HoursSettings=(Time.Hours+12);
		else
		UIDateTimeSettings.HoursSettings=(Time.Hours);
	}
	else
	{
		if(UIDateTimeSettings.AMPMSettings)
		{
			if(Time.Hours<12)
			UIDateTimeSettings.HoursSettings=Time.Hours;
			else
			UIDateTimeSettings.HoursSettings=(Time.Hours-12);
		}
		else
		UIDateTimeSettings.HoursSettings=Time.Hours;
	}
	DateTimeSettings.HoursSettings 			= SwapBytes(UIDateTimeSettings.HoursSettings);
	//___________________________________________________________________//
	DateTimeSettings.MinutesSettings 		= SwapBytes(Time.Minutes);
	UIDateTimeSettings.MinutesSettings 		= Time.Minutes;
	DateTimeSettings.AMPMSettings 			= SwapBytes(Time.TimeFormat);
	UIDateTimeSettings.AMPMSettings 		= Time.TimeFormat;
	memcpy(DataPtr, (uint8_t*)&DateTimeSettings, sizeof(DateTimeSettings));
	return sizeof(DateTimeSettings);
}


/*
 * *****************************************************************************
 * Function name: SendInit0to7FVP
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame UI 0 to 7F VP Init data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */

/*
 * *****************************************************************************
 * Function name: SendScheduledEventsSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame ScheduledEvents Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendScheduledEventsSettings(uint8_t *DataPtr)
{
	ScheduledSettings.ScheduledFillRemainderEn 	= SwapBytes((int16_t)UIScheduledEventData.ScheduledFillRemainderEn);
	ScheduledSettings.MaintRemainderEn 			= SwapBytes((int16_t)UIScheduledEventData.MaintRemainderEn);
	ScheduledSettings.FillRemainderEn 		= SwapBytes((int16_t)UIScheduledEventData.FillRemainderEn);


	memcpy(DataPtr, (uint8_t*)&ScheduledSettings, sizeof(ScheduledSettings));
	return sizeof(ScheduledSettings);
}
/*
 * *****************************************************************************
 * Function name: SendScheduledEventsSettings1
 * Created by : Venkatesh Arikanti
 * Date:        14-Jun-2023
 * Description:
 *  Function to frame ScheduledEvents Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendScheduledEventsSettings1(uint8_t *DataPtr)
{
	ScheduledSettings.ScheduledFillRemainderEn 	= SwapBytes((int16_t)UIScheduledEventData.ScheduledFillRemainderEn);
	ScheduledSettings.MaintRemainderEn 			= SwapBytes((int16_t)UIScheduledEventData.MaintRemainderEn);
	ScheduledSettings.FillRemainderEn 		= SwapBytes((int16_t)UIScheduledEventData.FillRemainderEn);


	memcpy(DataPtr, (uint8_t*)&ScheduledSettings, sizeof(ScheduledSettings));
	return sizeof(ScheduledSettings);
}

/*
 * *****************************************************************************
 * Function name: SenddayssSettings
 * Created by : Arikanti venkatesh
 * Date:        17-Nov-2023
 * Description:
 *  Function to frame Pick time data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SenddayssSettings(uint8_t *DataPtr)
{
	Daily_data.Daily 	    = SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x01));
	Daily_data.Monday	    = SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x02)>>1);
	Daily_data.Tuesday		= SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x04)>>2);
	Daily_data.Wednesday	= SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x08)>>3);
	Daily_data.Thursday	    = SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x10)>>4);
	Daily_data.Friday	    = SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x20)>>5);
	Daily_data.Saturday	    = SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x40)>>6);
	Daily_data.Sunday	    = SwapBytes((int16_t)(UIScheduledFillData.DaySettings & 0x80)>>7);
	memcpy(DataPtr, (uint8_t*)&Daily_data, sizeof(Daily_data));
	return sizeof(Daily_data);
}

/*uint8_t Send420mAEventsSettings(uint8_t *DataPtr)
{
	Network_4_20mA.EXD_MD_Switch    =	SwapBytes((int16_t)UI_4_20mAsettings.EXD_MD_Switch);
	Network_4_20mA.Chan1_Switch     =	SwapBytes((int16_t)UI_4_20mAsettings.Chan1_Switch);
	Network_4_20mA.Chan2_Switch	 	=	SwapBytes((int16_t)UI_4_20mAsettings.Chan2_Switch);
	Network_4_20mA.Chan1_Selction	=	SwapBytes((int16_t)UI_4_20mAsettings.Chan1_Selction);
	Network_4_20mA.Chan2_Selction	=	SwapBytes((int16_t)UI_4_20mAsettings.Chan2_Selction);
	Network_4_20mA.Source_sel	 	=	SwapBytes((int16_t)UI_4_20mAsettings.Source_sel);
	Network_4_20mA.Chan1_vol_cur	=	SwapBytes((int16_t)UI_4_20mAsettings.Chan1_vol_cur);
	Network_4_20mA.Chan2_vol_cur	=	SwapBytes((int16_t)UI_4_20mAsettings.Chan2_vol_cur);

	memcpy(DataPtr, (uint8_t*)&Network_4_20mA, sizeof(Network_4_20mA));
	return sizeof(Network_4_20mA);
}

uint8_t Send420mAconfigEventsSettings(uint8_t *DataPtr)
{
	Network4_20mA_config.T1_MIN_CONFIG_CH1   =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T1_MIN_CONFIG_CH1);
	Network4_20mA_config.T2_MIN_CONFIG_CH1   =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T2_MIN_CONFIG_CH1);
	Network4_20mA_config.DP_MIN_CONFIG_CH1	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.DP_MIN_CONFIG_CH1);
	Network4_20mA_config.T1_MIN_CONFIG_CH2	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T1_MIN_CONFIG_CH2);
	Network4_20mA_config.T2_MIN_CONFIG_CH2	  = SwapBytes((int16_t)UI_4_20mAconfigSettings.T2_MIN_CONFIG_CH2);
	Network4_20mA_config.DP_MIN_CONFIG_CH2	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.DP_MIN_CONFIG_CH2);
	Network4_20mA_config.T1_MAX_CONFIG_CH1	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T1_MAX_CONFIG_CH1);
	Network4_20mA_config.T2_MAX_CONFIG_CH1	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T2_MAX_CONFIG_CH1);
	Network4_20mA_config.DP_MAX_CONFIG_CH1	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.DP_MAX_CONFIG_CH1);
	Network4_20mA_config.T1_MAX_CONFIG_CH2	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T1_MAX_CONFIG_CH2);
	Network4_20mA_config.T2_MAX_CONFIG_CH2	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.T2_MAX_CONFIG_CH2);
	Network4_20mA_config.DP_MAX_CONFIG_CH2	  =	SwapBytes((int16_t)UI_4_20mAconfigSettings.DP_MAX_CONFIG_CH2);

	memcpy(DataPtr, (uint8_t*)&Network4_20mA_config, sizeof(Network4_20mA_config));
	return sizeof(Network4_20mA_config);
}*/

/*
 * *****************************************************************************
 * Function name: SendsfirmwareTimeSettings
 * Created by : Arikanti venkatesh
 * Date:        14-Nov-2023
 * Description:
 *  Function to frame Pick time data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendsfirmwareTimeSettings(uint8_t *DataPtr)
{
//	UIUpdateRemaindersettings.Minutes12Settings	    = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.MinutesSettings	    = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.Hours12Settings	    = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.HoursSettings	        = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.DateSettings          = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.MonthSettings	        = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.YearSettings          = SwapBytes((int16_t)(0));
	UIUpdateRemaindersettings.AMPMSettings          = SwapBytes((int16_t)(0));
	memcpy(DataPtr, (uint8_t*)&UIUpdateRemaindersettings, sizeof(UIUpdateRemaindersettings));
	return sizeof(UIUpdateRemaindersettings);
}
/*
 * *****************************************************************************
 * Function name: SendsGroupColourSettings
 * Created by : Arikanti venkatesh
 * Date:        14-Dec-2023
 * Description:
 *  Function to frame Node Id  data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
/*uint8_t SendsGroupColourSettings(uint8_t *DataPtr)
{
	UpdateGroupsettings.Group_col[0]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[0]);
	UpdateGroupsettings.Group_col[1]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[1]);
	UpdateGroupsettings.Group_col[2]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[2]);
	UpdateGroupsettings.Group_col[3]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[3]);
	UpdateGroupsettings.Group_col[4]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[4]);
	UpdateGroupsettings.Group_col[5]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[5]);
	UpdateGroupsettings.Group_col[6]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[6]);
	UpdateGroupsettings.Group_col[7]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[7]);
	UpdateGroupsettings.Group_col[8]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[8]);
	UpdateGroupsettings.Group_col[9]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[9]);
	UpdateGroupsettings.Group_col[10]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[10]);
	UpdateGroupsettings.Group_col[11]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[11]);
	UpdateGroupsettings.Group_col[12]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[12]);
	UpdateGroupsettings.Group_col[13]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[13]);
	UpdateGroupsettings.Group_col[14]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[14]);
	UpdateGroupsettings.Group_col[15]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[15]);
	UpdateGroupsettings.Group_col[16]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[16]);
	UpdateGroupsettings.Group_col[17]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[17]);
	UpdateGroupsettings.Group_col[18]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[18]);
	UpdateGroupsettings.Group_col[19]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[19]);
	UpdateGroupsettings.Group_col[20]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[20]);
	UpdateGroupsettings.Group_col[21]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[21]);
	UpdateGroupsettings.Group_col[22]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[22]);
	UpdateGroupsettings.Group_col[23]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[23]);
	UpdateGroupsettings.Group_col[24]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[24]);
	UpdateGroupsettings.Group_col[25]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[25]);
	UpdateGroupsettings.Group_col[26]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[26]);
	UpdateGroupsettings.Group_col[27]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[27]);
	UpdateGroupsettings.Group_col[28]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[28]);
	UpdateGroupsettings.Group_col[29]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[29]);
	UpdateGroupsettings.Group_col[30]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[30]);
	UpdateGroupsettings.Group_col[31]	            = SwapBytes((int16_t)UIUpdateGroupsettings.Group_col[31]);
	memcpy(DataPtr, (uint8_t*)&UpdateGroupsettings.Group_col, sizeof(UpdateGroupsettings.Group_col));
	return sizeof(UpdateGroupsettings.Group_col);
}


uint8_t SendNoResponseSettings(uint8_t *DataPtr)
{
	Group_inactive_nodes[0]	            = SwapBytes(0);
	Group_inactive_nodes[1]	            = SwapBytes((int16_t)node_info[1].Group_inactive);
	Group_inactive_nodes[2]	            = SwapBytes((int16_t)node_info[2].Group_inactive);
	Group_inactive_nodes[3]	            = SwapBytes((int16_t)node_info[3].Group_inactive);
	Group_inactive_nodes[4]	            = SwapBytes((int16_t)node_info[4].Group_inactive);
	Group_inactive_nodes[5]	            = SwapBytes((int16_t)node_info[5].Group_inactive);
	Group_inactive_nodes[6]	            = SwapBytes((int16_t)node_info[6].Group_inactive);
	Group_inactive_nodes[7]	            = SwapBytes((int16_t)node_info[7].Group_inactive);
	Group_inactive_nodes[8]	            = SwapBytes((int16_t)node_info[8].Group_inactive);
	Group_inactive_nodes[9]	            = SwapBytes((int16_t)node_info[9].Group_inactive);
	Group_inactive_nodes[10]	        = SwapBytes((int16_t)node_info[10].Group_inactive);
	Group_inactive_nodes[11]	        = SwapBytes((int16_t)node_info[11].Group_inactive);
	Group_inactive_nodes[12]	        = SwapBytes((int16_t)node_info[12].Group_inactive);
	Group_inactive_nodes[13]	        = SwapBytes((int16_t)node_info[13].Group_inactive);
	Group_inactive_nodes[14]	        = SwapBytes((int16_t)node_info[14].Group_inactive);
	Group_inactive_nodes[15]	        = SwapBytes((int16_t)node_info[15].Group_inactive);
	Group_inactive_nodes[16]	        = SwapBytes((int16_t)node_info[16].Group_inactive);
	Group_inactive_nodes[17]	        = SwapBytes((int16_t)node_info[17].Group_inactive);
	Group_inactive_nodes[18]	        = SwapBytes((int16_t)node_info[18].Group_inactive);
	Group_inactive_nodes[19]	        = SwapBytes((int16_t)node_info[19].Group_inactive);
	Group_inactive_nodes[20]	        = SwapBytes((int16_t)node_info[20].Group_inactive);
	Group_inactive_nodes[21]	        = SwapBytes((int16_t)node_info[21].Group_inactive);
	Group_inactive_nodes[22]	        = SwapBytes((int16_t)node_info[22].Group_inactive);
	Group_inactive_nodes[23]	        = SwapBytes((int16_t)node_info[23].Group_inactive);
	Group_inactive_nodes[24]	        = SwapBytes((int16_t)node_info[24].Group_inactive);
	Group_inactive_nodes[25]	        = SwapBytes((int16_t)node_info[25].Group_inactive);
	Group_inactive_nodes[26]	        = SwapBytes((int16_t)node_info[26].Group_inactive);
	Group_inactive_nodes[27]	        = SwapBytes((int16_t)node_info[27].Group_inactive);
	Group_inactive_nodes[28]	        = SwapBytes((int16_t)node_info[28].Group_inactive);
	Group_inactive_nodes[29]	        = SwapBytes((int16_t)node_info[29].Group_inactive);
	Group_inactive_nodes[30]	        = SwapBytes((int16_t)node_info[30].Group_inactive);
	Group_inactive_nodes[31]	        = SwapBytes((int16_t)node_info[31].Group_inactive);

	memcpy(DataPtr, (uint8_t*)&Group_inactive_nodes, sizeof(Group_inactive_nodes));
	return sizeof(Group_inactive_nodes);
}*/
/*
 * *****************************************************************************
 * Function name: SendADCData
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame system information Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
extern uint16_t DPAvg_Value;
uint8_t SendADCData(uint8_t *DataPtr)
{
	*DataPtr++ = (ADC_info.buffer[9]>>8) & 0x00FF;
	*DataPtr++ = (ADC_info.buffer[9]) & 0x00FF;
	*DataPtr++ = (DPAvg_Value>>8) & 0x00FF;
	*DataPtr = (DPAvg_Value) & 0x00FF;

	return 4;
}
/*
 * *****************************************************************************
 * Function name: SendSystemInfoSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame system information Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendSystemInfoSettings(uint8_t *DataPtr)
{
	SystemInfo.BatteryLevel = SwapBytes(vp_evnt_wr_str.batt_info.value);
	SystemInfo.BatterySavingMode = SwapBytes(vp_evnt_wr_str.batt_info.status);
	SystemInfo.Brightness = SwapBytes(vp_param_rw_str.brightness);
	SystemInfo.BrightnessPercentage = SwapBytes((((uint16_t)vp_param_rw_str.brightness)*100)/64);
	SystemInfo.DarkModeControl = SwapBytes(0);
	SystemInfo.FreezerID = SwapBytes(0x3412);
	SystemInfo.LoginStatus = SwapBytes((int16_t)(vp_rt_wr_str.LoginStatus));
	SystemInfo.PowerStatus = SwapBytes((int16_t)((~vp_rt_wr_str.PowerStatus)&0x01));
	SystemInfo.ScreenSaverTime = SwapBytes(vp_param_rw_str.screen_on_dly);
	SystemInfo.WiFiStatus = SwapBytes((int16_t)(vp_rt_wr_str.WiFiStatus));

	memcpy(DataPtr, (uint8_t*)&SystemInfo, sizeof(SystemInfo));
	return sizeof(SystemInfo);
}

/*
 * *****************************************************************************
 * Function name: SendParamSettings
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame Settings data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
//uint8_t SendParamSettings(uint8_t *DataPtr)
//{
//	TemperatureSettings.T1HighLimitSwitch = SwapBytes(1);  //Not defined anywhere
//	TemperatureSettings.T1HighLimit = SwapBytes((int16_t)(vp_param_rw_str.high_temp1.value*10));
//	TemperatureSettings.T1LowLimitSwitch = SwapBytes(1);  //Not defined anywhere
//	TemperatureSettings.T1LowLimit = SwapBytes((int16_t)(vp_param_rw_str.low_temp1.value*10));
//	TemperatureSettings.T2HighLimitSwitch = SwapBytes(1);  //Not defined anywhere
//	TemperatureSettings.T2HighLimit = SwapBytes((int16_t)(vp_param_rw_str.high_temp2.value*10));
//	TemperatureSettings.T2LowLimitSwitch = SwapBytes(1);  //Not defined anywhere
//	TemperatureSettings.T2LowLimit = SwapBytes((int16_t)(vp_param_rw_str.low_temp2.value*10));
//	TemperatureSettings.TempUnitSelection = SwapBytes((int16_t)vp_param_rw_str.high_temp1.units);
//	memcpy(DataPtr, (uint8_t*)&TemperatureSettings, sizeof(TemperatureSettings));
//	return sizeof(TemperatureSettings);
//}

 uint8_t SendCTRSData(uint8_t *DataPtr)
 {

	 Ctrsreading.Loadcell1 =Swap32Bytes(0);
	 Ctrsreading.Loadcell2 =Swap32Bytes(0);
	 Ctrsreading.Loadcell3 =Swap32Bytes(0);
	 Ctrsreading.Loadcell4 =Swap32Bytes(0);
	 Ctrsreading.Loadcell5 =Swap32Bytes(0);
	 Ctrsreading.LoadcellAvg =Swap32Bytes(0);
//	 uint8_t pressure_caccum_status=0;
//	 if(ctrs_ws_para.vaccum_value<750*1000*100)
//	 {
//		 pressure_caccum_status=1;
//	 }
//	 else
//	 {
//		 pressure_caccum_status=0;
//	 }

	 Ctrsreading.pressure_value =Swap32Bytes((int32_t)(mTorr*10));
	 memcpy(DataPtr, (uint8_t*)&Ctrsreading, sizeof(Ctrsreading));
	 return sizeof(Ctrsreading);
 }
 uint8_t Green_flag=1, LED_off=1;
 uint8_t Sendendiswifi(uint8_t *DataPtr)
 {
 	updatewifien.SCAN_WIFI_EN1 	    = SwapBytes((int16_t)(Wifi_ssid[0].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN2 	    = SwapBytes((int16_t)(Wifi_ssid[1].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN3 	    = SwapBytes((int16_t)(Wifi_ssid[2].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN4 	    = SwapBytes((int16_t)(Wifi_ssid[3].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN5 	    = SwapBytes((int16_t)(Wifi_ssid[4].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN6 	    = SwapBytes((int16_t)(Wifi_ssid[5].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN7 	    = SwapBytes((int16_t)(Wifi_ssid[6].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN8 	    = SwapBytes((int16_t)(Wifi_ssid[7].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN9 	    = SwapBytes((int16_t)(Wifi_ssid[8].SSid_signal ?1:0));
 	updatewifien.SCAN_WIFI_EN10 	    = SwapBytes((int16_t)(Wifi_ssid[9].SSid_signal ?1:0));
 	memcpy(DataPtr, (uint8_t*)&updatewifien, sizeof(updatewifien));
 	PCA9955B_SetAllLEDsOff();
 		PCA9955B_SetColorOn(2);
 		Green_flag=0;
 		LED_off=0;
 	return sizeof(updatewifien);

 }

 uint8_t Sendendisfile(uint8_t *DataPtr)
 {
	 updatefileen.FILE_TOUCH_EN1 	    = SwapBytes((int16_t)(dirList[0].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN2 	    = SwapBytes((int16_t)(dirList[1].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN3 	    = SwapBytes((int16_t)(dirList[2].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN4 	    = SwapBytes((int16_t)(dirList[3].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN5 	    = SwapBytes((int16_t)(dirList[4].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN6 	    = SwapBytes((int16_t)(dirList[5].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN7 	    = SwapBytes((int16_t)(dirList[6].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN8 	    = SwapBytes((int16_t)(dirList[7].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN9 	    = SwapBytes((int16_t)(dirList[8].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN10 	    = SwapBytes((int16_t)(dirList[9].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN11	    = SwapBytes((int16_t)(dirList[10].name[0] ? 1:0));
	 updatefileen.FILE_TOUCH_EN12 	    = SwapBytes((int16_t)(dirList[11].name[0] ? 1:0));

 	memcpy(DataPtr, (uint8_t*)&updatefileen, sizeof(updatefileen));

 	return sizeof(updatefileen);

 }


 uint8_t Sendendisfiletype(uint8_t *DataPtr)
 {
	 updatefiletypeen.FILE_TYPE_EN1 	    = SwapBytes((int16_t)(dirList[0].type));
	 updatefiletypeen.FILE_TYPE_EN2 	    = SwapBytes((int16_t)(dirList[1].type));
	 updatefiletypeen.FILE_TYPE_EN3 	    = SwapBytes((int16_t)(dirList[2].type));
	 updatefiletypeen.FILE_TYPE_EN4 	    = SwapBytes((int16_t)(dirList[3].type));
	 updatefiletypeen.FILE_TYPE_EN5 	    = SwapBytes((int16_t)(dirList[4].type));
	 updatefiletypeen.FILE_TYPE_EN6 	    = SwapBytes((int16_t)(dirList[5].type));
	 updatefiletypeen.FILE_TYPE_EN7 	    = SwapBytes((int16_t)(dirList[6].type));
	 updatefiletypeen.FILE_TYPE_EN8 	    = SwapBytes((int16_t)(dirList[7].type));
	 updatefiletypeen.FILE_TYPE_EN9 	    = SwapBytes((int16_t)(dirList[8].type));
	 updatefiletypeen.FILE_TYPE_EN10 	    = SwapBytes((int16_t)(dirList[9].type));
	 updatefiletypeen.FILE_TYPE_EN11	    = SwapBytes((int16_t)(dirList[10].type));
	 updatefiletypeen.FILE_TYPE_EN12 	    = SwapBytes((int16_t)(dirList[11].type));

 	memcpy(DataPtr, (uint8_t*)&updatefiletypeen, sizeof(updatefiletypeen));

 	return sizeof(updatefiletypeen);

 }

 uint8_t Sendfileselection(uint8_t *DataPtr)
 {
	 File_read_write.file_sel1 = SwapBytes((int16_t)(dirList[0].file_sel));
	 File_read_write.file_sel2 = SwapBytes((int16_t)(dirList[1].file_sel));
	 File_read_write.file_sel3 = SwapBytes((int16_t)(dirList[2].file_sel));
	 File_read_write.file_sel4 = SwapBytes((int16_t)(dirList[3].file_sel));
	 File_read_write.file_sel5 = SwapBytes((int16_t)(dirList[4].file_sel));
	 File_read_write.file_sel6 = SwapBytes((int16_t)(dirList[5].file_sel));
	 File_read_write.file_sel7 = SwapBytes((int16_t)(dirList[6].file_sel));
	 File_read_write.file_sel8 = SwapBytes((int16_t)(dirList[7].file_sel));
	 File_read_write.file_sel9 = SwapBytes((int16_t)(dirList[8].file_sel));
	 File_read_write.file_sel10 = SwapBytes((int16_t)(dirList[9].file_sel));
	 File_read_write.file_sel11 = SwapBytes((int16_t)(dirList[10].file_sel));
	 File_read_write.file_sel12 = SwapBytes((int16_t)(dirList[11].file_sel));

	 memcpy(DataPtr, (uint8_t*)&File_read_write, sizeof(File_read_write));

	 return sizeof(File_read_write);
 }

 uint8_t Sendfilesize(uint8_t *DataPtr)
 {
	 File_Size.file_size1 = SwapBytes((int16_t)(dirList[0].fileSizes));
	 File_Size.file_size2 = SwapBytes((int16_t)(dirList[1].fileSizes));
	 File_Size.file_size3 = SwapBytes((int16_t)(dirList[2].fileSizes));
	 File_Size.file_size4 = SwapBytes((int16_t)(dirList[3].fileSizes));
	 File_Size.file_size5 = SwapBytes((int16_t)(dirList[4].fileSizes));
	 File_Size.file_size6 = SwapBytes((int16_t)(dirList[5].fileSizes));
	 File_Size.file_size7 = SwapBytes((int16_t)(dirList[6].fileSizes));
	 File_Size.file_size8 = SwapBytes((int16_t)(dirList[7].fileSizes));
	 File_Size.file_size9 = SwapBytes((int16_t)(dirList[8].fileSizes));
	 File_Size.file_size10 = SwapBytes((int16_t)(dirList[9].fileSizes));
	 File_Size.file_size11 = SwapBytes((int16_t)(dirList[10].fileSizes));
	 File_Size.file_size12 = SwapBytes((int16_t)(dirList[11].fileSizes));

	 memcpy(DataPtr, (uint8_t*)&File_Size, sizeof(File_Size));

	 return sizeof(File_Size);
 }

 /*uint8_t SendCTRSconfigEventsSettings(uint8_t *DataPtr)
 {
	 ctrs_config.THigh_CONFIG                =  SwapBytes((int16_t)UICTRStemperature.THighLimit);
	 ctrs_config.TLow_CONFIG                 =  SwapBytes((int16_t)UICTRStemperature.TLowLimit);

 	memcpy(DataPtr, (uint8_t*)&ctrs_config, sizeof(ctrs_config));
 	return sizeof(ctrs_config);
 }*/

 /*
  * *****************************************************************************
  * Function name: SendRunTimeData
  * Created by : Venkatesh Arikanti
  * Date:        14-10-2025
  * Description:
  *  Function to frame Runtime data to LCD
  * Parameters:
  *  Input  : Data Pointer to fill the settings data
  *  Return : No of bytes added
  * Note:
  *
  * *****************************************************************************
  */
uint8_t SendRunTimeData(uint8_t *DataPtr)
{
	int16_t pwr_sts = 0;

	float T1ValueConverted = 0;
	float T2ValueConverted = 0;
	if(vp_param_rw_str.tempUnits)
	{
		T1ValueConverted = (vp_rt_wr_str.temp1 * 1.8 ) + 32;  // Fahrenheit
		T2ValueConverted = (vp_rt_wr_str.temp2 * 1.8 ) + 32;  // Fahrenheit
	}
	else
	{
		T1ValueConverted = vp_rt_wr_str.temp1;
		T2ValueConverted = vp_rt_wr_str.temp2;
	}
	SensorReading.Mute_control = SwapBytes((int16_t)(~mute_seq_start_flg));
	SensorReading.T1Value = SwapBytes((int16_t)(T1ValueConverted*10));
	SensorReading.T2Value = SwapBytes((int16_t)(T2ValueConverted*10));
	SensorReading.level   =SwapBytes((int16_t)(vp_rt_wr_str.level_status));
	SensorReading.weight_overall = SwapBytes((int16_t)vp_rt_wr_str.weight_overallStatus);	//Need to comeback
	if(!vp_rt_wr_str.PowerStatus)
	{
		if((vp_rt_wr_str.batt_volt<11.2))
		pwr_sts=4;
		else if((vp_rt_wr_str.batt_volt>=11.2)&&(vp_rt_wr_str.batt_volt<12.53))
		pwr_sts=3;
		else if((vp_rt_wr_str.batt_volt>=12.53)&&(vp_rt_wr_str.batt_volt<13.86))
		pwr_sts=2;
		else if(vp_rt_wr_str.batt_volt>=13.86)
		pwr_sts=1;
	}
	else
	pwr_sts=0;

	SensorReading.mains_battery = SwapBytes((int16_t)(pwr_sts));
	SensorReading.battery_values = SwapBytes((int16_t)(vp_rt_wr_str.batt_volt*10));
	SensorReading.login_index = SwapBytes((int16_t)vp_rt_wr_str.LoginStatus);
	SensorReading.internet_status = SwapBytes((int16_t)vp_rt_wr_str.WiFiStatus);
	SensorReading.BLE_Status = SwapBytes((int16_t)vp_rt_wr_str.BLEStatus);
	SensorReading.Mod_Status = SwapBytes((int16_t)vp_rt_wr_str.MBStatus);
	SensorReading.Temp_Units = SwapBytes((int16_t)vp_param_rw_str.tempUnits);
	SensorReading.T1_Status = SwapBytes((int16_t)vp_rt_wr_str.rtd1_status);
	SensorReading.T2_Status = SwapBytes((int16_t)vp_rt_wr_str.rtd2_status);


	if(vp_param_rw_str.t2enable==0)
	{

	   SensorReading.Red_T1en = SwapBytes(0);
	   SensorReading.wt_T1en = SwapBytes(0);
	   SensorReading.Red_T2en = SwapBytes(0);
	   SensorReading.wt_T2en = SwapBytes(0);
	   SensorReading.T1_open_en = SwapBytes(0);
	   SensorReading.T2_open_en = SwapBytes(0);
		if((vp_param_rw_str.RTD_T1_Low_Error_flg)||(vp_param_rw_str.RTD_T1_High_Error_flg))
		{
			   SensorReading.wt_t1_only_en = SwapBytes(1);
			   SensorReading.Red_t1_only_en = SwapBytes(1);
		}
		else
		{
			   SensorReading.wt_t1_only_en = SwapBytes(1);
			   SensorReading.Red_t1_only_en = SwapBytes(0);
		}


		if(vp_param_rw_str.Open_detection_rtd1 || vp_param_rw_str.Short_detection_rtd1)
			 SensorReading.t1only_open_en = SwapBytes(1);
		else
			 SensorReading.t1only_open_en = SwapBytes(0);
	}

	else
	{
		   SensorReading.wt_t1_only_en = SwapBytes(0);
		   SensorReading.Red_t1_only_en = SwapBytes(0);
		   SensorReading.t1only_open_en = SwapBytes(0);
		if((vp_param_rw_str.RTD_T1_Low_Error_flg)||(vp_param_rw_str.RTD_T1_High_Error_flg))
		{
			   SensorReading.Red_T1en = SwapBytes(1);
			   SensorReading.wt_T1en = SwapBytes(1);
		}
		else
		{
			   SensorReading.Red_T1en = SwapBytes(0);
			   SensorReading.wt_T1en = SwapBytes(1);
		}
		if((vp_param_rw_str.RTD_T2_Low_Error_flg)||(vp_param_rw_str.RTD_T2_High_Error_flg))
		{
			   SensorReading.Red_T2en = SwapBytes(1);
			   SensorReading.wt_T2en = SwapBytes(1);
		}
		else
		{
			   SensorReading.Red_T2en = SwapBytes(0);
			   SensorReading.wt_T2en = SwapBytes(1);
		}

		if(vp_param_rw_str.Open_detection_rtd1 || vp_param_rw_str.Short_detection_rtd1)
			 SensorReading.T1_open_en = SwapBytes(1);
		else
			SensorReading.T1_open_en = SwapBytes(0);
		if(vp_param_rw_str.Open_detection_rtd2 || vp_param_rw_str.Short_detection_rtd2)
			SensorReading.T2_open_en = SwapBytes(1);
		else
			SensorReading.T2_open_en = SwapBytes(0);
	}



	SensorReading.level_fault_en = SwapBytes(vp_rt_wr_str.level_status);			//Need to comeback
	SensorReading.weight_fault_en = SwapBytes(0);			//Need to comeback
	if(vp_rt_wr_str.PowerStatus)
	{
		SensorReading.Battery_en = SwapBytes(0);
		SensorReading.Mains_en = SwapBytes(1);
	}
	else
	{
		SensorReading.Battery_en = SwapBytes(1);
		SensorReading.Mains_en = SwapBytes(0);
	}
	SensorReading.RTC_Blink = SwapBytes((int16_t)Rtc_Toggle);

	if(vp_param_rw_str.STM_StatusLed)
	{
		if(Rtc_Toggle && GetTotalItemIndex())
		{
			if(Green_flag)
			{
				PCA9955B_SetAllLEDsOff();
			}
			PCA9955B_SetColorOn(0);
			Green_flag=0;
		}
		else if((GetTotalItemIndex()==0) && (Green_flag==0))
		{
			PCA9955B_SetAllLEDsOff();
			PCA9955B_SetColorOn(1);
			Green_flag=1;
		}
		else if( GetTotalItemIndex())
		{
			PCA9955B_SetAllLEDsOff();
			Green_flag=0;
		}
		LED_off=1;
	}
	else if(LED_off)
	{
		PCA9955B_SetAllLEDsOff();
		Green_flag=0;
		LED_off=0;
	}


	if(GetTotalItemIndex()&&(!mute_seq_start_flg))
	{
#ifdef EN_BUZZER
		HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
#endif
	}
	else
	{
		if(GetTotalItemIndex()==0)
		mute_seq_start_flg=0;
		if((!test_alarm_flg)||(spm_info.shutdown))
		{
			if(Init_Wait_Delay_for_UI_flg==1)
			{
#ifdef EN_BUZZER
				HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
#endif
			}
		}
	}
	if(GetTotalItemIndex()&&(!mute_rem_start_flag)&&(!spm_info.shutdown)&&vp_param_rw_str.RemoteAlarmDelaySwitch)
	{
		HAL_GPIO_WritePin(GPIOE, GPIO_ODR_OD8,GPIO_PIN_RESET);
		mute_rem_time=0;
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE, GPIO_ODR_OD8,GPIO_PIN_SET);
		if(GetTotalItemIndex()==0)
		{
			mute_rem_time=MUTE_REM_TIME_DELAY;
			mute_rem_start_flag=1;
		}
	}
	if ((vp_param_rw_str.aud_rep_dly<=0)&&(!vp_param_rw_str.AudiableRepeatDelaySwitch))
	{
		MUTE_TIME_DELAY=0;
	}
	else if(vp_param_rw_str.AudiableRepeatDelaySwitch)
	{
		MUTE_TIME_DELAY=(vp_param_rw_str.aud_rep_dly*60);
	}
	if ((vp_param_rw_str.rem_alm_dly<=0) &&(!vp_param_rw_str.RemoteAlarmDelaySwitch))
	{
		MUTE_REM_TIME_DELAY=0;
	}
	else if(vp_param_rw_str.RemoteAlarmDelaySwitch)
	{
		MUTE_REM_TIME_DELAY=(vp_param_rw_str.rem_alm_dly*60);
	}

	if(GetTotalUIItemIndex())
		SensorReading.Warning = SwapBytes(1);
	else
		SensorReading.Warning = SwapBytes(0);

	if(Time.TimeFormat)
	{
		SensorReading.time_am_pm = SwapBytes(1);
	}
	else
	{
		SensorReading.time_am_pm = SwapBytes(0);
	}
	if(UIDateTimeSettings.DateFormat)
	{
		SensorReading.dmyformat_en = SwapBytes(0);
		SensorReading.mdyformat_en = SwapBytes(1);
	}
	else
	{
		SensorReading.dmyformat_en = SwapBytes(1);
		SensorReading.mdyformat_en = SwapBytes(0);
	}
	if(mTorr>10)
	{
		SensorReading.Vaccum_sts = SwapBytes(0);
	}
	else
	{
		SensorReading.Vaccum_sts = SwapBytes(1);
	}


	SensorReading.FACT_RST_CNTDOWN = SwapBytes((int16_t)factory_load_delay);
	SensorReading.WIFI_runtime_test1 = SwapBytes((int16_t)rssi_value);


    memcpy(DataPtr, (uint8_t*)&SensorReading, sizeof(SensorReading));
    return sizeof(SensorReading);

}

/*
 * *****************************************************************************
 * Function name: SendUserActionEventData
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to frame user action event data to LCD
 * Parameters:
 *  Input  : Data Pointer to fill the settings data
 *  Return : No of bytes added
 * Note:
 *
 * *****************************************************************************
 */
uint8_t SendUserActionEventData( uint8_t *DataPtr)
{
	return 0;
}

/*
 * *****************************************************************************
 * Function name: DefaultTxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Tx handler Function, Not used
 * Parameters:
 *  Input  : Data pointer to fill data
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
uint8_t DefaultTxHandler( uint8_t *DataPtr)
{
	*DataPtr = 0x01;
	return 1;
}

