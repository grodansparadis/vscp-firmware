#******************************************************************************
# 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
#      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
#	Read the instructions in the licence.txt file. No warrenties at all       *
#******************************************************************************
# 	makefile
#******************************************************************************

#==========================================================================
# this section uses the environment variable VSCP_HOME as base for all
# vscp related files. If you dont have a seperate VSCP_FIRMWARE path in 
# use then simply dont set this variable, it becomes default :)
#==========================================================================
ifndef VSCP_HOME
   $(error > You must set environment variable VSCP_HOME in your system or shell)
endif
ifndef VSCP_FIRMWARE
   $(info > VSCP_FIRMWARE was unset, forcing it to $(VSCP_HOME)\firmware)
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
#--------------------------------------------------------------------------
#This subsection is only needed if you use multiple WinAVR versions
ifdef WINAVR_HOME
  EXTRAINCDIRS += $(WINAVR_HOME)/avr/include
#  EXTRAINCDIRS += $(WINAVR_HOME)/avr/include/avr
endif
#--------------------------------------------------------------------------
EXTRAINCDIRS +=  .
EXTRAINCDIRS += $(VSCP_HOME)/src/common
EXTRAINCDIRS += $(VSCP_HOME)/src/vscp/common
EXTRAINCDIRS += $(VSCP_FIRMWARE)/common
EXTRAINCDIRS += $(VSCP_FIRMWARE)/avr/common
#--------------------------------------------------------------------------


#==========================================================================
# Source files
#==========================================================================
OBJ  = digiprobe_gcc.c
OBJ += digiprobe_system.c
OBJ += digiprobe_work.c
OBJ += vscp_mandatory.c

OBJ += avr_adc.c
OBJ += avr_spi.c
OBJ += mcp355xs.c

OBJ += $(VSCP_FIRMWARE)/common/vscp_firmware.c
OBJ += $(VSCP_FIRMWARE)/avr/common/special_avr.c
OBJ += $(VSCP_FIRMWARE)/avr/common/can_at90can128.c

#==========================================================================
# assembler sources
#ASRC = soft_uart.S

#==========================================================================
# Project specific options and settings
# attention: you need to specify cpu clock in F_OSC and also F_CPU
#==========================================================================
#MCU_TARGET		= at90can128
#MCU_TARGET		= at90can64
MCU_TARGET		= at90can32

#TARGET			= digiprobe_gcc_$(MCU_TARGET)
TARGET			= digiprobe_gcc

OPTIMIZE        = -Os

# 'FOSC' is oscillator frequency in K
# Define 'USE_UART0' for uart0 'USE_UART1' for uart1
# 'BAUDRATE' for the configred baudrate
DEFS = -DF_CPU=8000000 -DUSE_UART1 -DBAUDRATE=9600 
LIBS =


#==========================================================================
# You should not have to change anything below here.

CC             = avr-gcc

#==========================================================================
# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -std=gnu99 -mmcu=$(MCU_TARGET) $(DEFS) $(patsubst %,-I %,$(EXTRAINCDIRS))
override LDFLAGS       = -Wl,-Map,$(TARGET).map

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


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
sizes:
	@avr-size --format=avr --totals --mcu=$(MCU_TARGET) -C $(TARGET).elf

#	@avr-nm --size-sort --print-size $(TARGET).elf
