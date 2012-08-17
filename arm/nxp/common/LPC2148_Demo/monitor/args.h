//
//  $Id: args.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/monitor/args.h $
//

#ifndef _ARGS_H_
#define _ARGS_H_

int argsGetLine (int fd, U8 *buffer, int bufferLength, const char *prompt);
int argsParse (char *cmd, char **argv, int sizeofArgv, int *argc);

#endif
