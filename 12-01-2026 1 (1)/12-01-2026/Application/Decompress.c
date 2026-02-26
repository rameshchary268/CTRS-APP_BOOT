/*
 * Decompress.c
 *
 *  Created on: Mar 22, 2024
 *      Author: venkatesh.arikanti
 */

//#include "ff.h"
//#include "SD_card.h"
#include "DisplayCmdHandlers.h"
#include "ADC.h"
#include "displaycommdriver.h"


extern FRESULT fresult;
extern FILINFO file_info;
extern TCHAR   sdpath[16];


//SD_DEC_CIR_BUF Sd_dec_cir_buffer;
FIL SDFile_comp;       /* File object for SD */
FIL COMP_FILE;          /* File object for SD */
DECOMP_COMP Comp_variables;
char FolderNameHeader[11] = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
char FileNameHeader[11] = {0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02};
char FileDataHeader[11] = {0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03};
char FileDataDelimiter[11] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04};
char FolderDataDelimiter[11] = {0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05};
uint16_t calcrc = 0x1D0F;
unsigned int com_bytes_write=0;
char output[DATA_DECOMPRESS_SIZE+0x100]={0};
char temp_folder[25]={0};

void decompress_rle(const char* input, uint16_t size)
{
    int input_length = size;
    int output_index = 0;
    if(size==0)
    {
    	return;
    }
    calcrc = crc16_ccitt_bin(input, size, calcrc);
    //memset(output,0,sizeof(output));
    // Loop through the input string
    for (int i = 0; i < input_length; i += 3) {
    	HAL_IWDG_Refresh(&hiwdg);
        // Get the count and character from the input string
        int count = input[i+2];
        //printf("%d\n",count);
        char character = input[i];
        char character1=input[i+1];
        //printf("%c\n",character);

        // Repeat the character count times in the output string
        for (int j = 0; j < count; j++) {
            output[output_index++] = character;
            output[output_index++] = character1;
        }
        if(output_index>0xF00)
        {
        	 //output[output_index] = '\0';
        	fresult = f_write (&SDFile_comp, &output, output_index, &com_bytes_write);
        	Set_delay(SW_TMR_USB_SD_POLL, DLY_USB_WR_POLL, &Comp_variables.file_poll_flag);
        	memset(output,0,sizeof(output));
        	output_index=0;
        }
    }

    // Null-terminate the output string
    //output[output_index] = '\0';
    char temp_data_file[]="0:/THMT/luamain.lua";
    char temp_private_file[]="0:/Private.key";
    if(((strcmp((char*)temp_folder,(char*)temp_data_file)==0)||(strcmp((char*)temp_folder,(char*)temp_private_file)==0)) &&(Comp_variables.compress_en==0)&& output[output_index =='\0'])
    {
    	output_index=output_index-1;
    }
    fresult = f_write (&SDFile_comp, &output, output_index, &com_bytes_write);
    Set_delay(SW_TMR_USB_SD_POLL, DLY_USB_WR_POLL, &Comp_variables.file_poll_flag);
    memset(output,0,sizeof(output));
    HAL_IWDG_Refresh(&hiwdg);
}


uint8_t file_detect(char* file_name)
{
	unsigned int com_bytes_read=0;
	char temp_data[11]={0};
	char temp_read_data[100]={0};
	uint16_t remain_size=0;
	Comp_variables.file_poll_flag=1;
	DIR new_dir={0};
	int p=0;
	uint8_t times=0;
	uint8_t data_count=0;
	Comp_variables.compress_en=0;
	System_Status[EVENT_FLAG] &= ~EVT_DECOMPRE_FAIL;
	UARTTxCompleted = 1;
	if((SD_info.state==SD_DONE ))
	{
		fresult=f_stat (file_name, &file_info);
		if(fresult==FR_OK)
		{
			HAL_IWDG_Refresh(&hiwdg);
			uint16_t StartCrc = 0x1D0F;

			if((fresult = f_open(&COMP_FILE, file_name,FA_READ))==FR_OK)
			{
				Comp_variables.comp_size=COMP_FILE.obj.objsize;
				if(COMP_FILE.obj.objsize<=256)
				{
					if(SD_PO(file_name))
					{
						SendN16DataToDisplay(STRING_POPUP_EN, 1);
						SendSTRDataToDisplay(POPUP_STRING_DATA,(uint8_t*)"Invalid firmware update file!!!");
					}
					fresult = f_close(&COMP_FILE);
					f_unlink(file_name);
					TargetPage =0;
					SendCmdToDisplay(LOAD_PAGE_CMD);
					return 1;
				}
				com_bytes_read=0;
				Turn_off_solenoids();
				TargetPage =62;
				SendCmdToDisplay(LOAD_PAGE_CMD);
				//SendN16DataToDisplay(0x080422, 1);
				UARTTxCompleted = 1;
				LCDDataProc();
				do{
					#ifdef EN_IWDG
					HAL_IWDG_Refresh(&hiwdg);
					#endif
					if(Comp_variables.comp_size< DATA_DECOMPRESS_SIZE)
					{
						Comp_variables.read_data_size=Comp_variables.comp_size;
					}
					else
					{
						Comp_variables.read_data_size= DATA_DECOMPRESS_SIZE;
					}
					if(Comp_variables.file_poll_flag)
					{
						fresult = f_read (&COMP_FILE, &Comp_variables.decompress_data[data_count], Comp_variables.read_data_size, &com_bytes_read);
						Set_delay(SW_TMR_USB_SD_POLL, DLY_USB_WR_POLL, &Comp_variables.file_poll_flag);
						if((fresult==FR_OK) && (com_bytes_read > 0))
						{
							StartCrc = crc16_ccitt_bin(&Comp_variables.decompress_data[data_count], (com_bytes_read), StartCrc);
						}
						times++;
					}
					else
					{
						Comp_variables.read_data_size=0;
						com_bytes_read=0;
					}
					if((fresult==FR_OK) && com_bytes_read)
					{
						times=0;
						int i=0,j=0;
						for(i=0;i<(Comp_variables.read_data_size+data_count);i++)
						{
							HAL_IWDG_Refresh(&hiwdg);
							if((Comp_variables.comp_size > DATA_DECOMPRESS_SIZE) && (i>((Comp_variables.read_data_size+data_count)-60)) && ((p%3)==0))
							{
								memcpy(temp_read_data,&Comp_variables.decompress_data[i],((Comp_variables.read_data_size+data_count)-i));
								remain_size=Comp_variables.read_data_size+data_count-i;
								break;
							}
							if((Comp_variables.decompress_data[i]==Comp_variables.decompress_data[i+1]) &&(Comp_variables.decompress_data[i+1]==Comp_variables.decompress_data[i+2])&&(Comp_variables.decompress_data[i+2]==Comp_variables.decompress_data[i+3])&&\
									((Comp_variables.decompress_data[i]==0x01)||(Comp_variables.decompress_data[i]==0x02)||(Comp_variables.decompress_data[i]==0x03)||(Comp_variables.decompress_data[i]==0x04)||(Comp_variables.decompress_data[i]==0x05))&&\
									((p==0) || (p%3==2)))
							{
								memset(temp_data,0,sizeof(temp_data));
								for(j=0;j<10;j++)
								{
									temp_data[j]=Comp_variables.decompress_data[i+j];
								}
								if(strcmp(temp_data,FolderNameHeader)==0)
								{
									i=i+10;
									memset(temp_folder,0,sizeof(temp_folder));
									memcpy(temp_folder,sdpath,4);
									memcpy(&temp_folder[3],&Comp_variables.decompress_data[i],4);
									i=i+3;

									 fresult=f_mkdir(temp_folder);
									 fresult = f_opendir(&new_dir, temp_folder);
									 if (fresult == FR_OK)
									 {
										 fresult=f_readdir(&new_dir, &file_info);
									 }
									 f_closedir(&new_dir);
								}
								else if(strcmp(temp_data,FileNameHeader)==0)
								{
									i=i+10;
									int k=0;
									f_close(&SDFile_comp);
									memset(temp_folder,0,sizeof(temp_folder));
									memcpy(temp_folder,sdpath,4);
									for(k=0;((Comp_variables.decompress_data[i+k]!=Comp_variables.decompress_data[i+k+1])||(Comp_variables.decompress_data[i+k+1]!=Comp_variables.decompress_data[i+k+2])||(Comp_variables.decompress_data[i+k+2]!=Comp_variables.decompress_data[i+k+3])||(Comp_variables.decompress_data[i+k+4]!=Comp_variables.decompress_data[i+k+3]));k++)
									{
										temp_folder[k+3]=Comp_variables.decompress_data[i+k];
									}
									i=i+k-1;
									fresult=f_unlink(temp_folder);
									if((fresult = f_open(&SDFile_comp, temp_folder,FA_CREATE_NEW|FA_OPEN_APPEND|FA_WRITE))!=FR_OK)
									{
										f_close(&SDFile_comp);
									}
									calcrc = 0x1D0F;
									//f_close(&SDFile_comp);
								}
								else if(strcmp(temp_data,FileDataHeader)==0)
								{
									i=i+9;
									Comp_variables.compress_en=1;
								}
								else if((strcmp(temp_data,FileDataDelimiter)==0) && Comp_variables.compress_en )
								{
									i=i+9;
									Comp_variables.compress_en=0;
									if(p)
									{
										decompress_rle(Comp_variables.compress_data,p);
										memset(Comp_variables.compress_data,0,sizeof(Comp_variables.compress_data));
									}
									f_close(&SDFile_comp);

									//Comp_variables.compress_en=0;
									p=0;
									if(calcrc!=0)
									{
										System_Status[EVENT_FLAG] |= EVT_DECOMPRE_FAIL;
										f_unlink(temp_folder);
										memset(&temp_folder[3],0,20);
										break;
									}
									memset(&temp_folder[3],0,20);
								}
								else if(strcmp(temp_data,FolderDataDelimiter)==0)
								{
									i=i+9;
									Comp_variables.compress_en=0;
								}
								else if(Comp_variables.compress_en)
								{
									Comp_variables.compress_data[p]=Comp_variables.decompress_data[i];
									p++;
									if(p==0x7FE)
									{

										decompress_rle(Comp_variables.compress_data,p);
										memset(Comp_variables.compress_data,0,sizeof(Comp_variables.compress_data));
										p=0;
									}
								}
							}
							else if(Comp_variables.compress_en)
							{
								Comp_variables.compress_data[p]=Comp_variables.decompress_data[i];
								p++;
								if(p==0x7FE)
								{
									decompress_rle(Comp_variables.compress_data,p);
									memset(Comp_variables.compress_data,0,sizeof(Comp_variables.compress_data));
//									f_close(&SDFile_comp);
									p=0;
								}

							}
						}
					}
					else if(times>5)
					{
						f_unlink("0:/BOOT.bin");
						f_unlink("0:/CFC4_20.bin");
						f_unlink("0:/CFC.bin");
						Delete_file("THMT");
						Delete_file("Font");
						break;
					}

					if(remain_size && com_bytes_read )
					{
//						if((fresult = f_open(&SDFile_comp, temp_folder,FA_OPEN_APPEND|FA_WRITE))!=FR_OK)
//						{
//							f_close(&SDFile_comp);
//						}

						if(p && (Comp_variables.compress_en) && p%3==0)
						{
							decompress_rle(Comp_variables.compress_data,p);
							memset(Comp_variables.compress_data,0,sizeof(Comp_variables.compress_data));
	//						f_close(&SDFile_comp);
							//Comp_variables.compress_en;
							p=0;
						}

//						if(remain_size)
//						{
							memcpy(Comp_variables.decompress_data,temp_read_data,remain_size);
//						}
						memset(temp_read_data,0,sizeof(temp_read_data));
						data_count=remain_size;
						//p++;
					}
					Comp_variables.comp_size-=com_bytes_read;
					if(System_Status[EVENT_FLAG] & EVT_DECOMPRE_FAIL)
					{
						f_unlink("0:/BOOT.bin");
						f_unlink("0:/CFC.bin");
						f_unlink("0:/CFC4_20.bin");
						Delete_file("THMT");
						Delete_file("Font");
						break;
					}
				}while(Comp_variables.comp_size>0);
				fresult=f_close(&SDFile_comp);
				memset(Comp_variables.compress_data,0,sizeof(Comp_variables.compress_data));
				Comp_variables.compress_en=0;
				memset(temp_folder,0,sizeof(temp_folder));
				p=0;
				if(StartCrc!=0)
				{
					f_unlink("0:/BOOT.bin");
					f_unlink("0:/CFC.bin");
					f_unlink("0:/CFC4_20.bin");
					Delete_file("THMT");
					Delete_file("Font");
				}
				TargetPage =0;
				SendCmdToDisplay(LOAD_PAGE_CMD);
			}
			fresult = f_close(&COMP_FILE);
			f_unlink(file_name);

		}
		System_Status[EVENT_FLAG] &= ~EVT_DECOMPRE_FAIL;
	}
	return 0;
}


void Delete_file(char* pat)
{
    DIR Deldir={0};
    UINT i=0;
    char path[10]={0};
    sprintf (path, "0:/%s",pat);
    char temp_file[25]={0};
    uint8_t Data_len=0;

    fresult = f_opendir(&Deldir, path);                       /* Open the directory */
    if (fresult == FR_OK)
    {
        for (i=0;i<500;i++)
        {
            fresult = f_readdir(&Deldir, &file_info);                   /* Read a directory item */
            if (fresult != FR_OK || file_info.fname[0] == 0)
            	break;  /* Break on error or end of dir */
               Data_len=snprintf(temp_file,25,"0:/%s", pat);
               snprintf(temp_file+Data_len,25-Data_len,"/%s",file_info.fname);
               f_unlink(temp_file);
               memset(temp_file,0,sizeof(temp_file));
        }
    }
    f_unlink(path);
    f_closedir(&Deldir);
    return ;
}
