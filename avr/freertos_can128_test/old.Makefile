PRG            = vscptest
OBJ            = vscptest.o ../common/can_at90can128.o ../common/uart.o ../../common/vscp.c
#MCU_TARGET     = atmega16
#MCU_TARGET     = atmega8
#MCU_TARGET     = atmega48
#MCU_TARGET     = atmega88
#MCU_TARGET     = atmega168
MCU_TARGET     = at90can128
OPTIMIZE       = -Os

EXTRAINCDIRS   = d:/winavr/avr/include/avr
# defs
# ====
# 'FOSC' is oscillator frequency in K
# Define 'USE_UART0' for uart0 'USE_UART1' for uart1
# 'BAUDRATE' for the configred baudrate
# 'GUID_IN_EEPROM' Define to have GUID in EEPROM
# 'ENABLE_WRITE_2PROTECTED_LOCATIONS' Make GUID etc writable
DEFS           = -DFOSC=16000 -DUSE_UART1 -DBAUDRATE=9600 -DENABLE_WRITE_2PROTECTED_LOCATIONS
LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) $(patsubst %,-I %,$(EXTRAINCDIRS))
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom

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
