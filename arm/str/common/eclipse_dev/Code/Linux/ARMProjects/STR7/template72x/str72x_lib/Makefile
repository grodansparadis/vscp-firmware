# efsl library Makefile for STR720 by Giacomo Fazio and Antonio Nasca
# (based on efsl library makefile for AT91SAM7S by Martin Thomas)

MCU   = arm7tdmi
#THUMB = -mthumb -mthumb-interwork
THUMB = 

LIBNAME = libSTR72x_lib.a

COPT= -mcpu=$(MCU) $(THUMB) -gdwarf-2 -Wall -Os 
INCLUDEDIRS=-Iinclude
CFLAGS=$(COPT) $(INCLUDEDIRS)
# gcc4 unused code-removal:
CFLAGS += -ffunction-sections -fdata-sections

CC=arm-elf-gcc
AR=arm-elf-ar
OBJCOPY=arm-elf-objcopy

OBJ=src/720_a3brg.o   src/720_adc.o   src/720_bspi.o   src/720_can.o
OBJ+=src/720_cgc.o   src/720_dmac.o   src/720_dramc.o  src/720_eft.o
OBJ+=src/720_eic.o    src/720_emi.o   src/720_gpio.o   src/720_ide.o      src/720_lib.o
OBJ+=src/720_rccu.o   src/720_rtc.o   src/720_uart.o   src/720_wdg.o      src/720_wiu.o


all: lib

libandclean: lib srcclean

lib: $(OBJ)
	$(AR) rcs $(LIBNAME) $(OBJ)
	
srcclean :
	rm -f $(OBJ)

clean :
	rm -f $(OBJ) $(LIBNAME)
