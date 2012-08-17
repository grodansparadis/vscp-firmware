/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  fat.h                                                           *
* Description : Headerfile for fat.c The files are an extend of the fs.c fs.h *
*               pair.                                                         *
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
******************************************************************************/

#ifndef __FAT_H_
#define __FAT_H_

/*****************************************************************************/
#include "config.h"
#include "error.h"
#include "file.h"
#include "debug.h"
#include "types.h"
/*****************************************************************************/

euint32 fat_getSectorAddressFatEntry(FileSystem *fs,euint32 cluster_addr);
euint32 fat_getNextClusterAddress(FileSystem *fs,	euint32 cluster_addr, euint16 *linear);
void fat_setNextClusterAddress(FileSystem *fs,euint32 cluster_addr,euint32 next_cluster_addr);
eint16 fat_isEocMarker(FileSystem *fs,euint32 fat_entry);
euint32 fat_giveEocMarker(FileSystem *fs);
euint32 fat_findClusterAddress(FileSystem *fs,euint32 cluster,euint32 offset, euint8 *linear);
euint32 fat_getNextClusterAddressWBuf(FileSystem *fs,euint32 cluster_addr, euint8 * buf);
void fat_setNextClusterAddressWBuf(FileSystem *fs,euint32 cluster_addr,euint32 next_cluster_addr,euint8 * buf);
esint16 fat_getNextClusterChain(FileSystem *fs, ClusterChain *Cache);
void fat_bogus(void);
esint16 fat_LogicToDiscCluster(FileSystem *fs, ClusterChain *Cache,euint32 logiccluster);
eint16 fat_allocClusterChain(FileSystem *fs,ClusterChain *Cache,euint32 num_clusters);
eint16 fat_unlinkClusterChain(FileSystem *fs,ClusterChain *Cache);
euint32 fat_countClustersInChain(FileSystem *fs,euint32 firstcluster);
euint32 fat_DiscToLogicCluster(FileSystem *fs,euint32 firstcluster,euint32 disccluster);

#endif
