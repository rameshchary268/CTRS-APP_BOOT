/*
 * DisplayTxHandlers.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef DISPLAYTXHANDLERS_H_
#define DISPLAYTXHANDLERS_H_

#include "DispVPAddrList.h"
#include "stdint.h"

#define  ST_MCU_VP  0


typedef struct{
	int32_t Loadcell1;
	int32_t Loadcell2;
	int32_t Loadcell3;
	int32_t Loadcell4;
	int32_t Loadcell5;
	int32_t LoadcellAvg;
	int32_t pressure_value;
} CTRSStruct;





typedef struct{
	int16_t Mute_control;//0x00
	int16_t T1Value;		//0x02
	int16_t T2Value;
	int16_t level;
	int16_t weight_overall;		//0x08
	int16_t mains_battery;
	int16_t battery_values;
	int16_t login_index;
	int16_t internet_status;	//0x10
	int16_t BLE_Status;
	int16_t Mod_Status;
	int16_t Temp_Units;			//0x16
	int16_t T1_Status;
	int16_t T2_Status;			//0x1a
	int16_t t1only_open_en;
	int16_t wt_t1_only_en;
	int16_t Red_t1_only_en;		//0x20
	int16_t T1_open_en;
	int16_t T2_open_en; 		//0x24
	int16_t Red_T1en;
	int16_t wt_T1en;
	int16_t Red_T2en;
	int16_t wt_T2en;		//0x2c
	int16_t level_fault_en;
	int16_t weight_fault_en;
	int16_t Battery_en;		//0x32
	int16_t Mains_en;
	int16_t RTC_Blink;
	int16_t Warning;
	int16_t time_am_pm;
	int16_t dmyformat_en;
	int16_t mdyformat_en;
	int16_t Hour;
	int16_t Vaccum_sts;
	int16_t FACT_RST_CNTDOWN;
	int16_t WIFI_runtime_test1;
	int16_t WIFI_runtime_test2;

} SensorReadingStruct;

typedef struct{
	int16_t EMPTY_EN;
	int16_t REFILLSOON;
	int16_t FILL;
} LEVELSettingsStruct;

typedef struct{
	int16_t ERRORS_ONLY;
	int16_t TEMP;
	int16_t LEVEL;
	int16_t VACUUM;
	int16_t STORAGE_FAIL;
	int16_t MAINS_BATT;
	int16_t NETWORK_ISSUES;
	int16_t BLINK_CTRL;
	int16_t NO_ERROR;
	int16_t ERRORS;
	int16_t WARNINGS;
	int16_t UPDATES;
	int16_t NEW_COLOR;
	int16_t OLD_CLR;
}LEDSettingsStruct;

typedef struct{
	int16_t T1HighLimit;		//0x1f6
	int16_t T1LowLimit;
	int16_t T2HighLimit;
	int16_t T2LowLimit;
	int16_t T1HighLimitSwitch;  //0x1fe
	int16_t T1LowLimitSwitch;
	int16_t T2HighLimitSwitch;
	int16_t T2LowLimitSwitch;
	int16_t T2Enable;
	int16_t TempUnitSelection;  //0x208

} TemperatureSettingsStruct;

typedef struct{
	float T1HighLimitFl;
	float T1LowLimitFl;
	float T2HighLimitFl;
	float T2LowLimitFl;
	float GBPlimitF1;
}TemperatureConvStruct;

typedef struct{
	int16_t PowerStatus;  //0x30
	int16_t BatteryLevel;
	int16_t LoginStatus;//34
	int16_t WiFiStatus;//36
	int16_t Brightness;
	int16_t BrightnessPercentage;
	int16_t ScreenSaverTime;
	int16_t BatterySavingMode;//not used 3E
	int16_t DarkModeControl;// not used
	int16_t FreezerID;   //0x42 //up to 0x4E not used

} SystemInfoStruct;



typedef struct{
	int16_t RemoteAlarmDelaySwitch;
	int16_t RemoteAlarmDelay;
	int16_t AudiableRepeatDelaySwitch; //
	int16_t AudiableRepeatDelay;
	int16_t LN2UsageDelaySwitch;
	int16_t LN2UsageAlarm_value;
	int16_t TestAlarmControl;
	int16_t Battery_sw_over;
} AlarmSettingsStruct;

typedef struct{
	int16_t LidSwitchSense; //0xB0
	int16_t LidOpenTime;
} LidFunctionsStruct;

typedef struct{
	int16_t Hours12Settings;
	int16_t HoursSettings;
	int16_t MinutesSettings;
	int16_t AMPMSettings;
	int16_t DaySettings;


} RemainderSettingsStruct;

typedef struct{
	int16_t MonthSettings;
	int16_t DateSettings;
	int16_t YearSettings;
	int16_t Hours12Settings;
	int16_t Hours24Settings;
	int16_t MinutesSettings;
	int16_t AMPMSettings;
} SCHRemainderSettingsStruct;

typedef struct{
	int16_t MonthSettings;
	int16_t DateSettings;
	int16_t YearSettings;
	int16_t Hours12Settings;
	int16_t HoursSettings;
	int16_t MinutesSettings;
	int16_t AMPMSettings;
//	int16_t Minutes12Settings;

} MaintenanceRemainderSettingsStruct;

typedef struct{
	int16_t MonthSettings;
	int16_t DateSettings;
	int16_t YearSettings;
	int16_t Hours12Settings;
	int16_t HoursSettings;
	int16_t MinutesSettings;
	int16_t AMPMSettings;
}FillRemainderStruct;


typedef struct{
	int16_t MinutesSettings;
	int16_t Hours12Settings;
	int16_t HoursSettings;
	int16_t MonthSettings;
	int16_t DateSettings;
	int16_t YearSettings;
	int16_t AMPMSettings;
	//int16_t Minutes12Settings;
} UpdateRemainderSettingsStruct;

typedef struct{
	int16_t UI_VP_Buff[64];//
} UIVPStruct;;

typedef struct{

	int16_t DateSettings;
	int16_t MonthSettings;
	int16_t YearSettings;
	int16_t DateFormat;
	int16_t AMPMSettings;
	int16_t MinutesSettings;
	int16_t Hours12Settings;
	int16_t TimeFormat;
	int16_t HoursSettings;

} DateTimeSettingsStruct;

typedef struct{
//	int16_t DP1PointValue;
	int16_t DP2PointValue1;
	int16_t DP2PointValue2;
//	int16_t T11PointValue;
//	int16_t T12PointValue1;
//	int16_t T12PointValue2;
//	int16_t T21PointValue;
//	int16_t T22PointValue1;
//	int16_t T22PointValue2;
} CalibDataStruct;

typedef struct{
	int8_t FreezerID[9];
} FreezerStruct;

typedef struct{
	uint8_t AddUserName[9];
	uint8_t EmployeID [9];
	int16_t select;
}
AddUserStruct;

typedef struct{
	int16_t delete_select[11];
	int16_t list_select;
}
User_access;

typedef struct{
//	int16_t en_dis;
	int8_t wifi_user[32];
	int8_t wifi_user_pswd[32];
	int8_t wifi_user_pswd_star[32];
	uint8_t connect_sts;
	uint8_t IOT_connect_sts;
	uint8_t Wifi_password_en;
}
Wifi;


typedef struct{
	int8_t Aws_url[128];
	int8_t Aws_email[64];
	int8_t Aws_phone[20];
	int8_t Aws_username[64];
}AWS;

typedef struct{
	int8_t login_user[9];
	int8_t login_pw[9];
	int8_t new_pw[9];
	int8_t conform_pw[9];
}
login_credentials;


typedef struct{
	int16_t Brightness;
	int16_t TouchSound;
	int16_t ScreenSaverTime;
	int16_t LED_Brightness;
	int16_t StatusLEDEn;
//	int16_t BatterSaver;
//	int16_t Darkmode;
//	int16_t Language;
} DisplayDataStruct;

typedef struct{
//	int16_t DeviceIDCryolink;
//	int16_t BaudRateCryolink;
//	int16_t FPByteOrderCryolink;
	int16_t DeviceIDPhasetwo;
	int16_t BaudRatePhasetwo;
	int16_t DataRead_En;
//	int16_t FillMethodPhasetwo;
	int16_t EN_4_20_mA;
//	int8_t NwID[8];
} ModbusDataStruct;

typedef struct{
	int16_t ScheduledFillRemainderEn;
	int16_t FillRemainderEn;
	int16_t MaintRemainderEn;
}ScheduledEventStruct;

typedef struct{
	int16_t Daily;
	int16_t Sunday;
	int16_t Monday;
	int16_t Tuesday;
	int16_t Wednesday;
	int16_t Thursday;
	int16_t Friday;
	int16_t Saturday;
}Daily_sel;


typedef struct{
	int16_t Group_en;
	int16_t Group_sel;
	int8_t Group_A[90];
	int8_t Group_B[90];
	int8_t Group_C[90];
	int8_t Group_D[90];
	int16_t Group_col[32];
} UpdateGroupSettingsStruct;

typedef struct{
	int16_t SCAN_WIFI_EN1;
	int16_t SCAN_WIFI_EN2;
	int16_t SCAN_WIFI_EN3;
	int16_t SCAN_WIFI_EN4;
	int16_t SCAN_WIFI_EN5;
	int16_t SCAN_WIFI_EN6;
	int16_t SCAN_WIFI_EN7;
	int16_t SCAN_WIFI_EN8;
	int16_t SCAN_WIFI_EN9;
	int16_t SCAN_WIFI_EN10;
} UpdateWifiEn;

typedef struct{
	int16_t FILE_TOUCH_EN1;
	int16_t FILE_TOUCH_EN2;
	int16_t FILE_TOUCH_EN3;
	int16_t FILE_TOUCH_EN4;
	int16_t FILE_TOUCH_EN5;
	int16_t FILE_TOUCH_EN6;
	int16_t FILE_TOUCH_EN7;
	int16_t FILE_TOUCH_EN8;
	int16_t FILE_TOUCH_EN9;
	int16_t FILE_TOUCH_EN10;
	int16_t FILE_TOUCH_EN11;
	int16_t FILE_TOUCH_EN12;
} UPDATEFILEEn;

typedef struct{
	int16_t FILE_TYPE_EN1;
	int16_t FILE_TYPE_EN2;
	int16_t FILE_TYPE_EN3;
	int16_t FILE_TYPE_EN4;
	int16_t FILE_TYPE_EN5;
	int16_t FILE_TYPE_EN6;
	int16_t FILE_TYPE_EN7;
	int16_t FILE_TYPE_EN8;
	int16_t FILE_TYPE_EN9;
	int16_t FILE_TYPE_EN10;
	int16_t FILE_TYPE_EN11;
	int16_t FILE_TYPE_EN12;
} UPDATEFILETYPE;

typedef struct{
	int16_t Edit_en;
	int8_t Supply_con[20];
	int8_t Service_con[20];
} UpdateSystemSettingsStruct;

typedef struct{
	int16_t file_sel1;
	int16_t file_sel2;
	int16_t file_sel3;
	int16_t file_sel4;
	int16_t file_sel5;
	int16_t file_sel6;
	int16_t file_sel7;
	int16_t file_sel8;
	int16_t file_sel9;
	int16_t file_sel10;
	int16_t file_sel11;
	int16_t file_sel12;
}FILE_SELECTION;

typedef struct{
	int16_t file_size1;
	int16_t file_size2;
	int16_t file_size3;
	int16_t file_size4;
	int16_t file_size5;
	int16_t file_size6;
	int16_t file_size7;
	int16_t file_size8;
	int16_t file_size9;
	int16_t file_size10;
	int16_t file_size11;
	int16_t file_size12;
}FILE_SIZE_SELECTION;

int16_t SwapBytes(int16_t Data);
int32_t Swap32Bytes(int32_t Data);
uint8_t DefaultTxHandler( uint8_t *DataPtr);
uint8_t DispHandshakeCmd( uint8_t *DataPtr);
uint8_t DispSetSysConfigCmd( uint8_t *DataPtr);
uint8_t DispSetScreenSaverCmd( uint8_t *DataPtr);
uint8_t DispSetTouchConfigCmd( uint8_t *DataPtr);
uint8_t DispSetInitPageIdCmd( uint8_t *DataPtr);
uint8_t DispLoadPageCmd( uint8_t *DataPtr);
uint8_t DispSetRTCCmd( uint8_t *DataPtr);

uint8_t SendParamSettings( uint8_t *DataPtr);
uint8_t SendCalibData( uint8_t *DataPtr);
uint8_t SendRunTimeData( uint8_t *DataPtr);
uint8_t SendCTRSData( uint8_t *DataPtr);
uint8_t Sendendiswifi(uint8_t *DataPtr);
uint8_t SendCTRSconfigEventsSettings(uint8_t *DataPtr);
uint8_t SendEventData( uint8_t *DataPtr);
uint8_t SendUserActionEventData( uint8_t *DataPtr);
uint8_t SendLevelParamSettings(uint8_t *DataPtr);
uint8_t SendTemperatureParamSettings(uint8_t *DataPtr);
uint8_t SendAlarmParamSettings(uint8_t *DataPtr);
uint8_t SendLidFunctionParameters(uint8_t *DataPtr);
uint8_t SendDisplaySettings(uint8_t *DataPtr);
uint8_t SendModbusSettings(uint8_t *DataPtr);
uint8_t SendScheduledFillSettings(uint8_t *DataPtr);
uint8_t SendFillRemainderSettings(uint8_t *DataPtr);
uint8_t SendMaintenanceRemainderSettings(uint8_t *DataPtr);
uint8_t SendDateTimeSettings(uint8_t *DataPtr);
uint8_t SendScheduledEventsSettings(uint8_t *DataPtr);
uint8_t SendScheduledEventsSettings1(uint8_t *DataPtr);
uint8_t SenddayssSettings(uint8_t *DataPtr);
uint8_t Send420mAEventsSettings(uint8_t *DataPtr);
uint8_t Send420mAconfigEventsSettings(uint8_t *DataPtr);
uint8_t SendsfirmwareTimeSettings(uint8_t *DataPtr);
uint8_t SendsGroupColourSettings(uint8_t *DataPtr);
uint8_t SendNoResponseSettings(uint8_t *DataPtr);

uint8_t SendSystemInfoSettings(uint8_t *DataPtr);
uint8_t SendADCData(uint8_t *DataPtr);
uint8_t Sendendiswifi(uint8_t *DataPtr);
uint8_t Sendendisfile(uint8_t *DataPtr);
uint8_t Sendendisfiletype(uint8_t *DataPtr);
uint8_t Sendfileselection(uint8_t *DataPtr);
uint8_t Sendfilesize(uint8_t *DataPtr);
void Icon_status(void);

uint8_t SendLedParamSettings(uint8_t *DataPtr);
#endif /* DISPLAYTXHANDLERS_H_ */
