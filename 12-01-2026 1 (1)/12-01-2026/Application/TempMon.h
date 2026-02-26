/*
 * TempMon.h
 *
 *  Created on: Nov 1, 2025
 *      Author: VenkateshArikanti
 */

#ifndef TEMPMON_H_
#define TEMPMON_H_
#define NO_OF_RTD_SENSORS     4
#define MAX_RTD_WAIT_SAMPLES	40

// Constants for Low Pass Filters
//#define EN_LPF_CALC
#define LPF_CONST1_VALUE      0.888889
#define LPF_CONST2_VALUE      0.111111

#define RTD_OPEN_LIMIT		  		700.0
#define RTD_SHORT_LIMIT		-240.0
#define RTD_FAULT_CHECK_ITERATION	2
/* Declaration of typedefs and enums */
typedef enum {
  TEMP_WAIT,
  TEMP_READ,
} TEMP_STATE;

typedef enum {
	RTD_STAT_GOOD,
	RTD_STAT_OPEN,
	RTD_STAT_SHORT
}RTD_STATUS;

typedef struct {
  float      temp;
  uint8_t    sign;
  uint8_t    fract_val;
  int16_t    int_val;
  uint8_t    fault_status;
} TEMP_VAL;

typedef struct {
  TEMP_STATE state;
  TEMP_VAL   temp_val;
  RTD_STATUS rtd_status;
  uint8_t    new_data;
  uint8_t    SampleCount;
  uint16_t   RTDVal;
} TEMP_INFO;

#define TEMP_INFO_DEFAULT (TEMP_INFO) {TEMP_WAIT, {0, }, 0, 0, 0,}

/* Function Declarations */
void TempMon_init(void);
void TempMon1_proc(void);
void TempMon2_proc(void);

#endif /* TEMPMON_H_ */
