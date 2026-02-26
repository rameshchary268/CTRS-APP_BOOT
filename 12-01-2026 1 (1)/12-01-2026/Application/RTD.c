/*
 * RTD.c
 *
 *  Created on: Sep 8, 2025
 *      Author: RameshcharyVadla
 */


#include <RTD.h>
#include "MyMain.h"

RTD_INFO rtd_info[MAX_RTD];


//VP_CAL_RD_STR    cal_par_str;

uint8_t deint_complete=0;
extern SPI_HandleTypeDef hspi1;

uint32_t reinit_spi3_flag=0;
uint8_t reinitialize_SPI=0;
volatile uint8_t SPITxRxCptl[2] = {1, 1};
volatile uint8_t RTDIndex = 0;
const uint8_t RTD_TxData[] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t RTD_ErrReg[] = { 0x07, 0xFF };
uint8_t chck_ctr=0;
volatile uint8_t SPIErrorDetected = 0;
/* MAX31865 fault clear command */
const uint8_t rtd_flt_clr[] = {
  0x80, 0xC2,            // Address of register (0x00) to be written to clear the fault with command byte followed
};
uint8_t RTD_ErrData[2] = {0};



uint8_t rtd_config[] = {
  0x80, 0xC0,            // Address of register (0x00) write command with configuration byte succeeding
  0x83, 0x58,            // Address of register (0x03) write command with High-temperature threshold - MS byte succeeding
  	  	0xA4,            // Address of register (0x04) write command with High-temperature threshold - LS byte succeeding
        0x09,            // Address of register (0x05) write command with Low-temperature threshold  - MS byte succeeding
		0x9A,            // Address of register (0x06) write command with Low-temperature threshold  - LS byte succeeding
};


void RTD_init(void)
{
  // Parameter initialization
  rtd_info[RTD_1] = RTD_INFO_DEFAULT;
  rtd_info[RTD_2] = RTD_INFO_DEFAULT;
  rtd_info[RTD_1].CsPort = GPIOE;
  rtd_info[RTD_1].CsPin = 0x01<<11; //PD_5
  rtd_info[RTD_2].CsPort = GPIOE;
  rtd_info[RTD_2].CsPin = 0x01<<12; //PB_3
  rtd_info[RTD_1].InitSeq = 0;
  rtd_info[RTD_2].InitSeq = 0;
  rtd_info[RTD_1].fault_status = 0;
  rtd_info[RTD_2].fault_status = 0;
  rtd_info[RTD_1].fault_ctr = 0;
  rtd_info[RTD_2].fault_ctr = 0;
  Set_delay(SW_TMR_RTD_DLY, 10, &rtd_info[0].poll_flag);
  //vp_param_rw_str.t2enable = 0;

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, 1);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
  TempMon_init();
}

void re_initialize_SPI3()
{
	if((deint_complete==0))
	{
		HAL_SPI_DeInit(&hspi1);
		GPIOD->MODER  &= ~(GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1);
		GPIOD->MODER  |=  GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0;
		GPIOC->MODER  &= ~(GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1);
		GPIOC->MODER  |=  GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0 ;
		GPIOB->MODER  &= ~(GPIO_MODER_MODE3_1);
		GPIOB->MODER  |=  GPIO_MODER_MODE3_0;
		HAL_GPIO_WritePin(rtd_info[0].CsPort, rtd_info[0].CsPin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(rtd_info[1].CsPort, rtd_info[1].CsPin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_ODR_OD10, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_ODR_OD11, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_ODR_OD6, GPIO_PIN_RESET);
		Set_delay(SW_TMR_REINIT_SPI_TIMEOUT, 100, &reinit_spi3_flag);
		vp_param_rw_str.Short_detection_rtd2++;
		vp_param_rw_str.Short_detection_rtd1++;
		vp_param_rw_str.Open_detection_rtd1++;
		vp_param_rw_str.Open_detection_rtd2++;
		vp_cal_pt_rd_str.WaitSamples = 0;
		temp_info[RTD_1].SampleCount=0;
		temp_info[RTD_2].SampleCount=0;
		deint_complete=1;
	}
	else if(reinit_spi3_flag)
	{
		HAL_SPI_Init(&hspi1);
		deint_complete=0;
		reinitialize_SPI=0;
		SPITxRxCptl[0]=1;
		SPITxRxCptl[1]=1;
		RTD_init();
	}

}

void RTD_proc(void)
{
	  uint16_t temp;
	 switch (rtd_info[RTDIndex].state)
	 {

	 case RTD_INIT:
    	  if( SPITxRxCptl[RTDIndex] == 1)
    	   {
    		 SPITxRxCptl[RTDIndex] = 0;
    		 //Start Init by reading the Config register
    		 HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_RESET);
    		 HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)RTD_TxData,(uint8_t *)rtd_info[RTDIndex].temp_data, 2);
     		 Set_delay(SW_TMR_RTD_DLY, 1000, &rtd_info[0].poll_flag);
    		 rtd_info[RTDIndex].state = RTD_INIT_CONT;
    	   }
        break;

      case RTD_INIT_CONT:
    	 if(SPITxRxCptl[RTDIndex] )
    	 {
    		 SPITxRxCptl[RTDIndex] = 0;
    	     rtd_info[RTDIndex].state = RTD_INIT_DONE;
    	     rtd_info[RTDIndex].InitSeq++;
    	     //Step 1: Set configuration register
    	     if(rtd_info[RTDIndex].InitSeq == 1)
    	     {
    	    	   HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_RESET);
    	           HAL_SPI_Transmit_IT(&hspi1,&rtd_config[0],2);
    	       }
    	       //Step 2: Set High/Low Limit registers
    	       else if(rtd_info[RTDIndex].InitSeq == 2)
    	         {
    	            HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_RESET);
    	            HAL_SPI_Transmit_IT(&hspi1,&rtd_config[2],5);
    	          }
    	  	   Set_delay(SW_TMR_RTD_DLY, 10, &rtd_info[0].poll_flag);
    	     }
          break;

      case RTD_INIT_DONE:
		if (SPITxRxCptl[RTDIndex]) {
			if (rtd_info[RTDIndex].InitSeq == 2) {
				rtd_info[RTDIndex].state = RTD_EOC;
			} else  // continue next init cmd
			{
				rtd_info[RTDIndex].state = RTD_INIT;

			}
			RTDIndex++;
			// Try to initialize if T2 enabled
			if (RTDIndex > (vp_param_rw_str.t2enable & 0x01))
				RTDIndex = 0;
		}
        break;

      //////////////////////////////////////////////
      // Repetitive Conversion and data read process
      //////////////////////////////////////////////
      case RTD_EOC:
    	  if( SPITxRxCptl[RTDIndex] == 1)  //Read data at EOC
    	  {
			  SPITxRxCptl[RTDIndex] = 0;
			  rtd_info[RTDIndex].state = RTD_EOC_CHK;
			  HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_RESET);
			  HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)RTD_TxData, (uint8_t *)rtd_info[RTDIndex].temp_data, 4);
    	  }
        break;

	//////////////////////////////////////////////
	// Process data read and check for fault bit status
	//////////////////////////////////////////////
      case RTD_EOC_CHK:
    	  if( SPITxRxCptl[RTDIndex] == 1 )  //Process Data read
    	  {
              HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_SET);

    		  rtd_info[RTDIndex].state = RTD_EOC;
              //Process valid data
              rtd_info[RTDIndex].fault_ctr = 0;
              temp = (rtd_info[RTDIndex].temp_data[2] << 8);
              temp |= rtd_info[RTDIndex].temp_data[3];
              temp = temp >> 1;
              temp_info[RTDIndex].RTDVal = temp;
              temp_info[RTDIndex].new_data = 1;

             if(temp_info[RTDIndex].RTDVal==0)
             {
            	 if(!(rtd_info[RTDIndex].temp_data[1])&&rtd_info[RTDIndex].temp_data[1]!=0xC0)
            	 {
            		 if(++chck_ctr==5)
            		 {
            		   reinitialize_SPI=1;
            		 }
            	 }
            	 else
            	 {
            		 chck_ctr=0;
            	 }
             }

              if(rtd_info[RTDIndex].temp_data[3] & 0x01) //If Error bit set read fault register
              {
            	  rtd_info[RTDIndex].state = RTD_ERR_READ;
            	  rtd_info[RTDIndex].InitSeq = 0;
              }
              else
              {
    			  RTDIndex++;
    			  if(RTDIndex > (vp_param_rw_str.t2enable & 0x01))
    				  RTDIndex = 0;
              }

   	          Set_delay(SW_TMR_RTD_DLY, 70, &rtd_info[0].poll_flag);//250ms delay
    	  }
        break;

        //////////////////////////////////////////////
        // Read fault register and clear
        //////////////////////////////////////////////
      case RTD_ERR_READ:
    	  if(SPITxRxCptl[RTDIndex] == 1)
    	  {
    		  SPITxRxCptl[RTDIndex] = 0;
    		  rtd_info[RTDIndex].state = RTD_ERR_CHK;
    		  rtd_info[RTDIndex].InitSeq++;
    		  if(rtd_info[RTDIndex].InitSeq == 1)
    		  {
        		  HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_RESET);
        		  HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t *)RTD_ErrReg, (uint8_t *)RTD_ErrData, 2);
    		  }
    		  if(rtd_info[RTDIndex].InitSeq == 2)
    		  {
        		  HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_RESET);
        		  HAL_SPI_TransmitReceive_IT(&hspi1,(uint8_t*)rtd_flt_clr, RTD_ErrData, 2);
    		  }
	          Set_delay(SW_TMR_RTD_DLY, 10, &rtd_info[0].poll_flag);
    	  }
    	  break;

          //////////////////////////////////////////////
          // Process Fault register data and set status
          //////////////////////////////////////////////
      case RTD_ERR_CHK:
    	  if(SPITxRxCptl[RTDIndex] == 1)
    	  {
              HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_SET);
     		  rtd_info[RTDIndex].state = RTD_ERR_READ;
     		  if(rtd_info[RTDIndex].InitSeq == 1)
     		  {
     			 rtd_info[RTDIndex].fault_status = RTD_ErrData[1];
     			 if((rtd_info[RTDIndex].fault_status & 0x40) || (rtd_info[RTDIndex].fault_status & 0x04 ))
     			 {
     				 //Treat as Open
     				 if(RTDIndex == RTD_1)
     				 {
        	    		  temp_info[RTD_1].rtd_status = RTD_STAT_SHORT;
     			          //vp_rt_wr_str.rtd1_status = RTD_STAT_SHORT;
     				 }
     				 else if(RTDIndex == RTD_2)
     				 {
        	    		  temp_info[RTD_2].rtd_status = RTD_STAT_SHORT;
     			         // vp_rt_wr_str.rtd2_status = RTD_STAT_SHORT;
     				 }
     			 }
     			 else if(rtd_info[RTDIndex].fault_status & 0x80)
     			 {
     				 //Treat as Open
     				 if(RTDIndex == RTD_1)
     				 {
        	    		  temp_info[RTD_1].rtd_status = RTD_STAT_OPEN;
     			         // vp_rt_wr_str.rtd1_status = RTD_STAT_OPEN;
     				 }
     				 else if(RTDIndex == RTD_2)
     				 {
        	    		  temp_info[RTD_2].rtd_status = RTD_STAT_OPEN;
     			         // vp_rt_wr_str.rtd2_status = RTD_STAT_OPEN;
     				 }
     			 }
     		  }
     		  else if(rtd_info[RTDIndex].InitSeq == 2)
    		  {
    			  rtd_info[RTDIndex].state = RTD_EOC;
        		  RTDIndex++;
        		  if(RTDIndex > (vp_param_rw_str.t2enable & 0x01))
        			  RTDIndex = 0;
    		  }
    	  }
    	  break;

          //////////////////////////////////////////////
          // Handle error condition
          //////////////////////////////////////////////
      case RTD_ERR:
    	  if(SPIErrorDetected)
    	  {
    		  if(HAL_OK ==HAL_SPI_Abort(&hspi1))
    		  {
    			  SPIErrorDetected = 0;
    		  }
    	  }
    	  else
    	  {
			  SPITxRxCptl[RTDIndex] = 1;
			  rtd_info[0].state = RTD_INIT;
			  rtd_info[1].state = RTD_INIT;
			  rtd_info[0].InitSeq = 0;
			  rtd_info[1].InitSeq = 0;
			  RTDIndex = 0;
    	  }
    	  break;

      default:
    	   // Error in getting temperature data from the IC MAX31865
    	        rtd_info[RTDIndex].state=RTD_INIT;
        break;
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &hspi1)
	{
		SPITxRxCptl[RTDIndex] = 1; //Set event flag
		//De-Assert chip select
		HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_SET);
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &hspi1)
	{
		SPITxRxCptl[RTDIndex] = 1; //Set event flag
		//De-Assert chip select
		HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_SET);
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &hspi1)
	{
		SPITxRxCptl[RTDIndex] = 1;  //Set event flag
		//De-Assert chip select
		HAL_GPIO_WritePin(rtd_info[RTDIndex].CsPort,rtd_info[RTDIndex].CsPin,GPIO_PIN_SET);
	}
}




