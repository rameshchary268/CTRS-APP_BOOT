/*
 * EEPROM.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef EEPROM_H_
#define EEPROM_H_

/* Includes */
#include "MyMain.h"
#include "stm32u5xx_hal_i2c.h"

#define EEP_PAGE_SIZE    256

#define EEPROM_SCHEDULED_EN_ADDRESS 			0//8
#define EEPROM_SCHEDULED_EN_ADDRESS_CRC         18//8
#define SCHEDULED_FILL_ADDRESS				 	22//12
#define SCHEDULED_FILL_ADDRESS_CRC              44//24
#define SCHEDULED_RLEVEL_ADDRESS 				48//12
#define SCHEDULED_RLEVEL_ADDRESS_CRC            70//4
#define SCHEDULED_MAINT_ADDRESS 				74//44//32
#define SCHEDULED_MAINT_ADDRESS_CRC             100//60

#define MODBUS_ADDRESS 							104//18
#define MODBUS_ADDRESS_CRC                      132			//changed from 128 to 132

#define ALARM_ADDRESS 							136//14
#define ALARM_ADDRESS_CRC                       166//4

#define DISPLAY_ADDRESS 						170//12
#define DISPLAY_ADDRESS_CRC                     192//116
//#define LID_ADDRESS 							196//120//92
//#define LID_ADDRESS_CRC                         214//128
//#define SCHEDULED_LN2SUPPLY_ADDRESS 			218//12
//#define SCHEDULED_LN2SUPPLY_ADDRESS_CRC         240//152

#define High_ln2_usage                          244
//#define ln2_usage                               248
//#define pre_mean_usage                          252

#define TEMP_RTD_ADDRESS			 			256//34
#define TEMP_RTD_ADDRESS_CRC                    300//2
#define LEVEL_ADDRESS			 			304//200//160
#define LEVEL_ADDRESS_CRC                    352//250
#define RTC_ADDRESS					 			356//6
#define RTC_ADDRESS_CRC                         372//4
#define FREEZER_ID_ADDRESS					 	376//20
#define FREEZER_ID_ADDRESS_CRC                  396//2
#define INIT_FACTORY_RESET					 	400//284
#define DP_CALIB_ADDRESS						512  //16 bytes
#define RTD1_CALIB_ADDRESS						528
#define RTD2_CALIB_ADDRESS						544
#define LN2_CHK_DATE                            584
#define SCH_DATE                                585
#define SUPPLY_DATE                             586
#define REM_CRC                                 588

#define admin0                                  592
#define admin0_crc                              600
#define emp0                                    602
#define emp0_crc0                               612
#define admin1                                  616
#define admin1_crc                              624
#define emp1                                    626
#define emp1_crc1                               636
#define admin1_sts                              638
#define admin2                                  640
#define admin2_crc                              648
#define emp2                                    650
#define emp2_crc2                               660
#define admin2_sts                              662
#define user0                                   664
#define user0_crc                               672
#define emp3                                    674
#define emp3_crc3                               684
#define user1                                   688
#define user1_crc                               696
#define emp4                                    698
#define emp4_crc4                               708
#define user2                                   712
#define user2_crc                               720
#define emp5                                    722
#define emp5_crc5                               730
#define user3                                   734
#define user3_crc                               742
#define emp6                                    744
#define emp6_crc6                               752
#define user4                                   756
#define user4_crc                               764
#define emp7                                    770
#define emp7_crc7                               778
#define user5                                   782
#define user5_crc                               790
#define emp8                                    792
#define emp8_crc8                               800
#define user6                                   804
#define user6_crc                               812
#define emp9                                    814
#define emp9_crc9                               822
#define user7                                   826
#define user7_crc                               834
#define emp10                                   836
#define emp10_crc10                             844

//passwords
#define emp0_pw                                 854
#define emp0_pw_crc                             862
#define emp1_pw                                 866
#define emp1_pw_crc                             874
#define emp2_pw                                 878
#define emp2_pw_crc                             886
#define emp3_pw                                 890
#define emp3_pw_crc                             898
#define emp4_pw                                 902
#define emp4_pw_crc                             910
#define emp5_pw                                 914
#define emp5_pw_crc                             922
#define emp6_pw                                 926
#define emp6_pw_crc                             934
#define emp7_pw                                 938
#define emp7_pw_crc                             946
#define emp8_pw                                 950
#define emp8_pw_crc                             958
#define emp9_pw                                 962
#define emp9_pw_crc                             970
#define emp10_pw                                974
#define emp10_pw_crc                            982

#define eactive_bank                            986
#define modbus_com                              990     //1 byte
#define Firmware_update_flag                    992     //3 bytes

#define eflash_bank1_crc16                      996    // 2 bytes - Should be written manually via external EEPROM programmer
#define eflash_bank2_crc16                      998    // 2 bytes - Should be written manually via external EEPROM programmer
#define NW_ID_ADDRESS                           1010
#define NW_ID_ADDRESS_CRC                       1020

#define FREZER_MODEL                            1040    //2 BYTES
#define PLUMBING_OPTION                         1042    //2BYTES
#define DEV_SER_NUM_ADDR                        1044    //20BYTES

#define ETH_MAC_ID_ADDR                         1064    // MAC address - 20 bytes

#define GROUP_ADDRESS                           1090  //184
#define GROUP_ADDRESS1                          1281  //1525-244

#define CONTACT_ADDRESS                         1537  //40

#define wifi_user_add                           1625
#define wifi_user_add_crc                       1677
#define wifi_pw_add                             1680
#define wifi_pw_add_crc                         1734
#define wifi_connect_sts                        1738
//#define Aws_cred_wr_sts                         1791
#define IOT_SER_NUM_ADDR                        1740    //36bytes
#define Aws_url_address                         1792//192
#define Aws_user_id                             1984
#define Sub_cli_address                         2048//20 1945
#define Token_address                           2068//36
#define Aws_missed_acks_add                     2104 //8

#define Purge_valve                             2112 //4
#define IOT_FIRM_DOW                            2120//2
#define FIRM_4_20_DOW                           2122


#define Wifi_ssid_add                           2140//160
#define Wifi_ssid_add_crc                       2301//2
#define wifi_pass_add                           2304//160
#define wifi_pass_add_crc                       2466//2

#define Network_4_20_mA                         2470//20
#define Network_4_20_mA_crc                     2490//2
#define Network_4_20_mA_config                  2494//30
#define Network_4_20_mA_config_crc              2524//2


#define load_CALIB_ADDRESS                      2560//28
#define SENSOR_SETTINGS                         2600//30
#define SENSOR_SETTINGS_CRC                     2630//2
#define ctrs_load_usage                         2634//4
//#define Aws_url_address_crc                     2149
//#define wifi_edm_sts_address                    1795//1996


//#define RCA_certificate_size                    4094//3kB
//#define RCA_certificate                         4096//3kB
//#define Device_certificate_size                 7166//3KB
//#define Device_certificate                      7168//3KB
//#define private_certificate_size                10238//3KB
//#define private_certificate                     10240//3KB


#define EEPROM_I2C &hi2c1

// EEPROM ADDRESS (8bits)
#define EEPROM_ADDR 0xA8

// Define the Page Size and number of pages
#define PAGE_SIZE1 256     // in Bytes
#define PAGE_NUM 1024  // number of pages


/* Declaration of constant defines and typedefs */
// Test parameters
#define EEPROM_INIT_CHR         0xFF
#define EEPROM_TEST_CHR         0x55
#define I2C_SEG_SPACE_ADDR_SIZE 2                                          // Address size within a segment
#define I2C_TXN_PAGE_SIZE       256                                        // Maximum of 256 bytes per page
#define I2C_TXN_PKT_SIZE        256                                        // Maximum of I2C_TXN_PAGE_SIZE bytes per transaction
#define I2C_TXN_PER_PAGE        (I2C_TXN_PAGE_SIZE/I2C_TXN_PKT_SIZE)       // Number of packets per page


// Process parameters are stored in EEPROM. Default values are factory set and user configurable
// 64 KBytes per segment
#define EEPROM_SEG1_ADDR      0xA8  // b0->0-Write, 1:Read; b1->A16; b2->A17; b3->1(as pin E2 is high) [0x00000 - 0x0ffff]
#define EEPROM_SEG2_ADDR      0xAA  // b0->0-Write, 1:Read; b1->A16; b2->A17; b3->1(as pin E2 is high) [0x10000 - 0x1ffff]
#define EEPROM_SEG3_ADDR      0xAC  // b0->0-Write, 1:Read; b1->A16; b2->A17; b3->1(as pin E2 is high) [0x20000 - 0x2ffff]
#define EEPROM_SEG4_ADDR      0xAE  // b0->0-Write, 1:Read; b1->A16; b2->A17; b3->1(as pin E2 is high) [0x30000 - 0x3ffff]

#define EEPROM_STRT_ADDRESS   0x000000
#define EEPROM_END_ADDRESS    0x03FFFF
#define EEPROM_MEM_SIZE       ((EEPROM_END_ADDRESS-EEPROM_STRT_ADDRESS)+1) // 256 KBytes total size

#define EEPROM_MAX_STR_SIZE   256                                        // 256 bytes gap between structures
#define EEPROM_ADDR_UC        (EEPROM_STRT_ADDRESS)                      // EEPROM address for user credentials info (max. 10 users)
#define EEPROM_ADDR_PARAM     (EEPROM_ADDR_UC + EEPROM_MAX_STR_SIZE)     // EEPROM address for process parameters info
#define EEPROM_ADDR_CALIB     (EEPROM_ADDR_PARAM + EEPROM_MAX_STR_SIZE)  // EEPROM address for most recent calibration data
#define EEPROM_ADDR_COMM      (EEPROM_ADDR_CALIB + EEPROM_MAX_STR_SIZE)  // EEPROM address for communication info

#define EEPROM_STR_ARRAY      {EEPROM_ADDR_UC, EEPROM_ADDR_PARAM, EEPROM_ADDR_CALIB, EEPROM_ADDR_COMM}

// Customer Support related parameters
#define PH_NUM_SIZE            24
#define CUST_SUPPORT_PH1       "+01 999 99999 99999"
#define CUST_SUPPORT_PH2       "+01 999 99999 99999"

/* Declaration of enumerators and typedefs */


/* Re Defined by Ajay */
#define Firmware_flag_address                   1022
#define Firmware_update_address                 1025    //20 spaces



typedef struct {
  uint8_t addr[2];                 // First two bytes of EEPROM address-MSB first
  uint8_t buf[I2C_TXN_PAGE_SIZE];  // Data buffer size is 256 bytes
} EEPROM_TXN;


#define EEPROM_INFO_DEFAULT (EEPROM_INFO) {EEPROM_POWERON, 0, 0, 0, 0, NULL, 0, 0, 0, }

typedef enum {
  EEPROM_READ,
  EEPROM_WRITE,
  EEPROM_ERASE,
} EEPROM_REQ;

/* Function Declarations */
void    EEPROM_init(void);

void    I2C1_TxCallback(I2C_HandleTypeDef *hi2c);
void    I2C1_RxCallback(I2C_HandleTypeDef *hi2c);
void    I2C1_ErrCallback(I2C_HandleTypeDef *hi2c);

void    EEPROM_Write (uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
void    EEPROM_Read (uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);

void EEPROM_Read_add (uint16_t MemAddress, uint16_t offset, uint8_t *data, uint16_t size);
void EEPROM_Write_add (uint16_t MemAddress, uint16_t offset, uint8_t *data, uint16_t size);


#endif /* EEPROM_H_ */
