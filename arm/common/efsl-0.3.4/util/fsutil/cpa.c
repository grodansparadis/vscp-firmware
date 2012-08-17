#include <stdio.h>
#include <stdlib.h>
#include "efs.h"

int main(int argc, char** argv)
{
	EmbeddedFileSystem efs;
	EmbeddedFile file;	
	unsigned short e;
	unsigned short bufsize;
	signed short ret;
	char *buf;
	FILE *localfile;
	
	if(argc<4){
		fprintf(stderr,"Argument error : cpi <fs> <local_read> <file_write> [bufsize]\n");
		exit(-1);
	}

	if(argc==5)
		bufsize=atoi(argv[4]);
	else
		bufsize=4096;
	buf=malloc(bufsize);
	
	if(efs_init(&efs,argv[1])!=0){
		printf("Could not open filesystem.\n");
		return(-1);
	}
	
	ret=file_fopen(&file,&efs.myFs,argv[3],'a');
	if((ret!=0)){
		printf("Could not open file %s (return value: %d).\n",argv[3],ret);
		return(-2);
	}

	if(!(localfile=fopen(argv[2],"r"))){
		printf("Could not open local file.\n");
		return(-3);
	}
	
	while((e=fread(buf,1,bufsize,localfile))){
		file_write(&file,e,buf);
	}

	file_fclose(&file);
	fclose(localfile);
	fs_umount(&(efs.myFs));

	return(0);
}
