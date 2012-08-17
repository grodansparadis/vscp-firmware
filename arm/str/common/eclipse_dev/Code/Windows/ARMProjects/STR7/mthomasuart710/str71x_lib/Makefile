# efsl library Makefile for STR710 by Giacomo Fazio and Antonio Nasca
# (based on efsl library makefile for AT91SAM7S by Martin Thomas)

MCU   = arm7tdmi
THUMB = -mthumb -mthumb-interwork
#THUMB = 

LIBNAME = libSTR71x_lib.a

COPT= -mcpu=$(MCU) $(THUMB) -gdwarf-2 -Wall -Os 
INCLUDEDIRS=-Iinclude
CFLAGS=$(COPT) $(INCLUDEDIRS)
# gcc4 unused code-removal:
CFLAGS += -ffunction-sections -fdata-sections

CC=arm-elf-gcc
AR=arm-elf-ar
OBJCOPY=arm-elf-objcopy

OBJ=src/71x_lib.o   src/adc12.o     src/apb.o      src/bspi.o
OBJ+=src/can.o      src/eic.o       src/emi.o      src/flash.o    src/gpio.o
OBJ+=src/i2c.o      src/pcu.o       src/rccu.o     src/rtc.o      src/tim.o       src/uart.o
OBJ+=src/wdg.o      src/xti.o       


all: lib

libandclean: lib srcclean

lib: $(OBJ)
	$(AR) rcs $(LIBNAME) $(OBJ)
	
srcclean :
	rm -f $(OBJ)

clean :
	rm -f $(OBJ) $(LIBNAME)
