/*
 * DispVPAddrList.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef DISPVPADDRLIST_H_
#define DISPVPADDRLIST_H_

enum {
	DISP_NO_RESP_EXPECTED,
	DISP_RESP_EXPECTED
};

typedef enum {
	DISP_NOT_INITIALIZED,
	DISP_INIT_INPROGRESS,
	DISP_INIT_COMPLETED,
	DISP_RUNNING,
	DISP_TX_SEQ_COMPLETED,
	DISP_TX_INPROGRESS
} DispStatusEnum;

typedef enum {
	DISP_DATA_WRITE,
	DISP_DATA_READ
} DispDataDirEnum;

typedef enum {
	N16_TYPE = 2,
	N32_TYPE = 4,
	N64_TYPE = 8,
	STR_TYPE = 128 // Default
} DispDataTypeEnum;

// VP address list - should be aligned with UI config
// N16 List
#define KEY_PRESS_EVENT_DATA_ADDRESS		0x080000
#define KEY_PRESS_EVENT_DATA_SIZE			0x02

#define USER_ACTION_DATA_ADDRESS			0x080100		//Need to comeback
#define USER_ACTION_DATA_SIZE				0x06

#define SYSTEM_PARAM_SETTINGS_ADDRESS		0x080200
#define SYSTEM_PARAM_SETTINGS_SIZE			0x74

#define SYSTEM_LOGIN_CREDENTIALS_ADDRESS	0x080300
#define SYSTEM_LOGIN_CREDENTIALS_SIZE		0xB4

#define SYSTEM_CALIB_DATA_ADDRESS			0x080400
#define SYSTEM_CALIB_DATA_SIZE				0x4C

#define SYSTEM_EVENT_ADDRESS				0x080500
#define SYSTEM_EVENT_DATA_SIZE				0x9C

#define SYSTEM_RUNTIME_DATA_ADDRESS			0x080600
#define SYSTEM_RUNTIME_DATA_SIZE			0x96

#define SYSTEM_BIST_DATA_ADDRESS			0x080700
#define SYSTEM_BIST_DATA_SIZE				0x36

#define SYSTEM_BIST1_DATA_ADDRESS			0x080900
#define SYSTEM_BIST1_DATA_SIZE				0x28

#define SYSTEM_INIT_DATA_ADDRESS			0x081000
#define SYSTEM_INIT_DATA_SIZE				0x32

#define SYSTEM_LOGIN_DATA_ADDRESS			0x081100
#define SYSTEM_LOGIN_DATA_SIZE				0x36

#define STRING_POPUP_EN                     0x0800C0
#define POPUP_STRING_DATA                   0x001E00

#define SENSOR_DATA_WRITE_ADDRESS							0x080000
#define MUTE_BUZZ_DATA_WRITE_ADDRESS						0x081002
#define WARNING_DATA_WRITE_ADDRESS							0x081008	//080038

#define SYSTEM_SETTINGS_DATA_ADDRESS						0x080030
#define TEMP_SETTINGS_DATA_WR_ADDRESS						0x080050
#define LEVEL_SETTINGS_DATA_WR_ADDRESS						0x080070
#define ALARM_SETTINGS_DATA_WR_ADDRESS						0x08008c
#define ALARM_SETTINGS_DATA_RD_ADDRESS						0x08058c
#define LID_FUNCTIONS_DATA_WR_ADDRESS						0x0800B0
//#define SCHEDULED_FILL_SETTINGS_DATA_WR_ADDRESS				0x0800D2
#define MAINTENANCE_REMAINDER_SETTINGS_DATA_WR_ADDRESS		0x0800EE
#define CHECKLEVEL_REMAINDER_SETTINGS_DATA_WR_ADDRESS		0x080112
#define LN2_SUPPLY_REMAINDER_SETTINGS_DATA_WR_ADDRESS		0x080132

#define DISPLAY_SETTINGS_DATA_WR_ADDRESS					0x080038
#define CRYO_SETTINGS_DATA_WR_ADDRESS						0x080200
#define PHASE_SETTINGS_DATA_WR_ADDRESS						0x080200

#define CRYO_PHASE_SETTINGS_RD_ADDRESS						0x080700

//Load address
#define INIT_UI_0_7F_START_WR_ADDRESS						0x080000
#define INIT_UI_80_FF_START_WR_ADDRESS						0x080080

//#define INIT_UI_100_17F_START_WR_ADDRESS					0x080100
#define INIT_UI_180_1FF_START_WR_ADDRESS					0x080180

#define INIT_UI_200_27F_START_WR_ADDRESS					0x080200
#define INIT_UI_280_2FF_START_WR_ADDRESS					0x080280

//Temp address
#define INIT_UI_500_57F_START_WR_ADDRESS					0x080500
#define INIT_UI_580_5FF_START_WR_ADDRESS					0x080580

#define INIT_UI_600_67F_START_WR_ADDRESS					0x080600
#define INIT_UI_680_6FF_START_WR_ADDRESS					0x080680

#define INIT_UI_700_77F_START_WR_ADDRESS					0x080700
#define INIT_UI_780_7FF_START_WR_ADDRESS					0x080780

#define SCHEDULEDEVENTS_SETTINGS_DATA_ADDRESS				0x0800E0
#define SCHEDULEDEVENTS_SETTINGS_RD_ADDRESS					0x0805E0

#define ANIMATION_STATUS_VP_ADDRESS							0x080104
//Specific VP Address
#define MUTE_KEY_VP_ADDRESS									0x080000
#define MUTE_KEY_ENABLE										0x0001
#define MUTE_KEY_DISABLE									0x0000

#define MANUAL_DEFOG_KEY_ADDRESS							0x08012A
#define STOP_KEY_VP_ADDRESS									0x080996
#define STOP_KEY_PRESSED									0x0001
#define STOP_KEY_RELEASED									0x0000

//#define MANUAL_DEFOG_KEY_VP_ADDRESS							0x0800C6

#define WiFi_VERTUAL_KEY_VP_ADDRESS						0x0800C6
#define MODBUS_VER_KEY_ADD								0x0800C2
#define BLE_VER_KEYVP_ADD								0x0800C4
#define RESTRICT_COMM_FLAG								0x089996

#define SECURITY_RESTRICT								0X089998

#define SELECT_ICON											0x0001
#define DE_SELECT_ICON										0x0000

#define MUTE_DISP_VP_ADDRESS								0x081006

#define T1_HIGH_LIMIT_VALUE_RD 								0x0806f6
#define T1_HIGH_LIMIT_VALUE_WR 								0x0801f6
#define T1_LOW_LIMIT_VALUE_RD 								0x0806f8
#define T1_LOW_LIMIT_VALUE_WR 								0x0801f8
#define T2_HIGH_LIMIT_VALUE_RD 								0x0806fa
#define T2_HIGH_LIMIT_VALUE_WR 								0x0801fa
#define T2_LOW_LIMIT_VALUE_RD 								0x0806fc
#define T2_LOW_LIMIT_VALUE_WR 								0x0801fc
#define T1_HIGH_LIMIT_CONTROL_RD							0x0806fe
#define T1_HIGH_LIMIT_CONTROL_WR							0x0801fe
#define T1_LOW_LIMIT_CONTROL_RD								0x080700
#define T1_LOW_LIMIT_CONTROL_WR								0x080200
#define T2_HIGH_LIMIT_CONTROL_RD							0x080702
#define T2_HIGH_LIMIT_CONTROL_WR							0x080202
#define T2_LOW_LIMIT_CONTROL_RD								0x080704
#define T2_LOW_LIMIT_CONTROL_WR								0x080204
#define T2_ENABLE_CONTROL_RD								0x080706
#define T2_ENABLE_CONTROL_WR								0x080206
#define TEMP_UNIT_SELECTION_RD								0x080708
#define TEMP_UNIT_SELECTION_WR								0x080208
#define T2_HEAADER_EN										0x0801EE
#define T2_STATUS_GOOD_BAD									0x0801F4

//VP Address - Level Settings
//#define THERMISTER_ENABLE_CONTROL_RD						0x080570

#define DP_TOUCH_EN_DIS                                     0x0802A0

//VP Address - Alarm Settings
#define BATT_BACK_EN_DIS								    0x080168
#define REMOTE_ALARM_DELAY_ENABLE_RD						0x08071e
#define REMOTE_ALARM_DELAY_ENABLE_WR						0x08021e
#define REMOTE_ALARM_DELAY_VALUE_RD							0x080720
#define REMOTE_ALARM_DELAY_VALUE_WR							0x080220
#define AUDIBLE_REPEAT_DELAY_ENABLE_RD						0x080722
#define AUDIBLE_REPEAT_DELAY_ENABLE_WR						0x080222
#define AUDIBLE_REPEAT_DELAY_VALUE_RD						0x080724
#define AUDIBLE_REPEAT_DELAY_VALUE_WR						0x080224
#define LN2_USAGE_ALARM_ENABLE_RD							0x080726
#define LN2_USAGE_ALARM_ENABLE_WR							0x080226
#define LN2_USAGE_ALARM_VALUE_RD							0x080728
#define LN2_USAGE_ALARM_VALUE_WR							0x080228
#define TEST_ALARM_ENABLE_RD								0x08072a
#define TEST_ALARM_ENABLE_WR								0x08022a
#define BAT_SW_OVR_RD							            0x08072c
#define BAT_SW_OVR_WR								        0x08022c





//VP Address - Calibration Data
//#define DP_1POINT_CAL_VALUE_RD								0x080670
//#define DP_1POINT_CAL_VALUE_WR								0x080170
#define DP_2POINT_CAL1_VALUE_RD								0x0807CA
#define DP_2POINT_CAL1_VALUE_WR								0x0802CA
#define DP_2POINT_CAL2_VALUE_RD								0x0807CC
#define DP_2POINT_CAL2_VALUE_WR								0x0802CC
#define RTD_1POINT_EN_TCHADDRS								0x08015C
#define RTD_2POINT_EN_TCHADDRS								0x080160
#define RTD_2POINT_STR_ADDRS								0x08015E

//#define RTD_
//#define T1_1POINT_CAL_VALUE_RD								0x080676
//#define T1_1POINT_CAL_VALUE_WR								0x080176
//#define T1_2POINT_CAL1_VALUE_RD								0x080678
//#define T1_2POINT_CAL1_VALUE_WR								0x080178
//#define T1_2POINT_CAL2_VALUE_RD								0x08067A
//#define T1_2POINT_CAL2_VALUE_WR								0x08017A
//#define T2_1POINT_CAL_VALUE_RD								0x08067C
//#define T2_1POINT_CAL_VALUE_WR								0x08017C
//#define T2_2POINT_CAL1_VALUE_RD								0x08067E
//#define T2_2POINT_CAL1_VALUE_WR								0x08017E
//#define T2_2POINT_CAL2_VALUE_RD								0x080680
//#define T2_2POINT_CAL2_VALUE_WR								0x080180
//#define CALIB_START_STOP_CONTROL_RD							0x080682
//#define CALIB_START_STOP_CONTROL_WR							0x080182

#define CALIBRATION_GRAY_EN                                 0x08015A
//#define CALIBRATION_TOUCH_EN                                0x080410

#define RTD1_CALIBRATION_GRAY_EN                            0x080152		//0x080152
//#define RTD1_CALIBRATION_TOUCH_EN                           0x088FF4
#define RTD2_CALIBRATION_GRAY_EN                            0x080154		//0x080154
//#define RTD2_CALIBRATION_TOUCH_EN                           0x080418

//VP Address - Display
#define DISPLAY_BRIGHTNESS_WR								0x08039C
#define DISPLAY_BRIGHTNESS_RD								0x08089C
#define DISPLAY_TOUCHSOUND_WR								0x08039E
#define DISPLAY_TOUCHSOUND_RD								0x08089E
#define DISPLAY_SCREENSAVER_WR								0x0803A0
#define DISPLAY_SCREENSAVER_RD								0x0808A0
#define DISP_LED_BRIGHTNESS_WR								0x0803A2
#define DISP_LED_BRIGHTNESS_RD								0X0808A2

//#define STATUS_LED_ENABLE_WR								0x0803A4
//#define STATUS_LED_ENABLE_RD								0x0808A4

//VP Address - Modbus
#define MODBUS_CRYODEVICEID_RD								0x0808C2
#define MODBUS_CRYODEVICEID_WR								0x0803C2
#define MODBUS_CRYOBAUDRATE_RD								0x0808C4
#define MODBUS_CRYOBAUDRATE_WR								0x0803C4
#define MODBUS_CRYOFPBBYTE_RD								0x0808C6
#define MODBUS_CRYOFPBBYTE_WR								0x0803C6
#define MODBUS_PONEDEVICEID_RD								0x0808B0
#define MODBUS_PONEDEVICEID_WR								0x0803B0
#define MODBUS_PONEBAUDRATE_RD								0x0808B2
#define MODBUS_PONEBAUDRATE_WR								0x0803B2
#define MODBUS_DATA_RD								        0x0808B4
#define MODBUS_DATA_WR								        0x0803B4
//3B6 for 4_20mA

//#define MODBUS_PONEFILLMETHOD_RD							0x08070C
//#define MODBUS_PONEFILLMETHOD_WR							0x08020C
//#define MODBUS_FILLCMD_RD									0x08070E
//#define MODBUS_FILLCMD_WR									0x08020E
//#define EN_DIS_4_20mA_WR                                    0x080210
//#define EN_DIS_4_20mA_RD                                    0x080710

#define MODBUS_SLAVE_EN										0x080104
#define MODBUS_MASTER_EN									0x080106

#define SCHEDULED_FILL_RD									0x08081A
#define SCHEDULED_FILL_WR									0x08031A
#define SCHEDULED_FREM_RD									0x08081C
#define SCHEDULED_FREM_WR									0x08031C
#define SCHEDULED_MAIN_RD									0x08081E
#define SCHEDULED_MAIN_WR									0x08031E




//#define SCHEDULED_CLVL_HR_RD								0x080612
//#define SCHEDULED_CLVL_HR_WR								0x080112
//#define SCHEDULED_CLVL_MIN_RD								0x080614
//#define SCHEDULED_CLVL_MIN_WR								0x080114
//#define SCHEDULED_CLVL_AMPM_RD								0x080616
//#define SCHEDULED_CLVL_AMPM_WR								0x080116
//#define SCHEDULED_CLVL_DAY_RD								0x080618
//#define SCHEDULED_CLVL_DAY_WR								0x080118
//#define SCHEDULED_CLVL_HR12_RD								0x08061A
//#define SCHEDULED_CLVL_HR12_WR								0x08011A
//#define SCHEDULED_CLVL_MIN12_RD								0x08061C
//#define SCHEDULED_CLVL_MIN12_WR								0x08011C

//schedule manual fill
#define SCHEDULED_FILL_DAILY_RD								0x080874
#define SCHEDULED_FILL_DAILY_WR								0x080374
#define SCHEDULED_FILL_SUNDAY_RD							0x080876
#define SCHEDULED_FILL_SUNDAY_WR							0x080376
#define SCHEDULED_FILL_MONDAY_RD							0x080878
#define SCHEDULED_FILL_MONDAY_WR							0x080378
#define SCHEDULED_FILL_TUESDAY_RD							0x08087A
#define SCHEDULED_FILL_TUESDAY_WR							0x08037A
#define SCHEDULED_FILL_WEDNESDAY_RD							0x08087C
#define SCHEDULED_FILL_WEDNESDAY_WR							0x08037C
#define SCHEDULED_FILL_THURSDAY_RD							0x08087E
#define SCHEDULED_FILL_THURSDAY_WR							0x08037E
#define SCHEDULED_FILL_FRIDAY_RD							0x080880
#define SCHEDULED_FILL_FRIDAY_WR							0x080380
#define SCHEDULED_FILL_SATURDAY_RD							0x080882
#define SCHEDULED_FILL_SATURDAY_WR							0x080382

#define SCHEDULED_FILL_HR12_RD								0x080884
#define SCHEDULED_FILL_HR12_WR								0x080384
#define SCHEDULED_FILL_HR_RD								0x080886
#define SCHEDULED_FILL_HR_WR								0x080386
#define SCHEDULED_FILL_MIN_RD								0x080888
#define SCHEDULED_FILL_MIN_WR								0x080388
#define SCHEDULED_FILL_AMPM_RD								0x08088A
#define SCHEDULED_FILL_AMPM_WR								0x08038A

#define SCHEDULED_FILL_EN_DIS_WEEKDAYS						0x08016E

//Maintainance Remainder

#define MAINT_MONTH_VALUE_WR								0x080320
#define MAINT_MONTH_VALUE_RD								0x080820
#define MAINT_DATE_VALUE_WR									0x080322
#define MAINT_DATE_VALUE_RD									0x080822
#define MAINT_YEAR_VALUE_WR									0x080324
#define MAINT_YEAR_VALUE_RD									0x080824
#define MAINT_12HOURS_VALUE_WR								0x080326
#define MAINT_12HOURS_VALUE_RD								0x080826
#define MAINT_HOURS_VALUE_WR								0x080328
#define MAINT_HOURS_VALUE_RD								0x080828
#define MAINT_MIN_VALUE_WR									0x08032A
#define MAINT_MIN_VALUE_RD									0x08082A
#define MAINT_AMPM_VALUE_WR									0x08032C
#define MAINT_AMPM_VALUE_RD									0x08082C

#define FREEZER_ID_VALUE_WR									0x000d00
#define FREEZER_ID_VALUE_RD									0x001200

//FILL REMAINDER

#define FILLREM_MONTH_WR									0x08032E
#define FILLREM_MONTH_RD									0x08082E
#define FILLREM_DATE_WR										0x080330
#define FILLREM_DATE_RD										0x080830
#define FILLREM_YEAR_WR										0x080332
#define FILLREM_YEAR_RD										0x080832
#define FILLREM_12HOUR_VALUE_WR								0x080334
#define FILLREM_12HOUR_VALUE_RD								0x080834
#define FILLREM_24HOUR_VALUE_WR								0x080336
#define FILLREM_24HOUR_VALUE_RD								0x080836
#define FILLREM_MINUTE_VALUE_WR								0x080338
#define FILLREM_MINUTE_VALUE_RD								0x080838
#define FILLREM_AMPM_VALUE_WR								0x08033A
#define FILLREM_AMPM_VALUE_RD								0x08083A

#define SCHEDULED_LN2S_HR_RD								0x080632
#define SCHEDULED_LN2S_HR_WR								0x080132
#define SCHEDULED_LN2S_MIN_RD								0x080634
#define SCHEDULED_LN2S_MIN_WR								0x080134
#define SCHEDULED_LN2S_AMPM_RD								0x080636
#define SCHEDULED_LN2S_AMPM_WR								0x080136
#define SCHEDULED_LN2S_DAY_RD								0x080638
#define SCHEDULED_LN2S_DAY_WR								0x080138
#define SCHEDULED_LN2S_HR12_RD								0x08063A
#define SCHEDULED_LN2S_HR12_WR								0x08013A
#define SCHEDULED_LN2S_MIN12_RD								0x08063C
#define SCHEDULED_LN2S_MIN12_WR								0x08013C



#define RTC_DATE_VALUE_WR									0x080264
#define RTC_DATE_VALUE_RD									0x080764
#define RTC_MONTH_VALUE_WR									0x080266
#define RTC_MONTH_VALUE_RD									0x080766
#define RTC_YEAR_VALUE_WR									0x080268
#define RTC_YEAR_VALUE_RD									0x080768
#define RTC_DATEFORMAT_VALUE_WR								0x08026a
#define RTC_DATEFORMAT_VALUE_RD								0x08076a
#define RTC_AMPM_VALUE_WR									0x08026c
#define RTC_AMPM_VALUE_RD									0x08076c
#define RTC_MIN_VALUE_WR									0x08026E
#define RTC_MIN_VALUE_RD									0x08076E
#define RTC_12HOURS_VALUE_WR								0x080270
#define RTC_12HOURS_VALUE_RD								0x080770
#define RTC_TIMEFORMAT_VALUE_WR								0x080272
#define RTC_TIMEFORMAT_VALUE_RD								0x080772
#define RTC_HOURS_VALUE_WR									0x080274
#define RTC_HOURS_VALUE_RD									0x080774




//user creation
#define User_Tick1_En                                       0x080172
#define User_Tick2_En                                       0x080174
#define User_Tick3_En                                       0x080176
#define User_Tick4_En                                       0x080178
#define User_Tick5_En                                       0x08017a
#define User_Tick6_En                                       0x08017c
#define User_Tick7_En                                       0x08017e
#define User_Tick8_En                                       0x080180
#define User_Tick9_En                                       0x080182
#define User_Tick10_En                                      0x080184

#define USER_NAME_1                                         0x001700
#define USER_NAME_2                                         0x001780
#define USER_NAME_3                                         0x001800
#define USER_NAME_4                                         0x001880
#define USER_NAME_5                                         0x001900
#define USER_NAME_6                                         0x001980
#define USER_NAME_7                                         0x001a00
#define USER_NAME_8                                         0x001a80
#define USER_NAME_9                                         0x001b00
#define USER_NAME_10                                        0x001b80


#define USER1_EN                                         	0x080186
#define USER2_EN                                         	0x080188
#define USER3_EN                                         	0x08018a
#define USER4_EN                                         	0x08018c
#define USER5_EN                                         	0x08018e
#define USER6_EN                                         	0x080190
#define USER7_EN                                         	0x080192
#define USER8_EN                                         	0x080194
#define USER9_EN                                         	0x080196
#define USER10_EN                                        	0x001b98


#define LIST_SELECT_WR                                      0x080296
#define LIST_SELECT_RD                                      0x080796
#define ADMIN_OR_USER_WR                                    0x080298
#define ADMIN_OR_USER_RD                                    0x080798
#define ADD_USER_ID_VALUE_WR								0x003800
#define ADD_USER_ID_VALUE_RD								0x003d00
#define ADD_EMP_ID_VALUE_WR									0x003880
#define ADD_EMP_ID_VALUE_RD									0x003d80
//#define DELETE_WR                                           0x08019E
//#define DELETE_RD                                           0x08069E

#define DELETE_SELECT_WR1                                   0x080282
#define DELETE_SELECT_RD1                                   0x080782
#define DELETE_SELECT_WR2                                   0x080284
#define DELETE_SELECT_RD2                                   0x080784
#define DELETE_SELECT_WR3                                   0x080286
#define DELETE_SELECT_RD3                                   0x080786
#define DELETE_SELECT_WR4                                   0x080288
#define DELETE_SELECT_RD4                                   0x080788
#define DELETE_SELECT_WR5                                   0x08028a
#define DELETE_SELECT_RD5                                   0x08078a
#define DELETE_SELECT_WR6                                   0x08028c
#define DELETE_SELECT_RD6                                   0x08078c
#define DELETE_SELECT_WR7                                   0x08028e
#define DELETE_SELECT_RD7                                   0x08078e
#define DELETE_SELECT_WR8                                   0x080290
#define DELETE_SELECT_RD8                                   0x080790
#define DELETE_SELECT_WR9                                   0x080292
#define DELETE_SELECT_RD9                                   0x080792
#define DELETE_SELECT_WR10                                  0x080294
#define DELETE_SELECT_RD10                                  0x080794

#define ADMIN1_EN                                  			0x08019a
#define ADMIN2_EN                                  			0x08019c

//need to verify with venkat
//#define FW_UPDATE_START_KEY_WR                              0x080914		//This one old not updated to new
#define FW_UPDATE_START_KEY_WR                              0x0801E6
#define FW_UPDATE_START_KEY_TOUCH_EN                        0x0801E4

//login credentials
#define ACCOUNT_MISMATCH_EN									0x0801A2
#define ACCOUNT_LOCKED_EN									0x0801A4
#define FIRST_USER_EN										0x0801a6
#define LOGIN_LIMITS										0x08029A
#define LOGIN_ID_WR                                         0x003900
#define LOGIN_ID_RD                                         0x003E00
#define LOGIN_NEW_PW_WR                                     0x003a00
#define LOGIN_NEW_PW_RD                                     0x003f00
#define LOGIN_CONFM_PW_WR                                   0x003a80
#define LOGIN_CONFM_PW_RD                                   0x003f80
#define LOGIN_PW_WR                                         0x003980
#define LOGIN_PW_RD                                         0x003e80
#define LOGEDIN_USER										0x000E00


//wifi

#define WIFI_SIG_STR_KEY_RD_WR                              0x0801B2
#define WIFI_SIG_STR_ICON_WR                                0x0801B4
#define WIFI_PASS_STR_En									0x0801b6
#define Wifi_refresh_en                                     0x0801b8
#define WIFI_SIGNAL_STRENGTH_TOUCH                          0x0801ba

#define WIFI_SCAN_EN_WR                                     0x0801BC
#define WIFI_SCAN_LAST_EN_WR                                0x0801ce
#define WIFI_NAVIGATION_EN									0x0801D0
#define IOT_DIS_EN											0x0801D4

#define WIFI_CONNECT_WR                                     0x0802F2
#define WIFI_CONNECT_RD                                     0x0807F2
#define IOT_CONNECT_WR                                      0x0802F4
#define IOT_CONNECT_RD                                      0x0807F4
#define WIFI_PASS_EN_WR                                     0x0802f6
#define WIFI_PASS_EN_RD                                     0x0807f6
#define WIFI_ADD_WR                                         0x001C00
#define WIFI_ADD_RD                                         0x002100
#define WIFI_PW_WR                                          0x001c80
#define WIFI_PW_RD                                          0x002180

#define WIFI_SIGNAL1_WR                                     0x08011E
#define WIFI_SIGNAL2_WR                                     0x080120
#define WIFI_SIGNAL3_WR                                     0x080122
#define WIFI_SIGNAL4_WR                                     0x080124
#define WIFI_SIGNAL5_WR                                     0x080126
#define WIFI_SIGNAL6_WR                                     0x080128
#define WIFI_SIGNAL7_WR                                     0x08012a
#define WIFI_SIGNAL8_WR                                     0x08012c
#define WIFI_SIGNAL9_WR                                     0x08012E
#define WIFI_SIGNAL10_WR                                    0x080130


#define WIFI_SCAN1_WR                                       0x002200
#define WIFI_SCAN2_WR                                       0x002280
#define WIFI_SCAN3_WR                                       0x002300
#define WIFI_SCAN4_WR                                       0x002380
#define WIFI_SCAN5_WR                                       0x002400
#define WIFI_SCAN6_WR                                       0x002480
#define WIFI_SCAN7_WR                                       0x002500
#define WIFI_SCAN8_WR                                       0x002580
#define WIFI_SCAN9_WR                                       0x002600
#define WIFI_SCAN10_WR                                      0x002680







//Aws
#define AWS_IOT_CNCTVTY                                     0x0802be
#define AWS_IOT_CNCTVTY_RD                                 	0x0807be
#define AWS_URL_WR                                          0x003080
#define AWS_URL_RD                                          0x003580
#define AWS_USER_NAME_WR                                    0x003100
#define AWS_USER_NAME_RD                                    0x003600
//#define AWS_PASSWORD_WR                                     0x006200
//#define AWS_PASSWORD_RD                                     0x006700
#define AWS_EMAIL_WR                                        0x003180
#define AWS_EMAIL_RD                                        0x003680
#define AWS_PHNO_WR                                         0x003200
#define AWS_PHNO_RD                                         0x003700
//#define AWS_TOKEN_WR                                        0x006380
//#define AWS_TOKEN_RD                                        0x006880

#define AWS_IOT_CNCTVTY_TOUCH                               0x0801D6		//0x0801D4(0ld address)
#define IOT_DEV_SL_NO										0x003000


//Need to comeback for Firmware download
#define IOT_FIRM_DOWN_EN                                    0x089010 //file downloading
#define IOT_FIRM_AVAIL_EN                                   0x08900E //IOT file available
#define IOT_FILE_STATUS                                     0x088FFE //0- availability, 1-downloading,2-downloaded
#define IOT_FIRM_POP_UP_EN                                  0x088000
#define IOT_FIRM_SEL_EN                                     0x088008
#define PICK_TIME_EN                                        0x088FFC//pick up time change
#define IOT_FIRM_FILE_DOWN_EN                               0x088FFA//file downloaded

#define IOT_FIRM_ICON_WR                                    0x088006
#define IOT_FIRM_ICON_RD                                    0x088506

//#define NW_ID_WR                                            0x000d00
//#define NW_ID_RD                                            0x001200

//Firmware Update Pick a Time

//#define UPDATE_12MIN_VALUE_WR                               0x0801CE
//#define UPDATE_12MIN_VALUE_RD                               0x0806CE
#define FIRMWARE_UPDATE_VKEY								0x0800BA
#define UPDATENOW_PICATIME_VKEY								0x0800BE
#define PICK_A_TIME_VKEY                                    0x0800B8
#define UPDATE_FIRMWARE_CANCEL								0x0801E8
#define UPDATE_MIN_VALUE_WR                                 0x080356
#define UPDATE_MIN_VALUE_RD                                 0x080856
#define UPDATE_12HOURS_VALUE_WR                             0x080352
#define UPDATE_12HOURS_VALUE_WD                             0x080852
#define UPDATE_HOURS_VALUE_WR                               0x080354
#define UPDATE_HOURS_VALUE_RD                               0x080854
#define UPDATE_MONTH_VALUE_WR                               0x08034C
#define UPDATE_MONTH_VALUE_RD                               0x08084C
#define UPDATE_DATE_VALUE_WR                                0x08034E
#define UPDATE_DATE_VALUE_WD                                0x08084E
#define UPDATE_YEAR_VALUE_WR                                0x080350
#define UPDATE_YEAR_VALUE_RD                                0x080850
#define UPDATE_AMPM_VALUE_WR                                0x080358
#define UPDATE_AMPM_VALUE_RD                                0x080858

#define GROUP_EN_WR                                         0x080324
#define GROUP_EN_RD                                         0x080824
#define GROUP_SEL_WR                                        0x080326
#define GROUP_SEL_RD                                        0x080826
#define GROUP_A_WR                                          0x005300
#define GROUP_A_RD                                          0x005800
#define GROUP_B_WR                                          0x005380
#define GROUP_B_RD                                          0x005880
#define GROUP_C_WR                                          0x005400
#define GROUP_C_RD                                          0x005900
#define GROUP_D_WR                                          0x005480
#define GROUP_D_RD                                          0x005980
#define GROUP_COL_WR                                        0x0802A4

#define EN_DIS_RED                                          0x0802E4

//System Info
#define EDIT_ADDRESS_WR                                     0x0800d8
#define EDIT_ADDRESS_RD                                     0x0805d8
#define HARDWARE_SERIAL_NO									0x000080
#define SOFTWARE_VERSION									0x000100
//#define SECURITY_EDITCH_EN								    0x080166
#define CONTACT_EDIT_EN										0x08016c
#define GUI_VERSION											0x000180
#define HARDWARE_VERSION									0x000200
#define SUPLY_CONTACT_WR                                    0x000280
#define SUPLY_CONTACT_RD                                    0x000780
#define SERVICE_CONTACT_WR                                  0x000300
#define SERVICE_CONTACT_RD                                  0x000800
#define SYS_CONTACT_FIELD_EN								0x08016C
#define PLUMBING_OPT										0x0800DA
#define PLUM_OPT_BATTERY									0x0800DC
#define PLUM_OPT_LEVEL_SENS									0x0800DE
#define PLUM_OPT_TEMP_PROBES								0x0800E0
#define PLUM_FREEZER_MODEL									0x0800E2

//Factory Reset
#define FACTORY_RST_CNTDWN									0x080044
#define FACTORY_RST_STATUSCH								0x0800F4
#define FACTORY_RST_TCH_EN									0x0801AA
#define FACTORY_RST_CNTDWN_EN								0x0801AC
#define FACTORY_RST_CRSCANCEL_EN							0x0801AE
#define FACTORY_RST_CANCEL_En								0x0801b0

#define FACTORY_RST_VKEY									0x0800BC




//Home Page address

#define NW_ID_WR                                            0x000d00		//Already defined at 536 line.
#define TIME_AM_PM											0x000D80
#define INTERNET_NAME										0x000E80
#define BLE_NAME											0x000F00
#define MODBUS_NAME											0x000F80
//#define TEMP_UNIT_KEY_ADDRESS								0x080002		//Already defined at 761 line.
#define T2_TEMP												0x080004
#define LEVEL_LN2											0x000006
#define WEIGHT_OVERALL										0x080008
#define MAINS_BATTERY										0x08000A
#define BATTERY_VALUES										0x08000C
#define LOGIN_INDEX											0x08000E
#define INTERNET_STATUS										0x080010
#define BLE_STATUS											0x080012
#define MODBUS_STATUS										0x080014
#define TEMPERATURE_UNITS									0x080016
#define T1_ONLY_OPEN_EN										0x08001C
#define WHITET1_ONLY_EN										0x08001E
#define REDT1_ONLY_EN										0x080020
#define T1_OPEN_EN											0x080022
#define T2_OPEN_EN											0x080024
#define REDT1_EN											0x080026
#define WHITET1_EN											0x080028
#define REDT2_EN											0x08002A
#define WHITET2_EN											0x08002C
#define LEVEL_FAULT_EN										0x08002E
#define WEIGHT_FAULT_EN										0x080030
#define BATTERY_EN											0x080032
#define MAINS_EN											0x080034
#define UNUSED_ADDRESS										0x080036
#define WARNING												0x080038
#define TIME_AM_PM_EN										0x08003A
#define DMY_FMT_EN											0x08003C
#define MDY_FMT_EN											0x08003E
#define HOUR												0x080040


#define DISABLE_COMM_FAILURE								0x080100

//Weight
#define LOAD_CELL1_VALUE									0x020010
#define LOAD_CELL2_VALUE									0x020014
#define LOAD_CELL3_VALUE									0x020018
#define LOAD_CELL4_VALUE									0x02001C
#define LOAD_CELL5_VALUE									0x020020
#define LOAD_CELL_AVG_VALUE									0x020024

//LEVEL
#define LEVEL_EMPTY_WR										0x08023C
#define LEVEL_EMPTY_RD										0x08073C
#define LEVEL_REFILLSOON_WR									0x08023E
#define LEVEL_REFILLSOON_RD									0x08073E
#define LEVEL_FILL_WR										0x080240
#define LEVEL_FILL_RD										0x080740



//LED Addresses
#define STATUS_LED_ENABLE_WR								0x0803A4
#define STATUS_LED_ENABLE_RD								0x0808A4

#define ERRORS_ONLY_WR										0x0803D8
#define ERRORS_ONLY_RD										0x0808D8
#define TEMP_BIT_WR											0x0803DA
#define TEMP_BIT_RD											0x0808DA
#define LEVEL_BIT_WR										0x0803DC
#define LEVEL_BIT_RD										0x0808DC
#define VACUUM_BIT_WR										0x0803DE
#define VACUUM_BIT_RD										0x0808DE
#define STORAGE_BIT_WR										0x0803E0
#define STORAGE_BIT_RD										0x0808E0
#define MAINS_FAIL_BIT_WR									0x0803E2
#define MAINS_FAIL_BIT_RD									0x0808E2
#define NETWORK_FAIL_BIT_WR									0x0803E4
#define NETWORK_FAIL_BIT_RD									0x0808E4
#define BLINK_CTRL_WR										0x0803E6
#define BLINK_CTRL_RD										0x0808E6
#define NORMAL_CLR_WR										0x0803E8
#define NORMAL_CLR_RD										0x0808E8
#define ERROR_CLR_WR										0x0803EA
#define ERROR_CLR_RD										0x0808EA
#define WARNING_CLR_WR										0x0803EC
#define WARNING_CLR_RD										0x0808EC
#define UPDATE_CLR_WR										0x0803EE
#define UPDATE_CLR_RD										0x0808EE
#define NEW_COLOR_WR										0x0803F0
#define NEW_COLOR_RD										0x0808F0
#define OLD_COLOR_WR										0x0803F2


#define COLOR_PANL_VER_KEY									0x0800B6




//File Explorer
#define FOLD_FILE_NAME_1									0x001280
#define FOLD_FILE_NAME_2									0x001300
#define FOLD_FILE_NAME_3									0x001380
#define FOLD_FILE_NAME_4									0x001400
#define FOLD_FILE_NAME_5									0x001480
#define FOLD_FILE_NAME_6									0x001500
#define FOLD_FILE_NAME_7									0x001580


#define FILE_SIZE_1											0x080914
#define FILE_SIZE_2											0x080916
#define FILE_SIZE_3											0x080918
#define FILE_SIZE_4											0x08091A
#define FILE_SIZE_5											0x08091C
#define FILE_SIZE_6											0x08091E
#define FILE_SIZE_7											0x080920

#define FILE_SEL1_WR										0x080400
#define FILE_SEL1_RD										0x080900
#define FILE_SEL2_WR										0x080402
#define FILE_SEL2_RD										0x080902
#define FILE_SEL3_WR										0x080404
#define FILE_SEL3_RD										0x080904
#define FILE_SEL4_WR										0x080406
#define FILE_SEL4_RD										0x080906
#define FILE_SEL5_WR										0x080408
#define FILE_SEL5_RD										0x080908
#define FILE_SEL6_WR										0x08040A
#define FILE_SEL6_RD										0x08090A
#define FILE_SEL7_WR										0x08040C
#define FILE_SEL7_RD										0x08090C

#define FILE_SEL1_EN										0x080132
#define FILE_SEL2_EN										0x080134
#define FILE_SEL3_EN										0x080136
#define FILE_SEL4_EN										0x080138
#define FILE_SEL5_EN										0x08013A
#define FILE_SEL6_EN										0x08013C
#define FILE_SEL7_EN										0x08013E


#define FOLD_FILE_LIN1_EN									0x080140
#define FOLD_FILE_LIN2_EN									0x080142
#define FOLD_FILE_LIN3_EN									0x080146
#define FOLD_FILE_LIN4_EN									0x080148
#define FOLD_FILE_LIN5_EN									0x08014A
#define FOLD_FILE_LIN6_EN									0x08014C
#define FOLD_FILE_LIN7_EN									0x08014E




//ERROR
#define ERRTOTAL_UIINDEX									0x0800CA
#define ERRCNT_CURRENT_INDEX								0x0800CC
#define ERRNAV_RIGHT_RED_EN									0x0800CE
#define ERRNAV_LEFT_RED_EN									0x0800D0
#define ERR_TEXT_ADDRS										0x001D00
#define ERR_ACK_EN_DIS										0x0801D8


//4-20mA
#define Click4_20mA_en_dis                                  0x088FD0 //not used

#define FIRM_4_20_mA_ICON_WR                                0x089F00
#define FIRM_4_20_mA_ICON_RD                                0x08A400


#define SLAVE_TER_EN_WR                                     0x089FFE
#define SLAVE_TER_EN_RD                                     0x08A4FE
#define MASTER_TER_EN_WR                                    0x08A000
#define MASTER_TER_EN_RD                                    0x08A500
#define EXT_MOD_EN_DIS_WR                                   0x08A002
#define EXT_MOD_EN_DIS_RD                                   0x08A502
#define CHA1_EN_DIS_WR                                      0x08A004
#define CHA1_EN_DIS_RD                                      0x08A504
#define CHA2_EN_DIS_WR                                      0x08A006
#define CHA2_EN_DIS_RD                                      0x08A506
#define CHA1_SEL_WR                                         0x08A008
#define CHA1_SEL_RD                                         0x08A508
#define CHA2_SEL_WR                                         0x08A00A
#define CHA2_SEL_RD                                         0x08A50A
#define CH1_SOURCE_WR                                       0x08A00C
#define CH1_SOURCE_RD                                       0x08A50C
#define CHA1_VOL_CUR_WR                                     0x08A00E
#define CHA1_VOL_CUR_RD                                     0x08A50E
#define CHA2_VOL_CUR_WR                                     0x08A010
#define CHA2_VOL_CUR_RD                                     0x08A510

#define CHA3_EN_DIS_WR                                      0x08A012
#define CHA3_EN_DIS_RD                                      0x08A512
#define CHA4_EN_DIS_WR                                      0x08A014
#define CHA4_EN_DIS_RD                                      0x08A514

#define CHA3_SEL_WR                                         0x08A016
#define CHA3_SEL_RD                                         0x08A516
#define CHA4_SEL_WR                                         0x08A018
#define CHA4_SEL_RD                                         0x08A518

#define CH2_SOURCE_WR                                       0x08A01A
#define CH2_SOURCE_RD                                       0x08A51A

#define MAX_CONIG_CH1_WR                                    0x08A01C
#define MAX_CONIG_CH1_RD                                    0x08A51C
#define MIN_CONIG_CH1_WR                                    0x08A01E
#define MIN_CONIG_CH1_RD                                    0x08A51E

#define MAX_CONIG_CH2_WR                                    0x08A020
#define MAX_CONIG_CH2_RD                                    0x08A520
#define MIN_CONIG_CH2_WR                                    0x08A022
#define MIN_CONIG_CH2_RD                                    0x08A522

#define MAX_CONIG_CH3_WR                                    0x08A024
#define MAX_CONIG_CH3_RD                                    0x08A524
#define MIN_CONIG_CH3_WR                                    0x08A026
#define MIN_CONIG_CH3_RD                                    0x08A526

#define MAX_CONIG_CH4_WR                                    0x08A028
#define MAX_CONIG_CH4_RD                                    0x08A528
#define MIN_CONIG_CH4_WR                                    0x08A02A
#define MIN_CONIG_CH4_RD                                    0x08A52A

#define FW_4_20_mAUpdate_En                                 0x088FEE
#define FIRMWARE_420_Virtual_Key                            0x087FFE
//#define CH1_CONFIG_EN                                       0x088FDC unused
//#define CH2_CONFIG_EN                                       0x088FDE
//
//#define CH1_T1_EN                                           0x088FE0
//#define CH1_T2_EN                                           0x088FE2
//#define CH1_DP_EN                                           0x088FE4
//#define CH2_T1_EN                                           0x088FE6
//#define CH2_T2_EN                                           0x088FE8
//#define CH2_DP_EN                                           0x088FEA

//system info 4-20mA
#define SERIAL_NO_4_20_ADDRESS							    0x00A000
#define HAEDWARE_4_20_ADDRESS							    0x00A080
#define SOFTWARE_4_20_ADDRESS							    0x00A100
//ctrs
#define CTRS_DATA_CMD_WR                                    0X020000//0x089080

typedef enum {
	T1_HIGH_LIMIT_CONTROL,
	T1_HIGH_LIMIT_VALUE,
	T1_LOW_LIMIT_CONTROL,
	T1_LOW_LIMIT_VALUE,
	T2_HIGH_LIMIT_CONTROL,
	T2_HIGH_LIMIT_VALUE,
	T2_LOW_LIMIT_CONTROL,
	T2_LOW_LIMIT_VALUE,
	TEMP_UNIT_SELECTION,
	T2_ENABLE_CONTROL
} UIDataInList;

// This list should align with definitions in DisplayTxHAndlers.c and DisplayRxHAndlers.c
enum {
	KEY_PRESS_DATA,
	USER_ACTION_DATA,
	SYS_SETTINGS_DATA,
	LOGIN_CREDENTIALS_DATA,
	SYS_CALIB_DATA,
	SYS_EVENT_DATA,
	SYS_RUNTIMEDATA,
	SYS_BIST_DATA,
	SYS_BIST1_DATA,
	SYS_INIT_DATA,
	SYS_LOGIN_DATA,
	MAX_VP_DATA_SETS
};

///Data size defines
#define N16_DATA_SIZE		2
#define N32_DATA_SIZE		4
#define N64_DATA_SIZE		8
#define STR_DATA_SIZE		128  //Max size, or \0 will be used

#define TEMP_LOW_INPUT_LIMT		-2000
#define TEMP_HIGH_INPUT_LIMT	1000

///Touch key VP addresses
//Home screen
#define MUTE_KEY_ADDRESS				0x080000

//Temp settings page
#define T1_HIGH_LIMIT_KEY_ADDRESS			0x080026
#define T1_HIGH_LIMIT_SET_KEY_ADDRESS		0x000300
#define T1_LOW_LIMIT_KEY_ADDRESS			0x080028
#define T1_LOW_LIMIT_SET_KEY_ADDRESS		0x000480
//#define T2_ENABLE_KEY_ADDRESS				0x0800D4
#define T2_HIGH_LIMIT_KEY_ADDRESS			0x08002A
#define T2_HIGH_LIMIT_SET_KEY_ADDRESS		0x000380
#define T2_LOW_LIMIT_KEY_ADDRESS			0x08002C
#define T2_LOW_LIMIT_SET_KEY_ADDRESS		0x000500
#define TEMP_UNIT_KEY_ADDRESS				0x080002
#define FACTORY_DEFAULTS_KEY_ADDRESS		0x080000  //Missing address

//System Settings page
#define SCREENSAVER_TIME_SET_KEY_ADDRESS	0x080050
#define BRIGHTNESS_SET_KEY_ADDRESS			0x080038
#define BATTERY_SAVER_SET_KEY_ADDRESS		0x08004C
#define DARK_MODE_SET_KEY_ADDRESS			0x08004E

//Alarm settings page
#define DARK_MODE_SET_KEY_ADDRESS			0x08004E

//Graph Selection input
#define GRAPH_SELECTION_KEY_ADDRESS_RD		0x089528
#define GRAPH_SELECTION_KEY_ADDRESS_WR		0x089028
#define GRAPH_SCALE_SELECTION_ADDRESS_RD	0x089550
#define GRAPH_SCALE_SELECTION_ADDRESS_WR	0x089050
#define GRAPH_DAY_ENABLE_ADDRESS_WR			0x089060
#define GRAPH_DAY_SCALE_SELECT_ADDRESS_WR	0x089064
#define GRAPH_DAY_ENABLE_ADDRESS_RD			0x089560
#define GRAPH_DAY_SCALE_SELECT_ADDRESS_RD	0x089564
#define GRAPH_DAY_DATE_ADDRESS_WR			0x089066
#define GRAPH_DAY_DATE_ADDRESS_RD			0x089566
#define GRAPH_DAY_MONTH_ADDRESS_WR			0x089068
#define GRAPH_DAY_MONTH_ADDRESS_RD			0x089568
#define GRAPH_DAY_YEAR_ADDRESS_WR			0x08906A
#define GRAPH_DAY_YEAR_ADDRESS_RD			0x08956A


//#define T2_EN_DIS_HOME                      0x0803EA
//#define THERM_EN_DIS_HOME                   0x0803EC
//#define SERIAL_ADDRESS                      0x000C00

//ctrs
#define RTD_HIGH_LIMIT_VALUE_RD 							0x0895A8
#define RTD_HIGH_LIMIT_VALUE_WR 							0x0890A8
#define RTD_LOW_LIMIT_VALUE_RD 								0x0895AA
#define RTD_LOW_LIMIT_VALUE_WR 								0x0890AA
#define LOAD_HIGHLIMIT_VALUE_RD 							0X020600
#define LOAD_HIGHLIMIT_VALUE_WR 							0X020100
#define CAL_LOAD_OFFSET_RD                                  0X020604
#define CAL_LOAD_OFFSET_WR                                  0X020104



//EMMC file explorer

//file name string address
#define FILE_NAME_STR1      0x002700
#define FILE_NAME_STR2		0x002780
#define FILE_NAME_STR3		0x002800
#define FILE_NAME_STR4		0x002880
#define FILE_NAME_STR5		0x002900
#define FILE_NAME_STR6		0x002980
#define FILE_NAME_STR7		0x002A00
#define FILE_NAME_STR8		0x002A80
#define FILE_NAME_STR9		0x002B00
#define FILE_NAME_STR10		0x002B80
#define FILE_NAME_STR11 	0x002C00
#define FILE_NAME_STR12		0x002C80


//touch enable
#define TOUCH_EN1			0x080132
#define TOUCH_EN2			0x080134
#define TOUCH_EN3			0x080136
#define TOUCH_EN4			0x080138
#define TOUCH_EN5			0x08013A
#define TOUCH_EN6			0x08013C
#define TOUCH_EN7			0x08013E
#define TOUCH_EN8			0x080140
#define TOUCH_EN9			0x080142
#define TOUCH_EN10			0x080144
#define TOUCH_EN11			0x080146
#define TOUCH_EN12			0x080148


//file size vp address
#define FILE_SIZE_VP1	0x08091E
#define FILE_SIZE_VP2	0x080920
#define FILE_SIZE_VP3	0x080922
#define FILE_SIZE_VP4	0x080924
#define FILE_SIZE_VP5	0x080926
#define FILE_SIZE_VP6	0x080928
#define FILE_SIZE_VP7	0x08092A
#define FILE_SIZE_VP8	0x08092C
#define FILE_SIZE_VP9	0x08092E
#define FILE_SIZE_VP10	0x080930
#define FILE_SIZE_VP11	0x080932
#define FILE_SIZE_VP12	0x080934

//icon selection
#define ICON_SELECTION1		0x08041E
#define ICON_SELECTION2		0x080420
#define ICON_SELECTION3		0x080422
#define ICON_SELECTION4		0x080424
#define ICON_SELECTION5		0x080426
#define ICON_SELECTION6		0x080428
#define ICON_SELECTION7		0x08042A
#define ICON_SELECTION8		0x08042C
#define ICON_SELECTION9		0x08042E
#define ICON_SELECTION10	0x080430
#define ICON_SELECTION11	0x080432
#define ICON_SELECTION12	0x080434

//tick read and write address
#define FILE_TICK_SEL1_WR	0x080400
#define FILE_TICK_SEL1_RD	0x080900
#define FILE_TICK_SEL2_WR	0x080402
#define FILE_TICK_SEL2_RD	0x080902
#define FILE_TICK_SEL3_WR	0x080404
#define FILE_TICK_SEL3_RD	0x080904
#define FILE_TICK_SEL4_WR	0x080406
#define FILE_TICK_SEL4_RD	0x080906
#define FILE_TICK_SEL5_WR	0x080408
#define FILE_TICK_SEL5_RD	0x080908
#define FILE_TICK_SEL6_WR	0x08040A
#define FILE_TICK_SEL6_RD	0x08090A
#define FILE_TICK_SEL7_WR	0x08040C
#define FILE_TICK_SEL7_RD	0x08090C
#define FILE_TICK_SEL8_WR	0x08040E
#define FILE_TICK_SEL8_RD	0x08090E
#define FILE_TICK_SEL9_WR	0x080410
#define FILE_TICK_SEL9_RD	0x080910
#define FILE_TICK_SEL10_WR	0x080412
#define FILE_TICK_SEL10_RD	0x080912
#define FILE_TICK_SEL11_WR	0x080414
#define FILE_TICK_SEL11_RD	0x080914
#define FILE_TICK_SEL12_WR	0x080416
#define FILE_TICK_SEL12_RD	0x080916

//FILE DECOMPRESSION virtual key

#define FILE_COPY_VIR_KEY 0x0800B4
#define FILE_EXPLR_NAVIGATION_KEY 0x080162

#endif /* DISPVPADDRLIST_H_ */
