ifndef VSCP_HOME
	$(error You must set environment variable VSCP_HOME in your system or shell)
endif
ifndef VSCP_FIRMWARE
#	$(info VSCP_FIRMWARE was unset, forcing it to be set to $(VSCP_HOME)\firmware)
export VSCP_FIRMWARE=$(VSCP_HOME)\firmware
endif
#$(info VSCP Paths set to $(VSCP_HOME) and $(VSCP_FIRMWARE) respectivly)

PRG            = olimex_can232

OBJ = main.c
OBJ+= $(VSCP_FIRMWARE)/avr/common/can_at90can128.c
OBJ+= uart0_int.c


#MCU_TARGET     = at90can128
MCU_TARGET     = at90can32
OPTIMIZE       = -Os


#EXTRAINCDIRS   = $(VSCP_FIRMWARE)/common 
EXTRAINCDIRS  += $(VSCP_FIRMWARE)/avr/common
EXTRAINCDIRS  += $(VSCP_HOME)/src/common
EXTRAINCDIRS  += $(VSCP_HOME)/src/vscp/common
EXTRAINCDIRS  += .

# defs
# ====
# 'FOSC' is oscillator frequency in KHz
# 'F_CPU' is oscillator frequency in Hz, Atmel libs require this
# 'BAUDRATE' used to set speed of uart
DEFS		= -DFOSC=16000 -DF_CPU=16000000 -DBAUDRATE=38400
LIBS            =

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -std=gnu99 -mmcu=$(MCU_TARGET) $(DEFS) $(patsubst %,-I %,$(EXTRAINCDIRS))

# older default: override LDFLAGS       = -Wl,-Map,$(PRG).map


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt -lm

# If this is left blank, then it will use the Standard printf version.
#PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min
# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -lm
# ^^^entscheidend für sprintf mit float/double!!
#---------------- Library Options ----------------

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm
# ^^^entscheidend für sprintf mit float/double!!
#---------------- Linker Options ----------------

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
