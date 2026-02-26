/*
 * BLE.h
 *
 *  Created on: Sep 15, 2025
 *      Author: SaikrishnaKarutu
 */

#ifndef BLE_H_
#define BLE_H_

#include "main.h"
#include "Delay.h"

#define MAX_BLE_UUID                     21
#define MAX_BLE_UUID_LEN                 95

typedef struct{
	uint8_t UUID[95];
	uint8_t Authenication_type;
	uint16_t UUID_signal;
}BLE_SSID_Details;

typedef enum {
	  BLE_RESET,
	  BLE_INIT,
	  BLE_STRT,
	  BLE_SET_MODE,
	  BLE_CHECK_NAME,
	  BLE_SCAN,
	  BLE_CONNECT,
	  BLE_CONNECTED,
	  BLE_CHAR_DISCOVER,
	  BLE_CONNECTION_DETAILS,
	  BLE_READ_DATA,
	  BLE_DATA_READ_SUCCESS,
	  BLE_STRENGTH,
	  BLE_COMM_FAIL,
	  BLE_IDLE,
} BLE_STATE;

typedef enum {
  BLE_CMD_MODE,
  BLE_DATA_MODE,
  BLE_XDATA_MODE,
} BLE_MODE;

typedef struct {
  BLE_STATE  state;
  BLE_STATE  Prevstate;
  BLE_MODE   mode;
  uint32_t    dly_flag;
  int16_t     handle;
//  uint32_t    poll_flag;
  uint8_t     fall_back;
  uint8_t     UART2ErrorDetected;
  uint8_t     RxCplt;
  uint8_t     TxCplt;
} BLE_INFO;

#define BLE_INFO_DEFAULT (BLE_INFO) {BLE_IDLE,BLE_INIT, BLE_CMD_MODE, 0, 0,0,0,0 }

void BLE_init(void);
void BLE_Process();
#endif /* BLE_H_ */
