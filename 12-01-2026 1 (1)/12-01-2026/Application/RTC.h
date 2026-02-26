/*
 * RTC.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef RTC_H_
#define RTC_H_

/* Includes */
#include "stm32u5xx_hal_tim.h"
/* Declaration of constant defines and typedefs */


typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint16_t year;
  uint8_t tenth_of_second;
  uint8_t timeformat;
} TIME;

// Default time is 12:00:00 AM
#define DEF_HOURS           11
#define DEF_MINUTES         46
#define DEF_SECONDS         0
#define DEF_SUB_SECONDS     0
#define DEF_SECONDS_FRAC    0
#define DEF_TIME_FORMAT     (RTC_HOURFORMAT12_AM)
// Default date is 01-Jan-2022 Saturday
#define DEF_WEEKDAY         (RTC_WEEKDAY_SUNDAY)
#define DEF_MONTH           (RTC_MONTH_MAY)
#define DEF_DATE            7
#define DEF_YEAR            23    // 2022

#define DEF_TIME_SETTING (RTC_TimeTypeDef) {DEF_HOURS, DEF_MINUTES, DEF_SECONDS, DEF_TIME_FORMAT, }
#define DEF_DATE_SETTING (RTC_DateTypeDef) {DEF_WEEKDAY, DEF_MONTH, DEF_DATE, DEF_YEAR}

#define ERTC_SLAVE_ADRESS   (0x51 << 1)  // D[8:1] with D0 at 0 for Write or 1 for Read
#define I2C_WR              0x00
#define I2C_RD              0x01
#define BYTES_PER_TX        1
#define BYTES_PER_RX        1

#define REG_SECONDS         0x04
#define REG_MINUTES         0x05
#define REG_HOURS           0x06
#define REG_DAY_OF_WEEK     0x07
#define REG_DAY_OF_MONTH    0x08
#define REG_MONTH           0x09
#define REG_YEAR            0x0A

typedef enum {
  ERTC_WAIT,
  ERTC_RX_STRT,
  ERTC_RX_CONT,
  ERTC_RX_DONE,
  ERTC_TX_STRT,
  ERTC_TX_DONE,
  ERTC_ERR,
} ERTC_STATE;

/* Function Declarations */
// Internal RTC related functions
void LRTC_init(void);
void Set_TimeDate(void);
void Get_TimeStamp(void);
void TimeBase_Gen(TIM_HandleTypeDef *htim);
void TimeBase_Gen_Microsec_timer(TIM_HandleTypeDef *htim);

// External RTC related functions
void ERTC_init(void);
void ERTC_proc(void);

#endif /* RTC_H_ */
