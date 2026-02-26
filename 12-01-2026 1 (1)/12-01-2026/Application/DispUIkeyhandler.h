/*
 * DispUIkeyhandler.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef INC_UIKEYHANDLER_H_
#define INC_UIKEYHANDLER_H_

#include "DispVPAddrList.h"
#include "DisplayTxHandlers.h"

typedef void (*UIDataHandler)(uint32_t, int16_t *DataInput, uint8_t*, uint16_t);
typedef void (*TouchKeyHandler)(uint8_t CmdCode, uint16_t, uint8_t);
#define Remote_local  0x089914			//0x08905E
#define No_devices    0x0800E8
typedef enum {
	HOME_SCREEN_PAGE,
	MENU_SCREEN_PAGE,
	TEMP_SETTINGS_PAGE,
	LEVEL_SELECT_PAGE,
	SYSTEM_SCREEN_PAGE,
	WEIGHT_SETTINGS_PAGE,
	VACCUM_SELECT_PAGE,
	SECURITY_SETTINGS_PAGE,
	DISPLAY_SCREEN_PAGE,
	SET_DATE_TIME_PAGE,
	SYSTEM_INFO_PAGE,
	ALARMS_SETTINGS_PAGE,
	SCHEDULE_EVENTS_PAGE,
	SCHEDULE_MFILL_PAGE,
	CALIB_SENSOR_SELECT_PAGE,
	RTD_CALIB_1P_2P_VALUE_PAGE,
	CALIB_SETTINGS_PAGE,
	WEIGHT_CALIB_PAGE,			//UNUSED
	VACCUM_CALIB_PAGE,			//UNUSED
	WEIGHT_LOADCELL_SETTINGS_PAGE,	//UNUSED
	USER_ACCESS_PAGE,
	USER_ACCESS_PAGE1,
	SCHEDULED_FILL_REMINDER_PAGE,
	MAINTENENCE_REMINDER_PAGE,
	ADD_USER_PAGE,
	LOGIN1_PAGE,
	LOGIN2_PAGE,
	LOGIN3_PAGE,
	LOGOUT_PAGE,
	LED_CTRL_PAGE,
	BLUETOOTH_PAGE,
	MODBUS_PHASETWO_PAGE,//Firmware update confirmation
	MODBUS_CRYOLINK_PAGE,
	ONNECTED_DEVICES_PAGE,
	WIFI_PAGE,
	WIFI_SCAN_PAGE,
	WIFI_SCAN_PAGE2,
	IOT_PAGE,
	SYSTEM_SETTINGS_PAGE,
	FIRMWARE_UPDATE_PAGE,
	USB_PAGE,
	FIRM_DECOMPRESS_PAGE,
	UPDATE_PAGE,
	UPDATE_TIME_PAGE,
	FIRM_UPDATING_PAGE,
	FILE_EXPLORER1_PAGE,
	FILE_EXPLORER2_PAGE,
	FILE_EXPLORER3_PAGE,//Firmware updating...
	CLR_SELECT_PAGE,
	FACTORY_PAGE,
	SCREEN_SAVER_PAGE,
	SHUTDOWN_PAGE,
	LOADING_PAGE,
	Graph,
	ERROR_NOTIFICATION,
	ERROR_POPUP_GREEN_PAGE,
	ERROR_POPUP_RED_PAGE,
	ERROR_POPUP_RED_NOESC_PAGE,
	NETWORK_PAGE,
	BLE_CONN_MANAGE_PAGE,
	WIFI_CONN_MANAGE_PAGE,
	T1_ONLY_TEMPERATURE,



	ERROR1_PAGE,
	MAX_PAGE_IDS
} PageIDList;

typedef enum {
	MENU_HOME_KEY,
	ENTER_KEY,
	CANCEL_KEY,
	BACK_KEY,
	TEMP_SETTINGS_KEY,
	MUTE_KEY,
	LEVEL_SETTINGS_KEY,
	DELETION_KEY,
	VACCUM_KEY,
	SETTINGS_KEY,
	SECURIY_KEY,
	SYSTEM_SETTINGS_KEY,
	DATE_TIME_KEY,
	MODBUS_CRYOLINK_KEY,
	SCHEDULED_LN2SUPPLY_KEY ,
	CALI_1P_KEY,
	CALI_2P_KEY,
	CALI_RTD1_KEY,
	NEXT,
	LOGIN,
	REGISTER,
	LID_SETTINGS_KEY,
	LOGOUT_YES,
	SCHEDULED_CLEVEL_KEY,
	SCHEDULED_MAINT_KEY,
	LEFT_NAV_KEY,
	RIGHT_NAV_KEY,
	MODBUS_PHASE2_KEY,
	SCAN9,
	SCAN10,
	REMOTE,
	LOCAL,
	UNUSED11,
	UNUSED10,
	UNUSED9,


	ACK_KEY,
	UNUSED2,
	UNUSED3,
	UNUSED7,
	UNUSED4,
	UNUSED8,
	UNUSED5,
	UNUSED6,
	UN_USED1,
	WIFI_NAVIGATION_KEY,
	MAX_KEY_IDS
} KeyIDList;

typedef struct {
	uint32_t VPAddr;
	DispDataTypeEnum DataType;
	uint8_t	DataSize;
	int16_t LowLimit;
	int16_t HighLimit;
	UIDataHandler UIDataHandlerFunc;
	int16_t *DataInPtr;
} UIDataHandlerStruct;



typedef struct {
	uint32_t User_add;
	uint32_t Status_add;
	uint32_t id_delete;
	uint32_t touch_id;
	uint32_t User;
	uint32_t admin;
}Register_address;

typedef struct {
	float DP_Offset;
	int16_t LLA_MIN;
	int16_t	LLA_MAX;
	int16_t HLA_MIN;
	int16_t HLA_MAX;
	int16_t Stopfill_MIN;
	int16_t Stopfill_MAX;
	int16_t Startfill_MIN;
	int16_t Startfill_MAX;
	int16_t LLA_MIN_MM;
	int16_t	LLA_MAX_MM;
	int16_t HLA_MIN_MM;
	int16_t HLA_MAX_MM;
	int16_t Stopfill_MIN_MM;
	int16_t Stopfill_MAX_MM;
	int16_t Startfill_MIN_MM;
	int16_t Startfill_MAX_MM;
}Version_value;

typedef struct {
	uint32_t User_add;
	uint32_t User_add_crc;
	uint32_t emp_add;
	uint32_t emp_add_crc;
	uint32_t sts_add;
	uint32_t pw_add;
	uint32_t pw_crc_add;
}Eeprom_address;

typedef struct {
	float   DP_Offset;
	int16_t LLA_MIN;
	int16_t	LLA_MAX;
	int16_t HLA_MIN;
	int16_t HLA_MAX;
	int16_t Stopfill_MIN;
	int16_t Stopfill_MAX;
	int16_t Startfill_MIN;
	int16_t Startfill_MAX;
	int16_t LLA_MIN_MM;
	int16_t	LLA_MAX_MM;
	int16_t HLA_MIN_MM;
	int16_t HLA_MAX_MM;
	int16_t Stopfill_MIN_MM;
	int16_t Stopfill_MAX_MM;
	int16_t Startfill_MIN_MM;
	int16_t Startfill_MAX_MM;
} UIVERSION_LIMITS;



typedef struct {
	PageIDList PageId;
	TouchKeyHandler TouchKeyHandlerFunc;
} TouchKeyHandlerStruct;

typedef struct {
	uint32_t check_error;
}Errors_restart;

void HandleUIDataEntryEvent(uint8_t CmdCode, uint8_t *DataPtr, uint16_t Size);
void HandleTouchKeyPressEvent(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleScreensaverFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleHomeScreenKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleTempSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleSystemSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandlePopupScreenKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleMenuScreenKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleDP1POr2PointCalibSelectionFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleRTDCalib1POr2PSelectFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleCalibSensorSelectFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleGraphFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleAlarmSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleLidFunctionSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleDisplaySettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleModbusCryolinkSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleModbusPhaseTwoSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleWIFISettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleBLEConnectionKeyHandler(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleLevelSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleScheduledPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);

void HandleMaintenencePageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleRTCPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);

void HandleSystemInfoPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleScheduledFillPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
//void HandleScheduledLN2SupplyPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleScheduledFillRemPageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleUserAcessFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleFIRMUpdateFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleUserLoginFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
//void Logoutfunction(void);



void ProcessUserStopCommand(void);
void resumefunctions(void);
void cancelfunctions(void);
void LoadErrorScreen(void);



void Update_UIandSTM_Variables_Fn(uint32_t VPAd,int16_t PValue);
/* Function Declarations */
void DefaultKeyHandler(uint32_t, int16_t *DataInput, uint8_t *DataPtr, uint16_t Size);
void DefaultTouchKeyHandler(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleDP2PCalibKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleDPCalibKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleAddUserKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleSecurityKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleSyatemsettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleError1PageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleError3PageKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void SendCalibAckToUI(uint16_t Ack);
void HandleNetworkKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleBISTFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void wififirmwareupdatepage(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleFileExplorerFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);

void Folder_Expanding(uint8_t index);
void Folder_back();
void remove_extension_global(const char *filename);

void HandleLEDControlFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleSelectCOLORFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
#endif /* INC_UIKEYHANDLER_H_ */
