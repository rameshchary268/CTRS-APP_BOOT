/*
 * ErrorLog.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef ERRORMON_H
#define	ERRORMON_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Includes */

/* Declaration of constant defines and typedefs */

#define ERR_BITS_GROUP1            32

#define ERR_DISPLAY_INTT				0x00000001
#define ERR_EEPROM_FAILURE				0x00000002
#define ERR_RTD1_SHORT			        0x00000004
#define ERR_RTD2_SHORT			        0x00000008
#define ERR_RTD1_SENSOR_FAILURE			0x00000010
#define ERR_RTD2_SENSOR_FAILURE			0x00000020
#define ERR_T1_HIGH						0x00000040
#define ERR_T2_HIGH						0x00000080
#define ERR_T1_LOW						0x00000100
#define ERR_T2_LOW						0x00000200
#define ERR_EMPTY_SENSOR_FAIL	        0x00000400
#define ERR_REFILL_SENSOR_FAIL	        0x00000800
#define ERR_FILL_SENSOR_FAIL		    0x00001000
#define ERR_MAIN_PWR_FAILED		        0x00002000
#define ERR_LID_OPEN_TOO_LONG			0x00004000
#define ERR_SD_CARD_FAILURE				0x00008000
#define ERR_WIFI_MODULE_FAILURE			0x00010000
#define ERR_UI_COM_FAILURE              0x00020000
#define ERR_BATTERY_BACKUP              0x00040000
#define ERR_DISP_TX_OVERFLOW            0x00080000


//error1


//Warning
#define WRN_LN2_USAGE_HIGH				0x00000001
#define WRN_BATT_NOT_CONNECTED			0x00000002
#define WRN_BATT_VOLTAGE_LOW			0x00000004
#define WRN_RTD1_CALIB_FAILURE			0x00000008
#define WRN_RTD2_CALIB_FAILURE			0x00000010
#define WRN_FILL_REMINDER				0x00000020	//Fill Remainder
#define WRN_LN2_FILL_REMINDER			0x00000040	//schedule manual fill
#define WRN_MAINTENANCE_REMINDER		0x00000080	//maintainance remainder
#define WRN_EMMC_CARD_ACCESS_ERROR		0x00000100
#define WRN_EMAIL_VALID                 0x00000200
#define WRN_EMAIL_FAIL                  0x00000400

//Events
#define EVT_LID_OPENED					0x00000001
#define EVT_MAIN_PWR_FAILED				0x00000002
#define EVT_MLN2_FILL_STARTED		    0x00000004
#define EVT_RTD2_CALIB_FAILURE		    0x00000008
#define EVT_CALIB_PROCESS_STARTED		0x00000010
#define EVT_RTD1_CALIB_FAILURE			0x00000020		//need to ask
#define EVT_RTD1_CALIB_SUCCESS			0x00000040
#define EVT_RTD2_CALIB_SUCCESS			0x00000080
#define EVT_DISPLAY_CLOSE               0X00000100
#define EVT_FIRMWARE_UPDATE             0x00000200
#define EVT_IOT_DIS_CONNECT_MANUAL      0x00000400
#define EVT_WIFI_CONNECT_MANUAL         0x00000800
#define EVT_IOT_CONNECT_MANUAL          0x00001000
#define EVT_WIFI_CONNECTION             0x00002000
#define EVT_IOT_CONNECTION              0x00004000
#define EVT_WIFI_POOR_RSSI              0x00008000
#define EVT_WIFI_DIS_CONNECT_MANUAL     0x00010000
#define EVT_DISPLAY_VERIFY              0x00020000
#define EVT_DECOMPRE_FAIL               0x00040000


#define NO_NOTIFICATION					0x00
#define UI_NOTIFICATION					0x01
#define EMMC_NOTIFICATION				0x02
#define UI_SD_NOTIFICATION				0x03
#define ALARM_NOTIFICATION				0x04
#define UI_ALARM_NOTIFICATION			0x05
#define ALARM_SD_NOTIFICATION			0x06
#define ALL_NOTIFICATION				0x07
#define ALARM_ONCE_NOTIFICATION			(0x08 | ALARM_NOTIFICATION)
#define UI_ALARM_ONCE_NOTIFICATION		(0x08 | UI_ALARM_NOTIFICATION)
#define ALARM_ONCE_SD_NOTIFICATION		(0x08 | ALARM_SD_NOTIFICATION)
#define ALL_ALARM_ONCE_NOTIFICATION		(0x08 | ALL_NOTIFICATION)

typedef struct {
	uint32_t EventMask;
	uint8_t EventNotifyFlag;
	char *EvSetDescp;
	char *EvClearDescp;
} EventLogTableStruct;

#define MAX_UI_ERROR_LOGS		32

typedef enum {
	ERROR_FLAG_0,
	ERROR_FLAG_1,
	WARNING_FLAG,
	EVENT_FLAG,
	MAX_ERROR_FLAGS
} ErrFlagEnum;

typedef struct{
	uint8_t Error_buff[MAX_ERROR_FLAGS][512];
} LOG_DATA_BUF;


enum {
	ERROR_RESET = 0,
	ERROR_SET = 1
};

typedef struct {
	ErrFlagEnum ErrFlag;
	uint8_t ErrIndex;
	uint8_t PrevIndex;
	uint8_t NextIndex;
} ErrLogStruct;

/* Function Declarations */
void Err_msg_init(void);
void Wrn_msg_init(void);
uint8_t GetErrorMessage(uint8_t ErrorLogIndex, uint8_t *Msgptr);
uint8_t GetListItemsInOrder(uint8_t IndexReset);
uint8_t GetPrevItemIndex(uint8_t CurrentIndex);
uint8_t GetNextItemIndex(uint8_t CurrentIndex);
uint8_t GetTotalItemIndex();
uint8_t GetTotalUIItemIndex();
void UpdateUIErrorLog(void);
void InitializeErrLogData(void);
void AddErrLogData(ErrFlagEnum ErrFlagId, uint8_t ErrIndex);
void RemoveErrLogData(ErrFlagEnum ErrFlagId, uint8_t ErrIndex);

#endif	/* ERRORMON_H */

