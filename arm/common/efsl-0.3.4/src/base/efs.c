/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : efs.h                                                            *
* Description : This should become the wrapper around efs. It will contain    *
*               functions like efs_init etc.                                  *
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
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Lennart Yseboodt *
******************************************************************************/

/*****************************************************************************/
#include "efs.h"
/*****************************************************************************/

esint8 EFSL_Storage_Init(EFSL_Storage *efsl_storage,EFSL_Storage_Config *config)
{
	if(
		if_init(&(efsl_storage->interface),
				config->hwObject,
				config->if_init_fptr,
				config->if_read_fptr,
				config->if_write_fptr,
				config->if_ioctl_fptr)){
			return(-1);
	}
	if(
		ioman_init(&(efsl_storage->ioman),&(efsl_storage->interface),config->ioman_bufmem)){
			return(-1);
	}
	disc_initDisc(&(efsl_storage->disc),&(efsl_storage->ioman));
	
	return(0);
}

esint8 EFSL_Filesystem_Init(EFSL_Filesystem *efsl_filesystem,EFSL_Filesystem_Config *config)
{
	efsl_filesystem->storage=config->storage;
	
	if(config->no_partitions){
		efsl_filesystem->storage->disc.partitions[0].type=0x0B;
		efsl_filesystem->storage->disc.partitions[0].LBA_begin=0;
		efsl_filesystem->storage->disc.partitions[0].numSectors=
			efsl_filesystem->storage->interface.sectorCount;
	}
		part_initPartition(&(efsl_filesystem->partition),&(efsl_filesystem->storage->disc));
	if(
		fs_initFs(&(efsl_filesystem->filesystem),&(efsl_filesystem->partition))){
			return(-1);
	}
	
	return(0);
}


