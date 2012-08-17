#  
#  Makefile --
# 
# 	This is the Makefile for nslib, a library of functions
#  	for the ARM7500.  See the file README for installation
#  	instructions.
# 
#  Copyright (c) 1996 Ben Williamson.
#  All rights reserved.
# 
#  This file is part of nslib, a library used by programs
#  running on the Netslate Development Board.
# 
#  This software is released under the GNU Public License.
#  See the file COPYING for more information.
# 

PREFIX =		/usr/local
CROSS =			arm-semi-aout
CROSS_PREFIX =		$(CROSS)-

AS =			$(CROSS_PREFIX)gcc
CC =			$(CROSS_PREFIX)gcc
AR =			$(CROSS_PREFIX)ar
RANLIB =		$(CROSS_PREFIX)ranlib

CFLAGS =		-O2 -ansi -pedantic -Wall
INCLUDES =		-I.
AR_FLAGS =		rc

INSTALL_DIR =		$(PREFIX)/$(CROSS)/lib
INSTALL_INC_DIR =	$(PREFIX)/$(CROSS)/include

HEADERS =		nslib.h ioregs.h vidregs.h

OBJS =			_getcpsr.o _setcpsr.o _getsp.o halt.o		\
			number.o print.o putchar.o resetmisc.o		\
			trapirq.o trapreset.o int.o irq.o free.o	\
			heapinit.o malloc.o memmap.o mmu.o _main.o	\
			random.o table.o mouse.o serial.o _clear.o	\
			bzero.o	vid1.o vid1asm.o vid2.o vid2asm.o	\
			vid3.o vid3asm.o vid4.o vid4asm.o		\
			vidn.o vidnasm.o

CRT_OBJS =		crt0.o

LIB_FILE =		libns-0.1.a


all:		$(LIB_FILE) $(CRT_OBJS)

install:
	cp $(LIB_FILE) $(INSTALL_DIR)
	cp $(CRT_OBJS) $(INSTALL_DIR)
	cp $(HEADERS) $(INSTALL_INC_DIR)

clean:
	rm -f $(LIB_FILE) $(OBJS) $(CRT_OBJS)

$(LIB_FILE):	$(OBJS)
	rm -f $(LIB_FILE)
	$(AR) $(AR_FLAGS) $(LIB_FILE) $(OBJS)
	$(RANLIB) $(LIB_FILE)

_getcpsr.o:	debug/_getcpsr.S
	$(CC) -c $(CFLAGS) debug/_getcpsr.S
	
_setcpsr.o:	debug/_setcpsr.S
	$(CC) -c $(CFLAGS) debug/_setcpsr.S
	
_getsp.o:	debug/_getsp.S
	$(CC) -c $(CFLAGS) debug/_getsp.S
	
halt.o:	debug/halt.S
	$(CC) -c $(CFLAGS) debug/halt.S

number.o:	debug/number.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) debug/number.c

print.o:	debug/print.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) debug/print.c

putchar.o:	debug/putchar.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) debug/putchar.c

resetmisc.o:	exception/resetmisc.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) exception/resetmisc.c

trapirq.o:	exception/trapirq.S
	$(CC) -c $(CFLAGS) exception/trapirq.S

trapreset.o:	exception/trapreset.S
	$(CC) -c $(CFLAGS) exception/trapreset.S

int.o:	intrpt/int.S
	$(CC) -c $(CFLAGS) intrpt/int.S

irq.o:	intrpt/irq.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) intrpt/irq.c

free.o:	mem/free.c  $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) mem/free.c
	
heapinit.o:	mem/heapinit.c  $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) mem/heapinit.c
	
malloc.o:	mem/malloc.c  $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) mem/malloc.c
	
memmap.o:	mem/memmap.S
	$(CC) -c $(CFLAGS) mem/memmap.S
	
mmu.o:	mem/mmu.c  $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) mem/mmu.c
	
_main.o:	misc/_main.c  $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) misc/_main.c
	
random.o:	misc/random.S
	$(CC) -c $(CFLAGS) misc/random.S

table.o:	misc/table.c  $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) misc/table.c
	
mouse.o:	mouse/mouse.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) mouse/mouse.c

serial.o:	serial/serial.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) serial/serial.c
	
crt0.o:		start/crt0.S
	$(CC) -c $(CFLAGS) start/crt0.S

_clear.o:	string/_clear.S
	$(CC) -c $(CFLAGS) string/_clear.S

bzero.o:	string/bzero.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) string/bzero.c

vid1.o:	video/vid1.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) video/vid1.c

vid1asm.o:	video/vid1asm.S
	$(CC) -c $(CFLAGS) video/vid1asm.S

vid2.o:	video/vid2.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) video/vid2.c

vid2asm.o:	video/vid2asm.S
	$(CC) -c $(CFLAGS) video/vid2asm.S

vid3.o:	video/vid3.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) video/vid3.c

vid3asm.o:	video/vid3asm.S
	$(CC) -c $(CFLAGS) video/vid3asm.S

vid4.o:	video/vid4.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) video/vid4.c

vid4asm.o:	video/vid4asm.S
	$(CC) -c $(CFLAGS) video/vid4asm.S

vidn.o:	video/vidn.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) video/vidn.c

vidnasm.o:	video/vidnasm.S
	$(CC) -c $(CFLAGS) video/vidnasm.S

