/*
 * UISettings.c
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#include "MyMain.h"
#include "DispVPAddrList.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
#include "DisplayTxHandlers.h"
#include "UserAccess.h"
#include "UISettings.h"
#include <stdlib.h>
#include "string.h"
#include "FactoryReset.h"

//#include "GraphData.h"

char CFFileName[16]="CTRS_CFC.bin";
uint8_t day_sel1=0x000;
uint8_t firmware_timer_en=0;
uint16_t Temp_group_col[32];
uint8_t prev_node_id=0;
extern uint8_t hours;

UPDATEStruct UpdateStruct;
GROUPStruct  GroupStruct;
SYSTEMStruct SystemStruct;
uint8_t update_error;
int8_t Group_temp_inactive_nodes[32];

extern SSID_Details Wifi_ssid[11];

TemperatureConvStruct TempSettingsConv;
FX_FILE CDC_SD_file;

char currentPath[MAX_PATH_LEN] = "/";  // Start at root
char g_filename_noext[MAX_NAME_LEN];

void add_folder(const char folderName[]) {
    if (strlen(currentPath) + strlen(folderName) + 1 < MAX_PATH_LEN) {
        if (strcmp(currentPath, "/") != 0) {
            strcat(currentPath, "/");
        }
        strcat(currentPath, folderName);
    }
}

void go_back() {
    char *lastSlash = strrchr(currentPath, '/');
    if (lastSlash && lastSlash != currentPath) {
        *lastSlash = '\0';  // Remove last folder
    } else {
        strcpy(currentPath, "/");  // Back to root
    }
}


void mutebuzzer(void)
{
	SendN16DataToDisplay(MUTE_KEY_VP_ADDRESS,MUTE_KEY_DISABLE);
	if(!mute_seq_start_flg)
	mute_seq_time=MUTE_TIME_DELAY;
	mute_seq_start_flg=1;
}
/*
 * *****************************************************************************
 * Function name: SaveAlarmSettings
  * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save temperature settings to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveAlarmSettings(AlarmSettingsStruct *AlarmSettings)
{
	uint8_t   bfr[2];

	vp_param_rw_str.RemoteAlarmDelaySwitch 		=  AlarmSettings->RemoteAlarmDelaySwitch;
	vp_param_rw_str.rem_alm_dly			 		=  AlarmSettings->RemoteAlarmDelay;
	vp_param_rw_str.AudiableRepeatDelaySwitch 	=  AlarmSettings->AudiableRepeatDelaySwitch;
	vp_param_rw_str.aud_rep_dly		 			=  AlarmSettings->AudiableRepeatDelay;

	vp_param_rw_str.LN2UsageAlarmDelaySwitch	=  AlarmSettings->LN2UsageDelaySwitch;
	vp_param_rw_str.usg_alm						=  AlarmSettings->LN2UsageAlarm_value;

	vp_param_rw_str.Battery_mode_sw_over        =  AlarmSettings->Battery_sw_over;
	MUTE_TIME_DELAY=(vp_param_rw_str.aud_rep_dly*60);
	mute_seq_time=MUTE_TIME_DELAY;
	MUTE_REM_TIME_DELAY=(vp_param_rw_str.rem_alm_dly*60);
	mute_rem_time=MUTE_REM_TIME_DELAY;

	EEPROM_Write(ALARM_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.RemoteAlarmDelaySwitch,14);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.RemoteAlarmDelaySwitch, 14);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(ALARM_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadAlarmSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load temperature settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadAlarmSettings(AlarmSettingsStruct *AlarmSettings)
{

	AlarmSettings->RemoteAlarmDelaySwitch=vp_param_rw_str.RemoteAlarmDelaySwitch;
	AlarmSettings->RemoteAlarmDelay=vp_param_rw_str.rem_alm_dly;
	AlarmSettings->AudiableRepeatDelaySwitch=vp_param_rw_str.AudiableRepeatDelaySwitch;
	AlarmSettings->AudiableRepeatDelay=vp_param_rw_str.aud_rep_dly;

	AlarmSettings->LN2UsageDelaySwitch = vp_param_rw_str.LN2UsageAlarmDelaySwitch;
	AlarmSettings->LN2UsageAlarm_value = vp_param_rw_str.usg_alm;

	 AlarmSettings->Battery_sw_over            =   vp_param_rw_str.Battery_mode_sw_over;
	if(Plumbing_options.Battery_status)
	{
		 SendN16DataToDisplay(BATT_BACK_EN_DIS, 1);
	}
	else
	{
		SendN16DataToDisplay(BATT_BACK_EN_DIS, 0);
	}
	MUTE_REM_TIME_DELAY=(vp_param_rw_str.rem_alm_dly*60);
	mute_rem_time=MUTE_REM_TIME_DELAY;

	SendVPDataCmdToDisplay(DISP_ALARM_SETTINGS_CMD);
	SendVPDataCmdToDisplay(DISP_ALARM_SETTINGS_CMD1);
}
/*
 * *****************************************************************************
 * Function name: SaveDispSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save temperature settings to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveDispSettings(DisplayDataStruct *DispSettings)
{
	uint8_t   bfr[2];
	vp_param_rw_str.STM_Brightness 				= DispSettings->Brightness;
	vp_param_rw_str.STM_TouchSound 				=  DispSettings->TouchSound;
	vp_param_rw_str.STM_ScreenSaverTime 		= DispSettings->ScreenSaverTime;
	vp_param_rw_str.STM_LED_Brightness 		    = DispSettings->LED_Brightness;
	vp_param_rw_str.STM_StatusLed				= DispSettings->StatusLEDEn;
//	vp_param_rw_str.STM_BatterSaver 			=  DispSettings->BatterSaver;
//	vp_param_rw_str.STM_Darkmode 				= DispSettings->Darkmode;
//	vp_param_rw_str.STM_Language 			    =  DispSettings->Language;

	UpdateScreenSaverSettings();

	EEPROM_Write(DISPLAY_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.STM_Brightness,12);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.STM_Brightness, 12);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(DISPLAY_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadDispSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load temperature settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadDispSettings(DisplayDataStruct *DispSettings)
{

	DispSettings->Brightness 		= vp_param_rw_str.STM_Brightness;
	DispSettings->TouchSound 		= vp_param_rw_str.STM_TouchSound;
	DispSettings->ScreenSaverTime 	= vp_param_rw_str.STM_ScreenSaverTime;
	if(DispSettings->LED_Brightness    != vp_param_rw_str.STM_LED_Brightness)
	{
		if(PCA9955B_SetGlobalBrightness(vp_param_rw_str.STM_LED_Brightness))
		{
			LED_disconnect=1;
		}
	}
	DispSettings->LED_Brightness    = vp_param_rw_str.STM_LED_Brightness;
	DispSettings->StatusLEDEn		= vp_param_rw_str.STM_StatusLed;
	SendVPDataCmdToDisplay(DISP_DISPLAY_SETTINGS_CMD);
	SendVPDataCmdToDisplay(DISP_DISPLAY_SETTINGS_CMD1);
}
/*
 * *****************************************************************************
 * Function name: SaveCryoSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save temperature settings to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveCryoSettings(ModbusDataStruct *CryoSettings)
{
	uint8_t   bfr[2];
//	vp_param_rw_str.STM_DeviceIDCryolink 	 = CryoSettings->DeviceIDCryolink;
//	vp_param_rw_str.STM_BaudRateCryolink 	 = CryoSettings->BaudRateCryolink;
//	vp_param_rw_str.STM_FPByteOrderCryolink	 = CryoSettings->FPByteOrderCryolink;

	EEPROM_Write(MODBUS_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.STM_DeviceIDCryolink,18);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.STM_DeviceIDCryolink, 18);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(MODBUS_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadCryoSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load temperature settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadCryoSettings(ModbusDataStruct *CryoSettings)
{
//	CryoSettings->DeviceIDCryolink 		= vp_param_rw_str.STM_DeviceIDCryolink;
//	CryoSettings->BaudRateCryolink 		= vp_param_rw_str.STM_BaudRateCryolink;
//	CryoSettings->FPByteOrderCryolink 	= vp_param_rw_str.STM_FPByteOrderCryolink;
//
//	SendVPDataCmdToDisplay(DISP_CRYO_SETTINGS_CMD);
}

/*
 * *****************************************************************************
 * Function name: SavePhaseSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save temperature settings to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SavePhaseSettings(ModbusDataStruct *PhaseSettings)
{
	uint8_t   bfr[2];
	if(vp_param_rw_str.STM_EN_4_20_mA  !=  PhaseSettings->EN_4_20_mA)
	{
	//	Set_delay(SW_TMR_4_20mACOMM_POLL, 90*667, (uint32_t*)&RS485_info.mA_communication);
	}
	vp_param_rw_str.STM_DeviceIDPhasetwo 	=  PhaseSettings->DeviceIDPhasetwo;
	vp_param_rw_str.STM_BaudRatePhasetwo 	=  PhaseSettings->BaudRatePhasetwo;
	vp_param_rw_str.DataRead_En 	        =  PhaseSettings->DataRead_En;
	//vp_param_rw_str.STM_FillMethodPhasetwo 	=  PhaseSettings->FillMethodPhasetwo;
//	vp_param_rw_str.Fill_cmd                =  PhaseSettings->Fill_Cmd;
//	vp_param_rw_str.STM_EN_4_20_mA          =  PhaseSettings->EN_4_20_mA;
//	memcpy((uint8_t*)&PhaseSettings->NwID, (uint8_t*)&NW_ID, sizeof(NW_ID));
	if(vp_param_rw_str.STM_DeviceIDPhasetwo==0)
	{
		SendN16DataToDisplay(MODBUS_MASTER_EN,1);
		SendN16DataToDisplay(MODBUS_SLAVE_EN,0);
		vp_param_rw_str.DataRead_En = 0;
		PhaseSettings->DataRead_En=0;
		SendN16DataToDisplay(MODBUS_DATA_RD,0);
		SendN16DataToDisplay(MODBUS_DATA_WR,0);
		prev_node_id=0;
	}
	else
	{
		if((vp_rt_wr_str.MBStatus == 0) || (vp_rt_wr_str.MBStatus == 2))
		{
			SendN16DataToDisplay(MODBUS_SLAVE_EN,1);
		}
		SendN16DataToDisplay(MODBUS_MASTER_EN,0);
		if(prev_node_id==0)
		{
		//	reset_Group_default();
			prev_node_id=PhaseSettings->DeviceIDPhasetwo;
		}
		SendN16DataToDisplay(No_devices,0);
//		vp_param_rw_str.STM_EN_4_20_mA=0;
//		SendN16DataToDisplay(EN_DIS_4_20mA_WR,0);
//		SendN16DataToDisplay(EN_DIS_4_20mA_RD,0);
	}
	if(vp_param_rw_str.STM_EN_4_20_mA)
	{
		if(vp_rt_wr_str.MBStatus==1)
		{
			vp_rt_wr_str.MBStatus=3;
		}
		else if(vp_rt_wr_str.MBStatus==0)
		{
			vp_rt_wr_str.MBStatus=2;
		}
	}
	else
	{
		if(vp_rt_wr_str.MBStatus==3)
		{
			vp_rt_wr_str.MBStatus=1;
		}
		else if(vp_rt_wr_str.MBStatus==2)
		{
			vp_rt_wr_str.MBStatus=0;
		}
	}
	SendN16DataToDisplay(Remote_local,vp_rt_wr_str.MBStatus);
	if(vp_param_rw_str.DataRead_En ==0)
	{
		vp_param_rw_str.Fill_cmd=0;
	//	PhaseSettings->Fill_Cmd=0;
//		SendN16DataToDisplay(MODBUS_FILLCMD_WR,vp_param_rw_str.Fill_cmd);
//		SendN16DataToDisplay(MODBUS_FILLCMD_RD,vp_param_rw_str.Fill_cmd);
	}

	if(vp_param_rw_str.STM_EN_4_20_mA)
	{
	//	SendN16DataToDisplay(Click4_20mA_en_dis,1);
	}
	else
	{
	//	SendN16DataToDisplay(Click4_20mA_en_dis,vp_param_rw_str.STM_EN_4_20_mA);
	}
//	EEPROM_Write(NW_ID_ADDRESS, 0,(uint8_t*)&PhaseSettings->NwID,8);
//	crc16 = usMBCRC16((uint8_t*)&PhaseSettings->NwID, 8);
//	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
//	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
//	EEPROM_Write(NW_ID_ADDRESS_CRC,0,bfr,2);
//	SendSTRDataToDisplay(NW_ID_WR,(uint8_t*)&PhaseSettings->NwID);

	EEPROM_Write(MODBUS_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.STM_DeviceIDCryolink,18);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.STM_DeviceIDCryolink, 18);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(MODBUS_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadPhaseSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load temperature settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadPhaseSettings(ModbusDataStruct *PhaseSettings)
{
	if(PhaseSettings->EN_4_20_mA   != vp_param_rw_str.STM_EN_4_20_mA)
	{
		if(vp_param_rw_str.STM_EN_4_20_mA)
		{
			SendN16DataToDisplay(Click4_20mA_en_dis,1);
		}
		else
		{
			SendN16DataToDisplay(Click4_20mA_en_dis,vp_param_rw_str.STM_EN_4_20_mA);
		}
	}
	PhaseSettings->DeviceIDPhasetwo 	= vp_param_rw_str.STM_DeviceIDPhasetwo;
	PhaseSettings->BaudRatePhasetwo 	= vp_param_rw_str.STM_BaudRatePhasetwo;
	PhaseSettings->DataRead_En      	= vp_param_rw_str.DataRead_En;
//	PhaseSettings->FillMethodPhasetwo 	= vp_param_rw_str.STM_FillMethodPhasetwo;
//	PhaseSettings->Fill_Cmd             = vp_param_rw_str.Fill_cmd;
//	PhaseSettings->EN_4_20_mA           = vp_param_rw_str.STM_EN_4_20_mA;

    if(vp_param_rw_str.STM_DeviceIDPhasetwo==0)
	{
		SendN16DataToDisplay(MODBUS_MASTER_EN,1);
		SendN16DataToDisplay(MODBUS_SLAVE_EN,0);
	}
	else
	{
		SendN16DataToDisplay(MODBUS_MASTER_EN,0);
		if((vp_rt_wr_str.MBStatus == 0) || (vp_rt_wr_str.MBStatus == 2))
		{
			SendN16DataToDisplay(MODBUS_SLAVE_EN,1);
		}
	}
    if(vp_param_rw_str.STM_EN_4_20_mA)
	{
		SendN16DataToDisplay(Click4_20mA_en_dis,1);
	}
	else
	{
		SendN16DataToDisplay(Click4_20mA_en_dis,vp_param_rw_str.STM_EN_4_20_mA);
	}
//	memcpy((uint8_t*)&NW_ID, (uint8_t*)&PhaseSettings->NwID, sizeof(NW_ID));
//	SendSTRDataToDisplay(NW_ID_WR,(uint8_t*)&PhaseSettings->NwID);
	SendVPDataCmdToDisplay(DISP_PHASE_SETTINGS_CMD);
	SendVPDataCmdToDisplay(DISP_PHASE_SETTINGS_CMD1);
}
/*
 * *****************************************************************************
 * Function name: SaveTempSettings
 * Created by :Venkatesh Arikanti
 * Date:       14-10-2025
 * Description:
 * 		Save temperature settings to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveTempSettings(TemperatureSettingsStruct *TemperatureSettings)
{
	uint8_t   bfr[2];
	float temp1_high=0,temp1_low=0,temp2_high=0,temp2_low=0;
	temp1_high=vp_param_rw_str.high_temp1.value;
	temp1_low=vp_param_rw_str.low_temp1.value;
	temp2_high=vp_param_rw_str.high_temp2.value;
	temp2_low=vp_param_rw_str.low_temp2.value;
	if(TemperatureSettings->TempUnitSelection)
	{
		vp_param_rw_str.high_temp1.value = (((float)TemperatureSettings->T1HighLimit)-320.0)/18.0;
		vp_param_rw_str.low_temp1.value = (((float)TemperatureSettings->T1LowLimit)-320.0)/18.0;
		vp_param_rw_str.high_temp2.value = (((float)TemperatureSettings->T2HighLimit)-320.0)/18.0;
		vp_param_rw_str.low_temp2.value = (((float)TemperatureSettings->T2LowLimit)-320.0)/18.0;
	}
	else
	{
		vp_param_rw_str.high_temp1.value = (float)TemperatureSettings->T1HighLimit/10.0;
		vp_param_rw_str.low_temp1.value = (float)TemperatureSettings->T1LowLimit/10.0;
		vp_param_rw_str.high_temp2.value = (float)TemperatureSettings->T2HighLimit/10.0;
		vp_param_rw_str.low_temp2.value = (float)TemperatureSettings->T2LowLimit/10.0;
	}
	TempSettingsConv.T1HighLimitFl = (float)TemperatureSettings->T1HighLimit;
	TempSettingsConv.T1LowLimitFl = (float)TemperatureSettings->T1LowLimit;
	TempSettingsConv.T2HighLimitFl = (float)TemperatureSettings->T2HighLimit;
	TempSettingsConv.T2LowLimitFl = (float)TemperatureSettings->T2LowLimit;

	vp_param_rw_str.tempUnits = (uint8_t)TemperatureSettings->TempUnitSelection;
	vp_param_rw_str.high_temp1.units = (uint8_t)TemperatureSettings->TempUnitSelection;
	vp_param_rw_str.low_temp1.units = (uint8_t)TemperatureSettings->TempUnitSelection;
	vp_param_rw_str.high_temp2.units = (uint8_t)TemperatureSettings->TempUnitSelection;
	vp_param_rw_str.low_temp2.units = (uint8_t)TemperatureSettings->TempUnitSelection;
	vp_param_rw_str.t2enable = (uint8_t)TemperatureSettings->T2Enable;
	if(vp_param_rw_str.t2enable==1)
	{
		  SendN16DataToDisplay(T2_HEAADER_EN, 1);
		//  SendN16DataToDisplay(T2_EN_DIS_HOME, 1);
		  SendN16DataToDisplay(T2_STATUS_GOOD_BAD, 1);
    }
	else
	{
		SendN16DataToDisplay(T2_HEAADER_EN, 0);
	//	SendN16DataToDisplay(T2_EN_DIS_HOME, 0);
		SendN16DataToDisplay(T2_STATUS_GOOD_BAD, 0);
	}

	vp_param_rw_str.high_temp1.is_enabled = (uint8_t)TemperatureSettings->T1HighLimitSwitch;
	vp_param_rw_str.low_temp1.is_enabled = (uint8_t)TemperatureSettings->T1LowLimitSwitch;
	vp_param_rw_str.high_temp2.is_enabled = (uint8_t)TemperatureSettings->T2HighLimitSwitch;
	vp_param_rw_str.low_temp2.is_enabled = (uint8_t)TemperatureSettings->T2LowLimitSwitch;

	if((UIwifi.IOT_connect_sts) && (temp1_high != vp_param_rw_str.high_temp1.value || temp1_low != vp_param_rw_str.low_temp1.value || temp2_high != vp_param_rw_str.high_temp2.value || temp2_low != vp_param_rw_str.low_temp2.value))
	{
		if(vp_rt_wr_str.WiFiStatus!=0)
		{
			SendN16DataToDisplay(AWS_IOT_CNCTVTY,0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD,0);
			vp_rt_wr_str.WiFiStatus=2;
		}
		//write down the URL details into SD card.
	}
	EEPROM_Write(TEMP_RTD_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.high_temp1.value,34);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.high_temp1.value, 34);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(TEMP_RTD_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadTempSettings
 * Created by : Venkatesh Arikanti
 * Date:       14-10-2025
 * Description:
 * 		Load temperature settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadTempSettings(TemperatureSettingsStruct *TemperatureSettings)
{
	if(vp_param_rw_str.tempUnits)
	{
		TemperatureSettings->T1HighLimit = (int16_t)(((vp_param_rw_str.high_temp1.value*1.8)+32)*10);
		TemperatureSettings->T1LowLimit = (int16_t)(((vp_param_rw_str.low_temp1.value*1.8)+32)*10);
		TemperatureSettings->T2HighLimit = (int16_t)(((vp_param_rw_str.high_temp2.value*1.8)+32)*10);
		TemperatureSettings->T2LowLimit = (int16_t)(((vp_param_rw_str.low_temp2.value*1.8)+32)*10);
	}
	else
	{
		TemperatureSettings->T1HighLimit = (int16_t)(vp_param_rw_str.high_temp1.value*10);
		TemperatureSettings->T1LowLimit = (int16_t)(vp_param_rw_str.low_temp1.value*10);
		TemperatureSettings->T2HighLimit = (int16_t)(vp_param_rw_str.high_temp2.value*10);
		TemperatureSettings->T2LowLimit = (int16_t)(vp_param_rw_str.low_temp2.value*10);
	}
	TempSettingsConv.T1HighLimitFl = (float)TemperatureSettings->T1HighLimit;
	TempSettingsConv.T1LowLimitFl = (float)TemperatureSettings->T1LowLimit;
	TempSettingsConv.T2HighLimitFl = (float)TemperatureSettings->T2HighLimit;
	TempSettingsConv.T2LowLimitFl = (float)TemperatureSettings->T2LowLimit;

	PrevTempUnit = vp_param_rw_str.tempUnits;
	TemperatureSettings->T1HighLimitSwitch = vp_param_rw_str.high_temp1.is_enabled;
	TemperatureSettings->T1LowLimitSwitch = vp_param_rw_str.low_temp1.is_enabled;
	TemperatureSettings->T2HighLimitSwitch = vp_param_rw_str.high_temp2.is_enabled;
	TemperatureSettings->T2LowLimitSwitch = vp_param_rw_str.low_temp2.is_enabled;

	TemperatureSettings->TempUnitSelection = ((int16_t)vp_param_rw_str.high_temp1.units);
	TemperatureSettings->T2Enable = vp_param_rw_str.t2enable;
	TemperatureSettings->TempUnitSelection = vp_param_rw_str.tempUnits;
	SendVPDataCmdToDisplay(DISP_TEMP_SETTINGS_CMD);
    SendVPDataCmdToDisplay(DISP_TEMP_SETTINGS_CMD1);
	if(vp_param_rw_str.t2enable==1)
	 {
		 SendN16DataToDisplay(T2_HEAADER_EN, 1);				//Need to comeback

	 }
	else{
		SendN16DataToDisplay(T2_HEAADER_EN, 0);					//Need to comeback
	}
}


/*
 * *****************************************************************************
 * Function name: SaveLevelSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save Level settings to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveLevelSettings(LEVELSettingsStruct *UILevelSettingsData)
{
	uint8_t   bfr[2];
	vp_param_rw_str.Empty_en 	     = UILevelSettingsData->EMPTY_EN;
	vp_param_rw_str.Refillsoon_en 	 = UILevelSettingsData->REFILLSOON;
	vp_param_rw_str.Fill_en	         = UILevelSettingsData->FILL;

	if(vp_param_rw_str.Empty_en)
	{
		LevelData.Enable_thermistor |= THERMISTOR_1_LEVEL_MASK;
	}
	else
	{
		LevelData.Enable_thermistor &= ~THERMISTOR_1_LEVEL_MASK;
	}
	if(vp_param_rw_str.Refillsoon_en)
	{
		LevelData.Enable_thermistor |= THERMISTOR_2_LEVEL_MASK;
	}
	else
	{
		LevelData.Enable_thermistor &= ~THERMISTOR_2_LEVEL_MASK;
	}
	if(vp_param_rw_str.Fill_en)
	{
		LevelData.Enable_thermistor |= THERMISTOR_3_LEVEL_MASK;
	}
	else
	{
		LevelData.Enable_thermistor &= ~THERMISTOR_3_LEVEL_MASK;
	}

	EEPROM_Write(LEVEL_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.Empty_en,6);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Empty_en, 6);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(LEVEL_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadLevelSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load LEvel settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadLevelSettings(LEVELSettingsStruct *UILevelSettingsData)
{
	UILevelSettingsData->EMPTY_EN 		= vp_param_rw_str.Empty_en;
	UILevelSettingsData->REFILLSOON 	= vp_param_rw_str.Refillsoon_en;
	UILevelSettingsData->FILL 	        = vp_param_rw_str.Fill_en;
	if(vp_param_rw_str.Empty_en)
	{
		LevelData.Enable_thermistor |= THERMISTOR_1_LEVEL_MASK;
	}
	else
	{
		LevelData.Enable_thermistor &= ~THERMISTOR_1_LEVEL_MASK;
	}
	if(vp_param_rw_str.Refillsoon_en)
	{
		LevelData.Enable_thermistor |= THERMISTOR_2_LEVEL_MASK;
	}
	else
	{
		LevelData.Enable_thermistor &= ~THERMISTOR_2_LEVEL_MASK;
	}
	if(vp_param_rw_str.Fill_en)
	{
		LevelData.Enable_thermistor |= THERMISTOR_3_LEVEL_MASK;
	}
	else
	{
		LevelData.Enable_thermistor &= ~THERMISTOR_3_LEVEL_MASK;
	}
	SendVPDataCmdToDisplay(DISP_LEVEL_SETTINGS_CMD);
	SendVPDataCmdToDisplay(DISP_LEVEL_SETTINGS_CMD1);
}

void SaveLedSettings(LEDSettingsStruct *UILEDSettingsData)
{
	uint8_t bfr[2];

	vp_param_rw_str.ERRORS_ONLY_en = UILEDSettingsData->ERRORS_ONLY;
	vp_param_rw_str.TEMP_en = UILEDSettingsData->TEMP;
	vp_param_rw_str.LEVEL_en = UILEDSettingsData->LEVEL;
	vp_param_rw_str.VACUUM_en = UILEDSettingsData->VACUUM;
	vp_param_rw_str.STORAGE_FAIL_en = UILEDSettingsData->STORAGE_FAIL;
	vp_param_rw_str.MAINS_BATT_en = UILEDSettingsData->MAINS_BATT;
	vp_param_rw_str.NETWORK_ISSUES_en = UILEDSettingsData->NETWORK_ISSUES;
	vp_param_rw_str.BLINK_CTRL_en = UILEDSettingsData->BLINK_CTRL;
	vp_param_rw_str.NO_ERROR_CLR = UILEDSettingsData->NO_ERROR;
	vp_param_rw_str.ERRORS_CLR = UILEDSettingsData->ERRORS;
	vp_param_rw_str.WARNINGS_CLR = UILEDSettingsData->WARNINGS;
	vp_param_rw_str.UPDATES_CLR = UILEDSettingsData->UPDATES;
	vp_param_rw_str.NEW_COLOR_Select = UILEDSettingsData->NEW_COLOR;


	EEPROM_Write(LED_ADDRESS, 0, (uint8_t *)&vp_param_rw_str.ERRORS_ONLY_en, 26);
	crc16 = usMBCRC16((uint8_t *)&vp_param_rw_str.ERRORS_ONLY_en, 26);
	bfr[0] = ((crc16 & 0xFF00) >> 8);
	bfr[1] = (crc16 & 0x00FF);
	EEPROM_Write(LED_ADDRESS_CRC, 0, bfr, 2);
}
void LoadLedSettings(LEDSettingsStruct *UILEDSettingsData)
{
	UILEDSettingsData->ERRORS_ONLY = vp_param_rw_str.ERRORS_ONLY_en;
	UILEDSettingsData->TEMP = vp_param_rw_str.TEMP_en;
	UILEDSettingsData->LEVEL = vp_param_rw_str.LEVEL_en;
	UILEDSettingsData->VACUUM = vp_param_rw_str.VACUUM_en;
	UILEDSettingsData->STORAGE_FAIL = vp_param_rw_str.STORAGE_FAIL_en;
	UILEDSettingsData->MAINS_BATT = vp_param_rw_str.MAINS_BATT_en;
	UILEDSettingsData->NETWORK_ISSUES = vp_param_rw_str.NETWORK_ISSUES_en;
	UILEDSettingsData->BLINK_CTRL = vp_param_rw_str.BLINK_CTRL_en;
	UILEDSettingsData->NO_ERROR = vp_param_rw_str.NO_ERROR_CLR;
	UILEDSettingsData->ERRORS = vp_param_rw_str.ERRORS_CLR;
	UILEDSettingsData->WARNINGS = vp_param_rw_str.WARNINGS_CLR;
	UILEDSettingsData->UPDATES = vp_param_rw_str.UPDATES_CLR;
	UILEDSettingsData->NEW_COLOR = vp_param_rw_str.NEW_COLOR_Select;

	SendVPDataCmdToDisplay(DISP_LED_SETTINGS_CMD);
	SendVPDataCmdToDisplay(DISP_LED_SETTINGS_CMD1);
}
/*
 * *****************************************************************************
 * Function name: SaveScheduledSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveScheduledSettings(ScheduledEventStruct *ScheduledSettings)
{
	 uint8_t   bfr[2];// *bPtr = &bfr[0];
	vp_param_rw_str.ScheduledFillRemainderEn 	= ScheduledSettings->ScheduledFillRemainderEn;
	vp_param_rw_str.MaintRemainderEn 			= ScheduledSettings->MaintRemainderEn;
	vp_param_rw_str.FillRemainderEn 		    = ScheduledSettings->FillRemainderEn;


	EEPROM_Write(EEPROM_SCHEDULED_EN_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.ScheduledFillRemainderEn,8);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.ScheduledFillRemainderEn, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(EEPROM_SCHEDULED_EN_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadScheduledSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load level settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadScheduledSettings(ScheduledEventStruct *ScheduledSettings)
{
	//EEPROM_Read(EEPROM_SCHEDULED_EN_ADDRESS, 0, (uint8_t *)vp_param_rw_str.ScheduledFillRemainderEn, 8);
	ScheduledSettings->ScheduledFillRemainderEn = vp_param_rw_str.ScheduledFillRemainderEn;
	ScheduledSettings->MaintRemainderEn 		= vp_param_rw_str.MaintRemainderEn;
	ScheduledSettings->FillRemainderEn 	= vp_param_rw_str.FillRemainderEn;


	SendVPDataCmdToDisplay(DISP_SCEDULEDEVENTS_CMD);
	SendVPDataCmdToDisplay(DISP_SCEDULEDEVENTS_CMD1);
}

//rv
/*
 * *****************************************************************************
 * Function name: SaveScheduledFillSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveScheduledFillSettings(RemainderSettingsStruct *ScheduledFillSettings)
{
	uint8_t   bfr[2];
	if(vp_param_rw_str.RTC_TimeFormat)
	{
		if(ScheduledFillSettings->HoursSettings==0)
		vp_param_rw_str.Fill_Hours12Settings 			= (ScheduledFillSettings->HoursSettings+12);
		else if(ScheduledFillSettings->HoursSettings>12)
		vp_param_rw_str.Fill_Hours12Settings 			= (ScheduledFillSettings->HoursSettings-12);
		else
		vp_param_rw_str.Fill_Hours12Settings 			= (ScheduledFillSettings->HoursSettings);

	//	vp_param_rw_str.Fill_Minutes12Settings			= ScheduledFillSettings->MinutesSettings;
		if(ScheduledFillSettings->HoursSettings>=12)
		vp_param_rw_str.Fill_AMPMSettings				= 1;
		else
		vp_param_rw_str.Fill_AMPMSettings				= 0;

		vp_param_rw_str.Fill_HoursSettings				= ScheduledFillSettings->HoursSettings;
	}
	else
	{
		if(ScheduledFillSettings->AMPMSettings==1)
		{
			if(ScheduledFillSettings->Hours12Settings<12)
			vp_param_rw_str.Fill_HoursSettings			= (ScheduledFillSettings->Hours12Settings+12);
			else
			vp_param_rw_str.Fill_HoursSettings			= (ScheduledFillSettings->Hours12Settings);
		}
		else
		{
			if(ScheduledFillSettings->Hours12Settings>=12)
			vp_param_rw_str.Fill_HoursSettings			= (ScheduledFillSettings->Hours12Settings-12);
			else
			vp_param_rw_str.Fill_HoursSettings			= (ScheduledFillSettings->Hours12Settings);
		}

		vp_param_rw_str.Fill_Hours12Settings 			= ScheduledFillSettings->Hours12Settings;

		vp_param_rw_str.Fill_AMPMSettings 				= ScheduledFillSettings->AMPMSettings;
	}
	vp_param_rw_str.Fill_DaySettings 					= ScheduledFillSettings->DaySettings;
	vp_param_rw_str.Fill_MinutesSettings 			= ScheduledFillSettings->MinutesSettings;

	EEPROM_Write(SCHEDULED_FILL_ADDRESS,0,(uint8_t*)&vp_param_rw_str.Fill_HoursSettings,12);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Fill_HoursSettings, 12);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(SCHEDULED_FILL_ADDRESS_CRC,0,bfr,2);
	//date_rem.date_sch_fill=0;
	//EEPROM_Write(SCH_DATE,0,&date_rem.date_sch_fill,1);


}

/*
 * *****************************************************************************
 * Function name: LoadScheduledFillSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load level settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadScheduledFillSettings(RemainderSettingsStruct *ScheduledFillSettings)
{
	ScheduledFillSettings->HoursSettings 		= vp_param_rw_str.Fill_HoursSettings;
	ScheduledFillSettings->MinutesSettings 		= vp_param_rw_str.Fill_MinutesSettings;
	ScheduledFillSettings->AMPMSettings 		= vp_param_rw_str.Fill_AMPMSettings;
	ScheduledFillSettings->DaySettings 			= vp_param_rw_str.Fill_DaySettings;
	ScheduledFillSettings->Hours12Settings 		= vp_param_rw_str.Fill_Hours12Settings;

	if(UIScheduledFillData.DaySettings & 0x01)
	{
		SendN16DataToDisplay(SCHEDULED_FILL_EN_DIS_WEEKDAYS, 0);	//0801ca old address
	}
	else
	{
		SendN16DataToDisplay(SCHEDULED_FILL_EN_DIS_WEEKDAYS, 1);
	}
	SendVPDataCmdToDisplay(DISP_SCHEDULED_FILL_CMD);
	SendVPDataCmdToDisplay(Day_shed);
	SendVPDataCmdToDisplay(Day_shed1);
}
//
/*
 * *****************************************************************************
 * Function name: SaveMaintenenceSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveRTCSettings(DateTimeSettingsStruct *DateTimeSettings)
{
	uint8_t   bfr[2];
	if(DateTimeSettings->TimeFormat)
	{
		if(DateTimeSettings->HoursSettings==0)
		vp_param_rw_str.RTC_Hours12Settings 		= (DateTimeSettings->HoursSettings+12);
		else if(DateTimeSettings->HoursSettings>12)
		vp_param_rw_str.RTC_Hours12Settings 		= (DateTimeSettings->HoursSettings-12);
		else
		vp_param_rw_str.RTC_Hours12Settings 		= (DateTimeSettings->HoursSettings);

		if(DateTimeSettings->HoursSettings>=12)
		vp_param_rw_str.RTC_AMPMSettings			= 1;
		else
		vp_param_rw_str.RTC_AMPMSettings			= 0;

		vp_param_rw_str.RTC_HoursSettings 			= DateTimeSettings->HoursSettings;

	}
	else
	{
		if(DateTimeSettings->AMPMSettings==1)
		{
			if(DateTimeSettings->Hours12Settings<12)
			vp_param_rw_str.RTC_HoursSettings	= (DateTimeSettings->Hours12Settings+12);
			else
			vp_param_rw_str.RTC_HoursSettings	= (DateTimeSettings->Hours12Settings);
		}
		else
		{
			if(DateTimeSettings->Hours12Settings>=12)
			vp_param_rw_str.RTC_HoursSettings	= (DateTimeSettings->Hours12Settings-12);
			else
			vp_param_rw_str.RTC_HoursSettings	= (DateTimeSettings->Hours12Settings);
		}


		vp_param_rw_str.RTC_Hours12Settings 		= DateTimeSettings->Hours12Settings;

		vp_param_rw_str.RTC_AMPMSettings 			= DateTimeSettings->AMPMSettings;
	}
	vp_param_rw_str.RTC_MinutesSettings 		= DateTimeSettings->MinutesSettings;
	vp_param_rw_str.RTC_DateFormat 				= DateTimeSettings->DateFormat;
	vp_param_rw_str.RTC_TimeFormat 				= DateTimeSettings->TimeFormat;
	vp_param_rw_str.RTC_MonthSettings 			= DateTimeSettings->MonthSettings;
	vp_param_rw_str.RTC_DateSettings 			= DateTimeSettings->DateSettings;
	int8_t time_dif=vp_param_rw_str.RTC_YearSettings -DateTimeSettings->YearSettings;
	if(0)//(time_dif>5)
	{
		vp_param_rw_str.RTC_YearSettings 			= vp_param_rw_str.RTC_YearSettings-5;
		DateTimeSettings->YearSettings              =vp_param_rw_str.RTC_YearSettings;
		SendN16DataToDisplay(RTC_YEAR_VALUE_WR, vp_param_rw_str.RTC_YearSettings);
	}
	else if(0)//(time_dif<-5)
	{
		vp_param_rw_str.RTC_YearSettings 			= vp_param_rw_str.RTC_YearSettings+5;
		DateTimeSettings->YearSettings              =vp_param_rw_str.RTC_YearSettings;
		SendN16DataToDisplay(RTC_YEAR_VALUE_WR, vp_param_rw_str.RTC_YearSettings);
	}
	else
	{
		vp_param_rw_str.RTC_YearSettings 			= DateTimeSettings->YearSettings;
	}

	EEPROM_Write(RTC_ADDRESS,0,(uint8_t*)&vp_param_rw_str.RTC_AutoTimeSyncControl,6);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.RTC_AutoTimeSyncControl, 6);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(RTC_ADDRESS_CRC,0,bfr,2);

	UI_RTC_set_flg=1;
	if(!just_for_display_flg)
	{
		RTC_TimeTypeDef  PrevTime;
		RTC_DateTypeDef  PrevDate;

		PrevTime.Hours = Time.Hours;
		PrevTime.Minutes = Time.Minutes;
		PrevTime.Seconds = Time.Seconds;
		PrevTime.TimeFormat = Time.TimeFormat;

		PrevDate.Date = Date.Date;
		PrevDate.Month = Date.Month;
		PrevDate.Year = Date.Year;

		Set_TimeDate();
		//ResyncGraphData(Date, Time, PrevDate, PrevTime);
//		if((Date.Date != PrevDate.Date)||(Date.Month != PrevDate.Month)||(Date.Year != PrevDate.Year))
//		{
//
//		}
	}
}

/*
 * *****************************************************************************
 * Function name: LoadMaintenenceSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load level settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadRTCSettings(DateTimeSettingsStruct *DateTimeSettings)
{


	DateTimeSettings->DateFormat 			= vp_param_rw_str.RTC_DateFormat;
	DateTimeSettings->TimeFormat 			= vp_param_rw_str.RTC_TimeFormat;
	PrevTimeFormat = UIDateTimeSettings.TimeFormat;

	SendVPDataCmdToDisplay(DISP_DATE_TIME_SETTINGS_CMD);
	SendVPDataCmdToDisplay(DISP_DATE_TIME_SETTINGS_CMD1);
	vp_param_rw_str.RTC_Hours12Settings     = DateTimeSettings->Hours12Settings ;

	vp_param_rw_str.RTC_MonthSettings       = DateTimeSettings->MonthSettings;
	vp_param_rw_str.RTC_DateSettings        = DateTimeSettings->DateSettings;
	vp_param_rw_str.RTC_YearSettings        = DateTimeSettings->YearSettings ;
	vp_param_rw_str.RTC_HoursSettings       = DateTimeSettings->HoursSettings;
	vp_param_rw_str.RTC_MinutesSettings     = DateTimeSettings->MinutesSettings;
	vp_param_rw_str.RTC_AMPMSettings        = DateTimeSettings->AMPMSettings;

}
/*
 * *****************************************************************************
 * Function name: SaveMaintenenceSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveMaintenenceSettings(MaintenanceRemainderSettingsStruct *MaintenanceRemainderSettings)
{
	uint8_t   bfr[2];
	if(vp_param_rw_str.RTC_TimeFormat)
	{
		if(MaintenanceRemainderSettings->HoursSettings==0)
		vp_param_rw_str.Maint_Hours12Settings 		= (MaintenanceRemainderSettings->HoursSettings+12);
		else if(MaintenanceRemainderSettings->HoursSettings>12)
		vp_param_rw_str.Maint_Hours12Settings 		= (MaintenanceRemainderSettings->HoursSettings-12);
		else
		vp_param_rw_str.Maint_Hours12Settings 		= (MaintenanceRemainderSettings->HoursSettings);

		if(MaintenanceRemainderSettings->HoursSettings>=12)
		vp_param_rw_str.Maint_AMPMSettings			= 1;
		else
		vp_param_rw_str.Maint_AMPMSettings			= 0;

		vp_param_rw_str.Maint_HoursSettings			= MaintenanceRemainderSettings->HoursSettings;
	}
	else
	{
		if(MaintenanceRemainderSettings->AMPMSettings==1)
		{
			if(MaintenanceRemainderSettings->Hours12Settings<12)
			vp_param_rw_str.Maint_HoursSettings	= (MaintenanceRemainderSettings->Hours12Settings+12);
			else
			vp_param_rw_str.Maint_HoursSettings	= (MaintenanceRemainderSettings->Hours12Settings);
		}
		else
		{
			if(MaintenanceRemainderSettings->Hours12Settings>=12)
			vp_param_rw_str.Maint_HoursSettings	= (MaintenanceRemainderSettings->Hours12Settings-12);
			else
			vp_param_rw_str.Maint_HoursSettings	= (MaintenanceRemainderSettings->Hours12Settings);
		}
	//	vp_param_rw_str.Maint_MinutesSettings	= MaintenanceRemainderSettings->Minutes12Settings;

		vp_param_rw_str.Maint_Hours12Settings 		= MaintenanceRemainderSettings->Hours12Settings;
	//	vp_param_rw_str.Maint_Minutes12Settings		= MaintenanceRemainderSettings->Minutes12Settings;
		vp_param_rw_str.Maint_AMPMSettings 			= MaintenanceRemainderSettings->AMPMSettings;
	}
	vp_param_rw_str.Maint_MinutesSettings 		= MaintenanceRemainderSettings->MinutesSettings;
	vp_param_rw_str.Maint_MonthSettings 		= MaintenanceRemainderSettings->MonthSettings;
	vp_param_rw_str.Maint_DateSettings 			= MaintenanceRemainderSettings->DateSettings;
	vp_param_rw_str.Maint_YearSettings 			= MaintenanceRemainderSettings->YearSettings;

	EEPROM_Write(SCHEDULED_MAINT_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.Maint_Hours12Settings,16);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Maint_Hours12Settings,16);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(SCHEDULED_MAINT_ADDRESS_CRC,0,bfr,2);
}

/*
 * *****************************************************************************
 * Function name: LoadMaintenenceSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load level settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadMaintenenceSettings(MaintenanceRemainderSettingsStruct *MaintenanceRemainderSettings)
{
	MaintenanceRemainderSettings->Hours12Settings 	= vp_param_rw_str.Maint_Hours12Settings;
//	MaintenanceRemainderSettings->Minutes12Settings = vp_param_rw_str.Maint_Minutes12Settings;
	MaintenanceRemainderSettings->MonthSettings 	= vp_param_rw_str.Maint_MonthSettings;
	MaintenanceRemainderSettings->DateSettings 		= vp_param_rw_str.Maint_DateSettings;
	MaintenanceRemainderSettings->YearSettings 		= vp_param_rw_str.Maint_YearSettings;
	MaintenanceRemainderSettings->HoursSettings 	= vp_param_rw_str.Maint_HoursSettings;
	MaintenanceRemainderSettings->MinutesSettings 	= vp_param_rw_str.Maint_MinutesSettings;
	MaintenanceRemainderSettings->AMPMSettings 		= vp_param_rw_str.Maint_AMPMSettings;

	SendVPDataCmdToDisplay(DISP_MAINT_REMAINDER_CMD);
}

/*
 * *****************************************************************************
 * Function name: SaveScheduledFillRemSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveScheduledFillRemSettings(SCHRemainderSettingsStruct *ScheduledRLevelSettings)
{
	uint8_t   bfr[2];
	if(vp_param_rw_str.RTC_TimeFormat)
	{
		if(ScheduledRLevelSettings->Hours24Settings==0)
		vp_param_rw_str.FILLR_Hours12Settings 			= (ScheduledRLevelSettings->Hours24Settings+12);
		else if(ScheduledRLevelSettings->Hours24Settings>12)
		vp_param_rw_str.FILLR_Hours12Settings 			= (ScheduledRLevelSettings->Hours24Settings-12);
		else
		vp_param_rw_str.FILLR_Hours12Settings 			= (ScheduledRLevelSettings->Hours24Settings);

		if(ScheduledRLevelSettings->Hours24Settings>=12)
		vp_param_rw_str.FILLR_AMPMSettings				= 1;
		else
		vp_param_rw_str.FILLR_AMPMSettings				= 0;

		vp_param_rw_str.FILLR_HoursSettings			= ScheduledRLevelSettings->Hours24Settings;

	}
	else
	{
		if(ScheduledRLevelSettings->AMPMSettings==1)
		{
			if(ScheduledRLevelSettings->Hours12Settings<12)
			vp_param_rw_str.FILLR_HoursSettings		= (ScheduledRLevelSettings->Hours12Settings+12);
			else
			vp_param_rw_str.FILLR_HoursSettings		= (ScheduledRLevelSettings->Hours12Settings);
		}
		else
		{
			if(ScheduledRLevelSettings->Hours12Settings>=12)
			vp_param_rw_str.FILLR_HoursSettings		= (ScheduledRLevelSettings->Hours12Settings-12);
			else
			vp_param_rw_str.FILLR_HoursSettings		= (ScheduledRLevelSettings->Hours12Settings);
		}
//		vp_param_rw_str.CLevel_MinutesSettings			= ScheduledRLevelSettings->Minutes12Settings;

		vp_param_rw_str.FILLR_Hours12Settings 			= ScheduledRLevelSettings->Hours12Settings;
//		vp_param_rw_str.CLevel_Minutes12Settings		= ScheduledRLevelSettings->Minutes12Settings;
		vp_param_rw_str.FILLR_AMPMSettings 			= ScheduledRLevelSettings->AMPMSettings;
	}
	vp_param_rw_str.FILLR_MinutesSettings 			= ScheduledRLevelSettings->MinutesSettings;
	vp_param_rw_str.FILLR_MonthSettings 			= ScheduledRLevelSettings->MonthSettings;
	vp_param_rw_str.FILLR_DateSettings 			    = ScheduledRLevelSettings->DateSettings;
	vp_param_rw_str.FILLR_YearSettings 			    = ScheduledRLevelSettings->YearSettings;

	EEPROM_Write(SCHEDULED_RLEVEL_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.FILLR_MonthSettings,14);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.FILLR_MonthSettings, 14);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(SCHEDULED_RLEVEL_ADDRESS_CRC,0,bfr,2);

	//date_rem.date_chk=0;
	//EEPROM_Write(LN2_CHK_DATE,0,&date_rem.date_chk,1);


}

/*
 * *****************************************************************************
 * Function name: LoadScheduledFillRemSettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Load level settings from application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadScheduledFillRemSettings(SCHRemainderSettingsStruct *ScheduledRLevelSettings)
{
	ScheduledRLevelSettings->MonthSettings 	= vp_param_rw_str.FILLR_MonthSettings;
	ScheduledRLevelSettings->DateSettings 		= vp_param_rw_str.FILLR_DateSettings;
	ScheduledRLevelSettings->YearSettings 		= vp_param_rw_str.FILLR_YearSettings;
	ScheduledRLevelSettings->Hours24Settings 		= vp_param_rw_str.FILLR_HoursSettings;
	ScheduledRLevelSettings->MinutesSettings 	= vp_param_rw_str.FILLR_MinutesSettings;
	ScheduledRLevelSettings->AMPMSettings 		= vp_param_rw_str.FILLR_AMPMSettings;
	ScheduledRLevelSettings->Hours12Settings 	= vp_param_rw_str.FILLR_Hours12Settings;

	SendVPDataCmdToDisplay(DISP_Fill_REMAINDER_CMD);
}
/*
 * *****************************************************************************
 * Function name: SaveScheduledLN2SupplySettings
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
//void SaveScheduledLN2SupplySettings(RemainderSettingsStruct *ScheduledLN2SupplySettings)
//{
//	uint8_t   bfr[2];
//	if(vp_param_rw_str.RTC_TimeFormat)
//	{
//		if(ScheduledLN2SupplySettings->HoursSettings==0)
//		vp_param_rw_str.LN2Supply_Hours12Settings 			= (ScheduledLN2SupplySettings->HoursSettings+12);
//		else if(ScheduledLN2SupplySettings->HoursSettings>12)
//		vp_param_rw_str.LN2Supply_Hours12Settings 			= (ScheduledLN2SupplySettings->HoursSettings-12);
//		else
//		vp_param_rw_str.LN2Supply_Hours12Settings 			= (ScheduledLN2SupplySettings->HoursSettings);
//
//		vp_param_rw_str.LN2Supply_Minutes12Settings		= ScheduledLN2SupplySettings->MinutesSettings;
//		if(ScheduledLN2SupplySettings->HoursSettings>=12)
//		vp_param_rw_str.LN2Supply_AMPMSettings				= 1;
//		else
//		vp_param_rw_str.LN2Supply_AMPMSettings				= 0;
//
//		vp_param_rw_str.LN2Supply_HoursSettings			= ScheduledLN2SupplySettings->HoursSettings;
//		vp_param_rw_str.LN2Supply_MinutesSettings 			= ScheduledLN2SupplySettings->MinutesSettings;
//	}
//	else
//	{
//		if(ScheduledLN2SupplySettings->AMPMSettings==1)
//		{
//			if(ScheduledLN2SupplySettings->Hours12Settings<12)
//			vp_param_rw_str.LN2Supply_HoursSettings			= (ScheduledLN2SupplySettings->Hours12Settings+12);
//			else
//			vp_param_rw_str.LN2Supply_HoursSettings			= (ScheduledLN2SupplySettings->Hours12Settings);
//		}
//		else
//		{
//			if(ScheduledLN2SupplySettings->Hours12Settings>=12)
//			vp_param_rw_str.LN2Supply_HoursSettings			= (ScheduledLN2SupplySettings->Hours12Settings-12);
//			else
//			vp_param_rw_str.LN2Supply_HoursSettings			= (ScheduledLN2SupplySettings->Hours12Settings);
//		}
////		vp_param_rw_str.LN2Supply_MinutesSettings			= ScheduledLN2SupplySettings->Minutes12Settings;
//
//		vp_param_rw_str.LN2Supply_Hours12Settings 			= ScheduledLN2SupplySettings->Hours12Settings;
////		vp_param_rw_str.LN2Supply_Minutes12Settings			= ScheduledLN2SupplySettings->Minutes12Settings;
//		vp_param_rw_str.LN2Supply_AMPMSettings 				= ScheduledLN2SupplySettings->AMPMSettings;
//	}
//
//	vp_param_rw_str.LN2Supply_DaySettings 					= ScheduledLN2SupplySettings->DaySettings;
//
//	EEPROM_Write(SCHEDULED_LN2SUPPLY_ADDRESS, 0,(uint8_t*)&vp_param_rw_str.LN2Supply_HoursSettings,12);
//	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.LN2Supply_HoursSettings, 12);
//	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
//	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
//	EEPROM_Write(SCHEDULED_LN2SUPPLY_ADDRESS_CRC,0,bfr,2);
//
//	//date_rem.date_ln2=0;
//	//EEPROM_Write(SUPPLY_DATE,0,&date_rem.date_ln2,1);
//
//}
//
///*
// * *****************************************************************************
// * Function name: LoadScheduledLN2SupplySettings
// * Created by: Arikanti venkatesh
// * Date:       14-10-2025
// * Description:
// * 		Load level settings from application
// * Parameters:
// *  Input  : Settings data pointer
// *  Return : None
// * Note:
// *
// * *****************************************************************************
// */
//void LoadScheduledLN2SupplySettings(RemainderSettingsStruct *ScheduledLN2SupplySettings)
//{
//	ScheduledLN2SupplySettings->HoursSettings 		= vp_param_rw_str.LN2Supply_HoursSettings;
//	ScheduledLN2SupplySettings->MinutesSettings 	= vp_param_rw_str.LN2Supply_MinutesSettings;
//	ScheduledLN2SupplySettings->AMPMSettings 		= vp_param_rw_str.LN2Supply_AMPMSettings;
//	ScheduledLN2SupplySettings->DaySettings 		= vp_param_rw_str.LN2Supply_DaySettings;
//	ScheduledLN2SupplySettings->Hours12Settings 	= vp_param_rw_str.LN2Supply_Hours12Settings;
////	ScheduledLN2SupplySettings->Minutes12Settings 	= vp_param_rw_str.LN2Supply_Minutes12Settings;
//
////	SendVPDataCmdToDisplay(DISP_LN2SUPPLY_REMAINDER_CMD);
//}

void SaveFreezerId(FreezerStruct *UIFreezerStruct)
{
	if(FrezzID[0]!=0)
	{
		uint8_t   bfr[2];
		memcpy((uint8_t*)&UIFreezerStruct->FreezerID, (uint8_t*)&FrezzID, sizeof(FrezzID));
		EEPROM_Write(FREEZER_ID_ADDRESS, 0,(uint8_t*)&UIFreezerStruct->FreezerID,8);
		crc16 = usMBCRC16((uint8_t*)&UIFreezerStruct->FreezerID, 8);
		bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
		bfr[1]  = (uint8_t)(crc16 & 0x00ff);
		EEPROM_Write(FREEZER_ID_ADDRESS_CRC,0,bfr,2);
		SendSTRDataToDisplay(FREEZER_ID_VALUE_WR,(uint8_t*)&UIFreezerStruct->FreezerID);
	}
	else
	{
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter a valid value!!!");
	}
}

void LoadFreezerId(FreezerStruct *UIFreezerStruct)
{
	memcpy((uint8_t*)&FrezzID, (uint8_t*)&UIFreezerStruct->FreezerID, sizeof(FrezzID));
	SendSTRDataToDisplay(FREEZER_ID_VALUE_RD,(uint8_t*)&UIFreezerStruct->FreezerID);
	SendSTRDataToDisplay(FREEZER_ID_VALUE_WR,(uint8_t*)&UIFreezerStruct->FreezerID);
	/*if(Control_versions.Firmware_avb_icon &&((Control_versions.IOT_File_status==0) || (Control_versions.IOT_File_status==1)))
	{
		SendN16DataToDisplay(IOT_FIRM_ICON_WR, UIUpdateIOTFIRMsettings.Firmware_update_en);
		SendN16DataToDisplay(IOT_FIRM_ICON_RD, UIUpdateIOTFIRMsettings.Firmware_update_en);
		SendN16DataToDisplay(IOT_FIRM_FILE_DOWN_EN, 1);
	}
	else
	{
		SendN16DataToDisplay(IOT_FIRM_FILE_DOWN_EN, 0);
	}*/
//	SendN16DataToDisplay(PICK_TIME_EN, firmware_timer_en);			////Need to comeback
}


/*
 * *****************************************************************************
 * Function name: HandleFirmwareupdateFunctions
 * Created by : Arikanti venkatesh
 * Date:        26-NOV-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleFirmwareupdateFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY://update now
				if((vp_rt_wr_str.LoginStatus==2)||((login_cred[1].admin_id[0] ==0)||(login_cred[1].admin_id[0] ==255)))
				{
					Update.update_display=0;
					Update.update_firmware=0;
					Update_now_system();
//					if(usb_info.state != USB_INIT)
//						usb_info.state=USB_DE_INIT;
//					usb_info.new_mode=USB_HOST;
				}
				else
				{
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"login as admin!!!");
				}
				break;
			case ENTER_KEY://pick a time
				if((vp_rt_wr_str.LoginStatus==2)||((login_cred[1].admin_id[0] ==0)||(login_cred[1].admin_id[0] ==255)))
				{
					SendN16DataToDisplay(PICK_A_TIME_VKEY, 1);
				}
				else
				{
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"login as admin!!!");
				}

				break;
			case CANCEL_KEY://escape

				break;
		}
	}
}
void Update_now_system(void)
{
	if(SD_info.state == SD_DONE && vp_rt_wr_str.PowerStatus)
	{
		//SendN16DataToDisplay(0x0801E4, 0);
//		if((FR_OK == f_stat ("0:/FONT", &file_info))||(FR_OK == f_stat ("0:/THMT", &file_info)))
//		{
//			Update.update_display=1;
//			usb_info.txn_state = USB_INIT;
//		}
		static UINT attributes;
		tx_semaphore_get(&emmc_semaphore, TX_WAIT_FOREVER);
		if (FX_SUCCESS == fx_file_attributes_read(&mmc_disk, CFFileName,&attributes))
		{
	    	File_crc_validation(CFFileName, mmc_disk);
		}
		tx_semaphore_put(&emmc_semaphore);
		SD_info.update_page_flag=0;
		firmware_timer_en=0;
		EEPROM_Write(Firmware_flag_address, 0,(uint8_t*)&firmware_timer_en ,1);
		EEPROM_Write(Firmware_update_flag, 0,(uint8_t*)&Update.update_firmware ,4);
		if(Update.update_firmware || Update.update_display)
		{
			Before_update();
		}

	}
}
void Update_system(void)
{
	if(SD_info.state == SD_DONE)
	{
		/*SendN16DataToDisplay(0x0801E4, 0);
		if((FR_OK == f_stat ("0:/FONT", &file_info))||(FR_OK == f_stat ("0:/THMT", &file_info)))
		{
			Update.update_display=1;
			usb_info.txn_state = USB_INIT;
		}
	    if(FR_OK == f_stat (CFFileName, &file_info))
		{
			File_validation(CFFileName);
		}
		if(Update.update_firmware || Update.update_display)
		{
			Before_update();
		}*/
		SD_info.init_done=1;
		SD_info.update_page_flag=0;
		firmware_timer_en=0;
		EEPROM_Write(Firmware_flag_address, 0,(uint8_t*)&firmware_timer_en ,1);
//		EEPROM_Write(Firmware_update_flag, 0,(uint8_t*)&Update.update_firmware ,4);
	}
	return;
}
void File_validation(char *Valid_name)
{
    /*unsigned int bytes_read;
//	Update.update_display=0;
//	uint8_t count=0;
	TCHAR   fwpath[16];

	if((fresult = f_open(&SDFile, Valid_name, FA_READ))==FR_OK)
	{
		uint8_t CFCBuffer[256];
		uint16_t StartCrc = 0x1D0F;
		do
		{
			HAL_IWDG_Refresh(&hiwdg);
			fresult = f_read (&SDFile, CFCBuffer, BLOCK_SIZE, &bytes_read);
			if((fresult==FR_OK) && (bytes_read > 0))
			{
				StartCrc = crc16_ccitt_bin(CFCBuffer, bytes_read, StartCrc);
			}
			else
			{
			  bytes_read = 0;
			  if(StartCrc == 0)
			  {
				  Update.update_firmware = 1;
				  SD_info.bin_present = 1;
			  }
			  else
			  {
				  fresult = f_close(&SDFile);
				  SendN16DataToDisplay(STRING_POPUP_EN, 1);
				  SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid firmware update file!!!");
				  Update.update_firmware = 0;
//				  strcat(path, "CFC.bin");
					strncpy(fwpath, Valid_name,16);
//					strcat(fwpath, CFFileName);
					fresult = f_unlink (fwpath);
				  TargetPage = 0;
				  SendCmdToDisplay(LOAD_PAGE_CMD);

			  }
			}
		}while(bytes_read > 0);
	}
	fresult = f_close(&SDFile);*/
}
/*uint8_t File_crc_validation(char *Valid_name)
{
    unsigned int bytes_read;
    uint16_t StartCrc = 0x1D0F;
//	Update.update_display=0;
//	uint8_t count=0;
    FIL  VERIFY_SDFile={0};
//	TCHAR   fwpath[25];

	if((fresult = f_open(&VERIFY_SDFile, Valid_name, FA_READ))==FR_OK)
	{
		uint8_t CFCBuffer[256];

		do
		{
			HAL_IWDG_Refresh(&hiwdg);
			fresult = f_read (&VERIFY_SDFile, CFCBuffer, 256, &bytes_read);
			if((fresult==FR_OK) && (bytes_read > 0))
			{
				StartCrc = crc16_ccitt_bin(CFCBuffer, bytes_read, StartCrc);
			}
			else
			{
			  bytes_read = 0;
			  if(StartCrc != 0)
			  {
				  fresult = f_close(&VERIFY_SDFile);
//					strncpy(fwpath, Valid_name,16);
//					strcat(fwpath, CFFileName);
					fresult = f_unlink (Valid_name);
				  TargetPage = 0;
				  SendCmdToDisplay(LOAD_PAGE_CMD);
			  }
			}
		}while(bytes_read > 0);
	}
	fresult = f_close(&VERIFY_SDFile);
	if(StartCrc==0)
	{
		return 1;
	}
	return 0;
	return 0;
}*/
uint8_t File_crc_validation(char *Valid_name, FX_MEDIA   pass_media)
{
    ULONG bytes_read;
    UINT status;
    uint16_t StartCrc = 0x1D0F;
    FX_FILE  VERIFY_SDFile;


    // Open the file
   status = fx_file_open(&pass_media, &VERIFY_SDFile, Valid_name, FX_OPEN_FOR_READ);
   if (status != FX_SUCCESS)
    {
        return 0; // File open failed
    }
   else
   {
   	uint8_t CFCBuffer[256];

		do {

			status = fx_file_read(&VERIFY_SDFile, CFCBuffer, sizeof(CFCBuffer),&bytes_read);
			if ((status == FX_SUCCESS) && (bytes_read > 0))
			{
				StartCrc = crc16_ccitt_bin(CFCBuffer, bytes_read, StartCrc);
			}
			else
			{
				bytes_read = 0;
				if(StartCrc == 0)
			   {
				  Update.update_firmware = 1;
			   }
				if (StartCrc != 0)
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
				    SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid firmware update file!!!");
					fx_file_close(&VERIFY_SDFile);
					fx_file_delete(&pass_media, Valid_name);
					TargetPage = 0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
			}
		} while (bytes_read > 0);
	fx_file_close(&VERIFY_SDFile);
	return (StartCrc == 0) ? 1 : 0;
}
}
/*
 * *****************************************************************************
 * Function name: HandleFirmwareupdatetimeFunctions
 * Created by : Arikanti venkatesh
 * Date:        27-NOV-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleFirmwareupdatetimeFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case SECURIY_KEY://save update time
				SaveUpdateTimeSettings(&UIUpdateRemaindersettings);
				break;
		}
	}
}

void HandleFileExplorerFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	UINT status;
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				if(usb_info.state != USB_INIT)
					usb_info.state=USB_DE_INIT;//should not do deinit without init
				usb_info.new_mode=USB_MSC;
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;

			case ENTER_KEY:
				if(media != NULL)
				{
					tx_semaphore_get(&emmc_semaphore, TX_WAIT_FOREVER);
					for(int i=0;i<MAX_ITEMS;i++)
					{
						if(dirList[i].file_sel == 1)
						{
							TargetPage = 47;
							SendCmdToDisplay(LOAD_PAGE_CMD);
							status = Copy_File_From_eMMC_To_Host_USB(currentPath , dirList[i].name);
							TargetPage = 45;
							SendCmdToDisplay(LOAD_PAGE_CMD);
							if(status == FX_SUCCESS){
								SendN16DataToDisplay(STRING_POPUP_EN, 2);
								SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"SUCCESS!!!");
							}
							else
							{
								SendN16DataToDisplay(STRING_POPUP_EN, 1);
								SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"FAIL!!!");
							}
						}
					}
					for(int i=0;i<MAX_ITEMS;i++)
					{
						dirList[i].file_sel = 0;
					}
					SendVPDataCmdToDisplay(FILE_SELECT_RD);
					SendVPDataCmdToDisplay(FILE_SELECT_WR);
					tx_semaphore_put(&emmc_semaphore);
				}
				break;

			case CANCEL_KEY:
				for(int i=0;i<MAX_ITEMS;i++)
				{
					dirList[i].file_sel = 0;
				}
				SendVPDataCmdToDisplay(FILE_SELECT_RD);
				SendVPDataCmdToDisplay(FILE_SELECT_WR);
				break;

			case DATE_TIME_KEY://first file
				Folder_Expanding(0);
				break;

			case MODBUS_CRYOLINK_KEY://second file
				Folder_Expanding(1);
				break;

			case SCHEDULED_LN2SUPPLY_KEY://third file
				Folder_Expanding(2);
				break;

			case CALI_1P_KEY://fourth file
				Folder_Expanding(3);
				break;

			case CALI_2P_KEY://fifth file
				Folder_Expanding(4);
				break;

			case CALI_RTD1_KEY://sixth file
				Folder_Expanding(5);
				break;

			case NEXT://seventh file
				Folder_Expanding(6);
				break;

			case BACK_KEY://back key
				Folder_back();
				break;
		}
	}
}

void Folder_Expanding(uint8_t index)
{
	if(dirList[index].type == 0)
	{
		add_folder(dirList[index].name);
		List_Folders(currentPath);
		for (uint8_t y = 0; (y < MAX_ITEMS); y++)
		{
			remove_extension_global(dirList[y].name);
			SendSTRDataToDisplay(File_add[y].File_address, (uint8_t*)g_filename_noext);
		}
		SendVPDataCmdToDisplay(FILE_TYPE_EN1);
		SendVPDataCmdToDisplay(FILE_TOUCH_EN);
		SendVPDataCmdToDisplay(FILE_SELECT_RD);
		SendVPDataCmdToDisplay(FILE_SELECT_WR);
		SendVPDataCmdToDisplay(FILE_SIZE);
//		TargetPage = 45;
//		SendCmdToDisplay(LOAD_PAGE_CMD);
	}
}

void Folder_back()
{
	go_back();
	List_Folders(currentPath);
	for (uint8_t y = 0; (y < MAX_ITEMS); y++)
	{
		remove_extension_global(dirList[y].name);
		SendSTRDataToDisplay(File_add[y].File_address, (uint8_t*)g_filename_noext);
	}
	SendVPDataCmdToDisplay(FILE_TYPE_EN1);
	SendVPDataCmdToDisplay(FILE_TOUCH_EN);
	SendVPDataCmdToDisplay(FILE_SIZE);
}
/*
 * *****************************************************************************
 * Function name: SaveUpdateTimeSettings
 * Created by : Arikanti venkatesh
 * Date:        12-NOV-2023
 * Description:
 * 		Save SaveScheduledSettings  to application
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveUpdateTimeSettings(UpdateRemainderSettingsStruct *UIUpdateRemaindersettings)
{
	uint8_t  Lp;
	if(vp_param_rw_str.RTC_TimeFormat)
	{
		UpdateStruct.Update_HoursSettings 			= UIUpdateRemaindersettings->HoursSettings;
		UpdateStruct.Update_MinutesSettings  		= UIUpdateRemaindersettings->MinutesSettings;
	}
	else
	{
		if(UIUpdateRemaindersettings->AMPMSettings==1)
		{
			if(UIUpdateRemaindersettings->Hours12Settings<12)
				UpdateStruct.Update_HoursSettings 	= (UIUpdateRemaindersettings->Hours12Settings+12);
			else
				UpdateStruct.Update_HoursSettings 	= (UIUpdateRemaindersettings->Hours12Settings);
		}
		else
		{
			if(UIUpdateRemaindersettings->Hours12Settings==12)
				UpdateStruct.Update_HoursSettings 	= (UIUpdateRemaindersettings->Hours12Settings-12);
			else
				UpdateStruct.Update_HoursSettings 	= (UIUpdateRemaindersettings->Hours12Settings);
		}
//		UpdateStruct.Update_MinutesSettings 	    = UIUpdateRemaindersettings->Minutes12Settings;
	}

	UpdateStruct.Update_MonthSettings  		= UIUpdateRemaindersettings->MonthSettings;
	UpdateStruct.Update_DateSettings  			= UIUpdateRemaindersettings->DateSettings;
	UpdateStruct.Update_YearSettings  			= UIUpdateRemaindersettings->YearSettings;

	if(Date.Year==UIUpdateRemaindersettings->YearSettings)
	{
		if(Date.Month==UIUpdateRemaindersettings->MonthSettings)
		{
			if(Date.Date==UIUpdateRemaindersettings->DateSettings)
			{
				if(hours==UpdateStruct.Update_HoursSettings)
				{
					if(Time.Minutes>=UpdateStruct.Update_MinutesSettings)
					{
						Remove_save_time();
						return;
					}
				}
				else if(hours>UpdateStruct.Update_HoursSettings)
				{
					Remove_save_time();
					return;
				}
			  }
			else if(Date.Date>UIUpdateRemaindersettings->DateSettings)
			{
				Remove_save_time();
				return ;
			}
			else if(Date.Date+7 <= UIUpdateRemaindersettings->DateSettings )
			{
				Remove_save_time();
				return ;
			}
		}
		else if(Date.Month>UIUpdateRemaindersettings->MonthSettings)
		{
			Remove_save_time();
			return;
		}
		else if(Date.Month<UIUpdateRemaindersettings->MonthSettings )
		{
			if((Date.Month==4)||(Date.Month==6)||(Date.Month==9)||(Date.Month==11))
			{
				if(Date.Date+7 <= UIUpdateRemaindersettings->DateSettings+30)
				{
					Remove_save_time();
					return ;
				}
			}
			else if(Date.Month==2)
			{
				Lp = (UIUpdateRemaindersettings->YearSettings %4);

				if(Lp==0)
				{
					if(Date.Date+7 <= UIUpdateRemaindersettings->DateSettings+29)
					{
						Remove_save_time();
						return ;
					}
				}
				else
				{
					if(Date.Date+7 <= UIUpdateRemaindersettings->DateSettings+28)
					{
						Remove_save_time();
						return ;
					}
				}
			}else
			{
				if(Date.Date+7 <= UIUpdateRemaindersettings->DateSettings+31)
				{
					Remove_save_time();
					return ;
				}
			}
		}
	}
	else if(Date.Year<UIUpdateRemaindersettings->YearSettings)
	{
		if(UIUpdateRemaindersettings->MonthSettings==1)
		{
			if(Date.Date+7 <= UIUpdateRemaindersettings->DateSettings+31)
			{
				Remove_save_time();
				return ;
			}
		}
		else
		{
			Remove_save_time();
			return;
		}

	}
	else
	{
		Remove_save_time();
		return;
	}

	SendN16DataToDisplay(UPDATE_FIRMWARE_CANCEL, 1);
	update_flag=1;
	firmware_timer_en=1;
	//SD_info.update_page_flag=0;
	EEPROM_Write(Firmware_flag_address, 0,(uint8_t*)&firmware_timer_en ,1);
	EEPROM_Write(Firmware_update_address, 0,(uint8_t*)&UpdateStruct.Update_MonthSettings ,10);
}

void Remove_save_time(void)
{
	SendN16DataToDisplay(UPDATE_FIRMWARE_CANCEL, 0);
	update_flag=0;
	firmware_timer_en=0;
	//SD_info.update_page_flag=1;
	SendN16DataToDisplay(STRING_POPUP_EN, 1);
	SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Maximum 7 days allowed");
	memset((uint8_t*)&UpdateStruct,0,sizeof(UpdateStruct));
	memset((uint8_t*)&UIUpdateRemaindersettings,0,sizeof(UIUpdateRemaindersettings));
	SendVPDataCmdToDisplay(firmware_time_clean);
	EEPROM_Write(Firmware_flag_address, 0,(uint8_t*)&firmware_timer_en ,1);
	EEPROM_Write(Firmware_update_address, 0,(uint8_t*)&UpdateStruct.Update_MonthSettings ,10);
}
/*
 * *****************************************************************************
 * Function name: HandleGroupFunctions
 * Created by : Arikanti Venkatesh
 * Date:        13-DEC-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleGroupFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:

				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case ENTER_KEY:
				//SaveGROUPSettings(&UIUpdateGroupsettings );
				break;

			case CANCEL_KEY:
				//LoadGROUPSettings(&UIUpdateGroupsettings );
				break;

			case BACK_KEY:
				//LoadGROUPSettings(&UIUpdateGroupsettings );
				break;
		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleGroupFunctions
 * Created by : Arikanti Venkatesh
 * Date:        13-DEC-2023
 * Description:
 *  Main function to handle the  touch events
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void Groupselection(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t Group_selection = (*(DataPtr+8) << 8) | *(DataPtr+9);
	Group_selection_string(Group_selection);
}
void Groupen_dis(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t Group_en_dis = (*(DataPtr+8) << 8) | *(DataPtr+9);
	if(Group_en_dis==0)
	{
		Group_selection_string(Group_en_dis);
	}
	else
	{
		for(int i=0;i<32;i++)
		{
			node_info[i].Group_inactive=Group_temp_inactive_nodes[i];
		}
		SendVPDataCmdToDisplay(GROUP_INACTIVE_NODES);
		Group_selection_string(UIUpdateGroupsettings.Group_sel);
	}
}
void Group_selection_string(int16_t value)
{
	int16_t Group_selection=value;
	if(Group_selection==1)
	{
		SendN16DataToDisplay(GROUP_SEL_WR, 1);
		SendN16DataToDisplay(0x080328, 1);
		SendN16DataToDisplay(0x08032A, 0);
		SendN16DataToDisplay(0x08032C, 0);
		SendN16DataToDisplay(0x08032E, 0);
	}
	else if(Group_selection==2)
	{
		SendN16DataToDisplay(0x080328, 0);
		SendN16DataToDisplay(GROUP_SEL_WR, 2);
		SendN16DataToDisplay(0x08032A, 1);
		SendN16DataToDisplay(0x08032C, 0);
		SendN16DataToDisplay(0x08032E, 0);
	}
	else if(Group_selection==3)
	{
		SendN16DataToDisplay(0x080328, 0);
		SendN16DataToDisplay(0x08032A, 0);
		SendN16DataToDisplay(GROUP_SEL_WR, 3);
		SendN16DataToDisplay(0x08032C, 1);
		SendN16DataToDisplay(0x08032E, 0);
	}
	else if(Group_selection==4)
	{
		SendN16DataToDisplay(0x080328, 0);
		SendN16DataToDisplay(0x08032A, 0);
		SendN16DataToDisplay(0x08032C, 0);
		SendN16DataToDisplay(GROUP_SEL_WR, 4);
		SendN16DataToDisplay(0x08032E, 1);
	}
	else if(Group_selection==0)
	{
		SendN16DataToDisplay(0x080328, 0);
		SendN16DataToDisplay(0x08032A, 0);
		SendN16DataToDisplay(0x08032C, 0);
		SendN16DataToDisplay(0x08032E, 0);
		for(int i=0;i<32;i++)
		{
			Group_temp_inactive_nodes[i]=node_info[i].Group_inactive;
			node_info[i].Group_inactive=0;
		}
		SendVPDataCmdToDisplay(GROUP_INACTIVE_NODES);
	}
}*/
/*
 * *****************************************************************************
 * Function name: GroupAStringHandler
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to handle the  touch events
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void GroupAStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	int temp=0;
	char temp_int[20] = {0, }; //revoke
	int8_t String_store[90];
	uint8_t temp_string[5];
	uint8_t string_len=0;
	uint16_t string_ptr=0;
	Fsize=0;
	y=0;
	memset(UIUpdateGroupsettings.Group_A, 0, sizeof(UIUpdateGroupsettings.Group_A));   // Erase
	memset(Temp_group_col,0,sizeof(Temp_group_col));
	memset(String_store,0,sizeof(String_store));
	for(i=0;i<90;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||(DataPtr[8+i]==0x2C))
			{
				UIUpdateGroupsettings.Group_A[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				if((DataPtr[8]==255))
				{
					SendSTRDataToDisplay(GROUP_A_RD,(uint8_t*)"\0");
					memset((uint8_t*)&GroupStruct.Group_A, 0, sizeof(GroupStruct.Group_A));
				}
				y=1;
				Error_string(y);
				return;
			}
		}
		else
		break;
	}

	int k=0;
	if((UIUpdateGroupsettings.Group_A[0]==0))
	{
		for(int z=0;z<32;z++)
		{
			if(UIUpdateGroupsettings.Group_col[z]==1)
			{
				UIUpdateGroupsettings.Group_col[z]=0;
			}
			if(Temp_group_col[z]==1)
			{
				UIUpdateGroupsettings.Group_col[z]=1;
			}
		}
		SendSTRDataToDisplay(GROUP_A_WR,(uint8_t*)&UIUpdateGroupsettings.Group_A);
		return;
	}
	for(int j=0;j<=i;j++)
	{
		if(((UIUpdateGroupsettings.Group_A[j]==',')||(UIUpdateGroupsettings.Group_A[j]==0))&&temp_int[0]!=0)
		{
			temp=convert_to_string(temp_int);
			memset(temp_int,0,sizeof(temp_int));
			if(temp<32 && temp>=0 )
			{
				if(Temp_group_col[temp]==0)
				{
					Temp_group_col[temp]=1;
					string_len=snprintf((char*)temp_string,sizeof(temp), "%d,", temp);
					memcpy((uint8_t*)&String_store[string_ptr], temp_string, string_len);
					string_ptr +=string_len;
				}
			}
			else
			{
				y=1;
				Error_string(y);
				return;
			}
			if((UIUpdateGroupsettings.Group_col[temp] != 1)&&(UIUpdateGroupsettings.Group_col[temp] != 0)&&\
				(UIUpdateGroupsettings.Group_col[temp] != 0xffff)&&(UIUpdateGroupsettings.Group_col[temp] != -1))
			{
				y=2;
				Error_string(y);
				return;
			}
			k=0;
			continue;
		}

		if((UIUpdateGroupsettings.Group_A[j]>=0x30)&&(UIUpdateGroupsettings.Group_A[j]<=0x39))
		{
			temp=0;
			temp_int[k]=(char)(UIUpdateGroupsettings.Group_A[j]);
			//temp_int[k]=sd_plumd_read[j];
			k++;
		}
	}
	for(int z=0;z<32;z++)
	{
		if(UIUpdateGroupsettings.Group_col[z]==1)
		{
			UIUpdateGroupsettings.Group_col[z]=0;
		}
		if(Temp_group_col[z]==1)
		{
			UIUpdateGroupsettings.Group_col[z]=1;
		}
	}
	if(y==0)
	{
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_A,(uint8_t*)&String_store, sizeof(String_store));
		SendSTRDataToDisplay(GROUP_A_RD,(uint8_t*)&UIUpdateGroupsettings.Group_A);
		SendSTRDataToDisplay(GROUP_A_WR,(uint8_t*)&UIUpdateGroupsettings.Group_A);
	}
}*/
/*
 * *****************************************************************************
 * Function name: GroupBStringHandler
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to handle the  touch events
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void GroupBStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	int temp=0;
	char temp_int[20]  = {0, }; //revoke
	int8_t String_store[90];
	uint8_t temp_string[5];
	uint8_t string_len=0;
	uint16_t string_ptr=0;
	Fsize=0;
	y=0;
	memset(String_store,0,sizeof(String_store));
	memset(UIUpdateGroupsettings.Group_B, 0, sizeof(UIUpdateGroupsettings.Group_B));   // Erase
	memset(Temp_group_col,0,sizeof(Temp_group_col));
	for(i=0;i<90;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||(DataPtr[8+i]==0x2C))
			{
				UIUpdateGroupsettings.Group_B[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				if((DataPtr[8]==255))
				{
					SendSTRDataToDisplay(GROUP_B_RD,(uint8_t*)"\0");
					memset((uint8_t*)&GroupStruct.Group_B, 0, sizeof(GroupStruct.Group_B));
				}
				y=1;
				Error_string(y);
				return;
			}
		}
		else
		break;
	}

	int k=0;
	if((UIUpdateGroupsettings.Group_B[0] == 0))
	{
		for(int z=0;z<32;z++)
		{
			if(UIUpdateGroupsettings.Group_col[z]==2)
			{
				UIUpdateGroupsettings.Group_col[z]=0;
			}
			if(Temp_group_col[z]==2)
			{
				UIUpdateGroupsettings.Group_col[z]=2;
			}
		}
		SendSTRDataToDisplay(GROUP_B_WR,(uint8_t*)&UIUpdateGroupsettings.Group_B);
		return;
	}
	for(int j=0;j<=i;j++)
	{
		if(((UIUpdateGroupsettings.Group_B[j]==',')||(UIUpdateGroupsettings.Group_B[j]==0))&&temp_int[0]!=0)
		{
			temp=convert_to_string(temp_int);
			memset(temp_int,0,sizeof(temp_int));
			if(temp<32 && temp>=0)
			{
				if(Temp_group_col[temp]==0)
				{
					Temp_group_col[temp]=2;
					string_len=snprintf((char*)temp_string,sizeof(temp),"%d,", temp);
					memcpy((uint8_t*)&String_store[string_ptr], temp_string, string_len);
					string_ptr +=string_len;
				}

			}
			else
			{
				y=1;
				Error_string(y);
				return;
			}
			if((UIUpdateGroupsettings.Group_col[temp] != 2)&&(UIUpdateGroupsettings.Group_col[temp] != 0)&&\
				(UIUpdateGroupsettings.Group_col[temp] != 0xffff)&&(UIUpdateGroupsettings.Group_col[temp] != -1))
			{
				y=2;
				Error_string(y);
				return;
			}
			k=0;
			continue;
		}

		if((UIUpdateGroupsettings.Group_B[j]>=0x30)&&(UIUpdateGroupsettings.Group_B[j]<=0x39))
		{
			temp=0;
			temp_int[k]=(char)(UIUpdateGroupsettings.Group_B[j]);
			//temp_int[k]=sd_plumd_read[j];
			k++;
		}
	}
	for(int z=0;z<32;z++)
	{
		if(UIUpdateGroupsettings.Group_col[z]==2)
		{
			UIUpdateGroupsettings.Group_col[z]=0;
		}
		if(Temp_group_col[z]==2)
		{
			UIUpdateGroupsettings.Group_col[z]=2;
		}
	}
	if(y==0)
	{
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_B,(uint8_t*)&String_store, sizeof(String_store));
		SendSTRDataToDisplay(GROUP_B_RD,(uint8_t*)&UIUpdateGroupsettings.Group_B);
		SendSTRDataToDisplay(GROUP_B_WR,(uint8_t*)&UIUpdateGroupsettings.Group_B);
	}
	//SendVPDataCmdToDisplay(GROUP_COLOUR_CMD);
}*/

/*
 * *****************************************************************************
 * Function name: GroupCStringHandler
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to handle the  touch events
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void GroupCStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	int temp=0;
	char temp_int[20] = {0, }; //revoke
	int8_t String_store[90];
	uint8_t temp_string[5];
	uint8_t string_len=0;
	uint16_t string_ptr=0;
	Fsize=0;
	y=0;
	memset(String_store,0,sizeof(String_store));
	memset(UIUpdateGroupsettings.Group_C, 0, sizeof(UIUpdateGroupsettings.Group_C));   // Erase
	memset(Temp_group_col,0,sizeof(Temp_group_col));
	for(i=0;i<90;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||(DataPtr[8+i]==0x2C))
			{
				UIUpdateGroupsettings.Group_C[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				if((DataPtr[8]==255))
				{
					SendSTRDataToDisplay(GROUP_C_RD,(uint8_t*)"\0");
					memset((uint8_t*)&GroupStruct.Group_C, 0, sizeof(GroupStruct.Group_C));
				}
				y=1;
				Error_string(y);
				return;
			}
		}
		else
		break;
	}

	int k=0;
	if((UIUpdateGroupsettings.Group_C[0] != 0))
	{
		for(int j=0;(j<=i);j++)
		{
			if(((UIUpdateGroupsettings.Group_C[j]==',')||(UIUpdateGroupsettings.Group_C[j]==0))&& temp_int[0]!=0)
			{
				temp=convert_to_string(temp_int);
				memset(temp_int,0,sizeof(temp_int));
				if(temp<32 && temp>=0)
				{
					if(Temp_group_col[temp]==0)
					{
						Temp_group_col[temp]=3;
						string_len=snprintf((char*)temp_string,sizeof(temp), "%d,", temp);
						memcpy((uint8_t*)&String_store[string_ptr], temp_string, string_len);
						string_ptr +=string_len;
					}
				}
				else
				{
					y=1;
					Error_string(y);
					return;
				}
				if((UIUpdateGroupsettings.Group_col[temp] != 3)&&(UIUpdateGroupsettings.Group_col[temp] != 0)&&\
					(UIUpdateGroupsettings.Group_col[temp] != 0xffff)&&(UIUpdateGroupsettings.Group_col[temp] != -1))
				{
					y=2;
					Error_string(y);
					return;
				}
				k=0;
				continue;
			}

			if((UIUpdateGroupsettings.Group_C[j]>=0x30)&&(UIUpdateGroupsettings.Group_C[j]<=0x39))
			{
				temp=0;
				temp_int[k]=(char)(UIUpdateGroupsettings.Group_C[j]);
				//temp_int[k]=sd_plumd_read[j];
				k++;
			}
		}

		for(int z=0;z<32;z++)
		{
			if(UIUpdateGroupsettings.Group_col[z]==3)
			{
				UIUpdateGroupsettings.Group_col[z]=0;
			}
			if(Temp_group_col[z]==3)
			{
				UIUpdateGroupsettings.Group_col[z]=3;
			}
		}
		if(y==0)
		{
			memcpy((uint8_t*)&UIUpdateGroupsettings.Group_C,(uint8_t*)&String_store, sizeof(String_store));
			SendSTRDataToDisplay(GROUP_C_RD,(uint8_t*)&UIUpdateGroupsettings.Group_C);
			SendSTRDataToDisplay(GROUP_C_WR,(uint8_t*)&UIUpdateGroupsettings.Group_C);
		}
	}
	else
	{
		for(int z=0;z<32;z++)
		{
			if(UIUpdateGroupsettings.Group_col[z]==3)
			{
				UIUpdateGroupsettings.Group_col[z]=0;
			}
			if(Temp_group_col[z]==3)
			{
				UIUpdateGroupsettings.Group_col[z]=3;
			}
		}
		SendSTRDataToDisplay(GROUP_C_WR,(uint8_t*)&UIUpdateGroupsettings.Group_C);
	}

	//SendVPDataCmdToDisplay(GROUP_COLOUR_CMD);
}*/

/*
 * *****************************************************************************
 * Function name: GroupDStringHandler
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to handle the  touch events
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void GroupDStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	int temp=0;
	char temp_int[20] = {0, }; //revoke
	int8_t String_store[90];
	uint8_t temp_string[5];
	uint8_t string_len=0;
	uint16_t string_ptr=0;
	Fsize=0;
	y=0;
	memset(String_store,0,sizeof(String_store));
	memset(UIUpdateGroupsettings.Group_D, 0, sizeof(UIUpdateGroupsettings.Group_D));   // Erase
	memset(Temp_group_col,0,sizeof(Temp_group_col));
	for(i=0;i<90;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||(DataPtr[8+i]==0x2C))
			{
				UIUpdateGroupsettings.Group_D[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				if((DataPtr[8]==255))
				{
					SendSTRDataToDisplay(GROUP_D_RD,(uint8_t*)"\0");
					memset((uint8_t*)&GroupStruct.Group_D, 0, sizeof(GroupStruct.Group_D));
				}
				y=1;
				Error_string(y);
				return;
			}
		}
		else
		break;
	}

	int k=0;
	if(UIUpdateGroupsettings.Group_D[0]==0)
	{
		for(int z=0;z<32;z++)
		{
			if(UIUpdateGroupsettings.Group_col[z]==4)
			{
				UIUpdateGroupsettings.Group_col[z]=0;
			}
			if(Temp_group_col[z]==4)
			{
				UIUpdateGroupsettings.Group_col[z]=4;
			}
		}
		SendSTRDataToDisplay(GROUP_D_WR,(uint8_t*)&UIUpdateGroupsettings.Group_D);
		return;
	}
	for(int j=0;j<=i;j++)
	{
		if(((UIUpdateGroupsettings.Group_D[j]==',')||(UIUpdateGroupsettings.Group_D[j]==0))&& temp_int[0]!=0)
		{
			temp=convert_to_string(temp_int);
			memset(temp_int,0,sizeof(temp_int));
			if(temp<32 && temp>=0)
			{
				if(Temp_group_col[temp]==0)
				{
					Temp_group_col[temp]=4;
					string_len=snprintf((char*)temp_string,sizeof(temp), "%d,", temp);
					memcpy((uint8_t*)&String_store[string_ptr], temp_string, string_len);
					string_ptr +=string_len;
				}
			}
			else
			{
				y=1;
				Error_string(y);
				return;
			}
			if((UIUpdateGroupsettings.Group_col[temp] != 4)&&(UIUpdateGroupsettings.Group_col[temp] != 0)&&\
				(UIUpdateGroupsettings.Group_col[temp] != 0xffff)&&(UIUpdateGroupsettings.Group_col[temp] != -1))
			{
				y=2;
				Error_string(y);
				return;
			}
			k=0;
			continue;
		}

		if((UIUpdateGroupsettings.Group_D[j]>=0x30)&&(UIUpdateGroupsettings.Group_D[j]<=0x39))
		{
			temp=0;
			temp_int[k]=(char)(UIUpdateGroupsettings.Group_D[j]);
			//temp_int[k]=sd_plumd_read[j];
			k++;
		}
	}

	for(int z=0;z<32;z++)
	{
		if(UIUpdateGroupsettings.Group_col[z]==4)
		{
			UIUpdateGroupsettings.Group_col[z]=0;
		}
		if(Temp_group_col[z]==4)
		{
			UIUpdateGroupsettings.Group_col[z]=4;
		}
	}
	if(y==0)
	{
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_D,(uint8_t*)&String_store, sizeof(String_store));
		SendSTRDataToDisplay(GROUP_D_RD,(uint8_t*)&UIUpdateGroupsettings.Group_D);
		SendSTRDataToDisplay(GROUP_D_WR,(uint8_t*)&UIUpdateGroupsettings.Group_D);
	}
	//SendVPDataCmdToDisplay(GROUP_COLOUR_CMD);
}*/
/*
 * *****************************************************************************
 * Function name: Error_string
 * Created by : Arikanti Venkatesh
 * Date:        26-DEC-2023
 * Description:
 *  Main function to handle the  Group string
 * Parameters:
 *  Input  : value
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Error_string(uint8_t value)
{
	SendN16DataToDisplay(STRING_POPUP_EN, 1);
	if(value==1)
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (0-31)(,) Only !!!");
	else if(value==2)
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Assigned multiple Group !!!");
	if(UIUpdateGroupsettings.Group_sel==1)
	{
		SendSTRDataToDisplay(GROUP_A_RD,(uint8_t*)&GroupStruct.Group_A);
		SendSTRDataToDisplay(GROUP_A_WR,(uint8_t*)&GroupStruct.Group_A);
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_A,(uint8_t*)&GroupStruct.Group_A, sizeof(GroupStruct.Group_A));
	}
	else if(UIUpdateGroupsettings.Group_sel==2)
	{
		SendSTRDataToDisplay(GROUP_B_RD,(uint8_t*)&GroupStruct.Group_B);
		SendSTRDataToDisplay(GROUP_B_WR,(uint8_t*)&GroupStruct.Group_B);
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_B,(uint8_t*)&GroupStruct.Group_B, sizeof(GroupStruct.Group_B));
	}
	else if(UIUpdateGroupsettings.Group_sel==3)
	{
		SendSTRDataToDisplay(GROUP_C_RD,(uint8_t*)&GroupStruct.Group_C);
		SendSTRDataToDisplay(GROUP_C_WR,(uint8_t*)&GroupStruct.Group_C);
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_C,(uint8_t*)&GroupStruct.Group_C, sizeof(GroupStruct.Group_C));
	}
	else if(UIUpdateGroupsettings.Group_sel==4)
	{
		SendSTRDataToDisplay(GROUP_D_RD,(uint8_t*)&GroupStruct.Group_D);
		SendSTRDataToDisplay(GROUP_D_WR,(uint8_t*)&GroupStruct.Group_D);
		memcpy((uint8_t*)&UIUpdateGroupsettings.Group_D,(uint8_t*)&GroupStruct.Group_D, sizeof(GroupStruct.Group_D));
	}
}
/*
 * *****************************************************************************
 * Function name: SaveGROUPSettings
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to save Groupsettings
 * Parameters:
 *  Input  : UpdateGroupSettingsStruct
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void SaveGROUPSettings(UpdateGroupSettingsStruct *UpdateGroupSetting)
{
	GroupStruct.Group_en=UpdateGroupSetting->Group_en;
	GroupStruct.Group_sel=UpdateGroupSetting->Group_sel;
	memcpy((uint8_t*)&GroupStruct.Group_col, (uint8_t*)&UpdateGroupSetting->Group_col, sizeof(GroupStruct.Group_col));
	memcpy((uint8_t*)&GroupStruct.Group_A, (uint8_t*)&UpdateGroupSetting->Group_A, sizeof(GroupStruct.Group_A));
	memcpy((uint8_t*)&GroupStruct.Group_B, (uint8_t*)&UpdateGroupSetting->Group_B, sizeof(GroupStruct.Group_B));
	memcpy((uint8_t*)&GroupStruct.Group_C, (uint8_t*)&UpdateGroupSetting->Group_C, sizeof(GroupStruct.Group_C));
	memcpy((uint8_t*)&GroupStruct.Group_D, (uint8_t*)&UpdateGroupSetting->Group_D, sizeof(GroupStruct.Group_D));

	EEPROM_Write(GROUP_ADDRESS, 0,(uint8_t*)&GroupStruct.Group_en ,184);
	EEPROM_Write(GROUP_ADDRESS1, 0,(uint8_t*)&GroupStruct.Group_C ,244);
	if((vp_rt_wr_str.MBStatus !=0) && (vp_rt_wr_str.MBStatus !=2))
	{
		for(int i=0;i<MB_MAX_NODES;i++)
		{
		  if(GroupStruct.Group_col[i]==0)
		  {
			  node_info[i].Group_inactive=0;
		  }
		}
	}
	else
	{
		for(int i=0;i<MB_MAX_NODES;i++)
		{
			  node_info[i].Group_inactive=0;
		}
	}

	if(GroupStruct.Group_en)
	{
		SendVPDataCmdToDisplay(GROUP_INACTIVE_NODES);
	}
	SendVPDataCmdToDisplay(GROUP_COLOUR_CMD);
	update_error=1;
}*/
/*
 * *****************************************************************************
 * Function name: LoadGROUPSettings
 * Created by : Arikanti Venkatesh
 * Date:        15-DEC-2023
 * Description:
 *  Main function to Load Groupsettings
 * Parameters:
 *  Input  : UpdateGroupSettingsStruct
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadGROUPSettings(UpdateGroupSettingsStruct *UpdateGroupSetting)
{
	UpdateGroupSetting->Group_en=GroupStruct.Group_en;
	UpdateGroupSetting->Group_sel=GroupStruct.Group_sel;
	memcpy((uint8_t*)&UpdateGroupSetting->Group_col, (uint8_t*)&GroupStruct.Group_col, sizeof(GroupStruct.Group_col));
	memcpy((uint8_t*)&UpdateGroupSetting->Group_A, (uint8_t*)&GroupStruct.Group_A, sizeof(GroupStruct.Group_A));
	memcpy((uint8_t*)&UpdateGroupSetting->Group_B, (uint8_t*)&GroupStruct.Group_B, sizeof(GroupStruct.Group_B));
	memcpy((uint8_t*)&UpdateGroupSetting->Group_C, (uint8_t*)&GroupStruct.Group_C, sizeof(GroupStruct.Group_C));
	memcpy((uint8_t*)&UpdateGroupSetting->Group_D, (uint8_t*)&GroupStruct.Group_D, sizeof(GroupStruct.Group_D));
	SendSTRDataToDisplay(GROUP_A_WR,(uint8_t*)&GroupStruct.Group_A);
	SendSTRDataToDisplay(GROUP_A_RD,(uint8_t*)&GroupStruct.Group_A);
	SendSTRDataToDisplay(GROUP_B_WR,(uint8_t*)&GroupStruct.Group_B);
	SendSTRDataToDisplay(GROUP_B_RD,(uint8_t*)&GroupStruct.Group_B);
	SendSTRDataToDisplay(GROUP_C_WR,(uint8_t*)&GroupStruct.Group_C);
	SendSTRDataToDisplay(GROUP_C_RD,(uint8_t*)&GroupStruct.Group_C);
	SendSTRDataToDisplay(GROUP_D_WR,(uint8_t*)&GroupStruct.Group_D);
	SendSTRDataToDisplay(GROUP_D_RD,(uint8_t*)&GroupStruct.Group_D);
	SendN16DataToDisplay(GROUP_EN_WR, GroupStruct.Group_en);
	SendN16DataToDisplay(GROUP_EN_RD, GroupStruct.Group_en);
	SendN16DataToDisplay(GROUP_SEL_WR, GroupStruct.Group_sel);
	SendN16DataToDisplay(GROUP_SEL_RD, GroupStruct.Group_sel);
	SendVPDataCmdToDisplay(GROUP_COLOUR_CMD);
//	if(GroupStruct.Group_en==0)
//	{
//		Group_selection_string(GroupStruct.Group_en);
//	}
//	else
//		Group_selection_string(GroupStruct.Group_sel);
}
/*
 * *****************************************************************************
 * Function name: SaveSystemSettings
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to save Systemsettings
 * Parameters:
 *  Input  : UpdateSystemSettingsStruct
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveSystemSettings(UpdateSystemSettingsStruct *UpdateSystemSetting)
{
	memcpy((uint8_t*)&SystemStruct.Supply_con, (uint8_t*)&UpdateSystemSetting->Supply_con, sizeof(SystemStruct.Supply_con));
	memcpy((uint8_t*)&SystemStruct.Service_con, (uint8_t*)&UpdateSystemSetting->Service_con, sizeof(SystemStruct.Service_con));

	EEPROM_Write(CONTACT_ADDRESS, 0,(uint8_t*)&SystemStruct.Supply_con,40);
}
/*
 * *****************************************************************************
 * Function name: LoadSystemSettings
 * Created by : Arikanti Venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  Main function to save Systemsettings
 * Parameters:
 *  Input  : UpdateSystemSettingsStruct
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadSystemSettings(UpdateSystemSettingsStruct *UpdateSystemSetting)
{
	memcpy((uint8_t*)&UpdateSystemSetting->Supply_con,(uint8_t*)&SystemStruct.Supply_con,  sizeof(SystemStruct.Supply_con));
	memcpy((uint8_t*)&UpdateSystemSetting->Service_con,(uint8_t*)&SystemStruct.Service_con,  sizeof(SystemStruct.Service_con));

	SendSTRDataToDisplay(SUPLY_CONTACT_WR,(uint8_t*)&SystemStruct.Supply_con);
	SendSTRDataToDisplay(SERVICE_CONTACT_WR,(uint8_t*)&SystemStruct.Service_con);
	UIUpdateSystemSettings.Edit_en=0;
	SendN16DataToDisplay(EDIT_ADDRESS_WR, UIUpdateSystemSettings.Edit_en);
	SendN16DataToDisplay(EDIT_ADDRESS_RD, UIUpdateSystemSettings.Edit_en);
	SendN16DataToDisplay(SYS_CONTACT_FIELD_EN, UIUpdateSystemSettings.Edit_en);
	

}
void Edit_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(UIUpdateSystemSettings.Edit_en)
	{
		SendN16DataToDisplay(SYS_CONTACT_FIELD_EN, UIUpdateSystemSettings.Edit_en);
	}
	else
	{
		SendN16DataToDisplay(SYS_CONTACT_FIELD_EN, UIUpdateSystemSettings.Edit_en);
	}
}


void HandleUSBSHIFTFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	UINT file_status;
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case LID_SETTINGS_KEY://start
				SendN16DataToDisplay(FW_UPDATE_START_KEY_TOUCH_EN, 0);//Enable usb page
				SendN16DataToDisplay(FW_UPDATE_START_KEY_WR, 1);//Enable usb page
				SendN16DataToDisplay(RESTRICT_COMM_FLAG, 1);
				Time_home_ret=1;
				vp_param_rw_str.STM_ScreenSaverTime = 4;
				UpdateScreenSaverSettings();

				tx_semaphore_get(&emmc_semaphore, TX_WAIT_FOREVER);

				file_status = fx_file_open(&mmc_disk, &CDC_SD_file, "CTRS_CFC.bin", FX_OPEN_FOR_WRITE);
				if (file_status == FX_SUCCESS)
				{
				//delete file
					fx_file_close(&CDC_SD_file);
					fx_file_delete(&mmc_disk,"CTRS_CFC.bin");

				}
				FirmwareUpdateModeOn=1;
				file_status = fx_file_create(&mmc_disk, "CTRS_CFC.bin");
				if (file_status == FX_SUCCESS)
				{
					file_status = fx_file_open(&mmc_disk, &CDC_SD_file, "CTRS_CFC.bin", FX_OPEN_FOR_WRITE);
					if(usb_info.state != USB_INIT)
						usb_info.state=USB_DE_INIT;//should not do deinit without init
					usb_info.new_mode=USB_CDC;
				}
				else
				{
					fx_file_close(&CDC_SD_file);
					vp_param_rw_str.STM_ScreenSaverTime = UIDisplayData.ScreenSaverTime;
					UpdateScreenSaverSettings();
					SendN16DataToDisplay(FW_UPDATE_START_KEY_WR, 0);//Enable usb page
					SendN16DataToDisplay(RESTRICT_COMM_FLAG, 0);
					SendN16DataToDisplay(FIRMWARE_UPDATE_VKEY, 0);//Disable usb page
					UARTTxCompleted = 0;
					if(1)//(FirmwareUpdateModeOn==0)
					{
						TargetPage =0;
						SendCmdToDisplay(LOAD_PAGE_CMD);
						UARTTxCompleted = 1;
					}
					FirmwareUpdateModeOn=0;
					Time_home_ret=0;
					file_status = fx_file_close(&CDC_SD_file);
					fx_media_flush(&mmc_disk);
					tx_semaphore_put(&emmc_semaphore);
				}

//					firmware_timer_en=0;
//					EEPROM_Write(Firmware_flag_address, 0,(uint8_t*)&firmware_timer_en ,1);
//					Update.update_firmware=0;
//					Update.update_display=0;
//					Update.Bist_update=0;
//					EEPROM_Write(Firmware_update_flag, 0,(uint8_t*)&Update.update_firmware ,5);
//					memset((uint8_t*)&UpdateStruct,0,sizeof(UpdateStruct));
//					update_flag=0;


//				f_unlink("0:/extra.bin");
//				Delete_file("THMT");
//				Delete_file("Font");
//				f_unlink ("0:/CFC4_20.bin");
//				Control_versions.Firmware_downloaded=0;
//				Control_versions.Display_downloaded=0;


				break;
			case LOGOUT_YES://Stop
				vp_param_rw_str.STM_ScreenSaverTime = UIDisplayData.ScreenSaverTime;
				UpdateScreenSaverSettings();
				SendN16DataToDisplay(FW_UPDATE_START_KEY_WR, 0);//Enable usb page
				SendN16DataToDisplay(RESTRICT_COMM_FLAG, 0);
				SendN16DataToDisplay(FIRMWARE_UPDATE_VKEY, 0);//Disable usb page
				UARTTxCompleted = 0;
				if(1)//(FirmwareUpdateModeOn==0)
				{
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					UARTTxCompleted = 1;
				}
				FirmwareUpdateModeOn=0;
				Time_home_ret=0;
				file_status = fx_file_close(&CDC_SD_file);
				fx_media_flush(&mmc_disk);
				tx_semaphore_put(&emmc_semaphore);
				if(usb_info.state != USB_INIT)
					usb_info.state=USB_DE_INIT;//should not do deinit without init
				usb_info.new_mode=USB_MSC;
				SD_info.File_Available=1;
				//iot_info.data_available=1;
				break;

		}
	}
}
/*
void wififirmwareupdatepage(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
//	TCHAR   fwpath[16];
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case LOGIN_QUICKACCESS_KEY://save update time
				SendN16DataToDisplay(0x080426, 1);//Enable usb page
				SendN16DataToDisplay(0x080428, 1);//Enable usb page
				SendN16DataToDisplay(RESTRICT_COMM_FLAG, 1);
				Time_home_ret=1;
				USBD_DeInit(&hUsbDeviceFS);
				if(Usb_bus_status())
				{
					if(SD_card_init()==0)
					{

					}
				}
				else
				{

				}
				FirmwareUpdateModeOn=1;
				sd_flag=0;
				usb_info.txn_state = USB_INIT;
				sd_flag=0;
				f_unlink("0:/NINA_W13X.bin");
				vp_param_rw_str.STM_ScreenSaverTime = 4;
				UpdateScreenSaverSettings();
				break;

			case BACK_KEY://save update time
				vp_param_rw_str.STM_ScreenSaverTime = UIDisplayData.ScreenSaverTime;
				UpdateScreenSaverSettings();
				SendN16DataToDisplay(RESTRICT_COMM_FLAG, 0);
				UARTTxCompleted = 0;
				if(FirmwareUpdateModeOn==0 || SD_info.state == SD_INIT)
				{
//					TargetPage =40;
//					SendCmdToDisplay(LOAD_PAGE_CMD);
					UARTTxCompleted = 1;
				}
				SD_info.init_done=1;
				Time_home_ret=0;
				FirmwareUpdateModeOn=0;
				USBD_Stop(&hUsbDeviceFS);
				USBD_DeInit(&hUsbDeviceFS);
				usb_info.txn_state = USB_INIT;
				f_close(&COM_SD_File);
				break;
		}
	}
}*/
void Home_page(void)
{
	if(FirmwareUpdateModeOn)
	{
		vp_param_rw_str.STM_ScreenSaverTime = UIDisplayData.ScreenSaverTime;
		UpdateScreenSaverSettings();
		SendN16DataToDisplay(FW_UPDATE_START_KEY_WR, 0);//Enable usb page
		SendN16DataToDisplay(RESTRICT_COMM_FLAG, 0);
		SendN16DataToDisplay(FIRMWARE_UPDATE_VKEY, 0);//Disable usb page
		UARTTxCompleted = 0;
		FirmwareUpdateModeOn=0;
		Time_home_ret=0;
		fx_file_close(&CDC_SD_file);
		fx_media_flush(&mmc_disk);
		tx_semaphore_put(&emmc_semaphore);
		if(usb_info.state != USB_INIT)
			usb_info.state=USB_DE_INIT;//should not do deinit without init
		usb_info.new_mode=USB_MSC;
		SD_info.File_Available=1;
	}
	if(vp_cal_pt_rd_str.status == CALIB_ACTIVE)
	{
		Time_start=0;
		//calib_count=60;
		UISensorCalibData.DP2PointValue1 = 0;
		UISensorCalibData.DP2PointValue2 = 0;
		SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
		SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, UISensorCalibData.DP2PointValue2);
		vp_cal_pt_rd_str.step_num = 0;
		vp_cal_pt_rd_str.status = CALIB_INACTIVE;
		vp_cal_pt_rd_str.WaitForInput = 0;
		cal_info.state = CAL_STOP;
		calibration_canceled_flg=1;
	}
}
/*
void HandleFIRMUpdateFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case TEMP_SETTINGS_KEY://YES
				if(UIUpdateIOTFIRMsettings.Firmware_update_en)
				{
					Control_versions.Display_download_icon=1;
					SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);
					iot_info.Topic_sel=3;
				}
				else
				{
					Control_versions.Display_download_icon=0;
					SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);//need to change
					Control_versions.IOT_File_status=0;
					SendN16DataToDisplay(IOT_FILE_STATUS, Control_versions.IOT_File_status);
					iot_info.Topic_sel=0;
				}
				EEPROM_Write(IOT_FIRM_DOW, 0,(uint8_t*)&UIUpdateIOTFIRMsettings.Firmware_update_en,1);
				break;

			case CALI_RTD1_KEY://NO
				if(UIUpdateIOTFIRMsettings.Firmware_update_en)
				{
					UIUpdateIOTFIRMsettings.Firmware_update_en=0;
					SendN16DataToDisplay(IOT_FIRM_ICON_WR, UIUpdateIOTFIRMsettings.Firmware_update_en);
					SendN16DataToDisplay(IOT_FIRM_ICON_RD, UIUpdateIOTFIRMsettings.Firmware_update_en);
				}
				else
				{
					UIUpdateIOTFIRMsettings.Firmware_update_en=1;
					SendN16DataToDisplay(IOT_FIRM_ICON_WR, UIUpdateIOTFIRMsettings.Firmware_update_en);
					SendN16DataToDisplay(IOT_FIRM_ICON_RD, UIUpdateIOTFIRMsettings.Firmware_update_en);
				}
				EEPROM_Write(IOT_FIRM_DOW, 0,(uint8_t*)&UIUpdateIOTFIRMsettings.Firmware_update_en,1);
				break;
		}
	}
}




void Firm_Update_process(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(strstr((char*)Control_versions.Wifi_chip_version,"5.2.2-001"))
	{
		if((firmware_timer_en==0) && (SD_info.update_page_flag==0))
		{
			uint8_t version[5]={0};
			uint8_t version_no=0;
			uint8_t update_version=0;
			strncpy((char*)&version,(char*)&Control_versions.UI_version[3],2);
			version_no=atoi((const char*)version);
			strncpy((char*)&version,(char*)&Control_versions.UI_Update_Version[3],2);
			update_version=atoi((const char*)version);

			if(((update_version-version_no)<=3) || (Control_versions.UI_version_avb==0))
			{
				if(UIUpdateIOTFIRMsettings.Firmware_update_en)//need to add sd card condition
				{
					FIL wifi_File={0};
					FSIZE_t	fptr;
				   if((fresult = f_open(&wifi_File, Controller_file_name, FA_READ))==FR_OK)
				   {
					   fptr=wifi_File.obj.objsize;
					   if((fptr%5120)==0)
					   {
						   comm_par.sy_par.iot.File_number=wifi_File.obj.objsize/5192;
						   comm_par.sy_par.iot.File_number+=2;
					   }
					   SendN16DataToDisplay(IOT_FIRM_SEL_EN, 1);
				   }
				   else  if((fresult = f_open(&wifi_File, UI_file_name, FA_READ))==FR_OK)
				   {
					   fptr=wifi_File.obj.objsize;
					   if((fptr%5120)==0)
					   {
						   comm_par.sy_par.iot.File_number=wifi_File.obj.objsize/5192;
						   comm_par.sy_par.iot.File_number+=2;
					   }
					   SendN16DataToDisplay(IOT_FIRM_SEL_EN, 1);
				   }
				   else
				   {
					   SendN16DataToDisplay(IOT_FIRM_SEL_EN, 0);
					   comm_par.sy_par.iot.File_number=1;
				   }
				   f_close(&wifi_File);
				}
				else
				{
					SendN16DataToDisplay(IOT_FIRM_SEL_EN, 2);
				}
				SendN16DataToDisplay(IOT_FIRM_POP_UP_EN, 1);
			}
			else
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Firmware availble contact Phase two!!!");
				UIUpdateIOTFIRMsettings.Firmware_update_en=0;
				SendN16DataToDisplay(IOT_FIRM_ICON_WR, UIUpdateIOTFIRMsettings.Firmware_update_en);
				SendN16DataToDisplay(IOT_FIRM_ICON_RD, UIUpdateIOTFIRMsettings.Firmware_update_en);
			}
		}
		else
		{
			UIUpdateIOTFIRMsettings.Firmware_update_en=0;
			SendN16DataToDisplay(IOT_FIRM_ICON_WR, UIUpdateIOTFIRMsettings.Firmware_update_en);
			SendN16DataToDisplay(IOT_FIRM_ICON_RD, UIUpdateIOTFIRMsettings.Firmware_update_en);
		}
	}
	else
	{
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Update firmware manually!!!");
		UIUpdateIOTFIRMsettings.Firmware_update_en=0;
		SendN16DataToDisplay(IOT_FIRM_ICON_WR, UIUpdateIOTFIRMsettings.Firmware_update_en);
		SendN16DataToDisplay(IOT_FIRM_ICON_RD, UIUpdateIOTFIRMsettings.Firmware_update_en);
	}
}

void DP_control(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	UILevelSettingsData.Dp_control = *(DataInPtr) ;
	if(Plumbing_options.Board_version == 4 && (UILevelSettingsData.Dp_control == 0))
	{
		UILevelSettingsData.Dp_control = 1;
		SendN16DataToDisplay(DP_ENABLE_WR, UILevelSettingsData.Dp_control);
		SendN16DataToDisplay(DP_ENABLE_RD, UILevelSettingsData.Dp_control);
//		SendN16DataToDisplay(STRING_POPUP_EN, 1);
//		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"");
	}
	else
	{

	}
}*/

uint8_t Checking_ssid(uint8_t Scan_no)
{
	if(isValidInput((const char*)Wifi_ssid[Scan_no].SSid))
	{
		strncpy((char*)UIwifi.wifi_user,(char*)Wifi_ssid[Scan_no].SSid,sizeof(UIwifi.wifi_user));
		SendSTRDataToDisplay(WIFI_ADD_WR,(uint8_t*)&UIwifi.wifi_user);
		SendSTRDataToDisplay(WIFI_ADD_RD,(uint8_t*)&UIwifi.wifi_user);
		for(uint8_t k=0; k<5;k++)
		{
			if(strstr((char*)comm_par.sy_par.wifi.Ssid[k],(char*)UIwifi.wifi_user))
			{
				strncpy((char*)UIwifi.wifi_user, (char*)comm_par.sy_par.wifi.Ssid[k],  sizeof(comm_par.sy_par.wifi.Ssid[k]));
				strncpy((char*)UIwifi.wifi_user_pswd, (char*)comm_par.sy_par.wifi.Sspswd[k],  sizeof(comm_par.sy_par.wifi.Sspswd[k]));
				SendSTRDataToDisplay(WIFI_PW_WR,(uint8_t*)&UIwifi.wifi_user_pswd);
				comm_par.sy_par.wifi.active_id=k;
				break;
			}
			comm_par.sy_par.wifi.active_id=5;
		}
		if(Wifi_ssid[Scan_no].Authenication_type==0)
		{
			SendN16DataToDisplay(WIFI_PASS_STR_En,0);
			memset(UIwifi.wifi_user_pswd,0x00,sizeof(UIwifi.wifi_user_pswd));

		}
		else
		{
			SendN16DataToDisplay(WIFI_PASS_STR_En,1);
			if(comm_par.sy_par.wifi.active_id==5)
			{
				memset(UIwifi.wifi_user_pswd,0x00,sizeof(UIwifi.wifi_user_pswd));
				SendSTRDataToDisplay(WIFI_PW_WR,(uint8_t*)&UIwifi.wifi_user_pswd);
			}
		}
	}
	else
	{
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)SSid_Warning);
	}
	return 0;
}

