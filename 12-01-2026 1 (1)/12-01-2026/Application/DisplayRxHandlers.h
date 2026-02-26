/*
 * DisplayRxHandlers.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef DISPLAYRXHANDLERS_H_
#define DISPLAYRXHANDLERS_H_


#include "stdint.h"

typedef void (*DispRxHandler)(uint32_t, uint8_t*, uint16_t);
typedef void (*DispRxAckHandler)(uint32_t);

typedef struct {
	uint32_t VPParamAddr; // VP Address
	uint16_t Size; // Used to compare the recved size to expected one
	DispRxHandler RxHandler; //Handler to be called upon recving the packet
	DispRxAckHandler RespAckHandler;
} DispRxHandlerStruct;

void ParseVPRxData(uint8_t Cmdcode, uint8_t *DataPtr, uint16_t Size);
void DisplayHandshakeRespHandler(uint8_t Cmdcode, uint8_t *DataPtr, uint16_t Size);
void DefaultRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size);
void SytemParamSettingsRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size);
void UserActionEventRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size);
void UserCredentialsRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size);
void SystemCalibrationRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size);
void SystemBISTDataRxHandler(uint32_t VPAddr, uint8_t *DataPtr, uint16_t Size);
uint16_t crc16_calc(unsigned char *q, int len);
#endif /* DISPLAYRXHANDLERS_H_ */
