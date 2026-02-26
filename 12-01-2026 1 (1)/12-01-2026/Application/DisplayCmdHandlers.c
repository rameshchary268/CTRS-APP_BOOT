/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: DisplayCmdHandlers.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains functions to handle the transmit/Receive data to LCD module
 *
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

#include "MyMain.h"
#include "TopwayCmdList.h"
#include "DisplayCmdHandlers.h"
#include "DispUIkeyHandler.h"
#include "DisplayRxHandlers.h"
#include "DisplayTxHandlers.h"
#include "DisplayCommDriver.h"
#include "DisplayRxHandlers.h"


/* Declaration of external global variables */
uint8_t ParamSettingsReqSent = 0;
uint8_t EventDataReqSent = 0;
uint8_t HandshakeCmdSent = 0;

uint8_t SendN16Data(uint8_t*);
uint8_t SendN32Data(uint8_t*);
uint8_t SendRxValueData(uint8_t*);

const DispCmdTxHandlerStruct DispCmdTxHandlers[] = {
		{ TOPWAY_HANDSHAKE_CMD,				DispHandshakeCmd, 			DISP_RESP_EXPECTED,		&HandshakeCmdSent	},
		{ TOPWAY_SET_SYSCONFIG_CMD,			DispSetSysConfigCmd, 		DISP_NO_RESP_EXPECTED,	NULL			 	},
		{ TOPWAY_SCREEN_SAVER_CMD,			DispSetScreenSaverCmd, 		DISP_NO_RESP_EXPECTED,	NULL			 	},
		{ TOPWAY_LOAD_PAGE_CMD,			    DispLoadPageCmd, 			DISP_NO_RESP_EXPECTED,	NULL			 	},
		{ TOPWAY_RTC_SET_CMD,				DispSetRTCCmd, 				DISP_NO_RESP_EXPECTED,	NULL			 	}
//		{ TOPWAY_DISP_PAGE_CMD,				DispSetInitPageIdCmd, 		DISP_NO_RESP_EXPECTED,	NULL			 	},
};

const DispVPDataRespHandlerStruct	DispVPDataRespHandlers[] = {
		{ KEY_PRESS_EVENT_DATA_ADDRESS, 	 	DefaultRxHandler,			NULL						},
		{ USER_ACTION_DATA_ADDRESS,				UserActionEventRxHandler,	&UserActionEventDataReqSent	},
		{ SYSTEM_PARAM_SETTINGS_ADDRESS,		SytemParamSettingsRxHandler,&ParamSettingsReqSent		},
		{ SYSTEM_LOGIN_CREDENTIALS_ADDRESS,		UserCredentialsRxHandler,	NULL						},
		{ SYSTEM_CALIB_DATA_ADDRESS,			SystemCalibrationRxHandler,	&CalibDataReqSent			},
		{ SYSTEM_EVENT_ADDRESS	,				DefaultRxHandler,			&EventDataReqSent			},
		{ SYSTEM_RUNTIME_DATA_ADDRESS,			DefaultRxHandler,			&RunTimeDataReqSent			},
		{ SYSTEM_BIST_DATA_ADDRESS,				SystemBISTDataRxHandler,	NULL						},
		{ SYSTEM_BIST1_DATA_ADDRESS,			DefaultRxHandler,			NULL						},
		{ SYSTEM_INIT_DATA_ADDRESS,				DefaultRxHandler,			NULL						},
		{ SYSTEM_LOGIN_DATA_ADDRESS,			DefaultRxHandler,			NULL						},
		{ 0x081200,								DefaultRxHandler,			NULL						}
};

const DispVPDataHandlerStruct DispVPDataHandlers[] = {
		{ SENSOR_DATA_WRITE_ADDRESS,						sizeof(SensorReadingStruct),					SendRunTimeData,							NULL	},
		{ SYSTEM_SETTINGS_DATA_ADDRESS,						sizeof(SystemInfoStruct),						SendSystemInfoSettings,						NULL	},
		{ T1_HIGH_LIMIT_VALUE_WR,					        sizeof(TemperatureSettingsStruct),				SendTemperatureParamSettings,				NULL	},
		{ T1_HIGH_LIMIT_VALUE_RD,							sizeof(TemperatureSettingsStruct),				SendTemperatureParamSettings,				NULL	},
		{ LEVEL_EMPTY_WR,					                sizeof(LEVELSettingsStruct),					SendLevelParamSettings,			            NULL	},		//Need to comeback
		{ LEVEL_EMPTY_RD	,					            sizeof(LEVELSettingsStruct),					SendLevelParamSettings,			            NULL	},		//Need to comeback
		{ REMOTE_ALARM_DELAY_ENABLE_WR,						sizeof(AlarmSettingsStruct),					SendAlarmParamSettings,						NULL	},
		{ REMOTE_ALARM_DELAY_ENABLE_RD,						sizeof(AlarmSettingsStruct),					SendAlarmParamSettings,						NULL	},
		{ SCHEDULED_FILL_HR12_WR,			                sizeof(RemainderSettingsStruct),				SendScheduledFillSettings,					NULL	},
		{ MAINT_MONTH_VALUE_WR,								sizeof(MaintenanceRemainderSettingsStruct),		SendMaintenanceRemainderSettings,			NULL	},
		{ MAINT_MONTH_VALUE_RD,								sizeof(MaintenanceRemainderSettingsStruct),		SendMaintenanceRemainderSettings,			NULL	},
		{ FILLREM_MONTH_WR, 								sizeof(FillRemainderStruct), 					SendFillRemainderSettings, 					NULL	},		//Need to comeback
//		{ FILLREM_MONTH_RD, 									sizeof(FillRemainderStruct), 					SendFillRemainderSettings, 					NULL	},		//Need to comeback
		{ RTC_DATE_VALUE_WR,								sizeof(DateTimeSettingsStruct),					SendDateTimeSettings,						NULL	},
		{ RTC_DATE_VALUE_RD				,					sizeof(DateTimeSettingsStruct),					SendDateTimeSettings,						NULL	},
		{ DISPLAY_BRIGHTNESS_WR	,				            sizeof(DisplayDataStruct),						SendDisplaySettings,						NULL	},
		{ DISPLAY_BRIGHTNESS_RD				,				sizeof(DisplayDataStruct),						SendDisplaySettings,						NULL	},
		{ CRYO_SETTINGS_DATA_WR_ADDRESS	,					sizeof(ModbusDataStruct),						SendModbusSettings,							NULL	},
		{ MODBUS_PONEDEVICEID_WR,					        sizeof(ModbusDataStruct),						SendModbusSettings,							NULL	},
		{ MODBUS_PONEDEVICEID_RD,					        sizeof(ModbusDataStruct),						SendModbusSettings,						NULL	},

		{ SCHEDULED_FILL_WR	,			                    sizeof(ScheduledEventStruct),					SendScheduledEventsSettings,				NULL	},
		{ SCHEDULED_FILL_RD	,			                    sizeof(ScheduledEventStruct),					SendScheduledEventsSettings,				NULL	},
		{ SCHEDULED_FILL_DAILY_WR,                         sizeof(Daily_sel) ,                             SenddayssSettings,                          NULL    },
		{ SCHEDULED_FILL_DAILY_RD,                         sizeof(Daily_sel) ,                             SenddayssSettings,                          NULL    },
		{ UPDATE_MIN_VALUE_WR,                            sizeof(UIUpdateRemaindersettings) ,             SendsfirmwareTimeSettings,                  NULL    },
		{ UPDATE_MIN_VALUE_RD,                            sizeof(UIUpdateRemaindersettings) ,             SendsfirmwareTimeSettings,                  NULL    },
		{ WIFI_SCAN_EN_WR,                                  sizeof(UpdateWifiEn) ,                          SendRunTimeData,                              NULL    },
		{ ERRORS_ONLY_WR, 									sizeof(LEDSettingsStruct), 						SendLedParamSettings, 					NULL},
		{ ERRORS_ONLY_RD, 									sizeof(LEDSettingsStruct), 						SendLedParamSettings, 					NULL},

		{ GROUP_COL_WR,                                     sizeof(UpdateGroupsettings.Group_col) ,         SendRunTimeData,                   NULL    },
		{ EN_DIS_RED,                                       sizeof(Group_inactive_nodes) ,                  SendRunTimeData,                     NULL    },
		{ TOUCH_EN1,                                        sizeof(UPDATEFILEEn) ,                          Sendendisfile,                              NULL    },
		{ ICON_SELECTION1,                                   sizeof(UPDATEFILETYPE) ,                       Sendendisfiletype,                              NULL    },
		{ FILE_TICK_SEL1_WR,                                 sizeof(FILE_SELECTION) ,                       Sendfileselection,                              NULL    },
		{ FILE_TICK_SEL1_RD,                                  sizeof(FILE_SELECTION) ,                       Sendfileselection,                              NULL    },
		{ FILE_SIZE_VP1,                                      sizeof(FILE_SIZE_SELECTION) ,                       Sendfilesize,                              NULL    },


};
//Init sequence commands, Add any additional required commands here.
const DispInitHandlerStruct DisplayInitCmdHandlers[] = {
		{ TOPWAY_HANDSHAKE_CMD,				DispHandshakeCmd, 			DISP_RESP_EXPECTED 		},
		{ TOPWAY_SET_SYSCONFIG_CMD,			DispSetSysConfigCmd, 		DISP_NO_RESP_EXPECTED 	},
		{ TOPWAY_SCREEN_SAVER_CMD,			DispSetScreenSaverCmd, 		DISP_NO_RESP_EXPECTED 	}
//		{ TOPWAY_TOUCH_DOWN_KEYID_EVENT,	DispSetTouchConfigCmd, 		DISP_NO_RESP_EXPECTED 	}
//		{ TOPWAY_DISP_PAGE_CMD,				DispSetInitPageIdCmd, 		DISP_NO_RESP_EXPECTED 	},
};

//Runtime commands table, Add other required commands in the array
const DispDataWriteHandlerStruct DisplayRunTimeDataHandlers[] = {
		{ SENSOR_DATA_WRITE_ADDRESS,						sizeof(SensorReadingStruct),		SendRunTimeData,			DISP_DATA_WRITE, NULL		},
		{ CTRS_DATA_CMD_WR		,						    sizeof(CTRSStruct),				    SendCTRSData,			DISP_DATA_WRITE, NULL		    },
};

const uint8_t NoOfVPRespHandlers = sizeof(DispVPDataRespHandlers)/sizeof(DispVPDataRespHandlers[0]);
const uint8_t NoOfInitCmds = sizeof(DisplayInitCmdHandlers)/sizeof(DisplayInitCmdHandlers[0]);
const uint8_t NoOfRunTimeCmds = sizeof(DisplayRunTimeDataHandlers)/sizeof(DisplayRunTimeDataHandlers[0]);
const uint8_t NoOfVPDataCmds = sizeof(DispVPDataHandlers)/sizeof(DispVPDataHandlers[0]);
const uint8_t NoOfGenericCmds = sizeof(DispCmdTxHandlers)/sizeof(DispCmdTxHandlers[0]);

static uint8_t DispAckCount = 0;
static uint8_t DispCmdCount = 0;
static uint8_t NoOfMissedResponse = 0;
static uint8_t LCD_Tx_Buffer[512]; //Array to frame Tx commands

uint8_t SendN16Data(uint8_t *DataPtr)
{
	uint8_t Ctr = 0;
	DataPtr[Ctr++] = 0xF8;
	DataPtr[Ctr++] = 0x2F;
//	DataPtr[Ctr++] = 0x86;
//	DataPtr[Ctr++] = 0xA0;

	return Ctr;

}

extern int16_t RxValue;
uint8_t SendRxValueData(uint8_t *DataPtr)
{
	uint8_t Ctr = 0;
	DataPtr[Ctr++] = RxValue >> 8;
	DataPtr[Ctr++] = RxValue&0xFF;
//	DataPtr[Ctr++] = 0x86;
//	DataPtr[Ctr++] = 0xA0;

	return Ctr;

}

uint8_t SendN32Data(uint8_t *DataPtr)
{
	uint8_t Ctr = 0;
	DataPtr[Ctr++] = 0x00;
	DataPtr[Ctr++] = 0x01;
	DataPtr[Ctr++] = 0x86;
	DataPtr[Ctr++] = 0xA0;

	return 2;

}


/*
 * *****************************************************************************
 * Function name: InitSeqResponseAck
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function will be called upon receiving response from LCD for Inti commands or
 *  after defined delay, in case no response is expected
 * Parameters:
 *  Input  : Response command code
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void InitSeqResponseAck(uint32_t Respcode)
{
	//Check whether response is received if expected
	if(DisplayInitCmdHandlers[DispAckCount].RespFlag == DISP_RESP_EXPECTED)
	{
		if(Respcode == DisplayInitCmdHandlers[DispAckCount].CmdCode)
		{
			DispAckCount++; //Proceed with next command
            System_Status[ERROR_FLAG_0] &= ~ERR_DISPLAY_INTT;
		}
		else
		{
			DispCmdCount = DispAckCount; //Resend the command
            System_Status[ERROR_FLAG_0] |= ERR_DISPLAY_INTT;
		}
	}
	else
	{
		DispAckCount++;
	}
}

/*
 * *****************************************************************************
 * Function name: SendDispInitSequence
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function will be called during LCD Init, will send the commands listed in the
 *  Init table
 * Parameters:
 *  Input  : Response command code
 *  Return : Init status
 * Note:
 *
 * *****************************************************************************
 */
DispStatusEnum SendDispInitSequence(void)
{
	uint16_t DataCtr = 0,CRC_VLU;

	if(DispAckCount >= NoOfInitCmds)
	{
		DispAckCount = 0;
		DispCmdCount = 0;
		return DISP_INIT_COMPLETED; // Init seq completed
	}
	else if(DispCmdCount)
	{
		if(DispCmdCount != DispAckCount)
			return DISP_INIT_INPROGRESS;
	}

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = DisplayInitCmdHandlers[DispCmdCount].CmdCode;

	DataCtr += (*DisplayInitCmdHandlers[DispCmdCount].TxHandler)(&LCD_Tx_Buffer[DataCtr]);

	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;

	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
	DispCmdCount++;

	return DISP_INIT_INPROGRESS;
}

/*
 * *****************************************************************************
 * Function name: SendCmdToDisplay
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to send generic commands to LCD
 * Parameters:
 *  Input  : command index
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SendCmdToDisplay(DispCmdCodes CmdIndex)
{
	uint16_t DataCtr = 0,CRC_VLU;

	if(CmdIndex >= NoOfGenericCmds)
	{
		//Invalid
		return;
	}

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = DispCmdTxHandlers[CmdIndex].CmdCode; //Write command code

	//Call function to fill the command data
	if(DispCmdTxHandlers[CmdIndex].TxHandler)
	{
		uint8_t Ctr = (*DispCmdTxHandlers[CmdIndex].TxHandler)(&LCD_Tx_Buffer[DataCtr]);
		DataCtr += Ctr;
	}
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;

	if(DispCmdTxHandlers[CmdIndex].RespAckFlag)
	{
		if(*DispCmdTxHandlers[CmdIndex].RespAckFlag)
		{
			//Log error if the flag is not cleared
	        NoOfMissedResponse++;
		}
		*DispCmdTxHandlers[CmdIndex].RespAckFlag = 1;
	}
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}


/*
 * *****************************************************************************
 * Function name: SendVPDataCmdToDisplay
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to send VP Data to LCD
 * Parameters:
 *  Input  : VP address index
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SendVPDataCmdToDisplay(DispVPDataCmds VPAddrIndex)
{
	uint16_t DataCtr = 0,CRC_VLU;

	if(VPAddrIndex >= NoOfVPDataCmds)
	{
		//Invalid
		return;
	}

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = TOPWAY_SUCCESSIVE_WRITE_CMD; //Write command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr) & 0xFF;
	//Fill Data size as defined size, will be updated with actual size value
	LCD_Tx_Buffer[DataCtr++] = DispVPDataHandlers[VPAddrIndex].Size>>1;
	//Call function to fill the command data
	if(DispVPDataHandlers[VPAddrIndex].TxHandler)
	{
		uint8_t Ctr = (*DispVPDataHandlers[VPAddrIndex].TxHandler)(&LCD_Tx_Buffer[DataCtr]);
		LCD_Tx_Buffer[DataCtr-1] = Ctr>>1; // re assign the length field with actual
		DataCtr += Ctr;
	}
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}

/*
 * *****************************************************************************
 * Function name: SendN16DataToDisplay
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to send N16 Data to LCD
 * Parameters:
 *  Input  : VP address and Data
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SendN16DataToDisplay(uint32_t VPAddr, uint16_t Data)
{
	uint16_t DataCtr = 0,CRC_VLU;

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = TOPWAY_SUCCESSIVE_WRITE_CMD; //Write command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr) & 0xFF;
	//Fill Data size as defined size, will be updated with actual size value
	LCD_Tx_Buffer[DataCtr++] = 1;
	LCD_Tx_Buffer[DataCtr++] = (Data >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (Data) & 0xFF;
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}

void SendN32DataToDisplay(uint32_t VPAddr, uint32_t Data)
{
	uint16_t DataCtr = 0,CRC_VLU;

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = TOPWAY_SUCCESSIVE_WRITE_CMD; //Write command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr) & 0xFF;
	//Fill Data size as defined size, will be updated with actual size value
	LCD_Tx_Buffer[DataCtr++] = 1;
	LCD_Tx_Buffer[DataCtr++] = (Data >> 24)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (Data >> 16)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (Data >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (Data) & 0xFF;
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}

void SendRegDataToDisplay(uint32_t VPAddr, uint16_t Data)
{
	uint16_t DataCtr = 0,CRC_VLU;

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = 0x3B; //TOPWAY_SUCCESSIVE_WRITE_CMD; //Write command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (Data) & 0xFF;
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}

/*
 * *****************************************************************************
 * Function name: SendSTRDataToDisplay
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to send string Data to LCD
 * Parameters:
 *  Input  : VP address and string ptr
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SendSTRDataToDisplay(uint32_t VPAddr, uint8_t *StrData)
{
	uint16_t DataCtr = 0,CRC_VLU;
	uint8_t LenCtr = 0;

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = TOPWAY_STR_WRITE_CMD; //String Write command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr) & 0xFF;
	//Fill Data
	for(LenCtr = 0; LenCtr<128; LenCtr++,StrData++)
	{
		LCD_Tx_Buffer[DataCtr++] = *StrData;
		if(*StrData == '\0')
		{
			break;
		}
	}
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}

/*
 * *****************************************************************************
 * Function name: ReadVPDataFromDisplay
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to read VP data from LCD
 * Parameters:
 *  Input  : VP address index
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
/*void ReadVPDataFromDisplay(DispVPDataCmds VPAddrIndex)
{
	uint16_t DataCtr = 0;

	if(VPAddrIndex >= NoOfVPDataCmds)
	{
		//Invalid
		return;
	}

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	LCD_Tx_Buffer[DataCtr++] = TOPWAY_SUCCESSIVE_READ_CMD; //Read command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DispVPDataHandlers[VPAddrIndex].VPParamAddr) & 0xFF;
	//Fill Data size as defined size, will be updated with actual size value
	LCD_Tx_Buffer[DataCtr++] = DispVPDataHandlers[VPAddrIndex].Size>>1;
	//Call function to fill the command data
	if(DispVPDataHandlers[VPAddrIndex].RespAckFlag)
	{
		if(*DispVPDataHandlers[VPAddrIndex].RespAckFlag)
		{
			//Log error if the flag is not cleared
	        NoOfMissedResponse++;
		}
		(*DispVPDataHandlers[VPAddrIndex].RespAckFlag) = 1;
	}
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[DataCtr++] = FOOTER3;
	LCD_Tx_Buffer[DataCtr++] = FOOTER4;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}*/

/*
 * *****************************************************************************
 * Function name: SendRunTimeDataToDisplay
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to send the runtime commands to LCD
 * Parameters:
 *  Input  : None
 *  Return : Disp execution status
 * Note:
 *
 * *****************************************************************************
 */
DispStatusEnum SendRunTimeDataToDisplay(void)
{
	uint16_t DataCtr = 0,CRC_VLU;

	if(DispAckCount >= NoOfRunTimeCmds)
	{
		DispAckCount = 0;
		DispCmdCount = 0;
		return DISP_TX_SEQ_COMPLETED;
	}
	else if(DispCmdCount) //Wait for response or defined delay
	{
		if(DispCmdCount != DispAckCount)
			return DISP_TX_INPROGRESS;
	}

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	if(DisplayRunTimeDataHandlers[DispCmdCount].DataDir == DISP_DATA_WRITE)
	{
		LCD_Tx_Buffer[DataCtr++] = TOPWAY_SUCCESSIVE_WRITE_CMD; //Write command code
	}
	else
	{
		LCD_Tx_Buffer[DataCtr++] = TOPWAY_SUCCESSIVE_READ_CMD;//Read command code
		if(DisplayRunTimeDataHandlers[DispCmdCount].RespAckFlag)
		{
			//Increment the flag for every associated command, Upon receiving this
			//flag will be decremented, If value is > 1 means we missed the response.
			if(*DisplayRunTimeDataHandlers[DispCmdCount].RespAckFlag)
			{
				//Log error if the flag is not cleared
		        NoOfMissedResponse++;
			}
			(*DisplayRunTimeDataHandlers[DispCmdCount].RespAckFlag) = 1;
		}
	}
	//Fill P address
	LCD_Tx_Buffer[DataCtr++] = (DisplayRunTimeDataHandlers[DispCmdCount].VPParamAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DisplayRunTimeDataHandlers[DispCmdCount].VPParamAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DisplayRunTimeDataHandlers[DispCmdCount].VPParamAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (DisplayRunTimeDataHandlers[DispCmdCount].VPParamAddr) & 0xFF;
	//Fill Data size
	if(DispCmdCount==0x2)
	{
		LCD_Tx_Buffer[DataCtr++] = DisplayRunTimeDataHandlers[DispCmdCount].Size>>2;
	}
	else
	{
		LCD_Tx_Buffer[DataCtr++] = DisplayRunTimeDataHandlers[DispCmdCount].Size>>1;
	}
	//Call function to fill the command data
	if(DisplayRunTimeDataHandlers[DispCmdCount].TxHandler)
	{
		uint8_t Ctr = (*DisplayRunTimeDataHandlers[DispCmdCount].TxHandler)(&LCD_Tx_Buffer[DataCtr]);
		if(DispCmdCount==0x1)
		{
			LCD_Tx_Buffer[DataCtr-1] = Ctr>>2; // re assign the length field with actual
		}
		else
		{
			LCD_Tx_Buffer[DataCtr-1] = Ctr>>1; // re assign the length field with actual
		}
		DataCtr += Ctr;
	}
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
	DispCmdCount++;
	return DISP_TX_INPROGRESS;
}

/*
 * *****************************************************************************
 * Function name: ParseVPRxData
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle VP data read response message
 * Parameters:
 *  Input  : RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void ParseVPRxData(uint8_t Cmdcode, uint8_t *DataPtr, uint16_t Size)
{
	uint32_t VPAddr = ((uint32_t)DataPtr[4]) << 24;
	VPAddr |= ((uint32_t)DataPtr[5]) << 16;
	VPAddr |= ((uint32_t)DataPtr[6]) << 8;
	VPAddr |= ((uint32_t)DataPtr[7]);

	//Find the VP linked to this address
	for(uint8_t Ctr = 0; Ctr < NoOfVPRespHandlers; Ctr++)
	{
		if(VPAddr == DispVPDataRespHandlers[Ctr].VPAddr)
		{
			if(DispVPDataRespHandlers[Ctr].RxHandler)
			{
				(*DispVPDataRespHandlers[Ctr].RxHandler)(VPAddr, &DataPtr[9], (uint16_t)DataPtr[8]<<1);
			}
			if(DispVPDataRespHandlers[Ctr].RespAckFlag)  //Ack, the requested cmd
			{
				if(*DispVPDataRespHandlers[Ctr].RespAckFlag)
				{
					(*DispVPDataRespHandlers[Ctr].RespAckFlag)--;
				}
			}
		}
	}
}

/*
 * *****************************************************************************
 * Function name: DispDataReadResponseAck
  * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function will be called upon receiving the response from LCD
 * Parameters:
 *  Input  : VP addr code
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void DispDataReadResponseAck(uint32_t VPAddr)
{
	DispAckCount++;
}

void ReadSTRDatafromDisplay(uint32_t VPAddr)
{
	uint16_t DataCtr = 0,CRC_VLU;

	LCD_Tx_Buffer[DataCtr++] = HEADER;
	DataCtr=3;
	LCD_Tx_Buffer[DataCtr++] = TOPWAY_STR_READ_CMD; //String read command code

	//Fill VP address
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 24) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 16) & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr >> 8)  & 0xFF;
	LCD_Tx_Buffer[DataCtr++] = (VPAddr) & 0xFF;
	//close with delimiter
	LCD_Tx_Buffer[DataCtr++] = FOOTER1;
	LCD_Tx_Buffer[DataCtr++] = FOOTER2;
	LCD_Tx_Buffer[1]=(uint8_t)(((DataCtr-1) & 0xff00) >> 8);
	LCD_Tx_Buffer[2]=(uint8_t)((DataCtr-1) & 0x00ff);
	CRC_VLU = crc16_calc(&LCD_Tx_Buffer[3],DataCtr-3);
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;
	LCD_Tx_Buffer[DataCtr++] = (uint8_t)((CRC_VLU & 0xff00) >> 8);//;FOOTER3;
	// Write to LCD Tx circular buffer
	WriteDataToTxBuff(LCD_Tx_Buffer, DataCtr);
}




