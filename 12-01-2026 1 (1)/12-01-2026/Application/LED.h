/*
 * LED.h
 *
 *  Created on: Oct 9, 2025
 *      Author: VenkateshArikanti
 */

#ifndef LED_H_
#define LED_H_
#include "main.h"

#define PCA9955B_ADDR           0x01 << 1//0xC0
#define PCA9955B_ADDR2          0x01 << 2//0xC0

#define PCA9955B_PWM0_REG  0x02

// PCA9955B Register Addresses
#define PCA9955B_MODE1          0x00
#define PCA9955B_MODE2          0x01
#define PCA9955B_LEDOUT0        0x02
#define PCA9955B_LEDOUT1        0x03
#define PCA9955B_LEDOUT2        0x04
#define PCA9955B_LEDOUT3        0x05
#define PCA9955B_GRPPWM         0x06
#define PCA9955B_GRPFREQ        0x07
#define PCA9955B_PWM0           0x08
#define PCA9955B_PWM1           0x09
#define PCA9955B_PWM2           0x0A
#define PCA9955B_PWM3           0x0B
#define PCA9955B_PWM4           0x0C
#define PCA9955B_PWM5           0x0D
#define PCA9955B_PWM6           0x0E
#define PCA9955B_PWM7           0x0F
#define PCA9955B_PWM8           0x10
#define PCA9955B_PWM9           0x11
#define PCA9955B_PWM10          0x12
#define PCA9955B_PWM11          0x13
#define PCA9955B_PWM12          0x14
#define PCA9955B_PWM13          0x15
#define PCA9955B_PWM14          0x16
#define PCA9955B_PWM15          0x17

// IREF registers
#define PCA9955B_IREF0          0x18
#define PCA9955B_IREF1          0x19
#define PCA9955B_IREF2          0x1A
#define PCA9955B_IREF3          0x1B
#define PCA9955B_IREF4          0x1C
#define PCA9955B_IREF5          0x1D
#define PCA9955B_IREF6          0x1E
#define PCA9955B_IREF7          0x1F
#define PCA9955B_IREF8          0x20
#define PCA9955B_IREF9          0x21
#define PCA9955B_IREF10         0x22
#define PCA9955B_IREF11         0x23
#define PCA9955B_IREF12         0x24
#define PCA9955B_IREF13         0x25
#define PCA9955B_IREF14         0x26
#define PCA9955B_IREF15         0x27

#define PCA9955B_OFFSET         0x28

// Auto-increment command
#define PCA9955B_AUTO_INC       0x80

// LED Output States
#define LED_OFF                 0x00
#define LED_ON                  0x01
#define LED_PWM                 0x02
#define LED_PWM_GRPPWM          0x03


// Sequence timing control - Adjust these to make sequences faster/slower
#define TIMER_FREQUENCY_MULTIPLIER  1   // 1=122Hz, 2=244Hz, etc.
#define SEQUENCE_STEP_DIVIDER       1   // 1=normal speed, 2=half speed, etc.

// RGB LED Channel Mapping
#define RED_CHANNELS    {2, 5, 8, 11, 14}        // RED channels (every 3rd starting from 2)
#define GREEN_CHANNELS  {1, 4, 7, 10, 13}        // GREEN channels (every 3rd starting from 1)
#define BLUE_CHANNELS   {0, 3, 6, 9, 12}         // BLUE channels (every 3rd starting from 0)

#define RED_CHANNELS_D1    {2,5,8}          // Driver 1 RED channels
#define GREEN_CHANNELS_D1  {1,4,7}          // Driver 1 GREEN channels
#define BLUE_CHANNELS_D1   {0,3,6}          // Driver 1 BLUE channels

#define RED_CHANNELS_D2    {14, 8}              // Driver 2 RED channels
#define GREEN_CHANNELS_D2  {13, 7}              // Driver 2 GREEN channels
#define BLUE_CHANNELS_D2   {12, 6}              // Driver 2 BLUE channels


#define AMBER_RED_CHANNELS   {1, 4, 7, 10, 13}   // Red component for amber
#define AMBER_GREEN_CHANNELS {2, 5, 8, 11, 14}   // Green component for amber

// Amber color mixing ratios (adjust these for desired amber hue)
#define AMBER_RED_RATIO     255    // Full red intensity for amber
#define AMBER_GREEN_RATIO   140    // ~55% green intensity for amber
#define AMBER_RED_IREF      0x2C   // Current reference for red in amber
#define AMBER_GREEN_IREF    0x30   // Current reference for green in amber

// Brightness and frequency settings for high performance
#define HIGH_BRIGHTNESS_IREF    0xFF    // Maximum current reference (120mA)
#define HIGH_BRIGHTNESS_PWM     0xFF    // Maximum PWM value (100% duty cycle)
#define HIGH_FREQ_GRPFREQ       0x00    // Maximum frequency (24kHz)

#define IREF_50MA   0x6A



// Display Configuration
#define BRIGHTNESS_VAR_ADDR_H   0x08    // High byte of 0x080038
#define BRIGHTNESS_VAR_ADDR_L   0x00    // Low byte of 0x080038
#define BRIGHTNESS_VAR_OFFSET   0x38    // Offset within page

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */

//uint8_t amber_channels[] = AMBER_CHANNELS;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

HAL_StatusTypeDef PCA9955B_WriteRegister(uint8_t reg, uint8_t value, uint16_t address);
HAL_StatusTypeDef PCA9955B_WriteRegister2(uint8_t reg, uint8_t value);
HAL_StatusTypeDef PCA9955B_Init(uint8_t Address);
HAL_StatusTypeDef PCA9955B_SetIREF(uint8_t channel, uint8_t current_value, uint16_t address);
//HAL_StatusTypeDef PCA9955B_SetPWM(uint8_t channel, uint8_t pwm_value);
//HAL_StatusTypeDef PCA9955B_SetLEDOutput(uint8_t channel, uint8_t output_state);
HAL_StatusTypeDef PCA9955B_SetPWM(uint8_t channel, uint8_t pwm_value, uint16_t address);
void PCA9955B_SetAllLEDsOff(void);
void PCA9955B_TestSequence(void);
void PCA9955B_SetColorOn(uint8_t color);
void PCA9955B_SetAllChannelsBrightness(uint8_t brightness);
HAL_StatusTypeDef PCA9955B_SetGlobalBrightness(uint8_t brightness);
void PCA9955B_SetChannelBrightness(uint8_t channel, uint8_t brightness);
void PCA9955B_SetHighFrequencyMode(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void PCA9955B_FastRedSequence(uint32_t delay_ns, uint8_t cycles);
HAL_StatusTypeDef PCA9955B_SetPWM_Fast(uint8_t channel, uint8_t pwm_value);
void PCA9955B_UltraFastRedSequence(uint32_t delay_us, uint8_t cycles);
void PCA9955B_UltraFastGreenSequence(uint32_t delay_us, uint8_t cycles);
void PCA9955B_UltraFastBlueSequence(uint32_t delay_us, uint8_t cycles);

void PCA9955B_UpdateBrightness(uint8_t brightness_value);
HAL_StatusTypeDef PCA9955B_ReadPWM0(I2C_HandleTypeDef *hi2c,uint8_t *pwm0);
void ProcessDisplayData(void);
void SendReadRequest(void);


#endif /* LED_H_ */
