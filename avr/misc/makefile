# Compile Test Makefile for AVRlib verification
# Author: Pascal Stang

#	MCU = atmega163
#	MCU = atmega161
#	MCU = atmega323

	MCU = atmega8
#	MCU = atmega16
#	MCU = atmega32
#	MCU = atmega64
#	MCU = atmega128

#assembler flags
	ASFLAGS = -Wa, -gstabs
#compiler flags
	CPFLAGS	= -g -Os -Wall -Wstrict-prototypes -I$(AVRLIB) -I$(AVRLIB)/conf -Wa,-ahlms=$(<:.c=.lst)

	SRC = $(AVRLIB)/a2d.c \
			$(AVRLIB)/bitbuf.c \
			$(AVRLIB)/buffer.c \
			$(AVRLIB)/cmdline.c \
			$(AVRLIB)/debug.c \
			$(AVRLIB)/encoder.c \
			$(AVRLIB)/fat.c \
			$(AVRLIB)/fixedpt.c \
			$(AVRLIB)/glcd.c \
			$(AVRLIB)/gps.c \
			$(AVRLIB)/i2c.c \
			$(AVRLIB)/i2ceeprom.c \
			$(AVRLIB)/i2csw.c \
			$(AVRLIB)/ks0108.c \
			$(AVRLIB)/lcd.c \
			$(AVRLIB)/nmea.c \
			$(AVRLIB)/pulse.c \
			$(AVRLIB)/pwmsw.c \
			$(AVRLIB)/rprintf.c \
			$(AVRLIB)/rtc.c \
			$(AVRLIB)/servo.c \
			$(AVRLIB)/spi.c \
			$(AVRLIB)/sramsw.c \
			$(AVRLIB)/stxetx.c \
			$(AVRLIB)/timer.c \
			$(AVRLIB)/tsip.c \
			$(AVRLIB)/uart.c \
			$(AVRLIB)/uartsw.c \
			$(AVRLIB)/vt100.c

#			$(AVRLIB)/sta013.c \
#			$(AVRLIB)/uart2.c \


###### BLOCK 1) define some variables based on the AVR base path in $(AVR) #######

	CC	= avr-gcc
	AS	= avr-gcc -x assembler-with-cpp	
	RM	= rm -f
	RN	= mv
	CP	= cp
	BIN	= avr-objcopy
	SIZE	= avr-size
	INCDIR	= .
#	LIBDIR	= $(AVR)/avr/lib
#	SHELL   = $(AVR)/bin/sh.exe

###### BLOCK 2) define all project specific object files ######

	OBJ	= $(SRC:.c=.o) 
	CPFLAGS += -mmcu=$(MCU)
	ASFLAGS += -mmcu=$(MCU)
  
###### BLOCK 3) this defines the aims of the make process ######

#all:	$(OBJ) $(TRG).ok
all:	$(OBJ)


###### BLOCK 4) compile: instructions to create assembler and/or object files from C source ######

%.o : %.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

%.s : %.c
	$(CC) -S $(CPFLAGS) -I$(INCDIR) $< -o $@


###### BLOCK 5) assemble: instructions to create object file from assembler files ######

%.o : %.s
	$(AS) -c $(ASFLAGS) -I$(INCDIR) $< -o $@


###### BLOCK 6) If all other steps compile ok then echo "Errors: none" ######

%ok:
	@echo "Errors: none" 


###### BLOCK 7)  make instruction to delete created files ######

clean:
	$(RM) $(OBJ)
	$(RM) $(SRC:.c=.s)
	$(RM) $(SRC:.c=.lst)
	$(RM) *.map
	$(RM) *.elf
	$(RM) *.obj
	$(RM) *.hex
	$(RM) *.bak
	$(RM) *.log
	@echo "Errors: none"

