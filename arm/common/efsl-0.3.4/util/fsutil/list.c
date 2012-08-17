#include <stdio.h>
#include <stdlib.h>
#include "efs.h"
#include "ls.h"

int main(int argc, char** argv)
{
	EmbeddedFileSystem efs;
	DirList *list;
	
	if(argc<3){
		fprintf(stderr,"Argument error : list <fs> <directory>\n");
		exit(-1);
	}

	if(efs_init(&efs,argv[1])!=0){
		printf("Could not open filesystem.\n");
		return(-1);
	}
	
	list = malloc(sizeof(*list));

	if((ls_openDir(list,&(efs.myFs),argv[2]))!=0){
	    printf("Unable to open directory for listing\n");
	    exit(-1);
	}

	while(ls_getNext(list)==0){
	    printf("%s\n",list->currentEntry.FileName);
	}
	
	fs_umount(&(efs.myFs));

	return(0);
}
