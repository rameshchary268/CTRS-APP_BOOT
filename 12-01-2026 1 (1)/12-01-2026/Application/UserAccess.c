/*
 * UserAccess.c
 *
 *  Created on: Oct 14, 2025
 *      Author: VenkateshArikanti
 */
#include "MyMain.h"
#include "DispVPAddrList.h"
#include "DispUIkeyhandler.h"
#include "DisplayCmdHandlers.h"
#include "DisplayTxHandlers.h"
#include "UserAccess.h"
#include "UISettings.h"
#include "FactoryReset.h"
#include "ctype.h"


const uint8_t clearPwd[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t NW_ID[8];
char WifiFileName[20]="0:/NINA_W13X_SW.bin";
uint8_t dummy_star[35]={"*********************************"};
//uint8_t attempt;
Login_access login_access;
extern IOT_INFO    iot_info;
COMM_PAR   comm_par;
uint16_t crc16;
/*
 * *****************************************************************************
 * Function name: NW_id_Handler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to create User ID string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void NW_id_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
/*	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;

	memset(NW_ID, 0, sizeof(NW_ID));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				NW_ID[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				SendSTRDataToDisplay(NW_ID_RD,(uint8_t*)"\0");
				memcpy((uint8_t*)&NW_ID, (uint8_t*)&UIModbusData.NwID, sizeof(NW_ID));
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(NW_ID_WR,(uint8_t*)&NW_ID);
		SendSTRDataToDisplay(NW_ID_RD,(uint8_t*)"\0");
	}*/
}

void IOT_WIFI_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	if(VPAddr == WIFI_CONNECT_RD)
	{
		wifi_info.UART1ErrorDetected=1;
		UIwifi.connect_sts= *(DataInPtr) ;
		if (UIwifi.connect_sts==1)
		{
			//SendN16DataToDisplay(WIFI_CONNECT_WR, 1);
			link_disconnect=1;
			System_Status[EVENT_FLAG] |= EVT_WIFI_CONNECT_MANUAL;
		}
		else
		{
			link_disconnect=2;
			System_Status[EVENT_FLAG] |= EVT_WIFI_DIS_CONNECT_MANUAL;
		}

		wifi_info.state=WIFI_CONN_MNGMNT;
		EEPROM_Write(wifi_connect_sts, 0,(uint8_t*)&UIwifi.connect_sts,1);
	}
	if(VPAddr == IOT_CONNECT_RD && (Plumbing_options.IOT_serial[0] != 0x00) && (Plumbing_options.IOT_serial[0] != 0xFF))
	{
		UIwifi.IOT_connect_sts= *(DataInPtr) ;
		wifi_info.UART1ErrorDetected=1;
		if(UIwifi.IOT_connect_sts)
		{
			peer_disconnect=2;
			System_Status[EVENT_FLAG] |= EVT_IOT_CONNECT_MANUAL;
		}
		else
		{
			peer_disconnect=2;
			System_Status[EVENT_FLAG] |= EVT_IOT_DIS_CONNECT_MANUAL;
		}
		wifi_info.state=WIFI_CONN_MNGMNT;
		EEPROM_Write(wifi_connect_sts, 0,(uint8_t*)&UIwifi.connect_sts,2);
		System_Status[WARNING_FLAG] &= ~WRN_EMAIL_FAIL;
		System_Status[WARNING_FLAG] &= ~WRN_EMAIL_VALID;
	}
	else
	{
		UIwifi.IOT_connect_sts=0;
		SendN16DataToDisplay(IOT_CONNECT_WR, UIwifi.IOT_connect_sts);
		SendN16DataToDisplay(IOT_CONNECT_RD, UIwifi.IOT_connect_sts);
	}
	wifi_info.UART1ErrorDetected = 1;
}

void WIFI_PASS_EN_Handler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	UIwifi.Wifi_password_en= *(DataInPtr) ;
	if (UIwifi.Wifi_password_en==1)
	{
		SendSTRDataToDisplay(WIFI_PW_WR, (uint8_t*)&UIwifi.wifi_user_pswd);
	}
	else
	{
		memset(&UIwifi.wifi_user_pswd_star,0x0,sizeof(UIwifi.wifi_user_pswd_star));
		strncpy((char*)UIwifi.wifi_user_pswd_star,(char*)dummy_star,strlen((char*)UIwifi.wifi_user_pswd));
		SendSTRDataToDisplay(WIFI_PW_WR, (uint8_t*)&UIwifi.wifi_user_pswd_star);
	}
}
/*
 * *****************************************************************************
 * Function name: WifipwHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifi  password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void WifipwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIwifi.wifi_user_pswd, 0, sizeof(UIwifi.wifi_user_pswd));   // Erase
	for(i=0;i<31;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(1)//(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIwifi.wifi_user_pswd[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				memcpy((char*)UIwifi.wifi_user_pswd, (char*)comm_par.sy_par.wifi.pswd,  sizeof(UIwifi.wifi_user_pswd));
				break;
			}
		}
		else
			break;
	}
	if(strlen((char*)UIwifi.wifi_user_pswd)<8)
	{
		y=1;
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter minimum 8 characters !!!");
		memcpy((char*)UIwifi.wifi_user_pswd, (char*)comm_par.sy_par.wifi.pswd,  sizeof(UIwifi.wifi_user_pswd));
	}
	if (y==0)
	{
		if(isValidInputpassword((const char*) UIwifi.wifi_user_pswd)&&(DataPtr[8]!=0x00))
		{
			if (UIwifi.Wifi_password_en == 1)
			{
				SendSTRDataToDisplay(WIFI_PW_WR,
						(uint8_t*) &UIwifi.wifi_user_pswd);
			} else
			{
				strncpy((char*) UIwifi.wifi_user_pswd_star, (char*) dummy_star,
						strlen((char*) UIwifi.wifi_user_pswd));
				SendSTRDataToDisplay(WIFI_PW_WR,
						(uint8_t*) &UIwifi.wifi_user_pswd_star);
			}
		}
		else
		{
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Character(, or \")and spaces(beginning and end)not allowed!!!");
			memcpy((char*)UIwifi.wifi_user_pswd, (char*)comm_par.sy_par.wifi.pswd,  sizeof(UIwifi.wifi_user_pswd));
		}

	}
}

/*
 * *****************************************************************************
 * Function name: WifiUserHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void WifiUserHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIwifi.wifi_user, 0, sizeof(UIwifi.wifi_user));   // Erase
	for(i=0;i<31;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(1)//(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIwifi.wifi_user[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter valid details !!!");
				memcpy((char*)UIwifi.wifi_user, (char*)comm_par.sy_par.wifi.uuid,  sizeof(UIwifi.wifi_user));
				break;
			}
		}
		else
		break;
	}
	if (y==0)
	{
		if(isValidInput((const char*) UIwifi.wifi_user)&&(DataPtr[8]!=0x00))
		{
			SendSTRDataToDisplay(WIFI_ADD_WR,(uint8_t*)&UIwifi.wifi_user);
			SendN16DataToDisplay(WIFI_PASS_STR_En,1);
			memset(UIwifi.wifi_user_pswd,0x00,sizeof(UIwifi.wifi_user_pswd));
			SendSTRDataToDisplay(WIFI_PW_WR,(uint8_t*)&UIwifi.wifi_user_pswd);
		}
		else
		{
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Character(, or \") are not allowed!!!");
			memcpy((char*)UIwifi.wifi_user, (char*)comm_par.sy_par.wifi.uuid,  sizeof(UIwifi.wifi_user));
		}

	}
}



/*
 * *****************************************************************************
 * Function name: WifiUserHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void AWSUrlHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y=0;
	Fsize=0;

	memset(UIAws.Aws_url, 0, sizeof(UIAws.Aws_url));   // Erase
	for(i=0;i<128;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(1)//(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIAws.Aws_url[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				memcpy((char*)UIAws.Aws_url,(char*)comm_par.sy_par.iot.iot_url,  sizeof(UIAws.Aws_url));
				break;
			}

		}
		else
			break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(AWS_URL_WR,(uint8_t*)&UIAws.Aws_url);
	}

}


/*
 * *****************************************************************************
 * Function name: AWS_UsernameidHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void AWS_UsernameidHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y=0;
	Fsize=0;

	memset(UIAws.Aws_username, 0, sizeof(UIAws.Aws_username));   // Erase
	for(i=0;i<64;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x40)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a))||(DataPtr[8+i]==0x2E)||(DataPtr[8+i]==0x2D)||(DataPtr[8+i]==0x5F))
			{
				UIAws.Aws_username[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9)(@)(-)(_)(.) Only !!!!!!");
				memcpy((char*)UIAws.Aws_username,(char*)comm_par.sy_par.iot.User_ID,  sizeof(UIAws.Aws_username));
				SendSTRDataToDisplay(AWS_USER_NAME_RD,(uint8_t*)&UIAws.Aws_username);
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(AWS_USER_NAME_WR,(uint8_t*)&UIAws.Aws_username);
	}
}



/*
 * *****************************************************************************
 * Function name: AWS_PasswordHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

/*void AWS_PasswordHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize;
	Fsize=0;

	memset(UIAws.Aws_password, 0, sizeof(UIAws.Aws_password));   // Erase
	for(i=0;i<50;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{

			UIAws.Aws_password[i]=DataPtr[8+i];
			Fsize++;
		}
		else
		break;
	}
//		SendSTRDataToDisplay(AWS_PASSWORD_WR,(uint8_t*)&UIAws.Aws_password);
}*/



/*
 * *****************************************************************************
 * Function name: AWS_EmailHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void AWS_EmailHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y=0;
	Fsize=0;

	memset(UIAws.Aws_email, 0, sizeof(UIAws.Aws_email));   // Erase
	for(i=0;i<64;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x40)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a))||(DataPtr[8+i]==0x2E)||(DataPtr[8+i]==0x2D)||(DataPtr[8+i]==0x5F))
			{
				UIAws.Aws_email[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9)(@)(-)(_)(.) Only !!!");
				memcpy((char*)UIAws.Aws_email,(char*)comm_par.sy_par.iot.iot_email,  sizeof(UIAws.Aws_email));
				SendSTRDataToDisplay(AWS_EMAIL_RD,(uint8_t*)&UIAws.Aws_email);
				break;
			}

		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(AWS_EMAIL_WR,(uint8_t*)&UIAws.Aws_email);
	}
}

/*
 * *****************************************************************************
 * Function name: AWS_PhoneHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void AWS_PhoneHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y=0;
	Fsize=0;

	memset(UIAws.Aws_phone, 0, sizeof(UIAws.Aws_phone));   // Erase
	for(i=0;i<20;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(DataPtr[8]!=0x2B)
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"country code missing !!!");
				memcpy((char*)UIAws.Aws_phone,(char*)comm_par.sy_par.iot.iot_phone,  sizeof(UIAws.Aws_phone));
				SendSTRDataToDisplay(AWS_PHNO_RD,(uint8_t*)&UIAws.Aws_phone);
				break;
			}
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||(DataPtr[8+i]==0x2B))
			{
				UIAws.Aws_phone[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (+)(0-9) Only !!!");
				memcpy((char*)UIAws.Aws_phone,(char*)comm_par.sy_par.iot.iot_phone,  sizeof(UIAws.Aws_phone));
				SendSTRDataToDisplay(AWS_PHNO_RD,(uint8_t*)&UIAws.Aws_phone);
				break;
			}

		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(AWS_PHNO_WR,(uint8_t*)&UIAws.Aws_phone);
	}
}

/*
 * *****************************************************************************
 * Function name: AWS_PhoneHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to wifiuser password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

/*void AWS_TokenHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize;
	Fsize=0;

	memset(UIAws.Aws_token, 0, sizeof(UIAws.Aws_token));   // Erase
	for(i=0;i<50;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{

			UIAws.Aws_token[i]=DataPtr[8+i];
			Fsize++;
		}
		else
		break;
	}
		SendSTRDataToDisplay(AWS_TOKEN_WR,(uint8_t*)&UIAws.Aws_token);
}*/


/*
 * *****************************************************************************
 * Function name: conformpwHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to conform  password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void conformpwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize;
	Fsize=0;
	memset(UIlogin.conform_pw, 0, sizeof(UIlogin.conform_pw));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIlogin.conform_pw[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				break;
			}
		}
		else
		break;
	}

	uint8_t n=strlen((char*)&UIlogin.conform_pw);
	uint8_t unvisible[8] ;
	memset(unvisible, 0, 8);
	for(i=0;i<n;i++)
	{
		unvisible[i]='*';
	}
	SendSTRDataToDisplay(LOGIN_CONFM_PW_WR,(uint8_t*)&unvisible);
	SendSTRDataToDisplay(0x004900,(uint8_t*)"\0");
}


/*
 * *****************************************************************************
 * Function name: NewpwHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to create  password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void NewpwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIlogin.new_pw, 0, sizeof(UIlogin.new_pw));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIlogin.new_pw[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				break;
			}
		}
		else
		break;
	}
	uint8_t n=strlen((char*)&UIlogin.new_pw);
	uint8_t unvisible[8] ;
	memset(unvisible, 0, 8);
	for(i=0;i<n;i++)
	{
		unvisible[i]='*';
	}
	if (y==0)
	{
		SendSTRDataToDisplay(LOGIN_NEW_PW_WR,(uint8_t*)&unvisible);
	}

}
/*
 * *****************************************************************************
 * Function name: LoginpwHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to login  password string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void LoginpwHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
		Fsize=0;
		y=0;
		memset(UIlogin.login_pw, 0, sizeof(UIlogin.login_pw));   // Erase
		for(i=0;i<8;i++)
		{
			if((DataPtr[8+i]!=0x00))
			{
				if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
				{
					UIlogin.login_pw[i]=DataPtr[8+i];
					Fsize++;
				}
				else
				{
					y=1;
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
					SendSTRDataToDisplay(0x004800,(uint8_t*)"\0");
					memset(UIlogin.login_pw, 0, 8);

					break;
				}
			}
			else
			break;
		}
        uint8_t n=strlen((char*)&UIlogin.login_pw);
        uint8_t unvisible[8] ;
        memset(unvisible, 0, 8);
        for(i=0;i<n;i++)
        {
        	unvisible[i]='*';
        }
		if((y==0))
		{
			SendSTRDataToDisplay(LOGIN_PW_WR,(uint8_t*)&unvisible);
		}
}



/*
 * *****************************************************************************
 * Function name: LoginHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to login string
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void LoginHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIlogin.login_user, 0, sizeof(UIlogin.login_user));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIlogin.login_user[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t*)"\0");
				break;
			}
		}
		else
		break;
	}
	int read=0;
	for(int i=0;i<11;i++)
	{
		if((strcmp((char*)login_cred[i].admin_id, (char*)UIlogin.login_user)==0) && (y==0))
		{
			read=1;
			Login_id_cnt=i;
			SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t*)&UIlogin.login_user);
			break;

		}
		else if((strcmp((char*)login_cred[i].user_id, (char*)UIlogin.login_user)==0) && (y==0))
		{
			read=1;
			Login_id_cnt=i;
			SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t*)&UIlogin.login_user);
            break;
		}
	}
	SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 0);
	if(read==0)
	{
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid Login Id !!!");
		SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t*)"\0");
		SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t*)"\0");
		memset((char*)UIlogin.login_user, 0x00, MAX_ID_SIZE);
	}
}


/*
 * *****************************************************************************
 * Function name: listall
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to select all list
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void listall(void)
{
	int i;
	for( i=1;(i<3 && (login_cred[i].admin_id[0]!=0) && (login_cred[i].admin_id[0]!=255));i++)
	{
			SendSTRDataToDisplay(address_reg[i].User_add,(uint8_t*)&login_cred[i].admin_id);
			SendN16DataToDisplay(address_reg[i].admin,1);
			SendN16DataToDisplay(address_reg[i].User,0);
			SendN16DataToDisplay(address_reg[i].Status_add,1);

	}
	int k=3;
	int j;
	for( j=i;(j<11 && (login_cred[k].user_id[0]!=0) && (login_cred[k].user_id[0]!=255));j++)
	{
			SendSTRDataToDisplay(address_reg[j].User_add,(uint8_t*)&login_cred[k].user_id);
			SendN16DataToDisplay(address_reg[j].admin,0);
			SendN16DataToDisplay(address_reg[j].User,1);
			SendN16DataToDisplay(address_reg[j].Status_add,1);
			k++;
	}
	int accounts=j-2;
	SendN16DataToDisplay(0x0801A0,0);
	if(accounts>4)
	{
		SendN16DataToDisplay(0x0801A0,1);
	}
	for(int z=j;(z<11&&((login_cred[k].user_id[0]==0)||(login_cred[k].user_id[0]==255)));z++)
	{
			SendSTRDataToDisplay(address_reg[z].User_add,(uint8_t*)&login_cred[k].user_id);
			SendN16DataToDisplay(address_reg[z].admin,0);
			SendN16DataToDisplay(address_reg[z].User,0);
			SendN16DataToDisplay(address_reg[z].Status_add,0);
			k++;
	}
}

/*
 * *****************************************************************************
 * Function name: listselection
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to select list
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void listselection(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	int16_t selection = (*(DataPtr+8) << 8) | *(DataPtr+9);
	for(int i=0;i<11;i++)
	{
		SendN16DataToDisplay(address_reg[i].id_delete,0);
		UIUserAccess.delete_select[i]=0;
		SendN16DataToDisplay(address_reg[i].Status_add,0);

	}

	if(selection==0)
	{
		int i;
		for( i=1;(i<3 && (login_cred[i].admin_id[0]!=0) && (login_cred[i].admin_id[0]!=255));i++)
		{
				SendSTRDataToDisplay(address_reg[i].User_add,(uint8_t*)&login_cred[i].admin_id);
				SendN16DataToDisplay(address_reg[i].admin,1);
				SendN16DataToDisplay(address_reg[i].User,0);
				SendN16DataToDisplay(address_reg[i].Status_add,1);
			//SendSTRDataToDisplay(address_reg[i].User_add,(uint8_t*)&login_cred[i].user_id);
		}

		for(int j=i;(j<11 && ((login_cred[i].admin_id[0]==0)||(login_cred[i].admin_id[0]==255)));j++)
		{
			SendSTRDataToDisplay(address_reg[j].User_add,(uint8_t*)"\0");
			SendN16DataToDisplay(address_reg[j].admin,0);
			SendN16DataToDisplay(address_reg[j].User,0);
		}
		SendN16DataToDisplay(0x082024,0);
		SendN16DataToDisplay(0x082026,0);
		SendN16DataToDisplay(0x082028,0);
	}
	else if(selection==1)
	{
		int k=3;
		int j=0;
		for(j=1;(j<11&&(login_cred[k].user_id[0]!=0)&&(login_cred[k].user_id[0]!=255));j++)
		{
				SendSTRDataToDisplay(address_reg[j].User_add,(uint8_t*)&login_cred[k].user_id);
				SendN16DataToDisplay(address_reg[j].admin,0);
				SendN16DataToDisplay(address_reg[j].User,1);
				SendN16DataToDisplay(address_reg[j].Status_add,1);
				k++;
			//SendSTRDataToDisplay(address_reg[i].User_add,(uint8_t*)&login_cred[i].user_id);
		}

		int accounts=j-2;
		SendN16DataToDisplay(0x0801A0,0);
		if(accounts>4)
		{
			SendN16DataToDisplay(0x0801A0,1);

		}

		for(int z=j;(z<11&&((login_cred[k].user_id[0]==0)||(login_cred[k].user_id[0]==255)));z++)
		{
				SendSTRDataToDisplay(address_reg[z].User_add,(uint8_t*)&login_cred[k].user_id);
				SendN16DataToDisplay(address_reg[z].admin,0);
				SendN16DataToDisplay(address_reg[z].User,0);
				k++;
		}
	}
	else
	{
		listall();
	}
}

/*
 * *****************************************************************************
 * Function name: AddUserHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to create User Id
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void AddUserHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIAddUserStruct.AddUserName, 0, sizeof(UIAddUserStruct.AddUserName));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIAddUserStruct.AddUserName[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)&UIAddUserStruct.AddUserName);
	}
}
/*
 * *****************************************************************************
 * Function name: EmployeIDHandler
 * Created by :Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *  Main function to create employee Id
 * Parameters:
 *  Input  :  VPAddr, DataInPtr, DataPtr,  Size
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void EmployeIDHandler(uint32_t VPAddr, int16_t *DataInPtr, uint8_t *DataPtr, uint16_t Size)
{
	uint8_t i,Fsize,y;
	Fsize=0;
	y=0;
	memset(UIAddUserStruct.EmployeID, 0, sizeof(UIAddUserStruct.EmployeID));   // Erase
	for(i=0;i<8;i++)
	{
		if((DataPtr[8+i]!=0x00))
		{
			if(((DataPtr[8+i]>=0x30)&&(DataPtr[8+i]<=0x39))||((DataPtr[8+i]>=0x41)&&(DataPtr[8+i]<=0x5a))||((DataPtr[8+i]>=0x61)&&(DataPtr[8+i]<=0x7a)))
			{
				UIAddUserStruct.EmployeID[i]=DataPtr[8+i];
				Fsize++;
			}
			else
			{
				y=1;
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter (a-z)(A-Z)(0-9) Only !!!");
				SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
				break;
			}
		}
		else
		break;
	}
	if(y==0)
	{
		SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)&UIAddUserStruct.EmployeID);
	}
}
/*
 * *****************************************************************************
 * Function name: Loginfunction
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Login account details.
 * Parameters:
 *  Input  :  None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Loginfunction(void)
{
	if(strcmp((char*)login_cred[Login_id_cnt].user_pswd, (char*)UIlogin.login_pw)==0)
	{
		login_cred[Login_id_cnt].attempts=0;
		SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
		if(Login_id_cnt<3)
		{
			vp_rt_wr_str.LoginStatus=2;
			if(Login_id_cnt<2)
			{
				login_access.Access_allow=1;
			}
			SendN16DataToDisplay(SECURITY_RESTRICT, 1);
			//ULOG_DATA("LD1, Login Id:%s, Emp Id:%s",login_cred[Login_id_cnt].admin_id,login_cred[Login_id_cnt].emp_id);
		}
		else
		{
			vp_rt_wr_str.LoginStatus=1;
			SendN16DataToDisplay(SECURITY_RESTRICT, 0);
			//ULOG_DATA("LD1, Login Id:%s, Emp Id:%s",login_cred[Login_id_cnt].user_id,login_cred[Login_id_cnt].emp_id);

		}
		SendSTRDataToDisplay(LOGEDIN_USER,(uint8_t*)&UIlogin.login_user);
		SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t*)"\0");
		SendSTRDataToDisplay(LOGIN_PW_WR,(uint8_t*)"\0");
		SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t*)"\0");
		TargetPage = 0;
		SendCmdToDisplay(LOAD_PAGE_CMD);
		SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 0);
		SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 0);
		memset((char*)UIlogin.login_pw, 0x00, MAX_ID_SIZE);
		memset((char*)UIlogin.login_user, 0x00, MAX_ID_SIZE);
	}
	else
	{
		SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 1);
		login_cred[Login_id_cnt].attempts++;
		SendSTRDataToDisplay(LOGIN_PW_WR,(uint8_t*)"\0");
		SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t*)"\0");
		if(login_cred[Login_id_cnt].attempts>9)
		{
			login_cred[Login_id_cnt].attempts=10;
			SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
		}
		else
		{
			SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
		}

	}
	if(login_cred[Login_id_cnt].attempts>9)
	{
		SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 1);
		SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 0);
		uint8_t lock[8]="*****";
		memcpy((uint8_t*)&login_cred[Login_id_cnt].user_pswd, (uint8_t*)&lock, sizeof(lock));
		EEPROM_Write(eep_user_reg[Login_id_cnt].pw_add, 0,(uint8_t*)&login_cred[Login_id_cnt].user_pswd,8);
	}
}

/*
 * *****************************************************************************
 * Function name: Logoutfunction
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Logout account details.
 * Parameters:
 *  Input  : None
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void Logoutfunction(void)
{
//	if(Login_id_cnt<3)
//		//ULOG_DATA("LD2, %s logged out" ,login_cred[Login_id_cnt].admin_id);
//	else
//		//ULOG_DATA("LD2, %s logged out" ,login_cred[Login_id_cnt].user_id);
	login_access.Access_allow=0;
	vp_rt_wr_str.LoginStatus=0;         // status icon
	SendSTRDataToDisplay(LOGEDIN_USER,(uint8_t*)"\0");   //string below icon 000E00
	TargetPage = 0;
	SendCmdToDisplay(LOAD_PAGE_CMD);
}

/*
 * *****************************************************************************
 * Function name: SavePasswordSettings
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 *   Main function to Save password.
 * Parameters:
 *  Input  : login credentials
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void SavePasswordSettings(login_credentials *UIloginData)
{
	if((login_cred[Login_id_cnt].user_pswd[0]==0)||(login_cred[Login_id_cnt].user_pswd[0]==255))
	{
		if(UIlogin.conform_pw[0]!=0)
		{
			if(strcmp((char*)UIlogin.new_pw,(char*)UIlogin.conform_pw)==0)
			{
				uint8_t bfr[2];
				memcpy((char*)login_cred[Login_id_cnt].user_pswd, (char*)UIlogin.conform_pw, MAX_ID_SIZE);
				EEPROM_Write(eep_user_reg[Login_id_cnt].pw_add, 0,(uint8_t*)&login_cred[Login_id_cnt].user_pswd,8);
				crc16 = usMBCRC16((uint8_t*)&login_cred[Login_id_cnt].user_pswd, 8);
				bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
				bfr[1]  = (uint8_t)(crc16 & 0x00ff);
				EEPROM_Write(eep_user_reg[Login_id_cnt].pw_crc_add, 0,bfr,2);
				SendSTRDataToDisplay(LOGIN_ID_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(LOGIN_NEW_PW_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t*)"\0");
		//		SendSTRDataToDisplay(0x004880,0);
				SendSTRDataToDisplay(LOGIN_CONFM_PW_WR,(uint8_t*)"\0");
		//		SendSTRDataToDisplay(LOGIN_CONFM_PW_WR,0);
				TargetPage = 0;
				Account_creation=1;
				login_cred[Login_id_cnt].attempts=0;
				SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
				//memset(UIlogin.conform_pw, 0, sizeof(UIlogin.conform_pw));
				SendN16DataToDisplay(STRING_POPUP_EN, 2);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User register success!!!");
			}
			else
			{
				SendSTRDataToDisplay(LOGIN_NEW_PW_WR,(uint8_t*)"\0");
				SendSTRDataToDisplay(LOGIN_ID_RD,(uint8_t*)"\0");
				//SendSTRDataToDisplay(0x004880,0);
				SendSTRDataToDisplay(LOGIN_CONFM_PW_WR,(uint8_t*)"\0");
				SendN16DataToDisplay(FIRST_USER_EN, 0);

				//SendSTRDataToDisplay(LOGIN_CONFM_PW_WR,0);

				login_cred[Login_id_cnt].attempts++;
				if(login_cred[Login_id_cnt].attempts>3)
				{
					SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
					login_cred[Login_id_cnt].attempts=4;
				}
				else
				{
					SendN16DataToDisplay(LOGIN_LIMITS, login_cred[Login_id_cnt].attempts);
					SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 1);
				}
			}
		}
		else
		{
			SendSTRDataToDisplay(LOGIN_NEW_PW_WR,(uint8_t*)"\0");
			SendSTRDataToDisplay(LOGIN_CONFM_PW_WR,(uint8_t*)"\0");
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter a valid password !!!");
		}
		memset((char*)UIlogin.login_user, 0x00, MAX_ID_SIZE);
		memset(UIlogin.conform_pw, 0, sizeof(UIlogin.conform_pw));
		memset((char*)UIlogin.new_pw, 0x00, MAX_ID_SIZE);
	}

	if(login_cred[Login_id_cnt].attempts>2)
	{
		SendN16DataToDisplay(ACCOUNT_LOCKED_EN, 1);
		SendN16DataToDisplay(ACCOUNT_MISMATCH_EN, 0);
		uint8_t lock[8]="*****";
		memcpy((uint8_t*)&login_cred[Login_id_cnt].user_pswd, (uint8_t*)&lock, sizeof(lock));
		EEPROM_Write(eep_user_reg[Login_id_cnt].pw_add, 0,(uint8_t*)&login_cred[Login_id_cnt].user_pswd,8);
	}
}
void remove_tick(void)
{
	for(int i=0;i<11;i++)
	{
		UIUserAccess.delete_select[i]=0;
		SendN16DataToDisplay(address_reg[i].id_delete,0);
		SendN16DataToDisplay(address_reg[i].touch_id,0);
	}
}
/*
 * *****************************************************************************
 * Function name: deleteUser
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		Deleting user account.
 * Parameters:
 *  Input  :  user Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void deleteUser(User_access  *UserSetings)
{
	uint8_t admins=0;
	uint8_t select_act=0;

	for(int i=1;i<3;i++)
	{
		if((login_cred[i].admin_id[0]!=0) && (login_cred[i].admin_id[0]!=255))
		{
			 admins++;
		}
	}
	if(UserSetings->list_select==0)
	{
		if((UserSetings->delete_select[0])&&(Login_id_cnt-1!=0)&&(login_access.Access_allow==1))
		{
			 memset((char*)login_cred[1].admin_id, 0x00, MAX_ID_SIZE);
			 memset((char*)login_cred[1].emp_id, 0x00, MAX_ID_SIZE);
			 memset((char*)login_cred[1].user_id, 0x00, MAX_ID_SIZE);
			 memset((char*)login_cred[1].user_pswd, 0xFF, MAX_ID_SIZE);
			 login_cred[1].user=0;
			 login_cred[1].attempts=0;
			 Login_id_cnt=Login_id_cnt-1;

		}
		else if((UserSetings->delete_select[1])&&(Login_id_cnt-1!=1))
		{
			 memset((char*)login_cred[2].admin_id, 0x00, MAX_ID_SIZE);
			 memset((char*)login_cred[2].emp_id, 0x00, MAX_ID_SIZE);
			 memset((char*)login_cred[2].user_id, 0x00, MAX_ID_SIZE);
			 memset((char*)login_cred[2].user_pswd, 0xFF, MAX_ID_SIZE);
			 login_cred[2].attempts=0;
			 login_cred[2].user=0;
		}
	}
	else if(UserSetings->list_select==1)
	{
		for(int i=0;i<8;i++)
		{
			if(UserSetings->delete_select[i])
			{
				 memset((char*)login_cred[i+3].user_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[i+3].emp_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[i+3].user_pswd, 0xFF, MAX_ID_SIZE);
				 login_cred[i+3].attempts=0;
				 select_act++;
			}

		}

	}
	else if(UserSetings->list_select)
	{
		int i;
		for(i=0;i<admins;i++)
		{
			if((UserSetings->delete_select[i])&&(Login_id_cnt-1!=i)&&(login_access.Access_allow==1))
			{
				 memset((char*)login_cred[i+1].admin_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[1+1].emp_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[i+1].user_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[i+1].user_pswd, 0xFF, MAX_ID_SIZE);
				 login_cred[i+1].attempts=0;
				 login_cred[i].user=0;
				 if(Login_id_cnt==2)
				 {
					 Login_id_cnt=Login_id_cnt-1;
				 }
			}
		}

		for(int j=i;j<11;j++)
		{
			if(UserSetings->delete_select[j])
			{
				 memset((char*)login_cred[3+j-i].user_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[3+j-i].emp_id, 0x00, MAX_ID_SIZE);
				 memset((char*)login_cred[3+j-i].user_pswd, 0xFF, MAX_ID_SIZE);
				 login_cred[3+j-i].attempts=0;
				 select_act++;
			}
		}
	}
	//moving data after delete
	for(int i=1;i<3;i++)
	{
		if((login_cred[i].admin_id[0]==0)||(login_cred[i].admin_id[0]==255))
		{
			int j;
			for (j=i; j<(2); j++)
			{
				memcpy((char*)login_cred[j].admin_id, (char*)login_cred[j+1].admin_id, MAX_ID_SIZE);
				memcpy((char*)login_cred[j].emp_id, (char*)login_cred[j+1].emp_id, MAX_ID_SIZE);
				memcpy((char*)login_cred[j].user_pswd, (char*)login_cred[j+1].user_pswd, MAX_PSWD_SIZE);
				memcpy((char*)&login_cred[j].user, (char*)&login_cred[j+1].user, 1);
				login_cred[j].attempts= login_cred[j+1].attempts;
			}
			memset((char*)login_cred[j].admin_id, 0x00, MAX_ID_SIZE);
		    memset((char*)login_cred[j].user_pswd, 0x00, MAX_PSWD_SIZE);
		    memset((char*)login_cred[j].emp_id, 0x00, MAX_PSWD_SIZE);
		    memset((char*)&login_cred[j].user, 0xFF, 1);
		    login_cred[j].attempts=0;

		}
	}
	for(int k=0;k<select_act;k++)
	{
		for(int i=3;i<11;i++)
		{
			if((login_cred[i].user_id[0]==0)||(login_cred[i].user_id[0]==255))
			{
				int j;
				for (j=i; j<(MAX_ADMIN_ACCOUNTS+MAX_USER_ACCOUNTS); j++)
				  {
					memcpy((char*)login_cred[j].user_id, (char*)login_cred[j+1].user_id, MAX_ID_SIZE);
					memcpy((char*)login_cred[j].emp_id, (char*)login_cred[j+1].emp_id, MAX_ID_SIZE);
					memcpy((char*)login_cred[j].user_pswd, (char*)login_cred[j+1].user_pswd, MAX_PSWD_SIZE);
					login_cred[j].attempts= login_cred[j+1].attempts;
				  }
				memset((char*)login_cred[j].user_id, 0x00, MAX_ID_SIZE);
				memset((char*)login_cred[j].user_pswd, 0x00, MAX_PSWD_SIZE);
				memset((char*)login_cred[j].emp_id, 0xFF, MAX_PSWD_SIZE);
				login_cred[j].attempts=0;
			}
		}
	}
	uint8_t bfr[2];
	EEPROM_Write(admin1, 0,(uint8_t*)&login_cred[1].admin_id,8);
	EEPROM_Write(admin1_sts, 0,(uint8_t*)&login_cred[1].user,1);
	crc16 = usMBCRC16((uint8_t*)&login_cred[1].admin_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(admin1_crc, 0,bfr,2);
	EEPROM_Write(emp1, 0,(uint8_t*)&login_cred[1].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[1].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp1_crc1, 0,bfr,2);
	EEPROM_Write(eep_user_reg[1].pw_add, 0,(uint8_t*)&login_cred[1].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[1].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[1].pw_crc_add, 0,bfr,2);

	EEPROM_Write(admin2, 0,(uint8_t*)&login_cred[2].admin_id,8);
	EEPROM_Write(admin2_sts, 0,(uint8_t*)&login_cred[2].user,1);
	crc16 = usMBCRC16((uint8_t*)&login_cred[2].admin_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(admin2_crc, 0,bfr,2);
	EEPROM_Write(emp2, 0,(uint8_t*)&login_cred[2].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[2].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp2_crc2, 0,bfr,2);
	EEPROM_Write(eep_user_reg[2].pw_add, 0,(uint8_t*)&login_cred[2].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[2].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[2].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user0, 0,(uint8_t*)&login_cred[3].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[3].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user0_crc, 0,bfr,2);
	EEPROM_Write(emp3, 0,(uint8_t*)&login_cred[3].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[3].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp3_crc3, 0,bfr,2);
	EEPROM_Write(eep_user_reg[3].pw_add, 0,(uint8_t*)&login_cred[3].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[3].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[3].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user1, 0,(uint8_t*)&login_cred[4].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[4].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user1_crc, 0,bfr,2);
	EEPROM_Write(emp4, 0,(uint8_t*)&login_cred[4].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[4].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp4_crc4, 0,bfr,2);
	EEPROM_Write(eep_user_reg[4].pw_add, 0,(uint8_t*)&login_cred[4].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[4].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[4].pw_crc_add, 0,bfr,2);


	EEPROM_Write(user2, 0,(uint8_t*)&login_cred[5].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[5].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user2_crc, 0,bfr,2);
	EEPROM_Write(emp5, 0,(uint8_t*)&login_cred[5].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[5].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp5_crc5, 0,bfr,2);
	EEPROM_Write(eep_user_reg[5].pw_add, 0,(uint8_t*)&login_cred[5].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[5].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[5].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user3, 0,(uint8_t*)&login_cred[6].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[6].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user3_crc, 0,bfr,2);
	EEPROM_Write(emp6, 0,(uint8_t*)&login_cred[6].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[6].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp6_crc6, 0,bfr,2);
	EEPROM_Write(eep_user_reg[6].pw_add, 0,(uint8_t*)&login_cred[6].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[6].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[6].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user4, 0,(uint8_t*)&login_cred[7].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[7].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user4_crc, 0,bfr,2);
	EEPROM_Write(emp7, 0,(uint8_t*)&login_cred[7].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[7].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp7_crc7, 0,bfr,2);
	EEPROM_Write(eep_user_reg[7].pw_add, 0,(uint8_t*)&login_cred[7].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[7].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[7].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user5, 0,(uint8_t*)&login_cred[8].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[8].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user5_crc, 0,bfr,2);
	EEPROM_Write(emp8, 0,(uint8_t*)&login_cred[8].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[8].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp8_crc8, 0,bfr,2);
	EEPROM_Write(eep_user_reg[8].pw_add, 0,(uint8_t*)&login_cred[8].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[8].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[8].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user6, 0,(uint8_t*)&login_cred[9].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[9].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user6_crc, 0,bfr,2);
	EEPROM_Write(emp9, 0,(uint8_t*)&login_cred[9].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[9].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp9_crc9, 0,bfr,2);
	EEPROM_Write(eep_user_reg[9].pw_add, 0,(uint8_t*)&login_cred[9].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[9].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[9].pw_crc_add, 0,bfr,2);

	EEPROM_Write(user7, 0,(uint8_t*)&login_cred[10].user_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[10].user_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(user7_crc, 0,bfr,2);
	EEPROM_Write(emp10, 0,(uint8_t*)&login_cred[10].emp_id,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[10].emp_id, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(emp10_crc10, 0,bfr,2);
	EEPROM_Write(eep_user_reg[10].pw_add, 0,(uint8_t*)&login_cred[10].user_pswd,8);
	crc16 = usMBCRC16((uint8_t*)&login_cred[10].user_pswd, 8);
	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
	EEPROM_Write(eep_user_reg[10].pw_crc_add, 0,bfr,2);

//	TargetPage = 41;
//	SendCmdToDisplay(LOAD_PAGE_CMD);
	UserSetings->list_select=2;
	SendN16DataToDisplay(LIST_SELECT_WR,2);

	listall();

	//removing delete select
	remove_tick();
	//memset(UIUserAccess.delete_select,0x00,sizeof(UIUserAccess.delete_select));

}


/*
 * *****************************************************************************
 * Function name: SaveAddUserSettings
 * Created by : Arikanti venkatesh
 * Date:       14-10-2025
 * Description:
 * 		  Application to save account details.
 * Parameters:
 *  Input  : Settings data pointer
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */
void SaveAddUserSettings(AddUserStruct *AddUserSettings)
{
	uint8_t  bfr[2];
	uint8_t i=0;
	uint8_t act_exit=1;
	if(UIAddUserStruct.select==1)
	{
		for(i=0;i<3;i++)
		{
			for(int j=3;(j<11&&act_exit);j++)
			{
				if(strcmp((char*)login_cred[j].user_id, (char*)UIAddUserStruct.AddUserName)==0)
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User ID existed!!!");
					act_exit=0;
				}
			}
			if(UIAddUserStruct.AddUserName[0]==0)
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter a valid value!!!");
				act_exit=0;
			}
			if(strcmp((char*)login_cred[i].admin_id, (char*)UIAddUserStruct.AddUserName)==0)
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User ID existed!!!");
				//memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
				act_exit=0;
			}
			else if(((login_cred[i].admin_id[0]==0)||(login_cred[i].admin_id[0]==255))&&(i<3)&&(act_exit))
			{
				memcpy((uint8_t*)&login_cred[i].admin_id, (uint8_t*)&UIAddUserStruct.AddUserName, sizeof(UIAddUserStruct.AddUserName));
				memcpy((uint8_t*)&login_cred[i].emp_id, (uint8_t*)&UIAddUserStruct.EmployeID, sizeof(UIAddUserStruct.EmployeID));
				login_cred[i].user=1;
				EEPROM_Write(eep_user_reg[i].sts_add, 0,(uint8_t*)&login_cred[i].user,1);
				EEPROM_Write(eep_user_reg[i].User_add, 0,(uint8_t*)&login_cred[i].admin_id,8);
				EEPROM_Write(eep_user_reg[i].pw_add, 0,(uint8_t *)clearPwd,8);
				crc16 = usMBCRC16((uint8_t*)&login_cred[i].admin_id, 8);
				bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
				bfr[1]  = (uint8_t)(crc16 & 0x00ff);
				EEPROM_Write(eep_user_reg[i].User_add_crc, 0,bfr,2);
				EEPROM_Write(eep_user_reg[i].emp_add, 0,(uint8_t*)&login_cred[i].emp_id,8);
				crc16 = usMBCRC16((uint8_t*)&login_cred[i].emp_id, 8);
				bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
				bfr[1]  = (uint8_t)(crc16 & 0x00ff);
				EEPROM_Write(eep_user_reg[i].emp_add_crc, 0,bfr,2);
				SendN16DataToDisplay(STRING_POPUP_EN, 2);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User ID Created!!!");

				TargetPage = 20;
				break;
			}
		}
		if((i==3) && act_exit)
		{
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Limit Exceed!!!");
		}
	}
	else if(Login_id_cnt>0 && Login_id_cnt<3)
	{
		for(i=3;(i<11&&act_exit);i++)
		{
			for(int j=0;j<3;j++)
			{
				if(strcmp((char*)login_cred[j].admin_id, (char*)UIAddUserStruct.AddUserName)==0)
				{
					SendN16DataToDisplay(STRING_POPUP_EN, 1);
					SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User ID existed!!!");
					//memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
					act_exit=0;
				}
			}
			if(UIAddUserStruct.AddUserName[0]==0)
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Enter a valid value!!!");
				act_exit=0;
			}
			else if(strcmp((char*)login_cred[i].user_id, (char*)UIAddUserStruct.AddUserName)==0)
			{
				SendN16DataToDisplay(STRING_POPUP_EN, 1);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User ID existed!!!");
				act_exit=0;
			}
			else if(((login_cred[i].user_id[0]==0)||(login_cred[i].user_id[0]==255))&&(i<11)&&(act_exit))
			{
				uint8_t bfr[2];
				memcpy((uint8_t*)&login_cred[i].user_id, (uint8_t*)&UIAddUserStruct.AddUserName, sizeof(UIAddUserStruct.AddUserName));
				memcpy((uint8_t*)&login_cred[i].emp_id, (uint8_t*)&UIAddUserStruct.EmployeID, sizeof(UIAddUserStruct.EmployeID));
				EEPROM_Write(eep_user_reg[i].User_add, 0,(uint8_t*)&login_cred[i].user_id,8);
				crc16 = usMBCRC16((uint8_t*)&login_cred[i].user_id, 8);
				bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
				bfr[1]  = (uint8_t)(crc16 & 0x00ff);
				EEPROM_Write(eep_user_reg[i].User_add_crc, 0,bfr,2);
				EEPROM_Write(eep_user_reg[i].emp_add, 0,(uint8_t*)&login_cred[i].emp_id,8);
				crc16 = usMBCRC16((uint8_t*)&login_cred[i].emp_id, 8);
				bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
				bfr[1]  = (uint8_t)(crc16 & 0x00ff);
				EEPROM_Write(eep_user_reg[i].emp_add_crc, 0,bfr,2);
				SendN16DataToDisplay(STRING_POPUP_EN, 2);
				SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"User ID Created!!!");
				TargetPage = 20;
				break;

			}
		}
		if((i==11)&& act_exit)
		{
			SendN16DataToDisplay(STRING_POPUP_EN, 1);
			SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Limit Exceed!!!");
		}
	}
	else
	{
		SendN16DataToDisplay(STRING_POPUP_EN, 1);
		SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"login Admin!!!");
	}

	SendSTRDataToDisplay(ADD_USER_ID_VALUE_WR,(uint8_t*)"\0");
	SendSTRDataToDisplay(ADD_USER_ID_VALUE_RD,(uint8_t*)"\0");
	SendSTRDataToDisplay(ADD_EMP_ID_VALUE_WR,(uint8_t*)"\0");
	SendSTRDataToDisplay(ADD_EMP_ID_VALUE_RD,(uint8_t*)"\0");
	memset((char*)UIAddUserStruct.AddUserName, 0x00, MAX_ID_SIZE);
	//listselection
	SendN16DataToDisplay(LIST_SELECT_WR,2);			//0x080296(new)  0x0800C8(old)
	SendN16DataToDisplay(ADMIN_OR_USER_WR,UIAddUserStruct.select);
	UIUserAccess.list_select=2;

	listall();

}
/*void ClearUIWifiCred()
{
	SendSTRDataToDisplay(WIFI_ADD_WR, (uint8_t*)"\0");
	SendSTRDataToDisplay(WIFI_ADD_RD, (uint8_t*)"\0");
	SendSTRDataToDisplay(WIFI_PW_WR, (uint8_t*)"\0");
	SendSTRDataToDisplay(WIFI_PW_RD, (uint8_t*)"\0");
	SendN16DataToDisplay(WIFI_CONNECT_WR, 0);
}
void DelWifiCred()
{
	memset((char*)comm_par.sy_par.wifi.uuid, 0xFF, sizeof(comm_par.sy_par.wifi.uuid));
	memset((char*)comm_par.sy_par.wifi.pswd, 0xFF, sizeof(comm_par.sy_par.wifi.pswd));
	UIwifi.connect_sts=0;
	EEPROM_Write(wifi_user_add, 0,(uint8_t*)&comm_par.sy_par.wifi.uuid[0],32);
	EEPROM_Write(wifi_pw_add, 0,(uint8_t*)&comm_par.sy_par.wifi.pswd[0],32);
	EEPROM_Write(wifi_connect_sts, 0,(uint8_t*)&UIwifi.connect_sts,1);
	SendN16DataToDisplay(WIFI_CONNECT_WR, UIwifi.connect_sts);
	SendN16DataToDisplay(WIFI_CONNECT_RD, UIwifi.connect_sts);
	ClearUIWifiCred();
}*/

void SaveWifiUserSettings(void)
{
	   if(UIwifi.connect_sts)
	    {
			wifi_info.state=WIFI_CONN_MNGMNT;
			wifi_info.UART1ErrorDetected = 1;
			link_disconnect=1;
			SendSTRDataToDisplay(INTERNET_NAME, (uint8_t*)"\0");
			vp_rt_wr_str.WiFiStatus=0;
			SendN16DataToDisplay(WIFI_SIGNAL_STRENGTH_TOUCH,0);
			SendN16DataToDisplay(WIFI_SIG_STR_KEY_RD_WR,0);
			SendN16DataToDisplay(WIFI_SIG_STR_ICON_WR,0);
		    SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
		    SendN16DataToDisplay(AWS_IOT_CNCTVTY,0);
		    SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD,0);
	    }
	    if(comm_par.sy_par.wifi.active_id && comm_par.sy_par.wifi.active_id<6)
	    {
	    	if(comm_par.sy_par.wifi.active_id>4)
	    	{
	    		comm_par.sy_par.wifi.active_id=4;
	    	}
	    	for(int8_t k=comm_par.sy_par.wifi.active_id; k>0;k--)
	    	{
	    		strncpy((char*)comm_par.sy_par.wifi.Ssid[k], (char*)comm_par.sy_par.wifi.Ssid[k-1], sizeof(comm_par.sy_par.wifi.Ssid[0]));
	    		strncpy((char*)comm_par.sy_par.wifi.Sspswd[k], (char*)comm_par.sy_par.wifi.Sspswd[k-1], sizeof(comm_par.sy_par.wifi.Sspswd[0]));
	    	}
	    }
	    strncpy((char*)comm_par.sy_par.wifi.Ssid[0], (char*)UIwifi.wifi_user, sizeof(comm_par.sy_par.wifi.uuid));
	    strncpy((char*)comm_par.sy_par.wifi.Sspswd[0], (char*)UIwifi.wifi_user_pswd, sizeof(comm_par.sy_par.wifi.pswd));
	    strncpy((char*)comm_par.sy_par.wifi.uuid, (char*)UIwifi.wifi_user, sizeof(comm_par.sy_par.wifi.uuid));
	    strncpy((char*)comm_par.sy_par.wifi.pswd, (char*)UIwifi.wifi_user_pswd, sizeof(comm_par.sy_par.wifi.pswd));
		wifi_data_store();
		//ULOG_DATA("DW3, Wi-Fi Credentials Updated, Wi-Fi SSID:%s",comm_par.sy_par.wifi.uuid);
		comm_par.sy_par.wifi.active_id=0;
}

void wifi_data_store(void)
{
	uint8_t brr[2];
	uint16_t crc_wifi;

		EEPROM_Write(wifi_user_add, 0,(uint8_t*)&comm_par.sy_par.wifi.uuid[0],32);
		crc_wifi = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.uuid[0],32);
		brr[0]= (uint8_t)((crc_wifi & 0xff00) >> 8);
		brr[1]  = (uint8_t)(crc_wifi & 0x00ff);
	    EEPROM_Write(wifi_user_add_crc,0,brr,2);
		EEPROM_Write(wifi_pw_add, 0,(uint8_t*)&comm_par.sy_par.wifi.pswd[0],32);
		crc_wifi = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.pswd[0],32);
		brr[0]= (uint8_t)((crc_wifi & 0xff00) >> 8);
		brr[1]  = (uint8_t)(crc_wifi & 0x00ff);
		EEPROM_Write(wifi_pw_add_crc,0,brr,2);
		EEPROM_Write(Wifi_ssid_add, 0,(uint8_t*)&comm_par.sy_par.wifi.Ssid[0][0],160);
		crc_wifi = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.Ssid[0][0],160);
		brr[0]= (uint8_t)((crc_wifi & 0xff00) >> 8);
		brr[1]  = (uint8_t)(crc_wifi & 0x00ff);
		EEPROM_Write(Wifi_ssid_add_crc,0,brr,2);
		EEPROM_Write(wifi_pass_add, 0,(uint8_t*)&comm_par.sy_par.wifi.Sspswd[0][0],160);
		crc_wifi = usMBCRC16((uint8_t*)&comm_par.sy_par.wifi.Sspswd[0][0],160);
		brr[0]= (uint8_t)((crc_wifi & 0xff00) >> 8);
		brr[1]  = (uint8_t)(crc_wifi & 0x00ff);
		EEPROM_Write(wifi_pass_add_crc,0,brr,2);
		//EEPROM_Write(wifi_connect_sts, 0,(uint8_t*)&UIwifi.connect_sts,1);
}

void LoadWifiUserSettings(void)
{
	memcpy((char*)UIwifi.wifi_user, (char*)comm_par.sy_par.wifi.uuid,  sizeof(comm_par.sy_par.wifi.uuid));
	memcpy((char*)UIwifi.wifi_user_pswd, (char*)comm_par.sy_par.wifi.pswd,  sizeof(comm_par.sy_par.wifi.pswd));
	if (UIwifi.Wifi_password_en==1)
	{
		SendSTRDataToDisplay(WIFI_PW_WR, (uint8_t*)&UIwifi.wifi_user_pswd);
	}
	else
	{
		strncpy((char*)UIwifi.wifi_user_pswd_star,(char*)dummy_star,strlen((char*)UIwifi.wifi_user_pswd));
		SendSTRDataToDisplay(WIFI_PW_WR, (uint8_t*)&UIwifi.wifi_user_pswd_star);
	}
	SendSTRDataToDisplay(WIFI_ADD_WR, (uint8_t*)&comm_par.sy_par.wifi.uuid);
//	SendSTRDataToDisplay(WIFI_PW_WR, (uint8_t*)&comm_par.sy_par.wifi.pswd);
	SendSTRDataToDisplay(WIFI_ADD_RD, (uint8_t*)&comm_par.sy_par.wifi.uuid);
	SendSTRDataToDisplay(WIFI_PW_RD, (uint8_t*)&comm_par.sy_par.wifi.pswd);
	if(comm_par.sy_par.wifi.pswd[0]==0)
	{
		SendN16DataToDisplay(WIFI_PASS_STR_En,0);
	}
	else
	{
		SendN16DataToDisplay(WIFI_PASS_STR_En,1);
	}
	comm_par.sy_par.wifi.active_id=0;
	if(UIwifi.connect_sts==1)
	{
		SendN16DataToDisplay(WIFI_CONNECT_WR, 1);
		SendN16DataToDisplay(WIFI_CONNECT_RD, 1);
	}
	else
	{
		SendSTRDataToDisplay(WIFI_ADD_RD, (uint8_t*)"\0");
		SendSTRDataToDisplay(WIFI_PW_RD, (uint8_t*)"\0");
		SendN16DataToDisplay(WIFI_CONNECT_WR, 0);
		SendN16DataToDisplay(WIFI_CONNECT_RD, 0);
		UIwifi.connect_sts=0;
	}
}




/*
 * *****************************************************************************
 * Function name: HandleAWSFunctions
 * Created by : Venkatesh Arikanti
 * Date:       14-10-2025
 * Description:
 *  Default Key Handling function
 * Parameters:
 *  Input  : Page, Key Id
 *  Return : None
 * Note:
 *
 * *****************************************************************************
 */

void HandleAWSFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
		{
			switch(KeyId)
			{
              case MENU_HOME_KEY:
            	  LoadAWSUserSettings();
				break;

              case MUTE_KEY:
            	  mutebuzzer();
              	break;

              case ENTER_KEY:
            	  SaveAWSuserSettings();
            	  break;

              case CANCEL_KEY:
            	  LoadAWSUserSettings();
            	  break;

              case BACK_KEY:
            	  LoadAWSUserSettings();
            	  break;

              default:
            	  break;
			}
		}
}


void HandleSYSTEMSettingsKeyFunctions(uint8_t CmdCode, uint16_t PageId, uint8_t KeyId)
{
	if((PageId < MAX_PAGE_IDS) || (KeyId < MAX_KEY_IDS))
		{
			switch(KeyId)
			{
              case MENU_HOME_KEY:
            	 // LoadAWSUserSettings();
				break;

              case MUTE_KEY:
            	  mutebuzzer();
              	break;

              case ENTER_KEY:
            	//  SaveAWSuserSettings();
            	  break;

              case CANCEL_KEY:
            	//  LoadAWSUserSettings();
            	  break;

              case BACK_KEY:
            	//  LoadAWSUserSettings();
            	  break;
				case LEVEL_SETTINGS_KEY:		//Firmware_Update Key
					break;
				case DELETION_KEY:		//Factory_Reset Key
					break;
				case VACCUM_KEY:		//File Explorer key
					break;
              default:
            	  break;
			}
		}
}


void SaveAWSuserSettings(void)
{
	uint8_t bfr[2]={0};
	crc16=0;
	memcpy((char*)comm_par.sy_par.iot.iot_url, (char*)UIAws.Aws_url, sizeof(UIAws.Aws_url));
	memcpy((char*)comm_par.sy_par.iot.User_ID, (char*)UIAws.Aws_username, sizeof(UIAws.Aws_username));
//	memcpy((char*)comm_par.sy_par.iot.iot_password, (char*)UIAws.Aws_password, sizeof(UIAws.Aws_password));
	memcpy((char*)comm_par.sy_par.iot.iot_email, (char*)UIAws.Aws_email, sizeof(UIAws.Aws_email));
	memcpy((char*)comm_par.sy_par.iot.iot_phone, (char*)UIAws.Aws_phone, sizeof(UIAws.Aws_phone));
	//memcpy((char*)comm_par.sy_par.iot.iot_token, (char*)UIAws.Aws_token, sizeof(UIAws.Aws_token));
	if(UIwifi.IOT_connect_sts)
	{
		if(vp_rt_wr_str.WiFiStatus!=0)
		{
			wifi_info.state=WIFI_CONN_MNGMNT;
			wifi_info.UART1ErrorDetected=1;
			peer_disconnect=2;
			iot_info.Topic_sel=0;
			SendN16DataToDisplay(AWS_IOT_CNCTVTY_TOUCH,0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY,0);
			SendN16DataToDisplay(AWS_IOT_CNCTVTY_RD,0);
			vp_rt_wr_str.WiFiStatus=2;
		}
		//write down the URL details into SD card.
	}
	EEPROM_Write(Aws_url_address, 0,(uint8_t*)&comm_par.sy_par.iot.iot_url,192);
	EEPROM_Write(Sub_cli_address, 0,(uint8_t*)&comm_par.sy_par.iot.iot_phone,20);
	if(comm_par.sy_par.iot.User_ID[0] == 0)
	{
		memcpy((char*)&comm_par.sy_par.iot.User_ID,(char*)comm_par.sy_par.iot.iot_email,sizeof(comm_par.sy_par.iot.iot_email));
		EEPROM_Write(Aws_user_id, 0,(uint8_t*)&comm_par.sy_par.iot.User_ID,63);
		SendSTRDataToDisplay(AWS_USER_NAME_WR,(uint8_t*)&comm_par.sy_par.iot.User_ID[0]);
		SendSTRDataToDisplay(AWS_USER_NAME_RD,(uint8_t*)&comm_par.sy_par.iot.User_ID[0]);
	}
	else
	{
		EEPROM_Write(Aws_user_id, 0,(uint8_t*)&comm_par.sy_par.iot.User_ID,63);
	}
	//ULOG_DATA("DW4, IoT Credentials Updated, AWS URL:%s",comm_par.sy_par.iot.iot_url);
	System_Status[WARNING_FLAG] &= ~WRN_EMAIL_FAIL;
	System_Status[WARNING_FLAG] &= ~WRN_EMAIL_VALID;
//	crc16 = usMBCRC16((uint8_t*)&comm_par.sy_par.iot.iot_url,200);
//	bfr[0]= (uint8_t)((crc16 & 0xff00) >> 8);
//	bfr[1]  = (uint8_t)(crc16 & 0x00ff);
//	EEPROM_Write(Aws_url_address_crc,0,bfr,2);
}

void LoadAWSUserSettings(void)
{
	memcpy((char*)UIAws.Aws_url,(char*)comm_par.sy_par.iot.iot_url,  sizeof(UIAws));
	SendSTRDataToDisplay(AWS_URL_WR,(uint8_t*)&comm_par.sy_par.iot.iot_url[0]);
	SendSTRDataToDisplay(AWS_USER_NAME_WR,(uint8_t*)&comm_par.sy_par.iot.User_ID[0]);
	SendSTRDataToDisplay(AWS_EMAIL_WR,(uint8_t*)&comm_par.sy_par.iot.iot_email[0]);
	SendSTRDataToDisplay(AWS_PHNO_WR,(uint8_t*)&comm_par.sy_par.iot.iot_phone[0]);
	SendSTRDataToDisplay(AWS_URL_RD,(uint8_t*)&comm_par.sy_par.iot.iot_url[0]);
	SendSTRDataToDisplay(AWS_USER_NAME_RD,(uint8_t*)&comm_par.sy_par.iot.User_ID[0]);
	SendSTRDataToDisplay(AWS_EMAIL_RD,(uint8_t*)&comm_par.sy_par.iot.iot_email[0]);
	SendSTRDataToDisplay(AWS_PHNO_RD,(uint8_t*)&comm_par.sy_par.iot.iot_phone[0]);
}

void ClearAwscredentials()
{
	memset(&comm_par.sy_par.iot.iot_url,  0x00, sizeof(comm_par.sy_par.iot.iot_url));
	SendSTRDataToDisplay(AWS_URL_WR,(uint8_t*)"\0");
//	SendSTRDataToDisplay(AWS_USER_NAME_WR,(uint8_t*)"\0");
//	SendSTRDataToDisplay(AWS_PASSWORD_WR,(uint8_t*)"\0");
	SendSTRDataToDisplay(AWS_EMAIL_WR,(uint8_t*)"\0");
	SendSTRDataToDisplay(AWS_PHNO_WR,(uint8_t*)"\0");
//	SendSTRDataToDisplay(AWS_TOKEN_WR,(uint8_t*)"\0");
	SendSTRDataToDisplay(0x06600,(uint8_t*)"\0");
//	SendSTRDataToDisplay(0x06680,(uint8_t*)"\0");
	SendSTRDataToDisplay(0x06700,(uint8_t*)"\0");
	SendSTRDataToDisplay(0x06780,(uint8_t*)"\0");
}

/*void DelAwscredentials()
{
	memset(&comm_par.sy_par.iot.iot_url,  0xFF, sizeof(comm_par.sy_par.iot.iot_url));
	EEPROM_Write(Aws_url_address, 0,(uint8_t*)&comm_par.sy_par.iot.iot_url[0],256);
	ClearAwscredentials();
}*/


int isValidInput(const char *str)
{
    int len = strlen(str);
    // Check for double quotes anywhere in the string
    for (int i = 0; i < len; i++) {
        if ((str[i] == '"') || str[i]<0x20 || str[i]>0x7E)
        {
            return 0;
        }
        else if(str[i] == ',')
        	return 0;
    }

    return 1;
}

int isValidInputpassword(const char *str)
{
    int len = strlen(str);

    // Check for leading or trailing spaces
    if (isspace((unsigned char)str[0]) || isspace((unsigned char)str[len - 1]))
    {
        return 0;
    }

    // Check for double quotes anywhere in the string
    for (int i = 0; i < len; i++) {
        if ((str[i] == '"') || str[i]<0x20 || str[i]>0x7E)
        {
            return 0;
        }
        else if(str[i] == ',')
        	return 0;
    }

    return 1;
}




