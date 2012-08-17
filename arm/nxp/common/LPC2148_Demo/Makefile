#
#  $Id: Makefile 156 2008-10-15 07:05:54Z akhe $
#  $Revision: 156 $
#  $Author: akhe $
#  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
#  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/Makefile $
#

.SILENT: 
.DELETE_ON_ERROR:

#
#  -D CFG_CONSOLE_USB   for console on USB
#  -D CFG_CONSOLE_UART0 for console on UART0
#  -D CFG_CONSOLE_UART1 for console on UART1 instead of USB (disables GPS task, baud rate set to 115200)
#  -D CFG_USB_MSC       to use SD/MMC as a mass storage class device over USB
#
export LPC2148DEMO_OPTS=-D CFG_CONSOLE_USB

#
#  These shouldn't need to be changed
#
export CC=arm-elf-gcc
export AR=arm-elf-ar
export OBJCOPY=arm-elf-objcopy
export OBJDUMP=arm-elf-objdump
export CRT0=boot.s
export WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused
export CFLAGS=$(WARNINGS) -D RUN_MODE=RUN_FROM_ROM -D GCC_ARM7 $(INCLUDES) $(BASEINCLUDE) -mcpu=arm7tdmi -T$(LDSCRIPT) -g -O3 -fomit-frame-pointer $(LPC2148DEMO_OPTS)
export LDSCRIPT=lpc2148-rom.ld
export LINKER_FLAGS=$(COMMON)/common.a -Xlinker -olpc2148.elf -Xlinker -M -Xlinker -Map=lpc2148.map
export ROOT=$(shell pwd)
export BASEINCLUDE=-I$(ROOT) -I$(ROOT)/FreeRTOS/include
export COMMON=$(ROOT)/common

#
#  Project sub-directories
#
SUBDIRS=FreeRTOS adc cpu dac eints fatfs fiq gps i2c iap lcd leds mmc monitor newlib pwm rtc sensors swi timer uart usb usbmass usbser

SRC_FILES = main.c

ARM_OBJ = $(SRC_FILES:.c=.o)

.PHONY: all
all : subdirs lpc2148.hex

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

lpc2148.hex : .depend Makefile lpc2148.elf
	$(OBJCOPY) lpc2148.elf -O ihex lpc2148.hex
	@echo "Length is " `grep __"end_of_text__ = ." *.map | cut -b 17-35` "bytes"

lpc2148.elf : .depend Makefile $(ARM_OBJ) $(COMMON)/common.a $(CRT0) $(LDSCRIPT)
	$(CC) $(CFLAGS) $(ARM_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)
	$(OBJDUMP) -d -S lpc2148.elf >lpc2148.lst
 
$(ARM_OBJ) : %.o : %.c Makefile .depend
	$(CC) -c $(CFLAGS) $< -o $@

#
#  Calculate sizes of all sections
#
sizes.csv : all
	find . -name \*.o | xargs arm-elf-size -A | ./size_to_csv.pl >sizes.csv

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
	$(CC) $(CFLAGS) -M $(SRC_FILES) > .depend

#
#  Utility targets
#
.PHONY: tags
tags :
	@rm -f tags ctags
	find . -name \*.c -exec ctags -a {} \;
	find . -name \*.h -exec ctags -a {} \;

.PHONY: clean
clean :
	find . -name \*.o -exec rm -f {} \;
	find . -name .depend -exec rm -f {} \;
	rm -f *.map *.lst *.elf *.hex .depend $(COMMON)/common.a sizes.csv tags

#
#
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif
