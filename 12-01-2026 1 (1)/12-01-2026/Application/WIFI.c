/*
 * WIFI.c
 *
 *  Created on: Aug 25, 2025
 *      Author: SaikrishnaKarutu
 */
#include "MyMain.h"
#include "stdbool.h"
#include "Xmodem.h"
#include <app_filex.h>
#include <fx_api.h>
#include <stdio.h>
#include "fx_stm32_mmc_driver.h"
#include "DisplayCmdHandlers.h"
#include "ErrorLog.h"
#include "LevelMon.h"
#include "UI.h"

uint8_t   wifi_tx_bfr1[3*WIFI_MTU] = {0};
uint8_t   wifi_tx_bfr[3*WIFI_MTU] = {0};//max 3times
uint8_t   wifi_rx_bfr[10*WIFI_MTU] = {0};
uint8_t   wifi_rx_copy[10 * WIFI_MTU] = {0};
uint8_t   wifi_data_bfr[10*WIFI_MTU] = {0};
uint8_t start_file_transfer=0;
uint8_t end_of_file=0;
uint8_t byte_sent_success=1;
uint8_t byte_resend=1;
uint8_t flag = 0;
uint8_t change_topic_3=0;
int rssi_value=0;
char rssi_val[10];
char wifi_urc_code[100]={0};
uint16_t read_size=0;
uint8_t send_data=0;
uint8_t  cert_upload_done=0;
uint8_t wifi_count = 0;
bool Rx_flag = 0;
uint32_t h=0;
uint8_t check =0;
uint16_t wifi_rx_size =0;
uint32_t t=0,g=0,r=0,u=0;
uint8_t bad_rssi_value=0;
uint8_t match_found=0;
uint16_t xmodem_packet_number = 1u;
uint8_t CRC_OK=0;
uint16_t wifi_urc_len=0;
uint8_t wifi_interface_id=0;
uint8_t   wifi_connected=0;
uint8_t Response_Time=0;
uint8_t transmit_to_iot=1;
uint16_t wifiMsgBuff[100] = {0};
uint16_t wifiMsgBuff1[1000] = {0};
uint8_t send_test_mail=0;
uint8_t wifi_hour;
extern uint8_t New_error_ind;
IOT_INFO    iot_info;

extern uint8_t retry_count;
uint8_t wifi_firmware_available =0;
uint8_t wifi_uart_rx_enable = 0;
uint8_t read_buffer[1800];
static uint16_t j=0;
extern BLE_INFO  BLE_info;
extern UART_HandleTypeDef huart1;

WIFI_INFO  wifi_info;
UART_INFO  uart1_info;
SSID_Details Wifi_ssid[11];
XMODEM_INFO xmodem_info;
ATTEMPTS_INFO attempts_info;
VERSIONS Control_versions;
extern LOG_DATA_BUF Log_data_buff;
uint8_t link_disconnect,peer_disconnect;
extern LN2_USAGE   Ln2_usage;
//extern VP_RT_WR_STR     vp_rt_wr_str;

/***************  EMMC Related ***********/
EMMC_INFO EMMC_info = EMMC_INFO_DEFAULT;
#define READ_CHUNK 1024
//UCHAR read_buffer[READ_CHUNK];
extern FX_MEDIA   mmc_disk;
FX_FILE fx_EMMC_file;
UINT status;
UINT mmc_status = FX_SUCCESS;
extern ULONG bytes_read;
extern ULONG file_size;
ULONG total_read = 0;

// Example reference buffer (if you want to compare against something)
UCHAR data[1024] = { /* some known data */ };


/* Default sector size, used by the driver */
#define FX_STM32_MMC_DEFAULT_SECTOR_SIZE                        512

extern ALIGN_32BYTES (uint32_t fx_mmc_mmc_disk_memory[FX_STM32_MMC_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);


SSID_REGISTERS SSID_address[]={
		{WIFI_SCAN1_WR,WIFI_SIGNAL1_WR},
		{WIFI_SCAN2_WR,WIFI_SIGNAL2_WR},
		{WIFI_SCAN3_WR,WIFI_SIGNAL3_WR},
		{WIFI_SCAN4_WR,WIFI_SIGNAL4_WR},
		{WIFI_SCAN5_WR,WIFI_SIGNAL5_WR},
		{WIFI_SCAN6_WR,WIFI_SIGNAL6_WR},
		{WIFI_SCAN7_WR,WIFI_SIGNAL7_WR},
		{WIFI_SCAN8_WR,WIFI_SIGNAL8_WR},
		{WIFI_SCAN9_WR,WIFI_SIGNAL9_WR},
		{WIFI_SCAN10_WR,WIFI_SIGNAL10_WR},
};

char  *WiFi_Init_Seq[] =
{
   AT_CMD_WIFI_DISCONNECT,
   AT_CMD_STA_USERID,
   AT_CMD_STA_PASSWD,
  // AT_CMD_STA_IP_STAT,
};

char  *WIFI_UPLOAD_CERT[]=
{
  AT_UPLOAD_RCA_CERT ,
  AT_UPLOAD_DEVICE_CERT,
  AT_UPLOAD_PRIVATE_KEY,
};

char *MQTT_SUB_READ_SEQ[]=
{
	AT_MQTT_SUBSCRIBE,
	AT_READ_MESSAGE,
};

char *EventTYPE[]=
{
	"N",
	"N",
	"W",
	"E",
};
char *Delete_Cert_Seq[]=
{
	AT_DELETE_CA_CERT,
	AT_DELETE_DEVICE_CERT,
	AT_DELETE_PRIVATE_CERT,
};

char *MQTT_Init_Seq[]=
{
		AT_MQTT_DISCONNECT,
		AT_CERT_CHECK,
		AT_TLS_CONFIG,
		AT_KEEP_ALIVE,
	//	AT_TLS_CONNECTION,
//		AT_MQTT_CONNECT,
};
char *Wifi_enter_at_seq[]=
{
	AT_ENTER_CMD_MODE,
	AT_CMD_RESET,
};

uint8_t Signal_icon(uint32_t signal, uint8_t authenication_type)
{
	uint8_t signal_value=0;
	if(authenication_type == 0)
	{
		if(signal<=55)
		{
			signal_value=4;
		}
		else if(signal<=60)
		{
			signal_value=3;
		}
		else if(signal<=65)
		{
			signal_value=2;
		}
		else if(signal<=75)
		{
			signal_value=1;
		}
		else if(signal<=90)
		{
			signal_value=0;
		}
	}
	else
	{
		if(signal<=55)
		{
			signal_value=9;
		}
		else if(signal<=60)
		{
			signal_value=8;
		}
		else if(signal<=65)
		{
			signal_value=7;
		}
		else if(signal<=75)
		{
			signal_value=6;
		}
		else if(signal<=90)
		{
			signal_value=5;
		}
	}
	return signal_value;
}

void Wifi_init(void)
{
  // Configurations for up to a maximum of 10 config IDs set
  wifi_info = WIFI_INFO_DEFAULT;
  huart1.TxCpltCallback = HAL_UART_TxCpltCallback;
  huart1.RxEventCallback = HAL_UARTEx_RxEventCallback;
  huart1.ErrorCallback = HAL_UART_ErrorCallback;
  memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
//  memcpy(Plumbing_options.IOT_serial,"c356e63d-a11f-410e-860b-a04bc3e78f81",sizeof("c356e63d-a11f-410e-860b-a04bc3e78f81"));
//  memcpy((char*)comm_par.sy_par.iot.iot_url, (char*)"a1ymfma3vzmew7-ats.iot.ap-south-1.amazonaws.com", sizeof("a1ymfma3vzmew7-ats.iot.ap-south-1.amazonaws.com"));
//  memcpy((char*)comm_par.sy_par.iot.User_ID, "balasaikrishna099@gmail.com", sizeof("balasaikrishna099@gmail.com"));
//  memcpy((char*)comm_par.sy_par.iot.iot_email, "balasaikrishna099@gmail.com", sizeof("balasaikrishna099@gmail.com"));
//  memcpy((char*)Plumbing_options.Iot_token, "2b7d28f8-a402-4449-acaf-692a2a211b69", sizeof("2b7d28f8-a402-4449-acaf-692a2a211b69"));
//  memcpy((char*)Plumbing_options.IOT_serial, "c356e63d-a11f-410e-860b-a04bc3e78f81", sizeof("c356e63d-a11f-410e-860b-a04bc3e78f81"));

}

uint8_t Check_CTS(void)
{
  return (((huart1.Instance->ISR & USART_ISR_CTS) != 0) ? 0:1);
}

void Assert_RTS(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
}

void Deassert_RTS(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}

 void WIFI_Process(void)
{
	static int32_t i = 0;
	if(vp_rt_wr_str.WiFiStatus !=2 && iot_info.logs_ready && Init_Wait_Delay_for_UI_flg && UICommFaildCounter<5)
	{
	  if(UIwifi.IOT_connect_sts)
	  {
	     update_device_logs();
	  }
//	  else if(iot_info.file_open)
//	  {
//			 	f_close(&IOT_SD_File);
//			 	iot_info.file_open=0;
//	//		 	file_ptr=IOT_SD_File.fptr;
//		  }
	  }
	  if(Init_Wait_Delay_for_UI_flg)
	  {
	     Icon_status();
	  }

	switch (wifi_info.state)
	{
	//////////////////////////////////
	// One-time initialization process
	//////////////////////////////////
	case WIFI_STRT:
	{
	    if (flag == 0)
	    {
	        Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &wifi_info.dly_flag);
	        flag = 1;   // mark that we have set the delay
	    }
	    else if ((wifi_info.dly_flag) && (flag == 1))
	    {
	     if (Check_CTS() == 0)
	      {
	        flag = 2;
	        wifi_info.RxCplt = 0;
	        i = 0;
	      }
	     else
	     {
	    	 wifi_info.state=WIFI_COMM_FAIL;
	     }
	    }
	  else if(flag == 2)
	  {
		flag=0;
		wifi_info.state=WIFI_RESET;
		Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &wifi_info.dly_flag);
	 }
	else
	 {
		//do nothing.
	 }
	}
	break;

	case WIFI_RESET:
	{
	  if(vp_rt_wr_str.WiFiStatus)
	  {
	   SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*)"\0");
	  }
	  vp_rt_wr_str.WiFiStatus=0;
	  if ((flag == 0) && (wifi_info.dly_flag == 1))
	  {
	    read_size = 0;
	    flag = 1;
		wifi_info.TxCplt = 0;
		wifi_info.RxCplt = 0;
		memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);
		memset(wifi_rx_copy, 0x00, 10 * WIFI_MTU);
	   }
	  else if (flag == 1)
	  {
	   if (Check_CTS() == 0)
		{
		  flag = 2;
		  Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
		}
	   else
	   {
		   wifi_info.state = WIFI_COMM_FAIL;
	   }
	  }
	  else if ((flag == 2) && (wifi_info.dly_flag))
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
		  Set_delay(SW_TMR_WIFI_DLY, 3000, &wifi_info.dly_flag);
		  flag = 4;
	   }
	  else if ((wifi_info.dly_flag) && (flag == 4))
		{
		   Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+STARTUP") != NULL)
			{
				wifi_info.state = WIFI_INIT;
				flag = 0;
			}
			else
			{
				wifi_info.state = WIFI_RESET;
				flag = 0;
			}
		}
	  else
		{
		}
	}
	break;

	case WIFI_INIT:
	{
		if (flag ==0)
		{
			uart1_info.rx_size =15;
			memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
			strcpy((char*) &wifi_tx_bfr[0], "AT\r\n");
			uart1_info.tx_size=strlen((char*)&wifi_tx_bfr[0]);
			wifi_info.RxCplt = 0;
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST, &wifi_info.dly_flag);
			Assert_RTS();
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
			flag =1;
		}
		else if (((wifi_info.dly_flag)||(wifi_info.RxCplt)) && (flag == 1))
		{
			Deassert_RTS();
			if(strstr((char*) wifi_rx_copy, "OK") != NULL)
			{
				  flag = 0;
			      wifi_info.state = WIFI_CHECK_VERSION;
			      i = 0;
			}
			else
			{
			  if (i++ >= 5)  // Retry up to 5 times
			  {
				 wifi_info.state = WIFI_INIT;  // Give up and go to init
				 flag = 0;
				 i = 0;
		      }
			 else
		      {
				 // Retry sending AT+GMR
				 flag = 0;
				 wifi_info.TxCplt = 0;
				 wifi_info.RxCplt = 0;
				 memset(wifi_rx_bfr, 0x00, sizeof(wifi_rx_bfr));
				 Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
			  }
			}
		}
	    else
	    {
		}
	}
	    break;

	case CHECK_CHIP_UPDATE:
	{
	  if (flag == 0)
	  {
		read_size = 0;
		flag = 1;
		wifi_info.TxCplt = 0;
		wifi_info.RxCplt = 0;
		memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);
		memset(wifi_rx_copy, 0x00, 10 * WIFI_MTU);
	  }
	  else if (flag == 1)
	  {
		if (Check_CTS() == 0)
		{
		  flag = 2;

		}
//		status = fx_file_open(&mmc_disk, &fx_EMMC_file, "NORA-3.0.bin", FX_OPEN_FOR_READ);
//		if (status == FX_SUCCESS)
//		{
//			wifi_firmware_available =1;
//		}
//		 status = fx_file_close(&fx_EMMC_file);
		}
	  else if(flag ==2)
	  {
		  wifi_info.state = WIFI_CHIP_UPDATE;
		  flag = 0;
	  }
	  else
	  {
		 wifi_info.state = CHECK_CERT_AVAILABILITY;
		 flag = 0;
	  }
	}
	break;
/*
	case WIFI_CHIP_UPDATE:
		{
			//HAL_UART_Abort(&huart1);
		if (flag == 0)
		{
			flag = 1;
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0x00, 3 * WIFI_MTU);
		}
		else if (flag == 1)
		{
			if (Check_CTS() == 0)
			{
				flag = 2;
				read_size = 0;
			}
			strcpy((char*) &wifi_tx_bfr[0], AT_CMD_FIRMWARE_UPDATE);
			uart1_info.tx_size = strlen((char*) &wifi_tx_bfr[0]);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
		}
		else if ((flag == 2) && (wifi_info.dly_flag == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.RxCplt == 1))
		{
			 Deassert_RTS();
			if ((strstr((char*) wifi_rx_copy, "CC") != NULL))
			{
				wifi_info.state = WIFI_BOOT_MODE;
				flag = 0;
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
			}
			else
			{
				wifi_info.state = WIFI_CHIP_UPDATE;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
				flag = 0;
			}
		}
	}
		break;

		case WIFI_BOOT_MODE:
		{
		  read_bin_file();
		  if (!start_file_transfer)
		  {
			start_file_transfer = 1;
			i = 0;
			memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
			xmodem_packet_number = 1;
		  }
		  else
		  {
			if (xmodem_info.bin_present == 1)
			{
			  if (!end_of_file)
			  {
				if (((byte_sent_success == 1)|| ((byte_resend == 1) && (wifi_info.dly_flag)))&& (send_data == 1))
				{
				   byte_sent_success = 0;
				   wifi_info.dly_flag = 0;
				   byte_resend = 0;
				   read_size = 0;
				   build_xmodem_packet_1K((uint8_t*) xmodem_info.buffer,bytes_read, xmodem_packet_number, wifi_tx_bfr);
				   if (CRC_OK == 1)
				   {
					 if (Check_CTS() == 0)
					 {
						uart1_info.tx_size = 1029; // Full packet size (SOH + packet number + complement + 128 payload + 2 CRC)
						memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
						HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0],uart1_info.tx_size);
						Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
						flag = 1;
					  }
					}
				}
				if ((wifi_info.dly_flag == 1) && (flag == 1))
				{
				   wifi_info.TxCplt = 0;
				   uart1_info.rx_size = 1100;
				   Set_delay(SW_TMR_WIFI_DLY, 1, &wifi_info.dly_flag);
					Assert_RTS();
				}
				else if ((wifi_info.dly_flag))
				{
				  HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
				  for (int i = 0; i < (uart1_info.rx_size); i++)
				  {
				    if (wifi_rx_bfr[i] == X_NAK)
				    {
					  byte_resend = 1;
					  Set_delay(SW_TMR_WIFI_DLY, 100,&wifi_info.dly_flag);
					  break;
					}
				    else if (wifi_rx_bfr[i] == X_ACK)
				    {
					  byte_sent_success = 1;
					  xmodem_info.file_size -= 1024;
					  byte_resend = 0;
					  xmodem_packet_number++;
					  Deassert_RTS();
					  flag = 0;
					 if (xmodem_info.file_size == 0)
					 {
						end_of_file = 1;
					 }
					break;
				  }
				}
			 }
		   }
		   else if ((wifi_info.dly_flag) && (end_of_file == 1))
			{
			  if (flag == 0)
			  {
				flag = 1;
				memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
				uart1_info.tx_size = 1;
				wifi_tx_bfr[0] = X_EOT;
				read_size = 0;
				HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			  }
			  else if ((flag == 1) && (wifi_info.TxCplt == 1))
			  {
				 flag = 2;
				 wifi_info.TxCplt = 0;
				 memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
				 uart1_info.rx_size = 1535;
				 Set_delay(SW_TMR_WIFI_DLY, 2000, &wifi_info.dly_flag);
			  }
			  else if (((wifi_info.dly_flag)) && (flag == 2))
			  {
				 flag =3;
				 Assert_RTS();
				 HAL_UART_Receive_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
				 Set_delay(SW_TMR_WIFI_DLY, 4000, &wifi_info.dly_flag);
			  }
			  else if((wifi_info.dly_flag) && (flag == 3))
		      {
			   if ((strstr((char*) &wifi_rx_bfr, "+STARTUP") != 0))
				{
				  match_found = 1;
				  wifi_info.state = WIFI_CHECK_VERSION;
				  flag = 0;
				  j = 0;
				  i = 0;
				}
				else
			    {
				  wifi_info.state = WIFI_CHIP_UPDATE;
				  flag =0;
				}
			  }
			}
		  }
		 }
	  }
  break;
*/
	case CHECK_CERT_AVAILABILITY:
		{
	      if (wifi_info.Wifi_cert_available == 1)
	      {
		     wifi_info.state = DELETE_CERT;   // directly go to delete state
			 flag = 0;
			 break;
		  }
		if ((flag == 0) && (wifi_info.dly_flag == 1))
		{
			read_size=0;
			flag = 1;
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);
			memset(wifi_rx_copy, 0x00, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0x00, 3 * WIFI_MTU);
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		   {
			flag = 2;
			Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
		   }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			flag = 3;
			read_size = 0;
			strcpy((char*) &wifi_tx_bfr[0], AT_CERT_CHECK);
			uart1_info.tx_size = strlen((char*) &wifi_tx_bfr[0]);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			//Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag =4;
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "+USECL:2") != NULL)
			{
				wifi_info.state = MQTT_CONNECTION;
				flag = 0;
			} else
			{
			  i++;
		      if (i >= 5)
			  {
			   wifi_info.state = WIFI_READ_AWS_CERT;
			   flag = 0;
			   i = 0;
			  }
			  else
				{
				  flag = 2;
				}
			 }
		    }
		    else
			{
			}
		}
		break;

	case DELETE_CERT:
		{
		if ((wifi_info.dly_flag) && (flag == 0))
		{
			flag = 1;
			i = 0;
			h = 0;
			j = 0;
		}
		else if (flag == 1)
		{
			if (Check_CTS() == 0)
			{
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
				match_found = 0;
				flag = 2;
			}
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) Delete_Cert_Seq[j]);
			strcpy((char*) &wifi_tx_bfr[5], Delete_Cert_Seq[j]);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			Set_delay(SW_TMR_WIFI_DLY, 10, &wifi_info.dly_flag);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.dly_flag == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 10 * WIFI_MTU;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag = 4;
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
			if ((strstr((char*) wifi_rx_copy, "OK") != NULL) || (strstr((char*) wifi_rx_copy, "ERROR") != NULL))
			{
				j++;
				i = 0;
				flag = 1;
			}
			if (j >= 3)
			{
				wifi_info.state = CHECK_CERT_AVAILABILITY;
				wifi_info.Wifi_cert_available =0;
				j = 0;
				flag = 0;
				i = 0;
				break;
			}
			i++;
			if (i >= 5)
			{
				wifi_info.state = DELETE_CERT;
				flag = 0;
				i = 0;
				j = 0;
			}
			else
			{
				//flag = 0;
			}
		  }
		}
      break;


	case WIFI_READ_AWS_CERT:
	{
		if(FirmwareUpdateModeOn==0)
		{
		 if (cert_upload_done == 0)
		 {
		  if((flag ==0) && (wifi_info.dly_flag == 1))
		  {
			read_size = 0;
			flag = 1;
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);
			memset(wifi_rx_copy, 0x00, 10 * WIFI_MTU);
		 }
		 else if (flag == 1)
		 {
		  if (Check_CTS() == 0)
			{
			  flag = 2;
			  Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
			}
		 }
		 else if ((flag == 2) && (wifi_info.dly_flag))
		 {
		    flag = 3;

//		    FX_MEDIA *media_ptr = &mmc_disk;
//		    if (media_ptr != UX_NULL)
//		    {
		    tx_semaphore_get(&emmc_semaphore, TX_WAIT_FOREVER);
			status = fx_file_open(&mmc_disk, &fx_EMMC_file, "RCA.pem", FX_OPEN_FOR_READ);
			if (status == FX_SUCCESS)
			{
			  EMMC_info.file_size = fx_EMMC_file.fx_file_current_file_size;

			  status = fx_file_read(&fx_EMMC_file, read_buffer, sizeof(read_buffer), &bytes_read);
			  if ((status != FX_SUCCESS) && (status != FX_END_OF_FILE))
				Error_Handler();

			  uint16_t total_length = 0;
			  total_length = snprintf((char*) wifi_tx_bfr,sizeof(wifi_tx_bfr), "%s", AT_UPLOAD_RCA_CERT);
			  memcpy(&wifi_tx_bfr[total_length], read_buffer, bytes_read);
			  total_length += bytes_read;
			  HAL_UART_Transmit_IT(&huart1, (uint8_t*) wifi_tx_bfr, total_length);
			  Set_delay(SW_TMR_WIFI_DLY, 10, &wifi_info.dly_flag);
			  status = fx_file_close(&fx_EMMC_file);
			  //status = fx_file_delete(&mmc_disk, "RCA.pem");
		 	}
	        tx_semaphore_put(&emmc_semaphore);

	     // }
		 }
		 else if ((flag == 3) && (wifi_info.dly_flag == 1))
		 {
			Assert_RTS();
			uart1_info.rx_size = 2000;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag = 4;
		  }
		 else if ((wifi_info.dly_flag) && (flag == 4))
		 {
			 Deassert_RTS();
			if ((strstr((char*) wifi_rx_copy, "OK") != NULL))
			{
			   wifi_info.state = WIFI_CERT_UPLOAD_DONE;
			   flag = 0;
			   retry_count = 0;
			 }
			else
			{
			  if (++retry_count >= 5)
			  {
				wifi_info.state = WIFI_READ_AWS_CERT; // or WIFI_INIT
				flag = 0;
				retry_count = 0;
			  }
			  else
			  {
				// Retry sending RCA.pem
				flag = 2;
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
				memset(wifi_rx_bfr, 0x00, sizeof(wifi_rx_bfr));
			    Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
			    cert_upload_done = 0;
			  }
		    }
		 }
		}
	 else if (cert_upload_done == 1)
	 {
		if ((flag == 0) && (wifi_info.dly_flag == 1))
		{
		  read_size = 0;
		  flag = 1;
		  wifi_info.TxCplt = 0;
		  wifi_info.RxCplt = 0;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		  {
		    flag = 2;
		    Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
		  }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
		  flag = 3;

//		  FX_MEDIA *media_ptr = &mmc_disk;
//		  if (media_ptr != UX_NULL)
//		  {
	        tx_semaphore_get(&emmc_semaphore, TX_WAIT_FOREVER);
		  status = fx_file_open(&mmc_disk, &fx_EMMC_file, "Device.pem", FX_OPEN_FOR_READ);
		  if (status == FX_SUCCESS)
		  {
			EMMC_info.file_size = fx_EMMC_file.fx_file_current_file_size;

			status = fx_file_read(&fx_EMMC_file, read_buffer,sizeof(read_buffer), &bytes_read);
			if ((status != FX_SUCCESS) && (status != FX_END_OF_FILE))
			  Error_Handler();

			uint16_t total_length = 0;
			total_length = snprintf((char*) wifi_tx_bfr,sizeof(wifi_tx_bfr), "%s", AT_UPLOAD_DEVICE_CERT);
			memcpy(&wifi_tx_bfr[total_length], read_buffer, bytes_read);
			total_length += bytes_read;
			HAL_UART_Transmit_IT(&huart1, (uint8_t*) wifi_tx_bfr, total_length);
			Set_delay(SW_TMR_WIFI_DLY, 10, &wifi_info.dly_flag);
			status = fx_file_close(&fx_EMMC_file);
			//status = fx_file_delete(&mmc_disk, "Device.pem");
		   }
	        tx_semaphore_put(&emmc_semaphore);

		  //}
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 2000;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag = 4;
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
		  if ((strstr((char*) wifi_rx_copy, "OK") != NULL))
		  {
			wifi_info.state = WIFI_CERT_UPLOAD_DONE;
			flag = 0;
			retry_count=0;
		  }
		  else
		  {
			if (++retry_count >= 5)
			{
			  wifi_info.state = WIFI_READ_AWS_CERT; // or WIFI_INIT
			  flag = 0;
			  retry_count = 0;
			}
			else
			{
			  // Retry sending Device.pem
				flag = 2;
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
				memset(wifi_rx_bfr, 0x00, sizeof(wifi_rx_bfr));
				Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1,&wifi_info.dly_flag);
				cert_upload_done = 1;
			 }
		  }
		}
	 }
	 else if (cert_upload_done == 2)
	 {
	   if ((flag == 0) && (wifi_info.dly_flag == 1))
	   {
		 read_size = 0;
		 flag = 1;
		 wifi_info.TxCplt = 0;
		 wifi_info.RxCplt = 0;
	    }
	   else if (flag == 1)
	   {
		if (Check_CTS() == 0)
		 {
		   flag = 2;
		   Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
		 }
		}
	   else if ((flag == 2) && (wifi_info.dly_flag))
	   {
		 flag = 3;
//		 FX_MEDIA *media_ptr = &mmc_disk;
//		 if (media_ptr != UX_NULL)
//		 {
	       tx_semaphore_get(&emmc_semaphore, TX_WAIT_FOREVER);
		 status = fx_file_open(&mmc_disk, &fx_EMMC_file, "Private.pem",FX_OPEN_FOR_READ);
		 if (status == FX_SUCCESS)
		 {
		   EMMC_info.file_size = fx_EMMC_file.fx_file_current_file_size;

		   status = fx_file_read(&fx_EMMC_file, read_buffer,sizeof(read_buffer), &bytes_read);
		   if ((status != FX_SUCCESS) && (status != FX_END_OF_FILE))
			Error_Handler();

		   uint16_t total_length = 0;
		   total_length = snprintf((char*) wifi_tx_bfr,sizeof(wifi_tx_bfr), "%s", AT_UPLOAD_PRIVATE_KEY);
		   memcpy(&wifi_tx_bfr[total_length], read_buffer, bytes_read);
		   total_length += bytes_read;
		   HAL_UART_Transmit_IT(&huart1, (uint8_t*) wifi_tx_bfr,total_length);
		   status = fx_file_close(&fx_EMMC_file);
		  }
	       tx_semaphore_put(&emmc_semaphore);

		// }
	   }
	   else if ((flag == 3) && (wifi_info.TxCplt == 1))
	   {
		   Assert_RTS();
		 uart1_info.rx_size = 2000;
		 HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
		 Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		 flag = 4;
		}
	   else if ((wifi_info.dly_flag) && (flag == 4))
	   {
		   Deassert_RTS();
		 if ((strstr((char*) wifi_rx_copy, "OK") != NULL))
		 {
		   wifi_info.state = WIFI_CERT_UPLOAD_DONE;
		   flag = 0;
		   retry_count = 0;
		}
		 else
		 {
		  if (++retry_count >= 5)
		  {
			 wifi_info.state = WIFI_READ_AWS_CERT; // or WIFI_INIT
			 flag = 0;
			 retry_count = 0;
		  }
		  else
		  {
		    // Retry sending Private.pem
			 flag = 2;
			 wifi_info.TxCplt = 0;
			 wifi_info.RxCplt = 0;
			 memset(wifi_rx_bfr, 0x00, sizeof(wifi_rx_bfr));
			 Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1,&wifi_info.dly_flag);
			 cert_upload_done = 2;
		   }
		 }
	   }
	}
  }
}
  break;

	case WIFI_VALIDATE_RCA_CERT:
	{
	  if (flag == 0)
	   { 			// Initial preparation
		 flag = 1;
		 i = 0;
		 h = 0;
		 j = 0;
	   }
	  else if (flag == 1)
	  {
	   if (Check_CTS() == 0)
		{
		   wifi_info.TxCplt = 0;
		   wifi_info.RxCplt = 0;
		   match_found = 0;
		   flag = 2;
		}
	  }
	  else if ((flag == 2) && (wifi_info.dly_flag))
	  {
		read_size = 0;
		memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
		memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
		memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

		uart1_info.tx_size = strlen((char*) AT_VALIDATE_RCA_CERT);
		strcpy((char*) &wifi_tx_bfr[5], AT_VALIDATE_RCA_CERT);
		HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
		//Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
		flag = 3;
	  }
	 else if ((flag == 3) && (wifi_info.dly_flag == 1))
	  {
		 Assert_RTS();
		uart1_info.rx_size = 10 * WIFI_MTU;
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
		Set_delay(SW_TMR_WIFI_DLY, 500, &wifi_info.dly_flag);
		flag = 4;
	 }
	 else if ((wifi_info.dly_flag) && (flag == 4))
	 {
	   Deassert_RTS();
	  if ((strstr((char*) wifi_rx_copy, "+USECD:3") != NULL))
	  {
		wifi_info.state = WIFI_VALIDATE_DEVICE_CERT;
		flag = 0;
		i=0;
	  }
	  else
	  {
		i++;
	   if (i >= 3)
	   {
		   wifi_info.state = WIFI_READ_AWS_CERT; // Failed after 3 tries
		   flag = 0;
		   i = 0;
	   }
	   else
	   {
	     flag = 2;
	   }
	  }
    }
  }
   break;

	case WIFI_VALIDATE_DEVICE_CERT:
	 {
	   if (flag == 0)
	   { 		// Initial preparation
		  flag = 1;
		  i = 0;
		  h = 0;
		  j = 0;
		}
	   else if (flag == 1)
	   {
		if (Check_CTS() == 0)
		 {
		  wifi_info.TxCplt = 0;
		  wifi_info.RxCplt = 0;
		  match_found = 0;
		  flag = 2;
		 }
		}
	   else if ((flag == 2) && (wifi_info.dly_flag))
	   {
		 read_size = 0;
		 memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
		 memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
		 memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

		 status = fx_file_delete(&mmc_disk, "Private.pem");
		 status = fx_file_delete(&mmc_disk, "RCA.pem");
		 status = fx_file_delete(&mmc_disk, "Device.pem");


		 uart1_info.tx_size = strlen((char*) AT_VALIDATE_DEVICE_CERT);
		 strcpy((char*) &wifi_tx_bfr[5], AT_VALIDATE_DEVICE_CERT);
		 HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
		// Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
		 flag = 3;
	   }
	   else if ((flag == 3) && (wifi_info.dly_flag == 1))
	   {
		  Assert_RTS();
		 uart1_info.rx_size = 10 * WIFI_MTU;
		 HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
		 Set_delay(SW_TMR_WIFI_DLY, 500, &wifi_info.dly_flag);
		 flag = 4;
		}
	   else if ((wifi_info.dly_flag) && (flag == 4))
	   {
		  Deassert_RTS();
		 if ((strstr((char*) wifi_rx_copy, "+USECD:3") != NULL))
		 {
			wifi_info.state = MQTT_CONNECTION;
			flag = 0;
			i=0;
		 }
		 else
		 {
			i++;
			if (i >= 3)
			{
			  wifi_info.state = WIFI_READ_AWS_CERT;
			  flag = 0;
			  i = 0;
			}
			else
			{
			  flag = 1;
			}
		  }
		}
	}
	break;

     case WIFI_CERT_UPLOAD_DONE:
     {
		if (++j == 3)
		{
			// All three certificates uploading is done.Delete the certs.
			//ULOG_DATA("DW19, Wi-Fi Certificate %d Updated",j);
			j = 0;
			cert_upload_done = 0;
			wifi_info.state = WIFI_VALIDATE_RCA_CERT;
			flag = 0;
			memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		}
		else
		{
			// Continue the cert uploading process
			flag = 0;
			wifi_info.state = WIFI_READ_AWS_CERT;
			cert_upload_done++;
			memset(&wifi_rx_copy, 0x00, (10*WIFI_MTU));
			memset(&wifi_rx_bfr, 0x00, (10*WIFI_MTU));
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			//ULOG_DATA("DW19, Wi-Fi Certficate %d Updated",j);
		}
      }
     break;

	case WIFI_CHECK_VERSION:
	      {
			if ((flag == 0))
			{
				i=0;
				flag = 1;
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
				memset(wifi_rx_bfr, 0x00, 10 * WIFI_MTU);

			} else if (flag == 1)
			{
			  if (Check_CTS() == 0)
			   {
				flag =2;
				Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
			   }
//			  else
//			  {
//				  wifi_info.state=WIFI_COMM_FAIL;
//			  }
			}
			else if ((flag == 2) && (wifi_info.dly_flag))
			{
				flag = 3;
                read_size =0;
				strcpy((char*)&wifi_tx_bfr[0], "AT+GMR\r\n");
				uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);
				HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			}
			else if ((flag == 3) && (wifi_info.TxCplt == 1))
			{
				Assert_RTS();
				uart1_info.rx_size = 100;
				HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
				Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
				flag = 4;
			}
			else if ((wifi_info.dly_flag) && (flag == 4))
			{
			   Deassert_RTS();
			   if (strstr((char*) wifi_rx_bfr, "OK") != NULL)
			   {
				memcpy(Control_versions.Wifi_chip_version, wifi_rx_bfr, 18);
				wifi_info.state = WIFI_LOGIN;
				flag = 0;
				i = 0; // reset retry counter
			   }
			   else
			   {
				if (i++ >= 5)  // Retry up to 5 times
				  {
					wifi_info.state = WIFI_INIT;  // Give up and go to init
					flag = 0;
					i = 0;
				  }
				else
				{
					// Retry sending AT+GMR
					flag = 2;
					wifi_info.TxCplt = 0;
					wifi_info.RxCplt = 0;
					memset(wifi_rx_bfr, 0x00, sizeof(wifi_rx_bfr));
					Set_delay(SW_TMR_WIFI_DLY, DLY_WIFI_RST1, &wifi_info.dly_flag);
				}
			}
		}
	   else
	   {
			// Do nothing
		}
	}
	break;

		case WIFI_SCAN:
		{
			if (flag == 0)
			{
				i = 0;
				j=0;
				flag = 1;
				match_found=0;
			}
			else if (flag == 1)
			{
			 if (Check_CTS() == 0)
			  {
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
				flag = 2;
				Set_delay(SW_TMR_WIFI_DLY, 10, &wifi_info.dly_flag);
			  }
//			 else
//			 {
//				 wifi_info.state = WIFI_COMM_FAIL;
//			 }
			}
			else if ((flag == 2) && (wifi_info.dly_flag))
			{
			  flag = 3;
			  read_size=0;
			  wifi_uart_rx_enable = 1;
			  memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
			  memset(&wifi_rx_bfr[0], 0x00, 10 * WIFI_MTU);
			  strcpy((char*) &wifi_tx_bfr[0], AT_SCAN_STATIONS);
			  uart1_info.tx_size = strlen((char*) AT_SCAN_STATIONS);
			  //AT_req_cmd();
			  uart1_info.tx_size = 100;
			  HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			}
			else if ((flag == 3) && (wifi_info.TxCplt == 1))
			{
				flag = 4;
				Assert_RTS();
				uart1_info.rx_size = 5000;

				HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
				Set_delay(SW_TMR_WIFI_DLY, 4000, &wifi_info.dly_flag);
			 }
			else if ((wifi_info.dly_flag) && (flag == 4))
			{
				flag =3;
			    uint16_t y = 0;
			    uint16_t k = 0, m = 0;
			    uint8_t ssid_buf[16] = {0};
			    Deassert_RTS();
			    for (y = 0; y < MAX_SSIDS; y++)
			    {
			        memset((char*)Wifi_ssid[y].SSid, 0x00, sizeof(Wifi_ssid[y].SSid));
			        Wifi_ssid[y].Authenication_type = 0;
			        Wifi_ssid[y].SSid_signal = 0;
			    }

			  //  wifi_count = 0; // reset count
			    y = 0;

			    while (k < read_size && y < MAX_SSIDS)
			    {
			        if ((wifi_rx_copy[k] == ',') && (wifi_rx_copy[k+1] == '\"'))
			        {
			            k +=2;
			            m = 0;
			            while (m < MAX_SSID_LEN-1 && k < read_size && wifi_rx_copy[k] != '\"')
			            {
			                Wifi_ssid[y].SSid[m++] = wifi_rx_copy[k++];
			            }
			            Wifi_ssid[y].SSid[m] = '\0'; // Null terminate

			            if (Wifi_ssid[y].SSid[0] == '\0')
			            {
			                continue;
			            }
			            k+=4;
			            while (k < read_size && (wifi_rx_copy[k+1] == '-'))
			            {
			                k++;
			            }
			            k++;
			            // ---- Extract RSSI ----
			            m = 0;
			            while (m < sizeof(ssid_buf)-1 && k < read_size && wifi_rx_copy[k] != ',')
			            {
			                ssid_buf[m++] = wifi_rx_copy[k++];
			            }
			            ssid_buf[m] = '\0';
			            Wifi_ssid[y].SSid_signal = atoi((const char*)ssid_buf);

			            // ---- Extract Authentication Type ----
			            k++;
			            memset(ssid_buf, 0, sizeof(ssid_buf));
			            m = 0;
			            while (m < sizeof(ssid_buf)-1 && k < read_size && wifi_rx_copy[k] != ',')
			            {
			                ssid_buf[m++] = wifi_rx_copy[k++];
			            }
			            ssid_buf[m] = '\0';
			            Wifi_ssid[y].Authenication_type = atoi((char *)ssid_buf);

			            // ---- Next SSID ----
			            y++;

						if (y > 10)
						{
							match_found = 1;
							break;
						}
			        }
			        if((wifi_rx_copy[k]=='O') && (wifi_rx_copy[k+1]=='K'))
			        {
			        	match_found = 1;
			        	break;
			        }
			        k++;
		       }
			if (match_found == 1)
			{
				int8_t q;
				if (y > 5)
				{
					SendN16DataToDisplay(WIFI_NAVIGATION_EN, 1);	//WIFI_NAVIGATION_EN
				} else {
					SendN16DataToDisplay(WIFI_NAVIGATION_EN, 0);
				}
				for (uint8_t p = 0; p < y; p++) //insertion sorting
				 {
					memcpy(&Wifi_ssid[10], &Wifi_ssid[p], sizeof(Wifi_ssid[p]));
					q = p;
					while (q > 0 && Wifi_ssid[q - 1].SSid_signal> Wifi_ssid[10].SSid_signal
							&& Wifi_ssid[q].SSid_signal != 0)
					{
						memcpy(&Wifi_ssid[q], &Wifi_ssid[q - 1],
								sizeof(Wifi_ssid[p]));
						q--;
					}
					memcpy(&Wifi_ssid[q], &Wifi_ssid[10], sizeof(Wifi_ssid[p]));
				}
				//SendN16DataToDisplay(WIFI_PAGE_EN,1);
				TargetPage = 35;
				SendCmdToDisplay(LOAD_PAGE_CMD);
				SendN16DataToDisplay(Wifi_refresh_en, 1);
				if (vp_rt_wr_str.WiFiStatus == 2)
				{
					wifi_info.state = wifi_info.Prevstate;
				} else
				{
					wifi_info.state = WIFI_LOGIN;
				}
				flag = 0;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
				for (uint8_t y = 0; (y < 10); y++)
				{
					uint8_t signal_stregth = 0;
					signal_stregth = Signal_icon(Wifi_ssid[y].SSid_signal, Wifi_ssid[y].Authenication_type);
					SendSTRDataToDisplay(SSID_address[y].SSID_add_WR, (uint8_t*) &Wifi_ssid[y].SSid);
					SendN16DataToDisplay(SSID_address[y].SING_add_WR, (uint16_t) signal_stregth);
				}
				SendVPDataCmdToDisplay(Wifi_scan_en);
			}
			else
			{
				if (i++ == 4)
				{
					wifi_info.state = WIFI_SCAN;
				}
				else
				{
					flag = 2;
				}
			}
		}

		else
		{
			//Do nothing
		}
	}
	break;
		case WIFI_LOGIN:
		{
		    iot_info.logs_ready = 1;

		    if ((wifi_info.dly_flag) && (flag == 0) && (UIwifi.connect_sts))
		    {
		        flag = 1;
		        i = 0;
		        h = 0;
		        j = 0;
		    }
		    else if (flag == 1)
		    {
		        if (Check_CTS() == 0)
		        {
		            wifi_connected = 0;
		            wifi_info.TxCplt = 0;
		            wifi_info.RxCplt = 0;
		            match_found = 0;
		            flag = 2;
		        }
		        // else { wifi_info.state = WIFI_COMM_FAIL; }
		    }
		    else if (flag == 2)
		    {
		        flag = 3;
		        read_size = 0;
		        memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
		        memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
		        memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

		        // Prepare command
		        strcpy((char*)&wifi_tx_bfr[5], WiFi_Init_Seq[j]);
		        uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[5]);

		        if (j == 1)
		        {
		            strcpy((char*)&wifi_tx_bfr[17], (char*)&comm_par.sy_par.wifi.uuid[0]);
		            uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[5]);
		            strcat((char*)&wifi_tx_bfr[uart1_info.tx_size + 5], "\"\r\n");
		            uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[5]);
		        }
		        else if (j == 2) // Password (only if exists)
		        {
		            if (comm_par.sy_par.wifi.pswd[0] != '\0')
		            {
		                strncpy((char*)&wifi_tx_bfr[17], (char*)&comm_par.sy_par.wifi.pswd[0],
		                        sizeof(wifi_tx_bfr) - 17);
		                wifi_tx_bfr[sizeof(wifi_tx_bfr) - 1] = '\0';
		                uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[5]);
		                strcat((char*)&wifi_tx_bfr[uart1_info.tx_size + 5], "\",0\r\n");
		                uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[5]);
		            }
		        }

		        HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
		    }
		    else if ((flag == 3) && (wifi_info.TxCplt == 1))
		    {
		        Assert_RTS();
		        uart1_info.rx_size = 10 * WIFI_MTU;
		        HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
		        wifi_info.RxCplt = 0;
		        Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
		        flag = 4;
		    }
		    else if ((wifi_info.dly_flag) && (flag == 4))
		    {
		        Deassert_RTS();

		        if (j == 0)
		        {
		            if (strstr((char*)wifi_rx_copy, "+UEWLD") != NULL ||
		                strstr((char*)wifi_rx_copy, "ERROR") != NULL)
		            {
		                match_found = 1;
		            }
		        }
		        if (strstr((char*)wifi_rx_copy, "OK") != NULL)
		        {
		            match_found = 1;
		        }

		        if (match_found == 1)
		        {
		            flag = 5;
		        }
		        else
		        {
		            if (++i == 5)
		            {
		                wifi_info.state = WIFI_CONNECTION_DETAILS;
		                flag = 0;
		                match_found = 0;
		            }
		            else
		            {
		                flag = 1;
		            }
		        }
		    }
		    else if (flag == 5)
		    {
		        // Skip password step if empty
		        if ((comm_par.sy_par.wifi.pswd[0] == 0x0) && (j == 1))
		        {
		            j++;
		        }

		        j++; // Increment for next command

		        if (j >= 3)
		        {
		            wifi_info.state = WIFI_LOGIN_CHECK;
		            flag = 0;
		            j = 0;
		            match_found = 0;
		            attempts_info.wifi_connect_attempts = 0;
		        }
		        else
		        {
		            flag = 1;
		        }

		        i = 0;
		        Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		    }
		    else
		    {
		        // Do nothing
		    }
		}
		break;


	case WIFI_LOGIN_CHECK:
		{
		if ((wifi_info.dly_flag) && (flag == 0))
		{
			flag = 1;
			i = 0;
			h = 0;
			j = 0;
		}
		else if (flag == 1)
		{
		if (Check_CTS() == 0)
		  {
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			flag = 2;
		  }
//		  else
//		  {
//		    wifi_info.state= WIFI_COMM_FAIL;
//		  }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, 3 * WIFI_MTU);

			uart1_info.tx_size = strlen((char*) AT_CMD_STA_ACTIVATE);
			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_STA_ACTIVATE);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			flag = 4;
			Assert_RTS();
			uart1_info.rx_size = 100;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);

			//wifi_info.RxCplt = 0;
			Set_delay(SW_TMR_WIFI_DLY, 5000, &wifi_info.dly_flag);
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
		if(i++!=5)
	    {
		   if ((strstr((char*) wifi_rx_copy, "+UEWSNU") != NULL))
		   {
			 match_found=1;
		   }
		if (match_found == 1)
		{
		  r = 0;
		  wifi_interface_id = atoi((const char*) &wifi_rx_bfr[h + 6]);
	
		  if (vp_rt_wr_str.WiFiStatus == 0)
		  {
			SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) &comm_par.sy_par.wifi.uuid[0]);
		  }
		   vp_rt_wr_str.WiFiStatus = 1;
		   System_Status[EVENT_FLAG] |= EVT_WIFI_CONNECTION;
		   if (attempts_info.wifi_connect_attempts)
		   {
			//  ULOG_DATA("DW1, Wi-Fi Connect Attempts :%d", attempts_info.wifi_connect_attempts);
		    }
		   strncpy((char*) &wifi_urc_code[0], (char*) &wifi_rx_bfr[h], 30);
		   wifi_urc_len = strlen(wifi_urc_code);
				//memset(&wifi_urc_code[wifi_urc_len-4], 0x00,4);
			for (int i = 0; i < wifi_urc_len; i++)
			{
			   if (wifi_urc_code[i] == ',')
			   {
				 wifi_urc_code[i] = '/';
			   }
			   else if ((wifi_urc_code[i] == '\r') && (wifi_urc_code[i + 1] == '\n'))
			   {
						//do nothing
				  wifi_urc_code[i] = 0;
				}
			}
				//ULOG_DATA("DW6, Wi-Fi URC Code:%s", &wifi_urc_code[0]);
				memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
				wifi_info.state = WIFI_CONNECTION_DETAILS;
				flag = 0;
				j = 0;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			}
		else
		 {
		   //check for proper credentials and WI-FI acess point network strength.
			memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
		    Assert_RTS();
		    HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
		    wifi_info.RxCplt=0;
			wifi_info.state = WIFI_LOGIN_CHECK;
		   // Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
			flag = 3;
			attempts_info.wifi_connect_attempts = (i + 1);
		 }
		}
		else
		{
			if (vp_rt_wr_str.WiFiStatus)
			{
				SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) "\0");
			}
			vp_rt_wr_str.WiFiStatus = 0;
			attempts_info.wifi_connect_attempts = 0;
			flag = 1;
			for (h = 0; h < uart1_info.rx_size; h++)
			{
			  if ((wifi_rx_bfr[h] == '+')&&(wifi_rx_bfr[h+1] == 'U') && (wifi_rx_bfr[h + 2] == 'E')&& (wifi_rx_bfr[h + 3] == 'W')&& (wifi_rx_bfr[h + 4] == 'L')&& ((wifi_rx_bfr[h + 5] == 'D')))
			  {
				match_found = 2;
				break;
			  }
			}
		}
	}
	else if(flag==1)
	{
		//wifi got disconnecetd clear sigstr,testiot sts
		SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR,0);
		SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,0);
		SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR,0);
		SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
		SendN16DataToDisplay(AWS_IOT_CNCTVTY,0);
		SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD,0);
		System_Status[EVENT_FLAG] &=~ EVT_WIFI_CONNECTION;
		if(match_found==2)
		{
			strncpy((char*)&wifi_urc_code[0],(char*)&wifi_rx_bfr[h],15);
			wifi_urc_len=strlen(wifi_urc_code);
			//memset(&wifi_urc_code[wifi_urc_len-4], 0x00,4);
			for(int i=0;i<wifi_urc_len;i++)
			{
				if(wifi_urc_code[i]==',')
				{
					wifi_urc_code[i]='/';
				}
				else if((wifi_urc_code[i]=='\r')&&(wifi_urc_code[i+1]=='\n'))
				{
					//do nothing
					wifi_urc_code[i]=0;
				}
			}
			//ULOG_DATA("DW8, Wi-Fi URC Code:%s",&wifi_urc_code[0]);
			memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
		}
		wifi_info.state=WIFI_MONITOR_CONN_STS;
		flag=0;
	}
  }
  break;

	case WIFI_CONNECTION_DETAILS:
	{
		if ((flag == 0) && (wifi_info.dly_flag))
		{
			flag = 1;
			i = 0;
			j = 0;
			retry_count=0;
		}
		else if (flag == 1)
		{
		  if (Check_CTS() == 0)
		  {
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			flag = 2;
		  }
//		  else
//		  {
//		    wifi_info.state=WIFI_COMM_FAIL;
//		  }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{

			read_size = 0;
			//memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_CONNECTION_DETAILS);
			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_CONNECTION_DETAILS);
			uart1_info.tx_size =strlen((char*)&wifi_tx_bfr[5]);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 10 * WIFI_MTU;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
			flag = 4;
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
		  Deassert_RTS();
		  if ((strstr((char*) wifi_rx_copy, "+UWSNST:0,0.0.0.0") != NULL))
		  {
			retry_count++;
			match_found = 0;
		  }
		  else
		  {
			match_found = 1;
			retry_count = 0;
		  }
		  if (retry_count >= 5)
		  {
			vp_rt_wr_str.WiFiStatus = 0;
			wifi_info.state = WIFI_LOGIN;

			retry_count = 0;
			i = 0;
			flag = 0;
			match_found = 0;
		  }
		  if (i++ == 3)
		  {
			wifi_info.state = WIFI_LOGIN;
			i = 0;
			flag =0;
			match_found=0;
		  }
		  else
		  {
			if (match_found == 0)
			{
			  flag = 3;
			}
			else
			{
				wifi_info.state = WIFI_STRENGTH;
				flag=0;
				match_found=0;
				strncpy((char*) &wifi_urc_code[0], (char*) &wifi_rx_bfr[h], 80);
				wifi_urc_len = strlen(wifi_urc_code);
					//memset(&wifi_urc_code[wifi_urc_len-2], 0x00, 2);
				for (int i = 0; i < wifi_urc_len; i++)
				{
				  if (wifi_urc_code[i] == ',')
				  {
					 wifi_urc_code[i] = '/';
				  }
				  else if ((wifi_urc_code[i] == '\r')&& (wifi_urc_code[i + 1] == '\n'))
				  {
					//do nothing
					 wifi_urc_code[i] = 0;
				   }
				}
				 wifi_urc_code[wifi_urc_len - 2] = 0;
				 //ULOG_DATA("DW1%d, Wi-Fi URC Code:%s", j, &wifi_urc_code[0]);
				 memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
				 flag = 0;
				 Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			  }
			}
		 }
		else
		{
		}
	}
	break;

	case WIFI_STRENGTH:
	{
		if ((flag == 0) && (wifi_info.dly_flag))
		{
			flag = 1;
			i = 0;
			j = 0;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		  {
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			flag = 2;
		  }
//		 else
//		 {
//		   wifi_info.state=WIFI_COMM_FAIL;
//		 }
	   }
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			read_size = 0;
			//memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

			uart1_info.tx_size = strlen((char*) AT_CMD_SIGNAL_STRENGTH);
			strcpy((char*) &wifi_tx_bfr[5], AT_CMD_SIGNAL_STRENGTH);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 10 * WIFI_MTU;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag = 4;
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
			for(int h=0;h<uart1_info.rx_size;h++)
		    {
			 if (wifi_rx_bfr[h] == ':' && wifi_rx_bfr[h + 1] == '4')
			 {
			   // Find the comma after ":4"
			   for (int g = h + 2; g < uart1_info.rx_size; g++)
			   {
				 if (wifi_rx_bfr[g] == ',')
				{
				   g++; // move to the first character after comma
				   int r = 0;
				   while (wifi_rx_bfr[g] != '\r' && wifi_rx_bfr[g] != '\n' && g < uart1_info.rx_size)
				   {
				     rssi_val[r++] = wifi_rx_bfr[g++];
				   }
				    rssi_val[r] = '\0';  // null terminate
				    match_found = 1;
				    break;
				   }
				}
		      }
		     }
			if (i++ == 3)
			{
				wifi_info.state = WIFI_CONNECTION_DETAILS;
				i = 0;
			}
			else
			{
			  if (match_found == 0)
			  {
				 flag = 3;
			   }
			  else
			  {
				r = 0;
				flag = 0;
				i = 0;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
				wifi_info.state = WIFI_SEND_SIGSTRENGTH;
			  }
			}
		}
		else
		{
		}
	}
	break;

//Need to check from here //
	case WIFI_SEND_SIGSTRENGTH:
	{
		rssi_value = atoi(rssi_val);
		rssi_value = abs(rssi_value);
		if ((rssi_value <= 55))
		{
			rssi_value = 5;
		} else if ((rssi_value >= 56) && ((rssi_value <= 60)))
		{
			rssi_value = 4;
		} else if ((rssi_value >= 61) && ((rssi_value <= 65)))
		{
			rssi_value = 3;
		} else if ((rssi_value >= 66) && ((rssi_value <= 70)))
		{
			rssi_value = 2;
		} else if (rssi_value == 32768)
		{
			bad_rssi_value = 1;
			rssi_value = 0;
		}
		else
		{
			//poor signal strength disconnect IOT later?
			//ULOG_DATA("DW18, Wi-Fi RSSI Value:-%d", rssi_value);
			rssi_value = 1;
		}
		if (rssi_value == 1)
		{
			System_Status[EVENT_FLAG] |= EVT_WIFI_POOR_RSSI;
		}
		else
		{
			System_Status[EVENT_FLAG] &= ~EVT_WIFI_POOR_RSSI;
		}
		if (bad_rssi_value == 1)
		{
			//wifi got disconnecetd clear sigstr,testiot sts
			if (vp_rt_wr_str.WiFiStatus)
			{
				SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 0);
				SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 0);
				SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
				SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
				SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) "\0");
				vp_rt_wr_str.WiFiStatus = 0;
				System_Status[EVENT_FLAG] &= ~ EVT_WIFI_CONNECTION;
				SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR, rssi_value);
				SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR, 0);
			}
			bad_rssi_value = 0;
			wifi_info.state = WIFI_MONITOR_CONN_STS;
		}
		else
		{
			if (vp_rt_wr_str.WiFiStatus == 0)
			SendSTRDataToDisplay(INTERNET_NAME,(uint8_t*) &comm_par.sy_par.wifi.uuid[0]);
			SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 1);
			SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR, rssi_value);
			SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR, 0);
			iot_info.Topic_sel = 0;
			wifi_info.state = CHECK_CERT_AVAILABILITY;
			System_Status[EVENT_FLAG] |= EVT_WIFI_CONNECTION;
			vp_rt_wr_str.WiFiStatus = 1;
		}
		Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);

  /* It is calling it here because checking before the wifi connect is effecting the WIFI connection */

		//wifi_info.state = CHECK_CERT_AVAILABILITY;
	}
	break;

	case MQTT_CONNECTION:
	 {
		if (wifi_info.RxCplt == 1)
		{
			strcpy((char*) wifi_rx_copy, (char*) wifi_rx_bfr);
			if (strstr((char*) wifi_rx_copy, "+UEWLD") != NULL)
			{
				if (vp_rt_wr_str.WiFiStatus)
				{
					SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) "\0");
				}
				//wifi got disconnecetd clear sigstr related sts
				SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR, 0);
				SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 0);
				SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR, 0);
				vp_rt_wr_str.WiFiStatus = 0;
				System_Status[EVENT_FLAG] &= ~ EVT_WIFI_CONNECTION;
				wifi_info.state = WIFI_MONITOR_CONN_STS;
				flag = 0;
				match_found = 0;
				break;
			}
		}

		if ((flag == 0) && (wifi_info.dly_flag)&&(UIwifi.IOT_connect_sts))
		{
			flag = 1;
			i = 0;
			j = 0;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		  {
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			flag = 2;
		  }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
			uart1_info.tx_size = strlen((char*) MQTT_Init_Seq[j]);
			strcpy((char*) &wifi_tx_bfr[5],MQTT_Init_Seq[j]);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			Assert_RTS();
			uart1_info.rx_size = 10 * WIFI_MTU;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			flag = 4;
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
			if (j == 0)
			{
			  if (strstr((char*) wifi_rx_copy, "+UEMQDC:") != NULL ||
				strstr((char*) wifi_rx_copy, "ERROR") != NULL)
			  {
				j++;
				flag = 2;
				wifi_info.TxCplt = 0;
				read_size = 0;
			 }
			  else
			  {
				if (++i >= 4)
				{
				  wifi_info.state = CHECK_CERT_AVAILABILITY;
				  flag = 0;
				  j = 0;
			   }
				else
				{
				  flag = 2;
				}
			  }
			}
			else
			{
			  if (strstr((char*) wifi_rx_copy, "OK") != NULL)
			  {
				j++;
				if (j >= 4)
			   {
				 wifi_info.state = TLS_CONNECTION;
				 flag = 0;
				 j = 0;
				 Set_delay(SW_TMR_WIFI_DLY, 5, &wifi_info.dly_flag);
			   }
				else
				{
				  flag = 2;
				  wifi_info.TxCplt = 0;
				  read_size = 0;
				}
			  }
			  else
			  {
				if (++i >= 4)
				{
				  wifi_info.state = CHECK_CERT_AVAILABILITY;
				  flag = 0;
				  j = 0;
				}
				else
				{
				  flag = 2;
				}
			  }
			}
		 }
	  }
	  break;

	case TLS_CONNECTION:
	{
		if ((flag == 0) && (wifi_info.dly_flag))
		{
			flag = 1;
			i = 0;
			j = 0;
		}
		else if (flag == 1)
		{
		  if (Check_CTS() == 0)
		  {
		    wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			flag = 2;
		  }
//		  else
//		  {
//			wifi_info.state = WIFI_COMM_FAIL;
//		  }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
		  read_size = 0;
		  memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
		  memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
		  memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));

		  strcpy((char*)&wifi_tx_bfr[0], AT_TLS_CONNECTION);
		  uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

		  strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size],(char*)&comm_par.sy_par.iot.iot_url);
		  uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

		  strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size], "\",8883,\"");
		  uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

		  strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size],(char*)&Plumbing_options.IOT_serial);
		  uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

		  strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size], "\"\r\n");
		  uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

		  HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
		  flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			flag = 4;
			Assert_RTS();
			uart1_info.rx_size = 10 * WIFI_MTU;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
			if (strstr((char*) wifi_rx_copy, "OK") != NULL)
			{
				match_found = 1;
			}
			if (i++ == 5)
			{
				wifi_info.state = CHECK_CERT_AVAILABILITY;
				flag =2;
			}
			else
			{
			  if (match_found == 1)
			  {
				flag = 0;
				wifi_info.state = CONNECT_MQTT;
				//Set_delay(SW_TMR_WIFI_DLY, 5, &wifi_info.dly_flag);
			 }
			  else
			  {
				wifi_info.TxCplt = 0;
				wifi_info.state = TLS_CONNECTION;
				flag = 2;
			   }
				Set_delay(SW_TMR_WIFI_DLY, 5, &wifi_info.dly_flag);
			}
		} else
		{
			//Do nothing.
		}
	}
	break;

	case CONNECT_MQTT:
	{
		if (flag == 0)
		{
			flag = 1;
			i = 0;
			j = 0;
		}
		else if (flag == 1)
		{
		  if (Check_CTS() == 0)
		  {
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			Set_delay(SW_TMR_WIFI_DLY, 10, &wifi_info.dly_flag);
			flag = 2;
		  }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			read_size = 0;
			memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
			uart1_info.tx_size = strlen((char*) AT_MQTT_CONNECT);
			strcpy((char*) &wifi_tx_bfr[0], AT_MQTT_CONNECT);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
			flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			flag = 4;
			Assert_RTS();
			uart1_info.rx_size = 10 * WIFI_MTU;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr, uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 4000, &wifi_info.dly_flag);
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
		  retry_count=0;
		  if (strstr((char*) wifi_rx_copy, "+UEMQC:0") != NULL)
		  {
			  flag=0;
			  match_found=1;
		   }
		  if (i++ == 5)
		  {
			  flag=0;
			  match_found=0;
			  wifi_info.state = MQTT_CONNECTION;
		  }
		  else
		  {
			if (match_found == 1)
			{
			  New_error_ind=0;
			  flag=0;
			  vp_rt_wr_str.WiFiStatus=2;
			 if(attempts_info.IOT_connect_attempts)
			 {
				//ULOG_DATA("DW2, IoT Connect attempts:%d",attempts_info.IOT_connect_attempts);
				attempts_info.IOT_connect_attempts=0;
			 }
				//ULOG_DATA("DW5, Topic Connected:%d",(iot_info.Topic_sel + 1));
				//ULOG_DATA("DW7, IoT URC Code:%s", &wifi_urc_code[0]);
				memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
				memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
				SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 1);
				SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 0);
				System_Status[EVENT_FLAG] |= EVT_IOT_CONNECTION;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
				wifi_info.state = MQTT_CONNECTED;
				memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
				if (clear_iot_popup)
				{
				  TargetPage = 0;
				  SendCmdToDisplay(LOAD_PAGE_CMD);
				}
				clear_iot_popup = 0;
			  flag = 0;
			 // wifi_info.state = MQTT_CONNECTED;
			}
			else
			{
			  wifi_info.TxCplt = 0;
			  wifi_info.RxCplt = 0;
			  flag = 2;
			}
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		  }
		}
		else if(flag==5)
		{
		  SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
		  SendN16DataToDisplay(AWS_IOT_CNCTVTY,0);
		  SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD,0);
		  SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,1);
		  SendN16DataToDisplay(STRING_POPUP_EN, 4);
		  SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Not able to reach IoT server!!!");
		  clear_iot_popup=1;
		  vp_rt_wr_str.WiFiStatus=1;
		  attempts_info.IOT_connect_attempts=0;
		  System_Status[EVENT_FLAG] &= ~EVT_IOT_CONNECTION;
		 // ULOG_DATA("DW17, Topic Disconnected:%d",(iot_info.Topic_sel+1));
		 // wifi_info.state = WIFI_MONITOR_WIFI_DISCONNECT;
		  flag=0;
		  New_error_ind=0;
		}
		else if (send_test_mail == 1)
		{
		    change_topic_3 = 2;
		    send_test_mail = 0;
		    iot_info.Topic_sel = 2;

		    flag = 0;
		}
		else if (Control_versions.Wifi_chip_version[0] == 0x00)
		{
		    wifi_info.Prevstate = wifi_info.state;
		    wifi_info.state = WIFI_CHECK_VERSION;
		    flag = 0;
		    i = 0;
		    j = 0;
		    match_found = 0;
		}
		else if ((send_test_mail == 0) &&(Control_versions.Wifi_chip_version[0] != 0x00) && (match_found == 1))
		{
		    wifi_info.state = WIFI_DAT_RX;
		    wifi_info.TxCplt = 1;
		    Response_Time = 0;
		    memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
		    flag = 0;
		}
		else
		{

		}
	}
	break;

	case MQTT_CONNECTED:
	{
		wifi_info.state = SUBSCRIBE_TOPIC;
	}
	case SUBSCRIBE_TOPIC:
	{
		if ((flag ==0) && (wifi_info.dly_flag))
		{
			flag = 1;
			i = 0;
			j = 0;
		}
		else if (flag == 1)
		{
		 if (Check_CTS() == 0)
		  {
			wifi_info.TxCplt = 0;
			wifi_info.RxCplt = 0;
			match_found = 0;
			flag = 2;
		  }
		 else
		 {
			wifi_info.state = WIFI_COMM_FAIL;
		 }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
			flag = 3;
			read_size = 0;
			memset(wifi_rx_copy, 0, 10 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 10 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
			uart1_info.tx_size = strlen((char*) AT_MQTT_SUBSCRIBE);
			strcpy((char*) &wifi_tx_bfr[5], AT_MQTT_SUBSCRIBE);
			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
			flag = 4;
			Assert_RTS();
			uart1_info.rx_size = 200;
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
			Deassert_RTS();
		 if(strstr((char*) wifi_rx_copy, "OK") != NULL)   // sometimes this response is not getting even though the command sent is correct
		{
			match_found=1;
		}
		if (i++ == 5)
	    {
		  wifi_info.state = SUBSCRIBE_TOPIC;
		}
		else
		{
		  if (match_found == 1)
		  {
			flag = 0;
			wifi_info.state = WIFI_MONITOR_WIFI_DISCONNECT;
		  }
		else
		{
		  wifi_info.TxCplt = 0;
		  wifi_info.state = SUBSCRIBE_TOPIC;
		  flag = 2;
		}
	  }
	}
   else
   {
   }
  }
	break;

	case PUBLISH_MESSAGE:
		{
		if ((flag == 0) && (wifi_info.dly_flag))
		{
			flag = 1;
			i = 0;
			j = 0;
		} else if (flag == 1)
		{
			if (Check_CTS() == 0)
			{
				wifi_info.TxCplt = 0;
				wifi_info.RxCplt = 0;
				match_found = 0;
				flag = 2;
			} else
			{
				wifi_info.state = WIFI_COMM_FAIL;
			}
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
		  if(!iot_info.Topic_sel)
          {
			flag = 3;
			read_size = 0;
			memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
			update_user_details();
			transmit_to_iot=1;
			strcpy((char*)&wifi_tx_bfr[0], AT_MQTT_PUBLISH);
			uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

			strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size],(char*)&Plumbing_options.IOT_serial);
			uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

			strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size],"\"\x01\x01\x61\{");
			uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

			strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size],(char*)&wifiMsgBuff1);
			uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

			strcpy((char*)&wifi_tx_bfr[uart1_info.tx_size], "\r\n");
		    uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

			HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
		}
		  else if(iot_info.Topic_sel==1)
		  {
			  flag = 3;
			  read_size = 0;
			  memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
			  memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
			  memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
			  transmit_to_iot=0;
			  update_device_logs();

			  uart1_info.tx_size = strlen((char*) AT_MQTT_PUBLISH);
			  strcpy((char*) &wifi_tx_bfr[5], AT_MQTT_PUBLISH);
			  strcpy((char*) &wifi_tx_bfr[5],(char*) &Plumbing_options.IOT_serial);
			  strcpy((char*) &wifi_tx_bfr[5], "\"\x01\x01\x94\{");

			  strcpy((char*)&wifi_tx_bfr[5], (char*)&wifiMsgBuff1);
			  strcpy((char*)&wifi_tx_bfr[5], "\r\n");
			  uart1_info.tx_size = strlen((char*)&wifi_tx_bfr[0]);

			  HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
		  }
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
		   flag = 4;
		   Assert_RTS();
		   uart1_info.rx_size = 7 * WIFI_MTU;
		   HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
		   Set_delay(SW_TMR_WIFI_DLY, 5000, &wifi_info.dly_flag);

		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
		  Deassert_RTS();
		  if (strstr((char*) wifi_rx_copy, "+UEMQDA:") != NULL)
		   {
			  match_found=1;
		   }
		if (i++ == 5)
		{
			wifi_info.state = WIFI_COMM_FAIL;
		}
		else
		{
		  if (match_found == 1)
		  {
			flag = 0;
			wifi_info.state = READ_MESSAGE;
		  }
		  else
		   {
			  wifi_info.TxCplt = 0;
			  wifi_info.state = SUBSCRIBE_TOPIC;
			  flag = 2;
			}
			Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
			}
		}
		else
		{

		}
	 }
		break;

	case READ_MESSAGE:
	{
	 if ((flag == 0) && (wifi_info.dly_flag))
	 {
		flag = 1;
		i = 0;
		j = 0;
	  }
	   else if (flag == 1)
	   {
	   if (Check_CTS() == 0)
		{
		 wifi_info.TxCplt = 0;
		 wifi_info.RxCplt = 0;
		 match_found = 0;
		 flag = 2;
		}
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
		 read_size = 0;
		 memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
		 memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
		 memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
		 uart1_info.tx_size = strlen((char*) AT_READ_MESSAGE);

		 strcpy((char*) &wifi_tx_bfr[5], AT_READ_MESSAGE);
		 HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
		 flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
		  flag = 4;
		  Assert_RTS();
		  uart1_info.rx_size = 7 * WIFI_MTU;
		  HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
		  Set_delay(SW_TMR_WIFI_DLY, 500, &wifi_info.dly_flag);
		}
		else if ((wifi_info.dly_flag) && (flag == 4))
		{
		  Deassert_RTS();
		  if (strstr((char*) wifi_rx_copy, "+UMQRB:") != NULL)
		  {
			wifi_info.state = WIFI_DAT_RX_DONE;
			flag = 0;
		  }
		  else
		  {
			wifi_info.state = SUBSCRIBE_TOPIC;
			flag = 0;
		  }
		}
	}
	break;

	case WIFI_DAT_RX_DONE:
	{
	   vp_rt_wr_str.WiFiStatus = 2;
	   if (wifi_info.dly_flag || wifi_info.RxCplt)
	   {
		  for (int i = 0; i < 2048; i++)
		  {
			if ((wifi_rx_bfr[i] != '\0'))
			{
			  match_found = 1;
			  h = i + 6;
			  break;
			}
		}
		  if (match_found == 1)
		  {
			if ((strstr((char*) &wifi_rx_bfr[h], "\"Status\":\"Success\"") != 0))
			{
			  Response_Time = 0;
			  //memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
			   if (!iot_info.Topic_sel)
			   {
				  iot_info.Topic_sel = 1;
				  wifi_info.state = PUBLISH_MESSAGE;
				  memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
				  memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
				}
			   else if (iot_info.Topic_sel == 1)
			   {
				  wifi_info.state = PUBLISH_MESSAGE;
				  //memset(&error_buff_wifi[0], 0x00, sizeof(error_buff_wifi));
				   memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
				   memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
				}
			   else if (iot_info.Topic_sel == 3)
			   {
				 wifi_info.state = PUBLISH_MESSAGE;
						//memset(&error_buff_wifi[0], 0x00, sizeof(error_buff_wifi));
				  memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
				  memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
				}
			   else if ((change_topic_3 == 2))
			   {
				 //ack recieved for testmail
				 change_topic_3 = 0;
			     iot_info.Topic_sel = 0;
				 wifi_info.state = PUBLISH_MESSAGE;
				 memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
				 memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
				 SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
				 SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
				 SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 1);
				}
				if ((strstr((char*) &wifi_rx_bfr[h],"\"EmailStatus\":\"Failed\"") != 0))
				{
				  System_Status[WARNING_FLAG] |= WRN_EMAIL_FAIL;
				}
				else if ((strstr((char*) &wifi_rx_bfr[h],"\"EmailStatus\":\"Success\"") != 0))
				{
				  System_Status[WARNING_FLAG] &= ~WRN_EMAIL_FAIL;
				}
		 		System_Status[WARNING_FLAG] &= ~WRN_EMAIL_VALID;
		    }
//			else
//			{
//			   iot_info.Topic_sel=1;
//			   if(Control_versions.Display_download_icon==1)
//	           {
//				  Control_versions.Display_download_icon=0;
//				  SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);
//			   }
//			 }
			else if ((strstr((char*) &wifi_rx_bfr[h],"\"Status\":\"Failed\"") != 0))
			{
			  //Error handling Mechanism(check ErrorMessage and act accordingly.
			  if ((strstr((char*) &wifi_rx_bfr[h], "100") != 0))
			  {
				// AWS RELATED -100
			  }
			  else if ((strstr((char*) &wifi_rx_bfr[h], "200") != 0))
			  {
				//CERTIFICATE -200
			  }
			  else if ((strstr((char*) &wifi_rx_bfr[h], "300") != 0))
			  {
				//MANDATORY FIELDS -300
				if ((strstr((char*) &wifi_rx_bfr[h], "3004") != 0))
				{
				  System_Status[WARNING_FLAG] |= WRN_EMAIL_VALID;
				}
				else
				{
				  System_Status[WARNING_FLAG] &= ~WRN_EMAIL_VALID;
				}
				if ((strstr((char*) &wifi_rx_bfr[h], "3008") != 0))
				{
				  memset(&comm_par.sy_par.iot.User_ID[0], 0x00,sizeof(comm_par.sy_par.iot.User_ID));
				  EEPROM_Write(Aws_user_id, 0,(uint8_t*) &comm_par.sy_par.iot.User_ID,63);
				  memset(&comm_par.sy_par.iot.iot_email, 0x00,sizeof(comm_par.sy_par.iot.iot_email));
				  EEPROM_Write(1920, 0,(uint8_t*) &comm_par.sy_par.iot.iot_email,64);
				  SendSTRDataToDisplay(AWS_EMAIL_WR,(uint8_t*) "\0");
				  SendSTRDataToDisplay(AWS_EMAIL_RD, (uint8_t*) "\0");
				}
			}
			  else if ((strstr((char*) &wifi_rx_bfr[h], "400") != 0))
			  {
				//NOT FOUND - 400
			  }
			  else if ((strstr((char*) &wifi_rx_bfr[h], "500") != 0))
			  {
				//EMAIL RELATED - 500
				if ((strstr((char*) &wifi_rx_bfr[h], "5001") != 0))
				{

				}
			  }
			else if ((strstr((char*) &wifi_rx_bfr[h], "600") != 0))
			{
			  //USERID OR TOKEN NOT MATCHING -600
			}
			if ((strstr((char*) &wifi_rx_bfr[h], "1404") != 0))
			{
			  //USERID OR TOKEN NOT MATCHING -600
			 memset(&wifi_tx_bfr1[0], 0x00,sizeof(wifi_tx_bfr1));
			 memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
			 Response_Time = 0;
			 iot_info.Topic_sel = 1;
			 if (Control_versions.IOT_File_status != 0)
			 {
				Control_versions.IOT_File_status = 0;
				SendN16DataToDisplay(IOT_FILE_STATUS,Control_versions.IOT_File_status);
			 }
	       }
			if ((strstr((char*) &wifi_rx_bfr[h], "500") == 0))
			{
			  if ((iot_info.Topic_sel == 1))
			  {
						//logs will not be sent to email
						// attempts_info.nof_iot_logs_missed++;
						// EEPROM_Write(Aws_missed_logs_add, 0,(uint8_t*)& attempts_info.nof_iot_logs_missed,4);
			  }
			}
			else
			{
			  if (iot_info.Topic_sel == 1)
			  {
				// memset(&error_buff_wifi[0], 0x00, sizeof(error_buff_wifi));
				memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
				memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
			   }
			}
			 wifi_info.state = PUBLISH_MESSAGE;
			 Response_Time = 0;
			}	//end of status failed

		   if ((strstr((char*) &wifi_rx_bfr[h], "+UEWLD:") != 0))
		   {
				//wi-fi link disconnected
			 wifi_info.state = WIFI_CONN_MNGMNT;
			 link_disconnect = 1;
			 System_Status[EVENT_FLAG] &= ~ EVT_WIFI_CONNECTION;
			 strncpy((char*) &wifi_urc_code[0], (char*) &wifi_rx_bfr[h + 1],15);
			 wifi_urc_len = strlen(wifi_urc_code);
				// memset(&wifi_urc_code[wifi_urc_len-4], 0x00,4);
			for (int i = 0; i < wifi_urc_len; i++)
			{
			  if (wifi_urc_code[i] == ',')
			  {
				wifi_urc_code[i] = '/';
			 }
			  else if ((wifi_urc_code[i] == '\r')&& (wifi_urc_code[i + 1] == '\n'))
			  {
				//do nothing
				wifi_urc_code[i] = 0;
			  }
			}
			//ULOG_DATA("DW10, Wi-Fi URC Code:%s", &wifi_urc_code[0]);
			memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
			SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) "\0");
			vp_rt_wr_str.WiFiStatus = 0;
			SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 0);
			SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR, 0);
			SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR, 0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
		 }
		   else if (strstr((char*) &wifi_rx_bfr[h], "+UEMQDC") != 0)
		   {
				//Peer got disconnected
				wifi_info.state = WIFI_CONN_MNGMNT;
				peer_disconnect = 2;
				System_Status[EVENT_FLAG] &= ~ EVT_IOT_CONNECTION;
				vp_rt_wr_str.WiFiStatus = 1;
				SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 0);
				SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
				SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
			}
			memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
			match_found = 0;
		}
	   else
	     {
		  if (change_topic_3 == 2)
		  {
		 		//wait for success or failure response.
			if (++Response_Time >= 10)
			{
			  // wait for 1 min to declare ack missed..
			  change_topic_3 = 0;
			  Response_Time = 0;
			  iot_info.Topic_sel = 0;
			  wifi_info.state = WIFI_DISCONNECT;
			  SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
			  SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
			  SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 1);
			  //attempts_info.nof_iot_acks_missed++;
			  //EEPROM_Write(Aws_missed_acks_add, 0,(uint8_t*)& attempts_info.nof_iot_acks_missed,4);
			}
		  }
	      else
	      {
		   if (transmit_to_iot)
		   {
			 if(++Response_Time>=4)
			  {
				//wait for 1 min to declare ack missed.
				wifi_info.state = PUBLISH_MESSAGE;
				if(iot_info.file_open_data)
				 {
					memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
					memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
					Response_Time=0;
					iot_info.file_open_data=0;
				 }
				  Response_Time=0;
				// attempts_info.nof_iot_acks_missed++;
				// EEPROM_Write(Aws_missed_acks_add, 0,(uint8_t*)& attempts_info.nof_iot_acks_missed,4);
			  }
			  else
			  {
				 wifi_info.state = WIFI_DAT_RX_DONE;
			  }
		    }
		   else
		   {
			  wifi_info.state = PUBLISH_MESSAGE;
		   }
	      }
        }
	     flag = 0;
	     memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
	     Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
	  }
	}
	break;

	case READ_MESSAGE_SUCCESSFUL:
	{
     if ((flag == 0) && (wifi_info.dly_flag))
     {
		flag = 1;
		i = 0;
		j = 0;
	  }
	   else if (flag == 1)
	   {
	    if (Check_CTS() == 0)
		{
		  wifi_info.TxCplt = 0;
		  wifi_info.RxCplt = 0;
		  match_found = 0;
		  flag = 2;
		 }
		}
		else if ((flag == 2) && (wifi_info.dly_flag))
		{
		 read_size = 0;
		 memset(wifi_rx_copy, 0, 7 * WIFI_MTU);
		 memset(wifi_rx_bfr, 0, 7 * WIFI_MTU);
		 memset(wifi_tx_bfr, 0, sizeof(wifi_tx_bfr));
		 uart1_info.tx_size = strlen((char*) AT_CMD_WIFI_DISCONNECT);

		 strcpy((char*) &wifi_tx_bfr[5], AT_CMD_WIFI_DISCONNECT);
		 HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[5], uart1_info.tx_size);
		 flag = 3;
		}
		else if ((flag == 3) && (wifi_info.TxCplt == 1))
		{
		  Assert_RTS();
		  uart1_info.rx_size = 7 * WIFI_MTU;
		  HAL_UARTEx_ReceiveToIdle_IT(&huart1, wifi_rx_bfr,uart1_info.rx_size);
		  Set_delay(SW_TMR_WIFI_DLY, 500, &wifi_info.dly_flag);
		  flag = 4;
		}
		else if ((flag == 4) && (wifi_info.dly_flag))
		{
		  Deassert_RTS();
		  if (strstr((char*) wifi_rx_copy, "+UEWSND") != NULL)
		  {
			wifi_info.state = WIFI_DISCONNECTED;

			flag = 0;
		  }
		  else
		  {
			wifi_info.state = WIFI_DISCONNECT;
			flag = 0;
		  }
		}
      }
	break;

	case WIFI_DISCONNECT:
	{
	  if (flag == 0)
	  {
		// Prepare for the transmit
		flag = 1;
		i = 0;
		h = 0;
		j = 0;
		match_found = 0;
	  }
	  else if (flag == 1)
	  {
		wifi_info.TxCplt = 0;
		wifi_connected = 0;
		// Wait here until WiFi module is ready to accept data
		if (Check_CTS() == 0)
		{
		  flag = 2;
		}
		else
		{
		  wifi_info.state = WIFI_COMM_FAIL;
		}
	   }
	  else if (flag == 2)
	  {
		// WiFi module is ready to accept data
		flag = 3;
		memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
		uart1_info.tx_size = strlen((char*) AT_CMD_WIFI_DISCONNECT);
		strcpy((char*) &wifi_tx_bfr[5], AT_CMD_WIFI_DISCONNECT);
		uart1_info.tx_size = 100;
		HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
	  }
	  else if ((flag == 3) && (wifi_info.TxCplt == 1))
	  {
		flag = 4;
		memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
		uart1_info.rx_size = 256;
		HAL_UART_Abort(&huart1);
		Assert_RTS();
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
		wifi_info.RxCplt = 0;
		Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
	  }
	  else if ((flag == 4) && ((wifi_info.dly_flag) || (wifi_info.RxCplt)))
	  {
		uart1_info.tx_size = 0;
		for (h = 0; h < uart1_info.rx_size; h++)
		{
		  if (((wifi_rx_bfr[h] == '+') && (wifi_rx_bfr[h + 1] == 'U') && (wifi_rx_bfr[h + 2] == 'E') && (wifi_rx_bfr[h + 3] == 'W') && (wifi_rx_bfr[h + 4] == 'L') && (wifi_rx_bfr[h + 5] == 'D')) ||
			((wifi_rx_bfr[h] == 'E') && (wifi_rx_bfr[h + 1] == 'R') && (wifi_rx_bfr[h + 2] == 'R') && (wifi_rx_bfr[h + 3] == 'O') && (wifi_rx_bfr[h + 4] == 'R')))
		  {
			match_found = 1;
			break;
		  }
		}
		if (match_found == 1)
		{
		  flag = 5;
		  UIwifi.connect_sts = 0;
		  SendN16DataToDisplay(WIFI_CONNECT_WR, UIwifi.connect_sts);
		  SendN16DataToDisplay(WIFI_CONNECT_RD, UIwifi.connect_sts);
		  if( UIwifi.connect_sts==0)
		  {
			  UIwifi.IOT_connect_sts = 0;
			  iot_info.Topic_sel = 0;
			  SendN16DataToDisplay(IOT_CONNECT_WR, UIwifi.IOT_connect_sts);
			  SendN16DataToDisplay(IOT_CONNECT_RD, UIwifi.IOT_connect_sts);
			  EEPROM_Write(wifi_connect_sts, 0,(uint8_t*) &UIwifi.connect_sts, 2);
		  }
		  if (vp_rt_wr_str.WiFiStatus)
		  {
			SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) "\0");
		  }
		  vp_rt_wr_str.WiFiStatus = 0;
		  SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR, 0);
		  SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 0);
		  SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR, 0);
		  SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 0);
		  SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
	      SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
		  //System_Status[EVENT_FLAG] |= EVT_WIFI_DIS_CONNECT_MANUAL;
		  System_Status[EVENT_FLAG] &= ~ EVT_IOT_CONNECTION;
		  System_Status[EVENT_FLAG] &= ~ EVT_WIFI_CONNECTION;
		  wifi_info.state = WIFI_LOGIN;
		  flag =0;
				//send disconnect status to UI
		}
		else
		{
		  if (++i == 3)
		  {
			wifi_info.state = WIFI_DISCONNECT;
			flag =0;
		  }
		  else
		  {
			flag = 1;
		   }
		}
	  }
	}
	break;

	case WIFI_CONN_MNGMNT:
	{
	  if (link_disconnect == 1)
	  {

		//try to reconnect using the credentials.
		wifi_info.state = WIFI_RESET;
		flag = 0;
		link_disconnect = 0;
	  }
	  else if (link_disconnect == 2)
	  {
		//disconnect Wi-Fi
		 wifi_info.state = WIFI_DISCONNECT;
		 link_disconnect = 0;
	 }
	  else if (peer_disconnect == 2)
	  {
		//disconnect from IOT
		wifi_info.state = WIFI_DISCONNECT_IOT;
		wifi_info.UART1ErrorDetected=1;
		peer_disconnect = 0;
	  }
	  Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
	  flag = 0;
	}
	break;

//	case WIFI_DISCONNECTED:
//	{
//		wifi_info.state = WIFI_IDLE;
//		BLE_info.state = BLE_INIT;
//	}
//	break;
//
//	case WIFI_IDLE:
//	{
//
//	}
//	break;

	case WIFI_MONITOR_CONN_STS:
	{
		if (flag == 0)
		{
			// Prepare for the transmit
			flag = 1;
			i = 0;
			h = 0;
			j = 0;
			match_found = 0;
		}
		else if (flag == 1)
		{
			flag = 2;
		}
		else if (flag == 2)
		{
			// WiFi module is ready to accept data
			flag = 3;
			memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
			memset(&wifi_rx_copy[0], 0x00, sizeof(wifi_rx_copy));
			uart1_info.rx_size = 150;
			Assert_RTS();
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
			wifi_info.RxCplt = 0;
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		}
		else if ((flag == 3) && ((wifi_info.dly_flag)|| (wifi_info.RxCplt)))
		{
			for (h = 0; h < uart1_info.rx_size; h++)
			{
			 if(strstr((char *)wifi_rx_copy, "+UEWSNU") != NULL)
//				if ((wifi_rx_bfr[h] == '+') && (wifi_rx_bfr[h + 1] == 'U')&& (wifi_rx_bfr[h + 2] == 'E')&& (wifi_rx_bfr[h + 3] == 'W')
//						&& (wifi_rx_bfr[h + 4] == 'S')&&(wifi_rx_bfr[h + 5] == 'N')&&(wifi_rx_bfr[h + 6] == 'U'))
				{
					match_found = 1;
					//break;
				}
				else
				{
					vp_rt_wr_str.WiFiStatus=0;
				}
			}
			if (match_found == 1)
			{
				flag = 4;
				j = 0;
			}
			else
			{
				if (++j == 10)
				{
					flag = 0;
					wifi_info.state = WIFI_STRENGTH;
				}
				else
				{
					flag = 2;
				}
				//Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			}
		}
		else if (flag == 4)
		{
			if (vp_rt_wr_str.WiFiStatus == 0)
			{
				SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) &comm_par.sy_par.wifi.uuid[0]);
			}
			System_Status[EVENT_FLAG] |= EVT_WIFI_CONNECTION;
//			strncpy((char*) &wifi_urc_code[0], (char*) &wifi_rx_bfr[h], 30);
//			wifi_urc_len = strlen(wifi_urc_code);
			//memset(&wifi_urc_code[wifi_urc_len-4], 0x00,4);

		//	ULOG_DATA("DW16, Wi-Fi URC Code:%s", &wifi_urc_code[0]);
			//memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
			wifi_info.state = WIFI_STRENGTH;
			flag = 0;
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		}
		else
		{

		}
	  }
		break;

	case WIFI_MONITOR_RSSI:
	{
	   wifi_info.state=WIFI_STRENGTH;
	   flag=0;
	   Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
	}
	break;

	case WIFI_MONITOR_WIFI_DISCONNECT:
	{
		if (flag == 0)
		{
			flag = 1;
			match_found = 0;
			h = 0;
			wifi_info.RxCplt=0;
			memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
			uart1_info.rx_size = 1534;
			Assert_RTS();
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
			Set_delay(SW_TMR_WIFI_DLY, 5000, &wifi_info.dly_flag);
		}
		else if (((wifi_info.dly_flag) || (wifi_info.RxCplt)) && (flag == 1))
		{
			for (h = 0; h < uart1_info.rx_size; h++)
			{
				if ((wifi_rx_bfr[h] == '+') && (wifi_rx_bfr[h + 1] == 'U')
						&& (wifi_rx_bfr[h + 2] == 'E')
						&& (wifi_rx_bfr[h + 3] == 'W')
						&& (wifi_rx_bfr[h + 4] == 'L')
						&& (wifi_rx_bfr[h + 5] == 'D'))
				{
					match_found = 1;
					//break;
				}
			}
			if (match_found == 1)
			{
				if (vp_rt_wr_str.WiFiStatus)
				{
				  SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*) "\0");
			    }
				//wifi got disconnecetd clear sigstr related sts
				SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR, 0);
				SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 0);
				SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR, 0);
				vp_rt_wr_str.WiFiStatus = 0;
				System_Status[EVENT_FLAG] &= ~ EVT_WIFI_CONNECTION;
				strncpy((char*) &wifi_urc_code[0], (char*) &wifi_rx_bfr[h], 15);
				wifi_urc_len = strlen(wifi_urc_code);
				//memset(&wifi_urc_code[wifi_urc_len-4], 0x00,4);
			//	ULOG_DATA("DW9, Wi-Fi URC Code:%s", &wifi_urc_code[0]);
				memset(&wifi_urc_code[0], 0x00, sizeof(wifi_urc_code));
				wifi_info.state = WIFI_MONITOR_CONN_STS;
				flag = 0;
				match_found = 0;
			}
			else
			{
				flag = 2;
			}
		}
		else if ((flag == 2))
		{
			flag = 0;
			if (UIwifi.IOT_connect_sts)
			{
			  New_error_ind = 0;
			  wifi_info.state = WIFI_MONITOR_WIFI_DISCONNECT;
			  Set_delay(SW_TMR_WIFI_DLY, 500, &wifi_info.dly_flag);
			}
		  }
		}
	break;

  case WIFI_DISCONNECT_IOT:
   {
	 if (flag == 0)
	 {
		wifi_info.TxCplt = 0;
		i = 0;
		j = 0;
		match_found = 0;
		flag = 1;
	  }
	 else if (flag == 1)
	 {
		i = 0;
		// Wait here until WiFi module is ready to accept data
	   if (Check_CTS() == 0)
	   {
		  flag = 2;
		  Set_delay(SW_TMR_WIFI_DLY, 10, &wifi_info.dly_flag);
	    }
//	   else
//	    {
//		 wifi_info.state = WIFI_COMM_FAIL;
//		}
	  }
	 else if ((flag == 2) && (wifi_info.dly_flag))
	 {
		flag = 3;
		memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
		memset(&wifi_rx_bfr[0], 0x00, sizeof(wifi_rx_bfr));
		strcpy((char*) &wifi_tx_bfr[0], AT_MQTT_DISCONNECT);
		uart1_info.tx_size = strlen(AT_MQTT_DISCONNECT);
		uart1_info.tx_size = 30;
		HAL_UART_Transmit_IT(&huart1, &wifi_tx_bfr[0], uart1_info.tx_size);
	}
	 else if ((flag == 3) && (wifi_info.TxCplt == 1))
	 {
		flag = 4;
		uart1_info.rx_size = 256;
		HAL_UART_Abort(&huart1);
		Assert_RTS();
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0],uart1_info.rx_size);
		//wifi_info.RxCplt = 0;
		Set_delay(SW_TMR_WIFI_DLY, 1000, &wifi_info.dly_flag);
	  }
	 else if ((flag == 4) && ((wifi_info.dly_flag) || (wifi_info.RxCplt)))
	 {
		if ((strstr((char*) &wifi_rx_bfr, "+UEMQDC:") != NULL) || (strstr((char*) &wifi_rx_bfr, "ERROR") != NULL))
		{
			match_found = 1;
		}
		else
		{
		  if (++i >= 5)
		  {
			match_found = 1;
		  }
		  else
		  {
			 for (int h = 0; h < uart1_info.rx_size; h++)
			 {
			   if (((wifi_rx_bfr[h] == 'O')&& (wifi_rx_bfr[h + 1] == 'K'))||
				((wifi_rx_bfr[h] == 'E')&& (wifi_rx_bfr[h + 1] == 'R')
				&& (wifi_rx_bfr[h + 2] == 'R')&& (wifi_rx_bfr[h + 3] == 'O')
				&& (wifi_rx_bfr[h + 4] == 'R')))
			   {
				 match_found = 1;
				 break;
				}
			  }
			}
		 }
	   }
	   if (match_found == 1)
	   {
		wifi_info.state = MQTT_CONNECTION;
		flag = 0;
		i = 0;
		j = 0;
		match_found = 0;

			// Check IOT connection status
		if (!UIwifi.IOT_connect_sts)
		 {
			SendN16DataToDisplay(IOT_CONNECT_WR, UIwifi.IOT_connect_sts);
			SendN16DataToDisplay(IOT_CONNECT_RD, UIwifi.IOT_connect_sts);
			EEPROM_Write(wifi_connect_sts, 0,(uint8_t*) &UIwifi.connect_sts, 2);
			iot_info.Topic_sel = 0;

		    SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH, 0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY, 0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD, 0);
			SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH, 1);

			vp_rt_wr_str.WiFiStatus = 1;
			System_Status[EVENT_FLAG] &= ~EVT_IOT_CONNECTION;
			wifi_info.state = WIFI_MONITOR_WIFI_DISCONNECT;
			flag = 0;
		    Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		  }
			else if (Control_versions.Wifi_chip_version[0] == 0x00)
			{
				wifi_info.Prevstate = wifi_info.state;
				wifi_info.state = WIFI_CHECK_VERSION;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			}
			else
			{
				flag = 1;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			}
		}
	   else
	   {
	 	//flag = 0;
	   }
     }
break;

	case WIFI_COMM_FAIL:
	{
		//wait here error detected in communication.
		//SD_info.certificate_up = 0;
		if (wifi_info.UART1ErrorDetected == 1)
		{
			if (HAL_OK == HAL_UART_Abort(&huart1))
			{
				wifi_info.UART1ErrorDetected = 0;
				HAL_UARTEx_ReceiveToIdle_IT(&huart1, &wifi_rx_bfr[0], uart1_info.rx_size);
				flag = 0;
				Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
			}
		}
		else
		{
			wifi_info.state = WIFI_RESET;
			flag = 0;
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		}
	}
		break;



	default:
	// Do nothing. Just wait to die (or) mains power restoration
		break;
	}
	if (wifi_info.UART1ErrorDetected == 1)
	{
		if (HAL_OK == HAL_UART_Abort(&huart1))
		{
			__HAL_UART_CLEAR_OREFLAG(&huart1);
			wifi_info.UART1ErrorDetected = 0;
			flag = 0;
			Set_delay(SW_TMR_WIFI_DLY, 100, &wifi_info.dly_flag);
		}
	}

}

 void read_bin_file()
 {

 	if(xmodem_info.bin_present==0)
 	{
 	  status = fx_file_open(&mmc_disk, &fx_EMMC_file, "NORA-3.0.bin", FX_OPEN_FOR_READ);
 	  if(status == FX_SUCCESS)
 	   {
 		 xmodem_info.offset = 0;
 		 xmodem_info.file_size = fx_EMMC_file.fx_file_current_file_size;
 		 xmodem_info.bin_present = 1;
 		}
    }
 	else if((start_file_transfer==1)&&(byte_sent_success==1))
 	{
 	   status = fx_file_read(&fx_EMMC_file, &xmodem_info.buffer[0], 1024, &bytes_read);
 	   if (status == FX_SUCCESS)
 	   {
 		   send_data =1;
 	   }
 	   if (xmodem_info.file_size <= 0)
 		{
 		  // Read operation is completed
 		  //fresult = f_close(&SDFile);
 		  status = fx_file_close(&fx_EMMC_file);
 		  end_of_file=1;
 		}
 	}
 }

 int calulatecrc(char *ptr, int count)
 {
     int crc = 0;
     char i;
     while (--count >= 0)
     {
         crc = crc ^ ((int)*ptr++ << 8);
         i = 8;
         do
         {
             if (crc & 0x8000)
                 crc = (crc << 1) ^ 0x1021;
             else
                 crc = crc << 1;
         } while (--i);
     }
     return (crc);
 }

 void build_xmodem_packet_1K(uint8_t *payload,uint16_t payload_len,uint8_t pkt_num,uint8_t *out_buf)
 {
     out_buf[0] = X_STX;            // 0x02 ? XMODEM-1K identifier
     out_buf[1] = pkt_num;          // Packet number
     out_buf[2] = 255 - pkt_num;    // Packet number complement

     memset(&out_buf[3], 0x1A, 1024);
     memcpy(&out_buf[3], payload, payload_len);

     uint16_t crc = calulatecrc((char*)&out_buf[3], 1024);

     out_buf[1027] = (crc >> 8) & 0xFF;   // CRC High byte
     out_buf[1028] = crc & 0xFF;          // CRC Low byte

     uint16_t crc_check = ((uint16_t)out_buf[1027] << 8) | out_buf[1028];

     if (crc == crc_check)
         CRC_OK = 1;
     else
         CRC_OK = 0;
 }

// void build_xmodem_packet(uint8_t *payload, uint16_t payload_len, uint8_t pkt_num, uint8_t *out_buf)
// {
//	 out_buf[0] = X_STX; // Start of header (0x01)
//	 out_buf[1] = pkt_num; // Packet number
//	 out_buf[2] = 255 - pkt_num; // Complement
//
//
//	 // Fill payload
//	 memset(&out_buf[3], 0x1A, 128); // Padding with 0x1A if less data
//	 memcpy(&out_buf[3], payload, payload_len);
//
//	 // CRC for 128 bytes
//	 uint16_t crc = calulatecrc((char*)&out_buf[3], 128);
//	 out_buf[131] = (crc >> 8) & 0xFF;
//	 out_buf[132] = crc & 0xFF;
//
//	 //uint16_t crc_check = calulatecrc((char*)&out_buf[3], 128);
//	 uint16_t crc_from_packet = ((uint16_t)out_buf[131] << 8) | ((uint16_t)out_buf[132]);
//
//	 if (crc == crc_from_packet)
//	 {
//	   CRC_OK =1;   // CRC OK, safe to send
//	 }
//	 else
//	 {
//	    //return false;   // CRC mismatch, do not send
//	 }
// }

 void update_user_details()
 {
 	  memset(&wifiMsgBuff1[0], 0x00, sizeof(wifiMsgBuff1));
 	  memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
 	 //#define   AT_MQTT_PUBLISH         "AT+UMQPB=0,0,0,\"deviceupdate/c356e63d-a11f-410e-860b-a04bc3e78f81\"\x01\x01\x38\{\"Topic\":\"requestnewfirmware\",\"RequestId\":\"1000\",\"DeviceSlNo\":\"c356e63d-a11f-410e-860b-a04bc3e78f81\",\"RequestedFileNumber\":\"133\",\"Data\":\"\",\"CurrentVersion\":\"v1.27\",\"NewVersion\":\"T1.111\",\"Type\":\"Firmware\",\"UserId\":\"ajaykumar636363@gmail.com\",\"UserToken\":\"2b7d28f8-a402-4449-acaf-692a2a211b69\",\"AppVersion\":\"V1.2\"}\r\n"

 	  snprintf((char*)wifiMsgBuff1,sizeof(wifiMsgBuff1),"\"Topic\":\"updateuser\",\"UserId\":\"%s\",\"EmailId\":\"%s\",\"MobileNumber\":\"8790812997\",\"DeviceSlNo\":\"%s\",\"HardwareSlNo\":\"HCHE01\",\"FreezerId\":\"01\",\"Temp1Low\":\"-200\",\"Temp1High\":\"50\",\"Temp2Low\":\"-200\",\"Temp2High\":\"50\",\"UIVersion\":\"1.0\",\"FirmwareVersion\":\"1.0\",\"AppVersion\":\"1.0\"}",\
 	  (char*)&comm_par.sy_par.iot.User_ID,(char*)&comm_par.sy_par.iot.iot_email,(char*)&comm_par.sy_par.iot.iot_phone,\
 	  (char*)&Plumbing_options.Iot_token,(char*)&Plumbing_options.IOT_serial,(char*)&Plumbing_options.serial,(char*)UIFreezerStruct.FreezerID,\
 	  vp_param_rw_str.low_temp1.value ,vp_param_rw_str.high_temp1.value,vp_param_rw_str.low_temp2.value ,vp_param_rw_str.high_temp2.value,Control_versions.UI_version,APP_FW_VER,APP_VER);
 	  /*snprintf((char*)wifiMsgBuff1,sizeof(wifiMsgBuff1),"{\"UserId\":\"%s\",\"EmailId\":\"%s\",\"MobileNumber\":\"%s\",\"UserToken\":\"%s\",\"DeviceSlNo\":\"%s\",\"FreezerId\":\"%s\"}",\
 	  (char*)&comm_par.sy_par.iot.User_ID,(char*)&comm_par.sy_par.iot.iot_email,(char*)&comm_par.sy_par.iot.iot_phone,\
 	  (char*)&Plumbing_options.Iot_token,(char*)&Plumbing_options.IOT_serial,(char*)UIFreezerStruct.FreezerID);*/
 	  strcpy((char*)&wifi_tx_bfr[29],(char*)&wifiMsgBuff1);
 }

 void update_device_logs()
 {
 	uint8_t serial_data[36]={0};
 	strcpy((char*)&serial_data,(const char*)&Plumbing_options.IOT_serial);
 	for(uint8_t i=0;i<MAX_ERROR_FLAGS;i++)
 	{
 		if((Log_data_buff.Error_buff[i][0] !=0 ))
 		{
 			update_time_stamp();
 			memset(&wifiMsgBuff1[0], 0x00, sizeof(wifiMsgBuff1));
 			memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));

 			if((wifi_tx_bfr1[6]==0) && (comm_par.sy_par.iot.User_ID[0] !=0x00) && (Plumbing_options.IOT_serial[0] != 0x00) && (Plumbing_options.IOT_serial[0] != 0xFF) )
 			{
 				snprintf((char*)wifiMsgBuff1,sizeof(wifiMsgBuff1), "{\"Topic\":\"updatedevicelog\",\"DeviceSlNo\":\"%s\",\"Temperature1\":\"%.1f\",\"Temperature2\":\"%.1f\",\"TimeStamp\":\"%s\",\"EventType\":\"%s\",\"EventCode\":\"%s\",\"Ln2Usage\":\"%.1f\",\"UserId\":\"%s\",\"UserToken\":\"%s\",\"FreezerId\":\"%s\",\"AppVersion\":\"%s\"}",\
 				(char*)&Plumbing_options.IOT_serial,vp_rt_wr_str.temp1,vp_rt_wr_str.temp2,(char*)wifiMsgBuff,(char*)EventTYPE[i],\
 				(char*)&Log_data_buff.Error_buff[i],Ln2_usage.datausage,(char*)&comm_par.sy_par.iot.User_ID,(char*)&Plumbing_options.Iot_token,(char*)&UIFreezerStruct.FreezerID,APP_VER);
 				memset(&Log_data_buff.Error_buff[i][0], 0x00, sizeof(Log_data_buff.Error_buff[i]));
 				strcpy((char*)&wifi_tx_bfr1[6],(char*)&wifiMsgBuff1);
 				iot_info.data_available=0;
 				if(transmit_to_iot==0)
 				{
 					strcpy((char*)&wifi_tx_bfr[6],(char*)&wifi_tx_bfr1[6]);
 					iot_info.file_open_data=0;
 					New_error_ind=0;
 				}
 				transmit_to_iot=1;
 				if((wifi_tx_bfr1[6]!=0))
 				{
 					break;
 				}
 			}
 			else
 			{
 				if(iot_info.file_open_data==1)
 				{
 					memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
 					memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
 				}
 				//.IOT_update_SDCARD();
 			}

 		}
 	}
 	if(iot_info.data_available)
 	{
 		update_time_stamp();
 		memset(&wifiMsgBuff1[0], 0x00, sizeof(wifiMsgBuff1));
 		memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));

 		if((wifi_tx_bfr1[6]==0) && (comm_par.sy_par.iot.User_ID[0] !=0x00) && (Plumbing_options.IOT_serial[0] != 0x00) && (Plumbing_options.IOT_serial[0] != 0xFF) )
 		{
// 			snprintf((char*)wifiMsgBuff1,sizeof(wifiMsgBuff1), "{\"Topic\":\"updatedevicelog\",\"DeviceSlNo\":\"%s\",\"Temperature1\":\"%.1f\",\"Temperature2\":\"%.1f\",\"TimeStamp\":\"%s\",\"EventType\":\"L\",\"EventCode\":\"\",\"Ln2Level\":\"%.1f\",\"Ln2Usage\":\"%.1f\",\"UserId\":\"%s\",\"UserToken\":\"%s\",\"FreezerId\":\"%s\",\"AppVersion\":\"%s\"}",\
// 				(char*)&Plumbing_options.IOT_serial,vp_rt_wr_str.temp1,vp_rt_wr_str.temp2,(char*)wifiMsgBuff,\
// 				vp_rt_wr_str.dp_level,Ln2_usage.datausage,(char*)&comm_par.sy_par.iot.User_ID,(char*)&Plumbing_options.Iot_token,(char*)&UIFreezerStruct.FreezerID,APP_VER);

 			strcpy((char*)&wifi_tx_bfr1[6],(char*)&wifiMsgBuff1);
 			iot_info.data_available=0;
 		}
 		else
 		{
 			if(iot_info.file_open_data==1)
 			{
 				memset(&wifi_tx_bfr[0], 0x00, sizeof(wifi_tx_bfr));
 				memset(&wifi_tx_bfr1[0], 0x00, sizeof(wifi_tx_bfr1));
 			}
 			//IOT_update_SDCARD();
 		}

 		if(transmit_to_iot==0)
 		{
 			strcpy((char*)&wifi_tx_bfr[6],(char*)&wifi_tx_bfr1[6]);
 			iot_info.file_open_data=0;
 			New_error_ind=0;
 		}
 		transmit_to_iot=1;
 	}
 	else if(wifi_tx_bfr1[6]== '{')
 		{
 			if(transmit_to_iot==0)
 			{
 				strcpy((char*)&wifi_tx_bfr[6],(char*)&wifi_tx_bfr1[6]);
 				iot_info.file_open_data=0;//need to change
 				New_error_ind=0;
 			}
 			transmit_to_iot=1;
 		}
   }

 void update_time_stamp()
 {

 	if(Time.TimeFormat)
 	{
 		if(Time.Hours<12)
 		{
 			wifi_hour=Time.Hours+12;
 		}
 		else
 		{
 			wifi_hour=Time.Hours;
 		}
 	}
 	else
 	{
 		if( Time.Hours==12)
 		{
 			wifi_hour=0;
 		}
 		else
 		{
 			wifi_hour=Time.Hours;
 		}
 	}

 	memset(&wifiMsgBuff[0], 0x00, sizeof(wifiMsgBuff));

     sprintf((char*)wifiMsgBuff, "%02d/%02d/%02d %02d:%02d:%02d",\
     		Date.Date,Date.Month,Date.Year, \
 									 wifi_hour,Time.Minutes,Time.Seconds);
 }

 void Icon_status(void)
 {
 	if(firmware_timer_en)
 	{
 		 if(Control_versions.Firmware_avb_icon==0)
 		 {
 			 Control_versions.Firmware_avb_icon=1;
 			 SendN16DataToDisplay(IOT_FIRM_AVAIL_EN, Control_versions.Firmware_avb_icon);
 		 }
 		 if(Control_versions.IOT_File_status != 3)
 		 {
 			 Control_versions.IOT_File_status=3;
 			 SendN16DataToDisplay(IOT_FILE_STATUS, Control_versions.IOT_File_status);
 		 }
 		 if(Control_versions.Display_download_icon)
 		 {
 			 Control_versions.Display_download_icon=0;
 			 SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);
 		 }
 	}
// 	else if(SD_info.update_page_flag)
// 	{
// 		 if(Control_versions.Firmware_avb_icon==0)
// 		 {
// 			 Control_versions.Firmware_avb_icon=1;
// 			 SendN16DataToDisplay(IOT_FIRM_AVAIL_EN, Control_versions.Firmware_avb_icon);
// 		 }
// 		 if(Control_versions.IOT_File_status != 2)
// 		 {
// 			 Control_versions.IOT_File_status=2;
// 			 SendN16DataToDisplay(IOT_FILE_STATUS, Control_versions.IOT_File_status);
// 		 }
// 		 if(Control_versions.Display_download_icon)
// 	     {
// 		     Control_versions.Display_download_icon=0;
// 		     SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);
// 	     }
// 	}
// 	else if(Control_versions.Firmware_avb_icon && (vp_rt_wr_str.WiFiStatus != 1))
// 	{
// 		 Control_versions.Firmware_avb_icon=0;
// 		 SendN16DataToDisplay(IOT_FIRM_AVAIL_EN, Control_versions.Firmware_avb_icon);
// 		 Control_versions.Display_download_icon=0;
// 		 SendN16DataToDisplay(IOT_FIRM_DOWN_EN, Control_versions.Display_download_icon);
// 	}
// 	else if((Control_versions.UI_version_avb || Control_versions.Firmware_version_avb) && vp_rt_wr_str.WiFiStatus==1)
// 	{
// 		 if((Control_versions.Firmware_avb_icon==0))
// 		 {
// 			 Control_versions.Firmware_avb_icon=1;
// 			 SendN16DataToDisplay(IOT_FIRM_AVAIL_EN, Control_versions.Firmware_avb_icon);// need to do change
// 		 }
// 		 if(Control_versions.IOT_File_status != 0)
// 		 {
// 			 Control_versions.IOT_File_status=0;
// 			 SendN16DataToDisplay(IOT_FILE_STATUS, Control_versions.IOT_File_status);
// 		 }
//	}
 }


