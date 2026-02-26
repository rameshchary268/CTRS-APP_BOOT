/*
 * MAX11206.c
 *
 *  Created on: Apr 9, 2025
 *      Author: rameshchary.vadla
 */

#include <MAX11206.h>
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
extern SPI_HandleTypeDef hspi2; // configure this SPI beforehand

#define MAX11206_CS_GPIO_Port GPIOB
#define MAX11206_CS_Pin GPIO_PIN_10

#define LINEF_BIT    (1U << 7)
#define UB_BIT       (1U << 6)
#define EXTCLK_BIT   (1U << 5)
#define REFBUF_BIT   (1U << 4)
#define SIGBUF_BIT   (1U << 3)
#define FORMAT_BIT   (1U << 2)
#define SCYCLE_BIT   (1U << 1)


#define MOVING_AVG_SIZE 20
uint32_t SOC_R,SGC_R,SCOC_R,SCGC_R;


uint32_t RAW_value;
float voltage =0;

uint32_t result;


uint32_t calib_result;
char voltage_arr[50];
char status_reg_arr[20];


float adc_voltage_buffer[MOVING_AVG_SIZE] = {0};
uint8_t buffer_index = 0;
uint8_t samples_collected = 0;
float averaged_voltage = 0.0f;
uint8_t reg_data = 0;
float voltage_filtered=0;

//static uint8_t gain_flag =1;
bool rdy;

double mTorr = 0;
char mTorr_arr[50];


float max_value =0;
float min_value =0;

uint8_t ctrl3_reg_data = 0x1E;

int32_t SCOC=0;
int32_t SCGC = 0;
uint32_t SGC = 0;
uint32_t SOC = 0;

bool vaccum_flag = 0;

uint8_t Vaccum_status =0;

// 1c =



// registers
/*
 STAT1  -  read
 CRTL1  - R/W
 CRTL2  - R/W
 CRTL3  - R/W
 DATA   - read
 SOC	- R/W
 SGC	- R/W
 SCOC	- R/W
 SCGC	- R/W


 **/



typedef struct {
    float voltage;  // Sensor Voltage in mV
    float pressure; // Pressure in mTorr
} SensorMap;

const SensorMap lookup_table[] = {
    {939, 4.3}, {921, 5.6}, {904.1, 6.9}, {871, 9.3},
    {861.2, 10.1}, {849.3, 11.2}, {838.3, 12.2}, {822.5, 13.6},
    {808.5, 14.9}, {788.1, 16.7}, {780.9, 17.3}, {694.25, 26.7},
    {682.8, 28}, {676.08, 29}, {667.3, 30}, {660, 31},
    {650.6, 32}, {642.3, 33.1}, {634.7, 34.1}, {624.21, 35.4},
    {610, 37}, {605.2, 38}, {586.2, 40.5}, {577.7, 41.6},
    {569.9, 42.6}, {557.5, 44.4}, {551.2, 45.3}, {542.8, 46.5},
    {539.7, 47}, {516, 50}, {324.7, 100}, {236.7, 149},
    {177, 200}, {158, 215}, {149.3, 250}, {127.5, 300},
    {106.2, 364}, {87.2, 438}, {74.9, 500}, {69.8, 550},
    {65.1, 600}, {61, 650}, {57.2, 700}, {51.27, 788},
    {49.7, 799}, {47.09, 865}, {44.8, 907}, {39, 1000},
    {36.8, 1200}
};
const int table_size = sizeof(lookup_table) / sizeof(lookup_table[0]);
float interpolate(float x, float x0, float y0, float x1, float y1) {
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}


// Lookup + interpolate based on input voltage
float get_mTorr_lookup(float adc_voltage) {
    for (int i = 1; i < table_size; ++i) {
        if (adc_voltage >= lookup_table[i].voltage && adc_voltage <= lookup_table[i - 1].voltage) {
            return interpolate(adc_voltage,
                               lookup_table[i].voltage, lookup_table[i].pressure,
                               lookup_table[i - 1].voltage, lookup_table[i - 1].pressure);
        }
    }
    // Outside range handling
    if (adc_voltage > lookup_table[0].voltage)
        return lookup_table[0].pressure;
    if (adc_voltage < lookup_table[table_size - 1].voltage)
        return lookup_table[table_size - 1].pressure;

    return -1; // Error
}


uint8_t build_register(
    bool linef,     // 0 or 1
    bool ub,        // 0 or 1
    bool extclk,    // 0 or 1
    bool refbuf,    // 0 or 1
    bool sigbuf,    // 0 or 1
    bool format,    // 0 or 1
    bool scycle     // 0 or 1
	)
{
    uint8_t reg = 0;

    if (linef)   reg |= LINEF_BIT;
    if (ub)      reg |= UB_BIT;
    if (extclk)  reg |= EXTCLK_BIT;
    if (refbuf)  reg |= REFBUF_BIT;
    if (sigbuf)  reg |= SIGBUF_BIT;
    if (format)  reg |= FORMAT_BIT;
    if (scycle)  reg |= SCYCLE_BIT;

    return reg;
}


void MAX11206_CS_Low() {
    HAL_GPIO_WritePin(MAX11206_CS_GPIO_Port, MAX11206_CS_Pin, GPIO_PIN_RESET);
}

void MAX11206_CS_High() {
    HAL_GPIO_WritePin(MAX11206_CS_GPIO_Port, MAX11206_CS_Pin, GPIO_PIN_SET);

}

void MAX11206_ConfigCTRL1(SPI_HandleTypeDef *hspi) {
    uint8_t ctrl1_val = 0;
    ctrl1_val = build_register(
            0,  // LINEF
            1,  // U/B
            0,  // EXTCLK
            0,  // REFBUF
            0,  // SIGBUF
            0,  // FORMAT ----> 0 binary , 1- unipolar
            0  // SCYCLE (default is 1 per table) 0 - contiouous,1- single shot
        );

//ctrl1_val = 0b01000100;
    // CTRL1 address is 0x01
    MAX11206_WriteRegister(&hspi2, CTRL1_reg, &ctrl1_val, 1);

    MAX11206_ReadRegister(&hspi2, CTRL1_reg, &reg_data, 1);
}



//reg - 0x80, data - 0x81



void Gain_overrange_error_check() //possibly due to an excessively large input signal or incorrect gain settings.
{
	uint8_t ctrl1_reg=0;
	MAX11206_Command_byte(&hspi2, SELF_CALIB);

	MAX11206_ReadRegister(&hspi2, STAT1_reg, &ctrl1_reg, 1);

	if((ctrl1_reg >>7)&&1)
	{
		//printf("incorrect gain settings\n\r");

	}




}
void MAX11206_WriteRegister24(SPI_HandleTypeDef *hspi, uint8_t reg_addr, uint32_t value)
{
    uint8_t cmd = 0xC0 | (reg_addr << 1);  // START=1, MODE=1, RS[3:0], W=0 (write)
    uint8_t data[3];

    data[0] = (value >> 16) & 0xFF;  // MSB
    data[1] = (value >> 8) & 0xFF;   // Mid byte
    data[2] = value & 0xFF;          // LSB

    MAX11206_CS_Low();
    HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(hspi, data, 3, HAL_MAX_DELAY);
    MAX11206_CS_High();
}

void MAX11206_WriteRegister(SPI_HandleTypeDef *hspi, uint8_t reg_addr, uint8_t *data, uint8_t size) {
    uint8_t cmd = 0xC0 | (reg_addr << 1); // START=1, MODE=1, RS[3:0], W=0 (write)
    MAX11206_CS_Low();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
    MAX11206_CS_High();

}

void MAX11206_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t reg_addr, uint8_t *data, uint8_t size) {
    uint8_t cmd = 0xC1 | (reg_addr << 1); // START=1, MODE=1, RS[3:0], R=1 (read)
    MAX11206_CS_Low();
    HAL_SPI_Transmit(&hspi2, (uint8_t *)&cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi2, data, size, HAL_MAX_DELAY);
    MAX11206_CS_High();



}
int32_t READ_SOC(SPI_HandleTypeDef *hspi)
{
	  uint8_t data[3] = {0};
	    MAX11206_ReadRegister(&hspi2,SOC_reg , data, 3); // DATA register is at address 0x04


	     calib_result= (data[0] << 16) | (data[1] << 8) | data[2];

	     return calib_result;

}
int32_t READ_SGC(SPI_HandleTypeDef *hspi)
{
	  uint8_t data[3] = {0};
	    MAX11206_ReadRegister(&hspi2,SGC_reg , data, 3); // DATA register is at address 0x04


	     calib_result= (data[0] << 16) | (data[1] << 8) | data[2];

	     return calib_result;

}
int32_t READ_SCOC(SPI_HandleTypeDef *hspi)
{
	  uint8_t data[3] = {0};
	    MAX11206_ReadRegister(&hspi2,SCOC_reg , data, 3); // DATA register is at address 0x04


	     calib_result= (data[0] << 16) | (data[1] << 8) | data[2];

	     return calib_result;

}
int32_t READ_SCGC(SPI_HandleTypeDef *hspi)
{
	  uint8_t data[3] = {0};
	    MAX11206_ReadRegister(&hspi2,SCGC_reg , data, 3); // DATA register is at address 0x04


	     calib_result= (data[0] << 16) | (data[1] << 8) | data[2];

	     return calib_result;

}
uint32_t MAX11206_ReadRegister24(SPI_HandleTypeDef *hspi, uint8_t reg_addr)
{
    uint8_t cmd = 0xC1 | (reg_addr << 1);  // START=1, MODE=1, RS[3:0], R=1 (read)
    uint8_t data[3];
    uint32_t result = 0;

    MAX11206_CS_Low();
    HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(hspi, data, 3, HAL_MAX_DELAY);
    MAX11206_CS_High();

    result = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
    return result;
}


int32_t MAX11206_ReadData(SPI_HandleTypeDef *hspi) {
    uint8_t data[3] = {0};
    MAX11206_ReadRegister(&hspi2,DATA_reg , data, 3); // DATA register is at address 0x04


     result = (data[0] << 16) | (data[1] << 8) | data[2];
     result = result>>4;

//     result &= 0xFFFFF;  // keep only 20 bits
    // Convert from 24-bit two's complement if needed:
//    if (result & 0x800000) result |= 0xFF000000; // Sign extend   as we are using unipolar  and format is 1
    return result;
}


void MAX11206_Command_byte(SPI_HandleTypeDef *hspi,uint8_t command) {
    uint8_t cmd = command; // START=1, MODE=0, CAL1=0, CAL0=0, IMPD=0, RATE=011 (10sps)
    MAX11206_CS_Low();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    MAX11206_CS_High();
}
void MAX11206_StartConversion(SPI_HandleTypeDef *hspi) {
    uint8_t cmd = CONVERT_10_SPS; // START=1, MODE=0, CAL1=0, CAL0=0, IMPD=0, RATE=011 (10sps)
    MAX11206_CS_Low();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    MAX11206_CS_High();


}

void self_calib(void)
{

//	2. try
//	read_CTRL3();
//	//	 >>>> self calibration process: test
//		ctrl3_reg_data = 0x18;
//		//1.enable self calib bits in ctrl3
//		MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
//		    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib
//
//		    //2. send the self calib command
//		    			MAX11206_Command_byte(&hspi2,SELF_CALIB);
//		    			  HAL_Delay(200);
//			SCOC =  READ_SCOC(&hspi2); // read scoc register
//			SCGC = READ_SCGC(&hspi2); // read scgc
//
//		    MAX11206_WriteRegister24(&hspi2, SCOC_reg, 0x00007E);
//		    MAX11206_WriteRegister24(&hspi2, SCGC_reg, 0xBFD345);
//
//			SCOC =  READ_SCOC(&hspi2); // read scoc register
//			SCGC = READ_SCGC(&hspi2); // read scgc
//
//
//
//
//		//3. wait for 200 ms for completing the calib

//
//
//	//	    Allow conversion time based on rate
//
//		    ctrl3_reg_data = 0x1E;
//		  	    	//1.enable self calib bits in ctrl3
//		  	    	MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
//		  	    	    read_CTRL3();

	//result no change


//	1st try
	/*
	////////*/////////////------------//////////////
	read_CTRL3();
//	 >>>> self calibration process: test
	ctrl3_reg_data = 0x18;
	//1.enable self calib bits in ctrl3
	MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
	    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib


		SCOC =  READ_SCOC(&hspi2); // read scoc register
		SCGC = READ_SCGC(&hspi2); // read scgc

	    MAX11206_WriteRegister24(&hspi2, SCOC_reg, 0x00007E);
	    MAX11206_WriteRegister24(&hspi2, SCGC_reg, 0xBFD345);
	   SCOC =  MAX11206_ReadRegister24(&hspi2,SCOC_reg);
	    SCGC = MAX11206_ReadRegister24(&hspi2,SCGC_reg);



		SCOC =  READ_SCOC(&hspi2); // read scoc register
		SCGC = READ_SCGC(&hspi2); // read scgc


	//2. send the self calib command
		MAX11206_Command_byte(&hspi2,SELF_CALIB);

	//3. wait for 200 ms for completing the calib

	    HAL_Delay(200);

//	    Allow conversion time based on rate

	    ctrl3_reg_data = 0x1E;
	  	    	//1.enable self calib bits in ctrl3
	  	    	MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
	  	    	    read_CTRL3();


	  	    	   /////////////////////-----------------//////////////////*/
}

void read_calib_reg()
{
	SCOC =  READ_SCOC(&hspi2); // read scoc register
	SCGC = READ_SCGC(&hspi2); // read scgc

	SOC =  READ_SOC(&hspi2); // read scoc register  //Calibration result is stored in the SOC (System Offset Coefficient) register.


	SGC = READ_SGC(&hspi2); // read scgc

	SOC = MAX11206_ReadRegister24(&hspi2,SOC_reg);
	SGC = MAX11206_ReadRegister24(&hspi2,SGC_reg);


}

void system_Calib()
{
//	//	1.initial powerup
//		ctrl3_reg_data = 0x1E;
//		MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
//		    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib
//
//	//	   2. enable the self calib registers
//		    ctrl3_reg_data = 0x06;
//			MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
//			    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib
//
//	//	    3.self calib command
//				MAX11206_Command_byte(&hspi2,SELF_CALIB);
//
//				HAL_Delay(200);



//	/// system calib only need to do for system calib
//read_calib_reg();
//
//	//5. enable system off set regster
//		ctrl3_reg_data = 0x10;
//		MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
//		    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib
//
//		    //6. send the command system offset calib
//		    MAX11206_Command_byte(&hspi2,SYS_OFFSET_CALIB);
//	HAL_Delay(100);
//
//		    //7.enable system gain register
//		    ctrl3_reg_data = 0x00;
//		    MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
//		    	    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib
//
//		    //apply full scale at input
//		    MAX11206_Command_byte(&hspi2,SYS_GAIN_CALIB);
//		    HAL_Delay(100);
//
//	read_calib_reg();



		    			/////////////---------user system calib code starts here----------//////


char value = 0;


//print_data("do you need system calib : give 1 \n\r");
//print_data("do you need self calib : give 2 else give any character \n\r");
//value = read_data();
//value = 'n';
if(value == '1')
{
	//	1.initial powerup
		ctrl3_reg_data = 0x1E;
		MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
		    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib

	//	   2. enable the self calib registers
		    ctrl3_reg_data = 0x06;
			MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
			    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib

	//	    3.self calib command
				MAX11206_Command_byte(&hspi2,SELF_CALIB);

				HAL_Delay(200);

	//4. read SCOC AND SCGC regisetrs
				SCOC =  READ_SCOC(&hspi2); // read scoc register
				SCGC = READ_SCGC(&hspi2); // read scgc


//

//	5. enable system off set regster
		ctrl3_reg_data = 0x00;
		MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
		    read_CTRL3();  // this is normal giving 0x1e correct data. 0x10

		    //print_data("give the zero scale\n\r");

		    while(value != 0)
		    {
		    	//value = read_data();
		    	if(value == 'y')
		    	{
		    		break;
		    	}
		    }
		    //6. send the command system offset calib
		    MAX11206_Command_byte(&hspi2,SYS_OFFSET_CALIB);
	HAL_Delay(100);

	 if(MAX11206_WaitForDataReady(&hspi2))// Wait for RDY = 1
	    {
		 read_calib_reg(); //Calibration result is stored in the SOC (System Offset Coefficient) register.


		 //print_data("system offset calibrated\n\r");
	    }
	 else
	 {
		 //print_data("system offset not calibrated\n\r");
		 Vaccum_status = read_status();
			sprintf(status_reg_arr,"status = %x\n",Vaccum_status);
//			    	    	//print_data(status_reg_arr);
	 }

		    //7.enable system gain register
		    ctrl3_reg_data = 0x00;
		    MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1); // register data ctrl3_reg data for self calib 0x06
		    	    read_CTRL3();  // this is normal giving 0x1e correct data. 0x06 for self calib

		    	    //print_data("give the max scale\n\r");

		    	    while(value != 0)
		    	    {
		    	    	//value = read_data();
		    	    	if(value == 'y')
		    	    	{
		    	    		break;
		    	    	}
		    	    }
		    //apply full scale at input
		    MAX11206_Command_byte(&hspi2,SYS_GAIN_CALIB);
		    HAL_Delay(100);

			 if(MAX11206_WaitForDataReady(&hspi2))// Wait for RDY = 1
			    {
				 read_calib_reg(); //Calibration result is stored in the SGC (System Gain Coefficient) register.



				 //print_data("system gain calibrated\n\r");
			    }
			 else
				 {
					 //print_data("system gain not calibrated\n\r");
				 }




}
if(value == '1')
{
	self_calib();
}

read_calib_reg(); //Calibration result is stored in the SGC (System Gain Coefficient) register.



}

void power_down(void)
{


	MAX11206_Command_byte(&hspi2,INTERMEDIATE_POWDOWN);
HAL_Delay(200);
}

void sys_off_calib(void)
{
	MAX11206_Command_byte(&hspi2,SYS_OFFSET_CALIB);

}

void read_CTRL3(void)
{
//	 MAX11206_WriteRegister(&hspi2, CTRL3_reg, 0x00, 1); //config gain 1 in ctrl3
	    MAX11206_ReadRegister(&hspi2, CTRL3_reg, &reg_data, 1);

}

//uint8_t MAX11206_WaitForDataReady(SPI_HandleTypeDef *hspi) {
//    uint8_t status = 0;
//    do {
//    	MAX11206_ReadRegister(&hspi2,STAT1_reg , &status, 1); // Read STAT1
//        rdy = (status & 0x01);
//
//    } while (rdy== 0); // RDY bit
//    return status;
//}
uint8_t read_status()
{

	 MAX11206_ReadRegister(&hspi2, STAT1_reg, &Vaccum_status, 1); // Read STAT1
	 return Vaccum_status;
}

uint8_t MAX11206_WaitForDataReady(SPI_HandleTypeDef *hspi) {
    uint8_t status = 0;
    uint32_t timeout = 10;  // Adjust based on expected delay

    while (timeout--) {
    	MAX11206_ReadRegister(hspi, STAT1_reg, &status, 1); // Read STAT1
        if (status & 0x01) {  // RDY bit is 1
            return 1; // Success
        }
        HAL_Delay(100);
    }
    return 0; // Timeout error
}

void set_gain(uint8_t gain)
{
    // Enable all calibration coefficients (NOSYSO, NOSYSG, NOSCO, NOSCG = 0)
    uint8_t ctrl3_reg_data = 0x00;
    ctrl3_reg_data |= gain;
    MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1);


}


void write_calib_data(uint8_t gain)
{
    // Example calibration values (adjust based on your calibration method or prior capture)


//		uint32_t SOC   = 0x140d3;
//	    uint32_t SGC   = 0x81241e;
//	    uint32_t SCOC  = 0xFFFdca;
//	    uint32_t SCGC  = 0xbef01b;

	//values after system calib by rightsifting the data reg by 4
			uint32_t SOC   = 0xce5;
		    uint32_t SGC   = 0x80a754;
		    uint32_t SCOC  = 0xFFFdbe;
		    uint32_t SCGC  = 0xbef01b;

    // Enable all calibration coefficients (NOSYSO, NOSYSG, NOSCO, NOSCG = 0)
    uint8_t ctrl3_reg_data = 0x00;
    ctrl3_reg_data |= gain;
    MAX11206_WriteRegister(&hspi2, CTRL3_reg, &ctrl3_reg_data, 1);




    // Optional: Read back CTRL3 to verify write
    read_CTRL3();

    // Write all calibration values
    MAX11206_WriteRegister24(&hspi2, SOC_reg, SOC);
    MAX11206_WriteRegister24(&hspi2, SGC_reg, SGC);
    MAX11206_WriteRegister24(&hspi2, SCOC_reg, SCOC);
    MAX11206_WriteRegister24(&hspi2, SCGC_reg, SCGC);

    // Read back to confirm
    SOC_R   = MAX11206_ReadRegister24(&hspi2, SOC_reg);
    SGC_R   = MAX11206_ReadRegister24(&hspi2, SGC_reg);
    SCOC_R  = MAX11206_ReadRegister24(&hspi2, SCOC_reg);
    SCGC_R  = MAX11206_ReadRegister24(&hspi2, SCGC_reg);
}


void MAX11206_Init(SPI_HandleTypeDef *hspi,uint8_t gain) {
    uint8_t status =0;

	MAX11206_ConfigCTRL1(hspi);         // Step 1: Configure CTRL1


	status = read_status();
	sprintf(status_reg_arr,"Initial status = %x\n",status);

	read_calib_reg();
 	write_calib_data(gain);
    read_calib_reg();
    system_Calib(); ///////////////////

    MAX11206_StartConversion(&hspi2);     // Step 2: Start conversion (e.g., 10sps)

}
//
float FilteredVoltage_IIR(float input) {
    static float previous_output = 0.0f;
    float alpha = 0.2f;  // Tuning: 0.1 = smoother, 0.9 = more responsive
    float output = alpha * input + (1.0f - alpha) * previous_output;
    previous_output = output;
    return output;
}

void max_min(float voltage)
{
	int flag =1;

	if(flag == 1)
	      {
	          flag =0;

	          min_value = max_value = voltage;
	      }

	if(voltage<=min_value)
	{

		min_value = voltage;

	}
	else if(voltage >= max_value)
	{

		max_value =voltage;


	}

}

// inter polation method 500-700Bytes


// Lookup table (digital voltage in mV vs mTORR)
typedef struct {
    double mV;
    double mTORR;
} LookupEntry;

static const LookupEntry lookupTable[] = {
		{88.2,1040}, {95.5, 942}, {95.1, 947}, {118, 751}, {120, 735}, {124, 706},
    {125.2, 701}, {127.6, 687}, {128, 679}, {131, 666}, {135, 642},
    {139, 618}, {148.7, 570}, {150.9, 558}, {161, 509}, {168, 486},
    {179, 462}, {185, 448}, {210, 399}, {225, 373}, {265, 316},
    {287, 289}, {324, 251}, {337, 239}, {382, 201}, {394, 195},
    {514, 150}, {578, 130}, {642, 114}, {711, 99.4}, {816, 84.2},
    {910, 72.4}, {993, 63.4}, {1086, 54}, {1263, 41.2}, {1514, 27.2},
    {1684, 18.7}, {1811, 13.5}, {1979, 7}, {2047, 4.7}, {2144, 4}
};

static const int lookupSize = sizeof(lookupTable) / sizeof(lookupTable[0]);

// Function for interpolation
double interpolate_mTORR(double mV)
{
    if (mV <= lookupTable[0].mV)
        return lookupTable[0].mTORR;
    if (mV >= lookupTable[lookupSize-1].mV)
        return lookupTable[lookupSize-1].mTORR;

    for (int i = 0; i < lookupSize - 1; i++) {
        if (mV >= lookupTable[i].mV && mV <= lookupTable[i+1].mV) {
            double x0 = lookupTable[i].mV;
            double y0 = lookupTable[i].mTORR;
            double x1 = lookupTable[i+1].mV;
            double y1 = lookupTable[i+1].mTORR;
            return y0 + ((mV - x0) * (y1 - y0) / (x1 - x0));
        }
    }
    return -1; // Should never reach here
}


// polinomial method 3-5KB with pow() functions without pow() 200-400 bytes


double calculate_mtorr(double adc) {


//	if(	(adc <=150)) //  650 above
//	{
//	 mTorr =   -4.44215725e-21 * pow(adc, 9)
//	            + 2.09397105e-17  * pow(adc, 8)
//	            - 4.20820567e-14  * pow(adc, 7)
//	            + 4.70541075e-11  * pow(adc, 6)
//	            - 3.20539494e-8   * pow(adc, 5)
//	            + 0.0000137043983 * pow(adc, 4)
//	            - 0.00365868907   * pow(adc, 3)
//	            + 0.589362359     * pow(adc, 2)
//	            - 53.4834893      * adc
//	            + 2388.07568;
//	}
//	else if(adc<=75 && adc >= 50) // 500mTorr to 800 mTorr
//	{
//		 mTorr= -8.57832132e-05 * pow(voltage, 4)
//		      + 2.09943587e-02 * pow(voltage, 3)
//		      - 1.77497568    * pow(voltage, 2)
//		      + 47.3972926    * voltage
//		      + 785.809745;
//
//	}
//
//
//
//else if((adc <=236 )&& (adc >= 75))  //150  -500 mTorr
//{
//	  mTorr  = -2.69960806e-08 * pow(adc, 5)
//	      + 2.15846522e-05 * pow(adc, 4)
//	      - 6.74326141e-03 * pow(adc, 3)
//	      + 1.03922806    * pow(adc, 2)
//	      - 81.8599484    * adc
//	      + 3027.94589;
//}
//else if((adc >=236) && (adc <=516 ))  /// 50-150mT
//{
//    mTorr =		0.000000001308   * pow(adc, 4)
//    		- 0.000005116        * pow(adc, 3)
//    		+0.005491	       * pow(adc,2)
//    		- 2.532*adc
//			+ 504.6079;
//
//
//
//
//}
//else if((adc >=516)&&(adc<=950) ) //4-50mTorr
//{
//	mTorr = 0.0001*pow(adc,2)
//			- 0.2516*adc + 154.26;
//
//}


	if(adc >= 532) // 150 and below mtorr
	{
		 mTorr =
		          -6.017e-08  * pow(adc, 3)
		        + 2.93152989e-04 * pow(adc, 2)
		        - 0.514550015    * pow(adc, 1)
		        + 344.553451;

	}
	else if((adc >= 360) && (adc<723 )) //
	{
		mTorr = -0.00000177598 * pow(adc, 3)
	        + 0.0034289      * pow(adc, 2)
	        - 2.4468         * adc
	        + 747.04;

	}
	else if((adc >=90) && (adc < 360) )
	{
		mTorr =   0.000000000687953414 * pow(adc, 5)
        - 0.000000461606874   * pow(adc, 4)
        - 0.0000257440183     * pow(adc, 3)
        + 0.0865183968        * pow(adc, 2)
        - 24.2264808          * adc
        + 2526.52612;
	}
	return mTorr;



}
int Read_MAX11206_ADC_Value() {

uint8_t status = 0;

static uint8_t GAIN_bit =1;

    if(MAX11206_WaitForDataReady(&hspi2))// Wait for RDY = 1
    {

    	if(vaccum_flag == 0)
    	{
    		status = read_status();
    		if(status == 0xA3)
    		{
    			//print_data("Error system gain calibration was Over-Range!!\n\r");
    			return 0;
    		}
    	    	sprintf(status_reg_arr,"status = %x\n",status);
//    	    	//print_data(status_reg_arr);
    	    	vaccum_flag = 1;

    	}

    	 RAW_value = MAX11206_ReadData(&hspi2);  // Read 20-bit result

    	 switch(GAIN_bit)
    	 {
    	 case 1:
    		 voltage = (((RAW_value / 1048575.0f) * 2.5f)/1);

//    		 voltage = ((voltage+0.011)+1000);
    		 voltage = (voltage*1000.0)+0.8;
			 break;
    	 case 2:
			 voltage = (((RAW_value / 1048575.0f) * 2.5f)/2);
			 voltage = (voltage*1000);
			 break;
    	case 4:
			 voltage = (((RAW_value / 1048575.0f) * 2.5f)/4);
			 voltage = (voltage*1000);
			 break;

    	 case 8:
			 voltage = (((RAW_value / 1048575.0f) * 2.5f)/8);
			 voltage = (voltage*1000);
			 break;

    	 case 16:
			 voltage = (((RAW_value / 1048575.0f) * 2.5f)/16);
			 voltage = (voltage*1000);
			 break;

    	 case 32:
			 voltage = (((RAW_value / 1048575.0f) * 2.5f)/32);
			 voltage = (voltage*1000);
			 break;

    	 }

    	 //1.        polynomial method  ***************************
//    	  mTorr = calculate_mtorr(voltage);
    	 //2. lookiptable and interpolation method *************

    	  mTorr = interpolate_mTORR(voltage);
    	  /// below are the steps for increasing the gain and getting the values at lower voltages
//    	  //adding gain below 40mV
//    		 if(voltage <39)
//    	    	 {
//    			 if(gain_flag)
//    			 {
//    				 gain_flag = 0;
//    				GAIN_bit = 32;
//    				  MAX11206_Init(&hspi2,GAIN32);
//    			 }
//
//
//    	    	 }
//
//    		 else if(voltage >=39)
//    		 {
//    			 if(gain_flag == 0)
//    			 {
//    				 gain_flag =1;
//
//    				 GAIN_bit = 1;
//    				  MAX11206_Init(&hspi2,GAIN1);
//    			 }
//
//    		 }

//    	 sprintf(voltage_arr,"voltage = %f  ",voltage);
//    	 sprintf(mTorr_arr,"mTorr = %f\n\r",mTorr);
    	 //print_data(voltage_arr);
//    	 MODBUS_transmit(voltage_arr);

    	 //print_data(mTorr_arr);
//    	 MODBUS_transmit(mTorr_arr);
//    	  max_min(voltage);

//    	 voltage_filtered = FilteredVoltage_IIR(voltage);
    }

//    HAL_Delay(100);


    // Process adc_value as needed...
    return 0;
}


//void Read_MAX11206_ADC_Value() {
//    if (MAX11206_WaitForDataReady(&hspi2)) {
//        adc_value = MAX11206_ReadData(&hspi2);  // Read 24-bit result
//
//        // Apply conversion formula
//        voltage = ((adc_value / 1048575.0f) * 3.3f)+0.0058f;
//
//        // Store voltage into buffer
//        adc_voltage_buffer[buffer_index++] = voltage;
//
//        // Wrap around index
//        if (buffer_index >= MOVING_AVG_SIZE) {
//            buffer_index = 0;
//        }
//
//        // Update sample count (max = MOVING_AVG_SIZE)
//        if (samples_collected < MOVING_AVG_SIZE) {
//            samples_collected++;
//        }
//
//        // Calculate moving average
//        float sum = 0.0f;
//        for (uint8_t i = 0; i < samples_collected; i++) {
//            sum += adc_voltage_buffer[i];
//        }
//        averaged_voltage = sum / samples_collected;
//    }
//
//    HAL_Delay(100);
//}
