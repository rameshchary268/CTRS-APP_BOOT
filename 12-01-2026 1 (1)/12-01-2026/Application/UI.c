/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: UI.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains definitions for UI communication process over UART1
 * as the TFT module used is TOPWAY HKT043ATA-5C
 *
 * Note:
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include <math.h>
#include <stdio.h>

#include "MyMain.h"
#include "DisplayCommDriver.h"
#include "DisplayCmdHandlers.h"
#include "DispUIkeyhandler.h"
#include "UISettings.h"
#include "UserAccess.h"

/* Declaration of external global variables */

uint8_t Init_Wait_Delay_for_UI_flg;
uint8_t iot_popup_timer=0;
uint8_t FirmwareUpdateModeOn;
uint8_t usb_flag;
uint8_t clear_iot_popup=1;

/* Declaration of global variables */
UI_INFO UI_info;

uint8_t RTC_Year,RTC_Month,RTC_Date,RTC_Hour,RTC_Min,RTC_Sec,UI_Hours_Check,time_sec;




///////////////////////////////
// COMMANDS FOR THE TFT MODULE
///////////////////////////////
// Required for configuration
const uint8_t TFT_HANDSHAKE_CMD[]       = {HEADER, 0x30, FOOTER1, FOOTER2, FOOTER3, FOOTER4};   // Handshake with the TFT module
const uint8_t TC_SEND_TD_PAGE_KEY_ID[]  = {HEADER, 0x79, FOOTER1, FOOTER2, FOOTER3, FOOTER4};   // Touch-down - send page and key IDs
const uint8_t TFT_DISP_PAGE[]           = {HEADER, 0x70, DISP_PAGE_IDH, DISP_PAGE_IDL, FOOTER1, FOOTER2, FOOTER3, FOOTER4}; // Display a pre-stored TML file
const uint8_t TFT_EN_IF_PROC_FN[]       = {HEADER, 0xE0, 0x55, 0xAA, 0x5A, 0xA5, BAUD_SET, TP_PARAM, 0x00, FOOTER1, FOOTER2, FOOTER3, FOOTER4}; // Config
const uint8_t TFT_SCREEN_SAVER[]        = {HEADER, 0x5E, TFT_BL_TIMEOUT_MSB, TFT_BL_TIMEOUT_LSB, TFT_BL_MIN_VAL, FOOTER1, FOOTER2, FOOTER3, FOOTER4};
//const uint8_t TFT_RTC_SET_DATE[]        = {HEADER, 0x9c, RTC_Year, RTC_Month, RTC_Hour,RTC_Min,RTC_Sec, FOOTER1, FOOTER2, FOOTER3, FOOTER4};
// Run-time commands
const uint8_t TFT_BACKLIGHT_MAX[]       = {HEADER, 0x5F, 0x3F, FOOTER1, FOOTER2, FOOTER3, FOOTER4};
const uint8_t TFT_BACKLIGHT_NOM[]       = {HEADER, 0x5F, TFT_BL_NOM_VAL, FOOTER1, FOOTER2, FOOTER3, FOOTER4};
const uint8_t BUZZER_TOUCH_SOUND_DIS[]  = {HEADER, 0x79, 0x00, FOOTER1, FOOTER2, FOOTER3, FOOTER4};   // Buzzer will be off
const uint8_t BUZZER_TOUCH_SOUND_EN[]   = {HEADER, 0x79, 0x0A, FOOTER1, FOOTER2, FOOTER3, FOOTER4};   // Buzzer will be on - 100ms pulse
const uint8_t BUZZER_ALARM_SOUND_SET[]  = {HEADER, 0x7A, };  // Parameters and footer to follow - T1, T2, Freq1, Freq2
const uint8_t TC_SEND_TR_PAGE_KEY_ID[]  = {HEADER, 0x78, FOOTER1, FOOTER2, FOOTER3, FOOTER4};   // Touch-release - send page and key IDs
const uint8_t TC_SEND_VP_ADDR_VP_VAL[]  = {HEADER, 0x77, FOOTER1, FOOTER2, FOOTER3, FOOTER4};   // Send touch VP_Address & VP_Value
const uint8_t VP_STR_WRITE[]            = {HEADER, 0x42, };   // VP Address, Data & Footer to follow (32KB)
const uint8_t VP_STR_READ[]             = {HEADER, 0x43, };   // VP Address & Footer to follow & Data will be sent back

// Response to any of the commands
const uint8_t HNDSHK_RESPONSE_HEADER[] = {0xaa, 0x30};  // Handshake
const uint8_t ACK_RESPONSE_HEADER[]    = {0x3a, 0x3e};  // ACK
const uint8_t NACK_RESPONSE_HEADER[]   = {0x21, 0x3e};  // NACK

const uint8_t  FOOTER[]            = {FOOTER1, FOOTER2, FOOTER3, FOOTER4};

uint8_t reset_check_flg,HeartbeatDelay;

uint16_t UICommFaildCounter;
uint8_t once_variable=1,once_auto=1,once_auto_fill=1,once_variable_fill=1,once_fill_en=1,error_en=1,Once_all_dis=1;
uint16_t init_time=0;
uint8_t  ui_init_time=0;
uint8_t modbus_timer=0;
//uint8_t Remote_access=0;
uint8_t repeat_local=1;
uint32_t out_error=0x000000;
uint8_t  waytimer=25;



////////////////////////////
// CU-UI INTERFACE STRUCTURES
/////////////////////////////
// Control module to UI module (Write-only) - Map to variables at VP_CHR Area
#pragma pack(1)
VP_EVNT_WR_STR   vp_evnt_wr_str;
VP_RT_WR_STR     vp_rt_wr_str;        // Real time data - 189 bytes
VP_PARAM_RW_STR  vp_param_rw_str;     // Parameters - 340 bytes [SECONDARY]
VP_CAL_RD_STR    vp_cal_pt_rd_str;    // Calibration points structure - 80 bytes
VP_CAL_RD_STR    cal_par_str;         // Calibration parameters structure - 80 bytes - Backed up in EEPROM
VP_CRED_RW_STR   login_cred_str[MAX_ADMIN_ACCOUNTS+MAX_USER_ACCOUNTS];  // Login credentials for CU - 462 bytes
/////////////////////////////

/* Declaration of static local variables */
/* Declaration of local functions */
/* Function Declarations start here */


//const Errors_restart error_st[]={
//		{ERR_DP_SENSOR_FAILURE},
//		{ERR_NO_LN2_SUPPLY},
//		{ERR_LN2_IMPROPER_FILL},
//		{ERR_GBP_STUCK_OPEN},
//		{ERR_MFV_STUCK_OPEN},
//};

/*
 * *****************************************************************************
 * Function name: UI_init()
 * Created by : Venkatesh Arikanti
 * Date :       03-Sep-2022
 * Description:
 *  This function initializes UI communication  parameters.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void UI_init(void)
{
	UI_info = UI_INFO_DEFAULT;

	Set_delay(SW_TMR_UI_TX_POLL, DLY_UI_STRT, &UI_info.poll_flag); // Execution control
	Set_delay(SW_TMR_UI_DLY, DLY_UI_TIMEOUT, &UI_info.dly_flag);

	InitLCDCirBuffer();
}

/*
 * *****************************************************************************
 * Function name: UI_proc()
 * Created by : Venkatesh Arikanti
 * Date :       03-Sep-2022
 * Description:
 *  This function handles the UI communication  activities.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */


void UI_proc(void)
{
  //Process Rx Data
  LCDDataProc();

  if((factory_ok_enable_flg)||(just_for_display_flg))
  {
	    factory_default();
  }

  if((UI_Hours_Check!=Time.Hours)||(UI_RTC_set_flg)||(UI_Communication_Disconnected_Flg))
  {
	  UpdateUIRTC();
	  if(UI_Communication_Disconnected_Flg)
	  UI_Load_Init();
	  UI_RTC_set_flg=0;
	  UI_Communication_Disconnected_Flg=0;
  }
  if(RTC_Sec!=Time.Seconds)
  {
	  RTC_Sec=Time.Seconds;
	  Rtc_Toggle=((~Rtc_Toggle)&0x01);
	  if(HeartbeatDelay<=60)
	  {
		  HeartbeatDelay++;
		  if(HeartbeatDelay>=10)
		  {
			  HeartbeatDelay=0;
			  UICommFaildCounter++;
			  SendCmdToDisplay(DISP_HANDSHAKE_CMD);
			  if(UICommFaildCounter>5)
				  UI_Communication_Disconnected_Flg=1;
		  }
	  }
	  if((UICommFaildCounter>18)&&(1))
	  {
		  System_Status[ERROR_FLAG_0] |= ERR_UI_COM_FAILURE;
		 // UpdateUIErrorLog();
		 // spm_info.shutdown=1;
		  //Shut_Load=0;
		  //shutdown_flag=1;
	  }
	  Time_out++;
	  if((Time_out>60) &&(Time_home_ret==0)&&(!FirmwareUpdateModeOn) &&(usb_flag)&&(vp_rt_wr_str.LoginStatus))
	  {
		 Logoutfunction();
		 SendN16DataToDisplay(FACTORY_RST_VKEY, 0);
	//	 SendN16DataToDisplay(0x080990, 1);
	  }
	  if(clear_iot_popup)
		  iot_popup_timer++;
	  else
		  iot_popup_timer=0;
	  if((iot_popup_timer>60)&&(clear_iot_popup))
	  {
		  clear_iot_popup=0;
		  TargetPage =0;
	      SendCmdToDisplay(LOAD_PAGE_CMD);
		  iot_popup_timer=0;
	  }
	  /*if(update_page_flag)
	  {
		  init_time++;
		  if((init_time>300) &&(SD_info.state == SD_DONE))
		  {
			  init_time=0;
			  update_page_flag=0;
			  Update_system();
			  Time_out=50;
		  }
	  }*/
	  if(Init_Wait_Delay_for_UI_flg==0)
	  {
		  ui_init_time++;
	  }
	  if(ui_init_time>20 && ui_init_time<55)
	  {
		  TargetPage = 0;
		  SendCmdToDisplay(LOAD_PAGE_CMD);
		  ui_init_time=56;
		  Init_Wait_Delay_for_UI_flg=1;
	  }
	 /* if(vp_param_rw_str.mb_cfg.node_id != RS485_MASTER_NODE_ID)
	  {
		  modbus_timer++;
		  if((modbus_timer>80)&&(repeat_local))
		  {
			  fill_mon_info.ofaf_cmd=1;//local
			  if(vp_param_rw_str.STM_EN_4_20_mA)
			  {
				  vp_rt_wr_str.MBStatus=2;
			  }
			  else
			  {
				  vp_rt_wr_str.MBStatus=0;
			  }
			  SendN16DataToDisplay(Remote_local,vp_rt_wr_str.MBStatus);
			  SendN16DataToDisplay(MODBUS_SLAVE_EN,1);
			  EEPROM_Write(modbus_com, 0,(uint8_t*)&vp_rt_wr_str.MBStatus,1);
			  repeat_local=0;
		  }
	  }
	  else
	  {
		  modbus_timer=0;
	  }*/
	  if ((factory_load_delay > 0) && (factory_ok_enable_flg))
	  	factory_load_delay--;
	  waytimer++;
	  if(waytimer>40)
	  {
		  Update.firm_page=1;
		  waytimer=25;
	  }

  }
  if(reset_check_flg)
  {
	  reset_check_flg=0;
	  reset_fn();
  }

  switch (UI_info.state)
  {
    //////////////////////////////////
    // One-time initialization process
    //////////////////////////////////
  case UI_INIT:
	if (UI_info.poll_flag)
	{
		if(DISP_INIT_COMPLETED == SendDispInitSequence())
		{
			UI_info.state = UI_WAIT;
			UI_Load_Init();
			UpdateScreenSaverSettings();
			SendN16DataToDisplay(0x0800C6, 0);
			error_en=1;
			reset_check_flg=1;
			SendN16DataToDisplay(STOP_KEY_VP_ADDRESS,STOP_KEY_RELEASED);
			SendSTRDataToDisplay(LOGEDIN_USER,(uint8_t*)"\0"); //string
			//SendN16DataToDisplay(0x089004, 0);
			SendN16DataToDisplay(DISABLE_COMM_FAILURE, 0);
			SendN16DataToDisplay(FACTORY_RST_VKEY, 0);//Enable Factory index icon
			/*if((Ln2_usage.datausage<1000 && Ln2_usage.datausage>=0)&&((Ln2_usage.pre_datausage<1000 && Ln2_usage.pre_datausage>=0)))
			{
				Ln2_usage.pre_datausage_cm=Ln2_usage.pre_datausage*2.54;
				Ln2_usage.datausage_cm=Ln2_usage.datausage*2.54;
				if(vp_param_rw_str.lvlunits)
				{
					SendN16DataToDisplay(0x08905A,(uint16_t)(Ln2_usage.pre_datausage*10));
					SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage*10));
				}
				else
				{
					SendN16DataToDisplay(0x08905A,(uint16_t)(Ln2_usage.pre_datausage_cm*10));
					SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage_cm*10));
				}
				Ln2_usage.pre_datausage=Ln2_usage.pre_ln2usage;
			}
			else
			{
				Ln2_usage.datausage=0;
				Ln2_usage.pre_datausage=0;
				Ln2_usage.pre_ln2usage=0;
				Ln2_usage.pre_datausage_cm=Ln2_usage.pre_datausage*0;
				Ln2_usage.datausage_cm=Ln2_usage.datausage*0;
				if(vp_param_rw_str.lvlunits)
				{
					SendN16DataToDisplay(0x08905A,(uint16_t)(Ln2_usage.pre_datausage*10));
					SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage*10));
				}
				else
				{
					SendN16DataToDisplay(0x08905A,(uint16_t)(Ln2_usage.pre_datausage_cm*10));
					SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage_cm*10));
				}
			}*/
		 if(((login_cred[1].admin_id[0]==0)||(login_cred[1].admin_id[0]==255))&&(vp_rt_wr_str.LoginStatus==0))
		 {
			SendN16DataToDisplay(SECURITY_RESTRICT, 1); //user access page enable/disable

			//stop_en();
		 }
//		 if(vp_param_rw_str.plumbing_options & MB_SINTESY_MODE )
//		 {
//			 if(vp_param_rw_str.STM_EN_4_20_mA)
//			 {
//				 vp_param_rw_str.STM_EN_4_20_mA=0;
//			 }
//		 }
//		 else
//		 {
			 if((vp_rt_wr_str.MBStatus !=0) && (vp_rt_wr_str.MBStatus !=2))
			 {
				if(vp_param_rw_str.DataRead_En)
				{
					vp_param_rw_str.DataRead_En =0;
					LoadPhaseSettings(&UIModbusData);
				}
			//	fill_mon_info.ofaf_cmd=0;//local
				repeat_local=1;
				if(vp_param_rw_str.STM_EN_4_20_mA)
				{
					vp_rt_wr_str.MBStatus=3;
				}
				else
				{
					vp_rt_wr_str.MBStatus=1;
				}
				SendN16DataToDisplay(Remote_local,SensorReading.Mod_Status);
				SendN16DataToDisplay(MODBUS_SLAVE_EN,0);
			 }
			 else
			 {
		//		fill_mon_info.ofaf_cmd=1;//Remote
				if(vp_param_rw_str.STM_EN_4_20_mA)
				{
					vp_rt_wr_str.MBStatus=2;
				}
				else
				{
					vp_rt_wr_str.MBStatus=0;
				}
				SendN16DataToDisplay(Remote_local,vp_rt_wr_str.MBStatus);
				SendN16DataToDisplay(MODBUS_SLAVE_EN,1);
			 }
//		 }

		}
		else
		{
			Set_delay(SW_TMR_UI_DLY, DLY_UI_TIMEOUT, &UI_info.dly_flag);
			Set_delay(SW_TMR_UI_TX_POLL, DLY_UI_TX_POLL, &UI_info.poll_flag);
			SendN16DataToDisplay(STRING_POPUP_EN, 0);
			UpdateUIRTC();
			UI_info.state = UI_INIT_CONT;
		}
	}
	break;

  case UI_INIT_CONT:
	if(UI_info.dly_flag)
	{
		InitSeqResponseAck(0); //Just return 0 to indicate no response
		UI_info.state = UI_INIT;
	}
	break;
  case UI_WAIT:
	  // Wait for timeouts to generate a transaction to the UI

		// TFT is not Busy (detected via RTS_RDY, using PB4)
	  if (UI_info.poll_flag)
		{
			UI_info.state = UI_TX_CMD;
#ifdef EN_CTRS
			Set_delay(SW_TMR_UI_TX_POLL, DLY_UI_TX_POLL+DLY_UI_TX_POLL, &UI_info.poll_flag);
#else
			Set_delay(SW_TMR_UI_TX_POLL, DLY_UI_TX_POLL, &UI_info.poll_flag);
#endif
		}
	 break;

	/////////////////////////////////
	//Run time data sending
	/////////////////////////////////
	case UI_TX_CMD:
		if(DISP_TX_SEQ_COMPLETED == SendRunTimeDataToDisplay())
		{
			UI_info.state = UI_TX_DONE;
		}
		else
		{
			UI_info.state = UI_TX_CONT;
			Set_delay(SW_TMR_UI_DLY, DLY_UI_TIMEOUT, &UI_info.dly_flag);

		}
		break;

  	case UI_TX_CONT:
	  if(UI_info.dly_flag)
	  {
		  DispDataReadResponseAck(0);//Ack with 0 as no response
		  UI_info.state = UI_TX_CMD;
	  }
  	  break;

    case UI_TX_DONE:
	  UI_info.state = UI_WAIT;
	  break;

  	case UI_ERR:
      default:
        Set_delay(SW_TMR_UI_TX_POLL, DLY_UI_STRT, &UI_info.poll_flag);
        UI_info.state = UI_INIT;
  	  break;

   }
}



void UpdateUIRTC(void)
{
	//Get_TimeStamp();
	RTC_Year=Date.Year;
	RTC_Month=Date.Month;
	RTC_Date=Date.Date;
	UI_Hours_Check = Time.Hours;;
	if(Time.TimeFormat==1)
	{
		if(Time.Hours<12)
		RTC_Hour=(Time.Hours+12);
		else
		RTC_Hour=(Time.Hours);
	}
	else
	{
		if(vp_param_rw_str.RTC_TimeFormat)
		{
			if(Time.Hours<12)
			RTC_Hour=Time.Hours;
			else
			RTC_Hour=(Time.Hours-12);
		}
		else
			RTC_Hour=Time.Hours;
	}
	RTC_Min=Time.Minutes;
	RTC_Sec=Time.Seconds;
	SendCmdToDisplay(SET_RTC_CMD);
}


void UpdateScreenSaverSettings()
{
	SendCmdToDisplay(DISP_SCREEN_SAVER_CMD);
}

/*
 ***********************************
 * *********************************
 * End of the module: UI.c
 * *********************************
 * *********************************
 */
