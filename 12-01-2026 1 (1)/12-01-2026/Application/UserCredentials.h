/*
 * UserCredentials.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */
#ifndef USERCREDENTIALS_H_
#define USERCREDENTIALS_H_

/* Includes */
#include "MyMain.h"

/* Declaration of constant defines */
#define MIN_ID_SIZE          4
#define MIN_PSWD_SIZE        4
#define MAX_ID_SIZE          9
#define MAX_PSWD_SIZE        9
#define MAX_USER_ACCOUNTS    8
#define MAX_ADMIN_ACCOUNTS   2

#define MAX_LOGIN_FAILURES   3

// Factory set Access credential for admin
#define DEF_LOGIN_USER_ID      "@Ph2"       // Default User-ID for user-1    to gain access to security setting menus
#define DEF_LOGIN_PASSCODE     "1234"       // Default pass-code for user-1  to gain access to security setting menus

/* Typedefs and enums */
typedef enum {
  UC_SEND,
  UC_SEND_DONE,
  UC_RECV,
  UC_RECV_DONE,
  UC_DONE,
  UC_ERR,
} UC_STATE;

typedef enum {
  LOGGED_OUT,
  LOGGED_IN,
  LOG_DISABLED,
  LOGGING_ERR,
} LOG_STS;

typedef struct {
  UC_STATE state;
  struct   {
    RTC_TimeTypeDef  time;
    RTC_DateTypeDef  date;
  } login[MAX_USER_ACCOUNTS+MAX_ADMIN_ACCOUNTS+1];
  struct   {
    RTC_TimeTypeDef  time;
    RTC_DateTypeDef  date;
  } logout[MAX_USER_ACCOUNTS+MAX_ADMIN_ACCOUNTS+1];
  uint8_t  active_user;   // 0: Factory Default account, 1-10: Registered Users
  LOG_STS  log_sts;
} UC_INFO;

#define UC_INFO_DEFAULT (UC_INFO) {UC_DONE, }

/* Function Declarations */

#endif /* USERCREDENTIALS_H_ */
