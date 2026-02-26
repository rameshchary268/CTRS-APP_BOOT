/*
 * DisplayCmdHandlers.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef DISPLAYCMDHANDLERS_H_
#define DISPLAYCMDHANDLERS_H_

#include "DispVPAddrList.h"
#include "stdint.h"

typedef enum {
	DISP_HANDSHAKE_CMD,
	DISP_SYS_CONFIG_CMD,
	DISP_SCREEN_SAVER_CMD,
	LOAD_PAGE_CMD,
	SET_RTC_CMD,
	MAX_DISPLAY_CMD_CODE
} DispCmdCodes;

typedef enum {
	DISP_SENSOR_READING_CMD,
	DISP_SYSTEM_SETTINGS_CMD,
	DISP_TEMP_SETTINGS_CMD,
	DISP_TEMP_SETTINGS_CMD1,
	DISP_LEVEL_SETTINGS_CMD,
	DISP_LEVEL_SETTINGS_CMD1,
	DISP_ALARM_SETTINGS_CMD,
	DISP_ALARM_SETTINGS_CMD1,
	//DISP_LID_SETTINGS_CMD,
	//DISP_LID_SETTINGS_CMD1,
	DISP_SCHEDULED_FILL_CMD,
	DISP_MAINT_REMAINDER_CMD,
	DISP_MAINT_REMAINDER_CMD1,
	DISP_Fill_REMAINDER_CMD,
//	DISP_Fill_REMAINDER_CMD1,
	DISP_DATE_TIME_SETTINGS_CMD,
	DISP_DATE_TIME_SETTINGS_CMD1,
	DISP_DISPLAY_SETTINGS_CMD,
	DISP_DISPLAY_SETTINGS_CMD1,
	DISP_CRYO_SETTINGS_CMD,
	DISP_PHASE_SETTINGS_CMD,
	DISP_PHASE_SETTINGS_CMD1,
	DISP_SCEDULEDEVENTS_CMD,
	DISP_SCEDULEDEVENTS_CMD1,
	Day_shed,
	Day_shed1,
	firmware_time_clean,
	firmware_time_clean1,
	Wifi_scan_en,
	DISP_LED_SETTINGS_CMD,
	DISP_LED_SETTINGS_CMD1,
	GROUP_COLOUR_CMD,
	GROUP_INACTIVE_NODES,
	FILE_TOUCH_EN,
	FILE_TYPE_EN1,
	FILE_SELECT_RD,
	FILE_SELECT_WR,
	FILE_SIZE,
	DISP_4_20MA_CMD,
	DISP_4_20MA_CMD1,
	DISP_4_20MA_CONFIG_CMD,
	DISP_4_20MA_CONFIG_CMD1,
	CTRS_TEMP_SETTINGS,
	CTRS_TEMP_SETTINGS1,
	MAX_VP_DATA_CMD
} DispVPDataCmds;


//Function pointers for Command handler
typedef uint8_t (*DispCmdTxHandler)( uint8_t* );
typedef void (*DispCmdRxHandler)(uint8_t, uint8_t*, uint16_t);

//Function Pointers for VP data Handler
typedef void (*DispVPDataRxHandler)(uint32_t, uint8_t*, uint16_t);

//Function pointers for Command handler
typedef uint8_t (*DispTxHandler)( uint8_t* );

typedef struct {
	uint8_t CmdCode; // Command Code
	DispTxHandler TxHandler; //Handler to be called to frame the write request
	uint8_t RespConfig;  //Flag to indicate the response expectation from Display
	uint8_t *RespAckFlag;  //Flag to indicate the response status
} DispCmdTxHandlerStruct;

typedef struct{
	uint32_t VPAddr;
	DispVPDataRxHandler RxHandler; //Handler to be called upon recving the packet
	uint8_t *RespAckFlag;  //Flag to indicate the response status
} DispVPDataRespHandlerStruct;

typedef struct {
	uint32_t VPParamAddr; // VP Address
	uint8_t Size;
	DispTxHandler TxHandler; //Handler to be called to frame the write request
	uint8_t *RespAckFlag;  //Flag to indicate the response status
} DispVPDataHandlerStruct;

typedef struct {
	uint8_t CmdCode; // Command Code
	DispTxHandler TxHandler; //Handler to be called to frame the write request
	uint8_t RespFlag;  //Flag to indicate the response expectation from Display
} DispInitHandlerStruct;

typedef struct {
	uint32_t VPParamAddr; // VP Address
	uint8_t Size;
	DispTxHandler TxHandler; //Handler to be called to frame the write request
	DispDataDirEnum DataDir;
	uint8_t *RespAckFlag;  //Flag to indicate the response status
} DispDataWriteHandlerStruct;

void ParseVPRxData(uint8_t Cmdcode, uint8_t *DataPtr, uint16_t Size);
DispStatusEnum SendDispInitSequence(void);
void SendVPDataCmdToDisplay(DispVPDataCmds VPAddrIndex);
void ReadVPDataFromDisplay(DispVPDataCmds VPAddrIndex);
DispStatusEnum SendRunTimeDataToDisplay(void);
void InitSeqResponseAck(uint32_t Respcode);
void DispDataReadResponseAck(uint32_t VPAddr);
void SendCmdToDisplay(DispCmdCodes CmdIndex);
void SendN16DataToDisplay(uint32_t VPAddr, uint16_t Data);
void SendN32DataToDisplay(uint32_t VPAddr, uint32_t Data);
void SendSTRDataToDisplay(uint32_t VPAddr, uint8_t *StrData);
void SendRegDataToDisplay(uint32_t VPAddr, uint16_t Data);
void ReadSTRDatafromDisplay(uint32_t VPAddr);
#endif /* DISPLAYCMDHANDLERS_H_ */
