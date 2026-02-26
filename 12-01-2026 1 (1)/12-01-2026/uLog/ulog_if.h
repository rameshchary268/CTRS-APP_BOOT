/*
 * ulog_if.h
 *
 *  Created on: Feb 14, 2023
 *      Author: Ilensys
 */

#ifndef ULOG_IF_H_
#define ULOG_IF_H_

#include "ulog.h"

#ifdef __cplusplus
extern "C" {
    #endif

void ulog_if_init(void);
void GetFileName(char *FName);
//UINT create_and_write_file(FX_MEDIA *media, UINT Year, CHAR *Month,CHAR *week, UINT Date, CHAR *filename, CHAR *data, uint16_t data_size);
UINT create_and_write_file(FX_MEDIA *media, UINT Year, CHAR *Month, CHAR *Week, UINT Date,
                            CHAR *filename, CHAR *data, uint16_t data_size);
UINT ensure_directory_exists(UINT Year, CHAR *Month, CHAR *Week);
//FRESULT Scan_sd (char* pat, char* filename);

#ifdef __cplusplus
}
#endif

#endif /* ULOG_IF_H_ */
