/*
 * MyMain.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef MYMAIN_H_
#define MYMAIN_H_

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <tx_port.h>
#include <stdbool.h>
#include <stm32u5xx_hal.h>
#include <stm32u5xx_hal_def.h>
#include <stm32u5xx_hal_uart.h>
#include "UserCredentials.h"
#include "ux_api.h"
#include "fx_api.h"

#include "ADC.h"
#include "AT_CMD.h"
#include "Calibration.h"
#include "EEPROM.h"
#include "ErrorLog.h"
#include "LevelMon.h"
#include "Modbus.h"
#include "MsgProc.h"
#include "RS485.h"
#include "TempMon.h"
#include "RTC.h"
#include "RTD.h"
#include "SPM.h"
#include "UI.h"
#include "USB.h"
#include "ulog.h"
#include "ulog_if.h"
#include "app_filex.h"
//#include "Delay.h"

#include "BLE.h"
#include "string.h"
#include "LED.h"
#include "ux_host_msc.h"

#include "MAX11206.h"
#include "WIFI.h"

//newly
#include "DisplayTxHandlers.h"
#include "FactoryReset.h"
#include "DispUIkeyhandler.h"
#include "UserAccess.h"
#include "Add_4_20mA.h"

#ifndef NULL
 #define NULL      0
#endif

/* Defines */

/////////////////////////////////////
// Defines for process configuration
/////////////////////////////////////

#define APPLICATION_STRT_ADDR      0x08000000
#define	MCU_IRQS	          70u
//#define EN_MQTT                  // Enable this for MQTT mode for communication over WiFi
//#define EN_AWS_IOT
//#define EN_HTTP_OVER_WIFI        // Enable this for HTTP server functionality over WiFi
#define EN_BUZZER                  // Enable this for on-board buzzer for error conditions
//#define EN_IWDG                  // Disable this if watch-dog functionality is to be blocked
//#define EN_CTRS
#define EN_EMMC
#define EN_MFNB_PD                 // Mains fail with no battery - interrupt to initiate shutdown
//////////////////////////////////////////
// Defines for Process run configurations
//////////////////////////////////////////
#define USR_CFG_REG_DEF_VAL        0xFFEFF8AA

// Manufacturer, Model and Release Details
#if 1  // 0:60Hz, 1:50Hz
 #define PWR_LINE_FREQ_60HZ
#else
 #define PWR_LINE_FREQ_50HZ
#endif

#define VENDOR_NAME                "PhaseTwo"
#define PRODUCT_CODE               "HCHE20"
#define REVISION_CODE              "1.00"

#define APP_VER                    "V1.0"
#define UI_FW_VER                  "A1.00"
#define APP_FW_VER                 "A1.00"          // Application Firmware version string
#define BLR_FW_VER                 "B1.00"          // Boot-loader Firmware version string
#define HW_REV_STR                 "V1.0"


#define PUBLIC_KEY                 0x1234
#define PRIVATE_KEY                0x55AA55AA

#define DATLOG_INTERVEL            (5*60/500)       // (5mins x 60seconds) / 500ms
// Masks for log enable/disable
#define DATA_LOG_ENABLE_BITS       0x00000001
#define EVENT_LOG_ENABLE_BITS1     0xFFFFFFFF
#define EVENT_LOG_ENABLE_BITS2     0xFFFFFFFF
#define ERROR_LOG_ENABLE_BITS1     0xFFFFFFFF
#define ERROR_LOG_ENABLE_BITS2     0xFFFFFFFF
#define ERROR_LOG_ENABLE_BITS3     0xFFFFFFFF
#define STATUS_LOG_ENABLE_BITS1    0xFFFFFFFF
#define STATUS_LOG_ENABLE_BITS2    0xFFFFFFFF
#define WARNING_LOG_ENABLE_BITS    0xFFFFFFFF

// Log masks for remote alarm activations
#define EVT_GR1_FOR_REM_ALM        0x00000000
#define EVT_GR2_FOR_REM_ALM        0x00000000
#define ERR_GR1_FOR_REM_ALM        0x00000000
#define ERR_GR2_FOR_REM_ALM       (ERR_TEMP1_HIGH | ERR_TEMP1_LOW | ERR_LEVEL_HIGH | ERR_LEVEL_LOW | \
                                   ERR_GBP_VALVE_STUCK | ERR_FILL_VALVE_STUCK | ERR_DP_SENSOR | ERR_TEMP2_HIGH | \
                                   ERR_TEMP2_LOW)
#define ERR_GR3_FOR_REM_ALM        0x00000000
#define STS_GR1_FOR_REM_ALM        0x00000000
#define STS_GR2_FOR_REM_ALM        0x00000000
#define WRN_GR1_FOR_REM_ALM        (WRN_LOST_MAINS_VOLT)
// Log masks for remote buzzer activations
#define EVT_GR1_FOR_REM_BZ         0x00000000
#define EVT_GR2_FOR_REM_BZ         0x00000000
#define ERR_GR1_FOR_REM_BZ         0x00000000
#define ERR_GR2_FOR_REM_BZ        (ERR_TEMP1_HIGH | ERR_TEMP1_LOW | ERR_LEVEL_HIGH | ERR_LEVEL_LOW | \
                                   ERR_GBP_VALVE_STUCK | ERR_FILL_VALVE_STUCK | ERR_DP_SENSOR | ERR_TEMP2_HIGH | \
		                           ERR_TEMP2_LOW)
#define ERR_GR3_FOR_REM_BZ         0x00000000
#define STS_GR1_FOR_REM_BZ         0x00000000
#define STS_GR2_FOR_REM_BZ         0x00000000
#define WRN_GR1_FOR_REM_BZ        (WRN_LOST_MAINS_VOLT)

/* Declaration of enumerators and typedefs */
#define SW_TMR_ADC              0
#define SW_TMR_SD_DLY           1
#define SW_TMR_ETH              2
#define SW_TMR_HB               3
#define SW_TMR_USB_POLL         4
#define SW_TMR_USB_WR_POLL      5
#define SW_TMR_RS485_POLL       6
#define SW_TMR_RS485_DLY        7
#define SW_TMR_DIS_POLL         8
#define SW_TMR_WIFI_DLY         9
#define SW_TMR_UI_TX_POLL       10
#define SW_TMR_UI_DLY           11
#define SW_TMR_4_20mACOMM_POLL  12
#define SW_SD_CARD_DLY          13
#define SW_TMR_RFV_POLL         14
#define SW_TMR_RTD2_EOC         15
#define SW_TMR_RTD_DLY         16
#define SW_TMR_BIST_DLY         17
#define SW_TMR_INIT_DLY         18
#define SW_TMR_batt_dly         19
#define SW_TMR_FILE_RDY         20
#define SW_TMR_RS485_RESP       21
#define SW_TMR_SHUTDOWN         22
#define SW_TMR_RFV_ON_DLY       23
#define SW_TMR_DM               24
#define SW_TMR_WIZ              25
#define SW_TMR_TCP_POLL         26
#define SW_TMR_HTTP_POLL        27
#define SW_TMR_BUZ_DLY          28
#define SW_FLASH_TIMEOUT        29
#define SW_TMR_USB_SD_POLL      30
#define SW_SD_BIN_CHK_DLY       31
#define SW_TMR_BIST             32
#define SW_TMR_XMODEM_TIMEOUT   33
#define SW_TMR_XMODEM_REC_TIMEOUT   34
#define SW_TMR_REINIT_SPI_TIMEOUT 35
#define SW_TMR_LAST             36


 /* This should be the last SW timer and should not be used */
#define NUM_SW_TMRS            (SW_TMR_LAST)

/* Delays / timeout values in milli-seconds for various asynchronous events */
#define DLY_SHUTDOWN            2000	//3s
#define DLY_ADC_STRT            10
#ifdef EN_ADC_DMA
 #define DLY_ADC_POLL           200    // 200ms
#else
 #define DLY_ADC_POLL           10     // 10ms
#endif
#define  DLY_BUZ                800   //30msec
#define DLY_MOV_AVG_CAL         9000//3000   // 9.sec//4.5 sec
#define DLY_INIT	            9000//3000   // 9.sec//4.5 sec
#define DLY_SPI_TIMEOUT         100
#define DLY_I2C_POLL            1000
#define DLY_I2C_TIMEOUT         20
#define DLY_EEPROM_WR_DLY       1      // 2ms
#define DLY_TC_EOC              200    // 200ms, Min: 205ms
#define DLY_TC_TIMEOUT          100
#define DLY_RTD_STRT            2000   // 2s
#define DLY_RTD_EOC             1000   // 1s, Min: 205ms
#define DLY_RTD_TIMEOUT         20     // 20ms
#define DLY_CONV_WAIT_TIMEOUT   200    // 200ms

#define DLY_RTD_RDY_TIMEOUT     20000  // 20s
#define DLY_RTD_LPF_RDY         10000  // 10s
#define DLY_DP_LPF_RDY          20000  // 20s
#define DLY_RS485_RESP          100    // 100ms
#define DLY_RS485_POLL          10000  // 10 s
#define DLY_RS485_POLL          10000  // 100 s
#define DLY_RS485_STRT          2000   // 2s
#define DLY_RS485_TIMEOUT       250     // 25ms
#define DLY_MB_TX_POLL          25     // 25ms
#define DLY_MB_RX_POLL          25     // 25ms
#define DLY_ETH_STRT            5000   // 5s
#define DLY_ETH_TXN_POLL        100    // 100ms
#define DLY_WIZNET_RST          20     // 20ms
#define DLY_WIZNET_MON          2000   // 2 s
#define DLY_DHCP_TIMEOUT        20000  // 20 seconds
#define DLY_WIFI_STRT           2500   // 2s
#define DLY_WIFI_POLL           100    // 10ms
#define DLY_WIFI_CMD_WAIT       40     // 40ms
#define DLY_WIFI_TXN_TIMEOUT    100    // 100ms
#define DLY_WIFI_RST            10     // 10ms
#define DLY_WIFI_TIMEOUT        100    // 100ms
#define DLY_WIFI_NET_DETECT     20     // 20ms
#define DLY_WIFI_NET_LOGIN      100    // 100ms
#define DLY_WIFI_RESP_DLY       1      // 1ms
#define DLY_TOUCH_dly           100    // 150ms
#define DLY_UI_STRT             2000   // 2s
#define DLY_UI_TIMEOUT          20     // 20ms
#define DLY_UI_TX_POLL          200    // 500ms//old rv changed -50
#define DLY_USB_SD_POLL         40    // 500ms
#define DLY_USB_WR_POLL         40    // 500ms
#define DLY_UI_SEC_POLL         200    // 500ms
#define DLY_UI_TER_POLL         1000   // 1s
#define DLY_HB_TIMEOUT          500    // 500ms
#define DLY_HB_RUN_MODE         500    // 500ms
#define DLY_HB_BOOT_MODE        100    // 100ms
#define DLY_HB_DISPLAY_MODE     1000
#define DLY_SPM_START           1000   // 1s
#define DLY_SPM_POLL            100    // 100ms
#define DLY_DEFOG_TIMEOUT       60000  // 1 minute
#define DLY_DFG_QCK_CHL_DLY     60000  // 1 minute
#define DLY_FSOL_POLL           120000 // 2minutes
#define DLY_CALIB_WAIT          300000 // 5minutes
#define DLY_CALIB_DLY           60000  // 1minute
#define DLY_PM_START            1000   // 1s
#define DLY_PM_POLL             1000   // 1s
#define DLY_SD_START            10     // 10ms
#define DLY_SD_WAIT             2000    // 500ms
#define DLY_SD_POLL             5000   // 5s
#define DLY_SD_TIMEOUT          30     // 30ms
#define DLY_SD_TX_TIMEOUT       1000   // 1s
#define DLY_USB_POLL            25     // 25ms
#define DLY_USB_DLY             10000  // 10s
#define DLY_BIST_DLY            1000   // 1sec
#define DLY_FV_TIMEOUT          300000 // 300 seconds (5 mins) - Timeout to teach Incremental step of 1 inch
#define DLY_GBPV_TIMEOUT        10000  // 10 seconds
#define DLY_PURGE_TIMEOUT       24*60*60*1000  // 24 Hours purge time - TBD
#define DLY_LOGIN_ATTEMPT       15*60*1000     // 15 minutes lockout time after 3 consecutive login failed attempts
#define DLY_LID_OPEN_TIMEOUT    60000  // 1 minute
#define DLY_DNS_WAIT            50     // 50ms
#define DLY_DM_RDY_DLY          5000   // 5s
#define DLY_DM_TIMEOUT          2      // 2ms
#define DLY_WIZ_TIMEOUT         100    // 100ms
#define DLY_TCP_POLL            100000 // 100s
#define DLY_HTTP_START          10000  // 10s
#define DLY_HTTP_TXN_POLL       100    // 100ms
#define DLY_FLASH_ERASE_DLY     1000   // 1s
#define DLY_FLASH_PAGE_DLY      10     // 1ms
#define DLY_WAY_TIMEOUT         667   //1000ms
#define DLY_CONT_SHUTDOWN       5000  //10 secs
/* Declaration of external variables */
extern  __IO uint32_t   uwTick;
extern  RTC_TimeTypeDef Time;
extern  RTC_DateTypeDef Date;
extern  RTC_TimeTypeDef NewTime;
extern  RTC_DateTypeDef NewDate;
extern  uint8_t SensorFaultDetected;
extern  USB_INFO usb_info;
extern  LevelDataStruct LevelData;
extern FX_MEDIA        mmc_disk;
extern FX_MEDIA        *media;
extern SD_INFO SD_info;
extern uint8_t LED_disconnect;
extern RS485_INFO RS485_info;
extern volatile uint32_t UART3DMACtr;
//extern VERSIONS Control_versions;
//ADC
//extern ADC_HandleTypeDef hadc1;
extern uint8_t TargetPage;
//extern IWDG_HandleTypeDef hiwdg;
extern VP_PARAM_RW_STR vp_param_rw_str;
extern SPM_INFO spm_info;
//extern uint8_t Remote_access;
//extern WIFI_INFO  wifi_info;
//extern IOT_INFO    iot_info;

//calibration.c
extern VP_CAL_RD_STR cal_par_str;        // Backed up in EEPROM
extern VP_CAL_RD_STR vp_cal_pt_rd_str;

//codemem.c
extern I2C_HandleTypeDef  hi2c1;
extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef  hspi1;
extern SPI_HandleTypeDef  hspi3;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_uart3_rx;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;

//extern SD_CIR_BUF Flash_rd_cir_buf[IOT_CIR_BUF_DEPTH];  // Circular buffer array that holds data to be read from SD card
extern TEMP_INFO temp_info[NO_OF_RTD_SENSORS];
extern SD_INFO    SD_info;
extern uint32_t   hb_blink_dly;
extern uint8_t    active_bank[4];       // Byte1 (current bank ID) => All except 2 - Boot, 2 - Application, Bytes[3:1] => FWDL Count
extern UPDATE Update;
//
////ctrs.c
//extern CTRS_CAL_VAR     CTRS_cal_var;
//extern CTRS_WR_PARA ctrs_ws_para;

//decompress.c
extern volatile uint8_t UARTTxCompleted;

//Displaycmdhandler.c
extern uint8_t CalibDataReqSent;
extern uint8_t RunTimeDataReqSent;
extern uint8_t UserActionEventDataReqSent;
extern UpdateRemainderSettingsStruct UIUpdateRemaindersettings;
extern UpdateGroupSettingsStruct  UpdateGroupsettings;
extern int16_t Group_inactive_nodes[32];
//extern Network_4_20mAStruct UI_4_20mAsettings;

//DisplayCommdriver.c
extern uint8_t UART5RxErrorDetected;
extern volatile uint32_t UART5DMACtr;
extern uint16_t UICommFaildCounter;
//extern VERSIONS Control_versions;

//displayrxhandlers.c
extern UI_INFO UI_info;

//DisplayTxhandlers.c
extern VP_EVNT_WR_STR   vp_evnt_wr_str;      // Events - 165 bytes
extern VP_RT_WR_STR     vp_rt_wr_str;        // Real time data - 153 bytes
extern TemperatureSettingsStruct UITempeSettingsData;
extern LEVELSettingsStruct       UILevelSettingsData;
extern LEDSettingsStruct 		  UILEDSettingsData;
extern AlarmSettingsStruct UIAlarmSettingsData;
extern LidFunctionsStruct UILidFunctionSettingsData;
extern DisplayDataStruct UIDisplayData;
extern ModbusDataStruct UIModbusData;
extern ScheduledEventStruct UIScheduledEventData;
extern RemainderSettingsStruct UIScheduledFillData;
extern SCHRemainderSettingsStruct UIFillRemainderData;
extern MaintenanceRemainderSettingsStruct UIMaintenanceRemainderSettings;
extern DateTimeSettingsStruct UIDateTimeSettings;
//extern LEVEL_INFO      level_info;
extern UpdateGroupSettingsStruct  UIUpdateGroupsettings;
//extern Network_CONFIG_Struct UI_4_20mAconfigSettings;
//extern CTRSTemperatureSettingsStruct UICTRStemperature;


/* Declaration of global variables */
extern uint8_t test_alarm_flg;
extern int rssi_value;
extern uint8_t mute_seq_start_flg,mute_rem_start_flag;
extern uint8_t Init_Wait_Delay_for_UI_flg;
extern float   Gbp_avg_value;
extern float Therm_resis;
extern uint8_t RTC_Year,RTC_Month,RTC_Date,RTC_Hour,RTC_Min,RTC_Sec;
extern uint8_t modbus_timer;
extern uint8_t GraphPageActive;
extern int16_t calib_count;
//extern GROUPStruct  GroupStruct;
extern uint8_t update_error;
extern COMM_PAR  comm_par;
extern  uint8_t firmware_update_4_20ma;
extern uint8_t Reminder_Id;

extern SensorReadingStruct SensorReading;
extern CAL_INFO   cal_info;
extern CAL_PT_STR cal_pt_str[MAX_CALIB_PARAMS];
extern uint8_t  error_en;
extern uint8_t actuation;


extern uint8_t UserStopCommand;
extern uint8_t UpdateErrorScreen;
extern TemperatureConvStruct TempSettingsConv;

extern uint16_t GraphSelectionId;
extern uint16_t GraphScaleSelection;

extern uint8_t repeat_local;
extern uint16_t DayGraphScaleSelection;
extern RTC_DateTypeDef DayGraphDateSelection;
extern uint16_t DayGraphActive;
extern uint16_t DayGraphDate;
extern uint16_t DayGraphMonth;
extern uint16_t DayGraphYear;
extern uint8_t DayGraphloaded;
extern PLUMBING_OPTIONS Plumbing_options;
extern uint8_t Buzzer_mute;
extern uint8_t clear_iot_popup;


extern uint8_t flag;

//errorlog.c
extern Wifi UIwifi;
extern uint8_t Mains_fail;

//factoryreset.c
extern const char *verStr;
extern uint16_t crc16;
extern const Eeprom_address eep_user_reg[];
extern uint8_t factory_load_delay_flg;
extern uint8_t factory_msg1_display_delay_flg;
extern uint8_t factory_msg2_display_delay_flg;
//extern uint8_t Factory_Reset_PowerON;
extern uint8_t Login_id_cnt;
extern VP_CRED_RW_STR   login_cred[MAX_ADMIN_ACCOUNTS+MAX_USER_ACCOUNTS+1];
extern User_access   UIUserAccess;
extern FreezerStruct UIFreezerStruct;
extern uint8_t firmware_timer_en;
extern UPDATEStruct UpdateStruct;
extern uint8_t update_flag;
extern SYSTEMStruct SystemStruct;
extern uint8_t Edm_Mode_Sts;
extern uint8_t serial_data[36];


extern uint8_t MFValveFaultDetected;
extern TIM_HandleTypeDef htim3;
extern volatile uint8_t LogGraphDataEn;
extern uint8_t MFV_ALARM_DIS;
////////////////////////////////////////////////////completed
//fillvalvecontrol.c

//extern LN2USAGE Ln2Usage;
extern uint8_t clear_reminder;

extern int16_t LevelReadingAtFillingStop;
extern uint8_t LN2ImproperSupplySensed;
extern uint8_t ValveSensorFaultDetected;
extern uint8_t ValveCheckTimeOut;
extern unsigned char defog_key_sence;
extern uint8_t once_variable;
extern uint8_t GBP_ALARM_DIS;
extern uint8_t  waytimer;
extern uint8_t valve3waytimeout;
extern uint8_t   LN2SupplyTimeOut;
extern uint8_t FirmwareUpdateModeOn;

//levelmon.c

extern uint8_t    clear_reminder;
extern uint32_t   out_error;

extern ADC_INFO   ADC_info;
extern uint16_t   avg_dp_val, prv_avg_dp_val;
extern volatile uint8_t ADCDataReady;
extern int32_t diff_count;
extern uint8_t second_time;

//lidfunctions.c

extern uint8_t DataLogTimeOut;
extern uint8_t ln2usagetimeout;

//modbus.c
extern uint32_t   System_Status[];

//msgproc.c

extern uint32_t         System_Status[];
extern uint32_t         usid;
extern uint8_t test_buf[255];

//mymain.c

extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern uint8_t factory_msg1_display_delay,factory_msg2_display_delay;
extern uint8_t bat_down;
extern uint8_t flag_no_bat;
//extern UPDATE  Update;
extern uint8_t AllBistComplete;
extern uint8_t reinitialize_SPI;

//rs485.c

extern uint32_t out_error;
extern uint8_t master_shed_fill;

//RTC.c

//rtd.c

//extern TEMP_INFO temp_info[];
extern uint8_t bist_sts_RTD[];
extern uint8_t rtdsts1,rtdsts2;

//sdcard.c
extern uint32_t   SystemCoreClock;
extern uint8_t semaphore[4]; // Semaphores for shared peripherals - SPI1, SPI3, I2C1 & FS
extern uint8_t Data_buffer[SD_BUFFER_SETTINGS];
extern uint8_t Data_buffer1[Data_Group_buffer];
//extern uint8_t FirmwareUpdateModeOn;

//spm.c

//tempmon.c
//extern RTD_ERRORS rtd_errors;

//UI.c
extern uint8_t UI_Communication_Disconnected_Flg;
extern uint8_t usb_flag;

extern uint8_t Time_start,one_day_3way;
extern uint16_t Time_out;
extern uint8_t Time_home_ret;
extern uint8_t UI_RTC_set_flg;
extern uint8_t Rtc_Toggle,factory_ok_enable_flg,just_for_display_flg;
extern uint8_t Shut_Load,shutdown_flag,factory_load_delay;

//uisettings.c
extern uint16_t mute_seq_time,mute_rem_time;
extern uint16_t MUTE_TIME_DELAY,MUTE_REM_TIME_DELAY;
extern volatile uint8_t PrevTempUnit;
extern volatile uint8_t PrevLevelUnit;
extern volatile uint8_t PrevTimeFormat;
extern uint8_t FrezzID[9];
extern uint8_t NW_ID[8];
extern UpdateSystemSettingsStruct UIUpdateSystemSettings;
extern uint8_t sd_flag;
extern CalibDataStruct UISensorCalibData;
extern uint8_t calibration_canceled_flg;
extern uint8_t hours;
extern uint8_t send_email_iot;
extern uint8_t send_test_mail;
//extern SSID_Details Wifi_ssid[11];

//usb.c
extern uint32_t hb_flag;
extern char FUFileName[16];
extern char CFFileName[16];

extern TX_SEMAPHORE emmc_semaphore;

//useraccess
extern const  Register_address address_reg[];
extern AddUserStruct UIAddUserStruct;
extern login_credentials UIlogin;
extern uint8_t Account_creation;
extern AWS UIAws;
extern uint8_t link_disconnect,peer_disconnect;

//wifi.c
extern BLE_INFO  BLE_info;
extern uint16_t read_size;
extern DIRECTORY_LIST dirList[MAX_ITEMS];
extern FILE_EXPLORER File_add[];
extern char currentPath[];
extern char g_filename_noext[];

#define LED_HB                      1
#define Power_status_LED            2
/* Function Declarations */
void     my_init(void);
void     my_main(void);
void     reset_controller(uint32_t app_vector_addr);
void     GPIO_init(void);
//void     PowerFailInt(void);
uint32_t GetSysTick(void);
uint8_t  ChkSysTickDly(uint32_t dly);
void     Set_delay(uint8_t tmr_id, uint32_t delay, uint32_t *flag);
void     Dis_sw_tmr(uint8_t tmr_id);
//void     Exti1Callback(void);
//void     Exti4Callback(void);
//void     Validate_user_cred(void);
//void     Daylight_saving(void);
//void     UI_selftest(void);
//void     Check_bist_condition (void);
void ReceiveWifiBin(uint8_t* Buf, uint32_t *Len);

#endif /* MYMAIN_H_ */
