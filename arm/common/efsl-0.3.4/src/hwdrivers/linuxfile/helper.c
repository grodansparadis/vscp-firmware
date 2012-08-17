/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : helper.c                                                         *
* Description : These functions may NOT BE USED ANYWHERE ! They are helper    *
*               functions for the Linux based developement. They use the GNU  *
*               C library and headers.                                        *
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
*                                                    (c)2004 Lennart Yseboodt *
*                                                    (c)2005 Michael De Nil   *
\*****************************************************************************/

#include "helper.h"

void* Malloc(eint32 size)
{
	void*x;
	
	if((x=malloc(size))==NULL){
		perror("Malloc: ");
		exit(-1);
	}
	return(x);
}
 
void Fopen(FILE **f,eint8* filename)
{
	*f=fopen(filename,"r+");
	if(*f==NULL){
		perror("Fopen: ");
		exit(-1);
	}
}

void MMap(eint8* filename,void**x,eint32 *size)
{
	FILE *tmp;
	eint32 filesize,c;
	
	Fopen(&tmp,filename);
	filesize=getFileSize(tmp);
	*x=Malloc(filesize);
	for(c=0;c<filesize;c++)*((char*)(*x)+c)=fgetc(tmp);
	*size=filesize;
	fclose(tmp);
}

int getFileSize(FILE* file)
{
	eint32 c=0;
	
	fseek(file,0,SEEK_END);
	c=ftell(file);
	return(c);
}

void PrintBuf(euint8* buf)
{
	euint16 c,cc;
	
	for(c=0 ; c<32 ; c++){
			printf("\n%4x : ",c*16);
		for(cc=0;cc<16;cc++){
			printf("%2x ",buf[c*16+cc]);
		}
		printf("   ");
		for(cc=0;cc<16;cc++){
			printf("%c",buf[c*16+cc]>=32?buf[c*16+cc]:'*');
		}
	}
	printf("\n");
}
