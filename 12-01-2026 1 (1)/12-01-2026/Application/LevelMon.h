/* 
 * File:   LevelMon.h
 * Author: Ilensys
 *
 * Created on 11 June, 2022, 9:31 AM
 */

#ifndef LEVELMON_H
#define	LEVELMON_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Includes */
#include <stdbool.h>

/* Declaration of constant defines and typedefs */
#define LVL_SENS_THRM_HIGH_UL   20   // Thermistor resistance for high temperature warning limit of -150 degC
#define LVL_SENS_THRM_HIGH_LL   968   // Thermistor resistance for high temperature warning limit of -152 degC with -2 degC hysteresis
#define LVL_SENS_THRM_LOW_UL   3200   // Thermistor resistance for high temperature warning limit of -182 degC
#define LVL_SENS_THRM_LOW_LL   3800   // Thermistor resistance for high temperature warning limit of -185 degC with -3 degC hysteresis

#define FV_GBPV_THRM_HIGH_VAL  160//400//160
#define FV_GBPV_THRM_LOW_VAL   3800 // Thermistor resistance for detection of LN2:    Lower  than -182 degC
#define FV_GBPV_THRM_HIGH      60//150//50

#define MAX_THERMISTOR_SENSORS		3
#define MAX_THERMISTOR_LEVELS		7
#define LPF_SETTLING_SAMPLE_COUNT	12
#define DP_SETTLING_SAMPLE_COUNT    40

#define THERMISTOR_1_LEVEL_MASK		0x01
#define THERMISTOR_2_LEVEL_MASK		0x02
#define THERMISTOR_3_LEVEL_MASK		0x04
#define THERMISTOR_4_LEVEL_MASK		0x08
#define THERMISTOR_5_LEVEL_MASK		0x10
#define THERMISTOR_6_LEVEL_MASK		0x20

#define MIN_DP_SENSOR_LEVEL				-1.0
#define MAX_DP_SENSOR_LEVEL				6.5
#define DP_THERMISTOR_LEVEL_TOLERENCE	0.5
#define MAX_VALID_LEVEL_DIFFERENCE		1.0

#define THERM_LN2_FILLING_SENSE_LIMIT			3000//3600
#define THERM_LN2_RECEDING_SENSE_LIMIT			3100
#define THERM_OPEN_SENSE_LIMIT					3750+100



typedef enum {
  LVL_WAIT,
  LVL_READ,
  LVL_ERR,
} LVL_STATE;

typedef struct {
  bool    level1 : 1;
  bool    level2 : 1;
  bool    level3 : 1;
  bool    level4 : 1;
  bool    level5 : 1;
  bool    level6 : 1;
} LEVEL;

typedef struct {
  bool    err1 : 1;
  bool    err2 : 1;
  bool    err3 : 1;
  bool    err4 : 1;
  bool    err5 : 1;
  bool    err6 : 1;
} THR_ERR;

typedef struct {
  LVL_STATE state;
  THR_ERR   error;
  uint8_t 	err[MAX_THERMISTOR_SENSORS];
  uint32_t  time_dly;
  uint8_t   fault_status;
  uint8_t   new_data;
} LEVEL_INFO;


typedef enum {
	THERM_STATE_G,
	THERM_STATE_L,
	THERM_STATE_O
} ThermistorStates;

typedef enum {
	THERM_ARRAY_OK,
	THERM_ARRAY_STATE_CHANGED,
	THERM_ARRAY_FAULTY,
	THERM_ARRAY_DISABLED
} ThermistorArrayStates;


typedef struct {
	uint8_t ThermBitMask;
	uint8_t ThermLevelPattern;
	uint8_t ThermLevel;
} ThermLevelStruct;

typedef struct {
//	LevelMeasurementState MeasurementState;
	uint8_t NoOfSamplesProcessed;
	uint8_t IsLevelDataValid;
	uint8_t	ThermistorLevelBitStates;
	//uint8_t	ThermistorFaultStates;
	uint8_t ThermistorFaultBits;
	//uint8_t CalculatedThermLevel;
	uint8_t ThermistorLevel;
	//float CalculatedLevel;
	uint8_t Thermistor_status;
	uint8_t Enable_thermistor;
} LevelDataStruct;

typedef struct {
	float Therm_Level;
    uint16_t   DP_Level_adc;
} Run_Calib_Values;


typedef struct {
	uint16_t ADCRefValue;
	uint16_t ADCLowerLimit;
	uint16_t ADCUpperLimit;
} ADCRefCountStruct;

typedef enum {
	TIME_START,
	INIT_VALUE,
	CALCULATION
} LN2USAGE;

#define LEVEL_INFO_DEFAULT (LEVEL_INFO) {LVL_WAIT, 0, 0, 0, 0, 0, 0, 0, }

typedef struct {
	float min;
	float Level1;
	float Time1;
	float Time2;
	float Level2;
	float datausage;
	float pre_datausage;
	float pre_ln2usage;
	float pre_datausage_cm;
	float datausage_cm;
	uint8_t dayincrease;
	uint8_t date_change;
	uint8_t Level_change;
}LN2_USAGE;

/* Function Declarations */
void LevelMon_init(void);
void LevelMon_proc(void);
float IIRLPF_THERM(float Vin, float VoutPrev,float TimeConstFactor1,float TimeConstFactor2);
void ln2usage(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LEVELMON_H */

