/*
 * MsgProc.h
 *
 *  Created on: 24-Aug-2022
 *      Author: Ilensys
 */

#ifndef MSGPROC_H_
#define MSGPROC_H_

/* Includes */
#include "MyMain.h"


/* Declaration of constant defines and typedefs */
#define DEF_CH_FOR_DAT_LOG      (COMM_SD)      // TBD
#define DEF_CH_FOR_ERR_LOG      (COMM_SD)      // TBD
#define DEF_CH_FOR_EVT_LOG      (COMM_SD)      // TBD
#define DEF_CH_FOR_STS_LOG      (COMM_SD)      // TBD
#define DEF_CH_FOR_WRN_LOG      (COMM_SD)      // TBD

#define DEF_CH_TO_SEND_LOG      (COMM_ETH)
#define MODBUS_PDU_BUF_SIZE     (255-2)

#define IOT_CIR_BUF_DEPTH       16
#define IOT_CIR_BUF_SIZE        256
#define UART_CIR_BUF_SIZE       550
#define MAX_ATTEMPTS            2

// Communication related
#define HEADER_ACK_MSG          0xB1
#define HEADER_NACK_MSG         0xB2
#define HEADER_ORG_MSG          0xB3

////////////////////////////////////////////////////////////////////////
// Ethernet / Modbus commands - Reserve IDs from 0-247 (0xF8 - 0xFF)
////////////////////////////////////////////////////////////////////////
// COMMON
#define DELETE_ALL_USER_ACC     0x39        // Clear all the user accounts (Admin & Secure User)
#define WRITE_RTC_SYNC_UP       0x40        // Synchronize with the master clock of host
// MODBUS ONLY (NON-SINTESY)
#define CMD_4_20mA_POLL         0x05
#define CMD_OFAF_FILL_STRT      0x41        // Start OFAF fill command to slave
#define CMD_OFAF_FILL_STOP      0x42        // Stop OFAF fill command to slave
#define CMD_SEND_LOG_FILE       0x56        // Send the log files
#define CMD_4_20mA_FIRM_RE      0XF0
#define CMD_4_20_FIRM_UP        0xF1
#define CMD_4_20_FIRM_STATUS    0xF2
#define CMD_POLL                0xF6        // OFAF mode request from slave
#define CMD_RD_DEVICE_INFO      0xF7        // Device info. Can also be used for polling
#define CMD_ERROR_F6_4_20mA     0xFE
// ETHERNET ONLY
#define CMD_FWDL_CU_HEADER      0xF8        // Lead packet for the firmware download to Control Unit
#define CMD_FWDL_CU_TAIL        0xF9        // Trail packet for the firmware download to Control Unit
#define CMD_FWDL_CU_MIDDLE      0xFA        // Middle packet for the firmware download to Control Unit
#define CMD_FWDL_UI_HEADER      0xFB        // Lead packet for the firmware download to UI Unit
#define CMD_FWDL_UI_TAIL        0xFC        // Trail packet for the firmware download to UI Unit
#define CMD_FWDL_UI_MIDDLE      0xFD        // Middle packet for the firmware download to UI Unit
//#define CMD_PARAMS_DOWNLOAD     0xFE        // Parameters download
#define CMD_TEST_JIG_MODE       0xFF        // Test jig mode

// OFAF internal commands
#define OFAF_STOP_FILL          0x00
#define OFAF_STRT_FILL          0x01
#define OFAF_CMD_DONE           0x02

//////////////////////////////////////////////////////////
// General commands applicable for all communication modes
//////////////////////////////////////////////////////////
#define CMD_SEND_SYS_PARAM      0x30
#define CMD_SEND_PRS_PARAM      0x31
#define CMD_REVC_SYS_PARAM      0x32
#define CMD_RECV_PRS_PARAM      0x33
#define CMD_SYS_SHUTDOWN        0x34
#define CMD_SLEEP_FOR_DURN      0x35
#define CMD_SLEEP_FOR_RST       0x36
#define CMD_DISCON_FOR_RST      0x37
#define CMD_DISCON_FOR_DURN     0x38
#define CMD_SOFT_RESET          0x39
#define CMD_POLLLING_MSG        0x3A
#define CMD_SEND_RT_DATA        0x3B
#define CMD_SEND_EVENT_FLAG     0x3C
#define CMD_SEND_ERROR_FLAG     0x3D
#define CMD_SEND_WARNING_FLAG   0x3E
#define CMD_SEND_PARAMETERS     0x3F
#define CMD_SYNC_DATE_TIME      0x40
#define CMD_SEND_LID_STATUS     0x41
#define CMD_OPEN_LID            0x42
#define CMD_CLOSE_LID           0x43
#define CMD_SEND_DEFOG_STATUS   0x44
#define CMD_ENABLE_QUICKCHILL   0x45
#define CMD_SEND_DATA_BATT      0x46
#define CMD_SEND_DATA_MAINS     0x47
#define CMD_SEND_DATA_FLOW1     0x48
#define CMD_SEND_DATA_FLOW2     0x49
#define CMD_SEND_DATA_TEMP1     0x4A
#define CMD_SEND_DATA_TEMP2     0x4B
#define CMD_SEND_DATA_DP_LVL    0x4C
#define CMD_SEND_DATA_TH_LVL    0x4D
#define CMD_SEND_DATA_LID       0x4E
#define CMD_SEND_DATA_DEFOG     0x4F
#define CMD_SEND_DATA_QICKCHIL  0x50
#define CMD_SEND_ERROR_LOG      0x51
#define CMD_SEND_EVENT_LOG      0x52
#define CMD_SEND_WARNING_LOG    0x53
#define CMD_SEND_STATUS_LOG     0x54
#define CMD_SEND_DATA_LOG       0x55
#define CMD_SEND_LOG_FILE       0x56

/* Declaration of enumerators and typedefs */
typedef enum {
  BUF_EMPTY,
  BUF_FULL,
  BUF_BUSY,
  BUF_ERR,
} BUF_STATE;

typedef struct {
  BUF_STATE  state;
  uint8_t    buffer[UART_CIR_BUF_SIZE];
  int32_t    size;
  uint8_t    attempt;
} CIR_BUF;

#define CIR_BUF_DEFAULT (CIR_BUF) {BUF_EMPTY, {0, }, 0, 0}

typedef struct {
  BUF_STATE  state;
  uint64_t    buffer[IOT_CIR_BUF_SIZE/8];
  int32_t    size;
  uint8_t    attempt;
} SD_CIR_BUF;

#define SD_CIR_BUF_DEFAULT (SD_CIR_BUF) {BUF_EMPTY, {0, }, 0, 0}

typedef struct {
  BUF_STATE  state;
  uint32_t   offset;
  int16_t    size;
  uint8_t    flag;
  uint8_t    *ptr;
} WIFI_RESP_BUF;

#define WIFI_RESP_BUF_DEFAULT (WIFI_RESP_BUF) {BUF_EMPTY, 0, 0, 0, NULL}

typedef enum {
  COMM_ETH,
  COMM_WIFI,
  COMM_RS485,
  COMM_SD,
  COMM_RAM,
  COMM_FLASH,
} COMM_MODE;

typedef enum {
  MSG_DAT,
  MSG_ERR,
  MSG_EVT,
  MSG_CMD,
  MSG_WRN,
  MSG_PAR,
  MSG_POLL,
} MSG_TYPE;

typedef enum {
  CAT_ORG,     // Originated message, not as response to any command
  CAT_ACK,
  CAT_NACK,
  CAT_MB,      // Modbus message
} CAT_INFO;

typedef  enum {
  TXN_READ,
  TXN_WRITE,
} TXN_DIR;

// LN2 temperature (2-bytes), Battery volt (2-bytes) and LN2 level (1-byte)
typedef struct {
  float batt_volt;
  float rtd1_temp;
  float rtd2_temp;
  float dp_level;
  float thr_level;
} RT_DATA;

typedef struct {
  uint32_t image_size;
  uint16_t packet_size;
  uint16_t img_checksum;
  char     file_name[16];
} FOTA_STR;

#define FOTA_STR_DEFAULT (FOTA_STR) {0, 0, 0, {0, }, }

typedef struct {
  // First packet contains image information - Image size, Current packet number (0 - this packet, >0 - Packet order received,
  // number of bytes in each packet (including any padded bytes at the last packet) & checksum of the image.
  uint8_t  img_id;
  uint8_t  pkt_rem;
  uint16_t pkt_size;
  uint32_t img_size;
  uint16_t crc16;
} IOT_FOTA_MSG;

typedef enum
{
  DRV_OFF,
  DRV_ON,
  DRV_TOGGLE,
} DRV_STATE;

/* Function Declarations */
void     Msg_init();
// Send messages (if any)
CIR_BUF* Get_cir_buf(TXN_DIR dir, COMM_MODE mode);
int8_t   Find_Tx_buf(BUF_STATE state, COMM_MODE mode);
uint8_t  Add_Timestamp(uint8_t *msg);
int8_t   Push_Message(uint8_t *msg, uint8_t len, COMM_MODE mode, MSG_TYPE type, CAT_INFO cat_info, uint8_t cmd);
// Receive and process messages (if any)
void     Process_Message(void);
int8_t   Find_Rx_buf(BUF_STATE state, COMM_MODE mode);

int8_t   Process_Modbus_Response(uint8_t *msg, uint16_t len);
int8_t   Process_Modbus_Command(uint8_t *msg, uint16_t len);
uint8_t modebus_error();

#endif /* MSGPROC_H_ */
