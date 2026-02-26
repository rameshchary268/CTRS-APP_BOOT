/* 
 * /////////////////////////////////////////////////////////////////////////////
 * Module: Error.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 * 
 * Description:
 * This module contains definitions of error monitoring functions.
 *  
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DisplayTxHandlers.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
//#include "GraphData.h"

/* Declaration of external global variables */

uint8_t Reminder_Id;
uint32_t System_Status[MAX_ERROR_FLAGS] = {0};

ErrLogStruct ErrLogData[MAX_UI_ERROR_LOGS];
uint8_t HeadErrIndex = 0; //Will point to the latest error
uint8_t TotalErrLogs = 0;
uint8_t TotalUILogs=0;
uint8_t UpdateErrorScreen = 0;
uint8_t MFV_ALARM_DIS=0;
uint8_t GBP_ALARM_DIS=0;
LOG_DATA_BUF Log_data_buff;
uint8_t New_error_ind=0;

/* Declaration of global variables */

const EventLogTableStruct EventLogTable[MAX_ERROR_FLAGS][32] = {

{//Error
	{ERR_DISPLAY_INTT,		  ALARM_ONCE_SD_NOTIFICATION,	"DI1, Display Init Failed!",		NULL	},
	{ERR_EEPROM_FAILURE,	  ALL_ALARM_ONCE_NOTIFICATION,"EE1, EEPROM RW Error!",			NULL	},
	{ERR_RTD1_SHORT,          ALL_NOTIFICATION,            "RS11, T1 Short!",                 NULL    },
	{ERR_RTD2_SHORT,          ALL_NOTIFICATION,            "RS21, T2 Short!",                 NULL    },
	{ERR_RTD1_SENSOR_FAILURE, ALL_NOTIFICATION,			"T11, T1 Open!",					NULL	},
	{ERR_RTD2_SENSOR_FAILURE, ALL_NOTIFICATION,			"T21, T2 Open!",					NULL	},
	{ERR_T1_HIGH,			  ALL_NOTIFICATION,			"T13, T1 High!",					"T15, T1 High Alarm cleared!"	},
	{ERR_T2_HIGH,			  ALL_NOTIFICATION,			"T23, T2 High!",					"T25, T2 High Alarm Cleared!"	},
	{ERR_T1_LOW,			  ALL_NOTIFICATION,			"T12, T1 Low!",						"T14, T1 Low Alarm Cleared!"	},
	{ERR_T2_LOW,			  ALL_NOTIFICATION,			"T22, T2 Low!",						"T24, T2 Low Alarm Cleared!"	},
	{ERR_EMPTY_SENSOR_FAIL,	          ALL_NOTIFICATION,			"LV1, Empty Sensor faulty!",					NULL	},
	{ERR_REFILL_SENSOR_FAIL,		      ALL_NOTIFICATION,			"LV2, ReFill Sensor faulty!",					NULL	},
	{ERR_FILL_SENSOR_FAIL,		      ALL_NOTIFICATION,			"LV3, Fill Sensor faulty!",				NULL	},
	{ERR_MAIN_PWR_FAILED,	  EMMC_NOTIFICATION,			"MP1, Mains Failed!",				"MP2, Mains Restored!"	},
	{ERR_LID_OPEN_TOO_LONG,	  ALL_NOTIFICATION,			"LI2, LID Open Too Long!",			NULL },
	{ERR_SD_CARD_FAILURE,	  UI_ALARM_ONCE_NOTIFICATION,	"FSDF, SD Card Failure!",			NULL	},
	{ERR_WIFI_MODULE_FAILURE, ALL_ALARM_ONCE_NOTIFICATION,"WF1, Wi-Fi Module Failure!",		NULL	},
	{ERR_UI_COM_FAILURE,      EMMC_NOTIFICATION,             "UI, Ui communication failure",    NULL    },
	{ERR_BATTERY_BACKUP,      ALL_NOTIFICATION,            "B11, Battery backup mode!",        "B12, Battery backup ack!"    },//revert back
	{ERR_DISP_TX_OVERFLOW,    EMMC_NOTIFICATION,            "DI2, Buffer Over Flow!",           NULL    },//revert back
},


{//Error 2 flag

},


{//Wrning flag
	{WRN_LN2_USAGE_HIGH,	ALL_ALARM_ONCE_NOTIFICATION,"LV7, High LN2 Consumption!",		NULL	},//remove
	{WRN_BATT_NOT_CONNECTED,ALL_ALARM_ONCE_NOTIFICATION,"MP4, Battery Not Connected!",		NULL	},
	{WRN_BATT_VOLTAGE_LOW,	ALL_ALARM_ONCE_NOTIFICATION,"MP3, Battery Voltage Low!",		NULL	},
	{WRN_RTD1_CALIB_FAILURE,NO_NOTIFICATION,			"WRCF, RTD1 Cal Failed!",			NULL	},
	{WRN_RTD2_CALIB_FAILURE,NO_NOTIFICATION,			"WR2F, RTD2 Cal Failed!",			NULL	},
	{WRN_FILL_REMINDER,ALL_ALARM_ONCE_NOTIFICATION,"WLSR, Fill Reminder!",		"ELSR, Fill Reminder Cleared!"	},
	{WRN_LN2_FILL_REMINDER, ALL_ALARM_ONCE_NOTIFICATION,               "WLFR, Schedule Fill!",				"ELFR, Schedule Fill Cleared!"	},
	{WRN_MAINTENANCE_REMINDER,ALL_ALARM_ONCE_NOTIFICATION,"WMTR, Maintenance Reminder!",	"EMRC, Maintenance Reminder Cleared!"	},
	{WRN_EMMC_CARD_ACCESS_ERROR,UI_ALARM_ONCE_NOTIFICATION,"FSAE, SD Card Access Error!",      NULL    },
	{WRN_EMAIL_VALID,         UI_NOTIFICATION,          ", Enter valid email id!",           NULL    },
	{WRN_EMAIL_FAIL,          UI_NOTIFICATION,          ", Failed to send email!",          NULL    },
},
{//Event 0
	{EVT_LID_OPENED,		EMMC_NOTIFICATION,			"LI1, Lid Open!",					"LI3, Lid Close!"	},
	{EVT_MAIN_PWR_FAILED,	EMMC_NOTIFICATION,			"MP1, Mains Failed!",				"MP2, Mains Restored!"	},
	{EVT_MLN2_FILL_STARTED, EMMC_NOTIFICATION,            "MLF9, Manual start Fill",          "MLFS, Manual Stop Fill!"       },
	{EVT_CALIB_PROCESS_STARTED,EMMC_NOTIFICATION,			"ECPS, Cal Process Started!",		"ECPC, Cal Process Completed!"	},
	{EVT_RTD1_CALIB_SUCCESS,EMMC_NOTIFICATION,			"T16, T1 Cal Success!",				NULL	},
	{EVT_RTD2_CALIB_SUCCESS,EMMC_NOTIFICATION,			"T26, T2 Cal Success!",				NULL	},
	{EVT_DISPLAY_CLOSE,     EMMC_NOTIFICATION,            ", Display closing!",                 NULL                    },
	{EVT_FIRMWARE_UPDATE,   EMMC_NOTIFICATION,            ", Firmware update",                  NULL},
	{EVT_IOT_DIS_CONNECT_MANUAL,    EMMC_NOTIFICATION,    "WF7, Disconnect IoT Pressed Manually!",  NULL},
	{EVT_WIFI_CONNECT_MANUAL, EMMC_NOTIFICATION,          "WF6, Connect Wi-Fi Pressed Manually!" , NULL},
	{EVT_IOT_CONNECT_MANUAL,  EMMC_NOTIFICATION,          "WF8, Connect IoT Pressed Manually!",NULL},
	{EVT_WIFI_CONNECTION,   EMMC_NOTIFICATION,            "WF1, Wi-Fi Connected!",             "WF2, Wi-Fi Disconnected!"},
	{EVT_IOT_CONNECTION,    EMMC_NOTIFICATION,            "WF3, IoT Server Connected!",       "WF4, IoT Server Disconnected!"},
	{EVT_WIFI_POOR_RSSI,    EMMC_NOTIFICATION,            "WF9, Poor Signal Strength!",       NULL},
	{EVT_WIFI_DIS_CONNECT_MANUAL,    EMMC_NOTIFICATION,   "WF5, Disconnect Wi-Fi Pressed Manually!", NULL},
	{EVT_DISPLAY_VERIFY,    UI_NOTIFICATION,            "DVM1, Display ver miss match!",            NULL},
	{EVT_DECOMPRE_FAIL,    NO_NOTIFICATION,             "DE3, Decompression data fail!",            NULL},
}
};//EUS, EUL, MLF,
//FSA, WMT, WLFR,WLLC, WLSR, DO NOT USE
const uint32_t *ErrorFlags[MAX_ERROR_FLAGS] = {
		&System_Status[0],
		&System_Status[1],
		&System_Status[2],
		&System_Status[3]
};

/* Declaration of local functions */

/* 
 * **************************************
 * **************************************
 * Function Declarations start here
 * **************************************
 * **************************************
 */

/* 
 * *****************************************************************************
 * Function name:   Err_msg_init()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the routine that initializes error logging info
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void Err_msg_init(void)
{
  InitializeErrLogData();
}

/* 
 * *****************************************************************************
 * Function name:   Wrn_msg_init()
 * Created by:Venkatesh Arikanti
 * Date:       14-10-2025
 * Description:
 *  This is the routine that initializes warning logging info at "warning_flag"
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void LogMsgToSDCard(ErrFlagEnum ErrFlagId, uint8_t ErrIndex, uint8_t ErrState)
{
	if((System_Status[0] & ERR_SD_CARD_FAILURE))
	{
		return;
	}
	//ToDo : USB connected log should be logged from USB interface

	if(ErrState == ERROR_RESET)
	{
		if(EventLogTable[ErrFlagId][ErrIndex].EvClearDescp)
		{
			ULOG_INFO(EventLogTable[ErrFlagId][ErrIndex].EvClearDescp);
			//AddRealTimeEvent(EventLogTable[ErrFlagId][ErrIndex].EvClearDescp);
		}
	}
	else if(EventLogTable[ErrFlagId][ErrIndex].EvSetDescp)
	{
		if(ErrFlagId == EVENT_FLAG)
		{
			ULOG_INFO(EventLogTable[ErrFlagId][ErrIndex].EvSetDescp);
		}
		else if(ErrFlagId == WARNING_FLAG)
		{
			ULOG_WARNING(EventLogTable[ErrFlagId][ErrIndex].EvSetDescp);
		}
		else
		{
			ULOG_ERROR(EventLogTable[ErrFlagId][ErrIndex].EvSetDescp);
		}
		//AddRealTimeEvent(EventLogTable[ErrFlagId][ErrIndex].EvSetDescp);
	}
}

void UpdateUIErrorLog(void)
{
	static uint32_t ErrorFlagStates[MAX_ERROR_FLAGS] = {0x00, 0x00, 0x00, 0x00};// = {error_flag[0],error_flag[1],error_flag[2]};

	for(uint8_t Ctr = 0; Ctr<MAX_ERROR_FLAGS; Ctr++)
	{
		uint32_t ErrorStatus = (ErrorFlagStates[Ctr]^ *ErrorFlags[Ctr]);
		if(ErrorStatus)
		{
			uint32_t BitPos = 0x01;
			for(uint8_t Pos = 0; Pos < 32; Pos++)
			{
				if(ErrorStatus & BitPos)
				{
					char Error_msg[10]={0};
					if(*ErrorFlags[Ctr] & BitPos)
					{
						//Error Set
						if(EventLogTable[Ctr][Pos].EventNotifyFlag & UI_NOTIFICATION)
						{
							//Notify in UI
							AddErrLogData(Ctr, Pos);
						}
						if(EventLogTable[Ctr][Pos].EventNotifyFlag & ALARM_NOTIFICATION)
						{
							if(EventLogTable[Ctr][Pos].EventNotifyFlag & ALARM_ONCE_NOTIFICATION)
							{
								//Do not repeat alarm for this error/event
							}
							else
							{
								//Repeatable alarm
							}
						}
						if(EventLogTable[Ctr][Pos].EventNotifyFlag & EMMC_NOTIFICATION)
						{
							//Log message to SD card
							LogMsgToSDCard(Ctr, Pos, ERROR_SET);
							if(UIwifi.IOT_connect_sts && Mains_fail)
							{
								if(((Ctr<3) || ((Ctr==3)&&Pos<20)) && (512-(strlen((const char*)Log_data_buff.Error_buff[Ctr])) > 10))
								{
									New_error_ind=1;
									for(int i=0;(i<strlen(EventLogTable[Ctr][Pos].EvSetDescp));i++)
									{
										if(EventLogTable[Ctr][Pos].EvSetDescp[i] != ',')
										{
											Error_msg[i]=EventLogTable[Ctr][Pos].EvSetDescp[i];
										}
										else
										{
											break;
										}
									}
									if((Ctr==3 && Pos==6) || (Ctr==2 && Pos==2))
									{
										if(Log_data_buff.Error_buff[0][0] != 0)
										{
											strcat((char*)&Log_data_buff.Error_buff[0],";");
										}
										strcat((char*)&Log_data_buff.Error_buff[0],(char*)&Error_msg);
									}
									else
									{
										if(Log_data_buff.Error_buff[Ctr][0] != 0)
										{
											strcat((char*)&Log_data_buff.Error_buff[Ctr],";");
										}
										strcat((char*)&Log_data_buff.Error_buff[Ctr],(char*)&Error_msg);
									}

								}
							}
						}
					}
					else
					{
						//Error Cleared
						if(EventLogTable[Ctr][Pos].EventNotifyFlag & UI_NOTIFICATION)
						{
							//Notify in UI
							RemoveErrLogData(Ctr, Pos);
						}
						if(EventLogTable[Ctr][Pos].EventNotifyFlag & ALARM_NOTIFICATION)
						{
							//Clear Alarm Notification
						}
						if(EventLogTable[Ctr][Pos].EventNotifyFlag & EMMC_NOTIFICATION)
						{
							//Log message to SD card
							LogMsgToSDCard(Ctr, Pos, ERROR_RESET);
							if(UIwifi.IOT_connect_sts && Mains_fail)
							{
								if(((Ctr<3) || ((Ctr==3)&&Pos<20)) && ((EventLogTable[Ctr][Pos].EvClearDescp)!= NULL)  && (512-(strlen((const char*)Log_data_buff.Error_buff[Ctr])) > 10) )
								{
									New_error_ind=1;
									for(int i=0;(i<strlen(EventLogTable[Ctr][Pos].EvClearDescp)) && ((EventLogTable[Ctr][Pos].EvClearDescp)!= NULL);i++)
									{
										if(EventLogTable[Ctr][Pos].EvClearDescp[i] != ',')
										{
											Error_msg[i]=EventLogTable[Ctr][Pos].EvClearDescp[i];
										}
										else
										{
											break;
										}
									}
									if((Ctr==3 && Pos==6) || (Ctr==2 && Pos==2))
									{
										if(Log_data_buff.Error_buff[0][0] != 0)
										{
											strcat((char*)&Log_data_buff.Error_buff[0],";");
										}
										strcat((char*)&Log_data_buff.Error_buff[0],(char*)&Error_msg);
									}
									else
									{
										if(Log_data_buff.Error_buff[Ctr][0] != 0)
										{
											strcat((char*)&Log_data_buff.Error_buff[Ctr],";");
										}
										strcat((char*)&Log_data_buff.Error_buff[Ctr],(char*)&Error_msg);
									}
								}
							}
						}
					}
				}
				BitPos <<= 1;
			}
		}
		ErrorFlagStates[Ctr] = *ErrorFlags[Ctr];
	}
	// System_Status[EVENT_FLAG] &= ~EVT_DP_CALIB_SUCCESS;
	 System_Status[EVENT_FLAG] &= ~EVT_RTD1_CALIB_SUCCESS;
	 System_Status[EVENT_FLAG] &= ~EVT_RTD2_CALIB_SUCCESS;
//	 System_Status[EVENT_FLAG] &= ~EVT_DP_CALIB_FAILURE;
	 System_Status[EVENT_FLAG] &= ~EVT_RTD1_CALIB_FAILURE;
	 System_Status[EVENT_FLAG] &= ~EVT_RTD2_CALIB_FAILURE;
}


void InitializeErrLogData(void)
{
	memset(ErrLogData, 0xFF, sizeof(ErrLogData));
}

void AddErrLogData(ErrFlagEnum ErrFlagId, uint8_t ErrIndex)
{
	uint8_t Ctr = 0;
	for(Ctr = 0; Ctr<MAX_UI_ERROR_LOGS; Ctr++)
	{
		if((ErrLogData[Ctr].PrevIndex == 0xFF) && (ErrLogData[Ctr].NextIndex == 0xFF) && \
		   (ErrLogData[Ctr].ErrFlag == 0xFF) && (ErrLogData[Ctr].ErrIndex == 0xFF))
			break;
	}
	if(Ctr == MAX_UI_ERROR_LOGS)//No Space found
		return;
	//Add the log index
	ErrLogData[Ctr].ErrFlag = ErrFlagId;
	ErrLogData[Ctr].ErrIndex = ErrIndex;
	if(TotalErrLogs == 0)
	{
		ErrLogData[Ctr].PrevIndex = 0xFF;
		ErrLogData[Ctr].NextIndex = 0xFF;
	}
	else
	{
		ErrLogData[Ctr].PrevIndex = HeadErrIndex;
		ErrLogData[HeadErrIndex].NextIndex = Ctr;
	}
	HeadErrIndex = Ctr;
	TotalErrLogs++;
	if((EventLogTable[ErrFlagId][ErrIndex].EventNotifyFlag & ALARM_NOTIFICATION))
	{
		TotalUILogs++;
	}
	UpdateErrorScreen = 1;
	LoadErrorScreen();
}

void RemoveErrLogData(ErrFlagEnum ErrFlagId, uint8_t ErrIndex)
{
	uint8_t Ctr = 0;
	for(Ctr = 0; Ctr<MAX_UI_ERROR_LOGS; Ctr++)
	{
		if((ErrLogData[Ctr].ErrFlag == ErrFlagId) && (ErrLogData[Ctr].ErrIndex == ErrIndex))
			break;
	}
	if(Ctr == MAX_UI_ERROR_LOGS)//No Space found
		return;

	//Remove the log index
	if((ErrLogData[Ctr].PrevIndex == 0xFF) && (ErrLogData[Ctr].NextIndex == 0xFF)) // First & last entry
	{
		HeadErrIndex = 0;
	}
	else if(ErrLogData[Ctr].PrevIndex == 0xFF) // oldest entry
	{
		ErrLogData[ErrLogData[Ctr].NextIndex].PrevIndex = 0xFF;
	}
	else if(ErrLogData[Ctr].NextIndex == 0xFF) // latest entry
	{
		ErrLogData[ErrLogData[Ctr].PrevIndex].NextIndex = 0xFF;
		HeadErrIndex = ErrLogData[Ctr].PrevIndex;
	}
	else
	{
		ErrLogData[ErrLogData[Ctr].PrevIndex].NextIndex = ErrLogData[Ctr].NextIndex;
		ErrLogData[ErrLogData[Ctr].NextIndex].PrevIndex = ErrLogData[Ctr].PrevIndex;
	}


	ErrLogData[Ctr].ErrFlag = 0xFF;
	ErrLogData[Ctr].ErrIndex = 0xFF;
	ErrLogData[Ctr].PrevIndex = 0xFF;
	ErrLogData[Ctr].NextIndex = 0xFF;
	if(TotalErrLogs)
	{
		TotalErrLogs--;
		if((EventLogTable[ErrFlagId][ErrIndex].EventNotifyFlag & ALARM_NOTIFICATION))
		{
			TotalUILogs--;
		}
	}


	UpdateErrorScreen = 1;
	LoadErrorScreen();
}

uint8_t GetTotalItemIndex()
{
	return TotalUILogs;
}
uint8_t GetTotalUIItemIndex()
{
	return TotalErrLogs;
}

uint8_t GetNextItemIndex(uint8_t CurrentIndex)
{
	if(CurrentIndex >= MAX_UI_ERROR_LOGS)
		return 0xFF;
	return ErrLogData[CurrentIndex].NextIndex;
}

uint8_t GetPrevItemIndex(uint8_t CurrentIndex)
{
	if(CurrentIndex >= MAX_UI_ERROR_LOGS)
		return 0xFF;
	return ErrLogData[CurrentIndex].PrevIndex;
}

uint8_t GetNthItemIndex(uint8_t SeqNo)
{
	uint8_t HeadIndex = HeadErrIndex;
	if(SeqNo >= MAX_UI_ERROR_LOGS)
		return 0xFF;

	for(uint8_t Ctr = 0; (Ctr < MAX_UI_ERROR_LOGS); Ctr++)
	{
		SeqNo--;
		if(!SeqNo)
		{
			return HeadIndex;
		}

		if(HeadIndex < MAX_UI_ERROR_LOGS)
		{
			HeadIndex = ErrLogData[HeadIndex].PrevIndex;
		}
		else  //end of list
		{
			return 0xFF;
		}

	}
	return 0xFF;
}

uint8_t GetListItemsInOrder(uint8_t IndexReset)
{
	static uint8_t HeadIndex = 0;

	if(IndexReset)
	{
		HeadIndex = HeadErrIndex;
		return HeadErrIndex;
	}

	if(HeadIndex < MAX_UI_ERROR_LOGS)
	{
		HeadIndex = ErrLogData[HeadIndex].PrevIndex;
	}
	else  //end of list
	{
		return 0xFF;
	}

	return HeadIndex;
}

uint8_t GetErrorMessage(uint8_t ErrorLogIndex, uint8_t *Msgptr)
{
	if(ErrorLogIndex >= MAX_UI_ERROR_LOGS)
		return 0; //Don't copy and return 0 length

	if((Msgptr == NULL) || (ErrLogData[ErrorLogIndex].ErrFlag >= MAX_ERROR_FLAGS) || (ErrLogData[ErrorLogIndex].ErrIndex >= ERR_BITS_GROUP1))
		return 0; //Don't copy and return 0 length

	uint8_t FlagId = ErrLogData[ErrorLogIndex].ErrFlag;
	uint8_t Index = ErrLogData[ErrorLogIndex].ErrIndex;

	if(((FlagId==2)&&(Index==6))||((FlagId==0)&&(Index==17)))
	{
		Reminder_Id=Index;
		SendN16DataToDisplay(ERR_ACK_EN_DIS, 1);
	}
	else
	{
		SendN16DataToDisplay(ERR_ACK_EN_DIS, 0);
	}

	if(EventLogTable[FlagId][Index].EvSetDescp)
	{
		char *StrPtr = strchr(EventLogTable[FlagId][Index].EvSetDescp, ',');
		StrPtr++; //Skip ',' char
		StrPtr++; //Skip ' ' char
		uint8_t MsgLen = strlen(StrPtr);
		if(MsgLen>245)
		{
			MsgLen=245;
		}
		memcpy(Msgptr, StrPtr, MsgLen+1); // Adding NULL char
		return MsgLen+1;
	}
	return 0;
}

/*
 * *********************************
 * *********************************
 * End of the module: ErrorMon.c
 * *********************************
 * *********************************
 */

