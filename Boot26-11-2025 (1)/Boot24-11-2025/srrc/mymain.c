/*
 * mymain.c
 *
 *  Created on: Oct 24, 2025
 *      Author: RameshcharyVadla
 */


#include <mymain.h>
#include <main.h>
#include "BOOT.h"



uint32_t sw_tmr_active[NUM_SW_TMRS];
uint32_t *sw_tmr_flag[NUM_SW_TMRS];
uint32_t sw_tmr_count[NUM_SW_TMRS];


extern  TIM_HandleTypeDef htim7;


void Set_delay(uint8_t tmr_id, uint32_t delay, uint32_t *flag)
{
  sw_tmr_active[tmr_id] = 1;
  sw_tmr_flag[tmr_id] = flag;
  sw_tmr_count[tmr_id] = delay;
  *sw_tmr_flag[tmr_id] = 0;
}

/*
 * *****************************************************************************
 * Function name: TimeBase_Gen()
 * Created by: \Ramesh chary
 * Date:       24-OCT-2025
 * Description:
 *  This is the callback function to the ISR for Timer7 interrupt vector.
 *  Interrupt duration is 1ms.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TimeBase_Gen(TIM_HandleTypeDef *htim)
{
  uint8_t i;

  for (i=0; i<NUM_SW_TMRS; i++)
  {
    if (sw_tmr_active[i] == 1)
    {
      if (--sw_tmr_count[i] == 0)
      {
        *sw_tmr_flag[i] = 1;
        sw_tmr_active[i] = 0;
      }
    }
  }
}

void my_init(void)
{

  if (HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, TimeBase_Gen) == HAL_OK)
  {
    // Successfully added the call-back function
	HAL_TIM_Base_Start_IT(&htim7);        // Start Timer7
    HAL_NVIC_EnableIRQ(TIM7_IRQn);        // Enable Timer7 interrupt
  }
 }



void my_func() {

	boot_proc();
}

