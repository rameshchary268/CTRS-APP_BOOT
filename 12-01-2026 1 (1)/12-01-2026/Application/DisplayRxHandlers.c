/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: DisplayRxHandlers.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains VP data receive handling functions for the LCD module
 *
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DisplayRxHandlers.h"
#include "DispVPAddrList.h"
#include "DisplayTxHandlers.h"
#include "DisplayCmdHandlers.h"
#include "math.h"
#include "UI.h"

/* Declaration of external functions */

/* Declaration of external global variables */

/* Function Declarations */
/* Declaration of global variables */

/* Function Declarations start here */
/*
 * *****************************************************************************
 * Function name: DisplayHandshakeRespHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle system parameter settings handling
 * Parameters:
 *  Input  : cmd code, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void DisplayHandshakeRespHandler(uint8_t Cmdcode, uint8_t *DataPtr, uint16_t Size)
{
	if(UI_info.state == UI_INIT)
	{
		InitSeqResponseAck(Cmdcode);
	}
}
/*
 * *****************************************************************************
 * Function name: SytemParamSettingsRxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle system parameter settings handling
 * Parameters:
 *  Input  : VP Address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SytemParamSettingsRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size)
{
	//empty
}

/*
 * *****************************************************************************
 * Function name: UserActionEventRxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle user action events data
 * Parameters:
 *  Input  : VP address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void UserActionEventRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size)
{
	VP_EVNT_RD_STR   vp_evnt_rd_str;

	memcpy((uint8_t*)&vp_evnt_rd_str, DataPtr, sizeof(vp_evnt_rd_str));
    //Reset_event(&vp_evnt_rd_str);
}

/*
 * *****************************************************************************
 * Function name: UserCredentialsRxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle user credentials data
 * Parameters:
 *  Input  : VP address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void UserCredentialsRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size)
{
//empty
}

/*
 * *****************************************************************************
 * Function name: SystemCalibrationRxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle system calibration data
 * Parameters:
 *  Input  : VP address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SystemCalibrationRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size)
{

}

/*
 * *****************************************************************************
 * Function name: SystemBISTDataRxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle BIST data
 * Parameters:
 *  Input  : VP address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SystemBISTDataRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size)
{
}

/*
 * *****************************************************************************
 * Function name: DefaultRxHandler
 * Created by : Venkatesh Arikanti
 * Date:        14-10-2025
 * Description:
 *  Function to handle VP read data
 * Parameters:
 *  Input  : VP address, RX buffer pointer and packet size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
int16_t RxValue = 0;
void DefaultRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t Value = 0;

	Value = DataPtr[0]<<8;
	Value |= DataPtr[1];
	if(VPAddr == 0x081200)
	{
		RxValue = Value;
//		SendVPDataCmdToDisplay(DISP_TEST_RXVAL_CMD);
	}

}

uint16_t const CRC16[256]={
/* 16: 8005 reflected */

0x0000,0xc0c1,0xc181,0x0140,0xc301,0x03c0,0x0280,0xc241,0xc601,0x06c0,0x0780,0xc741,0x0500,0xc5c1,0xc481,0x0440,

0xcc01,0x0cc0,0x0d80,0xcd41,0x0f00,0xcfc1,0xce81,0x0e40,0x0a00,0xcac1,0xcb81,0x0b40,0xc901,0x09c0,0x0880,0xc841,

0xd801,0x18c0,0x1980,0xd941,0x1b00,0xdbc1,0xda81,0x1a40,0x1e00,0xdec1,0xdf81,0x1f40,0xdd01,0x1dc0,0x1c80,0xdc41,

0x1400,0xd4c1,0xd581,0x1540,0xd701,0x17c0,0x1680,0xd641,0xd201,0x12c0,0x1380,0xd341,0x1100,0xd1c1,0xd081,0x1040,

0xf001,0x30c0,0x3180,0xf141,0x3300,0xf3c1,0xf281,0x3240,0x3600,0xf6c1,0xf781,0x3740,0xf501,0x35c0,0x3480,0xf441,

0x3c00,0xfcc1,0xfd81,0x3d40,0xff01,0x3fc0,0x3e80,0xfe41,0xfa01,0x3ac0,0x3b80,0xfb41,0x3900,0xf9c1,0xf881,0x3840,

0x2800,0xe8c1,0xe981,0x2940,0xeb01,0x2bc0,0x2a80,0xea41,0xee01,0x2ec0,0x2f80,0xef41,0x2d00,0xedc1,0xec81,0x2c40,

0xe401,0x24c0,0x2580,0xe541,0x2700,0xe7c1,0xe681,0x2640,0x2200,0xe2c1,0xe381,0x2340,0xe101,0x21c0,0x2080,0xe041,

0xa001,0x60c0,0x6180,0xa141,0x6300,0xa3c1,0xa281,0x6240,0x6600,0xa6c1,0xa781,0x6740,0xa501,0x65c0,0x6480,0xa441,

0x6c00,0xacc1,0xad81,0x6d40,0xaf01,0x6fc0,0x6e80,0xae41,0xaa01,0x6ac0,0x6b80,0xab41,0x6900,0xa9c1,0xa881,0x6840,

0x7800,0xb8c1,0xb981,0x7940,0xbb01,0x7bc0,0x7a80,0xba41,0xbe01,0x7ec0,0x7f80,0xbf41,0x7d00,0xbdc1,0xbc81,0x7c40,

0xb401,0x74c0,0x7580,0xb541,0x7700,0xb7c1,0xb681,0x7640,0x7200,0xb2c1,0xb381,0x7340,0xb101,0x71c0,0x7080,0xb041,

0x5000,0x90c1,0x9181,0x5140,0x9301,0x53c0,0x5280,0x9241,0x9601,0x56c0,0x5780,0x9741,0x5500,0x95c1,0x9481,0x5440,

0x9c01,0x5cc0,0x5d80,0x9d41,0x5f00,0x9fc1,0x9e81,0x5e40,0x5a00,0x9ac1,0x9b81,0x5b40,0x9901,0x59c0,0x5880,0x9841,

0x8801,0x48c0,0x4980,0x8941,0x4b00,0x8bc1,0x8a81,0x4a40,0x4e00,0x8ec1,0x8f81,0x4f40,0x8d01,0x4dc0,0x4c80,0x8c41,

0x4400,0x84c1,0x8581,0x4540,0x8701,0x47c0,0x4680,0x8641,0x8201,0x42c0,0x4380,0x8341,0x4100,0x81c1,0x8081,0x4040
};


static inline uint16_t rshiftu16(uint16_t value, int nb)
{
return (uint16_t)((value >> nb) & ~((( uint16_t) 0x8000) >> (nb-1)));
}

uint16_t crc16_calc(unsigned char *q, int len)
{
  uint16_t crc = 0xffff;

  while (len-- > 0)
  {
    crc = (rshiftu16(crc, 8) ^ CRC16[(crc ^ *q++) & 0xff]);
  }
  return crc;
}




