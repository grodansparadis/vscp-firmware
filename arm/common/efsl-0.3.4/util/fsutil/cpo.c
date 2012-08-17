#include <stdio.h>
#include <stdlib.h>
#include "efs.h"
#include "linuxfile.h"

int main(int argc, char** argv)
{
	linuxFileInterface *lfi=Malloc(sizeof(*lfi));
	File *file=Malloc(sizeof(*file));
		
	EFSL_Storage_Config *sconf=Malloc(sizeof(*sconf));
	EFSL_Filesystem_Config *fconf=Malloc(sizeof(fconf));
	EFSL_Storage *storage=Malloc(sizeof(*storage));
	EFSL_Filesystem *filesystem=Malloc(sizeof(*filesystem));
	
	unsigned short e;
	unsigned short bufsize;
	char *buf;
	FILE *localfile;
	
	if(argc<4){
		fprintf(stderr,"Argument error : cpo <fs> <file_read> <local_write> [bufsize]\n");
		exit(-1);
	}

	if(argc==5)
		bufsize=atoi(argv[4]);
	else
		bufsize=4096;
	buf=malloc(bufsize);

	lfi->fileName=argv[1];
	
	sconf->hwObject=lfi;
	sconf->if_init_fptr=lf_init;
	sconf->if_read_fptr=lf_readBuf;
	sconf->if_write_fptr=lf_writeBuf;
	sconf->if_ioctl_fptr=lf_ioctl;
	sconf->ioman_bufmem=buf;
	
	fconf->no_partitions=1;
	fconf->storage=storage;
	
	if(EFSL_Storage_Init(storage,sconf)){
		printf("Error initialising storage\n");
		exit(-1);
	}
	if(EFSL_Filesystem_Init(filesystem,fconf)){
		printf("Unable to mount filesystem\n");
		exit(-1);
	}
		
	if(file_fopen(file,&(filesystem->filesystem),argv[2],'r')!=0){
		printf("Could not open file.\n");
		return(-2);
	}

	if(!(localfile=fopen(argv[3],"w"))){
		printf("Could not open local file.\n");
		return(-3);
	}
	
	while((e=file_read(file,bufsize,buf))){
		fwrite(buf,1,e,localfile);
	}

	file_fclose(file);
	fclose(localfile);
	fs_umount(&(filesystem->filesystem));

	return(0);
}
