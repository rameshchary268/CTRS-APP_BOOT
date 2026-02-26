/*
 * USB.h
 *
 *  Created on: Sep 11, 2025
 *      Author: VenkateshArikanti
 */

#ifndef USB_H_
#define USB_H_


#include "MyMain.h"


typedef enum {
  USB_MSC,
  USB_CDC,
  USB_HOST,
} USB_MODE;

typedef enum {
  USB_INIT,
  USB_DE_INIT,
  USB_MSC_START,
  USB_MSC_STOP,
  USB_CDC_DONE,
  USB_HOST_DONE,
  USB_ERR,
  USB_HALT,
} USB_STATE;

typedef struct {
 USB_STATE   state;
 uint8_t     mode;
 uint8_t     new_mode;
 uint8_t     host_mode;
 uint32_t    data_rdy;
 uint32_t    poll_flag;
} USB_INFO;

UINT USB_DETECT();
void USB_VBUS(DRV_STATE state);
void USB_VBUS_DISPLAY(DRV_STATE state);
#endif /* USB_H_ */
