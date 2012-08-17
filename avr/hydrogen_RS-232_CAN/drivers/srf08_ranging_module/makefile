# Simple Makefile
# Volker Oth (c) 1999


########### change this lines according to your project ##################

#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
    MCU = atmega103

#put the name of the target file here (without extension)
        TRG     = test_srf08

#put your C sourcefiles here 
        SRC     = $(TRG).c lcd_io.c i2c.c srf08.c

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
$(TRG).o : $(TRG).c  lcd_io.h srf08.h 
srf08.o  : srf08.c srf08.h i2c.h
lcd_io.o : lcd_io.c lcd_io.h 
i2c.o    : i2c.C i2c.h 





