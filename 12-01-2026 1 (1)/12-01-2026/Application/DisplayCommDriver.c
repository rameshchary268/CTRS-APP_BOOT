/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: DisplayCommDriver.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains definitions for UI communication process over UART1
 * as the TFT module used is TOPWAY HKT043ATA-5C
 *
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "DisplayCommDriver.h"
#include "DisplayRxHandlers.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
#include "TopwayCmdList.h"
#include "ErrorLog.h"
#include "mymain.h"

/* Declaration of external global variables */

/* Declaration of global variables */
CirBuffStruct LCDRxCirBuff;
CirBuffStruct LCDTxCirBuff;

uint8_t LCDUARTRxBuff[MAX_NO_RX_BUFFERS][MAX_UART_BUFF_SIZE];
uint8_t LCDUARTTxBuff[MAX_UART_BUFF_SIZE];
uint8_t LCDAppBuff[MAX_UART_BUFF_SIZE];

volatile uint8_t UARTTxCompleted = 1;
volatile uint8_t UARTRxCompleted = 1;
volatile uint8_t UARTRxErrorDetected = 0;
volatile uint32_t PrevDMACtr = 0;
volatile uint32_t DMACtr = 0;
uint32_t uart_tx_dly_flag=0;

uint8_t UI_Communication_Disconnected_Flg;
extern VERSIONS Control_versions;
uint16_t Time_out=0;
/* Function Declarations start here */
/*
 * *****************************************************************************
 * Function name: IsDataAvailable
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to get the data size available in the circular buffer
 * Parameters:
 *  Input  : Circular Buff pointer
 *  Return : Data size in the buffer
 * Note:
 *
 * *****************************************************************************
 */
uint16_t IsDataAvailable(CirBuffStruct *Buff)
{
	if(Buff->WrPtr == Buff->RdPtr)
	{
		return 0;
	}
	else if(Buff->WrPtr > Buff->RdPtr)
	{
		return Buff->WrPtr - Buff->RdPtr;
	}
	else if(Buff->WrPtr < Buff->RdPtr)
	{
		return (MAX_CIRBUFF_SIZE - Buff->RdPtr) + Buff->WrPtr;
	}

	return 0;
}

/*
 * *****************************************************************************
 * Function name: IsSpaceAvailable
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to get the Free space available in the circular buffer
 * Parameters:
 *  Input  : Pointer to Circular Buff
 *  Return : free space available in the buffer
 * Note:
 *
 * *****************************************************************************
 */
uint16_t IsSpaceAvailable(CirBuffStruct *Buff)
{
	return MAX_CIRBUFF_SIZE - IsDataAvailable(Buff);
}

/*
 * *****************************************************************************
 * Function name: DiscardBuffData
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to discard unwanted data in the circular buffer
 * Parameters:
 *  Input  : Pointer to Circular Buff, Data size to discard
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void DiscardBuffData(CirBuffStruct *Buff, uint16_t Size)
{
	if( (Buff->RdPtr + Size) >= MAX_CIRBUFF_SIZE)
	{
		Buff->RdPtr = (Buff->RdPtr + Size) - MAX_CIRBUFF_SIZE;
	}
	else
	{
		Buff->RdPtr +=  Size;
	}
}

/*
 * *****************************************************************************
 * Function name: WriteDataToBuff
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to write data to circular buffer
 * Parameters:
 *  Input  : Pointer to Circular Buff, Data pointer, size
 *  Return : Number of bytes written to the buff
 * Note:
 *
 * *****************************************************************************
 */
uint16_t WriteDataToBuff(CirBuffStruct *Buff, uint8_t *DataPtr, uint16_t Size)
{
	if( IsSpaceAvailable(Buff) < Size)
	{
		return 0;
	}

	for(uint16_t Ctr=0; Ctr < Size; Ctr++)
	{
		Buff->CBuff[Buff->WrPtr] = *DataPtr;
		DataPtr++;
		Buff->WrPtr++;
		if(Buff->WrPtr >= MAX_CIRBUFF_SIZE)
		{
			Buff->WrPtr = 0;
		}
	}

	return Size;
}

/*
 * *****************************************************************************
 * Function name: ReadDataFromBuff
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to read data from circular buffer
 * Parameters:
 *  Input  : Pointer to Circular Buff, Data pointer to copy data and size
 *  Return : Number of bytes read
 * Note:
 *
 * *****************************************************************************
 */
uint16_t ReadDataFromBuff(CirBuffStruct *Buff, uint8_t *DataPtr, uint16_t Size)
{
	uint16_t AvailableDataSize = IsDataAvailable(Buff);
	uint16_t DataCtr = 0;

	if(!AvailableDataSize)
	{
		return 0;
	}

	for(DataCtr = 0; (DataCtr < AvailableDataSize) && (DataCtr < Size); DataCtr++)
	{
		*DataPtr = Buff->CBuff[Buff->RdPtr];
		DataPtr++;
		Buff->RdPtr++;
		if(Buff->RdPtr >= MAX_CIRBUFF_SIZE)
		{
			Buff->RdPtr = 0;
		}
	}
	return DataCtr;
}

/*
 * *****************************************************************************
 * Function name: WriteDataToTxBuff
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to write data ro Tx circular buffer the circular buffer
 * Parameters:
 *  Input  : Data pointer and Size
 *  Return : No of Bytes written to buffer
 * Note: This interface is defined to use from other files
 *
 * *****************************************************************************
 */
uint16_t WriteDataToTxBuff(uint8_t *DataPtr, uint16_t Size)
{
	uint16_t NoOfDataSent = WriteDataToBuff(&LCDTxCirBuff, DataPtr, Size);

	if(NoOfDataSent != Size) //Log Error
	{
		System_Status[ERROR_FLAG_0]  |= ERR_DISP_TX_OVERFLOW;
		UARTTxCompleted=1;
		UARTRxErrorDetected = 1;
	}
	else
	{
		System_Status[ERROR_FLAG_0]  &= ~ERR_DISP_TX_OVERFLOW;
	}

	return NoOfDataSent;
}

/*
 * *****************************************************************************
 * Function name: InitLCDCirBuffer
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to Initialize circular buffer
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void InitLCDCirBuffer(void)
{
	LCDRxCirBuff.WrPtr = 0;
	LCDRxCirBuff.RdPtr = 0;
	LCDRxCirBuff.State = CIRBUFF_EMPTY;

	LCDTxCirBuff.WrPtr = 0;
	LCDTxCirBuff.RdPtr = 0;
	LCDTxCirBuff.State = CIRBUFF_EMPTY;

	//Start receive DMA
	if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart6, LCDUARTRxBuff[0], MAX_UART_BUFF_SIZE))
	{
		UARTRxErrorDetected = 1;
	}
}

/*
 * *****************************************************************************
 * Function name: ParseLCDRxData
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Parse the data recved from LCD module and get a complete frame
 * Parameters:
 *  Input  : Circular buff pointer, data pointer to copy frame and size of the array
 *  Return : 0 - no valid frame, size of the frame, if found a valid frame
 * Note: This function works on the buffer data, doesn't read data from buffer
 *
 * *****************************************************************************
 */
uint16_t ParseLCDRxData(CirBuffStruct *Buff, uint8_t *DataPtr, uint16_t Size)
{
	static uint16_t StartOfNewFrame = 0;
	uint16_t Ctr = 0;
	uint16_t BuffRdPtr = Buff->RdPtr;

	uint16_t CRC_VLU;
	uint8_t CRC_LSB , CRC_MSB;

	if(Size == 0)
		return 0;

	//Find the start char
	if(StartOfNewFrame == 0)
	{
		for(Ctr = 0; Ctr < Size; Ctr++)
		{
			if(Buff->CBuff[BuffRdPtr] == 0xAA)
			{
				StartOfNewFrame = 1;
				//Discard unwanted data
				DiscardBuffData(Buff,Ctr);
				break;
			}
			BuffRdPtr++;
			if(BuffRdPtr >= MAX_CIRBUFF_SIZE)
			{
				BuffRdPtr = 0;
			}
		}
	}
	//CRC_VLU = crc16_calc(&Buff->CBuff[Size+3],Size-5);

	//Look for Delimiters
	if(StartOfNewFrame)
	{
		for(; Ctr < Size-3; Ctr++)
		{
			if( (Buff->CBuff[BuffRdPtr] == 0xCC) && \
				(Buff->CBuff[(BuffRdPtr+1) & CIRBUFF_SIZE_MASK] == 0x33) )
			{
				CRC_VLU = crc16_calc(&Buff->CBuff[Buff->RdPtr+3],Ctr-1);
				CRC_MSB = ((uint8_t)((CRC_VLU & 0xff00) >> 8));//;FOOTER3;
				CRC_LSB = (uint8_t)(CRC_VLU & 0x00ff);//FOOTER4;

				if(	(Buff->CBuff[BuffRdPtr+2] == CRC_LSB) && \
					(Buff->CBuff[BuffRdPtr+3] == CRC_MSB) )
				{
					ReadDataFromBuff(Buff, DataPtr, Ctr+4);//Valid frame found
					StartOfNewFrame = 0;
					return Ctr+4;
				}
				else
				{
					DiscardBuffData(Buff,Ctr+2);
					StartOfNewFrame = 0;
				}
			}
			BuffRdPtr++;
			if(BuffRdPtr >= MAX_CIRBUFF_SIZE)
			{
				BuffRdPtr = 0;
			}
		}
	}
	return 0;
}


/*
 * *****************************************************************************
 * Function name: ProcessLCDDataFrame
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Process the data recved from LCD module, this will call the respective processing functions
 * Parameters:
 *  Input  : Frame pointer and size of the packet
 *  Return : None
 * Note: Add more cases to handle other commands
 *
 * *****************************************************************************
 */
void ProcessLCDDataFrame(uint8_t *FrmPtr, uint16_t Size)
{

	switch(FrmPtr[3])
	{
		//Touch key event packet
		case TOPWAY_TOUCH_VP_ADDR_VALUE_EVENT:
            Time_out=0;
			HandleUIDataEntryEvent(FrmPtr[3], FrmPtr, Size);
			break;

		//Successive Read
		case TOPWAY_SUCCESSIVE_READ_CMD:
			Time_out=0;

			break;
		case TOPWAY_STR_READ_CMD:
            strncpy((char*)&Control_versions.UI_version,(const char*)&FrmPtr[4],5);
            if(strstr((const char*)Control_versions.UI_version,(const char*)UI_FW_VER)==0)
            {
            	System_Status[EVENT_FLAG] |= EVT_DISPLAY_VERIFY;
            }
            else
            {
            	System_Status[EVENT_FLAG] &= ~EVT_DISPLAY_VERIFY;
            }
			Time_out=0;
			break;

		case TOPWAY_N16_READ_CMD:
			Time_out=0;
			//ParseVPRxData(FrmPtr[3],FrmPtr, Size);
			break;

		case TOPWAY_TOUCH_RELEASE_KEYID_EVENT:
		case TOPWAY_TOUCH_DOWN_KEYID_EVENT:
			Time_out=0;
			HandleTouchKeyPressEvent(FrmPtr[3],(uint16_t)(((uint16_t)FrmPtr[4]<<8)|FrmPtr[5]),FrmPtr[6]);
			break;

		//Handshake response
		case TOPWAY_HANDSHAKE_CMD:
			DisplayHandshakeRespHandler(FrmPtr[3], FrmPtr, Size);
			UICommFaildCounter=0;
			break;

		case TOPWAY_READ_VERSION_CMD:
			break;

		case TOPWAY_READ_PAGEID_CMD:
			break;

		default:
			break;

	}
}

/*
 * *****************************************************************************
 * Function name: ProcessLCDRxData
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Process Received data from LCDProcessLCDRxData
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void ProcessLCDRxData(void)
{
	uint16_t FrameLength = ParseLCDRxData(&LCDRxCirBuff, LCDAppBuff, IsDataAvailable(&LCDRxCirBuff));

	if(FrameLength)
	{
		ProcessLCDDataFrame(LCDAppBuff, FrameLength);
	}

}

/*
 * *****************************************************************************
 * Function name: LCDDataProc
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Main function to handle Tx and RX data communication
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LCDDataProc(void)
{
	uint16_t TxDataSize = 0;

	//Incase of error abort DMA and restart the rx
	if(UARTRxErrorDetected)
	{
		HAL_UART_AbortReceive(&huart6);

		if(HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart6, LCDUARTRxBuff[0], MAX_UART_BUFF_SIZE))
		{
			UARTRxErrorDetected = 0;
		}
	}

	//Process rxed data
	if(UARTRxCompleted)
	{
		uint16_t RecvedDataSize = 0;
		UARTRxCompleted = 0;

		if(PrevDMACtr < DMACtr)
		{
			RecvedDataSize = PrevDMACtr + (MAX_UART_BUFF_SIZE - DMACtr);
		}
		else
		{
			RecvedDataSize = PrevDMACtr - DMACtr;
		}

		if(RecvedDataSize && RecvedDataSize<1024)
		{
			for(uint16_t Ctr = 0; Ctr < RecvedDataSize; Ctr++)
			{
				LCDUARTRxBuff[1][Ctr] = LCDUARTRxBuff[0][(MAX_UART_BUFF_SIZE - PrevDMACtr + Ctr)&UART_BUFF_SIZE_MASK];
			}
			WriteDataToBuff(&LCDRxCirBuff, LCDUARTRxBuff[1], RecvedDataSize);
		}
		PrevDMACtr = DMACtr;
	}


	// Transmit data if added to Tx buffer
	TxDataSize = IsDataAvailable(&LCDTxCirBuff);

	if(TxDataSize && (UARTTxCompleted || uart_tx_dly_flag))
	{
		UARTTxCompleted = 0;
		if(TxDataSize > MAX_UART_BUFF_SIZE)
		{
			TxDataSize = MAX_UART_BUFF_SIZE;
		}

		ReadDataFromBuff(&LCDTxCirBuff, LCDUARTTxBuff, TxDataSize);
		Set_delay(SW_TMR_DIS_POLL, DLY_WIFI_STRT+DLY_WIFI_STRT, &uart_tx_dly_flag);
		if(HAL_OK != HAL_UART_Transmit_DMA(&huart6, LCDUARTTxBuff, TxDataSize))
		{
			System_Status[ERROR_FLAG_0] |= ERR_DISP_TX_OVERFLOW;
			UARTTxCompleted=1;
		}
		else
		{
			System_Status[ERROR_FLAG_0] &= ~ERR_DISP_TX_OVERFLOW;
		}
	}
}
void ProcessLCDData()
{
	uint16_t RxDataSize = 0;
	RxDataSize = IsDataAvailable(&LCDRxCirBuff);

		if(RxDataSize)
		{
			if(RxDataSize > MAX_UART_BUFF_SIZE)
			{
				RxDataSize = MAX_UART_BUFF_SIZE;
			}
			ProcessLCDRxData();
		}
}

/*
 * *****************************************************************************
 * Function name: HAL_UART_ErrorCallback
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Callback function in case of error in uart handling
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart6)
	{
		UARTRxErrorDetected = 1;
	}
	else if (huart == &huart1)
	{
		wifi_info.UART1ErrorDetected = 1;
    	//wifi_info.state=WIFI_COMM_FAIL;

    	BLE_info.UART2ErrorDetected = 1;
    	BLE_info.state=BLE_COMM_FAIL;
	}
	else if(huart == &huart3)
	{
		RS485_info.UART3RxErrorDetected = 1;
	}
}

/*
 * *****************************************************************************
 * Function name: HAL_UARTEx_RxEventCallback
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Callback function for Receive complete
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &huart6)
	{
		DMACtr = huart->RxXferCount;

		UARTRxCompleted = 1;
	}
	else if (huart == &huart1)
	{
		wifi_info.RxCplt = 1;
		BLE_info.RxCplt = 1;
		uart1_info.rx_size = Size; // Store actual size

		if (read_size < WIFI_MTU*10)
		{
			memcpy(&wifi_rx_copy[read_size], wifi_rx_bfr, Size);
		}
		 read_size+=Size;

		HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, 10 * WIFI_MTU);
	}
	else if(huart == &huart3)
	{
		UART3DMACtr = huart->RxXferCount;

		RS485_info.rx_done = 1;
	}
}

/*
 * *****************************************************************************
 * Function name: HAL_UART_TxCpltCallback
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Calback function for Transmit complete
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart6)
	{
		UARTTxCompleted = 1;
	}
	else if (huart == &huart1)
	{
		 wifi_info.TxCplt = 1;
		 BLE_info.TxCplt = 1;
	}
	else if(huart == &huart3)
	{
		RS485_info.tx_done = 1;
//		    RS485_Dir(RS485_RECEIVE);
	}
}

