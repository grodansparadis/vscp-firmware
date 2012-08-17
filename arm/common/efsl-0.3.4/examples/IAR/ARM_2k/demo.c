/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  demo.c                                                          *
* Description : This file demonstrates several routines of EFSL               *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                    (c)2005 Joel Winarske    *
******************************************************************************/

#include <efs.h>
#include <ls.h>
#include <sd.h>

#include <arm2k_ssp.h>
#include <arm2k_pll.h>
#include <arm2k_uart.h>
#include <arm2k_rtc.h>

#define CREATE_SUBS   0
#define DELETE        0
#define SHOW_DIR      1
#define WRITE         0
#define DISPLAY       0

void hang(void);

void main(void)
{
	EFSL_Storage_Config sconf;
	EFSL_Filesystem_Config fconf;
	EFSL_Storage storage;
	EFSL_Filesystem filesystem;
#if (DISPLAY==1)
	File file_r;
#endif
#if (WRITE==1)
        File file_w;
#endif
        DirList list;

	SdSpiProtocol ssp;

        euint8 hour,min,sec,month,day;
        euint16 year;

#if (DISPLAY==1)
	euint8 buf[512];
#endif
        unsigned short i, e;
		
        PLL0_Init();               /* Init PLL */
        UART1_Init(115200);        /* Init UART1 */
        RTC_Init();                /* Init RTC */

        RTC_GetTimeDate(&hour, &min, &sec, &day, &month, &year);

	DBG((TXT("EFSL Demo - IAR LPC214x\n\r")));
        DBG((TXT("%u:%02u:%02u  %u\\%u\\%u\n\r\n\r"),hour,min,sec,month,day,year));

        /* Init library */
	ssp.spiHwInterface=NULL;
        ssp.spiHwInit=(euint8(*)(void*))&arm2k_ssp_init;
	ssp.spiSendByte=(euint8(*)(void*,euint8))&arm2k_ssp_send;

        sconf.hwObject=&ssp;
        sconf.if_init_fptr=(esint8(*)(void*))&sd_Init;
        sconf.if_read_fptr=(esint8(*)(void*,euint32,unsigned char*))&sd_readSector;
        sconf.if_write_fptr=(esint8(*)(void*,euint32,unsigned char*))&sd_writeSector;
        sconf.if_ioctl_fptr=(esint8(*)(void*,euint16,void*))&sd_ioctl;
        sconf.ioman_bufmem=NULL;

	fconf.no_partitions=0;
	fconf.storage=&storage;
        /* End Init library */
	
	if(EFSL_Storage_Init(&storage,&sconf)){
            DBG((TXT("Error initializing storage\n")));
            hang();
	}
	
	if(EFSL_Filesystem_Init(&filesystem,&fconf)){
            DBG((TXT("Unable to mount fs\n")));
            hang();
	}

        /***********************************************************************/
        /* Show Directory                                                      */
        /***********************************************************************/

        DBG((TXT("Directory of \\\n\r")));

        /* Open root dir */
        ls_openDir(&list,&filesystem.filesystem,"/");

        /* Print a list of all files and their filesize */
        while(ls_getNext(&list)==0) {
          DBG((TXT("%s (%li bytes)\n\r"),
               list.currentEntry.FileName,
               list.currentEntry.FileSize)) ;
        }

        /***********************************************************************/
        /* Create Subdirectories                                               */
        /***********************************************************************/

#if (CREATE_SUBS==1)

        mkdir(&filesystem.filesystem,"dir1";
#endif

        /***********************************************************************/
        /* Delete File                                                         */
        /***********************************************************************/

#if (DELETE==1)

        if(rmfile(&filesystem.filesystem,"test.txt")!=0) {
            DBG((TXT("Could not delete file\n\r")));
            hang();
	}
        DBG((TXT("Delete File sucessfull\n\r")));
#endif

        /***********************************************************************/
        /* Show Directory                                                      */
        /***********************************************************************/

#if (SHOW_DIR==1)

        DBG((TXT("Directory of \\\n\r")));

        /* Open root dir */
        ls_openDir(&list,&filesystem.filesystem,"/");

        /* Print a list of all files and their filesize */
        while(ls_getNext(&list)==0) {
          DBG((TXT("%s (%li bytes)\n\r"),
               list.currentEntry.FileName,
               list.currentEntry.FileSize)) ;
        }
#endif

        /***********************************************************************/
        /* Write File Contents                                                 */
        /***********************************************************************/

#if (WRITE==1)

        if(file_fopen(&file_w,&filesystem.filesystem,"test4.txt",'w')!=0) {
            DBG((TXT("Could not open file\n\r")));
            hang();
	}
        DBG((TXT("File opened for writing\n\r")));

        char const *test="This is a test\r\n\0";
            file_write(&file_w,strlen(test),(euint8*)test);

        euint8 *val;
        for(e=0;e<500;e++) {
            for(i=0;i<10;i++) {
                val = itoa(e);
                file_write(&file_w,strlen((char const*)val),val);
                file_putc(&file_w,";");
            }
            file_putc(&file_w,"\r");
            file_putc(&file_w,"\n");
        }

        file_fclose(&file_w);
#endif

        /***********************************************************************/
        /* Display File Contents                                               */
        /***********************************************************************/

#if (DISPLAY==1)

        if(file_fopen(&file_r,&filesystem.filesystem,"test4.txt",'r')!=0) {
            DBG((TXT("Could not open file\n\r")));
            hang();
	}
        DBG((TXT("File opened for reading\n\r")));

        while((e=file_read(&file_r,512,buf))) {
            for(i=0;i<e;i++){
                DBG((TXT("%c"),buf[i]));
            }
	}

        file_fclose(&file_r);
#endif

        /***********************************************************************/
        /* Clean up                                                            */
        /***********************************************************************/

        fs_umount(&filesystem.filesystem);

        RTC_GetTimeDate(&hour, &min, &sec, &day, &month, &year);
        DBG((TXT("%u:%02u:%02u  %u\\%u\\%u\n\r"),hour,min,sec,month,day,year));

        DBG((TXT("Done :-)")));


	hang();
}

void hang(void)
{
	while(1);
}
