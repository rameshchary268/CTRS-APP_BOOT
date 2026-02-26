/* 
 * /////////////////////////////////////////////////////////////////////////////
 * Module: RS485.c
 * Author: Ilensys
 *
 * Created on 11 June, 2022, 10:31 AM
 * 
 * Description:
 * This module contains definitions for RS485 data processing functions as a slave node.
 * This channel is used for communication with RTU server over MODBUS protocol.
 *  
 * Note:
 *  MCU supported: STM32L4P5VGT6
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DisplayTxHandlers.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
#include "RS485.h"

/* Declaration of external global variables */


/* Declaration of global variables */
RS485_INFO RS485_info;
PREV_INFO  Prev_info;
uint8_t    UART3_Tx_Buffer[UART_BUFFER_SIZE+UART_BUFFER_SIZE+20];
uint8_t    UART3_Rx_Buffer[UART_BUFFER_SIZE];
uint8_t    UART3_DMARx_Buffer[UART_BUFFER_SIZE];
//uint8_t    prev_mod_st=0,master_fill=0,prev_seq_fill=0,prev_active_nodes=0;
uint8_t    comd=0,seq_fill = 0;
uint8_t    Seq_shedule=0;
//static int8_t shed_master=1,Group_No=1,Fill_Group=1;

NODE_INFO  node_info[MB_MAX_NODES] = {0, };

/* Declaration of static local variables */
uint32_t poll_flag =0x01;
volatile uint32_t UART3PrevDMACtr = 0;
volatile uint32_t UART3DMACtr = 0;
volatile uint8_t EndOfFrame = 0;
uint8_t Next_group[5][32];
uint8_t Next_node=0;
uint8_t Response_node=0;
NODE_INFO        node_info[];
INFO_4_20mA Info_4_20mA;
uint8_t data_receive=0;
extern GROUPStruct  GroupStruct;

extern CIR_BUF rs485_tx_cir_buf[IOT_CIR_BUF_DEPTH];  // Circular buffer array that holds messages to be transmitted over RS485
extern CIR_BUF rs485_rx_cir_buf[IOT_CIR_BUF_DEPTH];
//DWORD uart_cluster=0;
//ctrs
//CTRS_WR_PARA     ctrs_ws_para;
//CTRS_CAL_VAR     CTRS_cal_var;
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
 * Function name: RS485_init()
 * Created by : Natarajan Kanniappan
 * Date:        01-Jul-2022
 * Description:
 *  This function initializes UART5 (configured as RS485) module, parameters and its state machine
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */

void RS485_init(void)
{

  if ((HAL_UART_RegisterCallback(&huart3, HAL_UART_TX_COMPLETE_CB_ID, Uart3_TxDone_callback) == HAL_OK))
  {
    if (System_Status[ERROR_FLAG_0] & ERR_EEPROM_FAILURE)
      vp_param_rw_str.mb_cfg.node_id = RS485_NODE_ADDRESS;
    else
    {
      if (vp_param_rw_str.mb_cfg.node_id == 0xff)
        vp_param_rw_str.mb_cfg.node_id = RS485_NODE_ADDRESS;
    }
    if(0)//(vp_param_rw_str.cryolink_phase2_sel == MB_SINTESY_MODE )
    {
	  vp_param_rw_str.mb_cfg.mode =vp_param_rw_str.STM_BaudRateCryolink ;
    }
    else
    {
	  vp_param_rw_str.mb_cfg.mode=vp_param_rw_str.STM_BaudRatePhasetwo;
    }
//    if (memcmp((uint8_t*)&mb_cfg, (uint8_t*)&vp_param_rw_str.mb_cfg, sizeof(MB_CFG)) == 0)
//      return;

   // memcpy((uint8_t*)&mb_cfg, (uint8_t*)&vp_param_rw_str.mb_cfg, sizeof(MB_CFG));
    //HAL_UART_ReceiverTimeout_Config(&huart3, RX_TIMEOUT_VAL);  // Required?
    // Set baud rate and other communication parameters:
    //vp_param_rw_str.mb_cfg.mode =2;
    if(RS485_info.Firmware_4_20==0)
    {

    	switch(vp_param_rw_str.mb_cfg.mode)
		{
		  case RS485_MODE_0:
			// Mode0 - Disable
	// 	    __HAL_UART_DISABLE(&huart3);
			  huart3.Init.BaudRate = 115200;
			  huart3.Init.WordLength = UART_WORDLENGTH_8B;
			  huart3.Init.StopBits = UART_STOPBITS_1;
			  huart3.Init.Parity = UART_PARITY_NONE;
		   // return;
			break;

		  case RS485_MODE_1:
			// Mode1-9600B8N1SNP
			huart3.Init.BaudRate = 9600;
			huart3.Init.WordLength = UART_WORDLENGTH_8B;
			huart3.Init.StopBits = UART_STOPBITS_1;
			huart3.Init.Parity = UART_PARITY_NONE;
			break;

		  case RS485_MODE_2:
		  default:
			// Mode2 - 19200B8N1SNP
			huart3.Init.BaudRate = 19200;
			huart3.Init.WordLength = UART_WORDLENGTH_8B;
			huart3.Init.StopBits = UART_STOPBITS_1;
			huart3.Init.Parity = UART_PARITY_NONE;
			break;

		  case RS485_MODE_3:
			// Mode3 - 38400B8N1SNP
			huart3.Init.BaudRate = 38400;
			huart3.Init.WordLength = UART_WORDLENGTH_8B;
			huart3.Init.StopBits = UART_STOPBITS_1;
			huart3.Init.Parity = UART_PARITY_NONE;
			break;
		}
    }
    else
    {
    	  huart3.Init.BaudRate = 115200;
		  huart3.Init.WordLength = UART_WORDLENGTH_8B;
		  huart3.Init.StopBits = UART_STOPBITS_1;
		  huart3.Init.Parity = UART_PARITY_NONE;
    }
    RS485_info.txBuf = &UART3_Tx_Buffer[0];
    RS485_info.rxBuf = &UART3_DMARx_Buffer[0];
    HAL_UART_Init(&huart3);
    RS485_info.resp_flag=1;
	//Start receive DMA
	if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RS485_info.rxBuf, UART_BUFFER_SIZE))
	{
		RS485_info.UART3RxErrorDetected = 1;
	}
  }
}

/* 
 * *****************************************************************************
 * Function name: RS485_master_proc()
 * Created by :Arikanti venkatesh
 * Date:        06-Mar-2023
 * Description:
 *  This is the State Machine to handle various RS485 master related activities
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void RS485_master_proc(void)
{
  static uint8_t i = MB_MAX_NODES, *bfr;
  static int8_t j = 0;
  uint8_t response_j=0;
  uint16_t crc16;
#ifdef EN_CTRS
  Calib_ctrs_proc();
#endif
  vp_param_rw_str.EXD_MD_Switch_settings=1;
 // vp_param_rw_str.mb_cfg.node_id =vp_param_rw_str.STM_DeviceIDCryolink;    // need to check
 // vp_param_rw_str.mb_cfg.node_id =vp_param_rw_str.STM_DeviceIDPhasetwo;
//  if((!vp_param_rw_str.STM_EN_4_20_mA) || ((vp_param_rw_str.mb_cfg.node_id == RS485_MASTER_NODE_ID) && ((vp_rt_wr_str.MBStatus == 0) || (vp_rt_wr_str.MBStatus == 2))))
//  {
//	  System_Status[ERROR_FLAG_0] &= ~ERR_4_20mA_CH1_FAULT;
//	  System_Status[ERROR_FLAG_0] &= ~ERR_4_20mA_CH2_FAULT;
//	  System_Status[WARNING_FLAG] &= ~WRN_4_20mA_FAIL;
//
//  }
//  else if(RS485_info.mA_communication)
//  {
//	  System_Status[WARNING_FLAG] |= WRN_4_20mA_FAIL;
//	  System_Status[ERROR_FLAG_0] &= ~ERR_4_20mA_CH1_FAULT;
//	  System_Status[ERROR_FLAG_0] &= ~ERR_4_20mA_CH2_FAULT;
//  }
//  else
//  {
//	  System_Status[WARNING_FLAG] &= ~WRN_4_20mA_FAIL;
//  }
  if(0)//(vp_param_rw_str.plumbing_options & MB_SINTESY_MODE )
  {
	vp_param_rw_str.mb_cfg.node_id =vp_param_rw_str.STM_DeviceIDCryolink;
	return;
  }
  else
  {
	vp_param_rw_str.mb_cfg.node_id =vp_param_rw_str.STM_DeviceIDPhasetwo;
  }
  vp_param_rw_str.mb_cfg.node_id=0;
  if (vp_param_rw_str.mb_cfg.node_id != RS485_MASTER_NODE_ID)
  {
//	  if(vp_param_rw_str.STM_EN_4_20_mA)
//	  {
//		  Slave_with_polling();
//	  }
    return;
  }
  if((vp_rt_wr_str.MBStatus == 0) || (vp_rt_wr_str.MBStatus == 2))
	  return;
  // As a bus master, this node generates commands, receives responses and manages all the slave nodes
  switch (RS485_info.polling_state)
  {
    case 0:
      // pushing a polling message to a slave node with address at "i" and wait for response
    	//  Prev_info.prev_mod_st ^= UIModbusData.FillMethodPhasetwo;
    	  Prev_info.prev_seq_fill ^=Seq_shedule;
    	if(Prev_info.prev_mod_st ||Prev_info. master_fill || Prev_info.prev_seq_fill)
    	{
	        i = 1;
//	        Group_No=1;
//	        Fill_Group=1;
	        RS485_info.resp_flag=1;
    	}
    	//Prev_info.prev_mod_st=UIModbusData.FillMethodPhasetwo;
    	Prev_info.prev_seq_fill = Seq_shedule;

        if((j = Find_Tx_buf(BUF_EMPTY, COMM_RS485)) >= 0)
		{
        	if(0)//ctrs
        	{
        		//vp_param_rw_str.STM_EN_4_20_mA=0;
        		 bfr = &rs485_tx_cir_buf[j].buffer[0];
				 *(bfr++) = 0xAA;
				 *(bfr++) = 0xA0;
				 *(bfr++) = 0x31;
				  crc16 = usMBCRC16(&rs485_tx_cir_buf[j].buffer[0], 3);//CRC for overall packet
				  *(bfr++) = (uint8_t)(crc16 & 0x00ff);
				  *(bfr) = (uint8_t)((crc16 & 0xff00) >> 8);
				  rs485_tx_cir_buf[j].size = 5;
				  rs485_tx_cir_buf[j].state = BUF_FULL;
				  Set_delay(SW_TMR_RS485_RESP, 8*DLY_RS485_RESP, &RS485_info.resp_flag);
				  RS485_info.polling_state = 1;
        	}
//        	else //if(polling_to_4_20mA(j))
//        	{
//        		//4 20mA firmware update
//            	//4 20mA polling
//        	}
        	//4 20mA polling
			else if(vp_param_rw_str.EXD_MD_Switch_settings)// || (vp_param_rw_str.STM_EN_4_20_mA==0))
			{
				{
					for (; ((i<MB_MAX_NODES) && RS485_info.resp_flag); i++)  // Exclude itself (master)
					{
					  // Check the status of a node by sending a message to this address
						if(0)//vp_param_rw_str.STM_EN_4_20_mA)
						{
							 bfr = &rs485_tx_cir_buf[j].buffer[0];
							 *(bfr++) = 0xAA;
							 *(bfr++) = 0x44;
							 *(bfr++) = 0x31;
							 *(bfr++) = 0x02;
						}
						else
						{
							  bfr = &rs485_tx_cir_buf[j].buffer[0];
						}
					  *(bfr++) = i;
					  *(bfr++) = CMD_POLL;
					  *(bfr++) = vp_param_rw_str.sch_fill_rem.state;
					  if((Seq_shedule)&&(node_info[i].active)&&((node_info[i].fill_comp_sts==0)||(node_info[i].fill_comp_sts==1)))
					  {
						  *(bfr++) = vp_param_rw_str.sch_fill_rem.state;
					  }
					  else
					  {
						  *(bfr++)=0;
					  }
					  if(0)//vp_param_rw_str.STM_EN_4_20_mA)
					  {
						  crc16 = usMBCRC16(&rs485_tx_cir_buf[j].buffer[4], 4);
						  *(bfr++) = (uint8_t)(crc16 & 0x00ff);
						  *(bfr++) = (uint8_t)((crc16 & 0xff00) >> 8);
						  crc16 = usMBCRC16(&rs485_tx_cir_buf[j].buffer[0], 10);
						  *(bfr++) = (uint8_t)(crc16 & 0x00ff);
						  *(bfr) = (uint8_t)((crc16 & 0xff00) >> 8);
						  rs485_tx_cir_buf[j].size = 12;
					  }
					  else
					  {
						  crc16 = usMBCRC16(&rs485_tx_cir_buf[j].buffer[0], 4);
						  *(bfr++) = (uint8_t)(crc16 & 0x00ff);
						  *(bfr) = (uint8_t)((crc16 & 0xff00) >> 8);
						  rs485_tx_cir_buf[j].size = 6;
					  }
					  rs485_tx_cir_buf[j].state = BUF_FULL;
					  Set_delay(SW_TMR_RS485_RESP, 10*DLY_RS485_RESP, &RS485_info.resp_flag);
					  node_info[i].active=0;
					  RS485_info.polling_state = 1;
					}    // End of "for (i=1; ((i<MB_MAX_NODES) && (nodeID[i] == 0) ..."
				}
			}
			else
			{
				 Set_delay(SW_TMR_RS485_RESP, 8*DLY_RS485_RESP, &RS485_info.resp_flag);
				 RS485_info.polling_state = 1;
			}
    	}
    	if(i == MB_MAX_NODES)
		{
			 Set_delay(SW_TMR_RS485_RESP, 8*DLY_RS485_RESP, &RS485_info.resp_flag);
			 RS485_info.polling_state = 1;
		}
	  break;

    case 1:
      // Wait for the response to the polling message
   	  // Check for any messages received at RS485 channel

        i=0;
        if(RS485_info.resp_flag)
		{
			 Set_delay(SW_TMR_RS485_RESP, 8*DLY_RS485_RESP, &RS485_info.resp_flag);
			 RS485_info.polling_state = 0;
		}

   	  break;

    default:
	  // TBD
	  break;
  }
}

/*
 * *****************************************************************************
 * Function name: Master_with_Group
 * Created by :Arikanti venkatesh
 * Date:        14-DEC-2023
 * Description:
 *  This is the State Machine to handle various RS485 master related activities
 * Parameters:
 *  Input  : NODE value
 *  Return : command
 * Note:
 *
 * *****************************************************************************
 */
/*uint8_t Master_with_Group(uint8_t value)
{
	uint8_t i=value;
	 if((vp_param_rw_str.sch_fill_rem.state==1) && (System_Status[WARNING_FLAG] & WRN_LN2_FILL_REMINDER))
	 {
		Seq_shedule=1;
		if (UIModbusData.FillMethodPhasetwo == CONCURRENT_FILL)
		{
		  // OFAF Concurrent mode algorithm need can be enhanced
			if((uint16_t)(DP_info.value*10) <= (vp_param_rw_str.stopfilllvlinch-20))
			{
				StartFillLevelRequest();
			}
			if((node_info[Response_node].fil_sts)&&(node_info[Response_node].errors==0))
			{
				if((GroupStruct.Group_col[Response_node]==Fill_Group))
				{
					comd = CMD_OFAF_FILL_STRT;
					node_info[Response_node].fill_comp_sts=1;
				}
				else
				{
					comd = CMD_OFAF_FILL_STOP;
				}
			}
			else if(node_info[Response_node].fill_comp_sts==1)
			{
			  comd = CMD_OFAF_FILL_STOP;
			  node_info[Response_node].fill_comp_sts=2;
			}
			seq_fill = 0;
		}
		else if(UIModbusData.FillMethodPhasetwo == SEQUENTIAL_FILL)
		{
			if(shed_master)
			{
				StartFillLevelRequest();
				shed_master=0;
			}
			if((!StartFillEvent.StartLevelFillRequest) ||(out_error!=0))
			{
			  if(seq_fill == Response_node)
			  {
				 if ((!node_info[Response_node].fil_sts)||(node_info[Response_node].errors!=0))
				 {
					seq_fill = 0;
					node_info[Response_node].fill_comp_sts=2;
				 }
				 else
				 {
					 comd = CMD_OFAF_FILL_STRT;
				 }
			  }
			  else if(node_info[seq_fill].active==0)
			  {
				  seq_fill = 0;
				  node_info[Response_node].fill_comp_sts=2;
			  }
			  else if(seq_fill != Response_node)
			  {
				  comd = CMD_OFAF_FILL_STOP;
			  }
			  if (seq_fill == 0)  // No filling activity to any of the slaves. Start filling for this slave.
			  {
			  // OFAF Sequential mode algorithm need can be enhanced
				  if(GroupStruct.Group_col[Response_node]==Fill_Group)
				  {
					  if((node_info[Response_node].fil_sts))
					  {
						  if(node_info[Response_node].errors)
						  {
							  seq_fill = 0;
							  node_info[Response_node].fill_comp_sts=2;
						  }
						  else
						  {
							  comd = CMD_OFAF_FILL_STRT;
							  node_info[Response_node].fill_comp_sts=1;
							  seq_fill = Response_node;
						  }
					  }
					  else
					  {
						  comd = CMD_OFAF_FILL_STOP;
						seq_fill = 0;
					  }
				  }
				  else
				  {
					  comd = CMD_OFAF_FILL_STOP;
				  }
			  }
			}
			else
			{
				seq_fill=0;
				comd = CMD_OFAF_FILL_STOP;
			}
		}//sequential fill
	 }//schedule enable
	 else  if(vp_param_rw_str.sch_fill_rem.state==1)
	 {
		//RS485_FILL_process(i);
//		seq_fill=0;
//		comd = CMD_OFAF_FILL_STOP;
		comd=Master_without_schedule_fill_with_group(i);
	 }
	 else if(vp_param_rw_str.sch_fill_rem.state==0)
	 {
		comd=Master_without_schedule_fill_with_group(i);
	 }
	 return comd;
}
*/
/*
 * *****************************************************************************
 * Function name: Master_without_schedule_fill_with_group
 * Created by :Arikanti venkatesh
 * Date:        12-DEC-2023
 * Description:
 *  This is the State Machine to handle various RS485 master related activities
 * Parameters:
 *  Input  : NODE value
 *  Return : command
 * Note:
 *
 * *****************************************************************************
 */
/*uint8_t Master_without_schedule_fill_with_group(uint8_t value)
{
	if (UIModbusData.FillMethodPhasetwo == CONCURRENT_FILL)
	{
	  // OFAF Concurrent mode algorithm need can be enhanced - TBD ???
		if((node_info[Response_node].fil_sts)&&(node_info[Response_node].errors==0))//(node_info[Response_node].level <= node_info[Response_node].str_fil_lvl)
		{
			comd = CMD_OFAF_FILL_STRT;
		}
		else
		{
			comd = CMD_OFAF_FILL_STOP;
		}
		seq_fill = 0;
	}
	else if(UIModbusData.FillMethodPhasetwo == SEQUENTIAL_FILL)
	{
		if((!StartFillEvent.StartLevelFillRequest)||(out_error !=0))
		{
		  if(seq_fill == Response_node)
		  {
			 if ((!node_info[Response_node].fil_sts)||(node_info[Response_node].errors!=0))
			 {
				seq_fill = 0;
				Next_group[GroupStruct.Group_col[Response_node]][Response_node]=0;
				for(int z=1;(z<32 );z++)
				{
					if(Next_group[GroupStruct.Group_col[Response_node]][z])
					{
						seq_fill=z;
						break;
					}
					else
					{
						seq_fill=0;
					}
				}
				if(seq_fill==0)
				{
					for(int z=1;z<5;z++)
					{
						if(seq_fill)
							break;
						for(int p=1;p<MB_MAX_NODES;p++)
						{
							if(Next_group[GroupStruct.Group_col[z]][p])
							{
								seq_fill=p;
								comd = CMD_OFAF_FILL_STRT;
								break;
							}
							else
							{
								//do nothing
							}
						}
					}
				}
			 }
			 else
			 {
				 comd = CMD_OFAF_FILL_STRT;
			 }
		  }
		  else if(node_info[seq_fill].active==0)
		  {
			  seq_fill = 0;
			  Next_group[GroupStruct.Group_col[seq_fill]][seq_fill]=0;
		  }
		  else if(seq_fill != Response_node)
		  {
			  comd = CMD_OFAF_FILL_STOP;
		  }
		  if (seq_fill == 0)  // No filling activity to any of the slaves. Start filling for this slave.
		  {
			  if((node_info[Response_node].fil_sts))
			  {
				  if(node_info[Response_node].errors)
				  {
					  seq_fill = 0;
					  comd = CMD_OFAF_FILL_STOP;
				  }
				  else
				  {
					  comd = CMD_OFAF_FILL_STRT;
					  seq_fill = Response_node;
				  }
			  }
			  else
			  {
				  comd = CMD_OFAF_FILL_STOP;
				  seq_fill = 0;
			  }
		  }
		  else if((node_info[Response_node].fil_sts)&&(node_info[Response_node].errors==0))
		  {
			  Next_group[GroupStruct.Group_col[Response_node]][Response_node]=1;
		  }
		  else
		  {
			  Next_group[GroupStruct.Group_col[Response_node]][Response_node]=0;
		  }
		}
		else
		{
			seq_fill=0;
			comd = CMD_OFAF_FILL_STOP;
		}
	}
	return comd;
}
*/


/*
 * *****************************************************************************
 * Function name: RS485_slave_resp()
 * Created by : Natarajan Kanniappan
 * Date:        14-Mar-2023
 * Description:
 *  This is the response processing routine of RS485 slave node
 *  Slaves respond back with level_info, DP_info, fvt_info & gbpvt_info values to decide by the master
 * Parameters:
 *  Input  : Pointer to buffer, message length
 *  Return : 0-OK, 1-Error
 * Note:
 *
 * *****************************************************************************
 */
/*int8_t RS485_slave_resp(uint8_t *resp, uint16_t len)
{
  uint16_t crc16;

  // Check CRC
  if (len == 0)
	return 1;

  // Message length is non-zero and node id is matching. Start the processing with CRC check
  crc16 = usMBCRC16(resp, len-2);
  if ((*(resp+len-1) != ((crc16 & 0xff00) >> 8)) || (*(resp+len-2) != (crc16 & 0x00ff)))
  {
    // CRC check failed. Discard the response
	return 1;
  }

  // Valid response received as CRC is matching.

#if 0
  uint8_t *bfr = resp;
  float    thr_lvl, dp_lvl;

  if (*bfr++ == vp_param_rw_str.mb_cfg.node_id)
  {
	// Node ID is matching. Get LN2 level values
    thr_lvl = *((float*)bfr);
    bfr += sizeof(float);
    dp_lvl = *((float*)bfr);
    bfr += sizeof(float);

    // Decide on fill stage
    if ((thr_lvl < (FILL_LOW_LIMIT/2)) && (dp_lvl < (FILL_LOW_LIMIT/2)))
    {
      // Ready for a LN2 fill

    }
    else
    {
      // Do nothing
    }
  }
#endif
  return 0;
}
*/

/*
 * *****************************************************************************
 * Function name: RS485_tx_proc()
 * Description:
 *  This is the State Machine to handle various RS485 Transmit related activities
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void RS485_tx_proc(void)
{
	static     int8_t i;
  switch (RS485_info.tx_state)
  {
    case RS485_INIT:
      RS485_init();
      RS485_info.tx_state = RS485_TX_STRT;
      Set_delay(SW_TMR_RS485_DLY,34, &RS485_info.dly_flag);
      break;

    case RS485_TX_STRT:
        // Update the data to be transmitted from "Tx_CirBuf" into "UART3_Tx_Buffer"
        // and size into "UART5_info.tx_size".
        if (((i = Find_Tx_buf(BUF_FULL, COMM_RS485)) >= 0) && (rs485_tx_cir_buf[i].size > 0) && (RS485_info.dly_flag))
        {
          RS485_info.tx_done = 0;
          RS485_info.tx_bytes = 0;
          RS485_info.tx_size = rs485_tx_cir_buf[i].size;
          if(RS485_info.tx_size>256)
          {
        	 // RS485_info.tx_size=256;
          }
          RS485_info.txBuf = &UART3_Tx_Buffer[0];
          RS485_info.rxBuf = &UART3_DMARx_Buffer[0];
          memcpy((char*)&UART3_Tx_Buffer[0], (char*)&rs485_tx_cir_buf[i].buffer[0], RS485_info.tx_size);
          RS485_Dir(RS485_TRANSMIT);
          HAL_Delay(1000);
          Set_delay(SW_TMR_RS485_DLY,34, &RS485_info.dly_flag);
          RS485_info.tx_state = RS485_TX_WAIT;
          if(0)//( RS485_info.tx_size<100)
          {
        	  Set_delay(SW_TMR_RS485_DLY, DLY_RS485_TIMEOUT+2000, &RS485_info.dly_flag);
			  if (HAL_UART_Transmit_IT(&huart3, RS485_info.txBuf, RS485_info.tx_size) == HAL_OK)
			  {
			    // Initiated the transmit process
			    RS485_info.tx_state = RS485_TX;
			    rs485_tx_cir_buf[i].state = BUF_EMPTY;
			  }
			  else
			  {
			  // Error in setting the transmission process.
			    RS485_info.tx_state = RS485_ERR;
			  }
          }
        }
        else
        {
          // Nothing to be transmitted. Keep polling.
   	      RS485_info.tx_state = RS485_TX_STRT;
        }
      break;

    case RS485_TX_WAIT:
    	if(RS485_info.dly_flag)
    	{
    		  Set_delay(SW_TMR_RS485_DLY, DLY_RS485_TIMEOUT+2000, &RS485_info.dly_flag);
			  if (HAL_UART_Transmit_DMA(&huart3, RS485_info.txBuf, RS485_info.tx_size) == HAL_OK)
			  {
				// Initiated the transmit process
				RS485_info.tx_state = RS485_TX;
				rs485_tx_cir_buf[i].state = BUF_EMPTY;
			  }
			  else
			  {
				// Error in setting the transmission process.
				RS485_info.tx_state = RS485_ERR;
			  }
			  HAL_Delay(1000);
    	}
    	break;

    case RS485_TX:
      if (RS485_info.tx_done)
      {
        // Transmission completed
    	  HAL_Delay(1000);
        RS485_Dir(RS485_RECEIVE);
        rs485_tx_cir_buf[i].state = BUF_EMPTY;
        RS485_info.tx_state = RS485_TX_STRT;
        RS485_info.dly_flag=1;
      }
      else if (RS485_info.dly_flag)
      {
        RS485_info.tx_state = RS485_ERR;
      }
      else
      {
        // Do nothing. Just wait for the transaction to complete
      }
      break;

    case RS485_TX_DONE:
   	  RS485_info.tx_state = RS485_TX_STRT;
      break;

    case RS485_ERR:
    default:
      RS485_Dir(RS485_RECEIVE);
      	if(1)
		{
			HAL_UART_Abort(&huart3);

			if(HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RS485_info.rxBuf, UART_BUFFER_SIZE))
			{
				RS485_info.UART3RxErrorDetected = 0;
				UART3PrevDMACtr=0;
				UART3DMACtr=0;
			}
		}
      RS485_info.tx_state = RS485_TX_STRT;
      break;
  }
}

/* 
 * *****************************************************************************
 * Function name: RS485_rx_proc()

 * Description:
 *  This is the State Machine to handle various RS485 Receive related activities.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void RS485_rx_proc(void)
{
  //static uint8_t peer_addr = 0xff;

  int8_t i;  
    
  switch (RS485_info.rx_state)
  {
    case RS485_RX_STRT:
      // Check for any data receive by preparing the process
      //RS485_info.rx_done = 0;
      RS485_info.rx_bytes = 0;
      RS485_info.rx_size = UART_BUFFER_SIZE;      // Maximum allowed
      Set_delay(SW_TMR_RS485_DLY, DLY_RS485_TIMEOUT, &RS485_info.dly_flag);
      RS485_info.rx_state = RS485_RX;
      break;
        
    case RS485_RX:
      if ((RS485_info.rx_done == 1) || (RS485_info.dly_flag == 1))
      {
      	// Message received or timed-out
  		uint16_t RecvedDataSize = 0;
  		RS485_info.rx_done = 0;

  		if(UART3PrevDMACtr < UART3DMACtr)
  		{
  			RecvedDataSize = UART3PrevDMACtr + (UART_BUFFER_SIZE - UART3DMACtr);
  		}
  		else
  		{
  			RecvedDataSize = UART3PrevDMACtr - UART3DMACtr;
  		}

  		if(RecvedDataSize>=4 && EndOfFrame)
  		{
  			EndOfFrame = 0;
  			for(uint8_t Ctr = 0; Ctr < RecvedDataSize; Ctr++)
  			{
  				UART3_Rx_Buffer[Ctr] = UART3_DMARx_Buffer[(UART_BUFFER_SIZE - UART3PrevDMACtr + Ctr)&0xFF];
  			}
  	  		UART3PrevDMACtr = UART3DMACtr;
  	  		RS485_info.rx_bytes = RecvedDataSize;
  		}

  		//RS485_info.rx_bytes  = huart3.RxXferSize - huart3.RxXferCount;
   	    if (RS485_info.rx_bytes > 0)
   	    {
          // Receive completed
          if ((i = Find_Rx_buf(BUF_EMPTY, COMM_RS485)) >= 0)
          {
        	 if(RS485_info.rx_bytes>256)
        	 {
        		 RS485_info.rx_bytes=256;
        	 }
            // Found an empty Rx cir buffer to store the received data. Push the received data from "UART_Rx_Buffer" with size "UART_info.rx_bytes".
            memcpy((char*)&rs485_rx_cir_buf[i].buffer[0], (char*)&UART3_Rx_Buffer[0], RS485_info.rx_bytes);
           // peer_addr = UART3_Rx_Buffer[0];
            rs485_rx_cir_buf[i].size = RS485_info.rx_bytes;
            rs485_rx_cir_buf[i].state = BUF_FULL;
            // Update active node tracker

       	    modbus_timer=0;
            RS485_info.rx_state = RS485_RX_DONE;
          }
          else
          {
            // Circular buffer not available
            RS485_info.rx_state = RS485_ERR;
          }
   	    }
   	    else
   	    {
   	      // Nothing is received
          RS485_info.rx_state = RS485_RX_STRT;
   	    }
      }
      else
      {
        // Do nothing. Just wait
      }
      if(RS485_info.UART3RxErrorDetected)
      {
    	  RS485_info.rx_state = RS485_ERR;
      }
      break;
      
    case RS485_RX_DONE:
      RS485_info.rx_state = RS485_RX_STRT;
      break;

    case RS485_ERR:
    default:
//      RS485_info.tx_state = RS485_TX_STRT;
      RS485_info.rx_state = RS485_RX_DONE;
      if(RS485_info.UART3RxErrorDetected)
      {
		HAL_UART_AbortReceive(&huart3);

		if(HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RS485_info.rxBuf, UART_BUFFER_SIZE))
		{
			RS485_info.UART3RxErrorDetected = 0;
			UART3PrevDMACtr=0;
			UART3DMACtr=0;
		}
      }
      break;
  }
}


void Uart3_TxDone_callback(UART_HandleTypeDef *huart)
{
    RS485_info.tx_done = 1;
    RS485_Dir(RS485_RECEIVE);
}

void NotifyUart5IdleDetection(void)
{
	EndOfFrame = 1;
}


void RS485_Dir(DRV_STATE state)
{
  // PA14 is also used for SWCLK debug signal. Hence cannot be used as GPIO during debug.
  // PD1 (TBD) is used to set the RS485 transceiver either in Tx (DE) or Rx (REn) mode
  switch (state)
  {
    case RS485_RECEIVE:
      HAL_GPIO_WritePin(GPIOD, GPIO_ODR_OD10, GPIO_PIN_RESET);
	  break;

    case RS485_TRANSMIT:
      HAL_GPIO_WritePin(GPIOD, GPIO_ODR_OD10, GPIO_PIN_SET);
	  break;

    default:
	  // Do nothing
	  break;
  }
}



/* 
 * *********************************
 * *********************************
 * End of the module: RS485.c
 * *********************************
 * *********************************
 */

