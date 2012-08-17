#==========================================================================
# this section uses the environment variable VSCP_HOME as base for all
# vscp related files. If you dont have a seperate VSCP_FIRMWARE path in 
# use then simply dont set this variable, it becomes default :)
# (C) 2010 Stefan Langer, datenheim, and others, part of VSCP project
#==========================================================================
ifndef VSCP_HOME
   $(error > You must set environment variable VSCP_HOME in your system or shell)
endif
ifndef VSCP_FIRMWARE
   $(info > VSCP_FIRMWARE was unset, forcing it to be set to $(VSCP_HOME)\firmware)
   export VSCP_FIRMWARE=$(VSCP_HOME)\firmware
endif
$(info > VSCP Paths set to $(VSCP_HOME) and $(VSCP_FIRMWARE) respectivly)

#--------------------------------------------------------------------------
# This subsection is only needed if you use multiple WinAVR versions
# in that case simply define WINAVR_HOME variable to selected version
ifndef WINAVR_HOME
   $(info > You can define the WINAVR_HOME environment variable in case)
   $(info > there are more WinAVR versions installed)
endif
#--------------------------------------------------------------------------


#==========================================================================
# here most often only the last line has to be adjusted to match platform
#==========================================================================
EXTRAINCDIRS =  $(VSCP_HOME)/src/common
EXTRAINCDIRS += $(VSCP_HOME)/src/vscp/common
EXTRAINCDIRS += $(VSCP_FIRMWARE)/common
EXTRAINCDIRS += $(VSCP_FIRMWARE)/avr/common
EXTRAINCDIRS += .
#--------------------------------------------------------------------------
#This subsection is only needed if you use multiple WinAVR versions
ifdef WINAVR_HOME
  EXTRAINCDIRS += $(WINAVR_HOME)/avr/include/avr
endif
#--------------------------------------------------------------------------


#==========================================================================
# Source files
#==========================================================================

OBJ = display3k_gcc.c
OBJ += vscp_actions.c
OBJ += $(VSCP_FIRMWARE)/common/vscp_firmware.c
OBJ += $(VSCP_FIRMWARE)/avr/common/can_at90can128.c

#==========================================================================
# assembler sources
#ASRC = soft_uart.S

#==========================================================================
# Project specific options and settings
# attention: you need to specify cpu clock in F_OSC and also F_CPU
#==========================================================================
TARGET             = display3k_gcc
 MCU_TARGET     = at90can128
#MCU_TARGET     = at90can64
#MCU_TARGET     = at90can32

OPTIMIZE        = -Os


# defs
# ====
# 'FOSC' is oscillator frequency in K
# Define 'USE_UART0' for uart0 'USE_UART1' for uart1
# 'BAUDRATE' for the configred baudrate
DEFS           = -DFOSC=16000 -DUSE_UART1 -DBAUDRATE=9600
LIBS            =

#==========================================================================
# You should not have to change anything below here.

CC             = avr-gcc

#==========================================================================
# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -std=gnu99 -mmcu=$(MCU_TARGET) $(DEFS) $(patsubst %,-I %,$(EXTRAINCDIRS))
override LDFLAGS       = -Wl,-Map,$(TARGET).map

# older default: override LDFLAGS       = -Wl,-Map,$(PRG).map


#==========================================================================
# Library Options, how to handle float to string conversions
#--------------------------------------------------------------------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt -lm
# If this is left blank, then it will use the Standard printf version.
#PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
PRINTF_LIB = $(PRINTF_LIB_FLOAT)
#--------------------------------------------------------------------------
# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min
# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt
# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)
#--------------------------------------------------------------------------
MATH_LIB = -lm
# ^^^entscheidend für sprintf mit float/double!!


#==========================================================================
#  Linker Options
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#---------------- Linker Options ----------------

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

#==========================================================================
# make all stuff
all: $(TARGET).elf $(TARGET).lst $(TARGET).hex eeprom sizes

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

#==========================================================================
# make clean
clean:
	rm -rf *.o ../common/*.o $(TARGET).elf *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

#==========================================================================
# Rules for building the .text rom images


%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# Rules for building the .eeprom rom images
# eeprom: ehex ebin esrec
eeprom: ehex

ehex:  $(TARGET)_eeprom.eep

%_eeprom.eep: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sizes:
	@avr-size --format=avr --totals --mcu=$(MCU_TARGET) -C $(TARGET).elf

#	@avr-nm --size-sort --print-size $(TARGET).elf

