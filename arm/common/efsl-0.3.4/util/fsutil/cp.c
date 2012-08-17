#include <stdio.h>
#include <stdlib.h>
#include "efs.h"
#include "linuxfile.h"
#include "include/init.h"

int main(int argc, char** argv)
{
	FILE *in,*out;
	EFSL_Storage *storage=Malloc(sizeof(*storage));
	EFSL_Filesystem *filesystem=Malloc(sizeof(*filesystem));
	
	Init_EFSL(&storage,0,&filesystem,0,&in,&out,argc,argv);
	return(0);
}
