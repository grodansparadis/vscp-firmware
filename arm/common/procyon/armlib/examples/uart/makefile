# Makefile for ARM
# Pascal Stang

########### change this lines according to your project ##################
#put the name of the target mcu architecture here (arm7tdmi, etc)
	CPU = arm7tdmi

#put the name of the target file here (without extension)
	TRG	= uarttest

#put your C sourcefiles here 

	ARMLIB = c:\code\arm\armlib
	ARCH = lpc2000

	ARMLIB_ARCH_SRC = processor.c timer.c uart.c 
	ARMLIB_SRC = rprintf.c vt100.c

	SRC = $(TRG).c

#put additional assembler source file here
	ASRC = $(ARMLIB)/arch/$(ARCH)/boot/boot.s

#additional libraries and object files to link
	LIB	=

#additional includes to compile
	INC	= 

#assembler flags
	ASFLAGS = -Wa, -gstabs

#compiler flags
	CPFLAGS	= -g -O1 -Wall -Wstrict-prototypes -I$(ARMLIB) -I$(ARMLIB)/arch/$(ARCH) -I$(ARMLIB)/arch/$(ARCH)/include -Ic:\programa\winarm/arm-elf/include -Wa,-ahlms=$(<:.c=.lst)

#linker flags
	LDFLAGS = -T$(ARMLIB)/arch/$(ARCH)/boot/lpc2106-rom.ld -lm -nostartfiles -Wl,-Map=$(TRG).map,--cref,-nostdlib
	
########### you should not need to change the following line #############
include $(ARMLIB)/make/armproj_make
		  
###### dependecies, add any dependencies you need here ###################
$(TRG).o		: $(TRG).c makefile global.h














