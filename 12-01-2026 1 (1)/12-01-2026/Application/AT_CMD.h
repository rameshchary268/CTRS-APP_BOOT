/*
 * AT_CMD.h
 *
 *  Created on: Sep 10, 2025
 *      Author: SaikrishnaKarutu
 */

#ifndef AT_CMD_H_
#define AT_CMD_H_

/****************AT COMMANDS FOR WIFI NORA MODULE*********************/

#define AT_SCAN_STATIONS           "AT+UWSSC\r\n"

#define AT_ENTER_CMD_MODE            "AT+USYFR\r\n"
#define AT_CMD_RESET                "AT+CPWROFF\r\n"
#define AT_CMD_FIRMWARE_UPDATE      "AT+USYFWU=0,115200\r\n"
#define AT_SAVE_CMD                 "AT&W\r\n"


#define   AT_CMD_STA_USERID         "AT+UWSCP=0,\""
#define   AT_CMD_STA_PASSWD         "AT+UWSSW=0,\""
#define   AT_CMD_STA_IP_STAT        "AT+UWSIPD=0\r\n"
#define   AT_CMD_STA_ACTIVATE       "AT+UWSC=0\r\n"

#define   AT_CMD_CONNECTION_DETAILS "AT+UWSNST=0\r\n"
#define   AT_CMD_SIGNAL_STRENGTH    "AT+UWSST=4\r\n"
#define   AT_CMD_WIFI_DISCONNECT    "AT+UWSDC\r\n"

#define   AT_UPLOAD_RCA_CERT         "AT+USECUB=0,\"RCA.pem\"\x01\x04\xA4"
#define   AT_UPLOAD_DEVICE_CERT     "AT+USECUB=1,\"Device.pem\"\x01\x04\xC4"
#define   AT_UPLOAD_PRIVATE_KEY     "AT+USECUB=2,\"Private.pem\"\x01\x06\x8B"

#define   AT_VALIDATE_RCA_CERT       "AT+USECD=\"RCA.pem\"\r\n"
#define   AT_VALIDATE_DEVICE_CERT   "AT+USECD=\"Device.pem\"\r\n"

#define   AT_DELETE_CA_CERT       "AT+USECR=0,\"CA.pem\"\r\n"
#define   AT_DELETE_RCA_CERT       "AT+USECR=0,\"RCA.pem\"\r\n"
#define   AT_DELETE_DEVICE_CERT    "AT+USECR=1,\"Device.pem\"\r\n"
#define   AT_DELETE_PRIVATE_CERT   "AT+USECR=2,\"Private.pem\"\r\n"


#define   AT_CERT_CHECK             "AT+USECL?\r\n"
#define   AT_TLS_CONFIG             "AT+UMQTLS=0,1,\"RCA.pem\",\"Device.pem\",\"Private.pem\"\r\n"
#define   AT_KEEP_ALIVE             "AT+UMQKA=0,120\r\n"
#define   AT_TLS_CONNECTION         "AT+UMQCP=0,\""
//#define   AT_TLS_CONNECTION       "AT+UMQCP=0,\"a1ymfma3vzmew7-ats.iot.ap-south-1.amazonaws.com\",8883,\"c356e63d-a11f-410e-860b-a04bc3e78f81\"\r\n"
#define   AT_MQTT_CONNECT     		"AT+UMQC=0\r\n"
#define   AT_MQTT_PUBLISH           "AT+UMQPB=0,0,0,\"deviceupdate/"
//#define   AT_MQTT_PUBLISH         "AT+UMQPB=0,0,0,\"deviceupdate/c356e63d-a11f-410e-860b-a04bc3e78f81\"\x01\x01\x38\{\"Topic\":\"requestnewfirmware\",\"RequestId\":\"1000\",\"DeviceSlNo\":\"c356e63d-a11f-410e-860b-a04bc3e78f81\",\"RequestedFileNumber\":\"133\",\"Data\":\"\",\"CurrentVersion\":\"v1.27\",\"NewVersion\":\"T1.111\",\"Type\":\"Firmware\",\"UserId\":\"ajaykumar636363@gmail.com\",\"UserToken\":\"2b7d28f8-a402-4449-acaf-692a2a211b69\",\"AppVersion\":\"V1.2\"}\r\n"
//#define   AT_MQTT_PUBLISH         "AT+UMQPB=0,0,0,\"deviceupdate/c356e63d-a11f-410e-860b-a04bc3e78f81\"\x01\x01\x38\{\"Topic\":\"requestnewfirmware\",\"RequestId\":\"1000\",\"DeviceSlNo\":\"c356e63d-a11f-410e-860b-a04bc3e78f81\",\"RequestedFileNumber\":\"133\",\"Data\":\"\",\"CurrentVersion\":\"v1.27\",\"NewVersion\":\"T1.111\",\"Type\":\"Firmware\",\"UserId\":\"ajaykumar636363@gmail.com\",\"UserToken\":\"2b7d28f8-a402-4449-acaf-692a2a211b69\",\"AppVersion\":\"V1.2\"}\r\n"
//#define   AT_MQTT_SUBSCRIBE         "AT+UMQS=0,0,\""
#define   AT_MQTT_SUBSCRIBE         "AT+UMQS=0,0,\"c356e63d-a11f-410e-860b-a04bc3e78f81/ack\"\r\n"
#define   AT_READ_MESSAGE			"AT+UMQRB=0\r\n"
#define   AT_MQTT_DISCONNECT        "AT+UMQDC=0\r\n"

/****************** AT COMMANDS FOR BLUETOOTH NORA MODULE***************/

#define   AT_CMD_BLE_MODE           "AT+UBTM?\r\n"
#define   AT_CMD_BLE_SET_MODE       "AT+UBTM=3\r\n"
#define   AT_CMD_SAVE		    "AT&W\r\n"
#define   AT_CMD_CHECK_NAME         "AT+UBTLN?\r\n"
#define   AT_CMD_BLE_SCAN	    "AT+UBTD=0,0,2000\r\n"
#define   AT_CMD_BLE_CONNECT        "AT+UBTC=0080E1225F74p\r\n"
#define   AT_CMD_BLE_CONNECTED      "AT+UBTGPSD=0\r\n"
#define   AT_CMD_CHAR_DISCOVER      "AT+UBTGSCD=0,12,14\r\n"
#define   AT_CMD_READ_DATA          "AT+UBTGR=0,17\r\n"
#define   AT_CMD_BLE_CONN_DETAILS   "AT+UBTCL\r\n"




#endif /* AT_CMD_H_ */
