/* 
 * File:   SPM.h
 * Author: Ilensys
 *
 * Created on 14 June, 2022, 9:23 AM
 */

#ifndef SPM_H
#define	SPM_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>

#include "MyMain.h"

#define LVL_SENS_SEL           0x01   // 1:DP based, 0:Both DP & Thermistor based
#define TEMP_SENS_SEL          0x02   // 1:RTD_1 only, 0-Both RTD_1 & RTD_2 present
#define BATTERY_SENS_SEL       0X04   // 0:battery less, 1:battery back up
///* Declaration of constant defines and typedefs */
//#define ADC_SHUTDOWN                0x01
//#define USB_SHUTDOWN                0x02
//#define WIFI_SHUTDOWN               0x04
//#define ETH_SHUTDOWN                0x08
//#define RTD1_SHUTDOWN               0x10
//#define RTD2_SHUTDOWN               0x20
//#define SD_SHUTDOWN                 0x40
//#define SHUTDOWN_RDY                0x80
//
//#define LED_HB                      1
//
//#define DATA_BATTERY                1
//#define DATA_LN2_FVT                2
//#define DATA_LN2_GBPVT              3
//#define DATA_LN2_TEMP1              4
//#define DATA_LN2_TEMP2              5
//#define DATA_LN2_DP_LVL             6
//#define DATA_LN2_TH_LVL             7
//#define DATA_LID_INFO               8
//#define DATA_DEFOG_INFO             9
//#define DATA_QUICKCHILL_INFO        10   // Do not change the order


//// LN2 level limits for filling
//#define MAX_STORAGE_LEVEL          6.0   // Inches
//#define FILL_HIGH_LIMIT            5.0   // Inches
//#define FILL_LOW_LIMIT             2.0   // Inches
//#define DEAD_STORAGE_LEVEL         1.0   // Inches
//#define MAX_DIFF_LVL_DP_THR        1.03  // Inches
//#define IMPR_FILL_LVL_DIFF         0.08  // Inches
//#define SHRT_FILL_LVL_DIFF         0.12  // Inches
// SPM limits for battery voltage
//#define MIN_BATT_VOLT              11.2//10.5    // Min for battery voltage before shut-down
//
//
//#define MIN_CONSUME_HOURS         (30*24) // 30 days is the minimum consumption duration before logging a warning
//
#define Max_Days                   7
//
//
//#define DAT_MSG_SIZE               32


typedef struct {
	uint8_t Day;
	uint8_t Daybits;
} DayStruct;



typedef struct {
  uint8_t     shutdown;
  uint8_t     md_sd_sts;   // Bit encoded
} SPM_INFO;

#define SPM_INFO_DEFAULT (SPM_INFO) {0, 0};


//
//typedef enum  {
//  SW_OFF,
//  SW_ON,
//  SW_ERR,
//} SW_STATE;


#define SW_INFO_DEFAULT (SW_INFO) {{SW_OFF, SW_OFF}, {SW_ON, SW_ON}, 0, {0, 0}};


typedef struct {
  TEMP_VAL start;
  TEMP_VAL current;
  TEMP_VAL end;
} TEMP_STAMP;

#define FILL_MON_INFO_DEFAULT (FILL_MON_INFO) {FILL_OFF, 0, 0, 0.0, {{0, }, }, 0, };

typedef struct{
uint8_t date_chk;
uint8_t date_sch_fill;
uint8_t date_ln2;
}DATE_REM;

typedef struct{
uint8_t update_firmware;
uint8_t update_display;
uint8_t Bist_update;
uint8_t firm_page;
uint8_t update_bootloader;
}UPDATE;


/* Function Declarations */
void  SPM_init();
void  Track_schedule(void);
uint32_t week_cal(uint8_t date, uint8_t mon, uint16_t year);
void update_firmwware_time(void);
void update_firmwware(void);
uint32_t week_cal_rem(uint8_t date, uint8_t mon, uint16_t year);

#ifdef	__cplusplus
}
#endif

#endif	/* SPM_H */

