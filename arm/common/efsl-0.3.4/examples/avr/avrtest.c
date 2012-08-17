#include <efs.h>
#include <sd.h>
#include <atmega_spi.h>

void hang(void);

void main(void)
{
	EFSL_Storage_Config sconf;
	EFSL_Filesystem_Config fconf;
	EFSL_Storage storage;
	EFSL_Filesystem filesystem;
	File file_r;

	atmegaSpiInterface asi;
	SdSpiProtocol ssp;
	
	char buf[512];
	unsigned short i,e;
		
	debug_init();

	DBG((TXT("\nHello :-)\n")));
	
	/* Init */
	asi.pinSelect=0x01;
	
	ssp.spiHwInterface=&asi;
	ssp.spiHwInit=(void *)atmega_spi_init;
	ssp.spiSendByte=(void *)atmega_spi_send;

    sconf.hwObject=&ssp;
    sconf.if_init_fptr=(void *)sd_Init;
    sconf.if_read_fptr=(void *)sd_readSector;
    sconf.if_write_fptr=(void *)sd_writeSector;
    sconf.if_ioctl_fptr=(void *)sd_ioctl;
    sconf.ioman_bufmem=0;

	fconf.no_partitions=0;
	fconf.storage=&storage;
	
	DBG((TXT("Let's go...\n")));
	
	if(EFSL_Storage_Init(&storage,&sconf)){
		DBG((TXT("Error initializing storage: %d")));
		hang();
	}
	
	if(EFSL_Filesystem_Init(&filesystem,&fconf)){
		DBG((TXT("Unable to mount fs")));
		hang();
	}

	if(file_fopen(&file_r,&filesystem.filesystem,"orig.txt",'r')!=0){
		DBG((TXT("Could not open file\n")));
		hang();
	}
	DBG((TXT("File opened")));

	while((e=file_read(&file_r,512,buf))){
		for(i=0;i<e;i++){
			DBG((TXT("%c"),buf[i]));
		}
	}

	DBG((TXT("Done :-)")));
	
	hang();
}

void hang(void)
{
	while((1))
		_NOP();
}
