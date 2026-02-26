/*
 * Xmodem.c
 *
 *  Created on: Oct 6, 2025
 *      Author: SaikrishnaKarutu
 */

#include "main.h"
//#include "usart.h"   // UART HAL
#include <string.h>
#include "Xmodem.h"

/* Global variables */
static uint8_t xmodem_packet_number = 1u;
static uint8_t x_first_packet_received = false;

uint8_t X_modem_Buffer[133] = {0};
uint32_t received_length = 0;
uint32_t timeout_receive = 1, timeout_xmodem_rec = 0;
uint8_t x_modem_error_detect = 0, xmodem_packet_success = 0, begin_wifi_update = 0;

extern uint8_t start_xmodem_mode, FirmwareUpdateModeOn;

extern UART_HandleTypeDef huart3;  // use UART3 for WiFi module

/* Local Functions */
int calulatecrc(char *ptr, int count);
xmodem_status xmodem_handle_packet(uint8_t header);
xmodem_status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number);

/**
 * @brief   Xmodem receiver over UART (instead of USB).
 */
void xmodem_receive(void)
{
    volatile xmodem_status status = X_OK;
    uint8_t error_number = 0u;
    x_first_packet_received = false;

    /* Spam host with ASCII "C" every 3 sec to request CRC mode */
    if ((FirmwareUpdateModeOn == 1) && (timeout_receive) && (start_xmodem_mode))
    {
        uint8_t reqC = 'C';
        HAL_UART_Transmit_IT(&huart3, &reqC, 1);
       // Set_delay(SW_TMR_XMODEM_TIMEOUT, 3000, &timeout_receive);
    }

    if (((uint32_t)received_length) > 128)
    {
        if (X_OK == status)
        {
            uint8_t header = X_modem_Buffer[0];

            if ((true == x_first_packet_received))
            {
                status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
            }

            xmodem_status packet_status = X_ERROR;
            switch (header)
            {
                case X_SOH:
                case X_STX:
                    packet_status = xmodem_handle_packet(header);
                    if (X_OK == packet_status)
                    {
                        xmodem_packet_success = 1;
                        // Here your raw 128 bytes data is ready in X_modem_Buffer[3..130]
                        // You can forward it for flashing / writing to memory
                        // e.g.: memcpy(app_buffer, &X_modem_Buffer[3], 128);

                        uint8_t ack = X_ACK;
                        HAL_UART_Transmit_IT(&huart3, &ack, 1);
                        memset(X_modem_Buffer, 0x00, sizeof(X_modem_Buffer));
                        received_length = 0;
                    }
                    else
                    {
                        status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
                    }
                    break;

                case X_EOT:
                    {
                        uint8_t ack = X_ACK;
                        HAL_UART_Transmit_IT(&huart3, &ack, 1);
                    }
                    break;

                case X_CAN:
                    status = X_ERROR;
                    break;

                default:
                    status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
                    break;
            }
        }
    }
    else
    {
        if (received_length == 1)
        {
            received_length = 0;
            if (X_modem_Buffer[0] == X_EOT)
            {
                uint8_t ack = X_ACK;
                HAL_UART_Transmit_IT(&huart3, &ack, 1);
                begin_wifi_update = 1;
            }
        }
    }
}

/**
 * @brief CRC-16 calculation
 */
//int calulatecrc(char *ptr, int count)
//{
//    int crc = 0;
//    char i;
//    while (--count >= 0)
//    {
//        crc = crc ^ ((int)*ptr++ << 8);
//        i = 8;
//        do
//        {
//            if (crc & 0x8000)
//                crc = (crc << 1) ^ 0x1021;
//            else
//                crc = crc << 1;
//        } while (--i);
//    }
//    return (crc);
//}

/**
 * @brief Handle Xmodem packet
 */
xmodem_status xmodem_handle_packet(uint8_t header)
{
    xmodem_status status = X_OK;
    uint16_t size = 0u;

    uint8_t received_packet_number[X_PACKET_NUMBER_SIZE];
    uint8_t received_packet_data[X_PACKET_1024_SIZE] = {0,};
    uint8_t received_packet_crc[X_PACKET_CRC_SIZE];

    if (X_SOH == header)
    {
        size = X_PACKET_128_SIZE;
    }
    else if (X_STX == header)
    {
        size = X_PACKET_1024_SIZE;
    }
    else
    {
        status |= X_ERROR;
    }

    memcpy(received_packet_number, &X_modem_Buffer[1], X_PACKET_NUMBER_SIZE);
    memcpy(received_packet_data, &X_modem_Buffer[3], size);
    memcpy(received_packet_crc, &X_modem_Buffer[131], X_PACKET_CRC_SIZE);

    uint16_t crc_received = ((uint16_t)received_packet_crc[X_PACKET_CRC_HIGH_INDEX] << 8u) |
                            ((uint16_t)received_packet_crc[X_PACKET_CRC_LOW_INDEX]);
    uint16_t crc_calculated = calulatecrc((char*)&X_modem_Buffer[3], 128);

    if ((X_OK == status) && (false == x_first_packet_received))
    {
        x_first_packet_received = true;
    }

    if (X_OK == status)
    {
        if (xmodem_packet_number != received_packet_number[0u])
        {
            status |= X_ERROR_NUMBER;
        }
        if (255u != (received_packet_number[X_PACKET_NUMBER_INDEX] +
                     received_packet_number[X_PACKET_NUMBER_COMPLEMENT_INDEX]))
        {
            status |= X_ERROR_NUMBER;
        }
        if (crc_calculated != crc_received)
        {
            status |= X_ERROR_CRC;
        }
    }

    if (X_OK == status)
    {
        xmodem_packet_number++;
    }

    return status;
}

/**
 * @brief Xmodem error handler (sends NAK on error)
 */
xmodem_status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number)
{
    xmodem_status status = X_OK;
    (*error_number)++;
    if ((*error_number) >= max_error_number)
    {
        uint8_t ack = X_ACK;
        HAL_UART_Transmit_IT(&huart3, &ack, 1);
        status = X_ERROR;
    }
    else
    {
        status = X_OK;
        x_modem_error_detect = 1;
        uint8_t nak = X_NAK;
        HAL_UART_Transmit_IT(&huart3, &nak, 1);
        memset(X_modem_Buffer, 0x00, sizeof(X_modem_Buffer));
        received_length = 0;
    }
    return status;
}

