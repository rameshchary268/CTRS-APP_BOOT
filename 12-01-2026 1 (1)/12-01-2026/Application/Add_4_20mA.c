/*
 * Add_4_20mA.c
 *
 *  Created on: Jun 25, 2025
 *      Author: VenkateshArikanti
 */

#include "MyMain.h"
#include "DisplayCmdHandlers.h"
#include "UISettings.h"

extern RS485_INFO RS485_info;
extern PREV_INFO  Prev_info;
Network_CONFIG_Struct      Temp_ConfigSettings;
Network_4_20mAStruct UI_4_20mAsettings;
Network_CONFIG_Struct UI_4_20mAconfigSettings;
UpdateFIRMWAREStruct       UIUpdateIOTFIRMsettings;

void Save420mASettings(Network_4_20mAStruct *Network4_20mASettings)
{
	uint8_t   bfr[2];
	/*if(!just_for_display_flg)
	{
		if((vp_param_rw_str.Slave_TER_Switch != Network4_20mASettings->Slave_Ter_switch) || (vp_param_rw_str.Master_TER_Switch != Network4_20mASettings->Master_Ter_switch) || (vp_param_rw_str.EXD_MD_Switch_settings != Network4_20mASettings->EXD_MD_Switch))
		{
			ULOG_CONFIG("CS, SLAVE_TER_Switch:%s,Master_TER_Switch:%s,Extended_Modbus:%s",Network4_20mASettings->Slave_Ter_switch ? "Enable":"Disable",Network4_20mASettings->Master_Ter_switch ? "Enable":"Disable",Network4_20mASettings->EXD_MD_Switch ? "Enable":"Disable");
		}
		if((vp_param_rw_str.Chan1_Switch_settings != Network4_20mASettings->Chan1_Switch) || (vp_param_rw_str.Chan1_Selction_settings != Network4_20mASettings->Chan1_Selction) || (vp_param_rw_str.Chan2_Switch_settings != Network4_20mASettings->Chan2_Switch) ||
				(vp_param_rw_str.Chan2_Selction_settings != Network4_20mASettings->Chan2_Selction))
		{
			ULOG_CONFIG("CS, Channel_1:%s,Channel_2:%s,Channel_selection_1:%02d,Channel_selection_2:%02d",Network4_20mASettings->Chan1_Switch ? "Enable":"Disable",Network4_20mASettings->Chan2_Switch ? "Enable":"Disable",Network4_20mASettings->Chan1_Selction,Network4_20mASettings->Chan2_Selction);
		}
		if((vp_param_rw_str.Chan3_Switch_settings != Network4_20mASettings->Chan3_Switch) || (vp_param_rw_str.Chan3_Selction_settings != Network4_20mASettings->Chan3_Selction) || (vp_param_rw_str.Chan4_Switch_settings != Network4_20mASettings->Chan4_Switch) ||
				(vp_param_rw_str.Chan4_Selction_settings != Network4_20mASettings->Chan4_Selction))
		{
			ULOG_CONFIG("CS, Channel_3:%s,Channel_4:%s,Channel_selection_3:%02d,Channel_selection_4:%02d",Network4_20mASettings->Chan3_Switch ? "Enable":"Disable",Network4_20mASettings->Chan4_Switch ? "Enable":"Disable",Network4_20mASettings->Chan3_Selction,Network4_20mASettings->Chan4_Selction);
		}

		if((vp_param_rw_str.Source1_sel_settings != Network4_20mASettings->Ch1_Source_sel) || (vp_param_rw_str.Source2_sel_settings != Network4_20mASettings->Ch2_Source_sel) || (vp_param_rw_str.Chan1_vol_cur_settings != Network4_20mASettings->Chan1_vol_cur) ||
				(vp_param_rw_str.Chan2_vol_cur_settings	!= Network4_20mASettings->Chan2_vol_cur))
		{
			ULOG_CONFIG("CS, Source_selection1 :%s,Source_selection2 :%s,Channel_1_vol_or_cur :%s,Channel_2_vol_or_cur :%s",Network4_20mASettings->Ch1_Source_sel ? "Passive":"Active",Network4_20mASettings->Ch2_Source_sel ? "Passive":"Active",Network4_20mASettings->Chan1_vol_cur ? "Voltage":"Current",Network4_20mASettings->Chan2_vol_cur ? "Voltage":"Current");
		}
	}*/


	vp_param_rw_str.Slave_TER_Switch		    = Network4_20mASettings->Slave_Ter_switch;
	vp_param_rw_str.Master_TER_Switch		    = Network4_20mASettings->Master_Ter_switch;
	vp_param_rw_str.EXD_MD_Switch_settings      = Network4_20mASettings->EXD_MD_Switch;
    vp_param_rw_str.Chan1_Switch_settings		= Network4_20mASettings->Chan1_Switch;
	vp_param_rw_str.Chan2_Switch_settings		= Network4_20mASettings->Chan2_Switch;
    vp_param_rw_str.Chan1_Selction_settings		= Network4_20mASettings->Chan1_Selction;
    vp_param_rw_str.Chan2_Selction_settings		= Network4_20mASettings->Chan2_Selction;
	vp_param_rw_str.Source1_sel_settings		= Network4_20mASettings->Ch1_Source_sel;
	vp_param_rw_str.Chan1_vol_cur_settings		= Network4_20mASettings->Chan1_vol_cur;
	vp_param_rw_str.Chan2_vol_cur_settings		= Network4_20mASettings->Chan2_vol_cur;
	vp_param_rw_str.Chan3_Switch_settings       = Network4_20mASettings->Chan3_Switch;
	vp_param_rw_str.Chan4_Switch_settings       = Network4_20mASettings->Chan4_Switch;
    vp_param_rw_str.Chan3_Selction_settings     = Network4_20mASettings->Chan3_Selction;
    vp_param_rw_str.Chan4_Selction_settings     = Network4_20mASettings->Chan4_Selction;
	vp_param_rw_str.Source2_sel_settings        = Network4_20mASettings->Ch2_Source_sel;

	if(vp_param_rw_str.Source1_sel_settings)
	{
		if(vp_param_rw_str.Chan1_vol_cur_settings)
		{
			vp_param_rw_str.Chan1_vol_cur_settings=0;
			Network4_20mASettings->Chan1_vol_cur=0;
			SendN16DataToDisplay(CHA1_VOL_CUR_WR, vp_param_rw_str.Chan1_vol_cur_settings);
			SendN16DataToDisplay(CHA2_VOL_CUR_RD, vp_param_rw_str.Chan1_vol_cur_settings);
		}
		if(vp_param_rw_str.Chan2_vol_cur_settings)
		{
			vp_param_rw_str.Chan2_vol_cur_settings=0;
			Network4_20mASettings->Chan2_vol_cur=0;
			SendN16DataToDisplay(CHA2_VOL_CUR_WR, vp_param_rw_str.Chan2_vol_cur_settings);
			SendN16DataToDisplay(CHA2_VOL_CUR_RD, vp_param_rw_str.Chan2_vol_cur_settings);
		}
	}
	if(!vp_param_rw_str.EXD_MD_Switch_settings)
	{
		 Prev_info.prev_active_nodes=0;
		 SendN16DataToDisplay(No_devices,0);
	}
	Set_delay(SW_TMR_4_20mACOMM_POLL, 90*667, (uint32_t*)&RS485_info.mA_communication);
	EEPROM_Write(Network_4_20_mA, 0,(uint8_t*)&vp_param_rw_str.Slave_TER_Switch,30);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.Slave_TER_Switch,30);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(Network_4_20_mA_crc,0,bfr,2);
	if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update==0)
	{
		RS485_info.Poll_4_20=1;
	}
}

void Load420mASettings(Network_4_20mAStruct *Network4_20mASettings)
{
	Network4_20mASettings->Slave_Ter_switch  = vp_param_rw_str.Slave_TER_Switch;
	Network4_20mASettings->Master_Ter_switch =   vp_param_rw_str.Master_TER_Switch;
	Network4_20mASettings->EXD_MD_Switch     =	vp_param_rw_str.EXD_MD_Switch_settings;
	Network4_20mASettings->Chan1_Switch      =	vp_param_rw_str.Chan1_Switch_settings;
	Network4_20mASettings->Chan2_Switch	     =	vp_param_rw_str.Chan2_Switch_settings;
	Network4_20mASettings->Chan1_Selction	 =	vp_param_rw_str.Chan1_Selction_settings;
	Network4_20mASettings->Chan2_Selction	 =	vp_param_rw_str.Chan2_Selction_settings;
	Network4_20mASettings->Ch1_Source_sel	 =	vp_param_rw_str.Source1_sel_settings;
	Network4_20mASettings->Chan1_vol_cur	 =	vp_param_rw_str.Chan1_vol_cur_settings;
	Network4_20mASettings->Chan2_vol_cur	 =	vp_param_rw_str.Chan2_vol_cur_settings;
	Network4_20mASettings->Chan3_Switch      =	vp_param_rw_str.Chan3_Switch_settings;
	Network4_20mASettings->Chan4_Switch	     =	vp_param_rw_str.Chan4_Switch_settings;
	Network4_20mASettings->Chan3_Selction	 =	vp_param_rw_str.Chan3_Selction_settings;
	Network4_20mASettings->Chan4_Selction	 =	vp_param_rw_str.Chan4_Selction_settings;
	Network4_20mASettings->Ch2_Source_sel	 =	vp_param_rw_str.Source2_sel_settings;
	if(vp_param_rw_str.Source1_sel_settings)
	{
		if(vp_param_rw_str.Chan1_vol_cur_settings)
		{
			vp_param_rw_str.Chan1_vol_cur_settings=0;
			Network4_20mASettings->Chan1_vol_cur=0;
		}
		if(vp_param_rw_str.Chan2_vol_cur_settings)
		{
			vp_param_rw_str.Chan2_vol_cur_settings=0;
			Network4_20mASettings->Chan2_vol_cur=0;
		}
	}
	SendVPDataCmdToDisplay(DISP_4_20MA_CMD);
	SendVPDataCmdToDisplay(DISP_4_20MA_CMD1);
}

void Save420mAconfigSettings(Network_CONFIG_Struct *Network4_20mA_configSettings)
{
	uint8_t   bfr[2];
	if(vp_param_rw_str.Chan1_Selction_settings==0)
	{
		vp_param_rw_str.DP_MIN_CONFIG_CH1_settings		  =	Network4_20mA_configSettings->MIN_CONFIG_CH1;
		vp_param_rw_str.DP_MAX_CONFIG_CH1_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH1;
	}
	else if(vp_param_rw_str.Chan1_Selction_settings==1)
	{
		vp_param_rw_str.T1_MIN_CONFIG_CH1_settings        =	Network4_20mA_configSettings->MIN_CONFIG_CH1;
		vp_param_rw_str.T1_MAX_CONFIG_CH1_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH1;
	}
	else if(vp_param_rw_str.Chan1_Selction_settings==2)
	{
		vp_param_rw_str.T2_MIN_CONFIG_CH1_settings		  = Network4_20mA_configSettings->MIN_CONFIG_CH1;
		vp_param_rw_str.T2_MAX_CONFIG_CH1_settings		  = Network4_20mA_configSettings->MAX_CONFIG_CH1;
	}

	if(vp_param_rw_str.Chan2_Selction_settings==0)
	{
		vp_param_rw_str.DP_MIN_CONFIG_CH2_settings		  =	Network4_20mA_configSettings->MIN_CONFIG_CH2;
		vp_param_rw_str.DP_MAX_CONFIG_CH2_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH2;
	}
	else if(vp_param_rw_str.Chan2_Selction_settings==1)
	{
		vp_param_rw_str.T1_MIN_CONFIG_CH2_settings		  =	Network4_20mA_configSettings->MIN_CONFIG_CH2;
		vp_param_rw_str.T1_MAX_CONFIG_CH2_settings 		  =	Network4_20mA_configSettings->MAX_CONFIG_CH2;
	}
	else if(vp_param_rw_str.Chan2_Selction_settings==2)
	{
		vp_param_rw_str.T2_MIN_CONFIG_CH2_settings	  	  =	Network4_20mA_configSettings->MIN_CONFIG_CH2;
		vp_param_rw_str.T2_MAX_CONFIG_CH2_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH2;
	}

	if(vp_param_rw_str.Chan3_Selction_settings==0)
	{
		vp_param_rw_str.DP_MIN_CONFIG_CH3_settings		  =	Network4_20mA_configSettings->MIN_CONFIG_CH3;
		vp_param_rw_str.DP_MAX_CONFIG_CH3_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH3;
	}
	else if(vp_param_rw_str.Chan3_Selction_settings==1)
	{
		vp_param_rw_str.T1_MIN_CONFIG_CH3_settings        =	Network4_20mA_configSettings->MIN_CONFIG_CH3;
		vp_param_rw_str.T1_MAX_CONFIG_CH3_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH3;
	}
	else if(vp_param_rw_str.Chan3_Selction_settings==2)
	{
		vp_param_rw_str.T2_MIN_CONFIG_CH3_settings		  = Network4_20mA_configSettings->MIN_CONFIG_CH3;
		vp_param_rw_str.T2_MAX_CONFIG_CH3_settings		  = Network4_20mA_configSettings->MAX_CONFIG_CH3;
	}

	if(vp_param_rw_str.Chan4_Selction_settings==0)
	{
		vp_param_rw_str.DP_MIN_CONFIG_CH4_settings		  =	Network4_20mA_configSettings->MIN_CONFIG_CH4;
		vp_param_rw_str.DP_MAX_CONFIG_CH4_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH4;
	}
	else if(vp_param_rw_str.Chan4_Selction_settings==1)
	{
		vp_param_rw_str.T1_MIN_CONFIG_CH4_settings		  =	Network4_20mA_configSettings->MIN_CONFIG_CH4;
		vp_param_rw_str.T1_MAX_CONFIG_CH4_settings 		  =	Network4_20mA_configSettings->MAX_CONFIG_CH4;
	}
	else if(vp_param_rw_str.Chan4_Selction_settings==2)
	{
		vp_param_rw_str.T2_MIN_CONFIG_CH4_settings	  	  =	Network4_20mA_configSettings->MIN_CONFIG_CH4;
		vp_param_rw_str.T2_MAX_CONFIG_CH4_settings		  =	Network4_20mA_configSettings->MAX_CONFIG_CH4;
	}
	/*if(!just_for_display_flg)
	{
		if((Temp_ConfigSettings.MIN_CONFIG_CH1 != Network4_20mA_configSettings->MIN_CONFIG_CH1) || (Temp_ConfigSettings.MAX_CONFIG_CH1 != Network4_20mA_configSettings->MAX_CONFIG_CH1) || (Temp_ConfigSettings.MIN_CONFIG_CH2 != Network4_20mA_configSettings->MIN_CONFIG_CH2) || (Temp_ConfigSettings.MAX_CONFIG_CH2 != Network4_20mA_configSettings->MAX_CONFIG_CH2))
		{
			ULOG_CONFIG("CS, CH1_MIN:%.1f,CH1_MAX:%.1f,CH2_MIN:%.1f,CH2_MAX:%.1f",Network4_20mA_configSettings->MIN_CONFIG_CH1/10,Network4_20mA_configSettings->MAX_CONFIG_CH1/10,Network4_20mA_configSettings->MIN_CONFIG_CH2/10,Network4_20mA_configSettings->MAX_CONFIG_CH2/10);
		}
		if((Temp_ConfigSettings.MIN_CONFIG_CH3 != Network4_20mA_configSettings->MIN_CONFIG_CH3) || (Temp_ConfigSettings.MAX_CONFIG_CH3 != Network4_20mA_configSettings->MAX_CONFIG_CH3) || (Temp_ConfigSettings.MIN_CONFIG_CH4 != Network4_20mA_configSettings->MIN_CONFIG_CH4) || (Temp_ConfigSettings.MAX_CONFIG_CH4 != Network4_20mA_configSettings->MAX_CONFIG_CH4))
		{
			ULOG_CONFIG("CS, CH3_MIN:%.1f,CH3_MAX:%.1f,CH4_MIN:%.1f,CH4_MAX:%.1f",Network4_20mA_configSettings->MIN_CONFIG_CH3/10,Network4_20mA_configSettings->MAX_CONFIG_CH3/10,Network4_20mA_configSettings->MIN_CONFIG_CH4/10,Network4_20mA_configSettings->MAX_CONFIG_CH4/10);
		}
	}*/

	Temp_ConfigSettings.MIN_CONFIG_CH1 = Network4_20mA_configSettings->MIN_CONFIG_CH1;
	Temp_ConfigSettings.MAX_CONFIG_CH1 = Network4_20mA_configSettings->MAX_CONFIG_CH1;
	Temp_ConfigSettings.MIN_CONFIG_CH2 = Network4_20mA_configSettings->MIN_CONFIG_CH2;
	Temp_ConfigSettings.MAX_CONFIG_CH2 = Network4_20mA_configSettings->MAX_CONFIG_CH2;
	Temp_ConfigSettings.MIN_CONFIG_CH3 = Network4_20mA_configSettings->MIN_CONFIG_CH3;
	Temp_ConfigSettings.MAX_CONFIG_CH3 = Network4_20mA_configSettings->MAX_CONFIG_CH3;
	Temp_ConfigSettings.MIN_CONFIG_CH4 = Network4_20mA_configSettings->MIN_CONFIG_CH4;
	Temp_ConfigSettings.MAX_CONFIG_CH4 = Network4_20mA_configSettings->MAX_CONFIG_CH4;

//	Network4_20mA_configSettings->MIN_CONFIG_CH1 = Temp_ConfigSettings.MIN_CONFIG_CH1;
//	Network4_20mA_configSettings->MAX_CONFIG_CH1 = Temp_ConfigSettings.MAX_CONFIG_CH1;
//	Network4_20mA_configSettings->MIN_CONFIG_CH2 = Temp_ConfigSettings.MIN_CONFIG_CH2;
//	Network4_20mA_configSettings->MAX_CONFIG_CH2 = Temp_ConfigSettings.MAX_CONFIG_CH2;
//	Network4_20mA_configSettings->MIN_CONFIG_CH3 = Temp_ConfigSettings.MIN_CONFIG_CH3;
//	Network4_20mA_configSettings->MAX_CONFIG_CH3 = Temp_ConfigSettings.MAX_CONFIG_CH3;
//	Network4_20mA_configSettings->MIN_CONFIG_CH4 = Temp_ConfigSettings.MIN_CONFIG_CH4;
//	Network4_20mA_configSettings->MAX_CONFIG_CH4 = Temp_ConfigSettings.MAX_CONFIG_CH4;


	EEPROM_Write(Network_4_20_mA_config, 0,(uint8_t*)&vp_param_rw_str.T1_MIN_CONFIG_CH1_settings,48);
	crc16 = usMBCRC16((uint8_t*)&vp_param_rw_str.T1_MIN_CONFIG_CH1_settings,48);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(Network_4_20_mA_config_crc,0,bfr,2);
	if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update==0)
	{
		RS485_info.Poll_4_20=1;
	}
}

void Load420mAconfigSettings(Network_CONFIG_Struct *Network4_20mA_configSettings)
{
	if(vp_param_rw_str.Chan1_Selction_settings==0)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH1  = vp_param_rw_str.DP_MIN_CONFIG_CH1_settings;
	    Network4_20mA_configSettings->MAX_CONFIG_CH1  = vp_param_rw_str.DP_MAX_CONFIG_CH1_settings;
	}
	else if(vp_param_rw_str.Chan1_Selction_settings==1)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH1  = vp_param_rw_str.T1_MIN_CONFIG_CH1_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH1  = vp_param_rw_str.T1_MAX_CONFIG_CH1_settings;
	}
	else if(vp_param_rw_str.Chan1_Selction_settings==2)
	{
	    Network4_20mA_configSettings->MIN_CONFIG_CH1  = vp_param_rw_str.T2_MIN_CONFIG_CH1_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH1  = vp_param_rw_str.T2_MAX_CONFIG_CH1_settings;
	}

	if(vp_param_rw_str.Chan2_Selction_settings==0)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH2  = vp_param_rw_str.DP_MIN_CONFIG_CH2_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH2  = vp_param_rw_str.DP_MAX_CONFIG_CH2_settings;
	}
	else if(vp_param_rw_str.Chan2_Selction_settings==1)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH2  = vp_param_rw_str.T1_MIN_CONFIG_CH2_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH2  = vp_param_rw_str.T1_MAX_CONFIG_CH2_settings;
	}
	else if(vp_param_rw_str.Chan2_Selction_settings==2)
	{
	    Network4_20mA_configSettings->MIN_CONFIG_CH2  = vp_param_rw_str.T2_MIN_CONFIG_CH2_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH2  = vp_param_rw_str.T2_MAX_CONFIG_CH2_settings;
	}

	if(vp_param_rw_str.Chan3_Selction_settings==0)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH3  = vp_param_rw_str.DP_MIN_CONFIG_CH3_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH3  = vp_param_rw_str.DP_MAX_CONFIG_CH3_settings;
	}
	else if(vp_param_rw_str.Chan3_Selction_settings==1)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH3  = vp_param_rw_str.T1_MIN_CONFIG_CH3_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH3  = vp_param_rw_str.T1_MAX_CONFIG_CH3_settings;
	}
	else if(vp_param_rw_str.Chan3_Selction_settings==2)
	{
	    Network4_20mA_configSettings->MIN_CONFIG_CH3  = vp_param_rw_str.T2_MIN_CONFIG_CH3_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH3  = vp_param_rw_str.T2_MAX_CONFIG_CH3_settings;
	}

	if(vp_param_rw_str.Chan4_Selction_settings==0)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH4  = vp_param_rw_str.DP_MIN_CONFIG_CH4_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH4  = vp_param_rw_str.DP_MAX_CONFIG_CH4_settings;
	}
	else if(vp_param_rw_str.Chan4_Selction_settings==1)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH4  = vp_param_rw_str.T1_MIN_CONFIG_CH4_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH4  = vp_param_rw_str.T1_MAX_CONFIG_CH4_settings;
	}
	else if(vp_param_rw_str.Chan4_Selction_settings==2)
	{
		Network4_20mA_configSettings->MIN_CONFIG_CH4  = vp_param_rw_str.T2_MIN_CONFIG_CH4_settings;
		Network4_20mA_configSettings->MAX_CONFIG_CH4  = vp_param_rw_str.T2_MAX_CONFIG_CH4_settings;
	}

	Temp_ConfigSettings.MIN_CONFIG_CH1 = Network4_20mA_configSettings->MIN_CONFIG_CH1;
	Temp_ConfigSettings.MAX_CONFIG_CH1 = Network4_20mA_configSettings->MAX_CONFIG_CH1;
	Temp_ConfigSettings.MIN_CONFIG_CH2 = Network4_20mA_configSettings->MIN_CONFIG_CH2;
	Temp_ConfigSettings.MAX_CONFIG_CH2 = Network4_20mA_configSettings->MAX_CONFIG_CH2;
	Temp_ConfigSettings.MIN_CONFIG_CH3 = Network4_20mA_configSettings->MIN_CONFIG_CH3;
	Temp_ConfigSettings.MAX_CONFIG_CH3 = Network4_20mA_configSettings->MAX_CONFIG_CH3;
	Temp_ConfigSettings.MIN_CONFIG_CH4 = Network4_20mA_configSettings->MIN_CONFIG_CH4;
	Temp_ConfigSettings.MAX_CONFIG_CH4 = Network4_20mA_configSettings->MAX_CONFIG_CH4;

	if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update)
	{
		RS485_info.Firmware_4_20=1;
		//RegisterAlarm(POLL_4_20mA, &RS485_info.Poll_4_20, 150, SYNC_ON_REGISTER);
		RS485_init();
	}
	SendN16DataToDisplay(FIRM_4_20_mA_ICON_WR, UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update);
	SendN16DataToDisplay(FIRM_4_20_mA_ICON_RD, UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update);
	SendVPDataCmdToDisplay(DISP_4_20MA_CONFIG_CMD);

}
void Handle4_20mAFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
			break;
			case MUTE_KEY:
				mutebuzzer();
				break;
//			case MANUAL_DEFOG_KEY://firmware
//
//				break;
			case ENTER_KEY:
				Save420mASettings(&UI_4_20mAsettings);
				Save420mAconfigSettings(&UI_4_20mAconfigSettings);
				break;

			case CANCEL_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
				break;
			case BACK_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
				break;
			case SCHEDULED_CLEVEL_KEY://4_20_mA_config
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);

				break;
		}
	}
}


void Handle4_20mATERFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				Load420mASettings(&UI_4_20mAsettings);
			break;
			case MUTE_KEY:
				mutebuzzer();
				break;
//			case MANUAL_DEFOG_KEY://firmware
//				break;
			case ENTER_KEY:
				Save420mASettings(&UI_4_20mAsettings);
				break;

			case CANCEL_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				break;
			case BACK_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				break;
			case SCHEDULED_CLEVEL_KEY://4_20_mA_config
				Load420mASettings(&UI_4_20mAsettings);
				break;
		}
	}
}
void Handle4_20mA_3wireFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
			break;
			case MUTE_KEY:
				mutebuzzer();
				break;
//			case MANUAL_DEFOG_KEY://firmware
//
//				break;
			case ENTER_KEY:
				Save420mASettings(&UI_4_20mAsettings);
				Save420mAconfigSettings(&UI_4_20mAconfigSettings);
				break;

			case CANCEL_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
				break;
			case BACK_KEY:
				Load420mASettings(&UI_4_20mAsettings);
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
				break;
			case SCHEDULED_CLEVEL_KEY://4_20_mA_config
				Load420mAconfigSettings(&UI_4_20mAconfigSettings);
				Load420mASettings(&UI_4_20mAsettings);
				break;
		}
	}
}

void Handle4_20mA_firmwareFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY://YES
			if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update)
			{
				RS485_info.Firmware_4_20=1;
				RS485_init();
				RS485_info.File_fail=0;
				//RegisterAlarm(POLL_4_20mA, &RS485_info.Poll_4_20, 150, SYNC_ON_REGISTER); //need to implement
			}
			else
			{
				RS485_info.Firmware_4_20=0;
				RS485_init();
				RS485_info.data_request=0;
//				RegisterAlarm(POLL_4_20mA, &RS485_info.Poll_4_20, 10, SYNC_ON_REGISTER);
			}
			EEPROM_Write(FIRM_4_20_DOW, 0,(uint8_t*)&UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update,1);
			break;
			case MUTE_KEY://NO
			if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update)
			{
				UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update=0;
				SendN16DataToDisplay(FIRM_4_20_mA_ICON_WR, UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update);
				SendN16DataToDisplay(FIRM_4_20_mA_ICON_RD, UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update);
			}
			else
			{
				UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update=1;
				SendN16DataToDisplay(FIRM_4_20_mA_ICON_WR, UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update);
				SendN16DataToDisplay(FIRM_4_20_mA_ICON_RD, UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update);
			}
			EEPROM_Write(FIRM_4_20_DOW, 0,(uint8_t*)&UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update,1);
			break;
		}
	}
}

void PassiveLimitCheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(vp_param_rw_str.Source1_sel_settings)
	{
		if((VPAddr == CHA1_VOL_CUR_RD))
		{
			if(UI_4_20mAsettings.Chan1_vol_cur)
			{
				UI_4_20mAsettings.Chan1_vol_cur=0;
				SendN16DataToDisplay(CHA1_VOL_CUR_WR, UI_4_20mAsettings.Chan1_vol_cur);
				SendN16DataToDisplay(CHA2_VOL_CUR_RD, UI_4_20mAsettings.Chan1_vol_cur);
			}

		}
		else if((VPAddr == CHA2_VOL_CUR_RD))
		{
			if(UI_4_20mAsettings.Chan2_vol_cur)
			{
				UI_4_20mAsettings.Chan2_vol_cur=0;
				SendN16DataToDisplay(CHA2_VOL_CUR_WR, UI_4_20mAsettings.Chan2_vol_cur);
				SendN16DataToDisplay(CHA2_VOL_CUR_RD, UI_4_20mAsettings.Chan2_vol_cur);
			}
		}
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Only current!!!");
	}
}


void Config4_20mALimitCheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t LimitValue = *(DataInPtr) ;
	if((VPAddr == MAX_CONIG_CH1_RD)||(VPAddr == MAX_CONIG_CH2_RD)||(VPAddr == MAX_CONIG_CH3_RD)||(VPAddr == MAX_CONIG_CH4_RD))
	{
		if(VPAddr == MAX_CONIG_CH1_RD)
		{
			if(UI_4_20mAsettings.Chan1_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue <= UI_4_20mAconfigSettings.MIN_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.DP_MAX_CONFIG_CH1_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.DP_MAX_CONFIG_CH1_settings;
				}
			}
			else if((LimitValue <= 000 && LimitValue >= -1900))
			{
				if(UI_4_20mAsettings.Chan1_Selction==1)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.T1_MAX_CONFIG_CH1_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				else if(UI_4_20mAsettings.Chan1_Selction==2)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.T2_MAX_CONFIG_CH1_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan1_Selction==1)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.T1_MAX_CONFIG_CH1_settings;

				}
				else if(UI_4_20mAsettings.Chan1_Selction==2)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.T2_MAX_CONFIG_CH1_settings;
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500,UI_4_20mAconfigSettings.MAX_CONFIG_CH1);
		}
		else if(VPAddr == MAX_CONIG_CH2_RD)
		{
			if(UI_4_20mAsettings.Chan2_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue <= UI_4_20mAconfigSettings.MIN_CONFIG_CH2)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.DP_MAX_CONFIG_CH2_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.DP_MAX_CONFIG_CH2_settings;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
				}
			}
			else if((LimitValue <= 000 && LimitValue >= -1900))
			{
				if(UI_4_20mAsettings.Chan2_Selction==1)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH2)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.T1_MAX_CONFIG_CH2_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				else if(UI_4_20mAsettings.Chan2_Selction==2)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH2)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.T2_MAX_CONFIG_CH2_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan2_Selction==1)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.T1_MAX_CONFIG_CH2_settings;
				}
				else if(UI_4_20mAsettings.Chan2_Selction==2)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.T2_MAX_CONFIG_CH2_settings;
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500, UI_4_20mAconfigSettings.MAX_CONFIG_CH2);
		}
		else if(VPAddr == MAX_CONIG_CH3_RD)
		{
			if(UI_4_20mAsettings.Chan3_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue <= UI_4_20mAconfigSettings.MIN_CONFIG_CH3)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.DP_MAX_CONFIG_CH3_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.DP_MAX_CONFIG_CH3_settings;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
				}
			}
			else if((LimitValue <= 000 && LimitValue >= -1900))
			{
				if(UI_4_20mAsettings.Chan3_Selction==1)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH3)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.T1_MAX_CONFIG_CH3_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}

				}
				else if(UI_4_20mAsettings.Chan3_Selction==2)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH3)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.T2_MAX_CONFIG_CH3_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan3_Selction==1)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.T1_MAX_CONFIG_CH3_settings;
				}
				else if(UI_4_20mAsettings.Chan3_Selction==2)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.T2_MAX_CONFIG_CH3_settings;
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500, UI_4_20mAconfigSettings.MAX_CONFIG_CH3);
		}
		else if(VPAddr == MAX_CONIG_CH4_RD)
		{
			if(UI_4_20mAsettings.Chan4_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue <= UI_4_20mAconfigSettings.MIN_CONFIG_CH4)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.DP_MAX_CONFIG_CH4_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH4= vp_param_rw_str.DP_MAX_CONFIG_CH4_settings;
				}
			}
			else if((LimitValue <= 000 && LimitValue >= -1900))
			{
				if(UI_4_20mAsettings.Chan4_Selction==1)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH4)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.T1_MAX_CONFIG_CH4_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
				if(UI_4_20mAsettings.Chan4_Selction==2)
				{
					if(LimitValue<=UI_4_20mAconfigSettings.MIN_CONFIG_CH4)
					{
						UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.T2_MAX_CONFIG_CH4_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter higher than Low Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan4_Selction==1)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.T1_MAX_CONFIG_CH4_settings;
				}
				else if(UI_4_20mAsettings.Chan4_Selction==2)
				{
					UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.T1_MAX_CONFIG_CH4_settings;

				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500,UI_4_20mAconfigSettings.MAX_CONFIG_CH4);
		}

	}
	else if((VPAddr == MIN_CONIG_CH1_RD)||(VPAddr == MIN_CONIG_CH2_RD) || (VPAddr == MIN_CONIG_CH3_RD)||(VPAddr == MIN_CONIG_CH4_RD))
	{
		if(VPAddr == MIN_CONIG_CH1_RD)
		{
			if(UI_4_20mAsettings.Chan1_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH1 = vp_param_rw_str.DP_MIN_CONFIG_CH1_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH1= vp_param_rw_str.DP_MIN_CONFIG_CH1_settings;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
				}
			}
			else if((LimitValue <= -1500 && LimitValue >= -2200))
			{
				if(UI_4_20mAsettings.Chan1_Selction==1)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH1 = vp_param_rw_str.T1_MIN_CONFIG_CH1_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else if(UI_4_20mAsettings.Chan1_Selction==2)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH1 = vp_param_rw_str.T2_MIN_CONFIG_CH1_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan1_Selction==1)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH1 = vp_param_rw_str.T1_MIN_CONFIG_CH1_settings;
				}
				else if(UI_4_20mAsettings.Chan1_Selction==2)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH1= vp_param_rw_str.T2_MIN_CONFIG_CH1_settings;
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500,UI_4_20mAconfigSettings.MIN_CONFIG_CH1);
		}
		else if(VPAddr == MIN_CONIG_CH2_RD)
		{
			if(UI_4_20mAsettings.Chan2_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH2)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.DP_MIN_CONFIG_CH2_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.DP_MIN_CONFIG_CH2_settings;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
				}
			}
			else if((LimitValue <= -1500 && LimitValue >= -2200))
			{
				if(UI_4_20mAsettings.Chan2_Selction==1)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH1)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.T1_MIN_CONFIG_CH2_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else if(UI_4_20mAsettings.Chan2_Selction==2)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH2)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.T2_MIN_CONFIG_CH2_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan2_Selction==1)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.T1_MIN_CONFIG_CH2_settings;

				}
				else if(UI_4_20mAsettings.Chan2_Selction==2)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.T2_MIN_CONFIG_CH2_settings;
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500,UI_4_20mAconfigSettings.MIN_CONFIG_CH2);
		}
		else if(VPAddr == MIN_CONIG_CH3_RD)
		{
			if(UI_4_20mAsettings.Chan3_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH3)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.DP_MIN_CONFIG_CH3_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.DP_MIN_CONFIG_CH3_settings;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
				}
			}
			else if((LimitValue <= -1500 && LimitValue >= -2200))
			{
				if(UI_4_20mAsettings.Chan3_Selction==1)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH3)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.T1_MIN_CONFIG_CH3_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else if(UI_4_20mAsettings.Chan3_Selction==2)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH3)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.T2_MIN_CONFIG_CH3_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan3_Selction==1)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.T1_MIN_CONFIG_CH3_settings;

				}
				if(UI_4_20mAsettings.Chan3_Selction==2)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.T2_MIN_CONFIG_CH3_settings;
				}
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500,UI_4_20mAconfigSettings.MIN_CONFIG_CH3);
		}
		else if(VPAddr == MIN_CONIG_CH4_RD)
		{
			if(UI_4_20mAsettings.Chan4_Selction==0)
			{
				if((LimitValue >= 000 && LimitValue <= 340))
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH4)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.DP_MIN_CONFIG_CH4_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.DP_MIN_CONFIG_CH4_settings;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
				}
			}
			else if((LimitValue <= -1500 && LimitValue >= -2200))
			{
				if(UI_4_20mAsettings.Chan4_Selction==1)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH4)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.T1_MIN_CONFIG_CH4_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
				else if(UI_4_20mAsettings.Chan4_Selction==2)
				{
					if(LimitValue >= UI_4_20mAconfigSettings.MAX_CONFIG_CH4)
					{
						UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.T2_MIN_CONFIG_CH4_settings;
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter Lower than High Limit!!!");
					}
				}
			}
			else
			{
				if(UI_4_20mAsettings.Chan4_Selction==1)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.T1_MIN_CONFIG_CH4_settings;
				}
				else if(UI_4_20mAsettings.Chan4_Selction==2)
				{
					UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.T2_MIN_CONFIG_CH4_settings;
				}

				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			SendN16DataToDisplay(VPAddr-0x500,UI_4_20mAconfigSettings.MIN_CONFIG_CH4);
		}
	}
}


void SEL4_20mACheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	//int16_t LimitValue = *(DataInPtr) ;
	if((VPAddr == CHA1_SEL_RD)||(VPAddr == CHA2_SEL_RD)||(VPAddr == CHA3_SEL_RD)||(VPAddr == CHA4_SEL_RD))
	{
		if(VPAddr == CHA1_SEL_RD)
		{
			if(UI_4_20mAsettings.Chan1_Selction==0)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.DP_MAX_CONFIG_CH1_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH1= vp_param_rw_str.DP_MIN_CONFIG_CH1_settings;
			}
			else if(UI_4_20mAsettings.Chan1_Selction==1)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.T1_MAX_CONFIG_CH1_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH1= vp_param_rw_str.T1_MIN_CONFIG_CH1_settings;
			}
			else if(UI_4_20mAsettings.Chan1_Selction==2)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH1= vp_param_rw_str.T2_MAX_CONFIG_CH1_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH1= vp_param_rw_str.T2_MIN_CONFIG_CH1_settings;
			}
			SendN16DataToDisplay(MAX_CONIG_CH1_WR,UI_4_20mAconfigSettings.MAX_CONFIG_CH1);
			SendN16DataToDisplay(MIN_CONIG_CH1_WR,UI_4_20mAconfigSettings.MIN_CONFIG_CH1);

		}
		else if(VPAddr == CHA2_SEL_RD)
		{
			if(UI_4_20mAsettings.Chan2_Selction==0)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH2= vp_param_rw_str.DP_MAX_CONFIG_CH2_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH2= vp_param_rw_str.DP_MIN_CONFIG_CH2_settings;
			}
			else if(UI_4_20mAsettings.Chan2_Selction==1)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.T1_MAX_CONFIG_CH2_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.T1_MIN_CONFIG_CH2_settings;
			}
			else if(UI_4_20mAsettings.Chan2_Selction==2)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH2 = vp_param_rw_str.T2_MAX_CONFIG_CH2_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH2 = vp_param_rw_str.T2_MIN_CONFIG_CH2_settings;
			}
			SendN16DataToDisplay(MAX_CONIG_CH2_WR,UI_4_20mAconfigSettings.MAX_CONFIG_CH2);
			SendN16DataToDisplay(MIN_CONIG_CH2_WR,UI_4_20mAconfigSettings.MIN_CONFIG_CH2);

		}
		else if(VPAddr == CHA3_SEL_RD)
		{
			if(UI_4_20mAsettings.Chan3_Selction==0)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH3= vp_param_rw_str.DP_MAX_CONFIG_CH3_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH3= vp_param_rw_str.DP_MIN_CONFIG_CH3_settings;
			}
			else if(UI_4_20mAsettings.Chan3_Selction==1)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.T1_MAX_CONFIG_CH3_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.T1_MIN_CONFIG_CH3_settings;
			}
			else if(UI_4_20mAsettings.Chan3_Selction==2)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH3 = vp_param_rw_str.T2_MAX_CONFIG_CH3_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH3 = vp_param_rw_str.T2_MIN_CONFIG_CH3_settings;
			}
			SendN16DataToDisplay(MAX_CONIG_CH3_WR,UI_4_20mAconfigSettings.MAX_CONFIG_CH3);
			SendN16DataToDisplay(MIN_CONIG_CH3_WR,UI_4_20mAconfigSettings.MIN_CONFIG_CH3);

		}
		else if(VPAddr == CHA4_SEL_RD)
		{
			if(UI_4_20mAsettings.Chan4_Selction==0)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH4= vp_param_rw_str.DP_MAX_CONFIG_CH4_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH4= vp_param_rw_str.DP_MIN_CONFIG_CH4_settings;
			}
			else if(UI_4_20mAsettings.Chan4_Selction==1)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.T1_MAX_CONFIG_CH4_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.T1_MIN_CONFIG_CH4_settings;
			}
			else if(UI_4_20mAsettings.Chan4_Selction==2)
			{
				UI_4_20mAconfigSettings.MAX_CONFIG_CH4 = vp_param_rw_str.T2_MAX_CONFIG_CH4_settings;
				UI_4_20mAconfigSettings.MIN_CONFIG_CH4 = vp_param_rw_str.T2_MIN_CONFIG_CH4_settings;
			}
			SendN16DataToDisplay(MAX_CONIG_CH4_WR,UI_4_20mAconfigSettings.MAX_CONFIG_CH4);
			SendN16DataToDisplay(MIN_CONIG_CH4_WR,UI_4_20mAconfigSettings.MIN_CONFIG_CH4);
		}
	}
}


void Firm_4_20_mA_Update_process(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update)
	{
		SendN16DataToDisplay(IOT_FIRM_SEL_EN, 0);
	}
	else
	{
		SendN16DataToDisplay(IOT_FIRM_SEL_EN, 2);
	}
	SendN16DataToDisplay(FIRMWARE_420_Virtual_Key, 1);
}
