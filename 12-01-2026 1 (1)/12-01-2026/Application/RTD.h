/*
 * RTD.h
 *
 *  Created on: Sep 8, 2025
 *      Author: RameshcharyVadla
 */

#ifndef RTD_H_
#define RTD_H_

#include <stdint.h>

#include <main.h>

/* Includes */

/* Declaration of constant defines and typedefs */
#define SPI_NSS_ACTIVE         (DRV_OFF)
#define SPI_NSS_INACTIVE       (DRV_ON)
#define SPI_INT_ACTIVE         (GPIO_PIN_RESET)
#define SPI_INT_INACTIVE       (GPIO_PIN_SET)

#define SPI_BUF_SIZE            16   /* Buffer size in number of bytes */
#define RTD_CMD_TX_BYTES        2
#define RTD_DAT_TX_BYTES        1
#define RTD_DAT_RX_BYTES        1
#define RTD_SPI_RX_BYTES        16

#define RTD_REF_RES             470     // Ohms
#define RTD_TYPE_2_4_WIRE       0x00
#define RTD_TYPE_3_WIRE         0x10

#define RTD_TYPE                RTD_TYPE_2_4_WIRE
#define MAX_RTD                 4

/* IC MAX31865 Register addresses */
#define MAX_RTD_REGISTERS       8     // Maximum number of registers available

#define MAX_RTD_DATA_REG        2

/* IC MAX31865 Register data */
// Automatic Conversion mode. Conversions occur continuously every 100ms (nominal). No conversions requested.
// Cold-junction temperature sensor enabled. Fault Detection Enabled: 40k? > RS > 5k?. Selects rejection of 50Hz and its harmonics
#define RTD_WR_CMD              0x80  // MSB bit (0:Read command, 1:Write command)

typedef enum {
  RTD_WAIT,
  RTD_BIST,
  RTD_INIT,
  RTD_INIT_CONT,
  RTD_INIT_DONE,
  RTD_EOC_CHK,
  RTD_EOC,
  RTD_EOC_DONE,
  RTD_ERR_READ,
  RTD_ERR_CHK,
  RTD_STOP_MODE,
  RTD_STOP,
  RTD_HALT,
  RTD_ERR,
} RTD_STATE;

typedef enum {
  RTD_1,
  RTD_2,
  RTD_3,
  RTD_4,
} RTD_ID;

typedef struct {
	uint8_t Short_detection_rtd1;
	uint8_t Short_detection_rtd2;
	uint8_t Open_detection_rtd1;
	uint8_t Open_detection_rtd2;
}RTD_ERRORS;
typedef struct {
 RTD_STATE state;
 uint32_t  poll_flag;
 uint32_t  dly_flag;
 uint32_t  lpf_flag;
 uint8_t   temp_data[MAX_RTD_REGISTERS];
 uint8_t   prev_data[MAX_RTD_DATA_REG];
 uint8_t   eoc_flag;
 uint8_t   fault_ctr;
 uint8_t   fault_status;
 GPIO_TypeDef *CsPort;
  uint16_t   CsPin;
  uint8_t InitSeq;
} RTD_INFO;

#define RTD_INFO_DEFAULT (RTD_INFO) {RTD_INIT, 0, 0, 0, {0, }, {0, }, 0, 0, }

/* Function Declarations */
void RTD_init(void);
void re_initialize_SPI3();
void RTD_proc(void);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);

#endif /* RTD_H_ */
