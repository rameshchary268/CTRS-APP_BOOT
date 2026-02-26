/*
 * MAX11206.h
 *
 *  Created on: Apr 9, 2025
 *      Author: rameshchary.vadla
 */

#ifndef INC_MAX11206_H_
#define INC_MAX11206_H_

#include <stdint.h>
#include "main.h"


extern SPI_HandleTypeDef hspi1;

void MAX11206_CS_Low();

void MAX11206_CS_High();

uint8_t MAX11206_ReadReady();




uint8_t MAX11206_ReadReady();
void MAX11206_ConfigCTRL1(SPI_HandleTypeDef *hspi);
void MAX11206_WriteRegister(SPI_HandleTypeDef *hspi, uint8_t reg_addr, uint8_t *data, uint8_t size);
void MAX11206_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t reg_addr, uint8_t *data, uint8_t size);
int32_t MAX11206_ReadData(SPI_HandleTypeDef *hspi);
void MAX11206_StartConversion(SPI_HandleTypeDef *hspi);
uint8_t MAX11206_WaitForDataReady(SPI_HandleTypeDef *hspi);
void MAX11206_Init(SPI_HandleTypeDef *hspi,uint8_t gain);
int Read_MAX11206_ADC_Value();
void MAX11206_Command_byte(SPI_HandleTypeDef *hspi,uint8_t command) ;
void self_calib(void);

uint8_t read_status();

float get_mTorr_lookup(float adc);
float interpolate(float x, float x0, float y0, float x1, float y1);


double interpolate_mTORR(double mV);

// these are the command bytes with mode 0
//The command byte initiates a conversion or an immediate power-down
#define SELF_CALIB 0x90
#define SYS_OFFSET_CALIB 0xA0
#define SYS_GAIN_CALIB 0xB0
#define INTERMEDIATE_POWDOWN 0X88
#define CONVERT_1SPS 0x80
#define CONVERT_2_5_SPS 0x81
#define CONVERT_5_SPS 0x82
#define CONVERT_10_SPS 0x83
#define CONVERT_15_SPS 0x84
#define CONVERT_30_SPS 0x85
#define CONVERT_60_SPS 0x86
#define CONVERT_120_SPS 0x87



//address commands bytes

#define CTRL1_reg 0x01
#define CTRL2_reg 0x02
#define CTRL3_reg 0x03
#define STAT1_reg 0x00
#define DATA_reg 0x04
#define SOC_reg 0x05
#define SGC_reg 0x06
#define SCOC_reg 0x07
#define SCGC_reg 0x08

#define STAT1  0xC1

#define GAIN1 0x00
#define GAIN2 0x20
#define GAIN4 0x40
#define GAIN8 0x60
#define GAIN16 0x80
#define GAIN32 0xA0



// Mode 1

void read_CTRL3(void);

#endif /* INC_MAX11206_H_ */
