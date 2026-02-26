/* 
 * /////////////////////////////////////////////////////////////////////////////
 * Module: TempMon.c
 * Author: Ilensys
 *
 * Created on 11 June, 2022, 10:31 AM
 * 
 * Description:
 * This module contains definitions of LN2 temperature monitoring functions.
 *  
 * Note:
 *  MCU supported: STM32L4P5VGT6
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "math.h"

/* Declaration of external global variables */

/* Declaration of global variables */
TEMP_INFO temp_info[NO_OF_RTD_SENSORS];
float temp_val;
float a1;
float a2;
float a3;

/* Declaration of static local variables */

/* Declaration of local functions */

/* 
 * **************************************
 * **************************************
 * Function Declarations start here
 * **************************************
 * **************************************
 */
/*
 * *****************************************************************************
 * Function name: IIRLPF
 * Created by:
 * Date:       21-Apr-2023
 * Description:
 *  This is IIR LPF filter
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
float IIRLPF(float Vin, float VoutPrev)
{
//	static float VoutPrev = 0;
	const float TimeConstFactor1 =0.111111; //a/(1+a)
	const float TimeConstFactor2 =0.888889; //1/(1+a)

	VoutPrev = (TimeConstFactor1 * Vin) + (TimeConstFactor2*VoutPrev);
	return VoutPrev;
}


/* 
 * *****************************************************************************
 * Function name: TempMon_init()
 * Created by   : Natarajan Kanniappan
 * Date:          01-Jul-2022
 * Description:
 *  This function initializes LN2 Temperature Monitoring  parameters.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void TempMon_init(void)
{
  temp_info[RTD_1] = TEMP_INFO_DEFAULT;
  temp_info[RTD_2] = TEMP_INFO_DEFAULT;

  //RTD 1 Calib Factor
  cal_par_str.rtd1_param[0] = 0.00000019;
  cal_par_str.rtd1_param[1] = 0.0281251;
  cal_par_str.rtd1_param[2] = -243.15385;

  //RTD 2 Calib Factor
  cal_par_str.rtd2_param[0] = 0.00000019;
  cal_par_str.rtd2_param[1] = 0.0281251;
  cal_par_str.rtd2_param[2] = -243.15385;
}

/* 
 * *****************************************************************************
 * Function name: TempMon1_proc()
 * Created by   :
 * Date:
 * Description:
 *  This is the State Machine to handle RTD1 LN2 temperature monitoring activities
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void TempMon1_proc(void)
{
 static float FillteredData = 0.0;

  switch (temp_info[RTD_1].state)
  {
    case TEMP_WAIT:
      if (temp_info[RTD_1].new_data)
      {
        temp_info[RTD_1].new_data = 0;
        temp_info[RTD_1].state = TEMP_READ;
      }
      break;

    case TEMP_READ:
        temp_info[RTD_1].state = TEMP_WAIT;
      // Apply Low Pass Filter (LPF) algorithm
      FillteredData = IIRLPF((float)temp_info[RTD_1].RTDVal,FillteredData);
      temp_val = (FillteredData*FillteredData* cal_par_str.rtd1_param[0])+(FillteredData*cal_par_str.rtd1_param[1])+ cal_par_str.rtd1_param[2];

      if ((vp_cal_pt_rd_str.status == CALIB_ACTIVE) && (vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD1))
        {
    	  vp_cal_pt_rd_str.WaitSamples++;
    	  if(vp_cal_pt_rd_str.WaitSamples >= NO_OF_WAIT_SAMPLES)
    	  {
        	  vp_cal_pt_rd_str.WaitSamples = 0;
              cal_pt_str[CALIB_PARAM_RTD1].y[vp_cal_pt_rd_str.step_num] = FillteredData;  // Y value (Y1 / Y2)
              vp_cal_pt_rd_str.step_ack = 1;
    	  }
        }
      temp_info[RTD_1].temp_val.temp = temp_val;

      if(temp_info[RTD_1].SampleCount < MAX_RTD_WAIT_SAMPLES)
      {
    	  temp_info[RTD_1].SampleCount++;
    	  return;
      }
      vp_rt_wr_str.temp1 = temp_val;
      
      //Check for RTD open condition
      if(temp_val > RTD_OPEN_LIMIT)
      {
    	  if(1)//(vp_param_rw_str.Open_detection_rtd1>3)
    	  {
    		  temp_info[RTD_1].temp_val.fault_status++;
			  if(temp_info[RTD_1].temp_val.fault_status > RTD_FAULT_CHECK_ITERATION)
			  {
				  temp_info[RTD_1].rtd_status = RTD_STAT_OPEN;
					vp_rt_wr_str.rtd1_status = RTD_STAT_OPEN;
					  System_Status[ERROR_FLAG_0] |= ERR_RTD1_SENSOR_FAILURE;
					System_Status[ERROR_FLAG_0] &= ~ERR_T1_LOW;
					System_Status[ERROR_FLAG_0] &= ~ERR_T1_HIGH;
					vp_param_rw_str.RTD_T1_High_Error_flg=0;
					vp_param_rw_str.RTD_T1_Low_Error_flg=0;
					  return;
			  }
			  vp_param_rw_str.Open_detection_rtd1=10;
    	  }
    	  else
		  {
			  reinitialize_SPI=1;
		  }
    	  vp_param_rw_str.Short_detection_rtd1=0;
      }
      else if(temp_val < RTD_SHORT_LIMIT)
      {
    	  if(vp_param_rw_str.Short_detection_rtd1>3)
    	  {
    		  temp_info[RTD_1].temp_val.fault_status++;
			  if(temp_info[RTD_1].temp_val.fault_status > RTD_FAULT_CHECK_ITERATION)
			  {
					temp_info[RTD_1].rtd_status = RTD_STAT_SHORT;
					vp_rt_wr_str.rtd1_status = RTD_STAT_SHORT;
					System_Status[ERROR_FLAG_0] |= ERR_RTD1_SHORT;
					System_Status[ERROR_FLAG_0] &= ~ERR_T1_LOW;
					System_Status[ERROR_FLAG_0] &= ~ERR_T1_HIGH;
					vp_param_rw_str.RTD_T1_High_Error_flg=0;
					vp_param_rw_str.RTD_T1_Low_Error_flg=0;
					  return;

			  }
			  vp_param_rw_str.Short_detection_rtd1=10;
    	  }
    	  else
		  {
			  reinitialize_SPI=1;
		  }
    	  vp_param_rw_str.Open_detection_rtd1=0;
      }
      else
      {
    	  temp_info[RTD_1].temp_val.fault_status = 0;
		  temp_info[RTD_1].rtd_status = RTD_STAT_GOOD;
          vp_rt_wr_str.rtd1_status = RTD_STAT_GOOD;
      	  System_Status[ERROR_FLAG_0] &= ~ERR_RTD1_SENSOR_FAILURE;
      	  System_Status[ERROR_FLAG_0] &= ~ERR_RTD1_SHORT;
      	vp_param_rw_str.Short_detection_rtd1=0;
      	vp_param_rw_str.Open_detection_rtd1=0;
     }

      // Check the temperature values against set values
      if ((temp_info[RTD_1].temp_val.temp >= vp_param_rw_str.high_temp1.value) && (vp_param_rw_str.high_temp1.is_enabled)&&(!vp_rt_wr_str.rtd1_status))
      {
  		System_Status[ERROR_FLAG_0] |= ERR_T1_HIGH;
  		System_Status[ERROR_FLAG_0] &= ~ERR_T1_LOW;
        vp_param_rw_str.RTD_T1_High_Error_flg=1;
        vp_param_rw_str.RTD_T1_Low_Error_flg=0;
      }
      else if ((temp_info[RTD_1].temp_val.temp <= vp_param_rw_str.low_temp1.value) && (vp_param_rw_str.low_temp1.is_enabled)&&(!vp_rt_wr_str.rtd1_status))
      {
    	System_Status[ERROR_FLAG_0] |= ERR_T1_LOW;
    	System_Status[ERROR_FLAG_0] &= ~ERR_T1_HIGH;
        vp_evnt_wr_str.low_rtd1_temp=1;
        vp_param_rw_str.RTD_T1_High_Error_flg=0;
        vp_param_rw_str.RTD_T1_Low_Error_flg=1;
      }
      else if((temp_info[RTD_1].temp_val.temp <= vp_param_rw_str.high_temp1.value-0.2) &&(temp_info[RTD_1].temp_val.temp >= vp_param_rw_str.low_temp1.value+0.2))
      {
        // Temperature within limits. Do nothing
    	  System_Status[ERROR_FLAG_0] &= ~ERR_T1_HIGH;
		  vp_param_rw_str.RTD_T1_High_Error_flg=0;
		  System_Status[ERROR_FLAG_0] &= ~ERR_T1_LOW;
		  vp_param_rw_str.RTD_T1_Low_Error_flg=0;
      }
      if (!vp_param_rw_str.low_temp1.is_enabled)
	  {
		   System_Status[ERROR_FLAG_0] &= ~ERR_T1_LOW;
		   vp_param_rw_str.RTD_T1_Low_Error_flg=0;
	  }
	  if(!vp_param_rw_str.high_temp1.is_enabled)
	  {
		   System_Status[ERROR_FLAG_0] &= ~ERR_T1_HIGH;
		   vp_param_rw_str.RTD_T1_High_Error_flg=0;
	  }

      temp_info[RTD_1].temp_val.fract_val = 10 * (temp_val - (int16_t)temp_val);
      temp_info[RTD_1].temp_val.int_val = (int16_t)temp_val;

      vp_evnt_wr_str.low_rtd1_temp=0;
      break;

    default:
      temp_info[RTD_1].state = TEMP_WAIT;
      break;
  }
}

/* 
 * *****************************************************************************
 * Function name: TempMon2_proc()
 * Created by   :
 * Date:
 * Description:
 *  This is the State Machine to handle RTD2 LN2 temperature monitoring activities
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void TempMon2_proc(void)
{
 static  float FillteredData = 0.0;

  if(!vp_param_rw_str.t2enable)
  {
	  System_Status[ERROR_FLAG_0] &= ~ERR_T2_LOW;
	  System_Status[ERROR_FLAG_0] &= ~ERR_T2_HIGH;
	  System_Status[ERROR_FLAG_0] &= ~ERR_RTD2_SENSOR_FAILURE;
	  System_Status[ERROR_FLAG_0] &= ~ERR_RTD2_SHORT;
	  return;
  }


  switch (temp_info[RTD_2].state)
  {
    case TEMP_WAIT:
      if (temp_info[RTD_2].new_data)
      {
        temp_info[RTD_2].new_data = 0;
        temp_info[RTD_2].state = TEMP_READ;
      }
      break;

    case TEMP_READ:
        temp_info[RTD_2].state = TEMP_WAIT;
      // Apply Low Pass Filter (LPF) algorithm
      FillteredData = IIRLPF((float)temp_info[RTD_2].RTDVal,FillteredData);
      temp_val = (FillteredData*FillteredData* cal_par_str.rtd2_param[0])+(FillteredData*cal_par_str.rtd2_param[1])+ cal_par_str.rtd2_param[2];

      if ((vp_cal_pt_rd_str.status == CALIB_ACTIVE) && (vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD2))
        {
    	  vp_cal_pt_rd_str.WaitSamples++;
    	  if(vp_cal_pt_rd_str.WaitSamples >= NO_OF_WAIT_SAMPLES)
    	  {
        	  vp_cal_pt_rd_str.WaitSamples = 0;
              cal_pt_str[CALIB_PARAM_RTD2].y[vp_cal_pt_rd_str.step_num] = FillteredData;  // Y value (Y1 / Y2)
              vp_cal_pt_rd_str.step_ack = 1;
    	  }
        }

        temp_info[RTD_2].temp_val.temp = temp_val;
        if(temp_info[RTD_2].SampleCount < MAX_RTD_WAIT_SAMPLES)
        {
      	  temp_info[RTD_2].SampleCount++;
      	  return;
        }
        vp_rt_wr_str.temp2 = temp_val;  // Celsius



        //Check for RTD open condition
        if(temp_val > RTD_OPEN_LIMIT)
        {
        	if(1)//(vp_param_rw_str.Open_detection_rtd2>3)
        	{
        		  temp_info[RTD_2].temp_val.fault_status++;
				  if(temp_info[RTD_2].temp_val.fault_status > RTD_FAULT_CHECK_ITERATION)
				  {
					  temp_info[RTD_2].rtd_status = RTD_STAT_OPEN;
					  vp_rt_wr_str.rtd2_status = RTD_STAT_OPEN;
					  System_Status[ERROR_FLAG_0] |= ERR_RTD2_SENSOR_FAILURE;
					  System_Status[ERROR_FLAG_0] &= ~ERR_T2_LOW;
					  System_Status[ERROR_FLAG_0] &= ~ERR_T2_HIGH;
					  vp_param_rw_str.RTD_T2_High_Error_flg=0;
					  vp_param_rw_str.RTD_T2_Low_Error_flg=0;
					  return;
				  }
				  vp_param_rw_str.Open_detection_rtd2=10;
        	}
        	else
		    {
			    reinitialize_SPI=1;
		    }
        	vp_param_rw_str.Short_detection_rtd2=0;
        }
        else if(temp_val < RTD_SHORT_LIMIT)
        {
        	if(vp_param_rw_str.Short_detection_rtd2>3)
        	{
        		temp_info[RTD_2].temp_val.fault_status++;
				if(temp_info[RTD_2].temp_val.fault_status > RTD_FAULT_CHECK_ITERATION)
				{
					temp_info[RTD_2].rtd_status = RTD_STAT_SHORT;
					vp_rt_wr_str.rtd2_status = RTD_STAT_SHORT;
					System_Status[ERROR_FLAG_0] |= ERR_RTD2_SHORT;
					System_Status[ERROR_FLAG_0] &= ~ERR_T2_LOW;
					System_Status[ERROR_FLAG_0] &= ~ERR_T2_HIGH;
					vp_param_rw_str.RTD_T2_High_Error_flg=0;
					vp_param_rw_str.RTD_T2_Low_Error_flg=0;
					return;
				}
				vp_param_rw_str.Short_detection_rtd2=10;
        	}
        	else
		    {
			    reinitialize_SPI=1;
		    }
        	vp_param_rw_str.Open_detection_rtd2=0;
        }
        else
        {
      	  temp_info[RTD_2].temp_val.fault_status = 0;
  		  temp_info[RTD_2].rtd_status = RTD_STAT_GOOD;
          vp_rt_wr_str.rtd2_status = RTD_STAT_GOOD;
      	  System_Status[ERROR_FLAG_0] &= ~ERR_RTD2_SENSOR_FAILURE;
      	  System_Status[ERROR_FLAG_0] &= ~ERR_RTD2_SHORT;
      	  vp_param_rw_str.Short_detection_rtd2=0;
      	  vp_param_rw_str.Open_detection_rtd2=0;
        }
	// Check the temperature values against set values
	   if ((temp_info[RTD_2].temp_val.temp >= vp_param_rw_str.high_temp2.value) && (vp_param_rw_str.high_temp2.is_enabled)&&(!vp_rt_wr_str.rtd2_status))
	   {
  	     System_Status[ERROR_FLAG_0] |= ERR_T2_HIGH;
	     System_Status[ERROR_FLAG_0] &= ~ERR_T2_LOW;
		 vp_param_rw_str.RTD_T2_High_Error_flg=1;
		 vp_param_rw_str.RTD_T2_Low_Error_flg=0;
	   }
	   else if ((temp_info[RTD_2].temp_val.temp <= vp_param_rw_str.low_temp2.value) && (vp_param_rw_str.low_temp2.is_enabled)&&(!vp_rt_wr_str.rtd2_status))
	   {
	  	 System_Status[ERROR_FLAG_0] |= ERR_T2_LOW;
		 System_Status[ERROR_FLAG_0] &= ~ERR_T2_HIGH;
		 vp_evnt_wr_str.low_rtd2_temp=1;
		 vp_param_rw_str.RTD_T2_High_Error_flg=0;
		 vp_param_rw_str.RTD_T2_Low_Error_flg=1;
	   }
	   else if((temp_info[RTD_2].temp_val.temp <= vp_param_rw_str.high_temp2.value-0.2)&&(temp_info[RTD_2].temp_val.temp >= vp_param_rw_str.low_temp2.value+0.2))
	   {
		 // Temperature within limits. Do nothing

			 System_Status[ERROR_FLAG_0] &= ~ERR_T2_HIGH;
		     vp_param_rw_str.RTD_T2_High_Error_flg=0;
		     System_Status[ERROR_FLAG_0] &= ~ERR_T2_LOW;
			 vp_param_rw_str.RTD_T2_Low_Error_flg=0;

	   }
	   if (!vp_param_rw_str.low_temp2.is_enabled)
	   {
		   System_Status[ERROR_FLAG_0] &= ~ERR_T2_LOW;
		   vp_param_rw_str.RTD_T2_Low_Error_flg=0;
	   }
	   if(!vp_param_rw_str.high_temp2.is_enabled)
	   {
		   System_Status[ERROR_FLAG_0] &= ~ERR_T2_HIGH;
		   vp_param_rw_str.RTD_T2_High_Error_flg=0;
	   }
      temp_info[RTD_2].temp_val.fract_val = 10 * (temp_val - (int16_t)temp_val);
      temp_info[RTD_2].temp_val.int_val = (int16_t)temp_val;
      vp_evnt_wr_str.low_rtd2_temp=0;
      break;

    default:
      temp_info[RTD_2].state = TEMP_WAIT;
      break;
  }
}
/*
 * *********************************
 * *********************************
 * End of the module: TempMon.c
 * *********************************
 * *********************************
 */

