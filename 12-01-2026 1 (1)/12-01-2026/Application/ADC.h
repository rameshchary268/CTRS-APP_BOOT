/*
 * ADC.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

/* Includes */
#include "MyMain.h"

/* Declaration of constant defines and typedefs */
#define MAX_ADC_VALUE             4096 // Maximum possible ADC value = 2^(no. of bits} = 16383, as no.of bits = 14

#define ADC_CH_THERMISTOR1          0    // Offset from buffer "ADC_info.buffer" for channel ADC12_IN5
#define ADC_CH_THERMISTOR2          1    // Offset from buffer "ADC_info.buffer" for channel ADC12_IN6
#define ADC_CH_HERMISTOR3           2    // Offset from buffer "ADC_info.buffer" for channel ADC12_IN7
#define ADC_CH_BATT_VOLT            3    // Offset from buffer "ADC_info.buffer" for channel ADC12_IN17
#define ADC_CH_REF                  4    // Offset from buffer "ADC_info.buffer" for channel ADC12_INREF
#define MAX_ADC_CHANNELS            5
#define SAMPLE_SIZE                 8

typedef enum {
  ADC_CALIBRATE,
  ADC_START,
  ADC_SOC,
  ADC_EOC,
  ADC_ERR,
  ADC_HALT,
} ADC_STATE;

typedef struct {
 ADC_STATE state;
 uint8_t   data_rdy;
 uint32_t  poll_flag;
 uint32_t  dly_flag;
 uint16_t  buffer[MAX_ADC_CHANNELS];
 uint16_t  Temp_buffer[MAX_ADC_CHANNELS];
 uint8_t   ch_id;
 uint32_t  cal_flag;
 uint8_t   ADC_Data_rdy;
 uint8_t   ADC_Fail;
 uint8_t   ADCDataReady;
} ADC_INFO;

#define ADC_INFO_DEFAULT (ADC_INFO) {ADC_CALIBRATE, 0, 0, 0, {0, }, {0, },0, 0,0}

typedef struct {
  float      volt;
} BATT_INFO;

#define BATT_INFO_DEFAULT (BATT_INFO) { 0.0}


/* Function Declarations */
void     ADC_init();
void     ADC_proc();
//void     ADC_ISR_EOC_CallbackFunction(ADC_HandleTypeDef *hadc);
void     ShutdownDisplay(void);
void     Before_update(void);


#endif /* SRC_ADC_H_ */
