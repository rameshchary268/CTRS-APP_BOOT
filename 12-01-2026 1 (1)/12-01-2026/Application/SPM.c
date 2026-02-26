 /*
 * /////////////////////////////////////////////////////////////////////////////
 * Module: SPM.c
 * Author: Ilensys
 *
 * Created on 11 June, 2022, 10:31 AM
 * 
 * Description:
 * This module contains definitions of command execution functions.
 *  
 * Note:
 *  MCU supported: STM32L4P5VGT6
 * /////////////////////////////////////////////////////////////////////////////
*/

/* Includes */
#include "MyMain.h"
#include "DisplayCmdHandlers.h"
#include "DisplayTxHandlers.h"
#include "UISettings.h"

/* Declaration of external global variables */

/* Declaration of global variables */
SPM_INFO spm_info;
DATE_REM date_rem;
UPDATE Update;
//uint8_t flag_chk=0;
uint8_t flag_sch_fill=0;
//uint8_t flag_ln2=0;
uint8_t flag_store=0;
uint8_t date_store=0;
uint8_t hours;
uint8_t Buzzer_mute=1;
uint8_t day_week;
uint8_t LastDataLogTime;
uint8_t DataLogTimeOut=1;

const DayStruct DayTable[Max_Days] = {
		{ 0x00,	 0x80 },
		{ 0x01,	 0x02 },
		{ 0x02,	 0x04},
		{ 0x03,	 0x08 },
		{ 0x04,	 0x10 },
		{ 0x05,	 0x20 },
		{ 0x06,	 0x40 },
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
 * Function name: SPM_init()
 * Created by  :Arikanti venkatesh
 * Date:         01-Jul-2025
 * Description:
 *  This function initializes command processing parameters.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 * 
 * *****************************************************************************
 */
void SPM_init(void)
{
  spm_info = SPM_INFO_DEFAULT;
}

/*
 * *****************************************************************************
 * Function name: update_firmwware_time
 * Created by : Arikanti venkatesh
 * Date:        02-DEC-2023
 * Description:
 *  This routine monitors time to update firmware
 *
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void update_firmwware_time(void)
{
	 //UPDATE firmware
  if(firmware_timer_en)
  {
	  if(UpdateStruct.Update_MinutesSettings==0)
	  {
		  if(Date.Year > UpdateStruct.Update_YearSettings)
		  {
			  update_firmwware();
		  }
		  else if(Date.Year == UpdateStruct.Update_YearSettings)
		  {
			  if(Date.Month > UpdateStruct.Update_MonthSettings)
			  {
				  update_firmwware();
			  }
			  else if(Date.Month == UpdateStruct.Update_MonthSettings)
			  {
				  if (Date.Date > UpdateStruct.Update_DateSettings)
				  {
					  update_firmwware();
				  }
				  else if (Date.Date == UpdateStruct.Update_DateSettings)
				  {
					  if (hours > UpdateStruct.Update_HoursSettings-1)
					  {
						  update_firmwware();
					  }
					  else if (hours == UpdateStruct.Update_HoursSettings-1)
					  {
						  if (Time.Minutes>=UpdateStruct.Update_MinutesSettings+59)
							  update_firmwware();
					  }
				   }
			  }
		  }
	  }
	  else if(Date.Year > UpdateStruct.Update_YearSettings)
	  {
		  update_firmwware();
	  }
	  else if(Date.Year == UpdateStruct.Update_YearSettings)
	  {
		  if(Date.Month > UpdateStruct.Update_MonthSettings)
		  {
			  update_firmwware();
		  }
		  else if(Date.Month == UpdateStruct.Update_MonthSettings)
		  {
			  if (Date.Date > UpdateStruct.Update_DateSettings)
			  {
				  update_firmwware();
			  }
			  else if (Date.Date == UpdateStruct.Update_DateSettings)
			  {
				  if (hours > UpdateStruct.Update_HoursSettings)
				  {
					  update_firmwware();
				  }
				  else if (hours == UpdateStruct.Update_HoursSettings)
				  {
					  if (Time.Minutes>=UpdateStruct.Update_MinutesSettings-1)
						  update_firmwware();
				  }
			   }
		  }
	  }
  }
}

/*
 * *****************************************************************************
 * Function name: update_firmwware
 * Created by : Arikanti venkatesh
 * Date:        03-DEC-2023
 * Description:
 *  This routine monitors time to update firmware
 *

 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void update_firmwware(void)
{
	if(vp_rt_wr_str.PowerStatus )
	{
		if(Time.Minutes>=UpdateStruct.Update_MinutesSettings)
		{
			Update_system();
		}
	}
}
/*
 * *****************************************************************************
 * Function name: Track_schedule
 * Created by : Arikanti venkatesh
 * Date:        11-Mar-2023
 * Description:
 *  This routine monitors schedules like Monthly/Bi-weeklt/Weekly LN2 filling reminder,
 *  Daily LN2 Level check, Annual / Bi-annual Preventive maintenance and
 *  Reminder to LN2 supplier
 * Parameters:
 *  Input  : Drive state
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Track_schedule(void)
{
  Get_TimeStamp();
  uint8_t week_calc_day = week_cal(Date.Date, Date.Month  ,Date.Year+2000);
  //uint8_t week_rem=week_cal_rem(Date.Date, Date.Month  ,Date.Year+2000);
  day_week=Date.WeekDay;
  if(Date.Date !=date_store)
  {
	  flag_store=0;
  }
  else
  {
	  flag_store=1;
  }
  if((Time.Hours==11)&&(Time.Minutes==59)&&(Time.Seconds>50)&&(Time.TimeFormat==1)&&(flag_store==0))
  {
	  bat_down=1;
	  flag_store=1;
	  ULOG_DATA("LU1, Day completed");
	  bat_down=0;
	  date_store=Date.Date;
	  for(uint8_t x=0;x<11;x++)
	  {
		  login_cred[x].attempts=0;
	  }
  }
  else
  {
	   bat_down=0;
  }


  if(Time.Minutes != LastDataLogTime)
  	{
  		if(vp_evnt_wr_str.lid_info.lid_sts || (Time.Minutes == 0 || Time.Minutes == 15 || Time.Minutes == 30 || Time.Minutes == 45))
  		{
  			DataLogTimeOut = 1;
  			//LogGraphDataEn = 1;
  			LastDataLogTime = Time.Minutes;
  		}
  	}

  	if(DataLogTimeOut)
  	{
  		DataLogTimeOut = 0;
  		//iot_info.data_available=1;
  		//LogData here during lid open condition
  		char DummyData_buffer[4][10]={0,};
  		sprintf((char*)(DummyData_buffer[0]),"%.1f\u00B0C",vp_rt_wr_str.temp1);
  		sprintf((char*)(DummyData_buffer[1]),"%.1f\u00B0C",vp_rt_wr_str.temp2);
//  		sprintf((char*)(DummyData_buffer[2]),"%.1f",vp_rt_wr_str.dp_level);
//  		sprintf((char*)(DummyData_buffer[3]),"0x%x",(LevelData.ThermistorFaultBits <<8 | LevelData.ThermistorLevelBitStates));
  		ULOG_DATA("D01, T1:%s, T2:%s", vp_rt_wr_str.rtd1_status ? "--.-" : &DummyData_buffer[0][0],  vp_rt_wr_str.rtd2_status ? "--.-" : &DummyData_buffer[1][0] );
  	}
  if(Time.TimeFormat)
  {
	  if(Time.Hours<12)
	  hours=Time.Hours+12;
	  else
	  hours=Time.Hours;
  }
  else
  {
	  if(Time.Hours == 12)
	  hours=Time.Hours-12;
	  else
	  hours=Time.Hours;
  }
  update_firmwware_time();
  vp_param_rw_str.maint_rem.state=vp_param_rw_str.MaintRemainderEn;
  vp_param_rw_str.maint_rem.time.date=vp_param_rw_str.Maint_DateSettings ;
  vp_param_rw_str.maint_rem.time.month=vp_param_rw_str.Maint_MonthSettings ;
  vp_param_rw_str.maint_rem.time.year = vp_param_rw_str.Maint_YearSettings ;

  if(vp_param_rw_str.RTC_TimeFormat == 1)
  {
	  vp_param_rw_str.maint_rem.time.hour =vp_param_rw_str.Maint_HoursSettings;
  }
  else
  {
	  if(vp_param_rw_str.Maint_AMPMSettings)
	  {
		  if(vp_param_rw_str.Maint_Hours12Settings<12)
		  vp_param_rw_str.maint_rem.time.hour=vp_param_rw_str.Maint_Hours12Settings+12 ;
		  else
		  vp_param_rw_str.maint_rem.time.hour=vp_param_rw_str.Maint_Hours12Settings;
	  }
	  else
	  {
		  if(vp_param_rw_str.Maint_Hours12Settings==12)
		  vp_param_rw_str.maint_rem.time.hour=vp_param_rw_str.Maint_Hours12Settings-12;
		  else
		  vp_param_rw_str.maint_rem.time.hour=vp_param_rw_str.Maint_Hours12Settings;
	  }
  }
  vp_param_rw_str.maint_rem.time.minute=vp_param_rw_str.Maint_MinutesSettings;




  // Run the maintenance reminder
  if ((vp_param_rw_str.maint_rem.state == 1))
  {
	  if(Date.Year > vp_param_rw_str.maint_rem.time.year)
	  {
		  vp_evnt_wr_str.maint_info.status = 1;
		  System_Status[WARNING_FLAG] |= WRN_MAINTENANCE_REMINDER;
	  }
	  else if(Date.Year == vp_param_rw_str.maint_rem.time.year)
	  {
		 if(Date.Month > vp_param_rw_str.maint_rem.time.month)
		 {
			 vp_evnt_wr_str.maint_info.status = 1;
			 System_Status[WARNING_FLAG] |= WRN_MAINTENANCE_REMINDER;
		 }else if(Date.Month == vp_param_rw_str.maint_rem.time.month)
		 {
			if(Date.Date > vp_param_rw_str.maint_rem.time.date)
			{
				 vp_evnt_wr_str.maint_info.status = 1;
				 System_Status[WARNING_FLAG] |= WRN_MAINTENANCE_REMINDER;
			}else if(Date.Date == vp_param_rw_str.maint_rem.time.date)
			{
				if(hours >  vp_param_rw_str.maint_rem.time.hour)
				{
					 vp_evnt_wr_str.maint_info.status = 1;
					 System_Status[WARNING_FLAG] |= WRN_MAINTENANCE_REMINDER;
				}else if (hours == vp_param_rw_str.maint_rem.time.hour)
				{
					if(Time.Minutes>=vp_param_rw_str.maint_rem.time.minute)
					{
						 vp_evnt_wr_str.maint_info.status = 1;
						 System_Status[WARNING_FLAG] |= WRN_MAINTENANCE_REMINDER;
					}else
					{
						 vp_evnt_wr_str.maint_info.status = 0;
						 System_Status[WARNING_FLAG] &= ~WRN_MAINTENANCE_REMINDER;
					}
				}else
				{
					 vp_evnt_wr_str.maint_info.status = 0;
					 System_Status[WARNING_FLAG] &= ~WRN_MAINTENANCE_REMINDER;
				}
			}else
			{
				 vp_evnt_wr_str.maint_info.status = 0;
				 System_Status[WARNING_FLAG] &= ~WRN_MAINTENANCE_REMINDER;
			}
		 }else
		  {
			 vp_evnt_wr_str.maint_info.status = 0;
			 System_Status[WARNING_FLAG] &= ~WRN_MAINTENANCE_REMINDER;
		  }
	  }else
	   {
		 vp_evnt_wr_str.maint_info.status = 0;
		 System_Status[WARNING_FLAG] &= ~WRN_MAINTENANCE_REMINDER;
	   }
  }
  else if ((System_Status[WARNING_FLAG] & WRN_MAINTENANCE_REMINDER) != 0)
	{
	  vp_evnt_wr_str.maint_info.status = 0;
	  System_Status[WARNING_FLAG] &= ~WRN_MAINTENANCE_REMINDER;
	}

//Fill  Remainder

  vp_param_rw_str.Fill_Rem.state = vp_param_rw_str.FillRemainderEn;
  vp_param_rw_str.Fill_Rem.time.year = vp_param_rw_str.FILLR_YearSettings;
  vp_param_rw_str.Fill_Rem.time.month = vp_param_rw_str.FILLR_MonthSettings;
  vp_param_rw_str.Fill_Rem.time.date = vp_param_rw_str.FILLR_DateSettings;

if(vp_param_rw_str.RTC_TimeFormat == 1)
{
	vp_param_rw_str.Fill_Rem.time.hour = vp_param_rw_str.FILLR_HoursSettings;
}
else
{
	if(vp_param_rw_str.FILLR_AMPMSettings == 1)
	{
		if(vp_param_rw_str.FILLR_Hours12Settings < 12)
			vp_param_rw_str.Fill_Rem.time.hour = vp_param_rw_str.FILLR_Hours12Settings + 12;
		else
			vp_param_rw_str.Fill_Rem.time.hour = vp_param_rw_str.FILLR_Hours12Settings;
	}
	else
	{
		if(vp_param_rw_str.FILLR_AMPMSettings == 12)
			vp_param_rw_str.Fill_Rem.time.hour = vp_param_rw_str.FILLR_Hours12Settings - 12;
		else
			vp_param_rw_str.Fill_Rem.time.hour = vp_param_rw_str.FILLR_Hours12Settings;
	}
}
vp_param_rw_str.Fill_Rem.time.minute = vp_param_rw_str.FILLR_MinutesSettings;

if(vp_param_rw_str.Fill_Rem.state == 1)
{
	if(Date.Year > vp_param_rw_str.Fill_Rem.time.year)
	{
		vp_evnt_wr_str.FILLR_Info.status = 1;
		System_Status[WARNING_FLAG] |=  WRN_FILL_REMINDER;
	}
	else if(Date.Year == vp_param_rw_str.Fill_Rem.time.year)
	{
		if(Date.Month > vp_param_rw_str.Fill_Rem.time.month)
		{
			vp_evnt_wr_str.FILLR_Info.status = 1;
			System_Status[WARNING_FLAG] |=  WRN_FILL_REMINDER;
		}
		else if(Date.Month == vp_param_rw_str.Fill_Rem.time.month)
		{
			if(Date.Date > vp_param_rw_str.Fill_Rem.time.date)
			{
				vp_evnt_wr_str.FILLR_Info.status = 1;
				System_Status[WARNING_FLAG] |=  WRN_FILL_REMINDER;
			}
			else if(Date.Date == vp_param_rw_str.Fill_Rem.time.date)
			{
				if(hours > vp_param_rw_str.Fill_Rem.time.hour)
				{
					vp_evnt_wr_str.FILLR_Info.status = 1;
					System_Status[WARNING_FLAG] |=  WRN_FILL_REMINDER;
				}
				else if(hours == vp_param_rw_str.Fill_Rem.time.hour)
				{
					if(Time.Minutes >= vp_param_rw_str.Fill_Rem.time.minute)
					{
						vp_evnt_wr_str.FILLR_Info.status = 1;
						System_Status[WARNING_FLAG] |=  WRN_FILL_REMINDER;
					}
					else
					{
						vp_evnt_wr_str.FILLR_Info.status = 0;
						System_Status[WARNING_FLAG] &=  ~WRN_FILL_REMINDER;
					}
				}
				else
				{
					vp_evnt_wr_str.FILLR_Info.status = 0;
					System_Status[WARNING_FLAG] &=  ~WRN_FILL_REMINDER;
				}
			}
			else
			{
				vp_evnt_wr_str.FILLR_Info.status = 0;
				System_Status[WARNING_FLAG] &=  ~WRN_FILL_REMINDER;
			}

		}
		else
		{
			vp_evnt_wr_str.FILLR_Info.status = 0;
			System_Status[WARNING_FLAG] &=  ~WRN_FILL_REMINDER;
		}
	}
	else
	{
		vp_evnt_wr_str.FILLR_Info.status = 0;
		System_Status[WARNING_FLAG] &=  ~WRN_FILL_REMINDER;
	}
}
else if((System_Status[WARNING_FLAG] & WRN_FILL_REMINDER) != 0)
{
	vp_evnt_wr_str.FILLR_Info.status = 0;
	System_Status[WARNING_FLAG] &=  ~WRN_FILL_REMINDER;
}




//Schedule Manual Fill

vp_param_rw_str.sch_fill_rem.state=	vp_param_rw_str.ScheduledFillRemainderEn;
vp_param_rw_str.sch_fill_rem.time.day = vp_param_rw_str.Fill_DaySettings ;
if(vp_param_rw_str.RTC_TimeFormat == 1)
{

  vp_param_rw_str.sch_fill_rem.time.hour=vp_param_rw_str.Fill_HoursSettings;

}
else
{
  if(vp_param_rw_str.Fill_AMPMSettings)
  {
	  if(vp_param_rw_str.Fill_Hours12Settings<12)
	  vp_param_rw_str.sch_fill_rem.time.hour=vp_param_rw_str.Fill_Hours12Settings+12 ;
	  else
	  vp_param_rw_str.sch_fill_rem.time.hour=vp_param_rw_str.Fill_Hours12Settings;
  }
  else
  {
	  if(vp_param_rw_str.Fill_Hours12Settings==12)
	  vp_param_rw_str.sch_fill_rem.time.hour=vp_param_rw_str.Fill_Hours12Settings-12;
	  else
	  vp_param_rw_str.sch_fill_rem.time.hour=vp_param_rw_str.Fill_Hours12Settings;
  }
}
vp_param_rw_str.sch_fill_rem.time.minute=	vp_param_rw_str.Fill_MinutesSettings ;

// Run the LN2 Fill Reminder


if(Date.Date !=date_rem.date_sch_fill)
{
  flag_sch_fill=0;
}
else
{
  flag_sch_fill=1;
}

if ((flag_sch_fill ==0) && (clear_reminder !=6)&&(vp_param_rw_str.sch_fill_rem.state == 1) &&
 ((week_calc_day & vp_param_rw_str.sch_fill_rem.time.day )||(vp_param_rw_str.sch_fill_rem.time.day & 0x01)))
{
//	if(hours > vp_param_rw_str.sch_fill_rem.time.hour)
//	{
//		vp_evnt_wr_str.maint_info.status = 1;
//		System_Status[WARNING_FLAG] |= WRN_LN2_FILL_REMINDER;
//	}else
	if(hours == vp_param_rw_str.sch_fill_rem.time.hour)
	{
//		if(Time.Minutes > vp_param_rw_str.sch_fill_rem.time.minute)
//		{
//			vp_evnt_wr_str.maint_info.status = 1;
//			System_Status[WARNING_FLAG] |= WRN_LN2_FILL_REMINDER;
//		}
//		else
		if(Time.Minutes == vp_param_rw_str.sch_fill_rem.time.minute)
		{
			if (( System_Status[WARNING_FLAG] & WRN_LN2_FILL_REMINDER) == 0)
			{
			  flag_sch_fill=1;
			  vp_evnt_wr_str.maint_info.status = 1;
			  System_Status[WARNING_FLAG] |= WRN_LN2_FILL_REMINDER;
			}
		}
	}
}
else if((clear_reminder ==6)||(vp_param_rw_str.sch_fill_rem.state==0))
{
  if (( System_Status[WARNING_FLAG] & WRN_LN2_FILL_REMINDER) != 0)
  {
	clear_reminder =0;
	EEPROM_Write(SCH_DATE,0, &Date.Date,1);
	date_rem.date_sch_fill=Date.Date;
	vp_evnt_wr_str.maint_info.status = 0;
	System_Status[WARNING_FLAG] &= ~WRN_LN2_FILL_REMINDER;
  }
  else if(vp_param_rw_str.sch_fill_rem.state==0)
  {
	  vp_evnt_wr_str.maint_info.status = 0;
	  System_Status[WARNING_FLAG] &= ~WRN_LN2_FILL_REMINDER;
  }
}




  // Track the data logs - every 1 minute, if lid is open. Otherwise, every 15 minutes
}         // End of function definition "Track_schedule"


uint32_t week_cal(uint8_t date, uint8_t mon, uint16_t year)
{
	 int month [ 12 ] = { 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
	 int  i  , calci = 0 ;
	 uint8_t week_day,week_bit;

	 if ((year%400==0) || ((year%4==0) && (year%100 !=0)))
	  month [ 1 ] = 29 ;
	 for (i=0;i<mon-1;i++)
	 {
		 calci = calci+month[i] ;
	 }
	 if ((year%400==0) || ((year%4==0) && (year%100 !=0)))
	 {
		 calci = calci + (date+year+ (year/4)-2)-1;
	 }
	 else
	 {
		 calci = calci + (date+year+ (year/4)-2);
	 }
	 week_day = calci % 7 ;
//	 for(uint8_t Ctr = 0; Ctr < Max_Days; Ctr++)
//	 {
//		if(week_day == DayTable[Ctr].Day)
//		{
//			return week_bit=DayTable[Ctr].Daybits;
//		}
//	 }
	 return week_bit=DayTable[week_day].Daybits;
}

uint32_t week_cal_rem(uint8_t date, uint8_t mon, uint16_t year)
{
	 int month [ 12 ] = { 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ;
	 int  i  , calci = 0 ;
	 uint8_t week_day;

	 if ((year%400==0) || ((year%4==0) && (year%100 !=0)))
	  month [ 1 ] = 29 ;
	 for (i=0;i<mon-1;i++)
	 {
		 calci = calci+month[i] ;
	 }
	 if ((year%400==0) || ((year%4==0) && (year%100 !=0)))
	 {
		 calci = calci + (date+year+ (year/4)-2)-1;
	 }
	 else
	 {
		 calci = calci + (date+year+ (year/4)-2);
	 }
	 week_day = calci % 7 ;
	 return week_day;
}


/* 
 * *************************
 * *************************
 * End of the module: SPM.c
 * *************************
 * *************************
 */
