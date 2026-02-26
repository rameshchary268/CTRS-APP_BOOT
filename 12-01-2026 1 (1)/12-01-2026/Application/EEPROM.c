/*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: EERPOM.c
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 *
 * Description:
 * This module contains definitions for data store/read with an I2C driven EEPROM IC STMicro M24M02-DR (2Mb / 256KB).
 * The data to be stored are process parameters / user configurations that are field programmable by user at run time.
 *
 * Note:
 *  MCU supported: STM32
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include <math.h>

#include "MyMain.h"

/* Defines */

/* Declaration of external global variables */

// Structures being stored in EEPROM as individual bytes
uint8_t  I2C1_tx_rdy=0;
uint8_t  I2C1_rx_rdy=0;

/* Declaration of global variables */
uint16_t        i2c_err_sts;
uint8_t         bytes_temp[4];

const uint8_t eeprom_seg_addr[] = {
  EEPROM_SEG1_ADDR,
  EEPROM_SEG2_ADDR,
  EEPROM_SEG3_ADDR,
  EEPROM_SEG4_ADDR,
};

/* Declaration of static local variables */

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
 * Function name: I2C1_TxCallback
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the call-back function of I2C1 transmit interrupt events
 * Parameters:
 *  Input  : Handle to I2C1 structure
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void I2C1_TxCallback(I2C_HandleTypeDef *hi2c)
{
  I2C1_tx_rdy = 1;
}

/*
 * *****************************************************************************
 * Function name: I2C1_RxCallback
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the call-back function of I2C1 receive interrupt events
 * Parameters:
 *  Input  : Handle to I2C1 structure
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void I2C1_RxCallback(I2C_HandleTypeDef *hi2c)
{
  I2C1_rx_rdy = 1;
}

/*
 * *****************************************************************************
 * Function name: I2C1_ErrCallback
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the call-back function of I2C1 error interrupt events
 * Parameters:
 *  Input  : Handle to I2C1 structure
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void I2C1_ErrCallback(I2C_HandleTypeDef *hi2c)
{
  i2c_err_sts = hi2c->Instance->ISR;
}

/*
 * *****************************************************************************
 * Function name: EEPROM_init()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function to initialize the EEPROM structure.
 * Parameters:
 *  Input:  None
 *  Return: None
 * Note:
 *
 * *****************************************************************************
*/
void EEPROM_init(void)
{

  if ((HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MASTER_TX_COMPLETE_CB_ID, I2C1_TxCallback) == HAL_OK) &&
	  (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MASTER_RX_COMPLETE_CB_ID, I2C1_RxCallback) == HAL_OK) &&
	  (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_ERROR_CB_ID, I2C1_ErrCallback) == HAL_OK))
  {
    // Callback functions initialized
  }
}


/*
 * *****************************************************************************
 * Function name: bytestowrite()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function is used write no. of bytes in a page.
 * Parameters:
 *  Input: size, offset, extrabytes.
 *  Return: size of bytes to be written in page.
 * Note:
 *
 * *****************************************************************************
*/
uint16_t bytestowrite (uint16_t size, uint16_t offset,uint16_t  extrabytes)
{
	if ((size+offset+extrabytes)<EEP_PAGE_SIZE) return size;
	else return EEP_PAGE_SIZE-offset-extrabytes;
}
/*
 * *****************************************************************************
 * Function name: EEPROM_Write()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function is used write the data to the EEPROM
 * Parameters:
 *  Input: size, offset, Memaddress, data.
 *  Return: None.
 * Note:
 *
 * *****************************************************************************
*/

	/* write the data to the EEPROM
	 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
	 * @offset is the start byte offset in the page. Range from 0 to EEP_PAGE_SIZE-1
	 * @data is the pointer to the data to write in bytes
	 * @size is the size of the data
	 */
void EEPROM_Write (uint16_t MemAddress, uint16_t offset, uint8_t *data, uint16_t size)
{
    uint8_t i;
    uint16_t bytesremaining;
	// calculate the start page and the end page
	uint16_t startPage =(MemAddress/EEP_PAGE_SIZE);
	uint16_t endPage =  ((MemAddress+size+offset)/EEP_PAGE_SIZE);
	// number of pages to be written
	uint16_t pos=0;
	uint16_t  extrabytes = (MemAddress%EEP_PAGE_SIZE);
	uint16_t numofpages = (endPage-startPage) + 1;
	// write the data
	for ( i=0; i<numofpages; i++)
	{
	    bytesremaining = bytestowrite(size, offset, extrabytes);  // calculate the remaining bytes to be written

		if(HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000) !=HAL_OK)  // write the data to the EEPROM
		{
			System_Status[ERROR_FLAG_0] |= ERR_EEPROM_FAILURE;
		}
		else
		{
			System_Status[ERROR_FLAG_0] &= ~ERR_EEPROM_FAILURE;
		}
		startPage += 1;  // increment the page, so that a new page address can be selected for further write
		offset=0;   // since we will be writing to a new page, so offset will be 0
		size = size-bytesremaining;  // reduce the size of the bytes
		pos += bytesremaining;  // update the position for the data buffer
		extrabytes = 0;
		HAL_Delay (10);  // Write cycle delay (5ms)
	}
}

/*
 * *****************************************************************************
 * Function name: EEPROM_Read()
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the function is used Read the data from the EEPROM
 * Parameters:
 *  Input: size, offset, Memaddress, data.
 *  Return: None.
 * Note:
 *
 * *****************************************************************************
*/
	/* READ the data from the EEPROM
	 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
	 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
	 * @data is the pointer to the data to write in bytes
	 * @size is the size of the data
	 */
void EEPROM_Read (uint16_t MemAddress, uint16_t offset, uint8_t *data, uint16_t size)
{
	uint8_t i;
	uint16_t bytesremaining;
	uint16_t startPage =(MemAddress/EEP_PAGE_SIZE);
	uint16_t endPage =  ((MemAddress+size+offset)/EEP_PAGE_SIZE);

	uint16_t numofpages = (endPage-startPage) + 1;
	uint16_t pos=0;
	uint16_t  extrabytes = (MemAddress%EEP_PAGE_SIZE);
	for ( i=0; i<numofpages; i++)
	{
		//uint16_t MemAddress = startPage<<paddrposition | offset;
		bytesremaining = bytestowrite(size, offset, extrabytes);
		if(HAL_I2C_Mem_Read(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000) != HAL_OK)
		{
			System_Status[ERROR_FLAG_0] |= ERR_EEPROM_FAILURE;
		}
		else
		{
			System_Status[ERROR_FLAG_0] &= ~ERR_EEPROM_FAILURE;
		}
		startPage += 1;
		offset=0;
		extrabytes =0;
		size = size-bytesremaining;
		pos += bytesremaining;
	}
}
//void EEPROM_Write_add (uint16_t MemAddress, uint16_t offset, uint8_t *data, uint16_t size)
//{
//	uint16_t bytesremaining;
//	bytesremaining =size;  // calculate the remaining bytes to be written
//	uint16_t pos=0;
//	if(HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000) !=HAL_OK)  // write the data to the EEPROM
//	{
//		System_Status[ERROR_FLAG_0] |= ERR_EEPROM_FAILURE;
//	}
//	else
//	{
//		System_Status[ERROR_FLAG_0] &= ~ERR_EEPROM_FAILURE;
//	}
//	HAL_Delay (10);  // Write cycle delay (5ms)
//}
//
//void EEPROM_Read_add (uint16_t MemAddress, uint16_t offset, uint8_t *data, uint16_t size)
//{
//	uint16_t bytesremaining;
//	uint16_t pos=0;
//	bytesremaining = size;
//	if(HAL_I2C_Mem_Read(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000) != HAL_OK)
//	{
//		System_Status[ERROR_FLAG_0] |= ERR_EEPROM_FAILURE;
//	}
//	else
//	{
//		System_Status[ERROR_FLAG_0] &= ~ERR_EEPROM_FAILURE;
//	}
//}
/*
 * *********************************
 * *********************************
 * End of the module: EEPROM.c
 * *********************************
 * *********************************
 */
