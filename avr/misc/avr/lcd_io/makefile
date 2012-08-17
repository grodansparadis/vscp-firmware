# Simple Makefile
# Volker Oth (c) 1999


########### change this lines according to your project ##################

#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
    MCU = atmega103

#put the name of the target file here (without extension)
        TRG     = test_lcd

#put your C sourcefiles here 
        SRC     = $(TRG).c lcd_io.c

#put additional assembler source file here
        ASRC    =

#additional libraries and object files to link
        LIB     =

#additional includes to compile
        INC     = 

#compiler flags
        CPFLAGS = -g -Os -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)

#linker flags
        LDFLAGS = -Wl,-Map=$(TRG).map,--cref



########### you should not need to change the following line #############
include $(AVR)/include/avr_make

###### dependecies, add any dependencies you need here ###################
$(TRG).o : $(TRG).c  lcd_io.h
lcd_io.o : lcd_io.c lcd_io.h




