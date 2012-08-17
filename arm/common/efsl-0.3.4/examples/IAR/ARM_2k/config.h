#ifndef __EFSL_CONFIG_H__
#define __EFSL_CONFIG_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HW_ENDPOINT_IAR_ARM_2K

#define MULTIPLE_INTERFACE_SUPPORT
#define VARSIZE_ARM_2K

//#define BYTE_ALIGNMENT

#define IOMAN_NUMBUFFER 4     //1 per filesystem, 2 buffers per file - 6 Max
#define IOMAN_NUMITERATIONS 3 // if more than 4 buffer, decrease to 2 or 1 for mem gain
#define IOMAN_DO_MEMALLOC

#define CLUSTER_PREALLOC_FILE 2
#define CLUSTER_PREALLOC_DIRECTORY 0

#define HOST_LITTLE_ENDIAN
#define DATE_TIME_SUPPORT

#define FULL_ERROR_SUPPORT
#define BASE_ERROR_SUPPORT

#define LIST_MAXLENFILENAME 12

#define DEBUG
#define DEBUG_PORT 1

#endif
