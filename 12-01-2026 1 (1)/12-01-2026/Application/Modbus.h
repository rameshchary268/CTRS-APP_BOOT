/*
 * Modbus.h
 *
 *  Created on: 04-Oct-2022
 *      Author: Ilensys
 */

#ifndef MODBUS_H_
#define MODBUS_H_

/* Includes */
#include "MyMain.h"

/* Declaration of constant defines and typedefs */
#define EN_SYNTESY_COM1_MODE        // Enable for COM1 mode. Default is COM2 mode
//#define EN_SYNTESY_COM2_MODE   1
//#define EN_SYNTESY_COM2_MODE
#ifndef EN_SYNTESY_COM1_MODE
 #define MAX_HOLD_REG_SIZE      32
#else
 #define MAX_HOLD_REG_SIZE      16
#endif

#define MB_MAX_NODES            32    /* Including master, ID 0x00 is for master, IDs 0x01-0xF7 are for slaves */

#define LEVEL_INCH               0x0000
#define LEVEL_MM                 0x0001
#define TEMP_DEG                 0x0000
#define TEMP_FAR                 0x0100
///////////////////////////
// Sintesy Specific Defines
///////////////////////////

/////////////////////////////////////////////////////////////
// Commands - Modbus supported (Syntesy and PhaseTWO schemes)
/////////////////////////////////////////////////////////////
// Bit access
#define CMD_RD_INT_BIT_SG       0x01        // ***NOT USED ???
#define CMD_WR_INT_BIT_SG       0x05        // ***NOT USED ???
#define CMD_WR_INT_BIT_ML       0x0F        // ***NOT USED ???
#define CMD_RD_DIG_INPUT        0x02        // Used to read Lid status
// 16-bit access
#define CMD_RD_INPUT_REG        0x04        // Used to read temperature and level data
#define CMD_RD_HOLDING_REG      0x03        // Used to read internal holding registers data
#define CMD_WR_SINGLE_REG       0x06        // Used to write single internal holding register
#define CMD_WR_MULTIPLE_REG     0x10        // ***NOT USED ???
#define CMD_RW_MULTIPLE_REG     0x17        // ***NOT USED ???
#define CMD_MASK_WR_REG         0x16        // ***NOT USED ???
#define CMD_READ_FIFO_QUEUE     0x18        // ***NOT USED ???
// File record access
#define CMD_READ_FILE_REC       0x14        // ***NOT USED ???
#define CMD_WRITE_FILE_REC      0x15        // ***NOT USED ???
// Diagnostics
#define CMD_RD_EXCEPTION_STS    0x07        // ***NOT USED ???
#define CMD_DIAGNOSTIC          0x08        // Sub-codes: 0x00-0x12 & 0x14
#define CMD_GET_COM_EVT_CNT     0x0B        // ***NOT USED ???
#define CMD_GET_COM_EVT_LOG     0x0C        // ***NOT USED ???
#define CMD_REPORT_SERVER_ID    0x11        // Used for polling
#define CMD_RD_DEVICE_ID        0x2B        // Sub-code: 0x0E

/* Declaration of enumerators and typedefs */

/* Function Declarations */
void*    MBI_rd_mreg_map(uint16_t sAddr, uint8_t qty);
void*    MBS_rd_mreg_map(uint16_t sAddr, uint8_t qty);
uint16_t Byte_order(int16_t fNum);
uint16_t usMBCRC16(uint8_t *pucFrame, uint16_t usLen);
unsigned short crc16_ccitt_bin(const void *buf, int len, unsigned short start);


#endif /* MODBUS_H_ */
