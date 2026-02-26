/*
 * UI.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef UI_H_
#define UI_H_

/* Includes */
#include "MyMain.h"
#include "RTC.h"
#include "Calibration.h"

/* Declaration of constant defines and typedefs */
#define UI_CIR_BUF_DEPTH           8

#define UI_TX                      0
#define UI_RX                      1

/* Back-lighting constants */
#define TFT_V_ON                   1
#define TFT_V_OFF                  10
#define TFT_ON_TIME                5
#define TFT_BL_NOM_VAL             50      // Range: 0x00-0x3F (0-63)
#define TFT_BL_MIN_VAL             0x10      // Range: 0x00-TFT_BL_NOM_VAL
#define TFT_BL_TIMEOUT_MSB         0x00
#define TFT_BL_TIMEOUT_LSB         0x07//0x1E    // 30 seconds
#define TFT_ID                     0x00
#define SET_TOUCH_BUZZ_DURN        0x0A    // 10 x 10 ms

#define DEF_SCREEN_ON_DLY          5       // Default screen on-delay - 5 minutes
#define DEF_SCREEN_BRIGHTNESS      60      // Default screen brightness level

/* TFT Pixel/size related */
#ifdef TFT_SIZE_9P1
 #define TFT_HOR_PIXELS            800
 #define TFT_VER_PIXELS            480
#else
 #define TFT_HOR_PIXELS            480
 #define TFT_VER_PIXELS            272
#endif
#define  DISP_PAGE_IDH             0x00   // MSB ID of default pre-stored home Screen TML file on power-on reset
#define  DISP_PAGE_IDL             0x01   // LSB ID of default pre-stored home Screen TML file on power-on reset

#define UI_BFR_SIZE                (TFT_HOR_PIXELS * TFT_VER_PIXELS)
#define UI_PAGE_SIZE               (TFT_HOR_PIXELS)
#define UI_MAX_PAGES               (UI_BFR_SIZE / UI_PAGE_SIZE)
#define UI_RESPONSE_SIZE           2

/* RS232 Communication related */
#define BAUD_SET                   0x07    // Default: 115200
#define TP_PARAM                   0x83    // Touch panel enabled with KeyID to be sent
#define MAX_RX_SIZE_FROM_TC        0x18    // 24 bytes maximum

/* UI commands related */
#define UI_INIT_CMDS               3       // IMPORTANT - TBD
//#define TFT_FIFO_SIZE              24
//#define TFT_MAX_PARAM_SIZE         16
//#define UI_CONN_CMDS               2
//#define UI_CMD_LID_OPEN            0x01
//#define UI_CMD_LID_CLOSE           0x02

/* Wrappers for RS232 packet */
#define HEADER                     0xAA
#define FOOTER1                    0xCC
#define FOOTER2                    0x33
#define FOOTER3                    0xC3
#define FOOTER4                    0x3C

// Command ACK / NACK sequence
#define RESP_ACK                   ":>"
#define RESP_NACK                  "!>"
#define SIZE_OF_RESP               2



typedef enum {
  UI_INIT,
  UI_INIT_CONT,
  UI_INIT_DONE,
  UI_WAIT,
  UI_TX_CMD,
  UI_TX_CONT,
  UI_TX_DONE,
  UI_RX_CMD,
  UI_RX_CONT,
  UI_RX_DONE,
  UI_ERR,
} UI_STATE;


typedef struct {
  UI_STATE  state;
  uint32_t  dly_flag;
  uint32_t  poll_flag;
  uint8_t   tx_done;
  uint8_t   rx_done;
} UI_INFO;

#define UI_INFO_DEFAULT (UI_INFO) {UI_INIT, 0, 0, 0, 0 }

typedef struct {
  uint8_t    node_id;
  uint8_t    mode;         // 0-9600B8N1S-NP, 1-19200B8N1S-NP, 2-36400B8N1S-NP
} MB_CFG;


//////////////////////////////////
// Interface Structures - CU to UI
//////////////////////////////////
//  Events occurred - 165 bytes
typedef struct  {
  uint8_t    low_ln2_lvl;
  uint8_t    lvl_sens_err;
  uint8_t    low_rtd1_temp;
  uint8_t    low_rtd2_temp;
  struct   {
  uint8_t    lid_sts;
  TIME       time_since;
  uint8_t    dummy[3];
  }          lid_info;
  struct   {
  uint8_t    status;
  uint16_t   value;
  uint8_t    dummy;
  }          batt_info;
  struct   {
  uint8_t    status;
  uint16_t   durn;
  uint8_t    dummy;
  }          mains_info;
  struct   {
  uint8_t    status;
  uint16_t   durn;
  uint8_t    dummy;
  }          low_ln2_supply;
  struct   {
  uint8_t    status;
  uint16_t   durn;
  uint8_t    dummy[3];
  }          high_ln2_lvl;
  struct   {
  uint8_t    status;
  float      value;
  uint8_t    dummy[3];
  }          dp_info;
  struct   {
  uint8_t    status;
  TIME       last_calib[MAX_CALIB_PARAMS];
  uint8_t    dummy;
  }          calib_info;
  struct   {
  uint8_t    status;
  TIME       time_since;
  uint8_t    dummy[3];
  }          maint_info;
  struct {
	  uint8_t status;
	  TIME 	  time_since;
	  uint8_t dummy[3];
  }			FILLR_Info;
  struct   {
  uint32_t   status;
  TIME       start_time;
  TIME       end_time;
  }          selftest_info;
  uint8_t    act_on_key;
  uint8_t    high_rtd1_temp;
  uint8_t    high_rtd2_temp;
  uint8_t    calib_fail_thr;
  uint8_t    calib_fail_dp;
  uint8_t    calib_fail_rtd1;
  uint8_t    calib_fail_rtd2;
  uint8_t    ln2_sply_remind;
  uint8_t    lvl_chk_remindr;
  uint8_t    rtd1_bist_fail;
  uint8_t    rtd2_bist_fail;
} VP_EVNT_WR_STR;

//  Run-time Data - 189 bytes
typedef struct  {

  float      temp1;
  float      temp2;

  uint8_t    status[4];    // NEW - Re-mapped event/error/status/warning for UI ???
  float      batt_volt;
  TIME       rtc_info;
  struct   {
  uint8_t    rv_fw_version[4];
  uint8_t    rv_serial_id[4];
  }          rv_dev_info;

  uint8_t    mb_conn_peers;    // Max 247
  uint32_t   error_flag[3];
  uint32_t   event_flag[2];
  uint32_t   status_flag[2];
  uint32_t   warning_flag;
  struct   {
  uint8_t    th_str[8];
  uint8_t    dp_str[8];
  uint8_t    temp1_str[8];
  uint8_t    temp2_str[8];
  uint8_t    bvolt_str[8];
  }          data_str;
  uint16_t    level_status;
  uint16_t    rtd1_status;
  uint16_t    rtd2_status;
  uint16_t	 PowerStatus;
  uint16_t   Lid_status;
  uint16_t	 LoginStatus;
  uint16_t	 WiFiStatus;
  uint16_t 	 BLEStatus;
  uint16_t   MBStatus;
  uint16_t 	weight_overallStatus;

} VP_RT_WR_STR;

//////////////////////////////////
// Interface Structures - UI to CU
//////////////////////////////////
// Key press - any one of these keys at a time - 1 byte
typedef struct  {
  uint8_t    key_id;        // One of these keys: mute, Defog, stop, login, menu, home, fac_def_set, alm_test
} VP_KEY_RD_STR;

// User actions - 4 bytes
typedef struct  {
  uint8_t    event_id;      // Events whose response pattern are modifiable by user from UI
  uint8_t    action;        // Action requested: 0-Mute audio, 1-Clear for now, 2-Clear for a duration, 3-Clear until restart
  uint16_t   delay;         // Duration for mute
} VP_EVNT_RD_STR;

// Parameters - 340 bytes [SECONDARY]
// To force compiler to use 1 byte packaging
//#pragma pack(1)
typedef struct  {
  float      low_batt_volt;
  uint16_t   plumbing_options;
  uint8_t    ext_comm_cfg;  // Sintesy floating point number byte ordering
  struct   {
  uint16_t   on_time;
  uint16_t   off_time;
  }          rfv_ctrl;
  MB_CFG     mb_cfg;
  struct   {
  uint8_t    fw_version[4];
  uint32_t   cfc_ser_num;
  uint8_t    plmb_combo;            // Not used for now
  }          dev_info;
  float      high_ln2_alm;
  float      low_ln2_alm;

  struct   {
  uint8_t    state;         // 0-No fill, 1-Fill command received
  uint8_t    mode;          // 0-Sequential fill, 1-Simple OFAF, 2-Auto (local request based)
  }          ofaf;
  struct   {
  uint8_t    state;         // 0-Clear the reminder, 1-Activate the reminder
  TIME       time;          // Time stamp of new reminder activation
  }          sch_fill_rem;  // Remind the user for LN2 filling schedule
  struct   {
  TIME       time;          // Time stamp of new reminder activation
  uint8_t    state;         // 0-Clear the reminder, 1-Activate the reminder
  }          chk_lvl_rem;   // Remind the user for periodic level checking
  struct   {
  uint8_t    state;         // 0-Clear the reminder, 1-Activate the reminder
  TIME       time;          // Time stamp of new reminder activation
  }          maint_rem;     // Remind the user for preventive maintenance

  //created by Ajay
  struct {
 uint8_t	state;
 TIME		time;
  }Fill_Rem;

  struct   {
  TIME       time;          // Time stamp of new reminder activation
  uint8_t    state;         // 0-Clear the reminder, 1-Activate the reminder
  }          ln2_supp_rem;  // Remind the user for scheduled LN2 supply
  uint8_t    screen_on_dly; // User modifiable parameter at UI and control the active screen duration
  uint8_t    brightness;    // User modifiable parameter at UI and control the brightness
  uint8_t    cryolink_phase2_sel; // 0 - Cryolink, 1- Phase2 Link
  // Level Settings Structure --------------------------------------------

  //Temperature Stetings Stucture---------------------------------------
  struct   {
  float      value;
  uint8_t    units;         // 0-Celsius, 1-Fahrenheit, >1- Unknown
  uint8_t    is_enabled;
  }high_temp1;
  struct   {
  float      value;
  uint8_t    units;         // 0-SI, 1-US metric
  uint8_t    is_enabled;
  }low_temp1;
  struct	{
  float      value;
  uint8_t    units;         // 0-Celsius, 1-Fahrenheit, >1- Unknown{
  uint8_t    is_enabled;
  }high_temp2;
  struct   {
  float      value;
  uint8_t    units;         // 0-SI, 1-US metric
  uint8_t    is_enabled;
  }low_temp2;
  uint8_t 	 tempUnits;
  uint8_t    t2enable;
  //----------------------------------------------------------------
  uint16_t   Empty_en;
  uint16_t   Refillsoon_en;
  uint16_t   Fill_en;

  uint16_t	 RemoteAlarmDelaySwitch;
  uint16_t   rem_alm_dly;
  uint16_t	 AudiableRepeatDelaySwitch;
  uint16_t   aud_rep_dly;
  uint16_t	 LN2UsageAlarmDelaySwitch;
  uint16_t   usg_alm;
  int16_t    Battery_mode_sw_over;

  int16_t    STM_Brightness;
  int16_t    STM_TouchSound;
  int16_t    STM_ScreenSaverTime;
  int16_t 	 STM_LED_Brightness;
  int16_t	 STM_StatusLed;

  int16_t    STM_DeviceIDCryolink;
  int16_t    STM_BaudRateCryolink;
  int16_t    STM_FPByteOrderCryolink;
  int16_t    STM_DeviceIDPhasetwo;
  int16_t    STM_BaudRatePhasetwo;
  int16_t    DataRead_En;
  int16_t    STM_FillMethodPhasetwo;
  int16_t    Fill_cmd;
  int16_t    STM_EN_4_20_mA;

  int16_t    ScheduledFillRemainderEn;
  int16_t    MaintRemainderEn;
  int16_t    FillRemainderEn;


  int16_t    Fill_HoursSettings;
  int16_t    Fill_MinutesSettings;
  int16_t    Fill_AMPMSettings;
  int16_t    Fill_DaySettings;
  int16_t    Fill_Hours12Settings;


  int16_t    FILLR_MonthSettings;
  int16_t    FILLR_DateSettings;
  int16_t    FILLR_YearSettings;
  int16_t    FILLR_Hours12Settings;
  int16_t    FILLR_HoursSettings;
  int16_t    FILLR_MinutesSettings;
  int16_t    FILLR_AMPMSettings;


  int16_t    Maint_Hours12Settings;
  int16_t    Maint_MonthSettings;
  int16_t    Maint_DateSettings;
  int16_t    Maint_YearSettings;
  int16_t    Maint_HoursSettings;
  int16_t    Maint_MinutesSettings;
  int16_t    Maint_AMPMSettings;

  int16_t    RTC_Hours12Settings;
  int16_t    RTC_AutoTimeSyncControl;
  int16_t    RTC_DateFormat;
  int16_t    RTC_TimeFormat;
  int16_t    RTC_MonthSettings;
  int16_t    RTC_DateSettings;
  int16_t    RTC_YearSettings;
  int16_t    RTC_HoursSettings;
  int16_t    RTC_MinutesSettings;
  int16_t    RTC_AMPMSettings;

  int16_t    RTD_T1_High_Error_flg;
  int16_t    RTD_T1_Low_Error_flg;
  int16_t    RTD_T2_High_Error_flg;
  int16_t    RTD_T2_Low_Error_flg;
  int16_t    Open_detection_rtd1;
  int16_t    Short_detection_rtd1;
  int16_t    Open_detection_rtd2;
  int16_t    Short_detection_rtd2;

  int16_t   Slave_TER_Switch;
  int16_t   Master_TER_Switch;
  int16_t 	EXD_MD_Switch_settings;
  int16_t 	Chan1_Switch_settings;
  int16_t 	Chan2_Switch_settings;
  int16_t 	Chan1_Selction_settings;
  int16_t 	Chan2_Selction_settings;
  int16_t 	Source1_sel_settings;
  int16_t 	Chan1_vol_cur_settings;
  int16_t 	Chan2_vol_cur_settings;
  int16_t 	Chan3_Switch_settings;
  int16_t 	Chan4_Switch_settings;
  int16_t 	Chan3_Selction_settings;
  int16_t 	Chan4_Selction_settings;
  int16_t 	Source2_sel_settings;

  int16_t T1_MIN_CONFIG_CH1_settings;
  int16_t T2_MIN_CONFIG_CH1_settings;
  int16_t DP_MIN_CONFIG_CH1_settings;
  int16_t T1_MIN_CONFIG_CH2_settings;
  int16_t T2_MIN_CONFIG_CH2_settings;
  int16_t DP_MIN_CONFIG_CH2_settings;
  int16_t T1_MAX_CONFIG_CH1_settings;
  int16_t T2_MAX_CONFIG_CH1_settings;
  int16_t DP_MAX_CONFIG_CH1_settings;
  int16_t T1_MAX_CONFIG_CH2_settings;
  int16_t T2_MAX_CONFIG_CH2_settings;
  int16_t DP_MAX_CONFIG_CH2_settings;

  int16_t T1_MIN_CONFIG_CH3_settings;
  int16_t T2_MIN_CONFIG_CH3_settings;
  int16_t DP_MIN_CONFIG_CH3_settings;
  int16_t T1_MIN_CONFIG_CH4_settings;
  int16_t T2_MIN_CONFIG_CH4_settings;
  int16_t DP_MIN_CONFIG_CH4_settings;
  int16_t T1_MAX_CONFIG_CH3_settings;
  int16_t T2_MAX_CONFIG_CH3_settings;
  int16_t DP_MAX_CONFIG_CH3_settings;
  int16_t T1_MAX_CONFIG_CH4_settings;
  int16_t T2_MAX_CONFIG_CH4_settings;
  int16_t DP_MAX_CONFIG_CH4_settings;


	uint16_t ERRORS_ONLY_en;
	uint16_t TEMP_en;
	uint16_t LEVEL_en;
	uint16_t VACUUM_en;
	uint16_t STORAGE_FAIL_en;
	uint16_t MAINS_BATT_en;
	uint16_t NETWORK_ISSUES_en;
	uint16_t BLINK_CTRL_en;
	uint16_t NO_ERROR_CLR;
	uint16_t ERRORS_CLR;
	uint16_t WARNINGS_CLR;
	uint16_t UPDATES_CLR;
	uint16_t NEW_COLOR_Select;
} VP_PARAM_RW_STR;


typedef struct  {
  int16_t    Update_MonthSettings;
  int16_t    Update_DateSettings;
  int16_t    Update_YearSettings;
  int16_t    Update_HoursSettings;
  int16_t    Update_MinutesSettings;
}UPDATEStruct;

typedef struct  {
  int16_t Group_en;
  int16_t Group_sel;
  int8_t  Group_A[90];
  int8_t  Group_B[90];
  int8_t  Group_C[90];
  int8_t  Group_D[90];
  int16_t Group_col[32];
}GROUPStruct;

typedef struct  {
  int8_t  Supply_con[20];
  int8_t  Service_con[20];
}SYSTEMStruct;


  /*FV_STUCK_ALM_STATE, LN2_SUPPLY_ALM_DLY, REMOTE_ALM_DLY, AUD_REP_ALM_DLY, \
    LN2_USAGE_ALM_STATE, GBPV_STUCK_ALM_STATE, \
 Login credentials - 18x10 = 180 bytes [TERTIARY]. Also the same typedef for current login info*/
typedef struct  {
  uint8_t    user_id[MAX_ID_SIZE];
  uint8_t    admin_id[MAX_ID_SIZE];
  uint8_t    user_pswd[MAX_PSWD_SIZE];
  uint8_t    emp_id[MAX_ID_SIZE];
  uint8_t    attempts;
  uint8_t    user;
  uint8_t    lock;
} VP_CRED_RW_STR;

#define VP_CRED_RW_STR_DEFAULT (VP_CRED_RW_STR) {{0, },{0, },{0, }, {0, }, WAIT_TO_LOGIN, 0, 0,}

// Calibration points - 80 bytes
typedef struct {
  float      th_param[MAX_CALIB_POINTS];
  float      rtd1_param[MAX_CALIB_POINTS];
  float      rtd2_param[MAX_CALIB_POINTS];
  float      rtd3_param[MAX_CALIB_POINTS];
  float      rtd4_param[MAX_CALIB_POINTS];
  uint8_t    param_under_calib;
  uint8_t    step_num;
  uint8_t    status;          // 0-Calibration mode inactive, 1-Calibration mode active
  uint8_t    step_ack;        // 0-Req ACK by UI, 1-ACK, 2-NACK
  uint8_t    WaitSamples;
  uint8_t    WaitForInput;
  uint8_t    IsSinglePointCalibration;
  uint8_t    DP_calibration_time;
  uint8_t    RTD1_calibration_time;
  uint8_t    RTD2_calibration_time;
  uint8_t    Calibration_time;
  uint8_t    calibration_stop;

} VP_CAL_RD_STR;

#define CAL_PT_DEFAULT (VP_CAL_RD_STR) { \
  {LVL_CAL_PT1_VAL,  LVL_CAL_PT2_VAL,  LVL_CAL_PT3_VAL}, \
  {RTD1_CAL_PT1_VAL, RTD1_CAL_PT2_VAL, RTD1_CAL_PT3_VAL}, \
  {RTD2_CAL_PT1_VAL, RTD2_CAL_PT2_VAL, RTD2_CAL_PT3_VAL}, \
  {RTD3_CAL_PT1_VAL, RTD3_CAL_PT2_VAL, RTD3_CAL_PT3_VAL}, \
  {RTD4_CAL_PT1_VAL, RTD4_CAL_PT2_VAL, RTD4_CAL_PT3_VAL}, \
  0, \
  0, \
  0, \
  0, \
  0, \
  0,0,1,1,1,1,1,\
}



typedef struct{
	uint8_t Access_allow;
}
Login_access;
/* Function declarations */
void   Uart1_TxCallback(UART_HandleTypeDef *huart);
void   Uart1_RxCallback(UART_HandleTypeDef *huart);
void   UI_init(void);
void   UI_proc(void);
int8_t UI_send_cmd(uint8_t *bfr, uint8_t size, uint8_t req_resp);
void UpdateScreenSaverSettings();
void UpdateUIRTC(void);
void InitUIVariables(void);

#endif /* UI_H_ */
