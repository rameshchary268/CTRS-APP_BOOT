/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FX_STM32_MMC_DRIVER_H
#define FX_STM32_MMC_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "fx_api.h"

#include "stm32u5xx_hal.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

extern TX_SEMAPHORE mmc_tx_semaphore;
extern TX_SEMAPHORE mmc_rx_semaphore;

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */
/* Default timeout used to wait for fx operations */
#define FX_STM32_MMC_DEFAULT_TIMEOUT                           (10 * TX_TIMER_TICKS_PER_SECOND)

/* let the filex low-level driver initialize the MMC driver */
#define FX_STM32_MMC_INIT                                       0

/* Enable the cache maintenance, needed when using MMC DMA
 * and accessing buffers in cacheable area
 * this is valid only for CM7 based products or those
 * with dedicated cache IP.
 * For STM32U5 this flag should be always set to 0 unless external
 * memories are being used.
 */
#define FX_STM32_MMC_CACHE_MAINTENANCE                    	    0

/* Use the MMC DMA API */
#define FX_STM32_MMC_DMA_API                              	    1

/* MMC instance to be used by FileX */
#define FX_STM32_MMC_INSTANCE                                   0

/* Default sector size, used by the driver */
#define FX_STM32_MMC_DEFAULT_SECTOR_SIZE                        512

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Define the macro to get the current time in ticks */
#define FX_STM32_MMC_CURRENT_TIME()                            tx_time_get()

/* Macro called before initializing the MMC driver
 * e.g. create a semaphore used for transfer notification */
/* USER CODE BEGIN FX_STM32_MMC_PRE_INIT_TX */

#define FX_STM32_MMC_PRE_INIT(_media_ptr)                do { \
                                                          if ((tx_semaphore_create(&mmc_rx_semaphore, "mmc rx transfer semaphore", 0) != TX_SUCCESS) || \
                                                              (tx_semaphore_create(&mmc_tx_semaphore, "mmc tx transfer semaphore", 0) != TX_SUCCESS))  \
                                                          { \
                                                            _media_ptr->fx_media_driver_status = FX_IO_ERROR; \
                                                          } \
                                                        } while(0)

/* USER CODE END FX_STM32_MMC_PRE_INIT_TX */

/* Macro called after initializing the MMC driver */
/* USER CODE BEGIN FX_STM32_MMC_POST_INIT */

#define FX_STM32_MMC_POST_INIT(_media_ptr)

/* USER CODE END FX_STM32_MMC_POST_INIT */

/* Macro called after the MMC deinit */

/* USER CODE BEGIN FX_STM32_MMC_POST_DEINIT_TX */

#define FX_STM32_MMC_POST_DEINIT(_media_ptr)             do { \
                                                          tx_semaphore_delete(&mmc_rx_semaphore); \
                                                          tx_semaphore_delete(&mmc_tx_semaphore); \
                                                        } while(0)

/* USER CODE END FX_STM32_MMC_POST_DEINIT_TX */

/* Macro called after the abort request */
/* USER CODE BEGIN FX_STM32_MMC_POST_ABORT */

#define FX_STM32_MMC_POST_ABORT(_media_ptr)

/* USER CODE END FX_STM32_MMC_POST_ABORT */

/* Macro called before performing read operation */
/* USER CODE BEGIN FX_STM32_MMC_PRE_READ_TRANSFER_DMA */

#define FX_STM32_MMC_PRE_READ_TRANSFER(_media_ptr)
/* USER CODE END FX_STM32_MMC_PRE_READ_TRANSFER_DMA */

/* Macro called after performing read operation */
/* USER CODE BEGIN FX_STM32_MMC_POST_READ_TRANSFER_TX */

#define FX_STM32_MMC_POST_READ_TRANSFER(_media_ptr)

/* USER CODE END FX_STM32_MMC_POST_READ_TRANSFER_TX */

/* Macro for read error handling */
/* USER CODE BEGIN FX_STM32_MMC_READ_TRANSFER_ERROR_TX */

#define FX_STM32_MMC_READ_TRANSFER_ERROR(_status_)

/* USER CODE END FX_STM32_MMC_READ_TRANSFER_ERROR_TX */

/* Define how to notify about Read completion operation */
/* USER CODE BEGIN FX_STM32_MMC_READ_CPLT_NOTIFY_TX */

#define FX_STM32_MMC_READ_CPLT_NOTIFY()                  do { \
                                                          if(tx_semaphore_get(&mmc_rx_semaphore, FX_STM32_MMC_DEFAULT_TIMEOUT) != TX_SUCCESS) \
                                                            { \
                                                              return FX_IO_ERROR; \
                                                            } \
                                                        } while(0)

/* USER CODE END FX_STM32_MMC_READ_CPLT_NOTIFY_TX */

/* Define how to notify about write completion operation */
/* USER CODE BEGIN FX_STM32_MMC_WRITE_CPLT_NOTIFY_TX */

#define FX_STM32_MMC_WRITE_CPLT_NOTIFY()                 do { \
                                                          if(tx_semaphore_get(&mmc_tx_semaphore, FX_STM32_MMC_DEFAULT_TIMEOUT) != TX_SUCCESS) \
                                                            { \
                                                              return FX_IO_ERROR; \
                                                            } \
                                                        } while(0)

/* USER CODE END FX_STM32_MMC_WRITE_CPLT_NOTIFY_TX */

/* Macro called before performing write operation */
/* USER CODE BEGIN FX_STM32_MMC_PRE_WRITE_TRANSFER_TX */

#define FX_STM32_MMC_PRE_WRITE_TRANSFER(_media_ptr)

/* USER CODE END FX_STM32_MMC_PRE_WRITE_TRANSFER_TX */

/* Macro called after performing write operation */
/* USER CODE BEGIN FX_STM32_MMC_POST_WRITE_TRANSFER */

#define FX_STM32_MMC_POST_WRITE_TRANSFER(_media_ptr)

/* USER CODE END FX_STM32_MMC_POST_WRITE_TRANSFER */

/* Macro for write error handling */
/* USER CODE BEGIN FX_STM32_MMC_WRITE_TRANSFER_ERROR */

#define FX_STM32_MMC_WRITE_TRANSFER_ERROR(_status_)

/* USER CODE END FX_STM32_MMC_WRITE_TRANFSER_ERROR */

/* Exported functions prototypes ---------------------------------------------*/

INT fx_stm32_mmc_init(UINT instance);
INT fx_stm32_mmc_deinit(UINT instance);

INT fx_stm32_mmc_get_status(UINT instance);

INT fx_stm32_mmc_read_blocks(UINT instance, UINT *buffer, UINT start_block, UINT total_blocks);
INT fx_stm32_mmc_write_blocks(UINT instance, UINT *buffer, UINT start_block, UINT total_blocks);

VOID fx_stm32_mmc_driver(FX_MEDIA *media_ptr);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif

#endif /* FX_STM32_MMC_DRIVER_H */
