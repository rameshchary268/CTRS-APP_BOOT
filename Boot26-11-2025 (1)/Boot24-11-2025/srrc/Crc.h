/*
 * Crc.h
 *
 *  Created on: 04-Oct-2022
 *      Author: Ilensys
 */

#ifndef CRC_H_
#define CRC_H_

/* Includes */
#include "MyMain.h"

/* Declaration of constant defines and typedefs */

/* Declaration of enumerators and typedefs */

/* Function Declarations */
unsigned short crc16_ccitt(const void *buf, int len);
unsigned short crc16_ccitt_bin(const void *buf, int len, unsigned short start);

#endif /* CRC_H_ */
