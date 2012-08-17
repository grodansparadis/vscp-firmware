# efsl library Makefile for STR730 by Giacomo Fazio and Antonio Nasca
# (based on efsl library makefile for AT91SAM7S by Martin Thomas)


MCU   = arm7tdmi
#THUMB = -mthumb -mthumb-interwork
THUMB = 

LIBNAME = libSTR73x_lib.a

COPT= -mcpu=$(MCU) $(THUMB) -gdwarf-2 -Wall -Os 
INCLUDEDIRS=-Iinclude
CFLAGS=$(COPT) $(INCLUDEDIRS)
# gcc4 unused code-removal:
CFLAGS += -ffunction-sections -fdata-sections

CC=arm-elf-gcc
AR=arm-elf-ar
OBJCOPY=arm-elf-objcopy

OBJ=src/73x_lib.o       src/73x_cfg.o       src/73x_adc.o      src/73x_bspi.o
OBJ+=src/73x_can.o      src/73x_cmu.o       src/73x_dma.o      src/73x_flash.o    src/73x_eic.o   src/73x_gpio.o
OBJ+=src/73x_i2c.o      src/73x_prccu.o     src/73x_pwm.o      src/73x_rtc.o      src/73x_tb.o    src/73x_tim.o
OBJ+=src/73x_uart.o     src/73x_wdg.o       src/73x_wiu.o      src/73x_wut.o     


all: lib

libandclean: lib srcclean

lib: $(OBJ)
	$(AR) rcs $(LIBNAME) $(OBJ)
	
srcclean :
	rm -f $(OBJ)

clean :
	rm -f $(OBJ) $(LIBNAME)
