/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_host_msc.c
  * @author  MCD Application Team
  * @brief   USBX host applicative file
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
#include "ux_host_msc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_usbx_host.h"
#include "app_filex.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern FX_MEDIA *media;
extern TX_EVENT_FLAGS_GROUP ux_app_EventFlag;

extern HAL_MMC_CardInfoTypeDef card_info;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
UCHAR *buffer;
extern FX_MEDIA mmc_disk;
FX_FILE EMMC_file;
FX_FILE USB_file;

SD_USB_CIR_BUF usb_cir_buf;
SD_USB_INFO sd_usb_info;
FILE_BUF    File_buf[200];


extern ALIGN_32BYTES (uint32_t fx_mmc_media_memory[FX_STM32_MMC_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);
extern CHAR read_buffer[30];
uint8_t write_error=0;
extern UINT status;
ULONG bytes_read;
CHAR entry_name[50];
uint16_t file_number=0;

char path_1[] = "HMT/ajay/kumar/chaurasia.TXT";
CHAR file_name[FX_MAX_LONG_NAME_LEN];
UINT i = 0;
static UINT attributes, year, month, Day;
static UINT hour, minute, second;
static ULONG size;
/* USER CODE END 0 */


/* USER CODE BEGIN 1 */
//VOID msc_process_thread_entry(ULONG thread_input)
//{
//  ULONG storage_media_flag = 0;
//
//  while(1)
//  {
//    /* Wait until the requested flag STORAGE_MEDIA is received */
//    if (tx_event_flags_get(&ux_app_EventFlag, STORAGE_MEDIA, TX_OR_CLEAR,
//                           &storage_media_flag, TX_WAIT_FOREVER) != TX_SUCCESS)
//    {
//      Error_Handler();
//    }
//  }
//}



//Copy file from EMMC to host(pendrive)

UINT Copy_File_From_eMMC_To_Host_USB(CHAR *path ,CHAR *filename)
{

	   status = fx_directory_default_set(&mmc_disk, path);
	   if (status != FX_SUCCESS)
	   {
		   return status;
	   }

       status = fx_file_open(&mmc_disk, &EMMC_file, filename, FX_OPEN_FOR_READ);
       if (status != FX_SUCCESS)
       {
           return status;
       }
       sd_usb_info.file_size = EMMC_file.fx_file_current_file_size;
       ULONG64 free_space = fx_get_free_space(media);

       //check weather space is available in media or not
       if(free_space > sd_usb_info.file_size){

    // Try to delete the file if it already exists
		status = fx_file_delete(media, filename);
		if (status != FX_SUCCESS && status != FX_NOT_FOUND)
		{
			// If deletion fails for reasons other than "file not found", return error
			fx_file_close(&EMMC_file);
			return status;
		}
		// Step 4: Create and open file on USB
		status = fx_file_create(media, filename);
		if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
		{
			//fx_file_delete(media,file_name);
			fx_file_close(&EMMC_file);
			return status;
		}
		status = fx_file_open(media, &USB_file, filename, FX_OPEN_FOR_WRITE);
		if (status != FX_SUCCESS)
		{
			fx_file_close(&EMMC_file);
			return status;
		}
    // Step 5: Copy contents
		while (sd_usb_info.file_size > 0 && status == FX_SUCCESS)
		{
			sd_usb_info.buffer_size = (sd_usb_info.file_size < SD_USB_BUFFER_SIZE) ? sd_usb_info.file_size : SD_USB_BUFFER_SIZE;

			if (usb_cir_buf.state == BUF_EMPTY)
			{
				status = fx_file_read(&EMMC_file, usb_cir_buf.buffer, sd_usb_info.buffer_size, &bytes_read);
				if (status != FX_SUCCESS) break;
				usb_cir_buf.state = BUF_FULL;
			}
			if (usb_cir_buf.state == BUF_FULL)
			{
				status = fx_file_write(&USB_file, usb_cir_buf.buffer, bytes_read);
				if (status != FX_SUCCESS) break;
				usb_cir_buf.state = BUF_EMPTY;
				sd_usb_info.file_size -= bytes_read;
			}
		}

		// Step 6: Close files
		fx_file_close(&EMMC_file);
		fx_file_close(&USB_file);
		// Step 7: Reset directories
		fx_directory_default_set(&mmc_disk, "/");
		fx_directory_default_set(media, "/");
		fx_media_flush(&mmc_disk);
		fx_media_flush(media);
		return status;
}
}

VOID usb_sd(VOID)
{
	CHAR buf_usb[20];
	memset(buf_usb, 0, sizeof(buf_usb));
	status = fx_directory_information_get(&mmc_disk, "FONT",&attributes,&size, &year, &month, &Day,&hour, &minute, &second);
	if (status == FX_SUCCESS) {
		strcpy(buf_usb, "FONT");
		Scan_sd_fx(buf_usb);
	}
	status = fx_directory_information_get(&mmc_disk, "THMT",&attributes,&size, &year, &month, &Day,&hour, &minute, &second);
	if(status == FX_SUCCESS)
	{
		strcpy(buf_usb, "THMT");
		Scan_sd_fx(buf_usb);
	}
}


UINT Scan_sd_fx(CHAR *path)
{
    UINT more_files=1;
	status = fx_directory_default_set(&mmc_disk,path);
    if (status != FX_SUCCESS)
    {
        write_error = 1;
        return status;
    }
    status = fx_directory_information_get(media, path,&attributes,&size, &year, &month, &Day,&hour, &minute, &second);
   	if (status != FX_SUCCESS) {
   	    status=fx_directory_create(media,path);
   	    if(status != FX_SUCCESS){
   	    	return status;
   	    }
   	    status = fx_directory_default_set(media, path);
   	    if (status != FX_SUCCESS)
   	    {
   	    	return status;
   	    }
   	 }
    while (more_files)
    {
        i = 0;
        // Start from the beginning of the directory
        status = fx_directory_first_entry_find(&mmc_disk, file_name);
        if (status != FX_SUCCESS)
        {
            write_error = 1;
            break;
        }
        // Read up to 200 entries
        while (i < 5 && status == FX_SUCCESS)
        {
        	  if ((strcmp(file_name, ".") == 0) || (strcmp(file_name, "..") == 0))
        	    {
        	        status = fx_directory_next_entry_find(&mmc_disk, file_name);
        	        continue;
        	    }
            snprintf(File_buf[i].file_name, sizeof(File_buf[i].file_name), "%s", (const char *)file_name);
            file_number = i;
            i++;
            status = fx_directory_next_entry_find(&mmc_disk, file_name);
        }
        // Process File_buf[0..i-1] here if needed
		for (int i = 0; i <= file_number; i++) {
			Create_disp_File(File_buf[i].file_name,path);
		}
        if (i < 5 || status != FX_SUCCESS)
        {
            more_files = 0; // No more files to read
        }

        if(write_error == 0)
        {
			for (int i = 0; i <= file_number; i++)
			{
				memset(File_buf[i].file_name, 0, sizeof(File_buf[i].file_name));
				// memset(File_buf[i].sd_file_name, 0, sizeof(File_buf[i].sd_file_name));
			}
        }
    }
    // Clear local path
	status =  fx_directory_default_set(&mmc_disk,"\\");
	if(status != FX_SUCCESS)return status;
	status = fx_directory_delete(&mmc_disk,path);
	if(status != FX_SUCCESS)return status;
    status = fx_directory_default_set(media,"\\");
    if(status != FX_SUCCESS)return status;
    status = fx_media_flush(media);
    if(status != FX_SUCCESS)return status;
    status = fx_media_flush(&mmc_disk);
    if(status != FX_SUCCESS)return status;
    return status;
}

UINT Create_disp_File(char* name,char* path)
{
	// Step 1: Open file on eMMC for reading
	status = fx_file_open(&mmc_disk, &EMMC_file, name, FX_OPEN_FOR_READ);
	if (status != FX_SUCCESS)
	{
	    return status;
	}

	// Step 2: Check if file exists on USB
	status = fx_file_open(media, &USB_file, name, FX_OPEN_FOR_READ);
	if (status == FX_SUCCESS)
	{
	    // File exists — close and delete it
	    fx_file_close(&USB_file);  // Ignore close error for robustness
	    status = fx_file_delete(media, name);
	    if (status != FX_SUCCESS)
	    {
	        fx_file_close(&EMMC_file);
	        return status;
	    }
	}
	else if (status != FX_NOT_FOUND)
	{
	    // Unexpected error while checking file existence
	    fx_file_close(&EMMC_file);
	    return status;
	}

	// Step 3: Create new file on USB
	status = fx_file_create(media, name);
	if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
	{
	    fx_file_close(&EMMC_file);
	    return status;
	}

	// Step 4: Open USB file for writing
	status = fx_file_open(media, &USB_file, name, FX_OPEN_FOR_WRITE);
	if (status != FX_SUCCESS)
	{
	    fx_file_close(&EMMC_file);
	    return status;
	}

	// Step 5: Get file size from eMMC file
	sd_usb_info.file_size = EMMC_file.fx_file_current_file_size;

	while (sd_usb_info.file_size > 0 && status == FX_SUCCESS) {
		sd_usb_info.buffer_size =(sd_usb_info.file_size < SD_USB_BUFFER_SIZE) ?sd_usb_info.file_size : SD_USB_BUFFER_SIZE;

		if (usb_cir_buf.state == BUF_EMPTY) {
			status = fx_file_read(&EMMC_file, usb_cir_buf.buffer,
					sd_usb_info.buffer_size, &bytes_read);
			if (status != FX_SUCCESS)
				break;
			usb_cir_buf.state = BUF_FULL;
		}

		if (usb_cir_buf.state == BUF_FULL) {
			status = fx_file_write(&USB_file, usb_cir_buf.buffer, bytes_read);
			if (status != FX_SUCCESS)
				break;
			usb_cir_buf.state = BUF_EMPTY;
			sd_usb_info.file_size -= bytes_read;
		}
	}
	status = fx_file_close(&EMMC_file);
	status = fx_file_close(&USB_file);
	if (write_error == 0) {
		status = fx_file_delete(&mmc_disk, name);
	}
	return status;
}

ULONG64 fx_get_free_space(FX_MEDIA *media)
{
    return  (ULONG64)media->fx_media_available_clusters *
            (ULONG64)media->fx_media_sectors_per_cluster *
            (ULONG64)media->fx_media_bytes_per_sector;
}


//Copy file from USB_host(Pendrive) to EMMC

//UINT Copy_File_From_Host_USB_To_eMMC(const CHAR *path)
//{
//    CHAR temp_path[100];
//    CHAR folder_path[100] = "";
//    CHAR *token;
//    CHAR *file_name;
//
//    // Step 1: Parse the path and create folders on USB
//    strncpy(temp_path, path, sizeof(temp_path));
//    temp_path[sizeof(temp_path) - 1] = '\0';
//
//    token = strtok(temp_path, "/");
//    while (token != NULL)
//    {
//        CHAR *next = strtok(NULL, "/");
//        if (next == NULL)
//        {
//            file_name = token;  // Last token is the file name
//            break;
//        }
//        // Reset folder_path to current token only
//            strncpy(folder_path, token, sizeof(folder_path));
//            folder_path[sizeof(folder_path) - 1] = '\0';
//
//        // Check if folder exists on USB
//        status = fx_directory_information_get(media, folder_path,&attributes,&size, &year, &month, &Day,&hour, &minute, &second);
//        if (status != FX_SUCCESS)
//        {
//            // Create folder on USB
//            status = fx_directory_create(media, folder_path);
//            if (status != FX_SUCCESS)
//            {
//                return status;
//            }
//            tx_thread_sleep(10);
//        }
//        // Navigate into folder on USB
//        status = fx_directory_default_set(media, folder_path);
//        if (status != FX_SUCCESS)
//        {
//            return status;
//        }
//        token = next;
//    }
//    status = fx_file_open(media, &USB_file, file_name, FX_OPEN_FOR_READ);
//    if (status != FX_SUCCESS)
//    {
//          return status;
//    }
//
//    // Step 2: Navigate into folder on eMMC
//    strncpy(temp_path, path, sizeof(temp_path));
//    temp_path[sizeof(temp_path) - 1] = '\0';
//    token = strtok(temp_path, "/");
//    folder_path[0] = '\0';
//
//    while (token != NULL)
//    {
//        CHAR *next = strtok(NULL, "/");
//        if (next == NULL)
//        {
//            file_name = token;
//            break;
//        }
//        strncpy(folder_path, token, sizeof(folder_path));
//        folder_path[sizeof(folder_path) - 1] = '\0';
//
//		status = fx_directory_information_get(&mmc_disk, folder_path, &attributes,&size, &year, &month, &Day, &hour, &minute, &second);
//		if (status != FX_SUCCESS) {
//			// Create folder on USB
//			status = fx_directory_create(&mmc_disk, folder_path);
//			if (status != FX_SUCCESS) {
//				return status;
//			}
//		}
//		// Navigate into folder on USB
//		status = fx_directory_default_set(&mmc_disk, folder_path);
//		if (status != FX_SUCCESS) {
//			return status;
//		}
//		token = next;
//    }
//
//    // Try to delete the file if it already exists
//       status = fx_file_delete(&mmc_disk, file_name);
//       if (status != FX_SUCCESS && status != FX_NOT_FOUND)
//       {
//           // If deletion fails for reasons other than "file not found", return error
//           fx_file_close(&USB_file);
//           return status;
//       }
//       // Step 4: Create and open file on USB
//       status = fx_file_create(&mmc_disk, file_name);
//       if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
//       {
//       	//fx_file_delete(media,file_name);
//           fx_file_close(&USB_file);
//           return status;
//       }
//
//		// Step 3: Open file on eMMC
//		status = fx_file_open(&mmc_disk, &EMMC_file, file_name,  FX_OPEN_FOR_WRITE);
//		if (status != FX_SUCCESS)
//		{
//			return status;
//		}
//		sd_usb_info.file_size = USB_file.fx_file_current_file_size;
//		// Step 5: Copy contents
//		while (sd_usb_info.file_size > 0 && status == FX_SUCCESS)
//		{
//			sd_usb_info.buffer_size = (sd_usb_info.file_size < SD_USB_BUFFER_SIZE) ? sd_usb_info.file_size : SD_USB_BUFFER_SIZE;
//
//			if (usb_cir_buf.state == BUF_EMPTY)
//			{
//				status = fx_file_read(&USB_file, usb_cir_buf.buffer, sd_usb_info.buffer_size, &bytes_read);
//				if (status != FX_SUCCESS) break;
//				usb_cir_buf.state = BUF_FULL;
//			}
//			if (usb_cir_buf.state == BUF_FULL)
//			{
//				status = fx_file_write(&EMMC_file, usb_cir_buf.buffer, bytes_read);
//				if (status != FX_SUCCESS) break;
//				usb_cir_buf.state = BUF_EMPTY;
//				sd_usb_info.file_size -= bytes_read;
//			}
//		}
//		// Step 6: Close files
//		fx_file_close(&EMMC_file);
//		fx_file_close(&USB_file);
//		// Step 7: Reset directories
//		fx_directory_default_set(&mmc_disk, "\\");
//		fx_directory_default_set(media, "\\");
//		fx_media_flush(&mmc_disk);
//		fx_media_flush(media);
//		return status;
//}


/*VOID usb_sd(VOID)
{
	CHAR buf_usb[20];
	memset(buf_usb, 0, sizeof(buf_usb));
	status = fx_directory_information_get(&mmc_disk, "FONT",&attributes,&size, &year, &month, &day,&hour, &minute, &second);
	if (status == FX_SUCCESS) {
		strcpy(buf_usb, "FONT");
		Scan_sd_fx(buf_usb);
	}
	status = fx_directory_information_get(&mmc_disk, "THMT",&attributes,&size, &year, &month, &day,&hour, &minute, &second);
	if(status == FX_SUCCESS)
	{
		strcpy(buf_usb, "THMT");
		Scan_sd_fx(buf_usb);
	}
}


UINT Scan_sd_fx(CHAR *path)
{
    UINT more_files=1;
	status = fx_directory_default_set(&mmc_disk,path);
    if (status != FX_SUCCESS)
    {
        write_error = 1;
        return status;
    }
    status = fx_directory_information_get(media, path,&attributes,&size, &year, &month, &day,&hour, &minute, &second);
   	if (status != FX_SUCCESS) {
   	    status=fx_directory_create(media,path);
   	    if(status != FX_SUCCESS){
   	    	return status;
   	    }
   	    status = fx_directory_default_set(media, path);
   	    if (status != FX_SUCCESS)
   	    {
   	    	return status;
   	    }
   	 }
    while (more_files)
    {
        i = 0;
        // Start from the beginning of the directory
        status = fx_directory_first_entry_find(&mmc_disk, file_name);
        if (status != FX_SUCCESS)
        {
            write_error = 1;
            break;
        }
        // Read up to 200 entries
        while (i < 5 && status == FX_SUCCESS)
        {
        	  if ((strcmp(file_name, ".") == 0) || (strcmp(file_name, "..") == 0))
        	    {
        	        status = fx_directory_next_entry_find(&mmc_disk, file_name);
        	        continue;
        	    }
            snprintf(File_buf[i].file_name, sizeof(File_buf[i].file_name), "%s", (const char *)file_name);
            file_number = i;
            i++;
            status = fx_directory_next_entry_find(&mmc_disk, file_name);
        }
        // Process File_buf[0..i-1] here if needed
		for (int i = 0; i <= file_number; i++) {
			Create_disp_File(File_buf[i].file_name,path);
		}
        if (i < 5 || status != FX_SUCCESS)
        {
            more_files = 0; // No more files to read
        }

        if(write_error == 0)
        {
			for (int i = 0; i <= file_number; i++)
			{
				memset(File_buf[i].file_name, 0, sizeof(File_buf[i].file_name));
				// memset(File_buf[i].sd_file_name, 0, sizeof(File_buf[i].sd_file_name));
			}
        }
    }
    // Clear local path
	status =  fx_directory_default_set(&mmc_disk,"\\");
	if(status != FX_SUCCESS)return status;
	status = fx_directory_delete(&mmc_disk,path);
	if(status != FX_SUCCESS)return status;
    status = fx_directory_default_set(media,"\\");
    if(status != FX_SUCCESS)return status;
    status = fx_media_flush(media);
    if(status != FX_SUCCESS)return status;
    status = fx_media_flush(&mmc_disk);
    if(status != FX_SUCCESS)return status;
    return status;
}

UINT Create_disp_File(char* name,char* path)
{
	// Step 1: Open file on eMMC for reading
	status = fx_file_open(&mmc_disk, &SD_file, name, FX_OPEN_FOR_READ);
	if (status != FX_SUCCESS)
	{
	    return status;
	}

	// Step 2: Check if file exists on USB
	status = fx_file_open(media, &USB_file, name, FX_OPEN_FOR_READ);
	if (status == FX_SUCCESS)
	{
	    // File exists — close and delete it
	    fx_file_close(&USB_file);  // Ignore close error for robustness
	    status = fx_file_delete(media, name);
	    if (status != FX_SUCCESS)
	    {
	        fx_file_close(&SD_file);
	        return status;
	    }
	}
	else if (status != FX_NOT_FOUND)
	{
	    // Unexpected error while checking file existence
	    fx_file_close(&SD_file);
	    return status;
	}

	// Step 3: Create new file on USB
	status = fx_file_create(media, name);
	if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
	{
	    fx_file_close(&SD_file);
	    return status;
	}

	// Step 4: Open USB file for writing
	status = fx_file_open(media, &USB_file, name, FX_OPEN_FOR_WRITE);
	if (status != FX_SUCCESS)
	{
	    fx_file_close(&SD_file);
	    return status;
	}

	// Step 5: Get file size from eMMC file
	sd_usb_info.file_size = SD_file.fx_file_current_file_size;

	while (sd_usb_info.file_size > 0 && status == FX_SUCCESS) {
		sd_usb_info.buffer_size =(sd_usb_info.file_size < SD_USB_BUFFER_SIZE) ?sd_usb_info.file_size : SD_USB_BUFFER_SIZE;

		if (usb_cir_buf.state == BUF_EMPTY) {
			status = fx_file_read(&SD_file, usb_cir_buf.buffer,
					sd_usb_info.buffer_size, &bytes_read);
			if (status != FX_SUCCESS)
				break;
			usb_cir_buf.state = BUF_FULL;
		}

		if (usb_cir_buf.state == BUF_FULL) {
			status = fx_file_write(&USB_file, usb_cir_buf.buffer, bytes_read);
			if (status != FX_SUCCESS)
				break;
			usb_cir_buf.state = BUF_EMPTY;
			sd_usb_info.file_size -= bytes_read;
		}
	}
	status = fx_file_close(&SD_file);
	status = fx_file_close(&USB_file);
	if (write_error == 0) {
		status = fx_file_delete(&mmc_disk, name);
	}
	return status;
}*/
