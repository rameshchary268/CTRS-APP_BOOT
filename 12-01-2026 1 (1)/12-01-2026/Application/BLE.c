/*
 * BLE.c
 *
 *  Created on: Sep 15, 2025
 *      Author: SaikrishnaKarutu
 */

#include <WIFI.h>
#include "main.h"
#include "Delay.h"
#include "BLE.h"
#include "string.h"
#include "AT_CMD.h"

extern uint8_t   wifi_rx_bfr[10*WIFI_MTU];
extern uint8_t   wifi_tx_bfr[10*WIFI_MTU];
extern uint8_t   wifi_rx_copy[10 * WIFI_MTU];
extern uint16_t read_size;
extern uint8_t flag;
uint16_t j =0;
extern uint8_t wifi_uart_rx_enable;
uint8_t retry_count = 0;
BLE_INFO  BLE_info;
BLE_SSID_Details BLE_ssid[21];

void BLE_init(void)
{
  // Configurations for up to a maximum of 10 config IDs set
  BLE_info = BLE_INFO_DEFAULT;
  huart1.TxCpltCallback = HAL_UART_TxCpltCallback;
  huart1.RxEventCallback = HAL_UARTEx_RxEventCallback;
  huart1.ErrorCallback = HAL_UART_ErrorCallback;
  memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
}

void BLE_Process()
{
	switch (BLE_info.state)
	{
	//static int32_t i = 0;
	 case BLE_RESET:
	 {
	  if ((flag == 0) && (wifi_info.dly_flag == 1))
	  {
		read_size = 0;
		flag = 1;
		BLE_info.TxCplt = 0;
		BLE_info.RxCplt = 0;
		memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);
		memset(wifi_rx_copy, 0x00, 10 * WIFI_MTU);
	  }
	  else if (flag == 1)
	  {
	   if (Check_CTS() == 0)
	   {
		flag = 2;
		Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &BLE_info.dly_flag);
	   }
	 }
	  else if ((flag == 2) && (BLE_info.dly_flag))
	  {
		flag = 3;
		strcpy((char*) &wifi_tx_bfr[0], AT_CMD_RESET);
		uart1_info.tx_size = strlen((char*) &wifi_tx_bfr[0]);
		HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
	  }
	  else if ((flag == 3) && (wifi_info.TxCplt == 1))
	  {
		  Assert_RTS();
		uart1_info.rx_size = 100;
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
		Set_delay(SW_TMR_WIFI_DLY, 3000, &BLE_info.dly_flag);
		flag = 4;
	  }
	  else if ((BLE_info.dly_flag == 1) && (flag == 4))
	  {
		  Deassert_RTS();
		if (strstr((char*) wifi_rx_copy, "+STARTUP") != NULL)
		{
		  wifi_info.state = BLE_STRT;
		 flag = 0;
		}
		else
		{
		  wifi_info.state = BLE_RESET;
		  flag = 0;
		 }
		}
	 else
		{
		}
	 }
  	  break;

	case BLE_INIT:
	{
		if (flag == 0)
		{
			flag = 1;
			Set_delay(SW_TMR_WIFI_DLY, 10, &BLE_info.dly_flag);
		}
		else if ((BLE_info.dly_flag) && (flag == 1))
		{
			flag = 2;
			BLE_info.RxCplt = 0;
			//i = 0;
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			flag = 0;
			BLE_info.state = BLE_STRT;
		} else
		{
			//do nothing.
		}
	}
	break;

	case BLE_STRT:
	{
		if ((flag == 0) &&(BLE_info.dly_flag))
		{
			flag = 1;
		    //Set_delay(SW_TMR_WIFI_DLY, 1, &BLE_info.dly_flag);
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		  {
		    BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		  }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			wifi_uart_rx_enable = 1;
			memset(&wifi_rx_copy[0], 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_BLE_MODE);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_BLE_MODE);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &BLE_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &BLE_info.dly_flag);
			flag = 4;
		}
		else if((flag ==4) && (BLE_info.dly_flag))
		{
			Deassert_RTS();
			if(strstr((char*) wifi_rx_copy, "+UBTM:3") != NULL)
		{
			flag = 0;
			BLE_info.state = BLE_CHECK_NAME;
		}
		else
		{
			BLE_info.state = BLE_SET_MODE;
		   flag =0;
		}
	}
	}
		break;

    case BLE_SET_MODE:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		 {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		 }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_BLE_SET_MODE);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_BLE_SET_MODE);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &BLE_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &BLE_info.dly_flag);
			flag = 4;
		}
		else if((flag ==4) && (BLE_info.dly_flag))
	    {
			Deassert_RTS();
		  if(strstr((char*) wifi_rx_copy, "OK") != NULL)
		  {
			BLE_info.state = BLE_STRT;
			flag =0;
		  }
	    }
		else
		{
			BLE_info.state = BLE_SET_MODE;
			flag =0;
		}
    }
    break;

    case BLE_CHECK_NAME:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
		  if (Check_CTS() == 0)
		  {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		  }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_CHECK_NAME);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_CHECK_NAME);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &BLE_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &BLE_info.dly_flag);
			flag = 4;
		}
		else if ((flag == 4) && (BLE_info.dly_flag))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+UBTLN:") != NULL)
			{
				flag = 0;
				BLE_info.state = BLE_SCAN;
			}
			else
			{
				BLE_info.state = BLE_RESET;
				flag = 0;
			}
		}
    }
    break;

    case BLE_SCAN:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		} else if (flag == 1)
		{
		if (Check_CTS() == 0)
		 {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		 }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_BLE_SCAN);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_BLE_SCAN);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &BLE_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 20000;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &BLE_info.dly_flag);
			flag = 4;
		}
		else if ((flag == 4) && (BLE_info.dly_flag))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "OK") != NULL)
			{
				flag = 0;
				BLE_info.state = BLE_CONNECT;
			}
			else
			{
				BLE_info.state = BLE_SCAN;
				flag = 0;
			}
		}
    }
    break;

    case BLE_CONNECT:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		  {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		  }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_BLE_CONNECT);
			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_BLE_CONNECT);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.RxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 5000, &BLE_info.dly_flag);
			flag = 4;
		}
		else if ((flag == 4) && (BLE_info.dly_flag == 1))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+UEBTC:") != NULL)
			{
				flag = 0;
				retry_count = 0;
				BLE_info.state = BLE_CONNECTED;
			}
			else
			{
				retry_count++;
				if(retry_count >= 4)
				{
					flag =0;
					retry_count = 0;
				  wifi_info.state = WIFI_STRT;
				  BLE_info.state = BLE_IDLE;
				}
				else
				{
				  BLE_info.state = BLE_CONNECT;
				  flag = 0;
				}
			}
		}
	}
    break;

    case BLE_CONNECTED:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
	     if (Check_CTS() == 0)
		  {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		  }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_BLE_CONNECTED);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_BLE_CONNECTED);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			//Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &BLE_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.RxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
			//Set_delay(SW_TMR_WIFI_DLY, 1000, &BLE_info.dly_flag);
			flag = 4;
		}
		else if ((flag == 4) && (BLE_info.dly_flag == 1))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+UBTGPSD:") != NULL)
			{
				flag = 0;
				BLE_info.state = BLE_CHAR_DISCOVER;
			} else
			{
				BLE_info.state = BLE_CONNECTED;
				flag = 0;
			}
		}
    }
    break;

    case BLE_CHAR_DISCOVER:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		 {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		 }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_CHAR_DISCOVER);
			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_CHAR_DISCOVER);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			//Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &BLE_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &BLE_info.dly_flag);
			flag = 4;
		} else if ((flag == 4) && (BLE_info.dly_flag))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+UBTGSCD:") != NULL)
			{
				flag = 0;
				BLE_info.state = BLE_CONNECTION_DETAILS;
			} else {
				BLE_info.state = BLE_CHAR_DISCOVER;
				flag = 0;
			}
		}
    }
    break;

    case BLE_CONNECTION_DETAILS:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		 {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		 }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_BLE_CONN_DETAILS);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_BLE_CONN_DETAILS);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;   // Go wait for Tx complete
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 500, &BLE_info.dly_flag);
			flag = 4;
		} else if ((flag == 4) && (BLE_info.dly_flag))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+UBTCL:") != NULL)
			{
				flag = 0;
				BLE_info.state = BLE_READ_DATA;
			}
			else
			{
				BLE_info.state = BLE_CONNECTION_DETAILS;
				flag = 0;
			}
		}
	}
    break;

    case BLE_READ_DATA:
    {
		if ((flag == 0) && (BLE_info.dly_flag))
		{
			flag = 1;
		}
		else if (flag == 1)
		{
		if (Check_CTS() == 0)
		 {
			BLE_info.TxCplt = 0;
			BLE_info.RxCplt = 0;
			flag = 2;
		 }
		}
		else if ((flag == 2) && (BLE_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_READ_DATA);

			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_READ_DATA);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (BLE_info.dly_flag))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &BLE_info.dly_flag);
			flag = 4;
		}
		else if ((flag == 4) && (BLE_info.dly_flag))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+UBTGR:") != NULL)
			{
				flag = 0;
				BLE_info.state = BLE_DATA_READ_SUCCESS;
			} else {
				BLE_info.state = BLE_READ_DATA;
				flag = 0;
			}
		}
	}
	break;

    case BLE_DATA_READ_SUCCESS:
    {
      wifi_info.state = WIFI_STRT;
      BLE_info.state = BLE_IDLE;
    }
    break;

    case BLE_IDLE:
    {
//        if ((flag == 0) && (wifi_info.dly_flag))
//       {
//            flag = 1;
//        }
//        else if (flag == 1)
//        {
//            if (BLE_info.state == BLE_DATA_READ_SUCCESS)
//            {
//                // Reset retry counters
//                i = 0;
//                j = 0;
//                flag = 0;
//                wifi_info.state = WIFI_LOGIN;
//            }
//        }
    }
    break;

	default:
	{

	}
	break;
	}

}
