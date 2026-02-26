/*
 * MsgProc.c
 *
 *  Created on: 24-Aug-2022
 *      Author: Ilensys
 */

/* Includes */

#include "MyMain.h"
#include "MsgProc.h"
#include "UISettings.h"

/* Declaration of external global variables */
extern NODE_INFO        node_info[];


/* Declaration of global variables */
CIR_BUF rs485_tx_cir_buf[IOT_CIR_BUF_DEPTH];  // Circular buffer array that holds messages to be transmitted over RS485
CIR_BUF rs485_rx_cir_buf[IOT_CIR_BUF_DEPTH];  // Circular buffer array that holds messages received over RS485
CIR_BUF *cir_buf;
SD_CIR_BUF *SD_cir_buf;

uint8_t msg_bfr[IOT_CIR_BUF_SIZE];
IOT_FOTA_MSG iot_fota_msg;

RT_DATA rt_data;

char *day[7] = {
  ":Mon:",
  ":Tue:",
  ":Wed:",
  ":Thu:",
  ":Fri:",
  ":Sat:",
  ":Sun:",
};

FOTA_STR fota_str = FOTA_STR_DEFAULT;

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
 * Function name: Msg_init()
 * Created by: Natarajan Kanniappan
 * Date:       03-Aug-2022
 * Description:
 *  This function initializes message processing module, circular buffers and the
 *  Msg_Tx / Msg_Rx state machines
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Msg_init(void)
{
  uint8_t i;

  for (i=0; i<IOT_CIR_BUF_DEPTH; i++)
  {
	rs485_tx_cir_buf[i].state = BUF_EMPTY;
	rs485_rx_cir_buf[i].state = BUF_EMPTY;
  }
}

/*
 * *****************************************************************************
 * Function name: Process_Message()
 * Created by: Natarajan Kanniappan
 * Date:       13-Aug-2022
 * Description:
 *  This function processes messages from various channels & process them
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Process_Message(void)
{
  int8_t i;
  uint8_t j;

  // Check for any messages received at RS485 channel
  if ((i = Find_Rx_buf(BUF_FULL, COMM_RS485)) >= 0)
  {
    // Found a message received that need to be processed
	if (vp_param_rw_str.mb_cfg.node_id == RS485_MASTER_NODE_ID)
	{
	  // Master mode. Process the response(only) from slave nodes
      if (Process_Modbus_Response(rs485_rx_cir_buf[i].buffer, rs485_rx_cir_buf[i].size) == 0)
      {
        // Response verified. Update active node list
    	for (j=0; ((j<MB_MAX_NODES) && (j != rs485_rx_cir_buf[i].buffer[0])); j++)
    	{
          continue;
    	}
   	    if (j < MB_MAX_NODES)
		{
   	      node_info[j].active = 1;
     	}
		else
		{
		  node_info[j].active = 0;
		}
      }
	}
	else
	{
		 memcpy(&test_buf,rs485_rx_cir_buf[i].buffer,rs485_rx_cir_buf[i].size);
	  // Slave node. Process the incoming message for commands and prepare response accordingly.
		 if((rs485_rx_cir_buf[i].buffer[0]== 0xAA && rs485_rx_cir_buf[i].buffer[1]== 0x44 && rs485_rx_cir_buf[i].buffer[2]== 0x31) )//&& vp_param_rw_str.STM_EN_4_20_mA)
		 {

		 }
		 else
		 {
			 if (Process_Modbus_Command(rs485_rx_cir_buf[i].buffer, rs485_rx_cir_buf[i].size) == 0)
			  {
				// Message successfully processed. Reply is already pushed into the transmit circular buffer
			  }
			  rs485_rx_cir_buf[i].state = BUF_EMPTY;   // Anyway, release the Rx buffer
		 }
	}
  }


}

/*
 * *****************************************************************************
 * Function name: Add_Timestamp()
 * Created by: Natarajan Kanniappan
 * Date:       13-Aug-2022
 * Description:
 *  This function prepares the status/events/error message and push to the
 *  available appropriate transmit circular buffer.
 *  Data message format example (non RS-485):
 *    29-09-22:Thu:12:17:22:Data->xxxxxxxx...$
 * Parameters:
 *  Input  : Pointer to the message to be time-stamped
 *  Return : Length
 * Note:
 *
 * *****************************************************************************
 */
uint8_t Add_Timestamp(uint8_t *msg)
{
  uint8_t *ptr = msg;
  TIME    time;

  if (msg == NULL)
    return 0;

  // Valid buffer is available for pushing the time stamp info.
  Get_TimeStamp();
  time.tenth_of_second = Time.SubSeconds/10;
  time.second = Time.Seconds;
  time.minute = Time.Minutes;
  time.hour = Time.Hours;
  time.day = Date.WeekDay;
  time.date = Date.Date;
  time.month = Date.Month;
  time.year = Date.Year;
  *ptr = 0x00;
  uint8_t Len = sprintf((char*)ptr, "%2d-%2d-%2d%s%2d:%2d:%2d:", time.date,time.month, time.year,(char*)day[time.day],\
		                                                         time.hour, time.minute, time.second);
  return Len;
}

/*
 * *****************************************************************************
 * Function name: Push_Message()
 * Created by: Natarajan Kanniappan
 * Date:       13-Aug-2022
 * Description:
 *  This function prepares the status/events/error message and push to the
 *  available appropriate transmit circular buffer.
 *  Data message format example (non RS-485):
 *    29-09-22:Thu:12:17:22:Data->xxxxxxxx...$
 * Parameters:
 *  Input  : Pointer to the message to be pushed, Length, Communication mode, Type, Category & Command
 *  Return : Status (0-Success, 1-Failed)
 * Note:
 *
 * *****************************************************************************
 */
int8_t Push_Message(uint8_t *msg, uint8_t len, COMM_MODE mode, MSG_TYPE type, CAT_INFO cat_info, uint8_t cmd)
{
  int8_t   i;
  uint8_t  *ptr, *ptr_strt, wrap_len = 0, size = 0;
  uint16_t crc16;

  if ((i = Find_Tx_buf(BUF_EMPTY, mode)) < 0)
    return -1;

  ptr_strt = (cir_buf+i)->buffer;
  size = len;
  ptr = ptr_strt;

  if ((mode != COMM_SD) && (mode != COMM_ETH))
  {
    // Update the Tx circular buffer with header
    if (cat_info == CAT_MB)
	  *ptr = vp_param_rw_str.mb_cfg.node_id;
    else
      *ptr = (cat_info == CAT_NACK) ? HEADER_NACK_MSG : ((cat_info == CAT_ACK) ? HEADER_ACK_MSG : HEADER_ORG_MSG);
    ptr += 1;
    *ptr = cmd;              // Add the command byte
    ptr += 1;
    if(type==MSG_CMD)
    {
    	wrap_len = 2;
    }
    else
    {
    	 *ptr = len;              // Add bytes info
		 ptr += 1;
		 wrap_len = 3;
    }

  }
  memcpy(ptr, msg, len);
  ptr += len;

  if (mode == COMM_RS485)
  {
    // Calculate CRC16 and update at the end
    crc16 = usMBCRC16(ptr_strt, (uint16_t)(ptr-ptr_strt));
    *ptr = (uint8_t)(crc16 & 0x00ff);
    ptr += 1;
    *ptr = (uint8_t)((crc16 & 0xff00) >> 8);
//    *ptr = (uint8_t)(crc16 & 0x00ff);
    wrap_len += 2;
  }
  (cir_buf+i)->size = wrap_len + size;  // Account for wrapper bytes (header, command & 2 bytes of CRCC) - if for RS485
  (cir_buf+i)->state = BUF_FULL;
  return 0;
}

/*
 * *****************************************************************************
 * Function name: Get_cir_buf()
 * Created by: Natarajan Kanniappan
 * Date:       03-Aug-2022
 * Description:
 *  This function gets the index of the circular Tx buffer array that has the
 *  required state (BUF_EMPTY or BUF_FULL) and returns the index, if available.
 *  Else, it returns -1.
 * Parameters:
 *  Input  :  Communication Direction, Communication  mode
 *  Return : Pointer to the Circular Buffer
 * Note:
 *
 * *****************************************************************************
 */
CIR_BUF* Get_cir_buf(TXN_DIR dir, COMM_MODE mode)
{
  CIR_BUF *buf = NULL;

  switch (mode)
  {
    case COMM_RS485:
   	  buf = (dir == TXN_READ) ? &rs485_rx_cir_buf[0] : &rs485_tx_cir_buf[0];
	  break;
    default:
	  break;
  }
  return (buf);
}

/*
 * *****************************************************************************
 * Function name: Find_Tx_buf()
 * Created by: Natarajan Kanniappan
 * Date:       03-Aug-2022
 * Description:
 *  This function gets the index of the circular Tx buffer array that has the
 *  required state (BUF_EMPTY or BUF_FULL) and returns the index, if available.
 *  Else, it returns -1.
 * Parameters:
 *  Input  : State, communication mode
 *  Return : Index
 * Note:
 *
 * *****************************************************************************
 */
int8_t Find_Tx_buf(BUF_STATE state, COMM_MODE mode)
{
  int8_t i;

  cir_buf = Get_cir_buf(TXN_WRITE, mode);
  if (cir_buf != NULL)
  {
    for (i=0; i<IOT_CIR_BUF_DEPTH; i++)
    {
      if ((cir_buf+i)->state == state)
        return i;
    }
  }
  return -1;
}

/*
 * *****************************************************************************
 * Function name: Find_Rx_buf()
 * Created by : Natarajan Kanniappan
 * Date:        03-Aug-2022
 * Description:
 *  This function gets the index of the circular Rx buffer array that has the
 *  required state (BUF_EMPTY or BUF_FULL) and returns the index, if available.
 *  Else, it returns -1.
 * Parameters:
 *  Input  : State, communication mode
 *  Return : Index
 * Note:
 *
 * *****************************************************************************
 */
int8_t Find_Rx_buf(BUF_STATE state, COMM_MODE mode)
{
  uint8_t i;

  cir_buf = Get_cir_buf(TXN_READ, mode);
  if (cir_buf != NULL)
  {
    for (i=0; i<IOT_CIR_BUF_DEPTH; i++)
    {
      if ((cir_buf+i)->state == state)
        return i;
    }
  }
  return -1;
}


/*
 * *****************************************************************************
 * Function name: Process_Modbus_Response()
 * Created by:
 * Date:
 * Description:
 *  This function process the received Modbus response from a slave and update its
 *  request engine
 * Parameters:
 *  Input  : Pointer to the response
 *  Return : Status (0:Success, 1:Failed)
 * Note:
 *
 * *****************************************************************************
 */
int8_t Process_Modbus_Response(uint8_t *msg, uint16_t len)
{
  uint8_t  *bfr = msg;
  uint8_t  i, command;
  uint16_t crc16;

  if (len == 0)
	return 1;
  if (*bfr == vp_param_rw_str.mb_cfg.node_id)
	return 1;

  // Message length is non-zero and node id is matching. Start the processing with CRC check
  crc16 = usMBCRC16(msg+1, len-3);
  if (*(msg+len-1) != ((crc16 & 0xff00) >> 8) ||
	  *(msg+len-2) != (crc16 & 0x00ff))
  {
    // CRC check failed. Discard the message
	return 1;
  }
  node_info[*bfr].active = 1;
  // Valid response received as CRC is matching. Acknowledge and update the response engine:
  bfr++;
  command = *bfr;
  for (i=0; i<IOT_CIR_BUF_DEPTH; i++)
  {
    if ((command == rs485_tx_cir_buf[i].buffer[1]) && (rs485_tx_cir_buf[i].state == BUF_FULL))
    {
  	  rs485_tx_cir_buf[i].state = BUF_EMPTY;
  	  break;
    }
  }
  return 0;
}

/*
 * *****************************************************************************
 * Function name: Process_Modbus_Command()
 * Created by:
 * Date:
 * Description:
 *  This function process the received Modbus message from a master and initiates
 *  appropriate action. Tx Circular buffer is updated with the response Modbus message
 * Parameters:
 *  Input  : Pointer to the message buffer, Length
 *  Return : Status (0:Success, 1:Failed)
 * Note:
 *
 * *****************************************************************************
 */
int8_t Process_Modbus_Command(uint8_t *msg, uint16_t len)
{
  uint8_t  i, *bfr = msg;
  uint8_t  command, msgLen = 0;
  uint16_t strt_addr, qty;
 // uint32_t  *data_p;

  MSG_TYPE type;
  CAT_INFO category;
  command = *bfr;
  bfr++;

  // Check for supported commands:
  switch (command)
  {
    ///////////////////////////
    // Syntesy Modbus commands
    ///////////////////////////
    case CMD_RD_DIG_INPUT:  // 0x02
      // Command to send digital input status
      type = MSG_DAT;
      category = CAT_MB;
      strt_addr = (*bfr << 8) | *(bfr+1);
      qty = (*(bfr+2) << 8) | *(bfr+3);
  	  break;

    case CMD_RD_INPUT_REG:  // 0x04
      // Command to send analog data - Battery voltage, Mains voltage, DPS, Thermistor Level, RTD1 Temperature & RTD2 Temperature
   	  strt_addr = (*bfr << 8) | *(bfr+1);
   	  qty = (*(bfr+2) << 8) | *(bfr+3);
      type = MSG_DAT;
      category = CAT_MB;
      break;

    case CMD_RD_HOLDING_REG:  // 0x03
      // Command to read Process Data at holding registers
   	  strt_addr = (*bfr << 8) | *(bfr+1);
//	  vp_param_rw_str.cryolink_phase2_sel  = 0x01;
   	  qty = (*(bfr+2) << 8) | *(bfr+3);
   	  if ((strt_addr > 32) || (qty == 0)|| (qty >32))
   	  {
   	     return 0;
   	  }
      //memcpy(&msg_bfr[msgLen], (uint8_t*)data_p, qty*2);
      msgLen += qty*2;
      type = MSG_EVT;
      category = CAT_MB;
      break;

    case CMD_WR_SINGLE_REG:  // 0x06
      // Command to single write into a holding register
      strt_addr = (*bfr << 8) | *(bfr+1);

      msg_bfr[msgLen]=*bfr;
      msgLen++;
      msg_bfr[msgLen]=*(bfr+1);
	  msgLen++;
	  msg_bfr[msgLen]=*(bfr+2);
	  msgLen++;
   	  msg_bfr[msgLen] = *(bfr+3);
   	  msgLen += 1;
   	  type = MSG_CMD;
   	  category = CAT_MB;
      break;

    case CMD_DIAGNOSTIC:    // 0x08
      // Command to run the diagnostics and get the Results
      // TBD

      type = MSG_DAT;
      category = CAT_MB;
      break;

    case CMD_REPORT_SERVER_ID:   // 0x11
      // Command to report the server ID.
      // TBD

      type = MSG_DAT;
      category = CAT_MB;
      break;

    ///////////////////
    // Common commands
    ///////////////////
    case CMD_RD_DEVICE_ID:   // 0x2B
      // Read device Identification - Report with Freezer ID
      i = strlen((char*)&FrezzID[0]);
      memcpy(&msg_bfr[msgLen], (uint8_t*)FrezzID, i);
      msgLen += i;
      type = MSG_DAT;
      category = CAT_MB;
      break;

    ////////////////////////
    // Non-Syntesy commands
    ////////////////////////
    case  WRITE_RTC_SYNC_UP:   // 0x40
   	  Date.Year = *(bfr);
      Date.Month = *(bfr+1);
      Date.Date = *(bfr+2);
      Date.WeekDay = *(bfr+3);
      Time.TimeFormat = *(bfr+4);
      Time.Hours = *(bfr+5);
      Time.Minutes = *(bfr+6);
      Time.Seconds = *(bfr+7);
      Time.SecondFraction = *(bfr+8);
      msg_bfr[msgLen] = Date.Year;
	  msgLen += 1;
	  msg_bfr[msgLen] =  Date.Month;
      msgLen += 1;
	  msg_bfr[msgLen] = Date.Date;
	  msgLen += 1;
	  msg_bfr[msgLen] = Date.WeekDay;
	  msgLen += 1;
	  msg_bfr[msgLen] = Time.TimeFormat;
	  msgLen += 1;
	  msg_bfr[msgLen] = Time.Hours;
	  msgLen += 1;
	  msg_bfr[msgLen] = Time.Minutes;
	  msgLen += 1;
	  msg_bfr[msgLen] = Time.Seconds;
	  msgLen += 1;
      Set_TimeDate();
      break;

    case CMD_OFAF_FILL_STRT:   // 0x41

      type = MSG_DAT;
      category = CAT_MB;
      // TBD

   	  break;

    case CMD_OFAF_FILL_STOP:   // 0x42
      // Process the fill command - Fill request is not acknowledged and put on hold
      type = MSG_DAT;
      category = CAT_MB;
      // TBD

   	  break;

    case CMD_SEND_LOG_FILE:    // 0x56
      // Send the file (from SD card) that has all the logs since the beginning of the day - in fragments
//      // This is initiated by sending a command via SD_rd_cir_buf
//      if (((i = Find_Rx_buf(BUF_EMPTY, COMM_SD)) >= 0))
//      {
//    	SD_rd_cir_buf[i].state = BUF_FULL;
//    	SD_rd_cir_buf[i].buffer[0] = CMD_SEND_LOG_FILE;
//    	SD_rd_cir_buf[i].size = MAX_FILE_SIZE;  // Set the maximum file size
//      }
//      msg_bfr[msgLen] = CMD_SEND_LOG_FILE;
//   	  msgLen += 1;
      type = MSG_DAT;
      category = CAT_MB;
      break;

    case CMD_POLL:             // 0xF6

      type = MSG_DAT;
      category = CAT_MB;
      break;

    case CMD_RD_DEVICE_INFO:   // 0xF7
      // Read device Identification

      type = MSG_DAT;
      category = CAT_MB;
      break;

    case CMD_TEST_JIG_MODE:   // 0xFF
//	  Process_test_jig_cmd(*bfr);
      break;

    default:
      // Command not recognized
      type = MSG_DAT;
      category = CAT_NACK;
      break;
  }
#ifndef EN_MB_TCP
  if (category == CAT_NACK)
  {
	strcpy((char*)&msg_bfr[0], "Command not recognized\n");
	msgLen = strlen((char*)&msg_bfr[0]);
  }
  Push_Message(&msg_bfr[0], msgLen, COMM_ETH, type, category, command);
#else
  if(msgLen)
  {
	  Push_Message(&msg_bfr[0], msgLen, COMM_RS485, type, category, command);
  }
#endif
  return 0;
}


uint8_t modebus_error()
{
  #define ERR_DP       0X01
  #define ERR_NO_LN2   0x02
  #define ERR_IMPRP    0x04
  #define ERR_GBP_STK  0x08
  #define ERR_MFV_STK  0x10
  #define ERR_GBP_OPN  0x20
  #define ERR_MFV_OPN  0x40

  uint8_t fill_err=0x00;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_DP_SENSOR_FAILURE)       		 ? ERR_DP          : 0x0000;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_NO_LN2_SUPPLY)       		     ? ERR_NO_LN2      : 0x0000;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_LN2_IMPROPER_FILL)       		 ? ERR_IMPRP       : 0x0000;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_GBP_STUCK_OPEN)       		     ? ERR_GBP_STK     : 0x0000;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_MFV_STUCK_OPEN)       		     ? ERR_MFV_STK     : 0x0000;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_GBP_THERMISTOR_OPEN)            ? ERR_GBP_OPN     : 0x0000;
//  fill_err |= (System_Status[ERROR_FLAG_0] & ERR_MFV_THERMISTOR_OPEN)       	 ? ERR_MFV_OPN     : 0x0000;
  return fill_err;
}
/*
 * *********************************
 * *********************************
 * End of the module: MsgProc.c
 * *********************************
 * *********************************
*/
