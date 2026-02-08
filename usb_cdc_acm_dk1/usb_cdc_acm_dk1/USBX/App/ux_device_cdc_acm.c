/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "Filex.h"
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
UX_SLAVE_CLASS_CDC_ACM *cdc_acm;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t EMMC_buffer[64];
extern FX_MEDIA        mmc_disk;
extern FX_FILE fx_file;
/* USER CODE END 0 */

/**
  * @brief  USBD_CDC_ACM_Activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Activate */
 // UX_PARAMETER_NOT_USED(cdc_acm_instance);
  cdc_acm = (UX_SLAVE_CLASS_CDC_ACM *)cdc_acm_instance;

  /* USER CODE END USBD_CDC_ACM_Activate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_Deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  /* USER CODE END USBD_CDC_ACM_Deactivate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_ParameterChange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  /* USER CODE END USBD_CDC_ACM_ParameterChange */

  return;
}

/* USER CODE BEGIN 1 */
VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input)
{
      /* Private Variables */
      ULONG tx_actual_length;
      const uint8_t message[] = "USBX Application Running!\r\n";
      while(1)
      {
             ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *)(message), sizeof(message), &tx_actual_length);
             tx_thread_sleep(100);
      }
}

void filex_write_test(void)
{
    FX_FILE file;
    UINT status;
    UCHAR buffer[64] = "Hello from FileX USB test!\r\n";

//    /* Wait for media ready */
//    while (mmc_disk.fx_media_id != FX_MEDIA_ID)
//    {
//        tx_thread_sleep(10);
//    }

    printf("Opening file...\n");

    status = fx_file_create(&mmc_disk, "test.bin");
    if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
        Error_Handler();

    status = fx_file_open(&mmc_disk, &file, "test.bin", FX_OPEN_FOR_WRITE);
    if (status != FX_SUCCESS)
        Error_Handler();

    fx_file_seek(&file, file.fx_file_current_file_size);

    printf("Writing...\n");
    status = fx_file_write(&file, buffer, sizeof(buffer));
    if (status != FX_SUCCESS)
        Error_Handler();

    fx_file_close(&file);
    fx_media_flush(&mmc_disk);

    printf("File write OK!\n");
}

VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input)
{
      /* Private Variables */
      ULONG rx_actual_length;
      uint8_t UserRxBuffer[64];
      ULONG total_bytes_written = 0;
      uint8_t file_opened = 0;
//Open_disk();
//      filex_write_test();

//      creating_file("created_app.bin");

      /* Infinite Loop */
      while(1)
      {
             if(cdc_acm != UX_NULL)
             {
//                   ux_device_class_cdc_acm_read(cdc_acm, (UCHAR *)UserRxBuffer, 64, &rx_actual_length);


//                     memcpy(EMMC_buffer,UserRxBuffer,64);
//                    Open_N_write("file1.bin");



             }
             tx_thread_sleep(10);
      }
while(1)
{
//	if (cdc_acm != UX_NULL)
//	             {
//	                 /* Blocking read — waits until host sends data */
//	                 if (ux_device_class_cdc_acm_read(cdc_acm, UserRxBuffer, sizeof(UserRxBuffer), &rx_actual_length) == UX_SUCCESS)
//	                 {
//	                     if (!file_opened)
//	                     {
//	                         /* Open or create file */
//	                         if (fx_file_create(&mmc_disk, "file1.bin") != FX_SUCCESS)
//	                         {
//	                             /* Ignore if file already exists */
//	                         }
//
//	                         if (fx_file_open(&mmc_disk, &fx_file, "file1.bin", FX_OPEN_FOR_WRITE) == FX_SUCCESS)
//	                         {
//	                             /* Seek to end of file */
//	                             fx_file_seek(&fx_file, fx_file.fx_file_current_file_size);
//	                             file_opened = 1;
//	                         }
//	                     }
//
//	                     if (file_opened && rx_actual_length > 0)
//	                     {
//	                         /* Write only the received data length */
//	                         fx_file_write(&fx_file, UserRxBuffer, rx_actual_length);
//	                         total_bytes_written += rx_actual_length;
//	                     }
//
//	                     /* Optional: detect transfer end */
//	                     if (rx_actual_length < sizeof(UserRxBuffer))
//	                     {
//	                         /* Maybe last chunk, close file */
//	                         fx_file_close(&fx_file);
//	                         fx_media_flush(&mmc_disk);   // flush cache to disk
//	                         file_opened = 0;
//
//	                         printf("File transfer complete. %lu bytes written\n", total_bytes_written);
//	                         total_bytes_written = 0;
//	                     }
//	                 }
//	             }
//	             tx_thread_sleep(1);
}


}
/* USER CODE END 1 */
