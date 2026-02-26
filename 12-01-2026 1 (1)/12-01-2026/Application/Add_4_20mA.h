/*
 * Add_4_20mA.h
 *
 *  Created on: Jun 25, 2025
 *      Author: VenkateshArikanti
 */

#ifndef ADD_4_20MA_H_
#define ADD_4_20MA_H_

typedef struct
{
	uint8_t  serial_no[20];
	uint8_t  hardware_ver[6];
	uint8_t  software_ver[6];
}INFO_4_20mA;

typedef struct{
	int8_t Firmware_update_en;
	int8_t Frmware_4_20_mA_update;
} UpdateFIRMWAREStruct;

typedef struct{
	int16_t Slave_Ter_switch;
	int16_t Master_Ter_switch;
	int16_t EXD_MD_Switch;
	int16_t Chan1_Switch;
	int16_t Chan2_Switch;
	int16_t Chan1_Selction;
    int16_t Chan2_Selction;
    int16_t Ch1_Source_sel;
	int16_t Chan1_vol_cur;
	int16_t Chan2_vol_cur;
	int16_t Chan3_Switch;
	int16_t Chan4_Switch;
	int16_t Chan3_Selction;
    int16_t Chan4_Selction;
    int16_t Ch2_Source_sel;
} Network_4_20mAStruct;

typedef struct{
	int16_t MAX_CONFIG_CH1;
	int16_t MIN_CONFIG_CH1;
	int16_t MAX_CONFIG_CH2;
	int16_t MIN_CONFIG_CH2;
	int16_t MAX_CONFIG_CH3;
	int16_t MIN_CONFIG_CH3;
	int16_t MAX_CONFIG_CH4;
	int16_t MIN_CONFIG_CH4;

} Network_CONFIG_Struct;

void Handle4_20mA_firmwareFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void Handle4_20mAFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
//void Handle4_20mA_CONFIG_Functions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void Handle4_20mATERFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void Handle4_20mA_3wireFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void PassiveLimitCheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Config4_20mALimitCheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void SEL4_20mACheck(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
#endif /* ADD_4_20MA_H_ */
