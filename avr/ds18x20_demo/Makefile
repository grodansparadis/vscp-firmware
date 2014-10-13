

PRG = main

#Used along with avr-size
TARGET = $(PRG).elf

OBJ = main.o
OBJ += uart.o
OBJ += uart_addon.o
OBJ += onewire.o
OBJ += ds18x20.o
OBJ += crc8.o


MCU_TARGET	= at90can32
OPTIMIZE	= -Os


EXTRAINCDIRS  += .


# defs
# ====
# 'FOSC' is oscillator frequency in K
# Define 'USE_UART0' for uart0 'USE_UART1' for uart1
# 'BAUDRATE' for the configred baudrate
//DEFS		=
DEFS        = -DFOSC=16000 -DUSE_UART2 -DBAUDRATE=19200 -DF_CPU=16000000UL
LIBS        =


# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) $(patsubst %,-I %,$(EXTRAINCDIRS)) -std=c99
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom sizes

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o ../common/*.o $(PRG).elf *.eps *.png *.pdf *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sizes:
	@echo
#	@avr-size --format=avr --totals --mcu=$(MCU_TARGET) $(TARGET)
#	@avr-nm --size-sort --print-size $(TARGET)


# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

JPEGFILES               = largedemo-setup.jpg largedemo-wiring.jpg \
                          largedemo-wiring2.jpg

JPEG2PNM           = jpegtopnm
PNM2EPS            = pnmtops
JPEGRESOLUTION     = 180
EXTRA_CLEAN_FILES  = *.hex *.bin *.srec *.eps

dox: ${JPEGFILES:.jpg=.eps}

%.eps: %.jpg
	$(JPEG2PNM) $< |\
	$(PNM2EPS) -noturn -dpi $(JPEGRESOLUTION) -equalpixels \
	> $@
