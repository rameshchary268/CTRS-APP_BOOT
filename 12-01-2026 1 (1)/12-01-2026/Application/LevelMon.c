/* 
 * /////////////////////////////////////////////////////////////////////////////
 * Module: LevelMon.c
 * Author: Ilensys
 *
 * Created on 11 June, 2022, 10:31 AM
 * 
 * Description:
 * This module contains definitions of LN2 level monitoring functions.
 *  
 * Note:
 *  MCU supported: STM32L4P5VGT6
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DispUIkeyhandler.h"
#include "FactoryReset.h"
#include "DisplayCmdHandlers.h"
#include "LevelMon.h"

/* Declaration of external global variables */
uint16_t prev_cal_value=0;
//ThermFillState ThermArrayState = THERM_STATE_NOT_CHANGED;

/* Declaration of global variables */
LEVEL_INFO level_info;

//Thermistor level data table
const ThermLevelStruct ThermLevelTable[MAX_THERMISTOR_SENSORS+1] = {
		{ 0x00,	 0x00,	0 },
		{ 0x01,	 0x01,	1 },
		{ 0x02,	 0x03,	2 },
		{ 0x04,	 0x07,	3 },
};

//ADC count limits for internal DP sensor calibration

uint8_t ThermLevelBitState = 0;
uint8_t Thermistor_level=0,prev_Thermistor_level=0;
LevelDataStruct LevelData = {0};
uint16_t ThermFilltered[MAX_THERMISTOR_SENSORS] = {0};
uint8_t FillterSampleCount = 0;
uint8_t ln2usagetimeout=0;
uint8_t valve3waytimeout=0;
ThermistorStates ThermistorStatus[MAX_THERMISTOR_SENSORS] = {THERM_STATE_G};
LN2USAGE Ln2Usage=0;
uint8_t sec_count;
uint32_t WayTimerflag;
uint8_t PulsewayTrainToRun=0;
uint8_t actuation=1;
uint8_t one_day_3way=0;
uint8_t dp_fault=0;
uint8_t master_shed_fill=0;

LN2_USAGE   Ln2_usage;

/* Declaration of static local variables */

/* Declaration of local functions */

/* 
 * **************************************
 * **************************************
 * Function Declarations start here
 * **************************************
 * **************************************
 */
float IIRLPF_THERM(float Vin, float VoutPrev,float TimeConstFactor1,float TimeConstFactor2)
{
//	static float VoutPrev = 0;
//	const float TimeConstFactor1 =0.333333;//0.2;//0.111111; //a/(1+a)
//	const float TimeConstFactor2 =0.666667;// 0.8;//0.888889; //1/(1+a)
	VoutPrev = (TimeConstFactor1 * Vin) + (TimeConstFactor2*VoutPrev);
	return VoutPrev;
}


/*
 * *****************************************************************************
 * Function name: GetThermisterMeasuredLevel
 * Created by : Veeravijayan Selvaraj
 * Date:            05-May-2023
 * Description:
 *  Function to calculate Thermistor reading
 * Parameters:
 *  Input  : Status of Thermistor array
 *  Return : Measured level
 * Note:
 *
 * *****************************************************************************
 */
uint8_t GetThermisterMeasuredLevel(uint8_t ThermLevelBits)
{
	//Get Thermister Level
	for(uint8_t Ctr = 0; Ctr <= MAX_THERMISTOR_SENSORS; Ctr++)
	{
		if(ThermLevelBits == ThermLevelTable[Ctr].ThermLevelPattern)
		{
			Thermistor_level=Ctr;
			LevelData.Thermistor_status=Ctr;
			return ThermLevelTable[Ctr].ThermLevel;
		}
	}
	Thermistor_level=0;
	LevelData.Thermistor_status=0;
	return 0;
}

/*
 * *****************************************************************************
 * Function name: ProcessThermSensorReading
 * Created by : Veeravijayan Selvaraj
 * Date:            05-May-2023
 * Description:
 *  Function to get status of individual thermistor sensor, Uses ADC Reading
 * Parameters:
 *  Input  : None
 *  Return : Status of the thermistor array
 * Note:
 *
 * *****************************************************************************
 */
uint8_t  ProcessThermSensorReading(void)
{
	for(uint8_t Ctr=0; Ctr < MAX_THERMISTOR_SENSORS; Ctr++)
	{
		ThermFilltered[Ctr] = (uint16_t)IIRLPF_THERM((float)ADC_info.buffer[ADC_CH_THERMISTOR1+Ctr], (float)ThermFilltered[Ctr],0.333333,0.666667);

		if((LevelData.Enable_thermistor & ThermLevelTable[Ctr+1].ThermBitMask))//(ThermistorStatus[Ctr] != THERM_STATE_O)
		{
			if((LevelData.ThermistorLevelBitStates & ThermLevelTable[Ctr+1].ThermBitMask) && \
			   (ThermFilltered[Ctr] < THERM_LN2_RECEDING_SENSE_LIMIT))
			{
				LevelData.ThermistorLevelBitStates &= ~ThermLevelTable[Ctr+1].ThermBitMask;
				LevelData.ThermistorFaultBits &= ~ThermLevelTable[Ctr+1].ThermBitMask;
				level_info.err[Ctr] = 0;
				ThermistorStatus[Ctr] = THERM_STATE_G;
			}
			else if(!(LevelData.ThermistorLevelBitStates & ThermLevelTable[Ctr+1].ThermBitMask) && \
			   (ThermFilltered[Ctr] > THERM_LN2_FILLING_SENSE_LIMIT) && \
			   (ThermFilltered[Ctr] < THERM_OPEN_SENSE_LIMIT))
			{
				LevelData.ThermistorLevelBitStates |= ThermLevelTable[Ctr+1].ThermBitMask;
				LevelData.ThermistorFaultBits &= ~ThermLevelTable[Ctr+1].ThermBitMask;
				level_info.err[Ctr] = 0;
				ThermistorStatus[Ctr] = THERM_STATE_L;
			}
			else if(ThermFilltered[Ctr] > THERM_OPEN_SENSE_LIMIT)
			{
				LevelData.ThermistorLevelBitStates &= ~ThermLevelTable[Ctr+1].ThermBitMask;
				LevelData.ThermistorFaultBits |= ThermLevelTable[Ctr+1].ThermBitMask;
				level_info.err[Ctr] = 1;
				ThermistorStatus[Ctr] = THERM_STATE_O;
			}
		}
		else
		{
			LevelData.ThermistorFaultBits &= ~ThermLevelTable[Ctr+1].ThermBitMask;
			LevelData.ThermistorLevelBitStates &= ~ThermLevelTable[Ctr+1].ThermBitMask;
		}
	}
	return LevelData.ThermistorFaultBits;
}
/*
 * *****************************************************************************
 * Function name: Level_Mon_Proc
 * Created by : Veeravijayan Selvaraj
 * Date:            05-May-2023
 * Description:
 *  Function measure and monitor the LN2 level
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *	Need to revisit and optimize the level measurement logic
 * *****************************************************************************
 */
void Level_Mon_Proc(void)
{
          ProcessThermSensorReading();
	//LevelData.ThermistorLevel = GetThermisterMeasuredLevel(LevelData.ThermistorLevelBitStates);
	//LevelData.ThermistorFaultStates = THERM_ARRAY_OK;
	if((LevelData.NoOfSamplesProcessed >= DP_SETTLING_SAMPLE_COUNT))
	{
		if(LevelData.ThermistorFaultBits)
		{
			vp_rt_wr_str.level_status=4;
		}
		else if(LevelData.ThermistorLevelBitStates)
		{
			vp_rt_wr_str.level_status =(uint16_t)GetThermisterMeasuredLevel(LevelData.ThermistorLevelBitStates);
		}
		else
		{
			vp_rt_wr_str.level_status=0;
		}
		if(vp_param_rw_str.Empty_en && (LevelData.ThermistorFaultBits & THERMISTOR_1_LEVEL_MASK))
		{
			System_Status[ERROR_FLAG_0] |= ERR_EMPTY_SENSOR_FAIL;
		}
		else
		{
			System_Status[ERROR_FLAG_0] &= ~ERR_EMPTY_SENSOR_FAIL;
		}
		if(vp_param_rw_str.Refillsoon_en && (LevelData.ThermistorFaultBits & THERMISTOR_2_LEVEL_MASK))
		{
			System_Status[ERROR_FLAG_0] |= ERR_REFILL_SENSOR_FAIL;
		}
		else
		{
			System_Status[ERROR_FLAG_0] &= ~ERR_REFILL_SENSOR_FAIL;
		}
		if(vp_param_rw_str.Fill_en && (LevelData.ThermistorFaultBits & THERMISTOR_3_LEVEL_MASK))
		{
			System_Status[ERROR_FLAG_0] |= ERR_FILL_SENSOR_FAIL;

		}
		else
		{
			System_Status[ERROR_FLAG_0] &= ~ERR_FILL_SENSOR_FAIL;
		}

	}
	else if(1)
	{
		LevelData.NoOfSamplesProcessed++;
	}
}

/* 
 * *****************************************************************************
 * Function name: LevelMon_init()
 * Created by : Natarajan Kanniappan
 * Date:            01-Jul-2022
 * Description:
 *  This function initializes LN2 Level Monitoring  parameters.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void LevelMon_init(void)
{
	LevelData.ThermistorFaultBits = 0;
	//LevelData.ThermistorFaultStates = THERM_ARRAY_OK;

	LevelData.ThermistorLevel = 0;
	LevelData.ThermistorLevelBitStates = 0;
	LevelData.NoOfSamplesProcessed = 0;
	//LevelData.CalculatedLevel = 0.0;
	//LevelData.CalculatedThermLevel = 0;


}

//extern PulseTrainState RunPulseTrainDrive(void);
/* 
 * *****************************************************************************
 * Function name: LevelMon_proc()
 * Created by : Natarajan Kanniappan
 * Date:            01-Jul-2022
 * Description:
 *  This is the State Machine to handle various LN2 level monitoring activities
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void LevelMon_proc(void)
{
  if(ADC_info.ADCDataReady)
  {
	  ADC_info.ADCDataReady = 0;
	  Level_Mon_Proc();
	  //ln2usage();
  }
  return;
}

/*void ln2usage(void)
{
	//float Level1,Time1,Time2,Level2,datausage;
	uint8_t Lu_hour;

	float Temp_hour;
	if(Time.TimeFormat)
	{
		if(Time.Hours<12)
		{
			Lu_hour=Time.Hours+12;
		}
		else
		{
			Lu_hour=Time.Hours;
		}
	}
	else
	{
		if( Time.Hours==12)
		{
			Lu_hour=0;
		}
		else
		{
			Lu_hour=Time.Hours;
		}
	}
	if(Date.Date != Ln2_usage.date_change)
	{
		Ln2_usage.dayincrease++;
		Ln2_usage.date_change=Date.Date;
	}
	else
	{
        //do nothing
	}
	if((vp_rt_wr_str.fill_info.fill_sts==0)&&(FillValveState.FillRequestOn==0)&&((StartFillEvent.StartLevelFillRequest==0)))
	{
		switch(Ln2Usage)
		{
		case TIME_START:
			ln2usagetimeout=0;
			RegisterAlarm(USAGE_TIMER, &ln2usagetimeout, 60*15, SYNC_ON_REGISTER);
			Ln2_usage.Level1=0;
			Ln2_usage.Time1=0;
			Ln2_usage.Time2=0;
			Ln2_usage.Level2=0;
			Ln2Usage=INIT_VALUE;
			break;
		case INIT_VALUE:
			if(ln2usagetimeout)
			{
				 Ln2_usage.Level1=DP_info.cascade_value;
				 Ln2_usage.min=Time.Minutes;
				 Ln2_usage.Time1=Lu_hour+Ln2_usage.min/60;
				 Ln2Usage=CALCULATION;
				 Ln2_usage.date_change=Date.Date;
				 Ln2_usage.dayincrease=0;
			}
			else
			{
				//do nothing
			}
			break;
		case CALCULATION:
			if(Time_start)
			{
				Ln2_usage.Level1=DP_info.cascade_value;
				Ln2_usage.min=Time.Minutes;
				Ln2_usage.Time1=Lu_hour+Ln2_usage.min/60;
				Ln2_usage.date_change=Date.Date;
				Ln2_usage.dayincrease=0;
			}

			if(ln2usagetimeout)
			{
				if((Ln2_usage.Level1-DP_info.cascade_value)>=0.5)
				{
					Ln2_usage.Level2=DP_info.cascade_value;
					Ln2_usage.min=Time.Minutes;
					Temp_hour=Lu_hour+Ln2_usage.min/60;
					Ln2_usage.Time2=Temp_hour+Ln2_usage.dayincrease*24;
					Ln2_usage.datausage=(Ln2_usage.Level1-Ln2_usage.Level2)*24/(Ln2_usage.Time2-Ln2_usage.Time1);//current ln2 usage
					Ln2_usage.Level1=Ln2_usage.Level2;
					Ln2_usage.Time1=Temp_hour;
					Ln2_usage.date_change=Date.Date;
					Ln2_usage.dayincrease=0;

					if((Ln2_usage.datausage>=0)&&(Ln2_usage.datausage<500))
					{
						if((Ln2_usage.pre_datausage==0)&&(Ln2_usage.pre_ln2usage==0))
						{
							Ln2_usage.pre_ln2usage=Ln2_usage.datausage;
							Ln2_usage.pre_datausage=Ln2_usage.datausage;//previous average
						}
						else
						{
							Ln2_usage.pre_ln2usage=0.2*Ln2_usage.pre_datausage+0.8*Ln2_usage.datausage;//current average ln2 usage
						}
						if(Ln2_usage.pre_ln2usage>1.2*Ln2_usage.pre_datausage)
						{
							Ln2_usage.pre_ln2usage=1.2*Ln2_usage.pre_datausage;
						}
						Ln2_usage.datausage_cm=Ln2_usage.datausage*2.54;
						if(Ln2_usage.datausage>2*Ln2_usage.pre_datausage)
						{
							if(vp_param_rw_str.LN2UsageAlarmDelaySwitch)
							{
								System_Status[WARNING_FLAG] |= WRN_LN2_USAGE_HIGH;
							}
							if(vp_param_rw_str.lvlunits)
							{
								SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage*10));
							}
							else
							{
								SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage_cm*10));
							}

							 EEPROM_Write(ln2_usage, 0,(uint8_t*)&Ln2_usage.datausage,4);
							 ULOG_DATA("LU1, Current_Ln2_Usage:%.1f, Mean_Ln2_Usage:%.1f,",Ln2_usage.datausage,Ln2_usage.pre_datausage);
							 SendN16DataToDisplay(0x08904E,1);//enable disble of red ln2 usage
						}
						else
						{
							if(vp_param_rw_str.lvlunits)
							{
								SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage*10));
							}
							else
							{
								SendN16DataToDisplay(0x089030,(uint16_t)(Ln2_usage.datausage_cm*10));
							}
							 EEPROM_Write(ln2_usage, 0,(uint8_t*)&Ln2_usage.datausage,4);
							 ULOG_DATA("LU1, Current_Ln2_Usage:%.1f, Mean_Ln2_Usage:%.1f,",Ln2_usage.datausage,Ln2_usage.pre_datausage);
							 SendN16DataToDisplay(0x08904E,0);//enable disble of red ln2 usage
							 System_Status[WARNING_FLAG] &= ~WRN_LN2_USAGE_HIGH;
						}
						Ln2_usage.pre_datausage_cm=Ln2_usage.pre_datausage*2.54;
						if(vp_param_rw_str.lvlunits)
						{
							SendN16DataToDisplay(0x08905A,(uint16_t)(Ln2_usage.pre_datausage*10));
						}
						else
						{
							SendN16DataToDisplay(0x08905A,(uint16_t)(Ln2_usage.pre_datausage_cm*10));
						}
//						EEPROM_Write(pre_mean_usage, 0,(uint8_t*)&pre_datausage,4);
						Ln2_usage.pre_datausage=Ln2_usage.pre_ln2usage;
						EEPROM_Write(High_ln2_usage, 0,(uint8_t*)&Ln2_usage.pre_ln2usage,4);
					}
					else
					{
						Ln2_usage.datausage=Ln2_usage.datausage_cm/2.54;
					}
				}
				else if((Ln2_usage.Level1-DP_info.cascade_value)<-0.3)
				{
					Ln2_usage.Level1=DP_info.cascade_value;
					Ln2_usage.min=Time.Minutes;
					Ln2_usage.Time1=Lu_hour+Ln2_usage.min/60;
					Ln2_usage.date_change=Date.Date;
					Ln2_usage.dayincrease=0;
				}
			}

			break;
		}
	}

	if(Time_start || Ln2_usage.Level_change)
	{
		Ln2_usage.Level1=DP_info.cascade_value;
		Ln2_usage.min=Time.Minutes;
		Ln2_usage.Time1=Lu_hour+Ln2_usage.min/60;
		Ln2_usage.date_change=Date.Date;
		Ln2_usage.dayincrease=0;
		Ln2_usage.Level_change=0;
	}

	if(vp_param_rw_str.LN2UsageAlarmDelaySwitch ==0)
	{
		System_Status[WARNING_FLAG] &= ~WRN_LN2_USAGE_HIGH;
	}
}*/
/* 
 * *********************************
 * *********************************
 * End of the module: LevelMon.c
 * *********************************
 * *********************************
 */

