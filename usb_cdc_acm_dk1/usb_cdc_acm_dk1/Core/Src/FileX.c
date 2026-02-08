/*
 * FileX.c
 *
 *  Created on: Mar 18, 2025
 *      Author: rameshchary.vadla
 */

#include <FileX.h>
#include <fx_api.h>
#include <app_filex.h>
#include <main.h>

extern ALIGN_32BYTES (uint32_t fx_mmc_media_memory[FX_STM32_MMC_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);
FX_MEDIA        mmc_disk;

CHAR read_buffer[32];
UINT mmc_status = FX_SUCCESS;
//CHAR data[] = "This is FileX working on STM32";

FX_FILE fx_file;
extern uint8_t EMMC_buffer[512];
void format(void)
{


	mmc_status = fx_media_format(&mmc_disk,					// eMMC_disk pointer
				fx_stm32_mmc_driver,								 // Driver entry
				(VOID*) FX_NULL,							// Device info pointer
				(VOID*) fx_mmc_media_memory,				// Media Buffer Pointer
				sizeof(fx_mmc_media_memory),					// Media Buffer Size
				FX_MMC_VOLUME_NAME,                            	   // Volume Name
				FX_MMC_NUMBER_OF_FATS,                             // Number of FATs
				32,                                             // Directory Entries
				FX_MMC_HIDDEN_SECTORS,                             // Hidden sectors
				MMC_NUM_OF_SECTORS, 								// Total sectors
				MMC_SECTORSIZE,		                               // Sector size
				1,                                            // Sectors per cluster
				1,                                                  // Heads
				1);                                             // Sectors per track

}

void Open_disk(void)
{


	/* Open the disk driver */
	  mmc_status =  fx_media_open(&mmc_disk, FX_MMC_VOLUME_NAME, fx_stm32_mmc_driver, (VOID *)FX_NULL, (VOID *) fx_mmc_media_memory, sizeof(fx_mmc_media_memory));

	  /* Check the media open mmc_mmc_status */
	    if (mmc_status != FX_SUCCESS)
	    {
	      /* USER CODE BEGIN MMC open error */
	  		Error_Handler();
	      /* USER CODE END MMC open error */
	    }
}


void creating_file(char *file_name)
{
	mmc_status = fx_file_create(&mmc_disk, file_name);
		/* Check the create mmc_status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Check for an already created mmc_status. This is expected on the
			 second pass of this loop!  */
			if (mmc_status != FX_ALREADY_CREATED) {
				/* Create error, call error handler.  */
				Error_Handler();
			}
		}

}

void Open_N_write(char *file_name,char *data){

	uint8_t check_buffer[] = "hgfdsdfghjlkjhgfds\n\r";

	/* Open the test file.  */
		mmc_status = fx_file_open(&mmc_disk, &fx_file, file_name, FX_OPEN_FOR_WRITE);
		/* Check the file open mmc_status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Error opening file, call error handler.  */
			Error_Handler();
		}
		/* Seek to the beginning of the test file.  */
//		mmc_status = fx_file_seek(&fx_file, 0);
//		/* Check the file seek mmc_status.  */
//		if (mmc_status != FX_SUCCESS) {
//			/* Error performing file seek, call error handler.  */
//			Error_Handler();
//		}
//		/* Write a string to the test file.  */
//		mmc_status = fx_file_write(&fx_file, EMMC_buffer, sizeof(EMMC_buffer));
		mmc_status = fx_file_write(&fx_file, data, strlen(data));

		/* Check the file write mmc_status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Error writing to a file, call error handler.  */
			Error_Handler();
		}
		/* Close the test file.  */
		mmc_status = fx_file_close(&fx_file);
		/* Check the file close mmc_status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Error closing the file, call error handler.  */
			Error_Handler();
		}
}

void flush(void)
{
	mmc_status = fx_media_flush(&mmc_disk);
		/* Check the media flush  status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Error closing the file, call error handler.  */
			Error_Handler();
		}
}

void Open_N_read_file(char *ptr)
{
	ULONG bytes_read;
	/* Open the test file.  */
		mmc_status = fx_file_open(&mmc_disk, &fx_file, ptr, FX_OPEN_FOR_READ);
		/* Check the file open status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Error opening file, call error handler.  */
			Error_Handler();
		}
		/* Read the first 28 bytes of the test file.  */
		mmc_status = fx_file_read(&fx_file, read_buffer, 10, &bytes_read);
		/* Check the file read status.  */
		if ((mmc_status != FX_SUCCESS) || (bytes_read != 10)) {
			/* Error reading file, call error handler.  */
			Error_Handler();
		}
		/* Check if the read data is equal to the written data.  */
		if (memcmp(read_buffer, ptr, strlen(ptr)) != 0) {
			/* Error reading file, call error handler.  */
			Error_Handler();
		} else {
	//                                                               HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		}
		/* Close the test file.  */
		mmc_status = fx_file_close(&fx_file);
		/* Check the file close status.  */
		if (mmc_status != FX_SUCCESS) {
			/* Error closing the file, call error handler.  */
			Error_Handler();
		}

}
void close_media(void)
{
	/* Close the media.  */
			mmc_status = fx_media_close(&mmc_disk);
			/* Check the media close status.  */
			if (mmc_status != FX_SUCCESS) {
				/* Error closing the media, call error handler.  */
				Error_Handler();
			}
}
