/*
 * Calibration.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef CALIBRATION_H
#define	CALIBRATION_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Includes */
#include "MyMain.h"
#include "RTC.h"

/* Declaration of constant defines and typedefs */
#define CALIB_PARAM_TH         0
#define CALIB_PARAM_DP         1
#define CALIB_PARAM_RTD1       2
#define CALIB_PARAM_RTD2       3
#define CALIB_PARAM_RTD3       4
#define CALIB_PARAM_RTD4       5
#define MAX_CALIB_PARAMS       (CALIB_PARAM_RTD4+1)

#define MAX_CALIB_POINTS       3
#define MAX_TH_PARAMS          3
#define MAX_DP_PARAMS          2
#define MAX_RTD_PARAMS         3

#define CALIB_INACTIVE         0
#define CALIB_ACTIVE           1
#define CALIB_DONE             2

#define CALIB_REQ_VALIDATE     0
#define CALIB_DATA_VALID       1
#define CALIB_DATA_INVALID     2

#define CAL_FAIL_TH            0x01
#define CAL_FAIL_DP            0x02
#define CAL_FAIL_RTD1          0x04
#define CAL_FAIL_RTD2          0x08
#define CAL_FAIL_RTD3          0x10
#define CAL_FAIL_RTD4          0x20

#define MAX_DEV_CAP_PAR        0.05     // 5%
#define NO_OF_WAIT_SAMPLES	   40

///////////////////////////////////////////////////
// Default calibration points - maximum of 3 points
///////////////////////////////////////////////////
// Calibration points for thermistor based LN2 sensing
#define LVL_CAL_PT1_XVAL       1.0     // Step1
#define LVL_CAL_PT2_XVAL       3.0     // Step2
#define LVL_CAL_PT3_XVAL       5.0     // Step3
// Calibration points for DP transducer based LN2 sensing
#define DP_CAL_PT1_XVAL        1.0     // Step1
#define DP_CAL_PT2_XVAL        3.0     // Step2
#define DP_CAL_PT3_XVAL        5.0     // Step3
// Calibration points for RTD1 transducer based temperature sensing
#define RTD1_CAL_PT1_XVAL      0.0     // Step1
#define RTD1_CAL_PT2_XVAL   -145.0     // Step2
#define RTD1_CAL_PT3_XVAL   -195.0     // Step3
// Calibration points for RTD2 transducer based temperature sensing
#define RTD2_CAL_PT1_XVAL      0.0     // Step1
#define RTD2_CAL_PT2_XVAL   -145.0     // Step2
#define RTD2_CAL_PT3_XVAL   -195.0     // Step3

/////////////////////////////////
// Default calibration parameters
/////////////////////////////////
#define LVL_CAL_PT1_VAL        2.0
#define LVL_CAL_PT2_VAL        4.0
#define LVL_CAL_PT3_VAL        6.0
#define DP_CAL_PT1_VAL         2.0
#define DP_CAL_PT2_VAL         4.0
#define DP_CAL_PT3_VAL         6.0
#define RTD1_CAL_PT1_VAL    -195.0
#define RTD1_CAL_PT2_VAL       0.0
#define RTD1_CAL_PT3_VAL       0.0
#define RTD2_CAL_PT1_VAL    -195.0
#define RTD2_CAL_PT2_VAL       0.0
#define RTD2_CAL_PT3_VAL       0.0
#define RTD3_CAL_PT1_VAL    -195.0
#define RTD3_CAL_PT2_VAL       0.0
#define RTD3_CAL_PT3_VAL       0.0
#define RTD4_CAL_PT1_VAL    -195.0
#define RTD4_CAL_PT2_VAL       0.0
#define RTD4_CAL_PT3_VAL       0.0

// Default Thermistor process constants
#define TH_CONST_C1            1          // TBD
#define TH_CONST_C2            1          // TBD
#define TH_OFFSET              0          // TBD
// Default Differential Pressure (DP) process constants
#define DP_SLOPE              0.017814573//2k- 0.00882    // TBD
#define DP_OFFSET             -2.97117//-13.48       // TBD

// Default RTD process constants
#define RTD_CONST_C1           0.00000019 // TBD
#define RTD_CONST_C2           0.0281251  // TBD
#define RTD_OFFSET          -243.15385    // TBD

// Allowed variations of process constant values during calibration
#define DP_SLOPE_VARIATION    	   30.0   // Percentage of variation for slope - TBD
#define DP_OFFSET_VARIATION   	   100.0   // Percentage of variation for offset - TBD
#define RTD_CONST_C1_VARIATION     20     // Percentage of variation for C1 - TBD
#define RTD_CONST_C2_VARIATION     20     // Percentage of variation for C2 - TBD
#define RTD_OFFSET_VARIATION       20     // Percentage of variation for Offset - TBD

#define CAL_PAR_DEFAULT (VP_CAL_RD_STR) {  \
  {TH_CONST_C1,  TH_CONST_C2,  TH_OFFSET},    \
  {RTD_CONST_C1, RTD_CONST_C2, RTD_OFFSET},   \
  {RTD_CONST_C1, RTD_CONST_C2, RTD_OFFSET},   \
  {RTD_CONST_C1, RTD_CONST_C2, RTD_OFFSET},   \
  {RTD_CONST_C1, RTD_CONST_C2, RTD_OFFSET},   \
  0, 0, 0,                                    \
  0x0000,                                     \
}

// Validity check of calibration parameters
#define MIN_RTD_CONST_C1        (RTD_CONST_C1*(1.0-(RTD_CONST_C1_VARIATION/100.0)))
#define MAX_RTD_CONST_C1        (RTD_CONST_C1*(1.0+(RTD_CONST_C1_VARIATION/100.0)))
#define MIN_RTD_CONST_C2        (RTD_CONST_C2*(1.0-(RTD_CONST_C1_VARIATION/100.0)))
#define MAX_RTD_CONST_C2        (RTD_CONST_C2*(1.0+(RTD_CONST_C1_VARIATION/100.0)))
#define MIN_RTD_OFFSET          (RTD_OFFSET*(1.0-(RTD_OFFSET_VARIATION/100.0)))
#define MAX_RTD_OFFSET          (RTD_OFFSET*(1.0+(RTD_OFFSET_VARIATION/100.0)))

// Typedefs and enums
typedef struct
{
  float    y[MAX_CALIB_POINTS];
  uint8_t  step;
  TIME     time_stamp;
} CAL_PT_STR;

/* Typedefs and enums */
typedef enum {
  CAL_VALIDATE,
  CAL_DONE,
  CAL_START,
  CAL_STEP1,
  CAL_STEP2,
  CAL_ERR,
  CAL_STOP,
} CAL_STATE;

typedef struct {
  CAL_STATE state;
  int8_t    read_ui;
  int8_t    write_eeprom;
  int8_t    write_ui;
  TIME      in;
  TIME      out;
} CAL_INFO;

typedef struct {
	double A1;
	double A2;
	double A3;
} RTDCalibFactorStruct;

typedef enum {
    START_TIME,
	TIME_STOP
} DPCALIB_TIME;

#define CAL_INFO_DEFAULT (CAL_INFO) {CAL_STOP,  0, 0, 0, {0, }, {0, }, }

/* Function Declarations */
void    Calib_init(void);
void    Calib_proc(void);
void    calib3wayout(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CALIBRATION_H */

