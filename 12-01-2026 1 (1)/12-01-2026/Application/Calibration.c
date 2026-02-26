/*
 * Calibration.c
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

////////////////////////////////////////////////////////
// CALIBRATION OF THERMISTOR ARRAY FOR LN2 LEVEL SENSING
////////////////////////////////////////////////////////
// Calibration steps:
// Three point (level) calibration is needed as specified in requirement document.
//   1.Place the thermistor sensor board in the freezer and insert the scale stick.
//   2.Indicate no liquid in screen. Press the soft key to verify that all the thermistor elements are not immersed
// 	   Elemt#1	Elemt#2	Elemt#3	Elemt#4	Elemt#5	Elemt#6
// 		  G     	G	   G	  G   	   G	   G
//   3.Fill the freezer with liquid Nitrogen up to a height till the first element senses the liquid Nitrogen
// 	   Elemt#1	Elemt#2	Elemt#3	Elemt#4	Elemt#5	Elemt#6
// 		  L	        G	   G	  G	       G	   G
//   4.Measure the liquid level using the scale stick. Ideally level should be 1 inch.
//     But due to some dead level below sensor board there can be some offset.
//   5.Enter the offset level in inches as prompted in the touch screen display field. Press enter
//     The offset value will be entered in non-volatile memory.
//   6.Pour liquid Nitrogen till the level rises to the point when the second thermistor element immerses.
//   7.The sensor status table will show as given below
// 	   Elemt#1	Elemt#2	Elemt#3	Elemt#4	Elemt#5	Elemt#6
// 		  L	       L	   G	   G	   G	  G
//   8.Press Enter soft key.
//   9.Continue this process till all six sensor elements are immersed
// 	   Elemt#1	Elemt#2	Elemt#3	Elemt#4	Elemt#5	Elemt#6
//        L	       L	   L	   L	   L	   L
//  10.Level will show as "6 inches+ offset level value"
//  11.Calibration process is complete.
////////////////////////////////////////////////////////

//////////////////////////////////////////////////
// CALIBRATION OF DIFFERENTIAL PRESSURE TRANSDUCER
//////////////////////////////////////////////////
// Calibration Steps:
// 1.Keep the sensor ports at same pressure which is ambient pressure.
// 2.Measure the output voltage. It is the offset voltage.
// 3.While keeping one port free to atmosphere, fill the freezer with LN2 up to 3 inches height
// 4.Measure the output voltage.
// 5.Repeat the steps 3 and 4 by filling LN2 up to 6 inches height from bottom
// 6.Measure the output voltage.
// 7.Fit in the above values considering the response to be linear
// Let y be the level of LN2 to be measured and x be the corresponding voltage output.
// To verify the above equation and to obtain typical values of these constants, Let us take values from data sheet.
// Against VCC = 5V, the voltages corresponding to 0 kPa DP and 10 kPa DP are 0.2V and 4.7V respectively.
// These correspond to LN2 levels of 0 inch and 49.64inches respectively.
// Thus during the initial factory default settings before calibration,
// we can set m=11.031 and C=2.206
// Please note that as per data sheet the output voltage is 0.2V when DP=0 which is satisfied by the above equation.
// After calibration, the new values replace the above values
////////////////////////////////////////////////////////

/////////////////////////////////////////////
// CALIBRATION OF RTD1 TEMPERAURE TRANSDUCER
/////////////////////////////////////////////
// Calibration Steps:
// 1.Keep the transducer tip at ice water (0 degC)
// 2.Enter the value of ice water as shown in the reference thermometer
// 3.Keep the transducer tip at LN2 (-195.8 degC)
// 4.Enter the value of ice water as shown in the reference thermometer
/////////////////////////////////////////////

/* Includes */
#include "MyMain.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"

/* Declaration of external global variables */

/* Declaration of global variables */
CAL_INFO   cal_info;
CAL_PT_STR cal_pt_str[MAX_CALIB_PARAMS];
DPCALIB_TIME dpcalib_time;
uint32_t calib_timer;
int16_t calib_count;
uint8_t way_sec;
int32_t diff_count;
uint8_t second_time=0;

const CAL_PT_STR CAL_PT_STR_DEFAULT[] = { \
  {{LVL_CAL_PT1_XVAL,  LVL_CAL_PT2_XVAL,   LVL_CAL_PT3_XVAL},  0, {0, }, },  \
  {{DP_CAL_PT1_XVAL,   DP_CAL_PT2_XVAL,    DP_CAL_PT3_XVAL},   0, {0, }, },  \
  {{RTD1_CAL_PT1_XVAL, RTD1_CAL_PT2_XVAL,  RTD1_CAL_PT3_XVAL}, 0, {0, }, },  \
  {{RTD2_CAL_PT1_XVAL, RTD2_CAL_PT2_XVAL,  RTD2_CAL_PT3_XVAL}, 0, {0, }, },  \

};

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
 * Function name: Calib_init
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the initialization function for the programmable calibration parameters
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void Calib_init(void)
{
  memcpy((uint8_t*)&cal_pt_str, (uint8_t*)CAL_PT_STR_DEFAULT, sizeof(CAL_PT_STR));
  vp_cal_pt_rd_str = CAL_PT_DEFAULT;
  cal_par_str = CAL_PAR_DEFAULT;
  cal_info = CAL_INFO_DEFAULT;
  dpcalib_time=START_TIME;
  calib_count=60;
}

/*
 * *****************************************************************************
 * Function name: PerformRTDCalibration
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the FSM function for the Programming, management & processing of calibration data.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void PerformRTDCalibration(RTDCalibFactorStruct *Calib, double T2, double C2, double T3, double C3)
{
	double T1 = 23.0;
	double C1 = 8925.0;

	Calib->A1 = (T1*(C2-C3) + T2*(C3-C1) + T3*(C1-C2)) / (C1*C1*(C2-C3) + C2*C2*(C3-C1) + C3*C3*(C1-C2));
	Calib->A2 = (T1*(C2*C2-C3*C3) + T2*(C3*C3-C1*C1) + T3*(C1*C1-C2*C2)) / (C1*(C2*C2-C3*C3) + C2*(C3*C3-C1*C1) + C3*(C1*C1-C2*C2));
	Calib->A3 = T1-Calib->A1*C1*C1 - Calib->A2*C1;
}

/*
 * *****************************************************************************
 * Function name: Calib_proc
 * Created by: Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  This is the FSM function for the Programming, management & processing of calibration data.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
*/
void Calib_proc(void)
{
//	if((vp_cal_pt_rd_str.status == CALIB_ACTIVE) && (vp_cal_pt_rd_str.calibration_stop))
//	{
//		vp_cal_pt_rd_str.status = CALIB_INACTIVE;
//		TargetPage = 0;
//	    SendCmdToDisplay(LOAD_PAGE_CMD);
//	}

  switch (cal_info.state)
  {
    case CAL_VALIDATE:
      cal_info.state = CAL_DONE;
      break;

    case CAL_DONE:
      // Successfully transacted data with EEPROM for any previously raised requests.
      // will change to to "CAL_SEND"
      if ((vp_cal_pt_rd_str.step_ack == CALIB_REQ_VALIDATE) && (vp_cal_pt_rd_str.status == CALIB_ACTIVE))
      {
        // UI indicates that the calibration process is over. Validate the data and send a response back
    	if (vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD1)
    	{
    		RTDCalibFactorStruct CalibData = {0.0};

       	  // Calculate the calibration data for the RTD1 sensor:
    		PerformRTDCalibration(&CalibData, (double)vp_cal_pt_rd_str.rtd1_param[0], (double)cal_pt_str[CALIB_PARAM_RTD1].y[0], \
    										  (double)vp_cal_pt_rd_str.rtd1_param[1], (double)cal_pt_str[CALIB_PARAM_RTD1].y[1]);
      	  // Validate the calibration data for RTD1 sensor
          if (((CalibData.A1 > MIN_RTD_CONST_C1) && (CalibData.A1 < MAX_RTD_CONST_C1)) &&
         	  ((CalibData.A2 > MIN_RTD_CONST_C2) && (CalibData.A2 < MAX_RTD_CONST_C2)) &&
			  ((CalibData.A3 < MIN_RTD_OFFSET)   && (CalibData.A3 > MAX_RTD_OFFSET)))
          {
       	    // Calibration data valid. ACK it
      		cal_par_str.rtd1_param[0] = (float)CalibData.A1;
      		cal_par_str.rtd1_param[1] = (float)CalibData.A2;
      		cal_par_str.rtd1_param[2] = (float)CalibData.A3;
   		    vp_cal_pt_rd_str.step_ack = CALIB_DATA_VALID;
   		    SendCalibAckToUI(CALIB_DATA_VALID);
   			EEPROM_Write(RTD1_CALIB_ADDRESS, 0,(uint8_t*)&cal_par_str.rtd1_param[0],12);
   	        System_Status[EVENT_FLAG] |= EVT_RTD1_CALIB_SUCCESS;
          }
          else
          {
      	    // Calibration data not valid. NACK it
   		    vp_cal_pt_rd_str.step_ack = CALIB_DATA_INVALID;
   		    SendCalibAckToUI(CALIB_DATA_INVALID);
   		    // Set alarm flag
   		    System_Status[EVENT_FLAG] |= EVT_RTD1_CALIB_FAILURE;
          }
          //ULOG_DATA("LU1, value1:%.1f, value2:%.1f, Constant1:%.4f, Constant2:%.2f, Constant3:%.2f,",vp_cal_pt_rd_str.rtd1_param[0],vp_cal_pt_rd_str.rtd1_param[1],CalibData.A1,CalibData.A2,CalibData.A3);
    	}
    	else if (vp_cal_pt_rd_str.param_under_calib == CALIB_PARAM_RTD2)
    	{
    	  // Validate the calibration data for RTD2 sensor:
    		RTDCalibFactorStruct CalibData = {0.0};

       	  // Calculate the calibration data for the RTD1 sensor:
    		PerformRTDCalibration(&CalibData, (double)vp_cal_pt_rd_str.rtd2_param[0], (double)cal_pt_str[CALIB_PARAM_RTD2].y[0], \
    										  (double)vp_cal_pt_rd_str.rtd2_param[1], (double)cal_pt_str[CALIB_PARAM_RTD2].y[1]);
       	  // Validate the calibration data for RTD2 sensor
            if (((CalibData.A1 > MIN_RTD_CONST_C1) && (CalibData.A1 < MAX_RTD_CONST_C1)) &&
           	  ((CalibData.A2 > MIN_RTD_CONST_C2) && (CalibData.A2 < MAX_RTD_CONST_C2)) &&
  			  ((CalibData.A3 < MIN_RTD_OFFSET)   && (CalibData.A3 > MAX_RTD_OFFSET)))
            {
         	    // Calibration data valid. ACK it
        		cal_par_str.rtd2_param[0] = (float)CalibData.A1;
        		cal_par_str.rtd2_param[1] = (float)CalibData.A2;
        		cal_par_str.rtd2_param[2] = (float)CalibData.A3;
        		vp_cal_pt_rd_str.step_ack = CALIB_DATA_VALID;
        		SendCalibAckToUI(CALIB_DATA_VALID);
       			EEPROM_Write(RTD2_CALIB_ADDRESS, 0,(uint8_t*)&cal_par_str.rtd2_param[0],12);
       			System_Status[EVENT_FLAG] |= EVT_RTD2_CALIB_SUCCESS;
           }
           else
           {
        	 // Calibration data not valid. NACK it
     		 vp_cal_pt_rd_str.step_ack = CALIB_DATA_INVALID;
     		SendCalibAckToUI(CALIB_DATA_INVALID);
   	        System_Status[EVENT_FLAG] |= EVT_RTD2_CALIB_FAILURE;
          }
          //ULOG_DATA("LU1, value1:%.1f, value2:%.1f, Constant1:%.4f, Constant2:%.2f, Constant3:%.2f,",vp_cal_pt_rd_str.rtd2_param[0],vp_cal_pt_rd_str.rtd2_param[1],CalibData.A1,CalibData.A2,CalibData.A3);
    	}
    	else
    	{
    	  // Un-supported sensor type. NACK it
   		  vp_cal_pt_rd_str.step_ack = CALIB_DATA_INVALID;
    	}
      }
      System_Status[EVENT_FLAG] &= ~EVT_CALIB_PROCESS_STARTED;

      break;

    case CAL_START:
		vp_cal_pt_rd_str.step_num = 0;
		vp_cal_pt_rd_str.step_ack = 0;
		vp_cal_pt_rd_str.status = CALIB_ACTIVE;
		vp_cal_pt_rd_str.WaitSamples = 0;
		vp_cal_pt_rd_str.WaitForInput = 0;
		cal_info.state = CAL_STEP1;
        System_Status[EVENT_FLAG] |= EVT_CALIB_PROCESS_STARTED;
    	break;

    case CAL_STEP1:
    	if(vp_cal_pt_rd_str.step_ack == 1 && vp_cal_pt_rd_str.WaitForInput == 2)
    	{
    		vp_cal_pt_rd_str.step_num = 1;
    		vp_cal_pt_rd_str.step_ack = 0;
    		vp_cal_pt_rd_str.WaitForInput = 0;
    		cal_info.state = CAL_STEP2;
    		SendCalibAckToUI(3);
    	}
    	break;

    case CAL_STEP2:
    	if(vp_cal_pt_rd_str.step_ack == 1 && vp_cal_pt_rd_str.WaitForInput == 1)
    	{
    		vp_cal_pt_rd_str.step_ack = 0;
    		cal_info.state = CAL_VALIDATE;
    		 System_Status[EVENT_FLAG] |= EVT_CALIB_PROCESS_STARTED;
    	}
    	break;
    case CAL_STOP:
          //wait for the calling function to acknowledge
    	cal_info.state = CAL_STOP;
          break;
    case CAL_ERR:
      // Assign default values and wait for the calling function to acknowledge
      cal_par_str = CAL_PAR_DEFAULT;
      cal_info.state = CAL_DONE;
      break;

    default:
      cal_info.state = CAL_ERR;
      break;
  }
}


/* 
 * *********************************
 * *********************************
 * End of the module: Calibration.c
 * *********************************
 * *********************************
 */

