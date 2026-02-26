/*
 * TouchHandler.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef TOUCHHANDLER_H_
#define TOUCHHANDLER_H_


#define Daily        0x01
#define Monday       0x02
#define Tuesday      0x04
#define Wednesday    0x08
#define Thursday     0x10
#define Friday       0x20
#define Saturday     0x40
#define Sunday       0x80


void TemperatureLimitCheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void TemperatureUnitConvHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void FreezerIdHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void TimeFormatHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void DateLimitHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void MonthDateLimitHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void LevelUnitConvHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void YearDateLimitHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Firm_Update_process(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Firm_4_20_mA_Update_process(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Manual_purge(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void LanguageHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void InchMmCopyHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void TestAlarmCtrl(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Led_britenessCtrl(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void CalibProcessControlHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);

void BinarySwitchHandler(uint32_t VPAddr, int16_t *DataInput, uint8_t *DataPtr, uint16_t Size);
void TempLimitInputHandler(uint32_t VPAddr, int16_t *DataInput, uint8_t *DataPtr, uint16_t Size);
void RefreshGraph(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void RefreshDayGraph(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
#endif /* TOUCHHANDLER_H_ */
