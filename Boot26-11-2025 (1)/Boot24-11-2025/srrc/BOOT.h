/*
 * BOOT.h
 *
 *  Created on: Oct 11, 2025
 *      Author: RameshcharyVadla
 */

#ifndef BOOT_H_
#define BOOT_H_

#include <stdint.h>

#include "app_filex.h"
#include <stdint.h>

#include <main.h>


#define READ_SIZE           512
#define APPLICATION_START_ADDR		0x08080000
#define APP_END_ADDR        0x083FFFFF          // End address of application flash area
#define MEM_BANK_APP_SIZE        ((APP_END_ADDR - APPLICATION_START_ADDR) + 1)   // 0x80000 (768 KB)

#define FLASH_USER_START_ADDR  0x08080000    // Example start address
#define FLASH_USER_END_ADDR    0x083FFBFE
#define	MCU_IRQS	          70u	// no. of NVIC IRQ inputs

#define VERIFY_CHUNK_SIZE   512


#define BLOCK_SIZE      256
//#define Firmware_update_flag  0x00   // EEPROM address (example)
#define APPLICATION_FILE_NAME "CTRS_CFC.bin"

typedef enum {
    EMMC_INIT = 0,
    EMMC_FILE_OPEN,
    EMMC_FILE_VALIDATE,
    EMMC_ERASE,
    EMMC_WRITE,
    EMMC_VERIFY,
    EMMC_CLEANUP,
    EMMC_JUMP,
	EMMC_CHECK_FIRMWARE,
    EMMC_DONE,
    EMMC_ERROR,
} EMMC_State_t;
typedef struct {
	EMMC_State_t state;
    UINT status;
    ULONG bytes_read;
    ULONG file_size;
    ULONG total_written;
    uint32_t flash_write_addr;
} EMMC_Info_t;

 #define FX_MMC_VOLUME_NAME "STM32_MMC_DISK"

typedef struct {
 uint32_t  poll_flag;
} LED_INFO;
typedef struct {
 uint32_t  app_flag;
} BOOT_INFO;

typedef struct {
    uint8_t update_firmware;
} UpdateFlag_t;

#define LED_INFO_DEFAULT (LED_INFO) {0}

#define BOOT_INFO_DEFAULT (BOOT_INFO) {0}
void write_flash_to_emmc(void);
void Read_Flash_Data(uint32_t app_address,uint32_t size);


void read_flash_write_emmc() ;
void File_validation(void);
void File_validation_flash(void);
void cleanup_threadx_and_middlewares(void);
void my_func();
void disable_irqs_and_systick(void);
void safe_middleware_cleanup(void);
void goto_application1(uint32_t app_vector_addr);

void RunFlashCheck(void);

void boot_proc(void);
HAL_StatusTypeDef Erase_Flash_SingleBank(void);
HAL_StatusTypeDef Flash_Write(uint8_t *data, uint32_t length, uint32_t start_address);


#endif /* BOOT_H_ */
