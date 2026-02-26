/*
 * UserAccess.h
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */

#ifndef USERACCESS_H_
#define USERACCESS_H_


#include <WIFI.h>

#define MAX_UID_SIZE      32
#define MAX_PWD_SIZE      32
#define MAX_WIFI_CFG_ID   5
#define UUID_LENGTH       16
#define MAX_PSWD_LENGTH   12
#define  MAX_WIFI_CH      (MAX_WIFI_CFG_ID)
#define CLIENT_ID_LENGTH  30

typedef struct {
  uint8_t   iot_url[128];      // Unique URL for the Modbus server
  uint8_t 	iot_email[64];
  uint8_t 	iot_phone[20];
  uint8_t 	User_ID[64];
  uint8_t   ip_addr[4];                   // Server's (destination) IP address - Static IP address or DNS assigned
  uint16_t  port;                         // Server's (destination) port address
  uint16_t  File_number;
  uint16_t  Total_files;


} IOT_CONFIG;

typedef struct {
  uint8_t  client[CLIENT_ID_LENGTH];
  uint8_t  uuid[MAX_UID_SIZE] ;
  uint8_t  pswd[MAX_PWD_SIZE];            // Password
  uint8_t  Ssid[MAX_WIFI_CFG_ID][MAX_UID_SIZE] ;
  uint8_t  Sspswd[MAX_WIFI_CFG_ID][MAX_PWD_SIZE];
  uint8_t  auth_type[MAX_WIFI_CFG_ID][MAX_WIFI_CFG_ID];       // 1:Open, 2:WPA/WPA2/WPA3,  3:LEAP, 4:PEAP, 5:EAP-TLS, 6:WPA2/WPA3 (WPA disabled), 7:WPA3 (WPA/WPA2 disabled)
  uint8_t  active_id;
} WIFI_CFG;

typedef struct {
  WIFI_CFG    wifi;
  IOT_CONFIG  iot;
} SY_PAR;

typedef struct {
  SY_PAR   sy_par;    // System parameters
} COMM_PAR;           // TOTAL: 528 bytes

#define SSid_Warning "Keypad characters are allowed, with the exception of (, and \")!!!"
void listall(void);
void remove_tick(void);
void SaveAddUserSettings(AddUserStruct *AddUserSettings);
void LoadAddUserSettings(AddUserStruct *AddUserSettings);
void SavePasswordSettings(login_credentials *UIloginData);
void SaveWifiUserSettings(void);
void LoadWifiUserSettings(void);
void wifi_data_store(void);
void DelWifiCred();
void ClearUIWifiCred();


void deleteUser(User_access   *UserSetings);
void listall(void);
void listselection(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AddUserHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void EmployeIDHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void LoginHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void LoginpwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void conformpwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void NewpwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void Logoutfunction(void);
void Loginfunction(void);
//wifi
void IOT_WIFI_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void WIFI_PASS_EN_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void WifipwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void WifiUserHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void NW_id_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AWSUrlHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AWS_UsernameidHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AWS_PasswordHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AWS_EmailHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AWS_TokenHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void AWS_PhoneHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size);
void HandleAWSFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void HandleSYSTEMSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId);
void SaveAWSuserSettings(void);
void LoadAWSUserSettings(void);
void ClearAwscredentials();
void DelAwscredentials();
int isValidInput(const char *str);
int isValidInputpassword(const char *str);
#endif /* USERACCESS_H_ */
