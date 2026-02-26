/*
 * UISettings.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef UISETTINGS_H_
#define UISETTINGS_H_

#define MAX_PATH_LEN 256

void SaveDispSettings(DisplayDataStruct *DispSettings);
void LoadDispSettings(DisplayDataStruct *DispSettings);
void SaveAlarmSettings(AlarmSettingsStruct *AlarmSettings);
void LoadAlarmSettings(AlarmSettingsStruct *AlarmSettings);
void SaveLidSettings(LidFunctionsStruct *LidSettings);
void LoadLidSettings(LidFunctionsStruct *LidSettings);

void SaveCryoSettings(ModbusDataStruct *CryoSettings);
void LoadCryoSettings(ModbusDataStruct *CryoSettings);
void SavePhaseSettings(ModbusDataStruct *PhaseSettings);
void LoadPhaseSettings(ModbusDataStruct *PhaseSettings);

void SaveTempSettings(TemperatureSettingsStruct *TemperatureSettings);
void LoadTempSettings(TemperatureSettingsStruct *TemperatureSettings);
void SaveLevelSettings(LEVELSettingsStruct *UILevelSettingsData);
void LoadLevelSettings(LEVELSettingsStruct *UILevelSettingsData);
void LoadScheduledSettings(ScheduledEventStruct *ScheduledSettings);
void SaveScheduledSettings(ScheduledEventStruct *ScheduledSettings);

void LoadMaintenenceSettings(MaintenanceRemainderSettingsStruct *MaintenanceRemainderSettings);
void SaveMaintenenceSettings(MaintenanceRemainderSettingsStruct *MaintenanceRemainderSettings);
void LoadRTCSettings(DateTimeSettingsStruct *DateTimeSettings);
void SaveRTCSettings(DateTimeSettingsStruct *DateTimeSettings);

void LoadScheduledFillSettings(RemainderSettingsStruct *ScheduledFillSettings);
void SaveScheduledFillSettings(RemainderSettingsStruct *ScheduledFillSettings);
void LoadScheduledFillRemSettings(SCHRemainderSettingsStruct *ScheduledRLevelSettings);
void SaveScheduledFillRemSettings(SCHRemainderSettingsStruct *ScheduledRLevelSettings);
void LoadScheduledLN2SupplySettings(RemainderSettingsStruct *ScheduledLN2SupplySettings);
void SaveScheduledLN2SupplySettings(RemainderSettingsStruct *ScheduledLN2SupplySettings);
void SaveFreezerId(FreezerStruct *UIFreezerStruct);
void LoadFreezerId(FreezerStruct *UIFreezerStruct);
void add_folder(const char folderName[]);
void go_back();
void mutebuzzer(void);
void SaveUpdateTimeSettings(UpdateRemainderSettingsStruct *UIUpdateRemaindersettings);
void Remove_save_time(void);
void SaveGROUPSettings(UpdateGroupSettingsStruct *UpdateGroupSetting);
void LoadGROUPSettings(UpdateGroupSettingsStruct *UpdateGroupSetting);
void SaveSystemSettings(UpdateSystemSettingsStruct *UpdateSystemSetting);
void LoadSystemSettings(UpdateSystemSettingsStruct *UpdateSystemSetting);

void Groupselection(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Groupen_dis(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Group_selection_string(int16_t value);
void GroupAStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void GroupBStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void GroupCStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void GroupDStringHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Error_string(uint8_t value);

void daily(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void HandleFirmwareupdateFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleUSBSHIFTFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void Update_system(void);
void Update_now_system(void);
void File_validation(char *Valid_name);
uint8_t File_crc_validation(char *Valid_name, FX_MEDIA   media);
void HandleFirmwareupdatetimeFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleGroupFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);

void Supply_con_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Service_con_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Edit_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Home_page(void);
void IOT_Connectivity(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void DP_control(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
uint8_t Checking_ssid(uint8_t Scan_no);
//void HandleRunCalibrationFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);

void SaveLedSettings(LEDSettingsStruct *UILEDSettingsData);
void LoadLedSettings(LEDSettingsStruct *UILEDSettingsData);
#endif /* UISETTINGS_H_ */
