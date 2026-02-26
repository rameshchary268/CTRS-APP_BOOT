/*
 * USB.c
 *
 *  Created on: Sep 11, 2025
 *      Author: VenkateshArikanti
 */

#include "MyMain.h"


UINT USB_DETECT()
{
	uint8_t PIN_STATUS = HAL_GPIO_ReadPin(GPIOA, GPIO_IDR_ID10);
	return PIN_STATUS;
}


void USB_VBUS(DRV_STATE state)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, state);
}

void USB_VBUS_DISPLAY(DRV_STATE state)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, state);
}
