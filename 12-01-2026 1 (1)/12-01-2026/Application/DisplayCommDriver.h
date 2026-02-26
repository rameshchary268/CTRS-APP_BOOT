/*
 * displaycommdriver.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef INC_DISPLAYCOMMDRIVER_H_
#define INC_DISPLAYCOMMDRIVER_H_

#include "stdint.h"

#define MAX_CIRBUFF_SIZE		0x02000  //1024
#define CIRBUFF_SIZE_MASK		0x1FFF  //1023
#define MAX_UART_BUFF_SIZE		1024
#define MAX_NO_RX_BUFFERS		2
#define UART_BUFF_SIZE_MASK     0X3FF

typedef enum {
	CIRBUFF_EMPTY,
	CIRBUFF_FULL,
	CIRBUFF_OVFLOW,
	CIRBUFF_UNFLOW
} CirBuffStates;

typedef struct {
	uint16_t WrPtr;
	uint16_t RdPtr;
	CirBuffStates State;
	uint8_t CBuff[MAX_CIRBUFF_SIZE];
} CirBuffStruct;

void InitLCDCirBuffer(void);
void LCDDataProc(void);
void ProcessLCDData();
uint16_t WriteDataToTxBuff(uint8_t *DataPtr, uint16_t Size);

#endif /* INC_DISPLAYCOMMDRIVER_H_ */
