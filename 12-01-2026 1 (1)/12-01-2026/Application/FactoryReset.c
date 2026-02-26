/*
 * FactoryReset.c
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
#include "FactoryReset.h"
//#include "FillManager.h"


UIVERSION_LIMITS UIversion_limits;
PLUMBING_OPTIONS Plumbing_options;
uint8_t ver;
uint8_t Dummy_string[8];
uint8_t Factory_Reset_PowerON;

uint8_t total_crc_chk[30]={0};
uint8_t update_flag;

 uint8_t Edm_Mode_Sts;
 uint8_t serial_data[36];

void reset_fn()
{
	TargetPage = 52;
	SendCmdToDisplay(LOAD_PAGE_CMD);
}

void reset_scheduler_default(void)
{
		UIScheduledEventData.ScheduledFillRemainderEn		= DEFAULT_KEY_DISABLE;
		UIScheduledEventData.MaintRemainderEn				= DEFAULT_KEY_DISABLE;
		UIScheduledEventData.FillRemainderEn			    = DEFAULT_KEY_DISABLE;

		SaveScheduledSettings(&UIScheduledEventData);
}



/*
 * *****************************************************************************
 * Function name: Settings_Init
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to set variables  at initial state
 * Parameters:
 *  Input  : none
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Settings_Init()
{
	uint8_t   brr[2];
	uint16_t crc16;
//	login_cred[0].user=1;
//	uint8_t data[300]={0};
//	memset((char*)data, 0xFF, 300);
//	EEPROM_Write(EEPROM_SCHEDULED_EN_ADDRESS, 0,data,254);
//	EEPROM_Write(TEMP_RTD_ADDRESS, 0,data,254);
//	EEPROM_Write(DP_CALIB_ADDRESS, 0,data,254);
//	EEPROM_Write(user4_crc, 0,data,254);
//	EEPROM_Write(1024, 0,data,254);
//	EEPROM_Write(1024+TEMP_RTD_ADDRESS, 0,data,254);
//	EEPROM_Write(1024+DP_CALIB_ADDRESS, 0,data,254);
//	EEPROM_Write(1024+user4_crc, 0,data,256);
//	EEPROM_Write(2048, 0,data,256);
//	EEPROM_Write(Aws_user_id, 0,data,64);
//	EEPROM_Write(wifi_user_add, 0,data,256);
	//EEPROM_Write(, 0,data,150);
//	EEPROM_Write(Aws_user_id, 0,(uint8_t*)&comm_par.sy_par.iot.User_ID,64);
//	EEPROM_Write(IOT_SER_NUM_ADDR, 0,(uint8_t*)&Plumbing_options.IOT_serial,36);
//	EEPROM_Write(Aws_url_address, 0,(uint8_t*)&comm_par.sy_par.iot.iot_url,192);
	memset(Dummy_string, 0, sizeof(Dummy_string));   // Erase
	//eeprom settings
  EEPROM_Read(PLUMBING_OPTION, 0,(uint8_t*)&vp_param_rw_str.plumbing_options,1);
  EEPROM_Read(FREZER_MODEL, 0,(uint8_t*)&Plumbing_options.Board_version,1);
  EEPROM_Read(DEV_SER_NUM_ADDR, 0,(uint8_t*)&Plumbing_options.serial,18);
  EEPROM_Read(IOT_SER_NUM_ADDR, 0,(uint8_t*)&Plumbing_options.IOT_serial,36);
  EEPROM_Read(Token_address, 0,(uint8_t*)&Plumbing_options.Iot_token,36);
  strcpy((char*)&serial_data,(const char*)&Plumbing_options.IOT_serial);
 // EEPROM_Read(IOT_FIRM_DOW, 0,(uint8_t*)&UIUpdateIOTFIRMsettings.Firmware_update_en,1);
  if(Plumbing_options.serial[0] == 0xff)
  {
	  memset(&Plumbing_options.serial,0x00,sizeof(Plumbing_options.serial));
  }

  /*if(UIUpdateIOTFIRMsettings.Firmware_update_en<0 || UIUpdateIOTFIRMsettings.Firmware_update_en==0xff)
  {
	  UIUpdateIOTFIRMsettings.Firmware_update_en=0;
  }

  EEPROM_Read(FIRM_4_20_DOW, 0,(uint8_t*)&UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update,1);
  if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update<0 || UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update==0xff)
  {
  	  UIUpdateIOTFIRMsettings.Firmware_update_en=0;
  }

  if(Plumbing_options.Board_version<0x0A)
  {
	  Plumbing_options.Board_version;
  }
  else
  {
	  Plumbing_options.Board_version=1;
  }*/
  if(vp_param_rw_str.plumbing_options==0xFF)
  {
	  vp_param_rw_str.plumbing_options =0x05;
  }
  if((vp_param_rw_str.plumbing_options & BATTERY_SENS_SEL))
  {
	 Plumbing_options.Battery_status=1;
  }
  else
  {
	 Plumbing_options.Battery_status=0;
  }

	for(int i=1;i<3;i++)
	{
		uint8_t bfr[2];
		//EEPROM_Write(eep_user_reg[i].User_add, 0,(uint8_t*)&login_cred[i].admin_id,8);
		EEPROM_Read(eep_user_reg[i].User_add, 0,(uint8_t*)&login_cred[i].admin_id,8);
		EEPROM_Read(eep_user_reg[i].sts_add, 0,(uint8_t*)&login_cred[i].user,1);
		EEPROM_Read(eep_user_reg[i].User_add_crc, 0,bfr,2);

		if(login_cred[i].admin_id[0]==0xff)
		{
			memset(login_cred[i].user_id,0x0,sizeof(login_cred[i]));
		}
		{
			crc16 = usMBCRC16((uint8_t*)&login_cred[i].admin_id, 8);
		}

		EEPROM_Read(eep_user_reg[i].emp_add, 0,(uint8_t*)&login_cred[i].emp_id,8);
		EEPROM_Read(eep_user_reg[i].emp_add_crc, 0,bfr,2);

		if(((bfr[0]==0xff)&&(bfr[1]==0xff)))//||((bfr[0]==0x00)&&(bfr[1]==0x00)))
		{
			 total_crc_chk[17]=1;
		}
		else
		{
			crc16 = usMBCRC16((uint8_t*)&login_cred[i].emp_id, 8);
		}

		EEPROM_Read(eep_user_reg[i].pw_add, 0,(uint8_t*)&login_cred[i].user_pswd,8);
		EEPROM_Read(eep_user_reg[i].pw_crc_add, 0,bfr,2);
		{
			crc16 = usMBCRC16((uint8_t*)&login_cred[i].user_pswd, 8);
		}
	}
	for(int i=3;i<11;i++)
	{
		uint8_t bfr[2];
		EEPROM_Read(eep_user_reg[i].User_add, 0,(uint8_t*)&login_cred[i].user_id,8);
		EEPROM_Read(eep_user_reg[i].User_add_crc, 0,bfr,2);

		if(login_cred[i].user_id[0]==0xff)
		{
			memset(login_cred[i].user_id,0x0,sizeof(login_cred[i]));
		}
		{
			crc16 = usMBCRC16((uint8_t*)&login_cred[i].user_id, 8);
			if ((bfr[0]) == ((crc16 & 0xff00) >> 8) &&
					  (bfr[1]) == (crc16 & 0x00ff))
			{
				 total_crc_chk[15]=1;
			}
		}
		EEPROM_Read(eep_user_reg[i].emp_add, 0,(uint8_t*)&login_cred[i].emp_id,8);
		EEPROM_Read(eep_user_reg[i].emp_add_crc, 0,bfr,2);
		{
			crc16 = usMBCRC16((uint8_t*)&login_cred[i].emp_id, 8);
			 if ((bfr[0]) == ((crc16 & 0xff00) >> 8) &&
					  (bfr[1]) == (crc16 & 0x00ff))
			  {
				total_crc_chk[14]=1;
			  }
		}

		EEPROM_Read(eep_user_reg[i].pw_add, 0,(uint8_t*)&login_cred[i].user_pswd,8);
		EEPROM_Read(eep_user_reg[i].pw_crc_add, 0,bfr,2);
		if(((bfr[0]==0xff)&&(bfr[1]==0xff)))//||((bfr[0]==0x00)&&(bfr[1]==0x00)))
		{
			total_crc_chk[13]=1;
		}
		else
		{
			crc16 = usMBCRC16((uint8_t*)&login_cred[i].user_pswd, 8);
			if ((bfr[0]) == ((crc16 & 0xff00) >> 8) &&
					  (bfr[1]) == (crc16 & 0x00ff))
			 {
				 total_crc_chk[13]=1;
			 }
		}
	}



	//wifi
	EEPROM_Read(wifi_user_add, 0,(uint8_t*)&comm_par.sy_par.wifi.uuid[0],32);
	EEPROM_Read(wifi_user_add_crc,0,brr,2);
	if(comm_par.sy_par.wifi.uuid[0]==0xFF)
	{
		memset((char*)comm_par.sy_par.wifi.uuid, 0x00, sizeof(comm_par.sy_par.wifi.uuid));
	}
	/*if(((brr[0]==0xff)&&(brr[1]==0xff)))//||((brr[0]==0x00)&&(brr[1]==0x00)))
	{
		total_crc_chk[19]=1;
		memset((char*)comm_par.sy_par.wifi.uuid, 0x00, sizeof(comm_par.sy_par.wifi.uuid));

	}
	else
	{
		crc16 = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.uuid[0][0], 32);
		if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
				  (brr[1]) == (crc16 & 0x00ff))
		{
			 total_crc_chk[19]=1;
		}
		else
		{
			memset((char*)comm_par.sy_par.wifi.uuid, 0x00, sizeof(comm_par.sy_par.wifi.uuid));
		}
	}*/
	EEPROM_Read(wifi_pw_add, 0,(uint8_t*)&comm_par.sy_par.wifi.pswd[0],32);
	EEPROM_Read(wifi_pw_add_crc,0,brr,2);
	if(comm_par.sy_par.wifi.pswd[0]==0xFF)
	{
		memset((char*)comm_par.sy_par.wifi.pswd, 0x00, sizeof(comm_par.sy_par.wifi.pswd));
	}
	/*if(((brr[0]==0xff)&&(brr[1]==0xff)))//||((brr[0]==0x00)&&(brr[1]==0x00)))
	{
		total_crc_chk[20]=1;
		memset((char*)comm_par.sy_par.wifi.pswd, 0x00, sizeof(comm_par.sy_par.wifi.pswd));
	}
	else
	{
		crc16 = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.pswd[0][0], 32);
		if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
				  (brr[1]) == (crc16 & 0x00ff))
		{
			 total_crc_chk[20]=1;
		}
		else
		{
			memset((char*)comm_par.sy_par.wifi.pswd, 0x00, sizeof(comm_par.sy_par.wifi.pswd));
		}
	}*/
	EEPROM_Read(Wifi_ssid_add, 0,(uint8_t*)&comm_par.sy_par.wifi.Ssid[0][0],160);
	EEPROM_Read(Wifi_ssid_add_crc,0,brr,2);
	crc16 = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.Ssid[0][0], 160);
	if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
	{
		// total_crc_chk[20]=1;
	}
	else
	{
		memset((char*)comm_par.sy_par.wifi.Ssid, 0x00, sizeof(comm_par.sy_par.wifi.Ssid));
	}

	EEPROM_Read(wifi_pass_add, 0,(uint8_t*)&comm_par.sy_par.wifi.Sspswd[0][0],160);
	EEPROM_Read(wifi_pass_add_crc,0,brr,2);
	crc16 = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.Sspswd[0][0], 160);
	if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
	{
		 //total_crc_chk[20]=1;
	}
	else
	{
		memset((char*)comm_par.sy_par.wifi.Sspswd, 0x00, sizeof(comm_par.sy_par.wifi.Sspswd));
	}


	EEPROM_Read(wifi_connect_sts, 0,(uint8_t*)&UIwifi.connect_sts,2);
	if(UIwifi.connect_sts==0xFF)
	{
		UIwifi.connect_sts=0;
		UIwifi.IOT_connect_sts=0;
	}
	EEPROM_Read(Aws_url_address, 0,(uint8_t*)&comm_par.sy_par.iot.iot_url,192);
	EEPROM_Read(Aws_user_id, 0,(uint8_t*)&comm_par.sy_par.iot.User_ID,63);
	EEPROM_Read(Sub_cli_address, 0,(uint8_t*)&comm_par.sy_par.iot.iot_phone,20);
	//EEPROM_Read(Aws_missed_acks_add, 0,(uint8_t*)&attempts_info.nof_iot_acks_missed,4);
	if(comm_par.sy_par.iot.iot_url[0]==0xff)
	{
		memset(&comm_par.sy_par.iot.iot_url,  0x00, sizeof(comm_par.sy_par.iot.iot_url));
	}
	if(comm_par.sy_par.iot.User_ID[0]==0xff)
	{
		memset(&comm_par.sy_par.iot.User_ID,  0x00, sizeof(comm_par.sy_par.iot.User_ID));
	}
	if(comm_par.sy_par.iot.iot_email[0]==0xff)
	{
		memset(&comm_par.sy_par.iot.iot_email,  0x00, sizeof(comm_par.sy_par.iot.iot_email));
	}
	if(comm_par.sy_par.iot.iot_phone[0]==0xff)
	{
		memset(&comm_par.sy_par.iot.iot_phone,  0x00, sizeof(comm_par.sy_par.iot.iot_phone));
	}
	/*if(attempts_info.nof_iot_acks_missed==0xff)
	{
		attempts_info.nof_iot_acks_missed=0;
	}

	if(((brr[0]==0xff)&&(brr[1]==0xff)))
	{
		total_crc_chk[20]=1; //21
		DelAwscredentials();
	}
	else
	{
		crc16 = usMBCRC16((uint8_t*)&comm_par.sy_par.iot.iot_url,200);
		if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
				  (brr[1]) == (crc16 & 0x00ff))
		{
			 total_crc_chk[20]=1;//21
		}
		else
		{
			// DelAwscredentials();
		}
	}
	*/

	  EEPROM_Read(INIT_FACTORY_RESET,0,(uint8_t*)&Factory_Reset_PowerON,1);

	  if(Factory_Reset_PowerON!=34)
	  {
		  Factory_Reset_PowerON=34;
		  EEPROM_Write(INIT_FACTORY_RESET,0,(uint8_t*)&Factory_Reset_PowerON,1);
		  reset_factory_default();
	  }

	  /*Scheduler Enable Disable Settings	---1*/
	  EEPROM_Read(EEPROM_SCHEDULED_EN_ADDRESS,0,(uint8_t*)&vp_param_rw_str.ScheduledFillRemainderEn,8);
	  EEPROM_Read(EEPROM_SCHEDULED_EN_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.ScheduledFillRemainderEn, 8);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			total_crc_chk[0]=1;
		  LoadScheduledSettings(&UIScheduledEventData);
		  }
		  else
		  {
				 reset_scheduler_default(); //reset_factory_default();
		  }
	  }
	  /*Scheduled Fill						---2*/
	  EEPROM_Read(SCHEDULED_FILL_ADDRESS,0,(uint8_t*)&vp_param_rw_str.Fill_HoursSettings,12);
	  EEPROM_Read(SCHEDULED_FILL_ADDRESS_CRC,0,brr,2);
      {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Fill_HoursSettings,12);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			total_crc_chk[1]=1;
		  LoadScheduledFillSettings(&UIScheduledFillData);
		  }
		  else
		  {
			  reset_scheduled_fill_default(); // reset_factory_default();
		  }
	  }
	  /*Scheduled Check level				---3*/
	  EEPROM_Read(SCHEDULED_RLEVEL_ADDRESS,0,(uint8_t*)&vp_param_rw_str.FILLR_MonthSettings,14);
	  EEPROM_Read(SCHEDULED_RLEVEL_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.FILLR_MonthSettings,14);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			  total_crc_chk[2]=1;
			  LoadScheduledFillRemSettings(&UIFillRemainderData);
		  }
		  else
		  {
			  reset_scheduled__REM_fill_default();// reset_factory_default();
		  }
	  }
	  /*Scheduled Maintenance				---4*/
	  EEPROM_Read(SCHEDULED_MAINT_ADDRESS,0,(uint8_t*)&vp_param_rw_str.Maint_Hours12Settings,16);
	  EEPROM_Read(SCHEDULED_MAINT_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Maint_Hours12Settings,16);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			  total_crc_chk[3]=1;
		  LoadMaintenenceSettings(&UIMaintenanceRemainderSettings);
		  }
		  else
		  {
				 reset_maintenence_default();// reset_factory_default();
		  }
	  }

	//  EEPROM_Read(NW_ID_ADDRESS, 0,(uint8_t*)&UIModbusData.NwID,8);
	  /*Modbus Settings						---5*/
	  EEPROM_Read(MODBUS_ADDRESS,0,(uint8_t*)&vp_param_rw_str.STM_DeviceIDCryolink,18);
	  EEPROM_Read(MODBUS_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.STM_DeviceIDCryolink,18);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
		  LoadCryoSettings(&UIModbusData);
		  LoadPhaseSettings(&UIModbusData);
		  }
		  else
		  {
			  reset_ModbusData_default();// reset_factory_default();
		  }
	  }

	 /* EEPROM_Read(Network_4_20_mA, 0,(uint8_t*)&vp_param_rw_str.EXD_MD_Switch_settings,16);
	  EEPROM_Read(Network_4_20_mA_crc,0,brr,2);
	  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.EXD_MD_Switch_settings,16);
	  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
		  (brr[1]) == (crc16 & 0x00ff))
	  {
		  Load420mASettings(&UI_4_20mAsettings);
	  }
	  else
	  {
		  reset_4_20_mA_default();// reset_factory_default();
	  }
	  EEPROM_Read(Network_4_20_mA_config, 0,(uint8_t*)&vp_param_rw_str.T1_MIN_CONFIG_CH1_settings,24);
	  EEPROM_Read(Network_4_20_mA_config_crc,0,brr,2);
	  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.T1_MIN_CONFIG_CH1_settings,24);
	  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
		  (brr[1]) == (crc16 & 0x00ff))
	  {
		  Load420mAconfigSettings(&UI_4_20mAconfigSettings);
	  }
	  else
	  {
		  reset_4_20_mA_config_default();// reset_factory_default();
	  }*/
	  /*Display Settings					---7*/
	  EEPROM_Read(DISPLAY_ADDRESS,0,(uint8_t*)&vp_param_rw_str.STM_Brightness,12);
	  EEPROM_Read(DISPLAY_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.STM_Brightness,12);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
		  LoadDispSettings(&UIDisplayData);
		  }
		  else
		  {
			  reset_DisplaySettings_default(); // reset_factory_default();
		  }
	  }
	  /*Lid Settings						---8*/
	  /*Scheduled LN2 Supply				---9*/
//	  EEPROM_Read(SCHEDULED_LN2SUPPLY_ADDRESS,0,(uint8_t*)&vp_param_rw_str.LN2Supply_HoursSettings,12);
//	  EEPROM_Read(SCHEDULED_LN2SUPPLY_ADDRESS_CRC,0,brr,2);
//	  if((brr[0]==0xff )&&(brr[1]==0xff))
//	   {
//
//	  }
//	  else
//	  {
//		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.LN2Supply_HoursSettings,12);
//		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
//			  (brr[1]) == (crc16 & 0x00ff))
//		  {
//		  LoadScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//		  }
//		  else
//		  {
//			  reset_SupplyRemainder_default(); // reset_factory_default();
//		  }
//	  }
	  /*Temperature Settings				---10*/
	  EEPROM_Read(TEMP_RTD_ADDRESS,0,(uint8_t*)&vp_param_rw_str.high_temp1.value,34);
	  EEPROM_Read(TEMP_RTD_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.high_temp1.value,34);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {

		      LoadTempSettings(&UITempeSettingsData);
		  }
		  else
		  {
			  reset_TempeSettingsData_default();// reset_factory_default();
		  }
	  }

	  EEPROM_Read(LEVEL_ADDRESS,0,(uint8_t*)&vp_param_rw_str.Empty_en,6);
	  EEPROM_Read(LEVEL_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Empty_en,6);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {

			  LoadLevelSettings(&UILevelSettingsData);
		  }
		  else
		  {
			  reset_LevelSettingsData_default();// reset_factory_default();
		  }
	  }

	  /*Alaram Settings						---6*/
	  EEPROM_Read(ALARM_ADDRESS,0,(uint8_t*)&vp_param_rw_str.RemoteAlarmDelaySwitch,14);
	  EEPROM_Read(ALARM_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.RemoteAlarmDelaySwitch,14);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			  total_crc_chk[5]=1;
		  LoadAlarmSettings(&UIAlarmSettingsData);
		  }
		  else
		  {
			  reset_AlarmSettings_default();// reset_factory_default();
		  }
	  }

	/*  EEPROM_Read(Purge_valve, 0,(uint8_t*)&purge_control.Skip_purge,2);
	   if(purge_control.Skip_purge==0xFF)
	   {
		  purge_control.Skip_purge=1;
		  purge_control.Manual_purge_con=1;
	   }
	   purge_control.Prev_skip=purge_control.Skip_purge;
	   purge_control.Prev_manual=purge_control.Manual_purge_con;
	   if(purge_control.Manual_purge_con==0)
	   {
		  RegisterAlarm(MANUAL_PURGE_EN, &purge_control.Manual_purge_con, 30*60, SYNC_ON_REGISTER);
	   }*/
	  /*Level Settings						---11*/
	  /*RTC Settings						---12*/
	  EEPROM_Read(RTC_ADDRESS,0,(uint8_t*)&vp_param_rw_str.RTC_AutoTimeSyncControl,6);
	  EEPROM_Read(RTC_ADDRESS_CRC,0,brr,2);
	  {
		   crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.RTC_AutoTimeSyncControl,6);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			  total_crc_chk[11]=1;
		  LoadRTCSettings(&UIDateTimeSettings);
		  }
		  else
		  {
			  reset_DateTimeSettings_default(); // reset_factory_default();
		  }
	  }
	  EEPROM_Read(FREEZER_ID_ADDRESS, 0,(uint8_t*)&UIFreezerStruct.FreezerID,8);
	  EEPROM_Read(FREEZER_ID_ADDRESS_CRC,0,brr,2);
	  {
		  crc16 = usMBCRC16((uint8_t*)&UIFreezerStruct.FreezerID,8);
		  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
			  (brr[1]) == (crc16 & 0x00ff))
		  {
			  SendSTRDataToDisplay(0x00080,(uint8_t *)&UIFreezerStruct.FreezerID);
		  }
		  else
		  {
				 memset(&UIFreezerStruct.FreezerID, 0,sizeof(UIFreezerStruct.FreezerID));
		  }
	  }

	  ULOG_TRACE("Freezer ID : %s", UIFreezerStruct.FreezerID);
	  ULOG_TRACE("SW Version : %s, HW Version : V1.0",APP_FW_VER);

	  float CalibData[3] = {0.0, 0.0, 0.0};
	  EEPROM_Read(RTD1_CALIB_ADDRESS, 0,(uint8_t*)&CalibData,12);
      if (((CalibData[0] > MIN_RTD_CONST_C1) && (CalibData[0] < MAX_RTD_CONST_C1)) &&
     	  ((CalibData[1] > MIN_RTD_CONST_C2) && (CalibData[1] < MAX_RTD_CONST_C2)) &&
		  ((CalibData[2] < MIN_RTD_OFFSET)   && (CalibData[2] > MAX_RTD_OFFSET)))
      {
    		cal_par_str.rtd1_param[0] = (float)CalibData[0];
    		cal_par_str.rtd1_param[1] = (float)CalibData[1];
    		cal_par_str.rtd1_param[2] = (float)CalibData[2];
      }

	  EEPROM_Read(RTD2_CALIB_ADDRESS, 0,(uint8_t*)&CalibData,12);
      if (((CalibData[0] > MIN_RTD_CONST_C1) && (CalibData[0] < MAX_RTD_CONST_C1)) &&
     	  ((CalibData[1] > MIN_RTD_CONST_C2) && (CalibData[1] < MAX_RTD_CONST_C2)) &&
		  ((CalibData[2] < MIN_RTD_OFFSET)   && (CalibData[2] > MAX_RTD_OFFSET)))
      {
    		cal_par_str.rtd2_param[0] = (float)CalibData[0];
    		cal_par_str.rtd2_param[1] = (float)CalibData[1];
    		cal_par_str.rtd2_param[2] = (float)CalibData[2];
      }

	  EEPROM_Read(DP_CALIB_ADDRESS, 0,(uint8_t*)&CalibData,12);
     
      //ctrs
#ifdef EN_CTRS

      EEPROM_Read(load_CALIB_ADDRESS, 0,(uint8_t*)&CTRS_cal_var.load_slope,28);
      if(CTRS_cal_var.load_cell1_raw_value== -1)
      {
    		CTRS_cal_var.load_slope=Load_slope;
    		CTRS_cal_var.load_constant=Load_constant;
    		CTRS_cal_var.load_cell1_raw_value=rawload_cell1_data;
    		CTRS_cal_var.load_cell2_raw_value=rawload_cell2_data;
    		CTRS_cal_var.load_cell3_raw_value=rawload_cell3_data;
    		CTRS_cal_var.load_cell4_raw_value=rawload_cell4_data;
    		CTRS_cal_var.load_cell5_raw_value=rawload_cell5_data;
    		CTRS_cal_var.total_wt_data=raw_total_data;
      }
      CTRS_cal_var.total_wt_data=CTRS_cal_var.load_cell1_raw_value+CTRS_cal_var.load_cell2_raw_value+CTRS_cal_var.load_cell3_raw_value+CTRS_cal_var.load_cell4_raw_value+CTRS_cal_var.load_cell5_raw_value;
      EEPROM_Read(SENSOR_SETTINGS, 0,(uint8_t*)&CTRS_rd_para.high_temp.value,24);
      EEPROM_Read(SENSOR_SETTINGS_CRC,0,brr,2);

	  crc16 = usMBCRC16((uint8_t*)&CTRS_rd_para.high_temp.value, 24);
	  if ((brr[0]) == ((crc16 & 0xff00) >> 8) &&
		  (brr[1]) == (crc16 & 0x00ff))
	  {
		  LoadCTRSSettings(&UICTRStemperature);
//		  reset_CTRS_config_default();
	  }
	  else
	  {
		  reset_CTRS_config_default(); // reset_factory_default();
	  }
	  EEPROM_Read(ctrs_load_usage, 0,(uint8_t*)&ctrs_ws_para.ctrs_ln2_usage,4);
	  if(ctrs_ws_para.ctrs_ln2_usage>1000)
	  {
		  ctrs_ws_para.ctrs_ln2_usage=0;
	  }
#endif

      //read ln2 usage
     /* EEPROM_Read(ln2_usage, 0,(uint8_t*)&Ln2_usage.datausage,4);
      EEPROM_Read(pre_mean_usage, 0,(uint8_t*)&pre_datausage,4);
      EEPROM_Read(High_ln2_usage, 0,(uint8_t*)&Ln2_usage.pre_ln2usage,4);
      Ln2_usage.datausage=Ln2_usage.pre_ln2usage;
      Ln2_usage.pre_datausage=Ln2_usage.pre_ln2usage;*/

      //remote or local
      EEPROM_Read(CONTACT_ADDRESS, 0,(uint8_t*)&SystemStruct.Supply_con,40);
      if(SystemStruct.Supply_con[0] == 0xFF)
      {
    	  memset(SystemStruct.Supply_con,0x0,sizeof(SystemStruct.Supply_con));
      }
      EEPROM_Read(modbus_com, 0,(uint8_t*)&vp_rt_wr_str.MBStatus,1);
      EEPROM_Read(Firmware_update_flag, 0,(uint8_t*)&Update.update_firmware ,5);
      if(Update.update_firmware==1)
	  {
//    	  SD_info.bin_present = 1;
	  }
//      EEPROM_Read(GROUP_ADDRESS, 0,(uint8_t*)&GroupStruct.Group_en ,184);
     /* EEPROM_Read(GROUP_ADDRESS1, 0,(uint8_t*)&GroupStruct.Group_C ,244);

      if((GroupStruct.Group_en==0xffff)||(GroupStruct.Group_en== -1) || (GroupStruct.Group_sel < 1)||(GroupStruct.Group_sel > 4))
      {
    	  reset_Group_default();
      }

      LoadGROUPSettings(&UIUpdateGroupsettings);
      if(Update.update_display==255)
      {
    	  Update.update_display=0;
      }
      if(Update.Bist_update==255)
      {
    	  Update.Bist_update=0;
      }
      if(Update.update_firmware==255)
	  {
    	  Update.update_firmware=0;
	  }*/

      EEPROM_Read(LED_ADDRESS, 0, (uint8_t *)&vp_param_rw_str.ERRORS_ONLY_en, 26);
      EEPROM_Read(LED_ADDRESS_CRC, 0, brr, 2);
      crc16 = usMBCRC16((uint8_t *)&vp_param_rw_str.ERRORS_ONLY_en, 26);
	  if(brr[0] == ((crc16 & 0xFF00) >> 8) && brr[1] ==(crc16 & 0x00FF))
	  {
		  LoadLedSettings(&UILEDSettingsData);
	  }
	  else
	  {
		  resetLedSettingsData_default();
	  }

      EEPROM_Read(Firmware_flag_address, 0,(uint8_t*)&firmware_timer_en ,1);
      if(firmware_timer_en==1)
      {
    	  update_flag=1;
    	  EEPROM_Read(Firmware_update_address, 0,(uint8_t*)&UpdateStruct.Update_MonthSettings ,10);
      }
      else
      {
    	  firmware_timer_en=0;//do nothing
      }
      if((vp_param_rw_str.plumbing_options & TEMP_SENS_SEL))
      {
		 vp_param_rw_str.t2enable=0;
		 Plumbing_options.Temp_sen=0;
	  }
      else
      {
      	  Plumbing_options.Temp_sen=1;
      }
  /*    if((vp_param_rw_str.plumbing_options & LVL_SENS_SEL))
      {
      	 vp_param_rw_str.thermisterenable=0;
      	 Plumbing_options.Level_sen=0;
      }
      else
      {
      	  Plumbing_options.Level_sen=1;
      }*/
      //
      /*uint8_t rem_data[3];
      EEPROM_Read(LN2_CHK_DATE,0,rem_data,3);

      if(1)// ((brr[0]) == ((crc16 & 0xff00) >> 8) &&(brr[1]) == (crc16 & 0x00ff))
	  {
    	  memcpy((uint8_t*)&date_rem.date_chk,rem_data,3);
	  }
      else
	  {
    	  date_rem.date_chk=0;
    	  date_rem.date_ln2=0;
    	  date_rem.date_sch_fill=0;
    	  EEPROM_Write(LN2_CHK_DATE,0,&date_rem.date_chk,2);
    	  EEPROM_Write(SUPPLY_DATE,0,&date_rem.date_sch_fill,1);
	  }
      if(vp_param_rw_str.plumbing_options & MB_SINTESY_MODE )
      {
    	  vp_param_rw_str.cryolink_phase2_sel=MB_SINTESY_MODE;
    	  if(vp_param_rw_str.STM_DeviceIDCryolink)
    	  {
    		  vp_param_rw_str.mb_cfg.node_id =vp_param_rw_str.STM_DeviceIDCryolink;
    	  }
    	  else
    	  {
    		  vp_param_rw_str.STM_DeviceIDCryolink=1;
    		  vp_param_rw_str.mb_cfg.node_id=vp_param_rw_str.STM_DeviceIDCryolink;
    	  }

      }
      else
      {
      	vp_param_rw_str.mb_cfg.node_id =vp_param_rw_str.STM_DeviceIDPhasetwo;
      }
      if(vp_param_rw_str.cryolink_phase2_sel == MB_SINTESY_MODE )
	  {
		vp_param_rw_str.mb_cfg.mode =vp_param_rw_str.STM_BaudRateCryolink ;
	  }
	  else
	  {
		vp_param_rw_str.mb_cfg.mode=vp_param_rw_str.STM_BaudRatePhasetwo;
	  }*/
      uint8_t  admin_id_def[MAX_ID_SIZE]={'s','u','p','e','r','a','d','m'};
      uint8_t  admin_pw_def[MAX_ID_SIZE];
      memset(admin_pw_def,0,MAX_ID_SIZE);
	  int i=0,j=0;
	  for(i=8;(i<18);i++)
	  {
		  if( Plumbing_options.serial[i] != '-')
		  {
			  admin_pw_def[j] =Plumbing_options.serial[i];
			  j++;
		  }
	  }
      memcpy((char*)login_cred[0].admin_id, (char*)admin_id_def, MAX_ID_SIZE);
      memcpy((char*)login_cred[0].user_pswd, (char*)admin_pw_def, MAX_ID_SIZE);
      //Settings_write_data();
}


void reset_Group_default(void)
{
	UIUpdateGroupsettings.Group_en=0;
	UIUpdateGroupsettings.Group_sel=1;
	memset((uint8_t*)&UIUpdateGroupsettings.Group_col, 0, sizeof(UIUpdateGroupsettings.Group_col));
	memset((uint8_t*)&UIUpdateGroupsettings.Group_A, 0, sizeof(UIUpdateGroupsettings.Group_A));
	memset((uint8_t*)&UIUpdateGroupsettings.Group_B, 0, sizeof(UIUpdateGroupsettings.Group_B));
	memset((uint8_t*)&UIUpdateGroupsettings.Group_C, 0, sizeof(UIUpdateGroupsettings.Group_C));
	memset((uint8_t*)&UIUpdateGroupsettings.Group_D, 0, sizeof(UIUpdateGroupsettings.Group_D));
	SaveGROUPSettings(&UIUpdateGroupsettings);
}
void reset_scheduled_fill_default(void)
{
	//2//Scheduled Fill Settings
		UIScheduledFillData.HoursSettings					= DEFAULT_HOURS;
		UIScheduledFillData.MinutesSettings					= DEFAULT_MIN;
		UIScheduledFillData.AMPMSettings					= DEFAULT_AMPM;
		UIScheduledFillData.DaySettings						= DEFAULT_DAY;
		UIScheduledFillData.Hours12Settings					= DEFAULT_HOURS;
//		UIScheduledFillData.Minutes12Settings				= DEFAULT_MIN;
		SaveScheduledFillSettings(&UIScheduledFillData);
}

void reset_scheduled__REM_fill_default(void)
{
	//3//Scheduled Check level Settings
	    UIFillRemainderData.MonthSettings		= DEFAULT_MONTH;
	    UIFillRemainderData.DateSettings			= DEFAULT_DAYE;
	    UIFillRemainderData.YearSettings			= DEFAULT_YEARS;
		UIFillRemainderData.Hours24Settings				= DEFAULT_HOURS;
		UIFillRemainderData.MinutesSettings			= DEFAULT_MIN;
		UIFillRemainderData.AMPMSettings				= DEFAULT_AMPM;
		UIFillRemainderData.Hours12Settings			= DEFAULT_HOURS;
		SaveScheduledFillRemSettings(&UIFillRemainderData);
}

void reset_maintenence_default(void)
{
	//4//Scheduled Maintenance Settings
		UIMaintenanceRemainderSettings.Hours12Settings		= DEFAULT_HOURS;
		//UIMaintenanceRemainderSettings.Minutes12Settings	= DEFAULT_MIN;
		UIMaintenanceRemainderSettings.MonthSettings		= DEFAULT_MONTH;
		UIMaintenanceRemainderSettings.DateSettings			= DEFAULT_DAYE;
		UIMaintenanceRemainderSettings.YearSettings			= DEFAULT_YEARS;
		UIMaintenanceRemainderSettings.HoursSettings		= DEFAULT_HOURS;
		UIMaintenanceRemainderSettings.MinutesSettings		= DEFAULT_MIN;
		UIMaintenanceRemainderSettings.AMPMSettings			= DEFAULT_AMPM;
		SaveMaintenenceSettings(&UIMaintenanceRemainderSettings);
}

void reset_ModbusData_default(void)
{
	//5//Modbus Settings
//		UIModbusData.DeviceIDCryolink						= 1;
//		UIModbusData.BaudRateCryolink						= 2;
//		UIModbusData.FPByteOrderCryolink					= 1;
		UIModbusData.DeviceIDPhasetwo						= 1;
		UIModbusData.BaudRatePhasetwo						= 2;
		UIModbusData.DataRead_En						    = 0;
		//UIModbusData.FillMethodPhasetwo						= 1;
//		UIModbusData.EN_4_20_mA                             = 0;
		SaveCryoSettings(&UIModbusData);
		SavePhaseSettings(&UIModbusData);
}

/*void reset_4_20_mA_default(void)
{
	//5//Modbus Settings
	UI_4_20mAsettings.EXD_MD_Switch    =	0;
	UI_4_20mAsettings.Chan1_Switch     =	0;
	UI_4_20mAsettings.Chan2_Switch	   =	0;
	UI_4_20mAsettings.Chan1_Selction   =	0;
	UI_4_20mAsettings.Chan2_Selction   =	0;
	UI_4_20mAsettings.Source_sel	   =	0;
	UI_4_20mAsettings.Chan1_vol_cur	   =	0;
	UI_4_20mAsettings.Chan2_vol_cur	   =    0;

	Save420mASettings(&UI_4_20mAsettings);
}

void reset_4_20_mA_config_default(void)
{
	//5//Modbus Settings
	UI_4_20mAconfigSettings.T1_MIN_CONFIG_CH1     =	-2200;
	UI_4_20mAconfigSettings.T2_MIN_CONFIG_CH1     =	-2200;
	UI_4_20mAconfigSettings.DP_MIN_CONFIG_CH1	  =	0;
	UI_4_20mAconfigSettings.T1_MIN_CONFIG_CH2	  =	-2200;
	UI_4_20mAconfigSettings.T2_MIN_CONFIG_CH2	  =	-2200;
	UI_4_20mAconfigSettings.DP_MIN_CONFIG_CH2	  =	0;
	UI_4_20mAconfigSettings.T1_MAX_CONFIG_CH1	  =	-1300;
	UI_4_20mAconfigSettings.T2_MAX_CONFIG_CH1	  =	-1300;
	UI_4_20mAconfigSettings.DP_MAX_CONFIG_CH1	  =	340;
	UI_4_20mAconfigSettings.T1_MAX_CONFIG_CH2	  =	-1300;
	UI_4_20mAconfigSettings.T2_MAX_CONFIG_CH2	  =	-1300;
	UI_4_20mAconfigSettings.DP_MAX_CONFIG_CH2	  =	340;


	Save420mAconfigSettings(&UI_4_20mAconfigSettings);
}

void reset_CTRS_config_default(void)
{
	//5//Modbus Settings
	UICTRStemperature.Highloadlimit = 1000;
	UICTRStemperature.THighLimit    = -1300;
	UICTRStemperature.TLowLimit     = -2000;

	SaveCTRSSettings(&UICTRStemperature);
}*/

void reset_AlarmSettings_default(void)
{

		UIAlarmSettingsData.RemoteAlarmDelaySwitch			= DEFAULT_KEY_ENABLE;
		UIAlarmSettingsData.RemoteAlarmDelay				= 30;
		UIAlarmSettingsData.AudiableRepeatDelaySwitch		= DEFAULT_KEY_ENABLE;
		UIAlarmSettingsData.AudiableRepeatDelay				= 30;

		UIAlarmSettingsData.LN2UsageDelaySwitch				= DEFAULT_KEY_ENABLE;
		UIAlarmSettingsData.LN2UsageAlarm_value				= 20;

		UIAlarmSettingsData.Battery_sw_over			        = DEFAULT_KEY_ENABLE;

		SaveAlarmSettings(&UIAlarmSettingsData);
}

void reset_DisplaySettings_default(void)
{
	//7//Display Settings
		UIDisplayData.Brightness							= 50;
		UIDisplayData.TouchSound							= 1;
		UIDisplayData.ScreenSaverTime						= 2;
		UIDisplayData.LED_Brightness 						= 150;
		UIDisplayData.StatusLEDEn							= 1;

		SaveDispSettings(&UIDisplayData);
}

//void reset_SupplyRemainder_default(void)
//{
//	//9//Scheduled LN2 Supply
//		UILN2SupplyRemainderData.HoursSettings				= DEFAULT_HOURS;
//		UILN2SupplyRemainderData.MinutesSettings			= DEFAULT_MIN;
//		UILN2SupplyRemainderData.AMPMSettings				= DEFAULT_AMPM;
//		UILN2SupplyRemainderData.DaySettings				= DEFAULT_DAY;
//		UILN2SupplyRemainderData.Hours12Settings			= DEFAULT_HOURS;
////		UILN2SupplyRemainderData.Minutes12Settings			= DEFAULT_MIN;
//		SaveScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//}

void reset_TempeSettingsData_default(void)
{
	//10//Temperature Settings
	if((Plumbing_options.Board_version == 5) || (Plumbing_options.Board_version == 6))
	{
		UITempeSettingsData.T1HighLimit						= -1500;
		UITempeSettingsData.T2HighLimit						= -1500;
	}
	else
	{
		UITempeSettingsData.T1HighLimit						= -1700;
		UITempeSettingsData.T2HighLimit						= -1700;
	}
	UITempeSettingsData.T1HighLimitSwitch				= DEFAULT_KEY_ENABLE;
	UITempeSettingsData.T1LowLimitSwitch				= DEFAULT_KEY_DISABLE;
	UITempeSettingsData.T1LowLimit						= -2000;
	UITempeSettingsData.T2HighLimitSwitch				= DEFAULT_KEY_DISABLE;
	UITempeSettingsData.T2LowLimitSwitch				= DEFAULT_KEY_DISABLE;
	UITempeSettingsData.T2LowLimit						= -2000;
	UITempeSettingsData.TempUnitSelection				= DEFAULT_KEY_DISABLE;
	UITempeSettingsData.T2Enable						= DEFAULT_KEY_DISABLE;
	SaveTempSettings(&UITempeSettingsData);

}

void reset_LevelSettingsData_default(void)
{
	//11//LEVEL_DP_ADDRESS
	UILevelSettingsData.EMPTY_EN                      = DEFAULT_KEY_ENABLE;
	UILevelSettingsData.REFILLSOON				      = DEFAULT_KEY_ENABLE;
	UILevelSettingsData.FILL						  = DEFAULT_KEY_ENABLE;
	SaveLevelSettings(&UILevelSettingsData);
}
void resetLedSettingsData_default(void)
{
	UILEDSettingsData.ERRORS_ONLY = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.TEMP = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.LEVEL = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.VACUUM = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.STORAGE_FAIL = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.MAINS_BATT = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.NETWORK_ISSUES = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.BLINK_CTRL = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.NO_ERROR = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.ERRORS = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.WARNINGS = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.UPDATES = DEFAULT_KEY_DISABLE;
	UILEDSettingsData.NEW_COLOR = DEFAULT_KEY_DISABLE;

	SaveLedSettings(&UILEDSettingsData);

}
void reset_DateTimeSettings_default(void)
{
//	UIDateTimeSettings.AutoTimeSyncControl=0;
	UIDateTimeSettings.DateFormat=0;
	UIDateTimeSettings.TimeFormat=0;
	SaveRTCSettings(&UIDateTimeSettings);
}

void reset_FrezzID_default(void)
{
	 memset(&UIFreezerStruct.FreezerID , 0,9);
	 SendSTRDataToDisplay(0x00080,(uint8_t *)&UIFreezerStruct.FreezerID);
}


void reset_factory_default(void)
{
	 just_for_display_flg=1;//factory reset flag
	 reset_scheduler_default();
	 reset_scheduled_fill_default();
	 reset_scheduled__REM_fill_default();
	 reset_maintenence_default();
	 reset_ModbusData_default();
	 //reset_4_20_mA_default();
	 reset_TempeSettingsData_default();
	 resetLedSettingsData_default();
	 //reset_4_20_mA_config_default();
#ifdef EN_CTRS
	 reset_CTRS_config_default();
#endif
	 reset_AlarmSettings_default();
	 reset_DisplaySettings_default();

//	 reset_SupplyRemainder_default();
	// reset_LevelSettingsData_default();
	 reset_DateTimeSettings_default();
}



void factory_default(void)
{
	if(!just_for_display_flg)
	{
		if(factory_load_delay_flg)
		{
			if(factory_load_delay<=0)
			{
				if(factory_ok_enable_flg)
				{
					factory_ok_enable_flg=0;
					factory_load_delay_flg=0;
					SendN16DataToDisplay(FACTORY_RST_CNTDWN_EN, 0);//Disable Factory Value
					SendN16DataToDisplay(FACTORY_RST_STATUSCH, 1);//Factory index 1
					SendN16DataToDisplay(FACTORY_RST_CANCEL_En, 1);//touch for cancel
					SendN16DataToDisplay(FACTORY_RST_TCH_EN, 1);//Enable Ok Keys			//801AA
				}
			}
		}
		else
		{
			SendN16DataToDisplay(FACTORY_RST_VKEY, 1);//Enable Factory index icon	//800bc
			SendN16DataToDisplay(FACTORY_RST_CRSCANCEL_EN, 0);
			SendN16DataToDisplay(FACTORY_RST_STATUSCH, 0);//Factory index 0
			SendN16DataToDisplay(FACTORY_RST_CANCEL_En, 1);//touch for cancel
			SendN16DataToDisplay(FACTORY_RST_CNTDWN_EN, 1);//Enable Factory Values Display

		//	SendN16DataToDisplay(0x080990, 0);//Disable Other Key Scence	//need to ask
		//	SendN16DataToDisplay(0x08202A, 0);//Disable Other Key Scence	//need to ask
		//	SendN16DataToDisplay(0x080984, 1);//Enable Cancel Keys			//need to ask
			factory_ok_enable_flg=1;
			factory_load_delay_flg=1;
			factory_load_delay=5;
		}
	}
	else
	{
		if(factory_msg1_display_delay_flg)
		{
			if(factory_msg1_display_delay<=0)
			{
				if(factory_msg2_display_delay_flg)
				{
					if(factory_msg2_display_delay<=0)
					{
						factory_msg1_display_delay_flg=0;
						factory_msg2_display_delay_flg=0;
						just_for_display_flg=0;
						SendN16DataToDisplay(FACTORY_RST_VKEY, 0);//Enable Factory index icon
				//		SendN16DataToDisplay(0x080990, 1);//Enable Other Key Scence
						if(Login_id_cnt<3)
						{
			//				SendN16DataToDisplay(0x08202A, 1);//Enable Other Key Scence
						}

						Factoryreset_data_UIupdate();
					}
				}
				else
				{
					SendN16DataToDisplay(FACTORY_RST_STATUSCH, 3);//Factory index 3
					SendN16DataToDisplay(FACTORY_RST_CRSCANCEL_EN, 1);
					factory_msg2_display_delay_flg=1;
					factory_msg2_display_delay=3;
				}
			}
		}
		else
		{
			factory_msg1_display_delay_flg=1;
			factory_msg1_display_delay=5;
			SendN16DataToDisplay(FACTORY_RST_STATUSCH, 2);//Factory index 2
			SendN16DataToDisplay(FACTORY_RST_TCH_EN, 0);//Disable Ok Keys
			SendN16DataToDisplay(FACTORY_RST_CRSCANCEL_EN, 0);
			SendN16DataToDisplay(FACTORY_RST_CANCEL_En, 0);//touch for cancel
			SendN16DataToDisplay(FACTORY_RST_CNTDWN_EN, 0);//Enable Factory Values Display
		//	SendN16DataToDisplay(0x080984, 0);//Disable Cancel Keys
		}
	}
}

void LoadUserData(void)
{
	if(vp_rt_wr_str.LoginStatus==0)
	{
		SendSTRDataToDisplay(LOGEDIN_USER,(uint8_t*)"\0");   //string below icon
	}
	SendN16DataToDisplay(LIST_SELECT_WR,2);
	UIUserAccess.list_select=2;
	listall();
}

void UI_Load_Init()
{
	  UpdateUIRTC();
	  SendN16DataToDisplay(STRING_POPUP_EN, 0);//Disable Home screen Pop-Up
//	  SendN16DataToDisplay(IOT_FIRM_AVAIL_EN, Control_versions.Firmware_avb_icon);//FIRMware download available
//	  SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);//FIRMware downloading
	  SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR,0);
	  SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,0);
	  SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR,0);
	  SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
	  SendN16DataToDisplay(AWS_IOT_CNCTVTY,0);
	  SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD,0);
	  SendN16DataToDisplay(IOT_FIRM_POP_UP_EN, 0);//FIRM_pop_en
	  LoadCryoSettings(&UIModbusData);
	  LoadPhaseSettings(&UIModbusData);
	  LoadTempSettings(&UITempeSettingsData);
	  LoadAlarmSettings(&UIAlarmSettingsData);
	  LoadRTCSettings(&UIDateTimeSettings);
	  SendN16DataToDisplay(TIME_AM_PM_EN, (~UIDateTimeSettings.TimeFormat));//Time format toogle Key
	  SendN16DataToDisplay(DMY_FMT_EN, (~UIDateTimeSettings.DateFormat));//Date format toogle Key
	  LoadFreezerId(&UIFreezerStruct);
	  SendN16DataToDisplay(WIFI_CONNECT_WR, UIwifi.connect_sts);
	  SendN16DataToDisplay(IOT_CONNECT_WR, UIwifi.IOT_connect_sts);
	  SendN16DataToDisplay(WIFI_CONNECT_RD, UIwifi.connect_sts);
	  SendN16DataToDisplay(IOT_CONNECT_RD, UIwifi.IOT_connect_sts);
#ifdef EN_CTRS
	  ctrs_ws_para.With_out_ble =1;
	  ctrs_ws_para.with_ble =0;
	  SendN16DataToDisplay(0x89092,  ctrs_ws_para.With_out_ble);
	  SendN16DataToDisplay(0x89094,  ctrs_ws_para.with_ble);
#endif
	  if(vp_rt_wr_str.WiFiStatus==0)
	  {
		SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*)"\0");	//wifi name
	  }
	  if(vp_rt_wr_str.WiFiStatus != 1)
	  {
		  SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
	  }
	  if(vp_rt_wr_str.WiFiStatus != 2)
	  {
		  SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,0);
		  SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR,0);
		  if(vp_rt_wr_str.WiFiStatus == 0)
		  {
			  SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR,0);
		  }
	  }
	  if((vp_param_rw_str.plumbing_options & TEMP_SENS_SEL))
      {
		// SendN16DataToDisplay(0x080166, 0);
		 SendN16DataToDisplay(T2_ENABLE_CONTROL_WR, 0);
		// SendN16DataToDisplay(T2_EN_DIS_HOME, 0);
//		 SendN16DataToDisplay(0x080168, 1);
	  }
	  else
	  {
		 // SendN16DataToDisplay(0x080166, 1);
		  if(vp_param_rw_str.t2enable)
		  {
			  //SendN16DataToDisplay(T2_EN_DIS_HOME, 1);
		  }
		  else
		  {
			  //SendN16DataToDisplay(T2_EN_DIS_HOME, 0);
		  }
	  }
	  //System Info
	  ReadSTRDatafromDisplay(GUI_VERSION);	//GUI Version
	  SendN16DataToDisplay(PLUMBING_OPT, Plumbing_options.plumbing_options);//plumbing Option
	  SendN16DataToDisplay(PLUM_FREEZER_MODEL, Plumbing_options.Board_version-1);//Model
	  SendN16DataToDisplay(PLUM_OPT_TEMP_PROBES, Plumbing_options.Temp_sen);//Temperature Probes
	  SendN16DataToDisplay(PLUM_OPT_BATTERY, Plumbing_options.Battery_status);//Battery
	  SendN16DataToDisplay(PLUM_OPT_LEVEL_SENS, Plumbing_options.Level_sen);//Level Probes
	  SendSTRDataToDisplay(HARDWARE_SERIAL_NO,(uint8_t *)Plumbing_options.serial);//Serial No
	  SendSTRDataToDisplay(IOT_DEV_SL_NO,(uint8_t *)Plumbing_options.IOT_serial);//Serial No
	  SendSTRDataToDisplay(SOFTWARE_VERSION,(uint8_t *)verStr);//Soft Ver
	  SendSTRDataToDisplay(HARDWARE_VERSION,(uint8_t *)HW_REV_STR);//Hardware Rev
	  SendSTRDataToDisplay(SERVICE_CONTACT_WR,(uint8_t *)SystemStruct.Service_con);//Service Contact
	  SendSTRDataToDisplay(SUPLY_CONTACT_WR,(uint8_t *)SystemStruct.Supply_con);//Supplier Contact
	  LoadUserData();
}

void Factoryreset_data_UIupdate()
{
	  SendN16DataToDisplay(STRING_POPUP_EN, 0);//Disable Home screen Pop-Up
	  SendN16DataToDisplay(IOT_FIRM_POP_UP_EN, 0);//FIRM_pop_en
	  LoadCryoSettings(&UIModbusData);
	  LoadPhaseSettings(&UIModbusData);
	  LoadTempSettings(&UITempeSettingsData);
	  SendN16DataToDisplay(TIME_AM_PM_EN, (~UIDateTimeSettings.TimeFormat));//Time format toogle Key
	  SendN16DataToDisplay(DMY_FMT_EN, (~UIDateTimeSettings.DateFormat));//Date format toogle Key
	  SendN16DataToDisplay(WIFI_CONNECT_WR, UIwifi.connect_sts);
	  SendN16DataToDisplay(IOT_CONNECT_WR, UIwifi.IOT_connect_sts);
	  SendN16DataToDisplay(WIFI_CONNECT_RD, UIwifi.connect_sts);
	  SendN16DataToDisplay(IOT_CONNECT_RD, UIwifi.IOT_connect_sts);
}

uint8_t Data_buffer[SD_BUFFER_SETTINGS];
uint8_t Data_buffer1[Data_Group_buffer];
void Settings_write_data(void)
{
	uint16_t DataCtr = 0;
//	DataCtr = sprintf((char*)(Data_buffer+DataCtr),"HardwareDeviceSlNo : %s,Freezer model : %s,Freezer ID : %s,SW Version : %s,HW Version : V1.2,Display Version : %s\r\n",
//			Plumbing_options.serial,(Plumbing_options.Board_version==4) ? "CRF6000":((Plumbing_options.Board_version==6) ? "HC32":((Plumbing_options.Board_version==5) ? "HC14":((Plumbing_options.Board_version==2) ? "HCHE44/50":((Plumbing_options.Board_version==3) ? "HCHE92/107":"HCHE20")))),UIFreezerStruct.FreezerID,APP_FW_VER,Control_versions.UI_version);

	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"Temp Units:%s,Temp1 High:%s,Temp1 High value:%.1f,Temp1 Low:%s,Temp1 Low value:%.1f \r\n", \
					vp_param_rw_str.tempUnits ?"deg F":"deg C",vp_param_rw_str.high_temp1.is_enabled? "Enable":"Disable",vp_param_rw_str.high_temp1.value,vp_param_rw_str.low_temp1.is_enabled? "Enable":"Disable",vp_param_rw_str.low_temp1.value);

	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"Temp2:%s,Temp2 High:%s,Temp2 High value:%.1f,Temp2 Low:%s,Temp2 Low value:%.1f\r\n", \
					vp_param_rw_str.t2enable? "Enable":"Disable",vp_param_rw_str.high_temp2.is_enabled? "Enable":"Disable",vp_param_rw_str.high_temp2.value,vp_param_rw_str.low_temp2.is_enabled ? "Enable":"Disable",vp_param_rw_str.low_temp2.value);

	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"Remote Alarm Switch:%s,Remote Alarm Value:%u,Audible Repeat Delay:%s,Audible Repeat Delay Value:%u,LN2 Usage Alarm Switch:%s\r\n",\
					vp_param_rw_str.RemoteAlarmDelaySwitch ? "Enable":"Disable",vp_param_rw_str.rem_alm_dly, vp_param_rw_str.AudiableRepeatDelaySwitch ? "Enable":"Disable",	vp_param_rw_str.aud_rep_dly,vp_param_rw_str.LN2UsageAlarmDelaySwitch ? "Enable":"Disable");


	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"ScheduledFillRemainder:%s,Day Selection:%d,Time:%02d:%02d\r\n",\
					vp_param_rw_str.ScheduledFillRemainderEn ? "Enable":"Disable",vp_param_rw_str.Fill_DaySettings,vp_param_rw_str.Fill_HoursSettings,vp_param_rw_str.Fill_MinutesSettings);

//	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"CheckLevelRemainder:%s,Day Selection:%d,Time:%02d:%02d\r\n",
//					vp_param_rw_str.CheckLevelRemainderEn ? "Enable":"Disable",vp_param_rw_str.CLevel_DaySettings,vp_param_rw_str.CLevel_HoursSettings,vp_param_rw_str.CLevel_MinutesSettings);

	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"MaintRemainder:%s,Date:%02d:%02d:%02d,Time:%02d:%02d\r\n",\
					vp_param_rw_str.MaintRemainderEn ? "Enable":"Disable",vp_param_rw_str.Maint_MonthSettings,vp_param_rw_str.Maint_DateSettings,vp_param_rw_str.Maint_YearSettings,vp_param_rw_str.Maint_HoursSettings,vp_param_rw_str.Maint_MinutesSettings);

	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"ScreenSaverTime:%02d,Brightness:%02d\r\n",\
					vp_param_rw_str.STM_ScreenSaverTime,vp_param_rw_str.STM_Brightness);

	DataCtr += 	sprintf((char*)(Data_buffer+DataCtr),"Node ID:%u,BaudRate:%u",\
			vp_param_rw_str.mb_cfg.node_id,vp_param_rw_str.mb_cfg.mode);

}
