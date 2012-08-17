# Simple Makefile
# Volker Oth (c) 1999


########### change this lines according to your project ##################

#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
    MCU = atmega103

#put the name of the target file here (without extension)
        TRG     = test_lm75

#put your C sourcefiles here 
        SRC     = $(TRG).c lcd_io.c i2c.c lm75.c

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
$(TRG).o : $(TRG).c  lcd_io.h lm75.h
lm75.o   : lm75.c lm75.h i2c.h
lcd_io.o : lcd_io.c lcd_io.h
i2c.o    : i2c.C i2c.h





