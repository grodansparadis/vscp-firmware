/*
 * Copyright ARM Limited 1999 - 2000.  All rights reserved.
 *
 * ARM impemented unzip which is less rigorous in checking the header. This
 * allows gzip or even simple zip files to be used. validZipID just checks
 * the first few bytes to make sure the magic numbers match a know type.
 */

#ifndef _ARM_ZLIB_H
#define _ARM_ZLIB_H

#include "zconf.h"

#ifdef __cplusplus
extern "C" {
#endif

ZEXTERN int ZEXPORT validZipID OF((const Bytef *source));

ZEXTERN int ZEXPORT unzip OF((Bytef *dest,   uLongf *destLen,
                                   const Bytef *source, uLong sourceLen));

#ifdef __cplusplus
}
#endif

#endif /* _ARM_ZLIB_H */

