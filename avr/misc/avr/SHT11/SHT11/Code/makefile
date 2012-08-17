include make.defs

#The name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
	MCU = atmega103

#put the name of the target file here (without extension)
	TRG	= SHT11

#all source files
	SRC	= 	main.c \
			hal_sht11.c \
			hal_lcd.c \

#all assembler source files
	ASRC = mirror.s

#additional libraries and object files to link
	LIB	=

#additional includes to compile
	INC	= 

#compiler flags
	CPFLAGS	= -DDEBUG -g -O3 -fshort-enums -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)

#linker flags
	LDFLAGS = -Wl,-Map=$(TRG).map,--cref

#assembler flags
	ASFLAGS = -Wa,-gstabs

include make.rules
		  
main.o:			main.c main.h typedefs.h hal_sht11.h hal_lcd.h 
hal_sht11.o:	hal_sht11.c hal_sht11.h typedefs.h 
hal_lcd.o:		hal_lcd.c hal_lcd.h typedefs.h 
