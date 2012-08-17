# efsl library Makefile for STR750 by Giacomo Fazio and Antonio Nasca
# (based on efsl library makefile for AT91SAM7S by Martin Thomas)


MCU   = arm7tdmi
#THUMB = -mthumb -mthumb-interwork
THUMB = 

LIBNAME = libSTR75x_lib.a

COPT= -mcpu=$(MCU) $(THUMB) -gdwarf-2 -Wall -Os 
INCLUDEDIRS=-Iinclude
CFLAGS=$(COPT) $(INCLUDEDIRS)
# gcc4 unused code-removal:
CFLAGS += -ffunction-sections -fdata-sections

CC=arm-elf-gcc
AR=arm-elf-ar
OBJCOPY=arm-elf-objcopy

OBJ=src/75x_lib.o       src/75x_cfg.o       src/75x_adc.o      src/75x_can.o
OBJ+=src/75x_dma.o      src/75x_eic.o       src/75x_extit.o    src/75x_gpio.o     src/75x_i2c.o   src/75x_mrcc.o
OBJ+=src/75x_pwm.o      src/75x_rtc.o       src/75x_smi.o      src/75x_ssp.o
OBJ+=src/75x_tb.o       src/75x_tim.o       src/75x_uart.o     src/75x_wdg.o


all: lib

libandclean: lib srcclean

lib: $(OBJ)
	$(AR) rcs $(LIBNAME) $(OBJ)
	
srcclean :
	rm -f $(OBJ)

clean :
	rm -f $(OBJ) $(LIBNAME)
