/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DISPLAY_UART_CTS_Pin GPIO_PIN_0
#define DISPLAY_UART_CTS_GPIO_Port GPIOC
#define USB_Switch_Pin GPIO_PIN_1
#define USB_Switch_GPIO_Port GPIOC
#define Display_UART_RX_Pin GPIO_PIN_2
#define Display_UART_RX_GPIO_Port GPIOC
#define Display_UART_TX_Pin GPIO_PIN_3
#define Display_UART_TX_GPIO_Port GPIOC
#define Level_Sensor_1_Pin GPIO_PIN_0
#define Level_Sensor_1_GPIO_Port GPIOA
#define Level_Sensor_2_Pin GPIO_PIN_1
#define Level_Sensor_2_GPIO_Port GPIOA
#define Level_Sensor_3_Pin GPIO_PIN_2
#define Level_Sensor_3_GPIO_Port GPIOA
#define TER_Switch_Pin GPIO_PIN_7
#define TER_Switch_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define Mains_mon_Pin GPIO_PIN_1
#define Mains_mon_GPIO_Port GPIOB
#define Mains_mon_EXTI_IRQn EXTI1_IRQn
#define BAT_MON_Pin GPIO_PIN_2
#define BAT_MON_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOE
#define REMOT_ALM_Pin GPIO_PIN_8
#define REMOT_ALM_GPIO_Port GPIOE
#define LID_SENSOR_Pin GPIO_PIN_9
#define LID_SENSOR_GPIO_Port GPIOE
#define Buzzer_control_Pin GPIO_PIN_10
#define Buzzer_control_GPIO_Port GPIOE
#define RTD1_SPI_CS_Pin GPIO_PIN_11
#define RTD1_SPI_CS_GPIO_Port GPIOE
#define RTD2_SPI_CS_Pin GPIO_PIN_12
#define RTD2_SPI_CS_GPIO_Port GPIOE
#define RTD_SPI_SCK_Pin GPIO_PIN_13
#define RTD_SPI_SCK_GPIO_Port GPIOE
#define RTS_SPI_MISO_Pin GPIO_PIN_14
#define RTS_SPI_MISO_GPIO_Port GPIOE
#define RTD_SPI_MOSI_Pin GPIO_PIN_15
#define RTD_SPI_MOSI_GPIO_Port GPIOE
#define Vacuum_SPI_CS_Pin GPIO_PIN_10
#define Vacuum_SPI_CS_GPIO_Port GPIOB
#define Vacuum_PWM_Pin GPIO_PIN_11
#define Vacuum_PWM_GPIO_Port GPIOB
#define Vaccum_SPI_SCK_Pin GPIO_PIN_13
#define Vaccum_SPI_SCK_GPIO_Port GPIOB
#define Vacuum_SPI_MISO_Pin GPIO_PIN_14
#define Vacuum_SPI_MISO_GPIO_Port GPIOB
#define Vacuum_SPI_MOSII_Pin GPIO_PIN_15
#define Vacuum_SPI_MOSII_GPIO_Port GPIOB
#define RS485_DE_Pin GPIO_PIN_10
#define RS485_DE_GPIO_Port GPIOD
#define LED_driver_scl_Pin GPIO_PIN_12
#define LED_driver_scl_GPIO_Port GPIOD
#define LED_driver_sda_Pin GPIO_PIN_13
#define LED_driver_sda_GPIO_Port GPIOD
#define USB_fault_Pin GPIO_PIN_8
#define USB_fault_GPIO_Port GPIOA
#define USB_en_Pin GPIO_PIN_9
#define USB_en_GPIO_Port GPIOA
#define USB_detect_Pin GPIO_PIN_10
#define USB_detect_GPIO_Port GPIOA
#define WIFI_RST_Pin GPIO_PIN_6
#define WIFI_RST_GPIO_Port GPIOD
#define WIFI_WHP_Pin GPIO_PIN_7
#define WIFI_WHP_GPIO_Port GPIOD
#define WIFI_UART_RTS_Pin GPIO_PIN_3
#define WIFI_UART_RTS_GPIO_Port GPIOB
#define WIFI_UART_CTS_Pin GPIO_PIN_4
#define WIFI_UART_CTS_GPIO_Port GPIOB
#define WIFI_WKP_Pin GPIO_PIN_5
#define WIFI_WKP_GPIO_Port GPIOB
#define WIFI_UART_TX_Pin GPIO_PIN_6
#define WIFI_UART_TX_GPIO_Port GPIOB
#define WIFI_UART_RX_Pin GPIO_PIN_7
#define WIFI_UART_RX_GPIO_Port GPIOB
#define EPROM_I2C_SCL_Pin GPIO_PIN_8
#define EPROM_I2C_SCL_GPIO_Port GPIOB
#define EPROM_I2C_SDA_Pin GPIO_PIN_9
#define EPROM_I2C_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
