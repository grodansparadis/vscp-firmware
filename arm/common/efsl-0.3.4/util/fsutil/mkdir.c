#include <stdio.h>
#include <stdlib.h>
#include "efs.h"
#include "ui.h"

int main(int argc, char **argv)
{
    EmbeddedFileSystem efs;

    if(argc<3){
	fprintf(stderr,"Argument error : mkdir <fs> <dirname>\n");
	exit(-1);
    }
    
    if(efs_init(&efs,argv[1])!=0){
	printf("Could not open filesystem.\n");
	return(-1);
    }
    
    if(!mkdir(&(efs.myFs),argv[2])){
	printf("Unable to make directory\n");
	fs_umount(&(efs.myFs));
	exit(-1);
    }
    fs_umount(&(efs.myFs));
    return(0);
}
