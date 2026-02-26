/*
 * LED.c
 *
 *  Created on: Oct 9, 2025
 *      Author: VenkateshArikanti
 */
#include "LED.h"
#include "main.h"
#include "MyMain.h"


uint8_t amber_red_channels[] = AMBER_RED_CHANNELS;
uint8_t amber_green_channels[] = AMBER_GREEN_CHANNELS;
uint8_t global_brightness = 255;
uint8_t current_color = 0; // 0=Red, 1=Green, 2=Blue
uint8_t red_channels[] = RED_CHANNELS;
uint8_t green_channels[] = GREEN_CHANNELS;
uint8_t blue_channels[] = BLUE_CHANNELS;

uint8_t red_channels_d1[] = RED_CHANNELS_D1;
uint8_t green_channels_d1[] = GREEN_CHANNELS_D1;
uint8_t blue_channels_d1[] = BLUE_CHANNELS_D1;

uint8_t red_channels_d2[] = RED_CHANNELS_D2;
uint8_t green_channels_d2[] = GREEN_CHANNELS_D2;
uint8_t blue_channels_d2[] = BLUE_CHANNELS_D2;

uint8_t uart_rx_buffer[10];
uint8_t display_brightness = 255;
uint8_t prev_brightness = 255;
volatile uint8_t uart_data_ready = 0;
extern  I2C_HandleTypeDef hi2c4;

HAL_StatusTypeDef PCA9955B_Init(uint8_t Address)
{
    HAL_StatusTypeDef status;

    // Wait for PCA9955B to be ready
    HAL_Delay(10);

    // Test if device is present
    status = HAL_I2C_IsDeviceReady(&hi2c4, Address, 3, 100);
    if (status != HAL_OK) {
        return status; // Device not responding
    }

    // Initialize using bulk write with high performance settings
    uint8_t init_data[] = {
        PCA9955B_AUTO_INC | PCA9955B_MODE1,     // Auto-increment starting from MODE1
        0x00,                                   // MODE1: Normal operation
        0x01,                                   // MODE2: Outputs change on ACK, totem pole for high frequency
        0xFF, 0xFF, 0xFF, 0xFF,                // LEDOUT0-3: All LEDs PWM controlled
        0xFF,                                   // GRPPWM: Maximum group PWM for brightness
        HIGH_FREQ_GRPFREQ,                      // GRPFREQ: High frequency (24kHz)
        // PWM registers (PWM0-PWM15) - all off initially
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // IREF registers (IREF0-IREF15) - High current reference for brightness
        HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF,
        HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF,
        HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF,
        HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF, HIGH_BRIGHTNESS_IREF,
        0x00                                   // OFFSET: 1µs delay offset
    };

    status = HAL_I2C_Master_Transmit(&hi2c4, Address, init_data, sizeof(init_data), HAL_MAX_DELAY);

    return status;
}

/**
 * @brief Set high frequency mode for optimal RGB LED performance
 */
void PCA9955B_SetHighFrequencyMode(void)
{
    // Set MODE2 for totem pole outputs and ACK update for high frequency
    PCA9955B_WriteRegister(PCA9955B_MODE2, 0x01,PCA9955B_ADDR);

    // Set high frequency group frequency (24kHz)
    PCA9955B_WriteRegister(PCA9955B_GRPFREQ, HIGH_FREQ_GRPFREQ,PCA9955B_ADDR);

    // Set maximum group PWM
    PCA9955B_WriteRegister(PCA9955B_GRPPWM, 0xFF,PCA9955B_ADDR);


    PCA9955B_WriteRegister2(PCA9955B_MODE2, 0x01);

	// Set high frequency group frequency (24kHz)
	PCA9955B_WriteRegister2(PCA9955B_GRPFREQ, HIGH_FREQ_GRPFREQ);

	// Set maximum group PWM
	PCA9955B_WriteRegister2(PCA9955B_GRPPWM, 0xFF);
}

/**
 * @brief Set brightness for all channels
 * @param brightness: 0-255 brightness level
 */
void PCA9955B_SetAllChannelsBrightness(uint8_t brightness)
{
    global_brightness = brightness;

    // Method 1: Use Group PWM for global brightness control
    PCA9955B_WriteRegister(PCA9955B_GRPPWM, brightness,PCA9955B_ADDR);
    PCA9955B_WriteRegister2(PCA9955B_GRPPWM, brightness);
    // Method 2: Alternative - Set individual channel PWM values
    // for(uint8_t i = 0; i < 16; i++) {
    //     PCA9955B_SetPWM(i, brightness);
    // }
}

/**
 * @brief Set global brightness using group PWM
 * @param brightness: 0-255 brightness level
 */
HAL_StatusTypeDef  PCA9955B_SetGlobalBrightness(uint8_t brightness)
{
	uint8_t status=0;
    global_brightness = brightness;
    status = PCA9955B_WriteRegister(PCA9955B_GRPPWM, brightness,PCA9955B_ADDR);
    status = PCA9955B_WriteRegister2(PCA9955B_GRPPWM, brightness);
    return status;
}

/**
 * @brief Set brightness for a specific channel
 * @param channel: Channel number (0-15)
 * @param brightness: 0-255 brightness level
 */
void PCA9955B_SetChannelBrightness(uint8_t channel, uint8_t brightness)
{
    if (channel > 15) return;

    // Set PWM value for individual channel brightness
    PCA9955B_SetPWM(channel, brightness,PCA9955B_ADDR);
}

/**
 * @brief Set IREF (current reference) for specific channel
 */
HAL_StatusTypeDef PCA9955B_SetIREF(uint8_t channel, uint8_t current_value, uint16_t address)
{
    if (channel > 15) return HAL_ERROR;

    	return PCA9955B_WriteRegister(PCA9955B_IREF0 + channel, current_value, address );
}

/**
 * @brief Write to PCA9955B register
 */
HAL_StatusTypeDef PCA9955B_WriteRegister(uint8_t reg, uint8_t value, uint16_t address)
{
    uint8_t data[2] = {reg, value};
    return HAL_I2C_Master_Transmit(&hi2c4, address, data, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef PCA9955B_WriteRegister2(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR2, data, 2, HAL_MAX_DELAY);
}

/**
 * @brief Set PWM value for specific channel
 */
HAL_StatusTypeDef PCA9955B_SetPWM(uint8_t channel, uint8_t pwm_value, uint16_t address)
{
    if (channel > 15) return HAL_ERROR;

    	return PCA9955B_WriteRegister(PCA9955B_PWM0 + channel, pwm_value, address);

}

/**
 * @brief Turn on specific color LEDs with high brightness and frequency
 * @param color: 0=Red, 1=Green, 2=Blue, 3=All
 */
//void PCA9955B_SetColorOn(uint8_t color)
//{
//    // First turn off all LEDs
//    PCA9955B_SetAllLEDsOff();
//
//    switch(color) {
//        case 0: // Red
//            for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
//                PCA9955B_SetIREF(red_channels[i], 0x2C);   // Max current reference
//                PCA9955B_SetPWM(red_channels[i], HIGH_BRIGHTNESS_PWM);     // Max PWM
//            }
//            break;
//
//        case 1: // Green
//            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
//                PCA9955B_SetIREF(green_channels[i], 0x59); // Max current reference
//                PCA9955B_SetPWM(green_channels[i], HIGH_BRIGHTNESS_PWM);   // Max PWM
//            }
//            break;
//
//        case 2: // Blue
//            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
//                PCA9955B_SetIREF(blue_channels[i], 0x50);  // Max current reference
//                PCA9955B_SetPWM(blue_channels[i], HIGH_BRIGHTNESS_PWM);    // Max PWM
//            }
//            break;
//
//        case 3: // Amber (Red + Green mix)
//            // Turn on RED channels for amber (full intensity)
//             for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
//                 PCA9955B_SetIREF(red_channels[i], 0x7C);  // Red current reference
//                 PCA9955B_SetPWM(red_channels[i], 255);    // Full red intensity
//             }
//
//             // Turn on GREEN channels for amber (reduced intensity for amber color)
//             for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
//                 PCA9955B_SetIREF(green_channels[i], 0x15); // Green current reference
//                 PCA9955B_SetPWM(green_channels[i], 30);   // Reduced green intensity (55% for amber)
//             }
//             break;
//    }
//}

/**
 * @brief Complete color palette with all possible RGB combinations
 * @param color: Color selection (0-15 for different colors)
 */
void PCA9955B_SetColorOn(uint8_t color)
{
    // First turn off all LEDs
//	PCA9955B_SetAllLEDsOff();

	    switch(color) {
	        case 0: // Pure Red
	            for (int i = 0; i < sizeof(red_channels_d1)/sizeof(red_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(red_channels_d1[i], 0xFF,PCA9955B_ADDR);
	                PCA9955B_SetPWM(red_channels_d1[i], 255,PCA9955B_ADDR);
	            }
	            for (int i = 0; i < sizeof(red_channels_d2)/sizeof(red_channels_d2[0]); i++) {
					PCA9955B_SetIREF(red_channels_d2[i], 0xFF,PCA9955B_ADDR2);
					PCA9955B_SetPWM(red_channels_d2[i], 255,PCA9955B_ADDR2);
				}
	            break;

	        case 1: // Pure Green
	            for (int i = 0; i < sizeof(green_channels_d1)/sizeof(green_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(green_channels_d1[i], 0xFF,PCA9955B_ADDR);
	                PCA9955B_SetPWM(green_channels_d1[i], 255,PCA9955B_ADDR);
	            }
	            for (int i = 0; i < sizeof(green_channels_d2)/sizeof(green_channels_d2[0]); i++) {
					PCA9955B_SetIREF(green_channels_d2[i], 0xFF,PCA9955B_ADDR2);
					PCA9955B_SetPWM(green_channels_d2[i], 255,PCA9955B_ADDR2);
				}
	            break;

	        case 2: // Pure Blue
	            for (int i = 0; i < sizeof(blue_channels_d1)/sizeof(blue_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(blue_channels_d1[i], 0xFF,PCA9955B_ADDR);
	                PCA9955B_SetPWM(blue_channels_d1[i], 255,PCA9955B_ADDR);
	            }
	            for (int i = 0; i < sizeof(blue_channels_d2)/sizeof(blue_channels_d2[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d2[i], 0xFF,PCA9955B_ADDR2);
					PCA9955B_SetPWM(blue_channels_d2[i], 255,PCA9955B_ADDR2);
				}
	            break;

	        case 3: // Amber/Orange (Red + little Green)
	            // Red at maximum
	            for (int i = 0; i < sizeof(red_channels_d1)/sizeof(red_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(red_channels_d1[i], 0xFF,PCA9955B_ADDR);
	                PCA9955B_SetPWM(red_channels_d1[i], 255,PCA9955B_ADDR);
	            }
	            for (int i = 0; i < sizeof(red_channels_d2)/sizeof(red_channels_d2[0]); i++) {
					PCA9955B_SetIREF(red_channels_d2[i], 0xFF,PCA9955B_ADDR2);
					PCA9955B_SetPWM(red_channels_d2[i], 255,PCA9955B_ADDR2);
				}
	            // Green at low intensity
	            for (int i = 0; i < sizeof(green_channels_d1)/sizeof(green_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(green_channels_d1[i], 0xBF,PCA9955B_ADDR);
	                PCA9955B_SetPWM(green_channels_d1[i], 50,PCA9955B_ADDR);
	            }
	            for (int i = 0; i < sizeof(green_channels_d2)/sizeof(green_channels_d2[0]); i++) {
					PCA9955B_SetIREF(green_channels_d2[i], 0xBF,PCA9955B_ADDR2);
					PCA9955B_SetPWM(green_channels_d2[i], 50,PCA9955B_ADDR2);
				}
	            break;

	        case 4: // Yellow (Red + Green)
	            // Red at maximum
	            for (int i = 0; i < sizeof(red_channels_d1)/sizeof(red_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(red_channels_d1[i], IREF_50MA,PCA9955B_ADDR);
	                PCA9955B_SetPWM(red_channels_d1[i], 255,PCA9955B_ADDR);
	            }

	            // Red at maximum
	            for (int i = 0; i < sizeof(red_channels_d2)/sizeof(red_channels_d2[0]); i++) {
	                PCA9955B_SetIREF(red_channels_d2[i], IREF_50MA,PCA9955B_ADDR2);
	                PCA9955B_SetPWM(red_channels_d2[i], 255,PCA9955B_ADDR2);
	            }

	            // Green at high intensity
	            for (int i = 0; i < sizeof(green_channels_d1)/sizeof(green_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(green_channels_d1[i], IREF_50MA,PCA9955B_ADDR);
	                PCA9955B_SetPWM(green_channels_d1[i], 200,PCA9955B_ADDR);
	            }


	            // Green at high intensity
	            for (int i = 0; i < sizeof(green_channels_d2)/sizeof(green_channels_d2[0]); i++) {
	                PCA9955B_SetIREF(green_channels_d2[i], IREF_50MA,PCA9955B_ADDR2);
	                PCA9955B_SetPWM(green_channels_d2[i], 200,PCA9955B_ADDR2);
	            }
	            break;

	        case 5: // Cyan (Green + Blue)
	            // Green at maximum
	            for (int i = 0; i < sizeof(green_channels_d1)/sizeof(green_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(green_channels_d1[i], IREF_50MA,PCA9955B_ADDR);
	                PCA9955B_SetPWM(green_channels_d1[i], 255,PCA9955B_ADDR);
	            }
	            // Blue at maximum
	            for (int i = 0; i < sizeof(blue_channels_d1)/sizeof(blue_channels_d1[0]); i++) {
	                PCA9955B_SetIREF(blue_channels_d1[i], IREF_50MA,PCA9955B_ADDR);
	                PCA9955B_SetPWM(blue_channels_d1[i], 255,PCA9955B_ADDR);
	            }

	            for (int i = 0; i < sizeof(green_channels_d2)/sizeof(green_channels_d2[0]); i++) {
	                PCA9955B_SetIREF(green_channels_d2[i], IREF_50MA,PCA9955B_ADDR2);
	                PCA9955B_SetPWM(green_channels_d2[i], 255,PCA9955B_ADDR2);
	            }
	            // Blue at maximum
	            for (int i = 0; i < sizeof(blue_channels_d2)/sizeof(blue_channels_d2[0]); i++) {
	                PCA9955B_SetIREF(blue_channels_d2[i], IREF_50MA,PCA9955B_ADDR2);
	                PCA9955B_SetPWM(blue_channels_d2[i], 255,PCA9955B_ADDR2);
	            }
	            break;

	        case 6: // White (Red + Green + Blue)
				// Red
				for (int i = 0; i < sizeof(red_channels_d1)/sizeof(red_channels_d1[0]); i++) {
					PCA9955B_SetIREF(red_channels_d1[i], 0xFF, PCA9955B_ADDR);//170
					PCA9955B_SetPWM(red_channels_d1[i], 255, PCA9955B_ADDR);
				}
				// Green
				for (int i = 0; i < sizeof(green_channels_d1)/sizeof(green_channels_d1[0]); i++) {
					PCA9955B_SetIREF(green_channels_d1[i], 0xFF, PCA9955B_ADDR);//5A
					PCA9955B_SetPWM(green_channels_d1[i], 255, PCA9955B_ADDR);
				}
				// Blue
				for (int i = 0; i < sizeof(blue_channels_d1)/sizeof(blue_channels_d1[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d1[i], 0xFF, PCA9955B_ADDR);
					PCA9955B_SetPWM(blue_channels_d1[i], 255, PCA9955B_ADDR);
				}

				// Red
				for (int i = 0; i < sizeof(red_channels_d2)/sizeof(red_channels_d2[0]); i++) {
					PCA9955B_SetIREF(red_channels_d2[i], 0xFF, PCA9955B_ADDR2);//170
					PCA9955B_SetPWM(red_channels_d2[i], 255, PCA9955B_ADDR2);
				}
				// Green
				for (int i = 0; i < sizeof(green_channels_d2)/sizeof(green_channels_d2[0]); i++) {
					PCA9955B_SetIREF(green_channels_d2[i], 0xFF, PCA9955B_ADDR2);//5A
					PCA9955B_SetPWM(green_channels_d2[i], 255, PCA9955B_ADDR2);
				}
				// Blue
				for (int i = 0; i < sizeof(blue_channels_d2)/sizeof(blue_channels_d2[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d2[i], 0xFF, PCA9955B_ADDR2);
					PCA9955B_SetPWM(blue_channels_d2[i], 255, PCA9955B_ADDR2);
				}
				break;

	        case 7: // Purple (Red + Blue, less red)
				// Red at medium
				for (int i = 0; i < sizeof(red_channels_d1)/sizeof(red_channels_d1[0]); i++) {
					PCA9955B_SetIREF(red_channels_d1[i], IREF_50MA, PCA9955B_ADDR);
					PCA9955B_SetPWM(red_channels_d1[i], 150, PCA9955B_ADDR);
				}
				// Blue at maximum
				for (int i = 0; i < sizeof(blue_channels_d1)/sizeof(blue_channels_d1[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d1[i], IREF_50MA, PCA9955B_ADDR);
					PCA9955B_SetPWM(blue_channels_d1[i], 255, PCA9955B_ADDR);
				}


				// Red at medium
				for (int i = 0; i < sizeof(red_channels_d2)/sizeof(red_channels_d2[0]); i++) {
					PCA9955B_SetIREF(red_channels_d2[i], IREF_50MA, PCA9955B_ADDR2);
					PCA9955B_SetPWM(red_channels_d2[i], 150, PCA9955B_ADDR2);
				}
				// Blue at maximum
				for (int i = 0; i < sizeof(blue_channels_d2)/sizeof(blue_channels_d2[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d2[i], IREF_50MA, PCA9955B_ADDR2);
					PCA9955B_SetPWM(blue_channels_d2[i], 255, PCA9955B_ADDR2);
				}
				break;


	        case 8: // Teal (Green + Blue, more green)
				// Green at maximum
				for (int i = 0; i < sizeof(green_channels_d1)/sizeof(green_channels_d1[0]); i++) {
					PCA9955B_SetIREF(green_channels_d1[i], IREF_50MA, PCA9955B_ADDR);
					PCA9955B_SetPWM(green_channels_d1[i], 255, PCA9955B_ADDR);
				}
				// Blue at medium
				for (int i = 0; i < sizeof(blue_channels_d1)/sizeof(blue_channels_d1[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d1[i], IREF_50MA, PCA9955B_ADDR);
					PCA9955B_SetPWM(blue_channels_d1[i], 180, PCA9955B_ADDR);
				}


				// Green at maximum
				for (int i = 0; i < sizeof(green_channels_d2)/sizeof(green_channels_d2[0]); i++) {
					PCA9955B_SetIREF(green_channels_d2[i], IREF_50MA, PCA9955B_ADDR2);
					PCA9955B_SetPWM(green_channels_d2[i], 255, PCA9955B_ADDR2);
				}
				// Blue at medium
				for (int i = 0; i < sizeof(blue_channels_d2)/sizeof(blue_channels_d2[0]); i++) {
					PCA9955B_SetIREF(blue_channels_d2[i], IREF_50MA, PCA9955B_ADDR2);
					PCA9955B_SetPWM(blue_channels_d2[i], 180, PCA9955B_ADDR2);
				}
				break;

	       /* case 6: // Magenta/Pink (Red + Blue)
	            // Red at maximum
	            for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
	                PCA9955B_SetIREF(red_channels[i], 0xAA);
	                PCA9955B_SetPWM(red_channels[i], 255);
	            }
	            // Blue at maximum
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], 0xFF);
	                PCA9955B_SetPWM(blue_channels[i], 255);
	            }
	            break;





	        case 9: // Lime Green (Green + little Blue)
	            // Green at maximum
	            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
	                PCA9955B_SetIREF(green_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(green_channels[i], 255);
	            }
	            // Blue at low intensity
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(blue_channels[i], 80);
	            }
	            break;

	        case 10: // Sky Blue (Blue + little Green)
	            // Blue at maximum
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(blue_channels[i], 255);
	            }
	            // Green at low intensity
	            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
	                PCA9955B_SetIREF(green_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(green_channels[i], 100);
	            }
	            break;

	        case 11: // Warm White (Red + Green + little Blue)
	            // Red
	            for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
	                PCA9955B_SetIREF(red_channels[i], 0xFF);
	                PCA9955B_SetPWM(red_channels[i], 255);
	            }
	            // Green
	            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
	                PCA9955B_SetIREF(green_channels[i], 0xFF);
	                PCA9955B_SetPWM(green_channels[i], 255);
	            }
	            // Blue (very little)
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], 0xFF);
	                PCA9955B_SetPWM(blue_channels[i], 255);
	            }
	            break;

	        case 12: // Cool White (Blue + Green + little Red)
	            // Blue
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(blue_channels[i], 255);
	            }
	            // Green
	            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
	                PCA9955B_SetIREF(green_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(green_channels[i], 200);
	            }
	            // Red (very little)
	            for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
	                PCA9955B_SetIREF(red_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(red_channels[i], 80);
	            }
	            break;

	        case 13: // Deep Orange (Red + tiny Green)
	            // Red at maximum
	            for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
	                PCA9955B_SetIREF(red_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(red_channels[i], 255);
	            }
	            // Green very low
	            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
	                PCA9955B_SetIREF(green_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(green_channels[i], 30);
	            }
	            break;

	        case 14: // Teal (Green + Blue, more green)
	            // Green at maximum
	            for (int i = 0; i < sizeof(green_channels)/sizeof(green_channels[0]); i++) {
	                PCA9955B_SetIREF(green_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(green_channels[i], 255);
	            }
	            // Blue at medium
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(blue_channels[i], 180);
	            }
	            break;

	        case 15: // Violet (Blue + little Red)
	            // Blue at maximum
	            for (int i = 0; i < sizeof(blue_channels)/sizeof(blue_channels[0]); i++) {
	                PCA9955B_SetIREF(blue_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(blue_channels[i], 255);
	            }
	            // Red at low intensity
	            for (int i = 0; i < sizeof(red_channels)/sizeof(red_channels[0]); i++) {
	                PCA9955B_SetIREF(red_channels[i], IREF_50MA);
	                PCA9955B_SetPWM(red_channels[i], 100);
	            }
	            break;*/

	        default:
	            // Turn off all LEDs for invalid color
	            PCA9955B_SetAllLEDsOff();
	            break;
    }
}

/**
 * @brief Test all colors in sequence
 */
void PCA9955B_TestAllColors(void)
{
//    const char* color_names[] = {
//        "Red", "Green", "Blue", "Amber", "Yellow", "Cyan",
//        "Magenta", "White", "Purple", "Lime", "Sky Blue",
//        "Warm White", "Cool White", "Deep Orange", "Teal", "Violet"
//    };

    for (int color = 0; color < 16; color++) {
        PCA9955B_SetColorOn(color);
        // Printf or debug output: color_names[color]
        HAL_Delay(2000);  // Display each color for 2 seconds

        PCA9955B_SetAllLEDsOff();
        HAL_Delay(500);   // Brief pause between colors
    }
}

/**
 * @brief Updated main loop to test all colors
 */
void main_with_all_colors(void)
{
    while (1)
    {
        // Cycle through all 16 colors
        for (int color = 0; color < 16; color++) {
            PCA9955B_SetColorOn(color);
            HAL_Delay(2000);
            PCA9955B_SetAllLEDsOff();
            HAL_Delay(500);
        }

        HAL_Delay(3000);
    }
}

void PCA9955B_DebugChannels(void)
{
    // Turn off all LEDs first
    PCA9955B_SetAllLEDsOff();

    // Test each channel individually
    for (int ch = 0; ch < 16; ch++) {
        // Turn off previous channel
        if (ch > 0) {
            PCA9955B_SetPWM(ch - 1, 0,PCA9955B_ADDR);
        }

        // Turn on current channel
        PCA9955B_SetIREF(ch, 0x50,PCA9955B_ADDR);  // Medium current
        PCA9955B_SetPWM(ch, 255,PCA9955B_ADDR);    // Full brightness

        // Wait and observe which LED and which color lights up
        HAL_Delay(2000);  // 2 seconds per channel
    }

    // Turn off all at the end
    PCA9955B_SetAllLEDsOff();
}
/**
 * @brief Turn off all LEDs properly
 */
void PCA9955B_SetAllLEDsOff(void)
{
    for (int i = 0; i < 16; i++) {
        PCA9955B_SetPWM(i, 0,PCA9955B_ADDR);
        // Keep IREF high for quick response when turning on
        // PCA9955B_SetIREF(i, HIGH_BRIGHTNESS_IREF);
    }
    for (int i = 0; i < 16; i++) {
		PCA9955B_SetPWM(i, 0,PCA9955B_ADDR2);
		// Keep IREF high for quick response when turning on
		// PCA9955B_SetIREF(i, HIGH_BRIGHTNESS_IREF);
	}
}

void PCA9955B_TestSequence(void)
{
    // Test each color at maximum brightness
    PCA9955B_SetColorOn(0); // Red
    HAL_Delay(1000);

    PCA9955B_SetColorOn(1); // Green
    HAL_Delay(1000);

    PCA9955B_SetColorOn(2); // Blue
    HAL_Delay(1000);

    PCA9955B_SetAllLEDsOff();
    HAL_Delay(1000);
}

/**
 * @brief Run red LEDs sequentially with nanosecond timing
 * @param delay_ns: Delay in nanoseconds between each LED (minimum ~100ns due to I2C limitations)
 * @param cycles: Number of complete cycles to run
 */
/**
 * @brief Run blue LEDs two by two with nanosecond timing
 * @param delay_ns: Delay in nanoseconds between each pair of LEDs (minimum ~100ns due to I2C limitations)
 * @param cycles: Number of complete cycles to run
 */
/*void PCA9955B_FastRedSequence(uint32_t delay_ns, uint8_t cycles)
{
    uint8_t blue_channels[] = BLUE_CHANNELS;
    uint8_t num_blue_channels = sizeof(blue_channels) / sizeof(blue_channels[0]);

    // Pre-configure all blue channels with maximum brightness settings
    for (int i = 0; i < num_blue_channels; i++) {
        PCA9955B_SetIREF(blue_channels[i], HIGH_BRIGHTNESS_IREF);
    }

    // Calculate delay in CPU cycles for nanosecond precision
    // Assuming 120MHz system clock (from your PLL config: MSI*60/2 = 4MHz*60/2 = 120MHz)
    uint32_t cpu_freq_mhz = 120;
    uint32_t delay_cycles = (delay_ns * cpu_freq_mhz) / 1000;

    // Minimum delay cycles to account for I2C transmission time
    if (delay_cycles < 1000) delay_cycles = 1000; // ~8.3µs minimum for I2C

    // Disable interrupts for precise timing
    __disable_irq();

    for (uint8_t cycle = 0; cycle < cycles; cycle++) {
        // Forward sequence: turn on LEDs two by two
        for (int i = 0; i < num_blue_channels; i += 2) {
            // Turn off previous pair of LEDs (if not first iteration)
            if (i > 1) {
                PCA9955B_SetPWM_Fast(blue_channels[i-2], 0);
                if (i-1 < num_blue_channels) {
                    PCA9955B_SetPWM_Fast(blue_channels[i-1], 0);
                }
            }

            // Turn on current pair of LEDs
            PCA9955B_SetPWM_Fast(blue_channels[i], HIGH_BRIGHTNESS_PWM);
            if (i+1 < num_blue_channels) {
                PCA9955B_SetPWM_Fast(blue_channels[i+1], HIGH_BRIGHTNESS_PWM);
            }

            // Precise nanosecond delay using CPU cycles
            for (volatile uint32_t delay = 0; delay < delay_cycles; delay++) {
                __NOP(); // No operation - 1 CPU cycle
            }
        }

        // Turn off last pair of LEDs and add inter-cycle delay
        int last_pair_index = ((num_blue_channels - 1) / 2) * 2;
        PCA9955B_SetPWM_Fast(blue_channels[last_pair_index], 0);
        if (last_pair_index + 1 < num_blue_channels) {
            PCA9955B_SetPWM_Fast(blue_channels[last_pair_index + 1], 0);
        }

        // Inter-cycle delay (10x the step delay)
        for (volatile uint32_t delay = 0; delay < (delay_cycles * 10); delay++) {
            __NOP();
        }
    }

    // Re-enable interrupts
    __enable_irq();

    // Ensure all blue LEDs are off at the end
    for (int i = 0; i < num_blue_channels; i++) {
        PCA9955B_SetPWM(blue_channels[i], 0);
    }
}*/

/**
 * @brief Fast PWM write function with minimal overhead
 * @param channel: LED channel number
 * @param pwm_value: PWM value (0-255)
 * @return HAL status (optimized for speed)
 */
//HAL_StatusTypeDef PCA9955B_SetPWM_Fast(uint8_t channel, uint8_t pwm_value)
//{
//    if (channel > 15) return HAL_ERROR;
//
//    // Direct register write with minimal function call overhead
//    uint8_t data[2] = {PCA9955B_PWM0 + channel, pwm_value};
//
//    // Use fast I2C transmission with reduced timeout for speed
//    return HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, data, 2, 10);
//}
//
///**
// * @brief Ultra-fast red sequence using DMA for maximum speed
// * @param delay_us: Delay in microseconds between LEDs (minimum 10µs)
// * @param cycles: Number of cycles
// */
//void PCA9955B_UltraFastRedSequence(uint32_t delay_us, uint8_t cycles)
//{
//    uint8_t red_channels[] = RED_CHANNELS;
//    uint8_t num_red_channels = sizeof(red_channels) / sizeof(red_channels[0]);
//
//    // Pre-prepared I2C command buffers for each LED
//    uint8_t led_on_commands[5][2];
//    uint8_t led_off_commands[5][2];
//
//    // Prepare command arrays
//    for (int i = 0; i < num_red_channels; i++) {
//        led_on_commands[i][0] = PCA9955B_PWM0 + red_channels[i];
//        led_on_commands[i][1] = HIGH_BRIGHTNESS_PWM;
//        led_off_commands[i][0] = PCA9955B_PWM0 + red_channels[i];
//        led_off_commands[i][1] = 0x00;
//    }
//
//    // Set IREF for all red channels once
//    for (int i = 0; i < num_red_channels; i++) {
//        PCA9955B_SetIREF(red_channels[i], HIGH_BRIGHTNESS_IREF);
//    }
//
//    // Convert microseconds to timer ticks for precise timing
//    uint32_t delay_ticks = delay_us * (SystemCoreClock / 1000000);
//
//    for (uint8_t cycle = 0; cycle < cycles; cycle++) {
//        for (int i = 0; i < num_red_channels; i++) {
//            // Turn off previous LED
//            if (i > 0) {
//                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, led_off_commands[i-1], 2, 5);
//            }
//
//            // Turn on current LED
//            HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, led_on_commands[i], 2, 5);
//
//            // Precise microsecond delay
//            uint32_t start_tick = DWT->CYCCNT;
//            while ((DWT->CYCCNT - start_tick) < delay_ticks);
//        }
//
//        // Turn off last LED
//        HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, led_off_commands[num_red_channels-1], 2, 5);
//
//        // Inter-cycle delay
//        uint32_t start_tick = DWT->CYCCNT;
//        while ((DWT->CYCCNT - start_tick) < (delay_ticks * 10));
//    }
//}
/**
 * @brief Fast PWM write function with minimal overhead
 * @param channel: LED channel number
 * @param pwm_value: PWM value (0-255)
 * @return HAL status (optimized for speed)
 */
//HAL_StatusTypeDef PCA9955B_SetPWM_Fast(uint8_t channel, uint8_t pwm_value)
//{
//    if (channel > 15) return HAL_ERROR;
//
//    // Direct register write with minimal function call overhead
//    uint8_t data[2] = {PCA9955B_PWM0 + channel, pwm_value};
//
//    // Use fast I2C transmission with reduced timeout for speed
//    return HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, data, 2, 10);
//}


/*void PCA9955B_UltraFastRedSequence(uint32_t delay_us, uint8_t cycles) {
    uint8_t red_channels[] = RED_CHANNELS;
    uint8_t num_red_channels = sizeof(red_channels) / sizeof(red_channels[0]);

    // Convert microseconds to milliseconds for HAL_Delay
    uint32_t delay_ms = delay_us / 1000;
    if (delay_ms == 0) delay_ms = 1;  // Minimum 1ms

    // Set higher IREF for increased brightness (40mA -> 60mA)
    for (int i = 0; i < num_red_channels; i++) {
        PCA9955B_SetIREF(red_channels[i], 255);  // Increased from ~120 to 180 for 60mA
    }

    // Turn off all LEDs initially (only once)
    for (int i = 0; i < num_red_channels; i++) {
        uint8_t off_cmd[2] = {PCA9955B_PWM0 + red_channels[i], 0x00};
        HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
    }

    for (uint8_t cycle = 0; cycle < cycles; cycle++) {
        for (int i = 0; i < num_red_channels; i++) {
            // Turn off only the previous LED (reduces I2C traffic and flickering)
            if (i > 0) {
                uint8_t off_cmd[2] = {PCA9955B_PWM0 + red_channels[i-1], 0x00};
                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
            } else if (cycle > 0) {
                // First LED of new cycle - turn off last LED from previous cycle
                uint8_t off_cmd[2] = {PCA9955B_PWM0 + red_channels[num_red_channels-1], 0x00};
                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
            }

            // Turn on current LED
            uint8_t on_cmd[2] = {PCA9955B_PWM0 + red_channels[i], HIGH_BRIGHTNESS_PWM};
            HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, on_cmd, 2, 5);

            // Use the delay parameter
            HAL_Delay(delay_ms);
        }
    }

    // Turn off last LED
    uint8_t off_cmd[2] = {PCA9955B_PWM0 + red_channels[num_red_channels-1], 0x00};
    HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
}

void PCA9955B_UltraFastGreenSequence(uint32_t delay_us, uint8_t cycles) {
    uint8_t green_channels[] = GREEN_CHANNELS;
    uint8_t num_green_channels = sizeof(green_channels) / sizeof(green_channels[0]);

    // Convert microseconds to milliseconds for HAL_Delay
    uint32_t delay_ms = delay_us / 1000;
    if (delay_ms == 0) delay_ms = 1;  // Minimum 1ms

    // Set higher IREF for increased brightness (40mA -> 60mA)
    for (int i = 0; i < num_green_channels; i++) {
        PCA9955B_SetIREF(green_channels[i], 255);  // Increased from ~120 to 180 for 60mA
    }

    // Turn off all LEDs initially (only once)
    for (int i = 0; i < num_green_channels; i++) {
        uint8_t off_cmd[2] = {PCA9955B_PWM0 + green_channels[i], 0x00};
        HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
    }

    for (uint8_t cycle = 0; cycle < cycles; cycle++) {
        for (int i = 0; i < num_green_channels; i++) {
            // Turn off only the previous LED (reduces I2C traffic and flickering)
            if (i > 0) {
                uint8_t off_cmd[2] = {PCA9955B_PWM0 + green_channels[i-1], 0x00};
                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
            } else if (cycle > 0) {
                // First LED of new cycle - turn off last LED from previous cycle
                uint8_t off_cmd[2] = {PCA9955B_PWM0 + green_channels[num_green_channels-1], 0x00};
                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
            }

            // Turn on current LED
            uint8_t on_cmd[2] = {PCA9955B_PWM0 + green_channels[i], HIGH_BRIGHTNESS_PWM};
            HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, on_cmd, 2, 5);

            // Use the delay parameter
            HAL_Delay(delay_ms);
        }
    }

    // Turn off last LED
    uint8_t off_cmd[2] = {PCA9955B_PWM0 + green_channels[num_green_channels-1], 0x00};
    HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
}

void PCA9955B_UltraFastBlueSequence(uint32_t delay_us, uint8_t cycles) {
    uint8_t blue_channels[] = BLUE_CHANNELS;
    uint8_t num_blue_channels = sizeof(blue_channels) / sizeof(blue_channels[0]);

    // Convert microseconds to milliseconds for HAL_Delay
    uint32_t delay_ms = delay_us / 1000;
    if (delay_ms == 0) delay_ms = 1;  // Minimum 1ms

    // Set higher IREF for increased brightness (40mA -> 60mA)
    for (int i = 0; i < num_blue_channels; i++) {
        PCA9955B_SetIREF(blue_channels[i], 255);  // Increased from ~120 to 180 for 60mA
    }

    // Turn off all LEDs initially (only once)
    for (int i = 0; i < num_blue_channels; i++) {
        uint8_t off_cmd[2] = {PCA9955B_PWM0 + blue_channels[i], 0x00};
        HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
    }

    for (uint8_t cycle = 0; cycle < cycles; cycle++) {
        for (int i = 0; i < num_blue_channels; i++) {
            // Turn off only the previous LED (reduces I2C traffic and flickering)
            if (i > 0) {
                uint8_t off_cmd[2] = {PCA9955B_PWM0 + blue_channels[i-1], 0x00};
                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
            } else if (cycle > 0) {
                // First LED of new cycle - turn off last LED from previous cycle
                uint8_t off_cmd[2] = {PCA9955B_PWM0 + blue_channels[num_blue_channels-1], 0x00};
                HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
            }

            // Turn on current LED
            uint8_t on_cmd[2] = {PCA9955B_PWM0 + blue_channels[i], HIGH_BRIGHTNESS_PWM};
            HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, on_cmd, 2, 5);

            // Use the delay parameter
            HAL_Delay(delay_ms);
        }
    }

    // Turn off last LED
    uint8_t off_cmd[2] = {PCA9955B_PWM0 + blue_channels[num_blue_channels-1], 0x00};
    HAL_I2C_Master_Transmit(&hi2c4, PCA9955B_ADDR, off_cmd, 2, 5);
}*/

/**
 * @brief Update LED brightness (0-255)
 */
void PCA9955B_UpdateBrightness(uint8_t brightness_value)
{
    global_brightness = brightness_value;
    PCA9955B_WriteRegister(PCA9955B_GRPPWM, brightness_value,PCA9955B_ADDR);
    PCA9955B_WriteRegister2(PCA9955B_GRPPWM, brightness_value);
}

HAL_StatusTypeDef PCA9955B_ReadPWM0(I2C_HandleTypeDef *hi2c,uint8_t *pwm0)
{
    return HAL_I2C_Mem_Read(
        hi2c,                     // I2C handle (e.g. &hi2c1)
		PCA9955B_ADDR,         // Device address
        PCA9955B_PWM0_REG,         // Register address
        I2C_MEMADD_SIZE_8BIT,      // 8-bit register
        pwm0,                      // Output buffer
        1,                         // 1 byte
        1000
    );
}

