/*
 * WIFI.h
 *
 *  Created on: Nov 17, 2025
 *      Author: VenkateshArikanti
 */

#ifndef WIFI_H_
#define WIFI_H_

/* Includes */
#include "MyMain.h"

/* Declaration of constant defines and typedefs */
#define WPA2_3                  6     // 1:Open, 2:WPA/WPA2/WPA3,  3:LEAP, 4:PEAP, 5:EAP-TLS, 6:WPA2/WPA3 (WPA disabled), 7:WPA3 (WPA/WPA2 disabled)

#define Controller_file_name    "0:/controller.bin"
#define UI_file_name            "0:/Display.bin"

#define WIFI_MTU                0x400
#define EMMC_BUF_SIZE            0x400        // Working buffer size 1KB
#define MAX_SSIDS                    11
#define MAX_SSID_LEN                 64

#define UART_INFO_DEFAULT (UART_INFO) {UART_INIT, UART_RX_DONE, 1, 1,0, 0, 0, 0, 0, NULL, NULL, 0, 0}

#define MAX_UID_SIZE      32
#define MAX_PWD_SIZE      32
#define MAX_WIFI_CFG_ID   5
#define UUID_LENGTH       16
#define MAX_PSWD_LENGTH   12
#define  MAX_WIFI_CH      (MAX_WIFI_CFG_ID)
#define CLIENT_ID_LENGTH  30

// WiFi network access credentials - default
// Network-1
#define UUID1             ""     // TBD
#define PSWD1             ""     // TBD
#define CFG_1             "2"
// Network-2
#define UUID2             "      "     // TBD
#define PSWD2             "      "     // TBD
#define CFG_2             2
// Network-3
#define UUID3             "      "     // TBD
#define PSWD3             "      "     // TBD
#define CFG_3             2
// Network-4
#define UUID4             "      "     // TBD
#define PSWD4             "      "     // TBD
#define CFG_4             2



#define WIFI_CFG_DEFAULT (WIFI_CFG) {    \
  IOT_CLIENT_ID, \
  {UUID1},  \
  {PSWD1},  \
  {{UUID1}, {UUID2}, {UUID3}, {UUID4}, {UUID4}},  \
  {{PSWD1}, {PSWD2}, {PSWD3}, {PSWD4}, {PSWD4}},  \
  {{CFG_1}, {CFG_2}, {CFG_3}, {CFG_4}, {CFG_4}},  \
  0,  \
}

typedef enum {
	  WIFI_STRT,
	  WIFI_INIT,
	  CHECK_CHIP_UPDATE,
	  WIFI_CHIP_UPDATE,
	  WIFI_BOOT_MODE,
	  WIFI_BOOT_RECOVERY,
	  CHECK_CERT_AVAILABILITY,
	  WIFI_CHECK_VERSION,
	  WIFI_READ_AWS_CERT,
	  WIFI_UPLOAD_AWS_CERT,
	  WIFI_VALIDATE_RCA_CERT,
	  WIFI_VALIDATE_DEVICE_CERT,
	  WIFI_CERT_UPLOAD_DONE,
	  WIFI_CERT_UPLOAD_SUCCESS,
	  WIFI_SCAN,
	  WIFI_IDLE,
	  WIFI_LOGIN,
	  WIFI_LOGIN_CHECK,
	  SAVE_WIFI_SSID,
	  WIFI_CONNECTION_DETAILS,
	  WIFI_MONITOR_CONN_STS,
	  WIFI_MONITOR_RSSI,
	  WIFI_STRENGTH,
	  WIFI_SEND_SIGSTRENGTH,
	  WIFI_DAT_TX,
	  WIFI_DAT_RX,
	  WIFI_DAT_RX_DONE,
	  WIFI_DISCONNECT_IOT,
	  WIFI_DISCONNECT,
	  WIFI_DISCONNECTED,
	  WIFI_MONITOR_WIFI_DISCONNECT,
	  WIFI_RESET,
	  WIFI_ERR,
	  WIFI_COMM_FAIL,
	  WIFI_STOP,
	  WIFI_STOP_DONE,
	  WIFI_HALT,
	  WIFI_RST,
	  MQTT_CONNECTION,
	  TLS_CONNECTION,
      CONNECT_MQTT,
	  SUBSCRIBE_TOPIC,
	  PUBLISH_MESSAGE,
	  READ_MESSAGE,
	  READ_MESSAGE_SUCCESSFUL,
	  CERTIFICATES_AVAILABLE,
	  CONNECT_MQTT_CMD,
	  MQTT_CONNECTED,
	  WIFI_CONN_MNGMNT,
	  DELETE_CERT,
} WIFI_STATE;

typedef enum {
  WIFI_CMD_MODE,
  WIFI_DATA_MODE,
  WIFI_XDATA_MODE,
} WIFI_MODE;

typedef struct {
  WIFI_STATE  state;
  WIFI_STATE  Prevstate;
  WIFI_MODE   mode;
  uint32_t    dly_flag;
 // int16_t     handle;
  uint8_t     Wifi_cert_available;
  uint8_t     wifi_firmware_available;
  uint8_t     fall_back;
  uint8_t     UART1ErrorDetected;
  uint8_t     RxCplt;
  uint8_t     TxCplt;
} WIFI_INFO;

typedef struct {
  float rtd1_temp;
  float rtd2_temp;
  float dp_level;
  float therm_level;
  uint8_t time_stamp;
} REAL_DATA;
typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint16_t year;
  uint8_t tenth_of_second;
  uint8_t timeformat;
} TIME_VAL;


#define WIFI_INFO_DEFAULT (WIFI_INFO) {WIFI_STRT,WIFI_INIT, WIFI_CMD_MODE, 0, 0,0,0,0 }

typedef enum {
  UART_INIT,
  UART_TX_STRT,
  UART_TX,
  UART_TX_DONE,
  UART_RX_STRT,
  UART_RX,
  UART_RX_DONE,
  UART_ERR,
} UART_STATE;

typedef struct {
 UART_STATE  tx_state;
 UART_STATE  rx_state;
 uint32_t    poll_flag;
 uint32_t    dly_flag;
 uint32_t    resp_flag;
 uint32_t    tx_size;
 uint32_t    rx_size;
 uint32_t    tx_bytes;
 uint32_t    rx_bytes;
 uint8_t     *txBuf;
 uint8_t     *rxBuf;
 uint8_t     tx_done;
 uint8_t     rx_done;

} UART_INFO;

//#define SOH 0x01
//#define EOT 0x04
//#define ACK 0x06
//#define NAK 0x15
//#define CAN 0x18
//#define ESC 0x1B
//
//#define PACKET_SIZE 128


typedef struct {
 uint32_t  poll_flag;
 uint32_t  wait_flag;
 uint32_t  offset;
 int32_t   file_size;
 uint8_t   bin_present;
 uint8_t    buffer[2048];
} XMODEM_INFO;

typedef struct{
	uint8_t SSid[64];
	uint16_t SSid_signal;
	uint8_t Authenication_type;
}SSID_Details;

typedef struct{
	uint32_t SSID_add_WR;
	uint32_t SING_add_WR;
}SSID_REGISTERS;


typedef struct {

 uint32_t  poll_flag;
 uint8_t   semaphore;
 uint32_t  wait_flag;
 uint32_t  bin_chk_flag;
// PEER_ID   peer_id;
 uint32_t  offset;
 int32_t   file_size;
 uint8_t   bin_present;
 uint8_t   init_done;
 uint8_t   boot_present;
} EMMC_INFO;

#define XMODEM_INFO_DEFAULT (XMODEM_INFO) {1,1, 1, 1,0,{0,}}
#define EMMC_INFO_DEFAULT (EMMC_INFO) {1,1, 1, 1, 0, EMMC_BUF_SIZE,0}

typedef struct{
	 uint8_t Total_files[5];
	 uint8_t UI_Update_Version[15];
     uint8_t FIRM_Update_Version[15];
     uint8_t UI_version[15];
     uint8_t Wifi_chip_version[10];
     uint8_t Firmware_version_avb;
     uint8_t UI_version_avb;
     uint8_t Firmware_downloaded;
     uint8_t Display_downloaded;
     uint8_t Display_download_icon;
     uint8_t Firmware_avb_icon;
     uint8_t IOT_File_status;
     uint8_t File_check;
}VERSIONS;

typedef struct {
	uint8_t wifi_connect_attempts;
	uint8_t IOT_connect_attempts;
	uint32_t nof_iot_logs_missed;
	uint32_t nof_iot_acks_missed;
}ATTEMPTS_INFO;

typedef struct{
	uint8_t   Topic_sel;
	uint8_t   data_available;
	uint8_t   Iot_connected;
	uint8_t   logs_ready;
	uint8_t   file_open;
	uint8_t   file_write;
	uint16_t  file_ptr;
	uint8_t   file_open_data;
}IOT_INFO;

#define ATTEMPTS_INFO_DEFAULT (ATTEMPTS_INFO) {0,0,0,0};

#define DLY_WIFI_RST1           5

extern WIFI_INFO  wifi_info;
extern UART_INFO  uart1_info;
extern uint8_t   wifi_rx_copy[10 * WIFI_MTU];
extern uint8_t   wifi_rx_bfr[10*WIFI_MTU];

/* Function declarations */
uint8_t Check_CTS(void);
void Assert_RTS(void);
void Deassert_RTS(void);
void HAL_UART3_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART3_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART3_ErrorCallback(UART_HandleTypeDef *huart);
void WIFI_Process(void);
void Wifi_init(void);
void read_bin_file();
void build_xmodem_packet_1K(uint8_t *payload, uint16_t payload_len, uint8_t pkt_num, uint8_t *out_buf);
int calulatecrc(char *ptr, int count);
void update_device_logs();
void update_user_details();
void update_time_stamp();
void Icon_status(void);

#endif /* WIFI_H_ */
