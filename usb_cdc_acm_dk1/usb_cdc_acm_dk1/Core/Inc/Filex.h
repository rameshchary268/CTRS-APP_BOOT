/*
 * Filex.h
 *
 *  Created on: Mar 18, 2025
 *      Author: rameshchary.vadla
 */

#ifndef INC_FILEX_H_
#define INC_FILEX_H_

#include "fx_stm32_mmc_driver.h"



 #define FX_MMC_VOLUME_NAME "STM32_MMC_DISK"
  #define FX_MMC_NUMBER_OF_FATS             1

#define MMC_NUM_OF_SECTORS            5     /* Total number of sectors   */

#define FX_MMC_HIDDEN_SECTORS             0

#define MMC_SECTORSIZE   ((uint32_t)512U) /*!< Sector size is 512 bytes */

#define MMC_SIZE        ((MMC_SECOTRSIZE * NUM_OF_SECTORS) >> 2) /* Total data size in bytes */



void Open_disk(void);
void format(void);
void creating_file(char *file_name);
void Open_N_read_file(char *ptr);
void Open_N_write(char *file_name,char *data);
#endif /* INC_FILEX_H_ */
