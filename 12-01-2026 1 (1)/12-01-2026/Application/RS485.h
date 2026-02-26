/* 
 * File:   RS485.h
 * Author: Ilensys
 *
 * Created on 14 June, 2022, 5:00 PM
 */

#ifndef RS485_H
#define	RS485_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Includes */
#include "MyMain.h"

/* Declaration of constant defines and typedefs */
#define UART_FIFO_SIZE        8        /* FIFO size (Tx and Rx) in number of bytes */
#define UART_BUFFER_SIZE      256      /* Buffer size in number of bytes */

#define RS485_MASTER_NODE_ID  0x00     /* 0x00 for the master node */
#define RS485_NODE_ADDRESS    (RS485_MASTER_NODE_ID)  /* 0x00 for the master node and 0x01 to 0x7F for slave nodes */
#define RS485_MODE_0          0        /* No RS485 communication */
#define RS485_MODE_1          1        /* 9600B-8N-1S-NP   */
#define RS485_MODE_2          2        /* 19200B-8N-1S-NP  */
#define RS485_MODE_3          3        /* 34600B-8N-1S-NP  */

#define RS485_RECEIVE         (DRV_OFF)
#define RS485_TRANSMIT        (DRV_ON)
#define RX_TIMEOUT_VAL        (UART_BUFFER_SIZE*11)  /* Rx timeout value in terms of number of bits during which there is no activity */

#define APPLICATION_4_20mA_ADD 0x08009000

#define IOT_CIR_BUF_DEPTH       16
#define IOT_CIR_BUF_SIZE        256
#define UART_CIR_BUF_SIZE       550
/* Declaration of enums and typedefs */   \


typedef enum {
  RS485_INIT,
  RS485_TX_STRT,
  RS485_TX_WAIT,
  RS485_TX,
  RS485_TX_DONE,
  RS485_RX_STRT,
  RS485_RX,
  RS485_RX_DONE,
  RS485_ERR,
} RS485_STATE;

typedef struct {
 RS485_STATE tx_state;
 RS485_STATE rx_state;
 uint8_t  polling_state;
 uint32_t    poll_flag;
 uint32_t    dly_flag;
 uint32_t    resp_flag;
 uint32_t    tx_size;
 uint32_t    rx_size;
 uint32_t    tx_bytes;
 uint32_t    rx_bytes;
 uint32_t    mA_file_ptr;
 uint32_t    mA_communication;
 uint8_t     *txBuf;
 uint8_t     *rxBuf;
 uint8_t     tx_done;
 uint8_t     rx_done;
 uint8_t     node_address;
 uint8_t     data_request;
 uint8_t     Poll_4_20;
 uint8_t     File_fail;
 uint8_t     Firmware_4_20;
 uint8_t     UART3RxErrorDetected;
} RS485_INFO;

#define RS485_INFO_DEFAULT (RS485_INFO) {RS485_INIT, RS485_RX_DONE,0, 1, 1, 1, 0, 0, 0, 0,0,0, NULL, NULL, 0, 0, 0,0,0,1,0,0}

typedef enum {
  SEQUENTIAL_FILL,  // First of the OFAF modes
  CONCURRENT_FILL,  // Second of the OFAF modes
  AUTO_FILL,        // Stand alone mode
} OFAF_MODE;



typedef struct NODE_INFO {
 uint8_t version[8];
 uint8_t   active;
// OFAF_MODE ofaf_mode;
 uint8_t   fill_comp_sts;
 uint8_t   stp_fil_lvl;
 uint8_t   fil_sts;
 uint8_t   errors;
 uint8_t   Group_inactive;
 float   level;
} NODE_INFO;


typedef struct  {
 float PREV_dp_level;
 float PREVRTD1_temp;
 float PREVRTD2_temp;
 uint8_t    prev_mod_st;
 uint8_t master_fill;
 uint8_t prev_seq_fill;
 uint8_t prev_active_nodes;
} PREV_INFO;

#define NODE_INFO_DEFAULT (NODE_INFO) {0, AUTO_FILL, 0, 0, 0, 0, }

/* Function Declarations */
void   RS485_init(void);
uint8_t Slave_with_polling(void);
void   RS485_master_proc(void);
void   RS485_Dir(DRV_STATE state);
int8_t RS485_slave_resp(uint8_t *resp, uint16_t len);
uint8_t polling_to_4_20mA(uint8_t j);
uint8_t respons_from_4_20mA(uint8_t j);
void   RS485_tx_proc(void);
void   RS485_rx_proc(void);
void   Uart3_TxDone_callback(UART_HandleTypeDef *huart);
void   Uart3_RxDone_callback(UART_HandleTypeDef *huart);
void   Uart3_Error_callback (UART_HandleTypeDef *huart);
void   RS485_FILL_process(uint8_t i);
uint8_t Master_without_schedule_fill_with_group(uint8_t value);
uint8_t Master_without_schedule_fill_without_group(uint8_t value);
uint8_t Master_with_Group(uint8_t value);
uint32_t Analog_value_4_20(uint8_t channel,uint8_t sensor);

#ifdef	__cplusplus
}
#endif

#endif	/* RS485_H */

