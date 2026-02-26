
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_filex.h
  * @author  MCD Application Team
  * @brief   FileX applicative header file
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
#ifndef __APP_FILEX_H__
#define __APP_FILEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "fx_api.h"
#include "fx_stm32_mmc_driver.h"
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
typedef enum {
  SD_INIT,
  SD_BUSY,
  SD_DONE,
  SD_WAIT,
  SD_STOP,
  SD_HALT,
  SD_ERR,
} SD_STATE;


typedef struct {
 SD_STATE  state;
 uint32_t  poll_flag;
 uint8_t   semaphore;
 uint32_t  wait_flag;
 uint8_t   File_Available;
 uint32_t  offset;
 int32_t   file_size;
 uint8_t   bin_present;
 uint8_t   init_done;
 uint8_t   Bist_done;
 uint8_t   CFC_4_20mA_file;
 uint8_t   boot_present;
 uint8_t   certificate_up;
 uint8_t   update_page_flag;
} SD_INFO;

#define SD_INFO_DEFAULT (SD_INFO) {SD_INIT, 1,1, 1, 1, 0, 0, 0,0,0,0,0,0}
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_FileX_Init(VOID *memory_ptr);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* Main thread Name */
#ifndef FX_APP_THREAD_NAME
  #define FX_APP_THREAD_NAME "FileX app thread"
#endif

/* Main thread time slice */
#ifndef FX_APP_THREAD_TIME_SLICE
  #define FX_APP_THREAD_TIME_SLICE TX_NO_TIME_SLICE
#endif

/* Main thread auto start */
#ifndef FX_APP_THREAD_AUTO_START
  #define FX_APP_THREAD_AUTO_START TX_AUTO_START
#endif

/* Main thread preemption threshold */
#ifndef FX_APP_PREEMPTION_THRESHOLD
  #define FX_APP_PREEMPTION_THRESHOLD FX_APP_THREAD_PRIO
#endif

/* fx mmc volume name */
#ifndef FX_MMC_VOLUME_NAME
  #define FX_MMC_VOLUME_NAME "STM32_MMC_DISK"
#endif

/* fx mmc number of FATs */
#ifndef FX_MMC_NUMBER_OF_FATS
  #define FX_MMC_NUMBER_OF_FATS             1
#endif

/* fx mmc Hidden sectors */
#ifndef FX_MMC_HIDDEN_SECTORS
  #define FX_MMC_HIDDEN_SECTORS             0
#endif

/* USER CODE BEGIN PD */
int dayOfWeek(int d, int m, int y);
int weekOfMonth(int day, int month, int year);
int Settings_Data_sd(void);
int SD_PO(char *name);
int convert_to_string (char *name);
UINT List_Folders(CHAR *path);
void Sorting_Array();
/* USER CODE END PD */

/* USER CODE BEGIN 1 */
#define MAX_ITEMS 13
#define MAX_NAME_LEN 255

typedef struct{
	uint32_t File_address;
}FILE_EXPLORER;

typedef struct {
	char name[MAX_NAME_LEN];
    ULONG fileSizes;
    UINT type;
    int16_t file_sel;
} DIRECTORY_LIST;


/* USER CODE END 1 */
#ifdef __cplusplus
}
#endif
#endif /* __APP_FILEX_H__ */
