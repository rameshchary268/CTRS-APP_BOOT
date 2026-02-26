/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: DispUIkeyhandler.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains Touch key handling functions for the LCD UI
 *
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DispVPAddrList.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
#include "DisplayTxHandlers.h"
#include "UISettings.h"
#include "UserAccess.h"
#include "EEPROM.h"
#include "FactoryReset.h"
#include  "UI.h"
#include "TouchHandler.h"

VP_CRED_RW_STR vp_cred_rw_str;

VP_CRED_RW_STR   login_cred[MAX_ADMIN_ACCOUNTS+MAX_USER_ACCOUNTS+1];  // Login credentials for CU - 180 bytes

uint8_t calibration_canceled_flg;

#define INCH_MM_TOGGLE_KEY	0x80572
#define DEGREE_F_TOGGLE_KEY 0x80560


volatile uint8_t PrevTempUnit = 0;
volatile uint8_t PrevTimeFormat = 0;

uint8_t ErrMessage[246];
uint8_t HeadErrId= 0;
uint8_t currentIndexvalue=1;



uint8_t factory_load_delay_flg,factory_load_delay=5,factory_ok_enable_flg;
uint8_t just_for_display_flg,factory_msg1_display_delay_flg,factory_msg1_display_delay,factory_msg2_display_delay_flg,factory_msg2_display_delay;
uint8_t clear_reminder;
uint8_t Login_id_cnt;

uint8_t Time_home_ret=0;
//uint8_t Touch_flag=1;
uint8_t Time_start=0;
uint8_t Error_flag_en=0;
uint8_t Account_creation=0;
uint8_t send_email_iot=0;

uint8_t CLR_Settings_Flag = 0;


uint8_t FrezzID[9];
uint8_t AddUserName[8],EmployeID[8];

/* Declaration of global variables */
volatile uint16_t DefaultHandlerRunCtr = 0;
CalibDataStruct UISensorCalibData;
TemperatureSettingsStruct UITempeSettingsData;
LEVELSettingsStruct       UILevelSettingsData;
LEDSettingsStruct 		  UILEDSettingsData;
AlarmSettingsStruct UIAlarmSettingsData;//6
LidFunctionsStruct UILidFunctionSettingsData;//8
DisplayDataStruct UIDisplayData;//7
ModbusDataStruct UIModbusData;//5
ScheduledEventStruct UIScheduledEventData;//1
RemainderSettingsStruct UIScheduledFillData;//2
SCHRemainderSettingsStruct UIFillRemainderData;//3
MaintenanceRemainderSettingsStruct UIMaintenanceRemainderSettings;//4
DateTimeSettingsStruct UIDateTimeSettings;
FreezerStruct UIFreezerStruct;
Wifi UIwifi;
AWS UIAws;
UpdateRemainderSettingsStruct UIUpdateRemaindersettings;
UpdateGroupSettingsStruct  UIUpdateGroupsettings;
UpdateSystemSettingsStruct UIUpdateSystemSettings;

User_access   UIUserAccess;
AddUserStruct UIAddUserStruct;
login_credentials UIlogin;

const Eeprom_address eep_user_reg[]={
		{admin0, admin0_crc, emp0, emp0_crc0,0,          	emp0_pw,emp0_pw_crc},
		{admin1, admin1_crc, emp1, emp1_crc1,admin1_sts, 	emp1_pw,emp1_pw_crc},
		{admin2, admin2_crc, emp2, emp2_crc2,admin2_sts, 	emp2_pw,emp2_pw_crc},
		{user0,  user0_crc,  emp3, emp3_crc3,0,          	emp3_pw,emp3_pw_crc},
		{user1,  user1_crc,  emp4, emp4_crc4,0,          	emp4_pw,emp4_pw_crc},
		{user2,  user2_crc,  emp5, emp5_crc5,0,				emp5_pw,emp5_pw_crc},
		{user3,  user3_crc,  emp6, emp6_crc6,0,				emp6_pw,emp6_pw_crc},
		{user4,  user4_crc,  emp7, emp7_crc7,0,				emp7_pw,emp7_pw_crc},
		{user5,  user5_crc,  emp8, emp8_crc8,0,				emp8_pw,emp8_pw_crc},
		{user6,  user6_crc,  emp9, emp9_crc9,0,				emp9_pw,emp9_pw_crc},
		{user7,  user7_crc,  emp10,emp10_crc10,0,			emp10_pw,emp10_pw_crc},
};

const  Register_address address_reg[]={
		{0,		  0,       0,                0,       0},
		{USER_NAME_1, User_Tick1_En,DELETE_SELECT_WR1,DELETE_SELECT_RD1,USER1_EN,ADMIN1_EN},
		{USER_NAME_2, User_Tick2_En,DELETE_SELECT_WR2,DELETE_SELECT_RD2,USER2_EN,ADMIN2_EN},
		{USER_NAME_3, User_Tick3_En,DELETE_SELECT_WR3,DELETE_SELECT_RD3,USER3_EN, 0},
		{USER_NAME_4, User_Tick4_En,DELETE_SELECT_WR4,DELETE_SELECT_RD4,USER4_EN, 0},
		{USER_NAME_5, User_Tick5_En,DELETE_SELECT_WR5,DELETE_SELECT_RD5,USER5_EN, 0},
		{USER_NAME_6, User_Tick6_En,DELETE_SELECT_WR6,DELETE_SELECT_RD6,USER6_EN, 0},
		{USER_NAME_7, User_Tick7_En,DELETE_SELECT_WR7,DELETE_SELECT_RD7,USER7_EN, 0},
		{USER_NAME_8, User_Tick8_En,DELETE_SELECT_WR8,DELETE_SELECT_RD8,USER8_EN, 0},
		{USER_NAME_9, User_Tick9_En,DELETE_SELECT_WR9,DELETE_SELECT_RD9,USER9_EN, 0},
		{USER_NAME_10,User_Tick10_En,DELETE_SELECT_WR10,DELETE_SELECT_RD10,USER10_EN,0},

};


const UIDataHandlerStruct UIDataHandlerList[] = {
		{ T1_HIGH_LIMIT_CONTROL_WR,		N16_TYPE,	1,	0,		1,		NULL,						&UITempeSettingsData.T1HighLimitSwitch},
		{ T1_HIGH_LIMIT_VALUE_WR,		N16_TYPE,	1,	-5200,	0,	    TemperatureLimitCheck,		&UITempeSettingsData.T1HighLimit},
		{ T1_LOW_LIMIT_CONTROL_WR,		N16_TYPE,	1,	0,		1,		NULL,						&UITempeSettingsData.T1LowLimitSwitch},
		{ T1_LOW_LIMIT_VALUE_WR,		N16_TYPE,	1,	-5640,	0,	    TemperatureLimitCheck,		&UITempeSettingsData.T1LowLimit},
		{ T2_HIGH_LIMIT_CONTROL_WR,		N16_TYPE,	1,	0,		1,		NULL,						&UITempeSettingsData.T2HighLimitSwitch},
		{ T2_HIGH_LIMIT_VALUE_WR,		N16_TYPE,	1,	-5200,	0,	    TemperatureLimitCheck,		&UITempeSettingsData.T2HighLimit},
		{ T2_LOW_LIMIT_CONTROL_WR,		N16_TYPE,	1,	0,		1,		NULL,						&UITempeSettingsData.T2LowLimitSwitch},
		{ T2_LOW_LIMIT_VALUE_WR,		N16_TYPE,	1,	-5640,	0,	    TemperatureLimitCheck,		&UITempeSettingsData.T2LowLimit},
		{ TEMP_UNIT_SELECTION_WR,		N16_TYPE,	1,	0,		1,		TemperatureUnitConvHandler,	&UITempeSettingsData.TempUnitSelection},
		{ T2_ENABLE_CONTROL_WR,			N16_TYPE,	1,	0,		1,		NULL,						&UITempeSettingsData.T2Enable},

		{ LEVEL_EMPTY_WR,			    N16_TYPE,	1,	0,		1,		NULL,						&UILevelSettingsData.EMPTY_EN},
		{ LEVEL_REFILLSOON_WR,			N16_TYPE,	1,	0,		1,		NULL,						&UILevelSettingsData.REFILLSOON},
		{ LEVEL_FILL_WR,			    N16_TYPE,	1,	0,		1,		NULL,						&UILevelSettingsData.FILL},

		{ REMOTE_ALARM_DELAY_ENABLE_WR,	N16_TYPE,	1,	0,		1,		NULL,						&UIAlarmSettingsData.RemoteAlarmDelaySwitch},
		{ REMOTE_ALARM_DELAY_VALUE_WR,	N16_TYPE,	1,	1,		60,		NULL,						&UIAlarmSettingsData.RemoteAlarmDelay},
		{ AUDIBLE_REPEAT_DELAY_ENABLE_WR,N16_TYPE,	1,	0,		1,		NULL,						&UIAlarmSettingsData.AudiableRepeatDelaySwitch},
		{ AUDIBLE_REPEAT_DELAY_VALUE_WR,N16_TYPE,	1,	1,		60,		NULL,						&UIAlarmSettingsData.AudiableRepeatDelay},
		{ LN2_USAGE_ALARM_ENABLE_WR,	N16_TYPE,	1,	0,		1,		NULL,						&UIAlarmSettingsData.LN2UsageDelaySwitch},
		{ LN2_USAGE_ALARM_VALUE_WR,	    N16_TYPE,	1,	1,		20,		NULL,						&UIAlarmSettingsData.LN2UsageAlarm_value},
		{ TEST_ALARM_ENABLE_WR,			N16_TYPE,	1,	0,		1,		TestAlarmCtrl,				&UIAlarmSettingsData.TestAlarmControl},
		{ BAT_SW_OVR_WR			        ,N16_TYPE,	1,	0,	    1,      NULL,		                &UIAlarmSettingsData.Battery_sw_over},

//		{ DP_1POINT_CAL_VALUE_WR,		N16_TYPE,	1,	0,		65,		NULL,						&UISensorCalibData.DP1PointValue},
		{ DP_2POINT_CAL1_VALUE_WR,		N16_TYPE,	1,	-3640,	800,	CalibProcessControlHandler,	&UISensorCalibData.DP2PointValue1},
		{ DP_2POINT_CAL2_VALUE_WR,		N16_TYPE,	1,	-3640,	800,	CalibProcessControlHandler,	&UISensorCalibData.DP2PointValue2},

//		{ CALIB_START_STOP_CONTROL_WR,	N16_TYPE,	1,	0,		1,		CalibProcessControlHandler,	&UISensorCalibData.CalibStartStopCtrl},
		{ DISPLAY_BRIGHTNESS_WR,		N16_TYPE,	1,	0,	    100,	NULL,		                &UIDisplayData.Brightness},
		{ DISPLAY_TOUCHSOUND_WR,		N16_TYPE,	1,	0,	    1,	    NULL,						&UIDisplayData.TouchSound},
		{ DISPLAY_SCREENSAVER_WR,		N16_TYPE,	1,	0,	    3,	    NULL,						&UIDisplayData.ScreenSaverTime},
		{ DISP_LED_BRIGHTNESS_WR,		N16_TYPE,	1,	10,		255,	Led_britenessCtrl,			&UIDisplayData.LED_Brightness},
		{ STATUS_LED_ENABLE_WR, 		N16_TYPE,   1,   0,     1,		NULL,						&UIDisplayData.StatusLEDEn},
		//{ MODBUS_CRYODEVICEID_WR,		N16_TYPE,	1,	0,	    247,	NULL,						&UIModbusData.DeviceIDCryolink},
		//{ MODBUS_CRYOBAUDRATE_WR,		N16_TYPE,	1,	0,	    3,		NULL,						&UIModbusData.BaudRateCryolink},
		//{ MODBUS_CRYOFPBBYTE_WR,		N16_TYPE,	1,	0,	    3,		NULL,						&UIModbusData.FPByteOrderCryolink},
		{ MODBUS_PONEDEVICEID_WR,		N16_TYPE,	1,	0,	    247,	NULL,						&UIModbusData.DeviceIDPhasetwo},
		{ MODBUS_PONEBAUDRATE_WR,		N16_TYPE,	1,	0,	    3,		NULL,						&UIModbusData.BaudRatePhasetwo},
		{ MODBUS_DATA_WR,		        N16_TYPE,	1,	0,	    1,		NULL,						&UIModbusData.DataRead_En},
		//{ MODBUS_FILLCMD_WR,		    N16_TYPE,	1,	0,	    1,		NULL,						&UIModbusData.Fill_Cmd},
		//{ MODBUS_PONEFILLMETHOD_WR,		N16_TYPE,	1,	0,	    1,		NULL,						&UIModbusData.FillMethodPhasetwo},
		//{ EN_DIS_4_20mA_WR,			    N16_TYPE,	1,	0,	    1,		NULL,		                &UIModbusData.EN_4_20_mA},

		{ SCHEDULED_FILL_WR,			N16_TYPE,	1,	0,	    1,		NULL,						&UIScheduledEventData.ScheduledFillRemainderEn},
		{ SCHEDULED_FREM_WR,			N16_TYPE,	1,	0,	    1,		NULL,						&UIScheduledEventData.FillRemainderEn},
		{ SCHEDULED_MAIN_WR,			N16_TYPE,	1,	0,	    1,		NULL,						&UIScheduledEventData.MaintRemainderEn},
//		{ SCHEDULED_LN2S_WR,			N16_TYPE,	1,	0,	    1,		NULL,						&UIScheduledEventData.FillRemainderEn},
		{ SCHEDULED_FILL_HR_WR,			N16_TYPE,	1,	0,	    23,		NULL,						&UIScheduledFillData.HoursSettings},
		{ SCHEDULED_FILL_MIN_WR,		N16_TYPE,	1,	0,	    59,		NULL,						&UIScheduledFillData.MinutesSettings},
		{ SCHEDULED_FILL_AMPM_WR,		N16_TYPE,	1,	0,	    1,		NULL,						&UIScheduledFillData.AMPMSettings},
		{ SCHEDULED_FILL_DAILY_WR,     N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_SUNDAY_WR,      N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_MONDAY_WR,     N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_TUESDAY_WR,    N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_WEDNESDAY_WR,  N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_THURSDAY_WR,   N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_FRIDAY_WR,     N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_SATURDAY_WR,   N16_TYPE,   1,  0,      1,      daily,                       NULL},
		{ SCHEDULED_FILL_HR12_WR,		N16_TYPE,	1,	0,	    12,		NULL,						&UIScheduledFillData.Hours12Settings},

		{ FILLREM_MONTH_WR,			   N16_TYPE,	1,	1,	    12,		MonthDateLimitHandler,		&UIFillRemainderData.MonthSettings},
		{ FILLREM_DATE_WR,			   N16_TYPE,	1,	1,	    31,		DateLimitHandler,			&UIFillRemainderData.DateSettings},

		{ FILLREM_YEAR_WR,			   N16_TYPE,	1,	25,		99,		YearDateLimitHandler,		&UIFillRemainderData.YearSettings},
		{ FILLREM_24HOUR_VALUE_WR,	   N16_TYPE,	1,	0,	    23,		NULL,						&UIFillRemainderData.Hours24Settings},
		{ FILLREM_MINUTE_VALUE_WR,		N16_TYPE,	1,	0,	    59,		NULL,						&UIFillRemainderData.MinutesSettings},
		{ FILLREM_AMPM_VALUE_WR,		N16_TYPE,	1,	0,	    1,		NULL,						&UIFillRemainderData.AMPMSettings},
		{ FILLREM_12HOUR_VALUE_WR,		N16_TYPE,	1,	0,	    12,		NULL,						&UIFillRemainderData.Hours12Settings},

		{ RTC_DATEFORMAT_VALUE_WR,		N16_TYPE,	1,	0,	    1,		NULL,						&UIDateTimeSettings.DateFormat},
		{ RTC_12HOURS_VALUE_WR,		    N16_TYPE,	1,	0,	    12,		NULL,						&UIDateTimeSettings.Hours12Settings},
		{ RTC_TIMEFORMAT_VALUE_WR,		N16_TYPE,	1,	0,	    1,		TimeFormatHandler,			&UIDateTimeSettings.TimeFormat},
		{ RTC_MONTH_VALUE_WR,			N16_TYPE,	1,	0,	    12,		MonthDateLimitHandler,						&UIDateTimeSettings.MonthSettings},
		{ RTC_DATE_VALUE_WR,			N16_TYPE,	1,	0,	    31,		DateLimitHandler,						&UIDateTimeSettings.DateSettings},
		{ RTC_YEAR_VALUE_WR,			N16_TYPE,	1,	23,		99,		YearDateLimitHandler,						&UIDateTimeSettings.YearSettings},
		{ RTC_HOURS_VALUE_WR,			N16_TYPE,	1,	0,	    23,		NULL,						&UIDateTimeSettings.HoursSettings},
		{ RTC_MIN_VALUE_WR	,			N16_TYPE,	1,	0,	    59,		NULL,						&UIDateTimeSettings.MinutesSettings},
		{ RTC_AMPM_VALUE_WR,			N16_TYPE,	1,	0,	    1,		NULL,						&UIDateTimeSettings.AMPMSettings},

		{ MAINT_12HOURS_VALUE_WR,		N16_TYPE,	1,	0,	    12,		NULL,						&UIMaintenanceRemainderSettings.Hours12Settings},
		{ MAINT_MONTH_VALUE_WR,			N16_TYPE,	1,	0,	    12,		MonthDateLimitHandler,		&UIMaintenanceRemainderSettings.MonthSettings},
		{ MAINT_DATE_VALUE_WR,			N16_TYPE,	1,	0,	    31,		DateLimitHandler,			&UIMaintenanceRemainderSettings.DateSettings},

		{ MAINT_YEAR_VALUE_WR,			N16_TYPE,	1,	23,		99,		YearDateLimitHandler,		&UIMaintenanceRemainderSettings.YearSettings},
		{ MAINT_HOURS_VALUE_WR,			N16_TYPE,	1,	0,	    23,		NULL,						&UIMaintenanceRemainderSettings.HoursSettings},
		{ MAINT_MIN_VALUE_WR,			N16_TYPE,	1,	0,	    59,		NULL,						&UIMaintenanceRemainderSettings.MinutesSettings},
		{ MAINT_AMPM_VALUE_WR,			N16_TYPE,	1,	0,	    1,		NULL,						&UIMaintenanceRemainderSettings.AMPMSettings},

		{ FREEZER_ID_VALUE_WR,			STR_TYPE,	0,	0,	    0,		FreezerIdHandler,			NULL},

		{ ADD_USER_ID_VALUE_WR,			STR_TYPE,	0,	0,	    0,		AddUserHandler,			    (int16_t*)&UIAddUserStruct.AddUserName},
		{ ADD_EMP_ID_VALUE_WR,			STR_TYPE,	0,	0,	    0,		EmployeIDHandler,			(int16_t*)&UIAddUserStruct.EmployeID},
		{ ADMIN_OR_USER_WR,             N16_TYPE,   1,  0,      1,      NULL,                       &UIAddUserStruct.select},
		{ LIST_SELECT_WR,               N16_TYPE,   1,  0,      2,      listselection,              &UIUserAccess.list_select},
		{ DELETE_SELECT_WR1,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[0]},//select delete
		{ DELETE_SELECT_WR2,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[1]},
		{ DELETE_SELECT_WR3,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[2]},
		{ DELETE_SELECT_WR4,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[3]},
		{ DELETE_SELECT_WR5,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[4]},
		{ DELETE_SELECT_WR6,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[5]},
		{ DELETE_SELECT_WR7,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[6]},
		{ DELETE_SELECT_WR8,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[7]},
		{ DELETE_SELECT_WR9,            N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[8]},
		{ DELETE_SELECT_WR10,           N16_TYPE,   0,  0,      0,      NULL,                       &UIUserAccess.delete_select[9]},
		{ LOGIN_ID_WR,                  STR_TYPE,   0,  0,      0,      LoginHandler,               (int16_t*)&UIlogin.login_user},
		{ LOGIN_PW_WR,                  STR_TYPE,   0,  0,      0,      LoginpwHandler,             (int16_t*)&UIlogin.login_pw},
		{ LOGIN_NEW_PW_WR,              STR_TYPE,   0,  0,      0,      NewpwHandler,               (int16_t*)&UIlogin.new_pw},
		{ LOGIN_CONFM_PW_WR,            STR_TYPE,   0,  0,      0,      conformpwHandler,           (int16_t*)&UIlogin.conform_pw},
		{ WIFI_ADD_WR,                  STR_TYPE,   0,  0,      0,      WifiUserHandler,            (int16_t*)&UIwifi.wifi_user},
		{ AWS_URL_WR,                   STR_TYPE,   0,  0,      0,      AWSUrlHandler,              (int16_t*)&UIAws.Aws_url},
		{ AWS_USER_NAME_WR,             STR_TYPE,   0,  0,      0,      AWS_UsernameidHandler,      (int16_t*)&UIAws.Aws_username},
		{ AWS_EMAIL_WR,                 STR_TYPE,   0,  0,      0,      AWS_EmailHandler,           (int16_t*)&UIAws.Aws_email},
		{ AWS_PHNO_WR,                  STR_TYPE,   0,  0,      0,      AWS_PhoneHandler,           (int16_t*)&UIAws.Aws_phone},
		{ WIFI_PW_WR,                   STR_TYPE,   0,  0,      0,      WifipwHandler,              (int16_t*)&UIwifi.wifi_user_pswd },
		{ WIFI_CONNECT_WR,              N16_TYPE,   0,  1,      0,      IOT_WIFI_Handler,            (int16_t*) &UIwifi.connect_sts},
		{ IOT_CONNECT_WR,               N16_TYPE,   0,  1,      0,      IOT_WIFI_Handler,           (int16_t*) &UIwifi.IOT_connect_sts},
		{ WIFI_PASS_EN_WR,              N16_TYPE,   0,  1,      0,      WIFI_PASS_EN_Handler,       (int16_t*) &UIwifi.Wifi_password_en},

//		{ GRAPH_SELECTION_KEY_ADDRESS_WR,N16_TYPE,   1,  0,      4,     RefreshGraph,               (int16_t*)&GraphSelectionId },
//		{ GRAPH_SCALE_SELECTION_ADDRESS_WR,N16_TYPE, 1,  0,      4,     RefreshGraph,               (int16_t*)&GraphScaleSelection },
		//{ NW_ID_WR,                      STR_TYPE,   0,  0,      0,     NW_id_Handler,               NULL},
		{ UPDATE_12HOURS_VALUE_WR,		 N16_TYPE,	1,	0,	    12,		NULL,						 &UIUpdateRemaindersettings.Hours12Settings},
		{ UPDATE_MONTH_VALUE_WR,       	 N16_TYPE,	1,	0,	    12,		MonthDateLimitHandler,		 &UIUpdateRemaindersettings.MonthSettings},
		{ UPDATE_DATE_VALUE_WR,			 N16_TYPE,	1,	0,	    31,		DateLimitHandler,			 &UIUpdateRemaindersettings.DateSettings},
		{ UPDATE_YEAR_VALUE_WR,			 N16_TYPE,	1,	23,		99,		YearDateLimitHandler,		 &UIUpdateRemaindersettings.YearSettings},
		{ UPDATE_HOURS_VALUE_WR,		 N16_TYPE,	1,	0,	    23,		NULL,						 &UIUpdateRemaindersettings.HoursSettings},
		{ UPDATE_MIN_VALUE_WR,			 N16_TYPE,	1,	0,	    59,		NULL,						 &UIUpdateRemaindersettings.MinutesSettings},
		{ UPDATE_AMPM_VALUE_WR,			 N16_TYPE,	1,	0,	    1,		NULL,						 &UIUpdateRemaindersettings.AMPMSettings},
//		{ GRAPH_DAY_ENABLE_ADDRESS_WR,	 N16_TYPE,	1,	0,	    1,		RefreshDayGraph,				(int16_t*)&DayGraphActive},
//		{ GRAPH_DAY_SCALE_SELECT_ADDRESS_WR,N16_TYPE,1,	0,	    2,		RefreshGraph,				(int16_t*)&DayGraphScaleSelection},
//		{ GRAPH_DAY_DATE_ADDRESS_WR,	 N16_TYPE,	 1,	1,	    31,		NULL,				        (int16_t*)&DayGraphDate},
//		{ GRAPH_DAY_MONTH_ADDRESS_WR,	 N16_TYPE,	 1,	1,	    12,		NULL,				  		(int16_t*)&DayGraphMonth},
//		{ GRAPH_DAY_YEAR_ADDRESS_WR,	 N16_TYPE,	 1,	0,	    99,		NULL,						(int16_t*)&DayGraphYear},
		{ AWS_IOT_CNCTVTY,                N16_TYPE,   1, 0,      1,     IOT_Connectivity,           (int16_t*)&send_email_iot},

//		{ GROUP_EN_WR,			         N16_TYPE,	1,	0,	    1,		Groupen_dis,				 &UIUpdateGroupsettings.Group_en},
//		{ GROUP_SEL_WR,			         N16_TYPE,	1,	1,	    4,		Groupselection,				 &UIUpdateGroupsettings.Group_sel},
//		{ GROUP_A_WR,			         STR_TYPE,	0,	0,	    0,		GroupAStringHandler,		 (int16_t*)&UIUpdateGroupsettings.Group_A},
//		{ GROUP_B_WR,			         STR_TYPE,	0,	0,	    0,		GroupBStringHandler,		 (int16_t*)&UIUpdateGroupsettings.Group_B},
//		{ GROUP_C_WR,			         STR_TYPE,	0,	0,	    0,		GroupCStringHandler,		 (int16_t*)&UIUpdateGroupsettings.Group_C},
//		{ GROUP_D_WR,			         STR_TYPE,	0,	0,	    0,		GroupDStringHandler,		 (int16_t*)&UIUpdateGroupsettings.Group_D},
		{ EDIT_ADDRESS_WR,			     N16_TYPE,	1,	0,	    1,		Edit_Handler,		         &UIUpdateSystemSettings.Edit_en},
		{ SUPLY_CONTACT_WR,			     STR_TYPE,	0,	0,	    0,		Supply_con_Handler,		     (int16_t*)&UIUpdateSystemSettings.Supply_con},
		{ SERVICE_CONTACT_WR,			 STR_TYPE,	0,	0,	    0,		Service_con_Handler,		 (int16_t*)&UIUpdateSystemSettings.Service_con},
//		{ IOT_FIRM_ICON_WR,			     N16_TYPE,	1,	0,	    1,		Firm_Update_process,		 (int16_t*)&UIUpdateIOTFIRMsettings.Firmware_update_en},
		//4-20mA
//		{ FIRM_4_20_mA_ICON_WR,			 N16_TYPE,	1,	0,	    1,		Firm_4_20_mA_Update_process,		 (int16_t*)&UIUpdateIOTFIRMsettings.Frmware_4_20_mA_update},
//		{ EXT_MOD_EN_DIS_WR,			 N16_TYPE,	1,	0,	    1,		NULL,		 			     &UI_4_20mAsettings.EXD_MD_Switch},
//		{ CHA1_EN_DIS_WR,			     N16_TYPE,	1,	0,	    1,		NULL,		 				 &UI_4_20mAsettings.Chan1_Switch},
//		{ CHA1_SEL_WR,			         N16_TYPE,	1,	0,	    2,		NULL,		 				 &UI_4_20mAsettings.Chan1_Selction},
//		{ CHA1_VOL_CUR_WR,			     N16_TYPE,	1,	0,	    1,		PassiveLimitCheck,		 	 &UI_4_20mAsettings.Chan1_vol_cur},
//		{ CHA2_EN_DIS_WR,			     N16_TYPE,	1,	0,	    1,		NULL,		 				 &UI_4_20mAsettings.Chan2_Switch},
//		{ CHA2_SEL_WR,			         N16_TYPE,	1,	0,	    2,		NULL,		 				 &UI_4_20mAsettings.Chan2_Selction},
//		{ CHA2_VOL_CUR_WR,			     N16_TYPE,	1,	0,	    1,		PassiveLimitCheck,			 &UI_4_20mAsettings.Chan2_vol_cur},
//		{ SOURCE_WR,			         N16_TYPE,	1,	0,	    1,		NULL,						 &UI_4_20mAsettings.Source_sel},
//
//		{ DP_MIN_CONIG_CH1_WR,			 N16_TYPE,	1,	0,	    340,    DP4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.DP_MIN_CONFIG_CH1},
//		{ DP_MAX_CONIG_CH1_WR,			 N16_TYPE,	1,	0,	    340,	DP4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.DP_MAX_CONFIG_CH1},
//		{ T1_MIN_CONIG_CH1_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T1_MIN_CONFIG_CH1},
//		{ T1_MAX_CONIG_CH1_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T1_MAX_CONFIG_CH1},
//		{ T2_MIN_CONIG_CH1_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T2_MIN_CONFIG_CH1},
//		{ T2_MAX_CONIG_CH1_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T2_MAX_CONFIG_CH1},
//
//		{ DP_MIN_CONIG_CH2_WR,			 N16_TYPE,	1,	0,	    340,    DP4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.DP_MIN_CONFIG_CH2},
//		{ DP_MAX_CONIG_CH2_WR,			 N16_TYPE,	1,	0,	    340,	DP4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.DP_MAX_CONFIG_CH2},
//		{ T1_MIN_CONIG_CH2_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T1_MIN_CONFIG_CH2},
//		{ T1_MAX_CONIG_CH2_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T1_MAX_CONFIG_CH2},
//		{ T2_MIN_CONIG_CH2_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T2_MIN_CONFIG_CH2},
//		{ T2_MAX_CONIG_CH2_WR,			 N16_TYPE,	1,	-2400,	0,		Temperature4_20mALimitCheck,		 				 &UI_4_20mAconfigSettings.T2_MAX_CONFIG_CH2},
		//ctrs
//		{ RTD_HIGH_LIMIT_VALUE_WR,			 N16_TYPE,	1,	-2400,	500,		CTRSTemperatureLimitCheck,		 				 &UICTRStemperature.THighLimit},
//		{ RTD_LOW_LIMIT_VALUE_WR,			 N16_TYPE,	1,	-2400,	500,		CTRSTemperatureLimitCheck,		 				 &UICTRStemperature.TLowLimit},
//		{ LOAD_HIGHLIMIT_VALUE_WR,			 N32_TYPE,	0,	0,	0,		CTRSloadLimitCheck,		 				 (int16_t*)&UICTRStemperature.Highloadlimit},
//		{ CAL_LOAD_OFFSET_WR,			 N32_TYPE,	0,	0,	0,		CTRSloadLimitCheck,		 				 (int16_t*)&UICTRStemperature.cal_load},
		{ FILE_TICK_SEL1_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[0].file_sel},
		{ FILE_TICK_SEL2_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[1].file_sel},
		{ FILE_TICK_SEL3_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[2].file_sel},
		{ FILE_TICK_SEL4_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[3].file_sel},
		{ FILE_TICK_SEL5_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[4].file_sel},
		{ FILE_TICK_SEL6_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[5].file_sel},
		{ FILE_TICK_SEL7_WR,			 N16_TYPE,	1,	0,	1,		NULL,		 				 &dirList[6].file_sel},

		{ERRORS_ONLY_WR, 				N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.ERRORS_ONLY},
		{TEMP_BIT_WR, 					N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.TEMP},
		{LEVEL_BIT_WR, 					N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.LEVEL},
		{VACUUM_BIT_WR, 				N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.VACUUM},
		{STORAGE_BIT_WR, 				N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.STORAGE_FAIL},
		{MAINS_FAIL_BIT_WR, 			N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.MAINS_BATT},
		{NETWORK_FAIL_BIT_WR, 			N16_TYPE, 	1, 0, 1, 					NULL,								&UILEDSettingsData.NETWORK_ISSUES},
		{BLINK_CTRL_WR, 				N16_TYPE, 	1, 0, 4, 					NULL,								&UILEDSettingsData.BLINK_CTRL},
		{NORMAL_CLR_WR, 				N16_TYPE, 	1, 0, 8, 					NULL,								&UILEDSettingsData.NO_ERROR},
		{ERROR_CLR_WR, 					N16_TYPE, 	1, 0, 8, 					NULL,								&UILEDSettingsData.ERRORS},
		{WARNING_CLR_WR, 				N16_TYPE, 	1, 0, 8, 					NULL,								&UILEDSettingsData.WARNINGS},
		{UPDATE_CLR_WR, 				N16_TYPE, 	1, 0, 8, 					NULL,								&UILEDSettingsData.UPDATES},
		{NEW_COLOR_WR, 					N16_TYPE, 	1, 0, 8, 					NULL,								&UILEDSettingsData.NEW_COLOR},
		{OLD_COLOR_WR,					N16_TYPE,   1, 0, 8,					NULL,								&UILEDSettingsData.OLD_CLR},
};


const TouchKeyHandlerStruct TouchKeyHandlerList[MAX_PAGE_IDS+1] = {
		{ HOME_SCREEN_PAGE, 					HandleHomeScreenKeyFunctions },
		{ MENU_SCREEN_PAGE, 					HandleMenuScreenKeyFunctions },
		{ TEMP_SETTINGS_PAGE, 					HandleTempSettingsKeyFunctions },
		{ LEVEL_SELECT_PAGE, 			        HandleLevelSettingsKeyFunctions },  // RTD calib value page
		{ SYSTEM_SCREEN_PAGE, 					HandleSystemSettingsKeyFunctions },
		{ WEIGHT_SETTINGS_PAGE, 				DefaultTouchKeyHandler },
		{ VACCUM_SELECT_PAGE, 			        DefaultTouchKeyHandler }, //Selection of RTD 1P or 2P
		{ SECURITY_SETTINGS_PAGE, 				HandleSecurityKeyFunctions },
		{ DISPLAY_SCREEN_PAGE, 					HandleDisplaySettingsKeyFunctions },
		{ SET_DATE_TIME_PAGE, 					HandleRTCPageKeyFunctions },
		{ SYSTEM_INFO_PAGE, 					HandleSystemInfoPageKeyFunctions },
		{ ALARMS_SETTINGS_PAGE, 				HandleAlarmSettingsKeyFunctions },
		{ SCHEDULE_EVENTS_PAGE, 				HandleScheduledPageKeyFunctions },	//need to change
		{ SCHEDULE_MFILL_PAGE, 					HandleScheduledFillPageKeyFunctions },	//need to change
		{ CALIB_SENSOR_SELECT_PAGE, 			HandleCalibSensorSelectFunctions },//DP, T1, T2 Calib selection page
		{ RTD_CALIB_1P_2P_VALUE_PAGE, 			HandleRTDCalib1POr2PSelectFunctions },  // RTD calib value page
		{ CALIB_SETTINGS_PAGE, 					HandleDP2PCalibKeyFunctions }, //DP calibration value page
		{ WEIGHT_CALIB_PAGE,	 						DefaultTouchKeyHandler },	//need to change
		{ VACCUM_CALIB_PAGE, 							DefaultTouchKeyHandler },	//need to change
		{ WEIGHT_LOADCELL_SETTINGS_PAGE, 				DefaultTouchKeyHandler },	//need to change
		{ USER_ACCESS_PAGE, 				    HandleUserAcessFunctions },
		{ USER_ACCESS_PAGE1, 					HandleUserAcessFunctions },
		{ SCHEDULED_FILL_REMINDER_PAGE, 		HandleScheduledFillRemPageKeyFunctions },
		{ MAINTENENCE_REMINDER_PAGE, 			HandleMaintenencePageKeyFunctions },
		{ ADD_USER_PAGE, 						HandleAddUserKeyFunctions },
		{ LOGIN1_PAGE, 						    HandleUserLoginFunctions },
		{ LOGIN2_PAGE, 						    HandleUserLoginFunctions },
		{ LOGIN3_PAGE, 						    HandleUserLoginFunctions },
		{ LOGOUT_PAGE, 						    HandleUserLoginFunctions },
		{ LED_CTRL_PAGE, 						    HandleLEDControlFunctions },			//UNUSED
		{ BLUETOOTH_PAGE, 						DefaultTouchKeyHandler },
		{ MODBUS_PHASETWO_PAGE, 				HandleModbusPhaseTwoSettingsKeyFunctions },
		{ MODBUS_CRYOLINK_PAGE, 				HandleModbusCryolinkSettingsKeyFunctions },
		{ ONNECTED_DEVICES_PAGE, 						HandleGroupFunctions },
		{ WIFI_PAGE, 							HandleWIFISettingsKeyFunctions },
		{ WIFI_SCAN_PAGE, 					    HandleWIFISettingsKeyFunctions },
		{ WIFI_SCAN_PAGE2, 					    HandleWIFISettingsKeyFunctions },
		{ IOT_PAGE, 							HandleAWSFunctions },//iot
		{ SYSTEM_SETTINGS_PAGE,					HandleSyatemsettingsKeyFunctions},
		{ FIRMWARE_UPDATE_PAGE, 					DefaultTouchKeyHandler },		//UNUSED
		{ USB_PAGE, 						    HandleUSBSHIFTFunctions },
		{ FIRM_DECOMPRESS_PAGE, 					DefaultTouchKeyHandler },
		{ UPDATE_PAGE, 				            HandleFirmwareupdateFunctions },
		{ UPDATE_TIME_PAGE, 					HandleFirmwareupdatetimeFunctions },
		{ FIRM_UPDATING_PAGE,						DefaultTouchKeyHandler },
		{ FILE_EXPLORER1_PAGE, 					HandleFileExplorerFunctions },
		{ FILE_EXPLORER2_PAGE, 				    HandleFileExplorerFunctions },
		{ FILE_EXPLORER3_PAGE, 					DefaultTouchKeyHandler},
		{ CLR_SELECT_PAGE, 					HandleSelectCOLORFunctions},
		{ FACTORY_PAGE, 						HandleSyatemsettingsKeyFunctions },
		{ SCREEN_SAVER_PAGE, 					HandleScreensaverFunctions },
		{ SHUTDOWN_PAGE, 						DefaultTouchKeyHandler },
		{ LOADING_PAGE, 						DefaultTouchKeyHandler },
		{ Graph, 					            HandleGraphFunctions },
		{ ERROR_NOTIFICATION, 					HandleError1PageKeyFunctions},
		{ ERROR_POPUP_GREEN_PAGE, 					HandleError3PageKeyFunctions},
		{ ERROR_POPUP_RED_PAGE, 					HandleError3PageKeyFunctions},
		{ ERROR_POPUP_RED_NOESC_PAGE, 					DefaultTouchKeyHandler},
		{ NETWORK_PAGE, 						HandleNetworkKeyFunctions },
		{ BLE_CONN_MANAGE_PAGE, 					HandleBLEConnectionKeyHandler},
		{ WIFI_CONN_MANAGE_PAGE, 						HandleHomeScreenKeyFunctions },
		{T1_ONLY_TEMPERATURE,					DefaultTouchKeyHandler},


		{ ERROR1_PAGE, 							HandleError3PageKeyFunctions },
		{ MAX_PAGE_IDS, 						DefaultTouchKeyHandler }

};

const uint16_t MaxUIDataElements = sizeof(UIDataHandlerList)/sizeof(UIDataHandlerList[0]);

/* Function Declarations start here */
/*
 * *****************************************************************************
 * Function name: HandleUIDataEntryEvent
 * Created by : Venkatesh Arikanti
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
void HandleUIDataEntryEvent(uint8_t CmdCode, uint8_t *DataPtr, uint16_t Size)
{
	uint32_t VPAddr = ((uint32_t)DataPtr[4]) << 24;
	VPAddr |= ((uint32_t)DataPtr[5]) << 16;
	VPAddr |= ((uint32_t)DataPtr[6]) << 8;
	VPAddr |= ((uint32_t)DataPtr[7]);
	if((VPAddr==0x0807CA)||(VPAddr==0x0807CC)||(VPAddr==0x080158) ||(VPAddr==0x08015E))
	{
		Time_home_ret=1;
	}
	else
	{
		Time_home_ret=0;
	}
	for(uint16_t Ctr = 0; Ctr < MaxUIDataElements; Ctr++)
	{
		if( (VPAddr - 0x500) == UIDataHandlerList[Ctr].VPAddr)
		{

			int16_t ParamValue = (*(DataPtr+8) << 8) | *(DataPtr+9);
			if(((ParamValue < UIDataHandlerList[Ctr].LowLimit || ParamValue > UIDataHandlerList[Ctr].HighLimit))&&(UIDataHandlerList[Ctr].HighLimit!=0))
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
			}
			else
			{
				if(UIDataHandlerList[Ctr].DataInPtr)
				{
					*(UIDataHandlerList[Ctr].DataInPtr) = ParamValue;
				}
				SendN16DataToDisplay(VPAddr-0x500, ParamValue);
				//Update_UIandSTM_Variables_Fn(VPAddr,ParamValue);
			}
			if(UIDataHandlerList[Ctr].UIDataHandlerFunc)
			{
				(*UIDataHandlerList[Ctr].UIDataHandlerFunc)(VPAddr, UIDataHandlerList[Ctr].DataInPtr, DataPtr, Size);
			}
			break;
		}
	}
}


/*
 * *****************************************************************************
 * Function name: HandleTouchKeyPressEvent
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  :  Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleTouchKeyPressEvent(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId==15)||(PageId==73)||(PageId==16))
	{
		Time_home_ret=1;
	}
	else
	{
		Time_home_ret=0;
	}
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		if(TouchKeyHandlerList[PageId].TouchKeyHandlerFunc)
		{
			(*TouchKeyHandlerList[PageId].TouchKeyHandlerFunc)(CmdCode, PageId, KeyId);
		}
	}
}

/*
 * *****************************************************************************
 * Function name: LoadErrorScreen
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Load error screen on touch key event
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoadErrorScreen(void)
{

	UpdateErrorScreen = 0;
	if((!GetTotalUIItemIndex())&& Error_flag_en)
	{
	  TargetPage = 0;
	  SendCmdToDisplay(LOAD_PAGE_CMD);
	  return;
	}
	HeadErrId = GetListItemsInOrder(1);
	currentIndexvalue=1;
	//uint8_t MsgLen = GetErrorMessage(HeadErrId,ErrMessage);
	GetErrorMessage(HeadErrId,ErrMessage);
	//SendN16DataToDisplay(0x0800C2, 1);		//Not used now
	SendN16DataToDisplay(ERRCNT_CURRENT_INDEX, currentIndexvalue);
	SendN16DataToDisplay(ERRTOTAL_UIINDEX,GetTotalUIItemIndex());
	SendSTRDataToDisplay(ERR_TEXT_ADDRS, ErrMessage);
	SendN16DataToDisplay(ERRNAV_LEFT_RED_EN, 0);
	if(GetTotalUIItemIndex()>1)
	SendN16DataToDisplay(ERRNAV_RIGHT_RED_EN, 1);
	else
	SendN16DataToDisplay(ERRNAV_RIGHT_RED_EN, 0);
}

void HandleScreensaverFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MUTE_KEY:
//				SD_info.init_done = 1;
				break;
		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleHomeScreenKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
unsigned char defog_key_sence;
unsigned char stop_key_sence;

void HandleHomeScreenKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	Home_page();
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		Error_flag_en=0;
		switch(KeyId)
		{
			case MENU_HOME_KEY:
//				HAL_GPIO_TogglePin(GPIOA, GPIO_ODR_OD7);
				if((vp_rt_wr_str.LoginStatus==0)&&(login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255))
				{

					TargetPage =25;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					TargetPage =1;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;
			case ENTER_KEY://wifi selection
				if((vp_rt_wr_str.LoginStatus==0)&&((login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255)))
				{
					TargetPage =25;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					SendN16DataToDisplay(WiFi_VERTUAL_KEY_VP_ADDRESS,1);//need to come back  080430 -> old address
				}
				break;
			case CANCEL_KEY://BLE selection
				if((vp_rt_wr_str.LoginStatus==0)&&((login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255)))
				{
					TargetPage =25;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					SendN16DataToDisplay(BLE_VER_KEYVP_ADD,1);
				}
				break;

			case BACK_KEY://modbus status
				if((vp_rt_wr_str.LoginStatus==0)&&(login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255))
				{
					TargetPage =25;
					SendCmdToDisplay(LOAD_PAGE_CMD);

				}
				else
				{
					SendN16DataToDisplay(MODBUS_VER_KEY_ADD,1);
				}
				break;

			case TEMP_SETTINGS_KEY: //graph id
			/*	if((vp_rt_wr_str.LoginStatus==0)&&(login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255))
				{
					TargetPage =14;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					SendN16DataToDisplay(0x089560, 1);
					SendN16DataToDisplay(0x080358, 1);
					SendN16DataToDisplay(0x08035A, 0);
					DayGraphloaded = 0;
					DayGraphActive = 0;
					DayGraphDateSelection.Date = Date.Date;
					DayGraphDateSelection.Month = Date.Month;
					DayGraphDateSelection.Year = Date.Year;
					GetPastDate(&DayGraphDateSelection, 1);
					DayGraphScaleSelection = DAY_24_HR_GRAPH;
					DayGraphDate=DayGraphDateSelection.Date;
					DayGraphMonth = DayGraphDateSelection.Month;
					DayGraphYear = DayGraphDateSelection.Year;

					SendN16DataToDisplay(GRAPH_DAY_SCALE_SELECT_ADDRESS_WR, (uint16_t)DayGraphScaleSelection);
					SendN16DataToDisplay(GRAPH_DAY_DATE_ADDRESS_WR, (uint16_t)DayGraphDateSelection.Date);
					SendN16DataToDisplay(GRAPH_DAY_MONTH_ADDRESS_WR, (uint16_t)DayGraphDateSelection.Month);
					SendN16DataToDisplay(GRAPH_DAY_YEAR_ADDRESS_WR, (uint16_t)DayGraphDateSelection.Year);

					//SendN16DataToDisplay(0x080560, 0);
					TargetPage =30;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					GraphPageActive = 1;
					SendGraphData();
				}*/
				break;

			case LEVEL_SETTINGS_KEY:// error_log
				Error_flag_en=1;
				LoadErrorScreen();
			break;

			case SECURIY_KEY: //fill
//				if((vp_rt_wr_str.LoginStatus==0)&& vp_rt_wr_str.lid_info.lid_sts==0 &&(login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255))
//				{
//					TargetPage =25;
//					SendCmdToDisplay(LOAD_PAGE_CMD);
//				}
//				else
//				{
//				}

				break;

			case DELETION_KEY://check_login

				if((login_cred[1].admin_id[0] ==0) || (login_cred[1].admin_id[0] ==255))
				{
					UIAddUserStruct.select = 1;
					SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)"\0");
					SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
					SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)"\0");
					SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
					memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
					SendN16DataToDisplay(ADMIN_OR_USER_WR,UIAddUserStruct.select);
					TargetPage = 24;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else if(vp_rt_wr_str.LoginStatus)
				{
					TargetPage = 28;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					TargetPage = 25;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					memset((char*)UIlogin.login_user, 0x00, MAX_ID_SIZE);
					Login_id_cnt=0;
				}
				break;

//			case Log_Id:
//				if((vp_rt_wr_str.LoginStatus==0)&&(login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255))
//				{
//					TargetPage =25;
//					SendCmdToDisplay(LOAD_PAGE_CMD);
//				}
//				else
//				{
//					TargetPage =31;
//					SendCmdToDisplay(LOAD_PAGE_CMD);
//				}
//				break;


			case WIFI_NAVIGATION_KEY://wifi_navigating_page
				if((vp_rt_wr_str.LoginStatus==2)||((login_cred[1].admin_id[0] ==0)||(login_cred[1].admin_id[0] ==255)))
				{
					TargetPage =34;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					LoadWifiUserSettings();
					SendN16DataToDisplay(Wifi_refresh_en,1);
					if(vp_rt_wr_str.WiFiStatus==1)
					{
						 SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,1);//need explaination
						 SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR,0);
					}

				}
				else
				{
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"login as admin!!!");
				}
				break;
			case CALI_1P_KEY://Quick acces file download and pick atime
				if((vp_rt_wr_str.LoginStatus==2)||((login_cred[1].admin_id[0] ==0)||(login_cred[1].admin_id[0] ==255)))
				{
//					if((Control_versions.IOT_File_status==0) ||(Control_versions.IOT_File_status==1))
//					{
//						TargetPage =40;
//						SendCmdToDisplay(LOAD_PAGE_CMD);
//						LoadFreezerId(&UIFreezerStruct);
//					}
//					else if(Control_versions.IOT_File_status)
//					{
//						   TargetPage = 0;
//						   SendCmdToDisplay(LOAD_PAGE_CMD);
//						   SendN16DataToDisplay(UPDATENOW_PICATIME_VKEY, 1);//update and pick
//						   SendN16DataToDisplay(UPDATE_FIRMWARE_CANCEL, 0);
//						   SendVPDataCmdToDisplay(firmware_time_clean);
//						   SendVPDataCmdToDisplay(firmware_time_clean1);
//					}
				}
				else
				{
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"login as admin!!!");
				}

				break;
		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleNetwork
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleNetworkKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case REMOTE:		//Remote
				if((vp_rt_wr_str.MBStatus == 0) || (vp_rt_wr_str.MBStatus == 2))
				{
//					fill_mon_info.ofaf_cmd=0;//remote
					if(0)//vp_param_rw_str.STM_EN_4_20_mA)
					{
						vp_rt_wr_str.MBStatus=3;
					}
					else
					{
						vp_rt_wr_str.MBStatus=1;
					}
					SendN16DataToDisplay(Remote_local,vp_rt_wr_str.MBStatus);
					SendN16DataToDisplay(MODBUS_VER_KEY_ADD,0);
					SendN16DataToDisplay(MODBUS_SLAVE_EN,0);
					repeat_local=1;
					if(vp_param_rw_str.DataRead_En)
					{
						vp_param_rw_str.DataRead_En =0;
					//	vp_param_rw_str.Fill_cmd=0;
						LoadPhaseSettings(&UIModbusData);
					}
				}
//				for(int i=0;i<MB_MAX_NODES;i++)
//				{
//				  if(GroupStruct.Group_col[i]==0)
//				  {
//					  node_info[i].Group_inactive=0;
//				  }
//				}
				modbus_timer=0;
//				Set_delay(SW_TMR_4_20mACOMM_POLL, 90*667, (uint32_t*)&RS485_info.mA_communication);
				break;
			case LOCAL:		//Local
//				fill_mon_info.ofaf_cmd=1;//local
				if(0)//vp_param_rw_str.STM_EN_4_20_mA)
			    {
					vp_rt_wr_str.MBStatus=2;
			    }
			    else
			    {
			    	vp_rt_wr_str.MBStatus=0;
			    }
				SendN16DataToDisplay(Remote_local,vp_rt_wr_str.MBStatus);
				if(vp_param_rw_str.mb_cfg.node_id)
				{
					SendN16DataToDisplay(MODBUS_SLAVE_EN,1);
				}
				SendN16DataToDisplay(MODBUS_VER_KEY_ADD,0);
//				for(int i=0;i<MB_MAX_NODES;i++)
//				{
//					  node_info[i].Group_inactive=0;
//				}
//				Set_delay(SW_TMR_4_20mACOMM_POLL, 90*667, (uint32_t*)&RS485_info.mA_communication);
				break;
		}
//		if(GroupStruct.Group_en)
//		{
//			SendVPDataCmdToDisplay(GROUP_INACTIVE_NODES);
//		}
		update_error=1;
		EEPROM_Write(modbus_com, 0,(uint8_t*)&vp_rt_wr_str.MBStatus,1);
	}
}
/*
 * *****************************************************************************
 * Function name: HandlePopupScreenKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:       14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandlePopupScreenKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				if((vp_rt_wr_str.LoginStatus==0)&&(login_cred[1].admin_id[0] !=0)&&(login_cred[1].admin_id[0] !=255))
				{
					TargetPage =14;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					TargetPage =1;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;



			case 33:
				if(vp_rt_wr_str.LoginStatus)
				{
					TargetPage = 35;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					TargetPage = 14;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				break;

		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleMenuScreenKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleMenuScreenKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				break;
			case BACK_KEY:
				break;
			case TEMP_SETTINGS_KEY:
				if((vp_param_rw_str.plumbing_options & TEMP_SENS_SEL))
				{
					TargetPage =61;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				else
				{
					TargetPage =2;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				LoadTempSettings(&UITempeSettingsData);
				break;
			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case LEVEL_SETTINGS_KEY:	//Level
				LoadLevelSettings(&UILevelSettingsData);
				break;
			case DELETION_KEY:			//weight
				break;
			case VACCUM_KEY:			//Vaccum
				break;
			case SETTINGS_KEY:
				if(vp_cal_pt_rd_str.Calibration_time)
				{
					SendN16DataToDisplay(CALIBRATION_GRAY_EN,1);

				}
				else
				{
					SendN16DataToDisplay(CALIBRATION_GRAY_EN,0);

				}
				break;

			case SECURIY_KEY: //security
				LoadFreezerId(&UIFreezerStruct);
				break;

		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleCalibSensorSelectFunctions
 * Created by : Venkatesh Arikanti
 * Date:        27-Apr-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleCalibSensorSelectFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
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
				break;

			case CANCEL_KEY:
				break;

			case BACK_KEY:
				break;

			case TEMP_SETTINGS_KEY:			//CALI_RTD1_KEY
				vp_cal_pt_rd_str.param_under_calib = CALIB_PARAM_RTD1;

				break;

			case LEVEL_SETTINGS_KEY:		//CALI_RTD2_KEY
				vp_cal_pt_rd_str.param_under_calib = CALIB_PARAM_RTD2;
				break;

//			case LOGIN:
//				vp_cal_pt_rd_str.param_under_calib = CALIB_PARAM_DP;
//				calibration_canceled_flg=0;
//				SendN16DataToDisplay(CALIB_START_STOP_CONTROL_RD, 0);
//				SendN16DataToDisplay(CALIB_START_STOP_CONTROL_WR, 0);
//				vp_cal_pt_rd_str.IsSinglePointCalibration = 1;
//
//				UISensorCalibData.DP2PointValue1 = 0;
//				UISensorCalibData.DP2PointValue2 = 0;
//				UISensorCalibData.DP1PointValue = 0;
//				SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
//				SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x00);//Below line disable
//				SendN16DataToDisplay(RTD_1POINT_EN_TCHADDRS, 0x01);//Above line enable
//				vp_cal_pt_rd_str.step_num = 1;
//				vp_cal_pt_rd_str.step_ack = 0;
//				vp_cal_pt_rd_str.WaitSamples = 0;
//				vp_cal_pt_rd_str.WaitForInput = 0;
//				vp_cal_pt_rd_str.status = CALIB_ACTIVE;
//				//RegisterAlarm(CAL_COME_OUT, &vp_cal_pt_rd_str.calibration_stop,30*60, SYNC_ON_REGISTER);
//				cal_info.state = CAL_STEP2;
//
//				break;

			case DELETION_KEY:			//VACUUM CALIBRATION key
				break;
			case VACCUM_KEY:						//WEIGHT_KEY
				break;


		}
	}
}


void HandleGraphFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		/*switch(KeyId)
		{
			case MENU_HOME_KEY:
				DayGraphActive = 0;
				GraphPageActive = 0;
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case ENTER_KEY:
				LoadErrorScreen();
				break;

			case CANCEL_KEY:
				break;

			case BACK_KEY:

				break;
			case REGISTER:
				SendN16DataToDisplay(0x080358,0);
				SendN16DataToDisplay(0x08035A,1);
				break;
			case LEVEL_SETTINGS_KEY:
				SendN16DataToDisplay(0x080358,1);
				SendN16DataToDisplay(0x08035A,0);
				break;

		}*/
	}
}
/*
 * *****************************************************************************
 * Function name: HandleRTDCalib1POr2PSelectFunctions
 * Created by : Venkatesh Arikanti
 * Date:        27-Apr-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleRTDCalib1POr2PSelectFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				vp_cal_pt_rd_str.step_num = 0;
				vp_cal_pt_rd_str.step_ack = 0;
				vp_cal_pt_rd_str.status = CALIB_INACTIVE;
				vp_cal_pt_rd_str.WaitForInput = 0;
				cal_info.state = CAL_STOP;
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case ENTER_KEY:
			    break;

			case CANCEL_KEY:
				break;

			case BACK_KEY:
				vp_cal_pt_rd_str.step_num = 0;
				vp_cal_pt_rd_str.step_ack = 0;
				vp_cal_pt_rd_str.status = CALIB_INACTIVE;
				vp_cal_pt_rd_str.WaitForInput = 0;
				cal_info.state = CAL_STOP;
				break;

			case CALI_1P_KEY://Single point
				calibration_canceled_flg=0;
				vp_cal_pt_rd_str.IsSinglePointCalibration = 1;

				UISensorCalibData.DP2PointValue1 = 0;
				UISensorCalibData.DP2PointValue2 = 0;

				SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
				SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x00);//Below line disable
				SendN16DataToDisplay(RTD_1POINT_EN_TCHADDRS, 0x01);//Above line enable

			//	SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x00);//Below line disable
				SendN16DataToDisplay(RTD_2POINT_STR_ADDRS, 0x00);		//RTD_2POINT_STR_ADDRS

				vp_cal_pt_rd_str.step_num = 1;
				vp_cal_pt_rd_str.step_ack = 0;
				vp_cal_pt_rd_str.WaitSamples = 0;
				vp_cal_pt_rd_str.WaitForInput = 0;
				vp_cal_pt_rd_str.status = CALIB_ACTIVE;
//				RegisterAlarm(CAL_COME_OUT, &vp_cal_pt_rd_str.calibration_stop,30*60, SYNC_ON_REGISTER);
				cal_info.state = CAL_STEP2;
				break;

			case CALI_2P_KEY://2 Point
				calibration_canceled_flg=0;
				SendN16DataToDisplay(RTD_1POINT_EN_TCHADDRS, 0x01);//Above line enable
				vp_cal_pt_rd_str.IsSinglePointCalibration = 0;

				UISensorCalibData.DP2PointValue1 = 0;
				UISensorCalibData.DP2PointValue2 = 0;
				SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
				SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x00);//Below line disable
				SendN16DataToDisplay(RTD_2POINT_STR_ADDRS, 0x01);
				cal_info.state = CAL_START;
//				RegisterAlarm(CAL_COME_OUT, &vp_cal_pt_rd_str.calibration_stop,30*60, SYNC_ON_REGISTER);
				break;


		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleAlarmSettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        27-Apr-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleAlarmSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadAlarmSettings(&UIAlarmSettingsData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case ENTER_KEY:
				SaveAlarmSettings(&UIAlarmSettingsData);
				break;

			case CANCEL_KEY:
				LoadAlarmSettings(&UIAlarmSettingsData);
				break;

			case BACK_KEY:
				LoadAlarmSettings(&UIAlarmSettingsData);
				break;
		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleSystemSettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        16-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleSystemSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
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
				break;

			case CANCEL_KEY:
				break;

			case BACK_KEY:
				break;
			case LEVEL_SETTINGS_KEY://calibration
				TargetPage = 14;
				SendCmdToDisplay(LOAD_PAGE_CMD);
				if(vp_cal_pt_rd_str.RTD1_calibration_time)
				{
					SendN16DataToDisplay(RTD1_CALIBRATION_GRAY_EN,1);
			
				}
				else
				{
					SendN16DataToDisplay(RTD1_CALIBRATION_GRAY_EN,0);
			
				}
				if(vp_cal_pt_rd_str.RTD2_calibration_time && vp_param_rw_str.t2enable)
				{
					SendN16DataToDisplay(RTD2_CALIBRATION_GRAY_EN,1);
			
				}
				else
				{
					SendN16DataToDisplay(RTD2_CALIBRATION_GRAY_EN,0);
			
				}
				break;

			case VACCUM_KEY://scheuleevents
				LoadScheduledSettings(&UIScheduledEventData);
				break;
			case TEMP_SETTINGS_KEY://display
				LoadDispSettings(&UIDisplayData);
				break;
			case DELETION_KEY://Alarm_settings
				LoadAlarmSettings(&UIAlarmSettingsData);
				 break;
		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleDisplaySettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        27-Apr-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleDisplaySettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadDispSettings(&UIDisplayData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case ENTER_KEY:
				SaveDispSettings(&UIDisplayData);
				break;

			case CANCEL_KEY:
				LoadDispSettings(&UIDisplayData);
				break;

			case BACK_KEY:
				LoadDispSettings(&UIDisplayData);
				break;

			case DATE_TIME_KEY://Date and Time Key
				LoadRTCSettings(&UIDateTimeSettings);
				break;

			case SYSTEM_SETTINGS_KEY://system info
				LoadSystemSettings(&UIUpdateSystemSettings);
				break;

			case CALI_1P_KEY://LED colour selection
				LoadLedSettings(&UILEDSettingsData);
				break;

		}
	}
}
void HandleBLEConnectionKeyHandler(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{

	}
}


/*
 * *****************************************************************************
 * Function name: HandleWIFISettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        19-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleWIFISettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadWifiUserSettings();
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;


			case VACCUM_KEY://wifi_scan
				if (vp_rt_wr_str.WiFiStatus==2)
				{
					wifi_info.Prevstate=MQTT_CONNECTED;
				}
				wifi_info.state =WIFI_SCAN;
			    wifi_info.UART1ErrorDetected = 1;
				flag = 0;
				SendN16DataToDisplay(Wifi_refresh_en,0);
				break;

			case DELETION_KEY:
				wifi_info.state = WIFI_STRENGTH;
				break;

			case ENTER_KEY:
				SaveWifiUserSettings();
				break;

			case CANCEL_KEY:
				LoadWifiUserSettings();
				break;

			case BACK_KEY:
				LoadWifiUserSettings();
				break;
			case CALI_1P_KEY:
				break;
			case REGISTER://scan1
				Checking_ssid(0);
				break;
			case LID_SETTINGS_KEY://scan2
				Checking_ssid(1);
				break;
			case LOGOUT_YES://scan3
				Checking_ssid(2);

				break;
			case SCHEDULED_CLEVEL_KEY://scan4
				Checking_ssid(3);
				break;
			case SCHEDULED_MAINT_KEY://scan5
				Checking_ssid(4);

				break;
			case LEFT_NAV_KEY://scan6
				Checking_ssid(5);

				break;
			case RIGHT_NAV_KEY://scan7
				Checking_ssid(6);

				break;
			case MODBUS_PHASE2_KEY://scan8
				Checking_ssid(7);
				break;
			case SCAN9://scan9
				Checking_ssid(8);

				break;
			case SCAN10://scan10
				Checking_ssid(9);

				break;
			case SYSTEM_SETTINGS_KEY://left arrow for scan
				SendN16DataToDisplay(WIFI_NAVIGATION_EN,2);
				break;
			case LEVEL_SETTINGS_KEY://right_arrow for scan
				SendN16DataToDisplay(WIFI_NAVIGATION_EN,1);
				break;
		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleModbusSettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        27-Apr-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleModbusCryolinkSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadCryoSettings(&UIModbusData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//TBD
				break;

			case ENTER_KEY:
				SaveCryoSettings(&UIModbusData);
				break;

			case CANCEL_KEY:
				LoadCryoSettings(&UIModbusData);
				break;

			case BACK_KEY:
				LoadCryoSettings(&UIModbusData);
				break;

			case MODBUS_PHASE2_KEY:
				LoadPhaseSettings(&UIModbusData);
				break;

		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleModbusPhaseTwoSettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        27-Apr-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleModbusPhaseTwoSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadPhaseSettings(&UIModbusData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SavePhaseSettings(&UIModbusData);
				break;

			case CANCEL_KEY:
				LoadPhaseSettings(&UIModbusData);
				break;

			case BACK_KEY:
				LoadPhaseSettings(&UIModbusData);
				break;
			case LOGIN://4_20mA
				//Load420mASettings(&UI_4_20mAsettings);
				break;

//			case RIGHT_NAV_KEY:
//				LoadCryoSettings(&UIModbusData);
//				break;

			case 9: //Group selection page
				UIUpdateGroupsettings.Group_sel = 1;
				LoadGROUPSettings(&UIUpdateGroupsettings );
				break;

		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleTempSettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleTempSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadTempSettings(&UITempeSettingsData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveTempSettings(&UITempeSettingsData);
//				if(usb_info.state != USB_INIT)
//					usb_info.state=USB_DE_INIT;//should not do deinit without init
//				usb_info.new_mode=USB_HOST;
				break;

			case CANCEL_KEY:
				LoadTempSettings(&UITempeSettingsData);
				break;

			case BACK_KEY:
				LoadTempSettings(&UITempeSettingsData);
				break;

		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleUserLoginFunctions
 * Created by : Arikanti venkatesh
 * Date:        13-JULY-2023
 * Description:
 * Main function to login icon and logout icon
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleUserLoginFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t *)"\0");
				SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t *)"\0");
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case BACK_KEY:
				Account_creation=0;
				SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t *)"\0");
				SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t *)"\0");
				memset((char*)UIlogin.login_user, 0x00, MAX_ID_SIZE);
				break;
			case NEXT:				//NEXT
				if(UIlogin.login_user[0]==0)
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter valid ID!!!");

				}
				else if((login_cred[Login_id_cnt].user_pswd[0]==0) || (login_cred[Login_id_cnt].user_pswd[0]==255))
				{
					TargetPage = 27;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 0);
					SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 0);
					//attempt=0;
					memset(UIlogin.conform_pw, 0, sizeof(UIlogin.conform_pw));
					memset(UIlogin.new_pw, 0, sizeof(UIlogin.new_pw));
					SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
					SendN16DataToDisplay(FIRST_USER_EN, 1);
				}
				else
				{
					memset((char*)UIlogin.login_pw, 0x00, MAX_ID_SIZE);
					if((login_cred[Login_id_cnt].user_pswd[0]=='*'))
					{
						SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 1);
					}
					else
					{
						TargetPage = 26;
						SendCmdToDisplay(LOAD_PAGE_CMD);
						SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 0);	//Account mismatch
						SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 0);	//Account Locked
						//attempt=0;
						SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
					}
				}

                break;

			case LOGIN:				//LOGIN
				Loginfunction();
				break;

			case REGISTER:									//REGISTER
				SavePasswordSettings(&UIlogin);
				break;
			case LOGOUT_YES:	//LOGOUT_YES
				Logoutfunction();
				break;
		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleLevelSettingsKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleLevelSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadLevelSettings(&UILevelSettingsData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveLevelSettings(&UILevelSettingsData);
				break;

			case CANCEL_KEY:
				LoadLevelSettings(&UILevelSettingsData);
				break;

			case BACK_KEY:
				LoadLevelSettings(&UILevelSettingsData);
				break;

		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleScheduledPageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        09-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleScheduledPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadScheduledSettings(&UIScheduledEventData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveScheduledSettings(&UIScheduledEventData);
				break;

			case CANCEL_KEY:
				LoadScheduledSettings(&UIScheduledEventData);
				break;

			case BACK_KEY:
				LoadScheduledSettings(&UIScheduledEventData);
				break;

			case LEVEL_SETTINGS_KEY:	// schedule Manual Fill
				LoadScheduledFillSettings(&UIScheduledFillData);
				break;
			case DELETION_KEY:	// Fill Remainder
				LoadScheduledFillRemSettings(&UIFillRemainderData);
				break;
			case VACCUM_KEY:	// Maintainance
				LoadMaintenenceSettings(&UIMaintenanceRemainderSettings);
				break;
//			case LEFT_NAV_KEY:
//				LoadScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//				break;


		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleSystemInfoPageKeyFunctions
 * Created by : Arikanti Venkatesh
 * Date:        10-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleSystemInfoPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadSystemSettings(&UIUpdateSystemSettings);
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;

			case ENTER_KEY:
				SaveSystemSettings(&UIUpdateSystemSettings);
				break;

			case CANCEL_KEY:
				LoadSystemSettings(&UIUpdateSystemSettings);
				break;

			case BACK_KEY:
				LoadSystemSettings(&UIUpdateSystemSettings);
				break;


		}
	}
}

//rv
/*
 * *****************************************************************************
 * Function name: HandleScheduledFillPageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        10-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleScheduledFillPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadScheduledFillSettings(&UIScheduledFillData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveScheduledFillSettings(&UIScheduledFillData);
				break;

			case CANCEL_KEY:
				LoadScheduledFillSettings(&UIScheduledFillData);
				break;

			case BACK_KEY:
				LoadScheduledFillSettings(&UIScheduledFillData);
				break;

		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleMaintenencePageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        12-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleMaintenencePageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadMaintenenceSettings(&UIMaintenanceRemainderSettings);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveMaintenenceSettings(&UIMaintenanceRemainderSettings);
				break;

			case CANCEL_KEY:
				LoadMaintenenceSettings(&UIMaintenanceRemainderSettings);
				break;

			case BACK_KEY:
				LoadMaintenenceSettings(&UIMaintenanceRemainderSettings);
				break;


		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleRTCPageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        13-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleRTCPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadRTCSettings(&UIDateTimeSettings);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveRTCSettings(&UIDateTimeSettings);
				break;

			case CANCEL_KEY:
				LoadRTCSettings(&UIDateTimeSettings);
				break;

			case BACK_KEY:
				LoadRTCSettings(&UIDateTimeSettings);
				break;


		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleScheduledPageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        09-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
//void HandleScheduledLN2SupplyPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
//{
//	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
//	{
//		switch(KeyId)
//		{
//			case MENU_HOME_KEY:
//				LoadScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//				break;
//
//			case MUTE_KEY:
//				mutebuzzer();
//				//Process Key Action
//				//TBD
//				break;
//
//			case ENTER_KEY:
//				SaveScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//				break;
//
//			case CANCEL_KEY:
//				LoadScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//				break;
//
//			case BACK_KEY:
//				LoadScheduledLN2SupplySettings(&UILN2SupplyRemainderData);
//				break;
//
//
//		}
//	}
//}
/*
 * *****************************************************************************
 * Function name: HandleScheduledFillRemPageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        09-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleScheduledFillRemPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadScheduledFillRemSettings(&UIFillRemainderData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				SaveScheduledFillRemSettings(&UIFillRemainderData);
				break;

			case CANCEL_KEY:
				LoadScheduledFillRemSettings(&UIFillRemainderData);
				break;

			case BACK_KEY:
				LoadScheduledFillRemSettings(&UIFillRemainderData);
				break;


		}
	}
}
//rv
/*
 * *****************************************************************************
 * Function name: HandleError1PageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleError1PageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	uint8_t ErrorId;

	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case ACK_KEY://ACK Key
				clear_reminder=Reminder_Id;
				if(clear_reminder==11)
				{
					//System_Status[WARNING_FLAG] &= ~WRN_DP_CHECK;
				}
				else if(clear_reminder==17)
				{
					 System_Status[ERROR_FLAG_0] &= ~ERR_BATTERY_BACKUP;
				}
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case LEFT_NAV_KEY:	//Left_Nav_key
				if(UpdateErrorScreen)
				{
					LoadErrorScreen();
				}
				else
				{
					ErrorId = GetNextItemIndex(HeadErrId);
					if(ErrorId < MAX_UI_ERROR_LOGS)
					{
						HeadErrId = ErrorId;
						currentIndexvalue--;
						SendN16DataToDisplay(ERRCNT_CURRENT_INDEX, currentIndexvalue);
					}
					else
					{
						currentIndexvalue=1;
						return;
					}
					if(currentIndexvalue==1)
					SendN16DataToDisplay(ERRNAV_LEFT_RED_EN, 0);
					else
					SendN16DataToDisplay(ERRNAV_LEFT_RED_EN, 1);
					if(currentIndexvalue==GetTotalUIItemIndex())
					SendN16DataToDisplay(ERRNAV_RIGHT_RED_EN, 0);
					else
					SendN16DataToDisplay(ERRNAV_RIGHT_RED_EN, 1);
					GetErrorMessage(HeadErrId,ErrMessage);//MsgLen =
					//SendN16DataToDisplay(0x0800C2, 1);
					SendSTRDataToDisplay(ERR_TEXT_ADDRS, ErrMessage);
				}
				break;

			case RIGHT_NAV_KEY:	//Right_Nav_key
				if(UpdateErrorScreen)
				{
					LoadErrorScreen();
				}
				else
				{
					ErrorId = GetPrevItemIndex(HeadErrId);
					if(ErrorId < MAX_UI_ERROR_LOGS)
					{
						HeadErrId = ErrorId;
						currentIndexvalue++;
						SendN16DataToDisplay(ERRCNT_CURRENT_INDEX, currentIndexvalue);
					}
					else
					{
						currentIndexvalue=GetTotalUIItemIndex();
						return;
					}
					if(currentIndexvalue==GetTotalUIItemIndex())
					SendN16DataToDisplay(ERRNAV_RIGHT_RED_EN, 0);
					else
					SendN16DataToDisplay(ERRNAV_RIGHT_RED_EN, 1);
					if(currentIndexvalue==1)
					SendN16DataToDisplay(ERRNAV_LEFT_RED_EN, 0);
					else
					SendN16DataToDisplay(ERRNAV_LEFT_RED_EN, 1);
					GetErrorMessage(HeadErrId,ErrMessage);
					SendSTRDataToDisplay(ERR_TEXT_ADDRS, ErrMessage);
				}
				break;
			case REMOTE://esc in error page
				Error_flag_en=0;
				break;
		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleError3PageKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleError3PageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{

	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY://close Key
				if(vp_cal_pt_rd_str.status==CALIB_ACTIVE || Account_creation)
				{
					Account_creation=0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				if(TargetPage == 28)
				{

					if(vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD1)
					{
						if(vp_cal_pt_rd_str.RTD1_calibration_time)
						{
							SendN16DataToDisplay(RTD1_CALIBRATION_GRAY_EN,1);
				
						}
						else
						{
							SendN16DataToDisplay(RTD1_CALIBRATION_GRAY_EN,0);
					
						}
					}
					if(vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD2)
					{
						if(vp_cal_pt_rd_str.RTD2_calibration_time && vp_param_rw_str.t2enable)
						{
							SendN16DataToDisplay(RTD2_CALIBRATION_GRAY_EN,1);
				
						}
						else
						{
							SendN16DataToDisplay(RTD2_CALIBRATION_GRAY_EN,0);
				
						}
					}
					Time_home_ret=1;
					UISensorCalibData.DP2PointValue1 = 0;
					UISensorCalibData.DP2PointValue2 = 0;
//					UISensorCalibData.DP1PointValue = 0;
					SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
					SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, UISensorCalibData.DP2PointValue2);
					vp_cal_pt_rd_str.step_num = 0;
//					vp_cal_pt_rd_str.step_ack = 0;
					vp_cal_pt_rd_str.status = CALIB_INACTIVE;
					vp_cal_pt_rd_str.WaitForInput = 0;
					cal_info.state = CAL_STOP;
					calibration_canceled_flg=1;
				}
				/*if(TargetPage == 16)
				{
					 SendN16DataToDisplay(STRING_POPUP_EN, 0);
					TargetPage=50;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					CTRS_cal_var.step_num = 0;
					CTRS_cal_var.step_ack = 0;
					CTRS_cal_var.status = CALIB_INACTIVE;
					CTRS_cal_var.WaitForInput = 0;
					CTRS_cal_var.state=CAL_START;
					SendN16DataToDisplay(0x0890A2, 0x00);
					CTRS_cal_var.status = CALIB_ACTIVE;
				}*/
				break;
			case MUTE_KEY://close Key
				if(vp_cal_pt_rd_str.status==CALIB_ACTIVE)
				{
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				if(TargetPage == 28)
				{

					if(vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD1)
					{
						if(vp_cal_pt_rd_str.RTD1_calibration_time)
						{
							SendN16DataToDisplay(RTD1_CALIBRATION_GRAY_EN,1);
					
						}
						else
						{
							SendN16DataToDisplay(RTD1_CALIBRATION_GRAY_EN,0);
					
						}
					}
					if(vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_RTD2)
					{
						if(vp_cal_pt_rd_str.RTD2_calibration_time && vp_param_rw_str.t2enable)
						{
							SendN16DataToDisplay(RTD2_CALIBRATION_GRAY_EN,1);
					
						}
						else
						{
							SendN16DataToDisplay(RTD2_CALIBRATION_GRAY_EN,0);
				
						}
					}
					Time_home_ret=1;
					UISensorCalibData.DP2PointValue1 = 0;
					UISensorCalibData.DP2PointValue2 = 0;
//					UISensorCalibData.DP1PointValue = 0;
					SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
					SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, UISensorCalibData.DP2PointValue2);
					vp_cal_pt_rd_str.step_num = 0;
//					vp_cal_pt_rd_str.step_ack = 0;
					vp_cal_pt_rd_str.status = CALIB_INACTIVE;
					vp_cal_pt_rd_str.WaitForInput = 0;
					cal_info.state = CAL_STOP;
					calibration_canceled_flg=1;
				}
				break;

			default:
				break;
		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleAddUserKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        28-June-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleAddUserKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
				memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;

			case ENTER_KEY:
				SaveAddUserSettings(&UIAddUserStruct);
				break;

			case CANCEL_KEY:
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
				memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
				break;

			case BACK_KEY:
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
				memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
				break;


		}
	}
}
/*
 * *****************************************************************************
 * Function name: HandleUserAcessFunctions
 * Created by : Arikanti venkatesh
 * Date:        11-JULY-2023
 * Description:
 *  Main function to access key icon's
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleUserAcessFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				break;

			case MUTE_KEY:
				mutebuzzer();
				//Process Key Action
				//TBD
				break;

			case ENTER_KEY:
				//SaveLevelSettings(&UILevelSettingsData);
				break;

			case CANCEL_KEY:
				//LoadLevelSettings(&UILevelSettingsData);
				break;
			case LEVEL_SETTINGS_KEY:		//PLUS KEY
				Account_creation=1;
				break;
			case BACK_KEY:
				Account_creation=0;
				if(vp_rt_wr_str.LoginStatus==0)
				{
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				break;
			case TEMP_SETTINGS_KEY:		//LEFT_NAV_KEY
				break;

			case DELETION_KEY:		//DELETION_KEY
				deleteUser(&UIUserAccess);
				break;

			case VACCUM_KEY:		//RGHT_NAV_KEY
				break;

		}
	}
}

/*
 * *****************************************************************************
 * Function name: HandleSecurityKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        19-May-2023
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleSecurityKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				LoadFreezerId(&UIFreezerStruct);
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;


			case DELETION_KEY://Freezer network
				if(1)//((vp_param_rw_str.plumbing_options & MB_SINTESY_MODE))
				{
					TargetPage =31;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					LoadPhaseSettings(&UIModbusData);
				}
				else
				{
					TargetPage =48;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					LoadPhaseSettings(&UIModbusData);
					update_error=1;
				}
				break;
			case ENTER_KEY:
				SaveFreezerId(&UIFreezerStruct);
				break;

			case CANCEL_KEY:
				LoadFreezerId(&UIFreezerStruct);
				break;

			case BACK_KEY:
				LoadFreezerId(&UIFreezerStruct);
				break;

			case LEVEL_SETTINGS_KEY: //user access
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
				memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
				SendN16DataToDisplay(ADMIN_OR_USER_WR,UIAddUserStruct.select);
				remove_tick();
				break;

			case VACCUM_KEY:			//wifi settings
				LoadWifiUserSettings();
				SendN16DataToDisplay(Wifi_refresh_en,1);
				if(vp_rt_wr_str.WiFiStatus==1)
				{
					 SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,1);//need explaination
					 SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR,0);
				}
				break;
			case SETTINGS_KEY:			//IOT Settings
				 LoadAWSUserSettings();
				break;
			case SECURIY_KEY:			//Bluetooth key
				break;
			case SYSTEM_SETTINGS_KEY:			//system settigs
				break;

			case NEXT://pick up time
			   TargetPage = 0;
			   SendCmdToDisplay(LOAD_PAGE_CMD);
			   //update_page_flag=1;//need to change
			   SendN16DataToDisplay(UPDATENOW_PICATIME_VKEY, 1);//update and pick
			   SendN16DataToDisplay(UPDATE_FIRMWARE_CANCEL, 0);
			   SendVPDataCmdToDisplay(firmware_time_clean);
			   SendVPDataCmdToDisplay(firmware_time_clean1);
				break;
		}
	}
}

void HandleSyatemsettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;
			case ENTER_KEY:
				break;

			case CANCEL_KEY:
				break;

			case BACK_KEY:
				break;

			case LEVEL_SETTINGS_KEY: //Firmware Update(USB)
				SendN16DataToDisplay(FIRMWARE_UPDATE_VKEY, 1);
				SendN16DataToDisplay(FW_UPDATE_START_KEY_TOUCH_EN, 1);//Enable usb page
				SendN16DataToDisplay(FW_UPDATE_START_KEY_WR, 0);//Enable usb page
				break;
			case DELETION_KEY://Factory Reset
				//SendN16DataToDisplay(FACTORY_RST_VKEY, 1);
				factory_default();
				break;
			case SYSTEM_SETTINGS_KEY: //Factory reset cancel
				factory_load_delay_flg=0;
				factory_ok_enable_flg=0;
				SendN16DataToDisplay(FACTORY_RST_TCH_EN, 0);//Enable Ok Keys
				SendN16DataToDisplay(FACTORY_RST_VKEY, 0);//Enable Factory index icon
				SendN16DataToDisplay(FACTORY_RST_CNTDWN_EN, 0);//Enable Factory Values Display
				break;
			case REGISTER:	//Factory reset OK
				reset_factory_default();
				break;
			case VACCUM_KEY:
				SendN16DataToDisplay(FIRMWARE_UPDATE_VKEY, 1);
				SendN16DataToDisplay(FW_UPDATE_START_KEY_TOUCH_EN, 1);//Enable usb page
				SendN16DataToDisplay(FW_UPDATE_START_KEY_WR, 0);//Enable usb page
				break;

			case TEMP_SETTINGS_KEY://File explorer
				if(usb_info.state != USB_INIT)
					usb_info.state=USB_DE_INIT;//should not do deinit without init
				usb_info.new_mode=USB_HOST;

				List_Folders("/");
				for (uint8_t y = 0; (y < 12); y++)
				{
					remove_extension_global(dirList[y].name);
					SendSTRDataToDisplay(File_add[y].File_address, (uint8_t*)g_filename_noext);
				}
				SendVPDataCmdToDisplay(FILE_TYPE_EN1);
				SendVPDataCmdToDisplay(FILE_TOUCH_EN);
				SendVPDataCmdToDisplay(FILE_SIZE);
				TargetPage = 45;
				SendCmdToDisplay(LOAD_PAGE_CMD);
				memset(currentPath,0x0,256);
				currentPath[0] = '/';
		}
	}
}

// Remove extension in-place (modifies input string).
//void remove_extension(char *filename) {
//    if (filename == NULL || *filename == '\0') return;
//
//    char *last_dot = strrchr(filename, '.');
//    if (!last_dot) return;
//    if (last_dot == filename) return; // hidden file, keep as-is
//
//    *last_dot = '\0';
//}

void remove_extension_global(const char *filename) {
    if (filename == NULL) {
        g_filename_noext[0] = '\0';
        return;
    }

    // Copy safely into global buffer
    strncpy(g_filename_noext, filename, MAX_NAME_LEN - 1);
    g_filename_noext[MAX_NAME_LEN - 1] = '\0';  // ensure null-termination

    // Find the last dot in the copied name
    char *last_dot = strrchr(g_filename_noext, '.');
    if (!last_dot) return;                   // no dot -> nothing to remove
    if (last_dot == g_filename_noext) return; // ".bashrc" -> keep as-is

    *last_dot = '\0'; // truncate at the dot
}


/*
 * *****************************************************************************
 * Function name: HandleDP2PCalibKeyFunctions
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle the Key touch events
 * Parameters:
 *  Input  : Command code, Page and Key Id's
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HandleDP2PCalibKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				vp_cal_pt_rd_str.step_num = 0;
//				vp_cal_pt_rd_str.step_ack = 0;
				vp_cal_pt_rd_str.status = CALIB_INACTIVE;
				vp_cal_pt_rd_str.WaitForInput = 0;
				cal_info.state = CAL_STOP;
				calibration_canceled_flg=1;
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;

			case ENTER_KEY:
				if(((vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD1) && vp_cal_pt_rd_str.RTD1_calibration_time)|| ((vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD2) && (vp_cal_pt_rd_str.RTD2_calibration_time)))
				{

					if(vp_cal_pt_rd_str.IsSinglePointCalibration)
					{
						if((UISensorCalibData.DP2PointValue2 <50 && UISensorCalibData.DP2PointValue2 >= -2200))
						{
							vp_cal_pt_rd_str.WaitForInput = 1;
							SendN16DataToDisplay(STRING_POPUP_EN, 3);
							SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Calculating Coefficients...!!!");
							TargetPage = PageId;
							UISensorCalibData.DP2PointValue2=0;
							if(vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD1)
							{
								//RegisterAlarm(RTD1_CALIBRATION, &vp_cal_pt_rd_str.RTD1_calibration_time, 30*60, SYNC_ON_REGISTER);
							}
							else if(vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD2)
							{
								//RegisterAlarm(RTD2_CALIBRATION, &vp_cal_pt_rd_str.RTD2_calibration_time, 30*60, SYNC_ON_REGISTER);
							}
						}else
						{
							SendN16DataToDisplay(STRING_POPUP_EN, 1);
							SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
						}

					}
					else if((UISensorCalibData.DP2PointValue2 <250 && UISensorCalibData.DP2PointValue2 >= -2200)&&(UISensorCalibData.DP2PointValue1 < 00 && UISensorCalibData.DP2PointValue1 >= -2200) && cal_info.state==CAL_STEP2)
					{
						vp_cal_pt_rd_str.WaitForInput = 1;
						SendN16DataToDisplay(STRING_POPUP_EN, 3);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Calculating Coefficients...!!!");
						TargetPage = 14;
						UISensorCalibData.DP2PointValue2=0;
						UISensorCalibData.DP2PointValue1=0;
//						if(vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD1)
//						{
//							RegisterAlarm(RTD1_CALIBRATION, &vp_cal_pt_rd_str.RTD1_calibration_time, 30*60, SYNC_ON_REGISTER);
//						}
//						else if(vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD2)
//						{
//							RegisterAlarm(RTD2_CALIBRATION, &vp_cal_pt_rd_str.RTD2_calibration_time, 30*60, SYNC_ON_REGISTER);
//						}
					}
					else
					{
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Values!!!");
					}
				}
				else
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Time ON!!!");
				}

				break;

			case CANCEL_KEY:
				Time_start=0;
				calib_count=60;
				//actuation=1;
				UISensorCalibData.DP2PointValue1 = 0;
				UISensorCalibData.DP2PointValue2 = 0;
//				UISensorCalibData.DP1PointValue = 0;
				SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
				SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, UISensorCalibData.DP2PointValue2);
				vp_cal_pt_rd_str.step_num = 0;
				vp_cal_pt_rd_str.step_ack = 0;
				vp_cal_pt_rd_str.WaitForInput = 0;
				cal_info.state = CAL_START;
				//LoadLevelSettings(&UILevelSettingsData);
				break;

			case BACK_KEY:
				Time_start=0;
				calib_count=60;
				//actuation=1;
				UISensorCalibData.DP2PointValue1 = 0;
				UISensorCalibData.DP2PointValue2 = 0;
				//UISensorCalibData.DP1PointValue = 0;
				SendN16DataToDisplay(DP_2POINT_CAL1_VALUE_WR, UISensorCalibData.DP2PointValue1);
				SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, UISensorCalibData.DP2PointValue2);
				vp_cal_pt_rd_str.step_num = 0;
//				vp_cal_pt_rd_str.step_ack = 0;
				vp_cal_pt_rd_str.status = CALIB_INACTIVE;
				vp_cal_pt_rd_str.WaitForInput = 0;
				cal_info.state = CAL_STOP;
				calibration_canceled_flg=1;
				break;

			case 14:  //Calib Start Key
				vp_cal_pt_rd_str.param_under_calib = CALIB_PARAM_DP;
				break;

		}
	}
}

/*
 * *****************************************************************************
 * Function name: DefaultKeyHandler
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
void DefaultKeyHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	DefaultHandlerRunCtr++;
}

/*
 * *****************************************************************************
 * Function name: TempLimitInputHandler
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
void TempLimitInputHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t TempValue = (*(DataPtr+8) << 8) | *(DataPtr+9);
	if(TempValue < TEMP_LOW_INPUT_LIMT+1 || TempValue > TEMP_HIGH_INPUT_LIMT-1)
	{
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Out of Bound Value!!!");
	}
	else
	{
		*DataInPtr = TempValue;
		SendN16DataToDisplay(VPAddr-0x500, TempValue);
	}
}

/*
 * *****************************************************************************
 * Function name: BinarySwitchHandler
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
void BinarySwitchHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t SwitchValue = *(DataPtr+9);

	*DataInPtr = SwitchValue;
	SendN16DataToDisplay(VPAddr-0x500, SwitchValue);
}


void LanguageHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t LanValue = *(DataInPtr) ;
	if(LanValue==0)
	{
		SendN16DataToDisplay(0x080196,1);
		SendN16DataToDisplay(0x080198,0);
		SendN16DataToDisplay(0x08019A,0);
		SendN16DataToDisplay(0x08019C,0);
	}
	else if(LanValue==1)
	{
		SendN16DataToDisplay(0x080196,0);
		SendN16DataToDisplay(0x080198,1);
		SendN16DataToDisplay(0x08019A,0);
		SendN16DataToDisplay(0x08019C,0);
	}
	else if(LanValue==2)
	{
		SendN16DataToDisplay(0x080196,0);
		SendN16DataToDisplay(0x080198,0);
		SendN16DataToDisplay(0x08019A,1);
		SendN16DataToDisplay(0x08019C,0);
	}
	else if(LanValue==3)
	{
		SendN16DataToDisplay(0x080196,0);
		SendN16DataToDisplay(0x080198,0);
		SendN16DataToDisplay(0x08019A,0);
		SendN16DataToDisplay(0x08019C,1);
	}
}



/*
 * *****************************************************************************
 * Function name: RefreshDayGraph
 * Created by : Venkatesh Arikanti
 * Date:        20-Sept-2023
 * Description:
 *  Function to refresh the day graph page
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 *
void RefreshDayGraph(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	RTC_DateTypeDef SeltdDate;
	if(DayGraphActive)
	{
		SeltdDate.Date = DayGraphDate;
		SeltdDate.Month = DayGraphMonth;
		SeltdDate.Year = DayGraphYear;

		if(IsDateValid(SeltdDate) == -1) //Invalid
		{
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Date is not Valid!!!");

			if(!DayGraphloaded)
			{
				DayGraphActive = 0;
			}
			DayGraphDate=DayGraphDateSelection.Date;
			DayGraphMonth = DayGraphDateSelection.Month;
			DayGraphYear = DayGraphDateSelection.Year;
			SendN16DataToDisplay(GRAPH_DAY_DATE_ADDRESS_WR, DayGraphDate);
			SendN16DataToDisplay(GRAPH_DAY_MONTH_ADDRESS_WR, DayGraphMonth);
			SendN16DataToDisplay(GRAPH_DAY_YEAR_ADDRESS_WR, DayGraphYear);
			return;
		}

		if(GetDaysCount(SeltdDate, Date) <= 0) //Selection not valid
		{
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Select Past Date!!!");
			if(!DayGraphloaded)
			{
				DayGraphActive = 0;
			}
			DayGraphDate=DayGraphDateSelection.Date;
			DayGraphMonth = DayGraphDateSelection.Month;
			DayGraphYear = DayGraphDateSelection.Year;
			SendN16DataToDisplay(GRAPH_DAY_DATE_ADDRESS_WR, DayGraphDate);
			SendN16DataToDisplay(GRAPH_DAY_MONTH_ADDRESS_WR, DayGraphMonth);
			SendN16DataToDisplay(GRAPH_DAY_YEAR_ADDRESS_WR, DayGraphYear);
			return;
		}
		if(DayGraphloaded)
		{
			if((DayGraphDateSelection.Date != DayGraphDate) || (DayGraphDateSelection.Month != DayGraphMonth) || \
					(DayGraphDateSelection.Year != DayGraphYear))
			{
				DayGraphDateSelection.Date = DayGraphDate;
				DayGraphDateSelection.Month = DayGraphMonth;
				DayGraphDateSelection.Year = DayGraphYear;
				DayGraphloaded = 0;

			}
			else
			{
				//SendGraphData();
				SendN16DataToDisplay(0x080358, 0);
				SendN16DataToDisplay(0x08035A, 1);
			}
		}
		else
		{
			DayGraphDateSelection.Date = DayGraphDate;
			DayGraphDateSelection.Month = DayGraphMonth;
			DayGraphDateSelection.Year = DayGraphYear;
			SendN16DataToDisplay(0x080358, 0);
			SendN16DataToDisplay(0x08035A, 1);
		}
	}
	else
	{
		SendGraphData();
		SendN16DataToDisplay(0x080358, 1);
		SendN16DataToDisplay(0x08035A, 0);
	}
}*/

/*
 * *****************************************************************************
 * Function name: SendCalibAckToUI
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Sending Calibration ack to UI
 * Parameters:
 *  Input  : Ack status
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SendCalibAckToUI(uint16_t Ack)
{
	if(Ack == 2)
	{
		//SendN16DataToDisplay(0x0800C2, 0);
		if(!calibration_canceled_flg)
		{
			if(vp_cal_pt_rd_str.param_under_calib==CALIB_PARAM_DP)
			{
				TargetPage = 17;
//				Ln2_usage.datausage = 0;
//				Ln2_usage.pre_datausage = 0;
			}
			else

			SendCmdToDisplay(LOAD_PAGE_CMD);
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Calibration Failed!!!");
			TargetPage = 14;

		}
		calibration_canceled_flg=0;
//		SendN16DataToDisplay(CALIB_START_STOP_CONTROL_WR, 0);
//		SendN16DataToDisplay(CALIB_START_STOP_CONTROL_RD, 0);
		SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, 0);
		SendN16DataToDisplay(RTD_1POINT_EN_TCHADDRS, 0x0);
		SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x0);
		//vp_cal_pt_rd_str.param_under_calib = 0;
	}
	else if(Ack == 1)
	{
		if(!calibration_canceled_flg)
		{
			//	TargetPage = 16;
			SendCmdToDisplay(LOAD_PAGE_CMD);
			SendN16DataToDisplay(STRING_POPUP_EN, 2);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Calibration Success!!!");
			TargetPage = 14;
		}
		calibration_canceled_flg=0;
//		SendN16DataToDisplay(CALIB_START_STOP_CONTROL_WR, 0);
//		SendN16DataToDisplay(CALIB_START_STOP_CONTROL_RD, 0);
		SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, 0);
		SendN16DataToDisplay(RTD_1POINT_EN_TCHADDRS, 0x0);
		SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x0);
		//vp_cal_pt_rd_str.param_under_calib = 0;
	}
	else if(Ack == 3)
	{
		SendN16DataToDisplay(DP_2POINT_CAL2_VALUE_WR, UISensorCalibData.DP2PointValue2);
		SendN16DataToDisplay(RTD_2POINT_EN_TCHADDRS, 0x01);

	}
}

/*
 * *****************************************************************************
 * Function name: HandleBISTFunctions
 * Created by :Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : Page, Key Id
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void HandleBISTFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{

}

void HandleLEDControlFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case MENU_HOME_KEY:
				 LoadLedSettings(&UILEDSettingsData);
				break;

			case MUTE_KEY:
				mutebuzzer();
				break;

			case ENTER_KEY:
				SaveLedSettings(&UILEDSettingsData);
				break;

			case CANCEL_KEY:
				 LoadLedSettings(&UILEDSettingsData);
				break;

			case BACK_KEY:
				 LoadLedSettings(&UILEDSettingsData);
				break;

			case SYSTEM_SETTINGS_KEY:
				CLR_Settings_Flag = 1;
				UILEDSettingsData.OLD_CLR = UILEDSettingsData.NO_ERROR;
				vp_param_rw_str.NEW_COLOR_Select=UILEDSettingsData.OLD_CLR;
				SendN16DataToDisplay(NEW_COLOR_WR, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(NEW_COLOR_RD, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(OLD_COLOR_WR, UILEDSettingsData.OLD_CLR);

				SendN16DataToDisplay(COLOR_PANL_VER_KEY, 1);

				break;
			case DATE_TIME_KEY:
				CLR_Settings_Flag = 2;
				UILEDSettingsData.OLD_CLR = UILEDSettingsData.ERRORS;
				vp_param_rw_str.NEW_COLOR_Select=UILEDSettingsData.OLD_CLR;
				SendN16DataToDisplay(NEW_COLOR_WR, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(NEW_COLOR_RD, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(OLD_COLOR_WR, UILEDSettingsData.OLD_CLR);
				SendN16DataToDisplay(COLOR_PANL_VER_KEY, 1);
				break;
			case MODBUS_CRYOLINK_KEY:
				CLR_Settings_Flag = 3;
				UILEDSettingsData.OLD_CLR = UILEDSettingsData.WARNINGS;
				vp_param_rw_str.NEW_COLOR_Select=UILEDSettingsData.OLD_CLR;
				SendN16DataToDisplay(NEW_COLOR_WR, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(NEW_COLOR_RD, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(OLD_COLOR_WR, UILEDSettingsData.OLD_CLR);
				SendN16DataToDisplay(COLOR_PANL_VER_KEY, 1);

				break;
			case SCHEDULED_LN2SUPPLY_KEY:
				CLR_Settings_Flag = 4;
				UILEDSettingsData.OLD_CLR = UILEDSettingsData.UPDATES;
				vp_param_rw_str.NEW_COLOR_Select=UILEDSettingsData.OLD_CLR;
				SendN16DataToDisplay(NEW_COLOR_WR, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(NEW_COLOR_RD, vp_param_rw_str.NEW_COLOR_Select);
				SendN16DataToDisplay(OLD_COLOR_WR, UILEDSettingsData.OLD_CLR);
				SendN16DataToDisplay(COLOR_PANL_VER_KEY, 1);
				break;

		}
	}
}
void HandleSelectCOLORFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
	{
		switch(KeyId)
		{
			case LOGIN:			//SAVE
				if(CLR_Settings_Flag == 1)
				{
					UILEDSettingsData.NO_ERROR =UILEDSettingsData.NEW_COLOR;
					SendN16DataToDisplay(NORMAL_CLR_WR, UILEDSettingsData.NEW_COLOR);
					CLR_Settings_Flag = 0;
				}
				else if(CLR_Settings_Flag == 2)
				{
					UILEDSettingsData.ERRORS =UILEDSettingsData.NEW_COLOR;
					SendN16DataToDisplay(ERROR_CLR_WR,UILEDSettingsData.NEW_COLOR);
					CLR_Settings_Flag = 0;
				}
				else if(CLR_Settings_Flag == 3)
				{
					UILEDSettingsData.WARNINGS = UILEDSettingsData.NEW_COLOR;
					SendN16DataToDisplay(WARNING_CLR_WR, UILEDSettingsData.NEW_COLOR);
					CLR_Settings_Flag = 0;
				}
				else if(CLR_Settings_Flag == 4)
				{
					UILEDSettingsData.UPDATES = UILEDSettingsData.NEW_COLOR;
					SendN16DataToDisplay(UPDATE_CLR_WR, UILEDSettingsData.NEW_COLOR);
					CLR_Settings_Flag = 0;
				}
				else
				{

				}

				break;
		}

	}
}
/*
 * *****************************************************************************
 * Function name: DefaultTouchKeyHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : Page, Key Id
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void DefaultTouchKeyHandler(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	DefaultHandlerRunCtr++;
}



