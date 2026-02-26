/*
 * mymain.h
 *
 *  Created on: Oct 24, 2025
 *      Author: RameshcharyVadla
 */

#ifndef MYMAIN_H_
#define MYMAIN_H_
#include <main.h>
#include <stdint.h>


#define DLY_LED_START            100   // 10ms
#define DLY_BOOT_START           10000
#define DLY_CDC_START           30000
#define SW_SD_CARD_DLY          1
#define BOOT_DELAY          2

#define SW_TMR_LAST             35
#define NUM_SW_TMRS            (SW_TMR_LAST)
void TimeBase_Gen(TIM_HandleTypeDef *htim);

void Set_delay(uint8_t tmr_id, uint32_t delay, uint32_t *flag);

#endif /* MYMAIN_H_ */
