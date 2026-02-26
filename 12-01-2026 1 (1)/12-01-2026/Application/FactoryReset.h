/*
 * FactoryReset.h
 *
 *  Created on: Dec 7, 2023
 *      Author: venkatesh.arikanti
 */

#ifndef FACTORYRESET_H_
#define FACTORYRESET_H_


#define DEFAULT_KEY_ENABLE			1
#define DEFAULT_KEY_DISABLE			0
#define DEFAULT_HOURS				11
#define DEFAULT_MIN					30
#define DEFAULT_AMPM				0
#define DEFAULT_DAY					1
#define DEFAULT_MONTH				1
#define SD_BUFFER_SETTINGS          0x800
#define Data_Group_buffer           0x200

#define DEFAULT_YEARS				23

#define DEFAULT_DAYE				1
#define DEFAULT_ONE					1
#define HCHE20                      0x01
#define HCHE44                      0x02
#define HCHE92                      0x04

typedef struct  {
  volatile uint8_t    Board_version;
  uint8_t    plumbing_options;
  uint8_t    Battery_status;
  uint8_t    Level_sen;
  uint8_t    Temp_sen;
  uint8_t    serial[24];
  uint8_t    IOT_serial[40];
  uint8_t 	 Iot_token[40];
} PLUMBING_OPTIONS;

void Settings_Init();
void reset_fn();
void factory_default(void);
void reset_factory_default(void);
void reset_scheduler_default(void);
void reset_scheduled_fill_default(void);
void reset_scheduled__REM_fill_default(void);
void reset_maintenence_default(void);
void reset_ModbusData_default(void);
void reset_4_20_mA_default(void);
void reset_4_20_mA_config_default(void);
void reset_CTRS_config_default(void);
void reset_AlarmSettings_default(void);
void reset_DisplaySettings_default(void);
void reset_SupplyRemainder_default(void);
void reset_TempeSettingsData_default(void);
void reset_LevelSettingsData_default(void);
void resetLedSettingsData_default(void);
void reset_DateTimeSettings_default(void);
void reset_FrezzID_default(void);
void reset_Group_default(void);
void UI_Load_Init();
void Factoryreset_data_UIupdate();
void Settings_write_data(void);


#endif /* FACTORYRESET_H_ */
