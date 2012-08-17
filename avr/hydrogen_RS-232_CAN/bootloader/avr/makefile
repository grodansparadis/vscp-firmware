###############################################################################
#
# Directories and targets
#
###############################################################################

	TOOL_DIR = $(AVR)

	BASE_DIR = __exe__
	OBJ_DIR  = $(BASE_DIR)\gnu_obj
	LST_DIR  = $(BASE_DIR)\gnu_lst
	EXE_DIR  = $(BASE_DIR)\gnu_exe

	TRG      = acsgnu


###############################################################################
#
# Tools and their options
#
###############################################################################

	CC      = $(TOOL_DIR)\bin\avr-gcc
	AS      = $(TOOL_DIR)\bin\avr-gcc -x assembler-with-cpp	
	RM      = $(TOOL_DIR)\bin\rm -f
	RN      = $(TOOL_DIR)\bin\mv
	BIN     = $(TOOL_DIR)\bin\avr-objcopy
	SIZE	= $(TOOL_DIR)\bin\avr-size
	INCDIRS	= -I$(TOOL_DIR)\avr\include
	LIBDIR	= $(TOOL_DIR)\avr\lib
	SHELL   = $(TOOL_DIR)\bin\sh.exe
	MKDIR   = md 
	FORMAT  = srec
	MCU     = atmega161
	MCUL    = atmega161

    CDEFS   = -DSYS_TARGET_GNU  -D__AVR_ATmega161__ __stack=0x45f  

	CPFLAGS	= -Os -Wall -Wstrict-prototypes  -DNDEBUG -mmcu=$(MCU) \
		     $(CDEFS)  -Wa,-ahlms=$(LST_DIR)\$(notdir $(<:.c=.lst))\

	CPFLAGS_NO_OPT = -O0 -Wall -Wstrict-prototypes -DNDEBUG -mmcu=$(MCU) \
			 $(CDEFS) -Wa,-ahlms=$(LST_DIR)\$(notdir $(<:.c=.lst))   

	ASFLAGS = -Wa,-gstabs -mmcu=$(MCU) 
	LDFLAGS = -Wl,-Map=$(LST_DIR)\$(TRG).map,--cref -mmcu=$(MCUL) \
			-nostartfiles -Ttext=0x01FC00 -nodefaultlibs 

###############################################################################
#
# Source files
#
###############################################################################

# C object files

	OBJECTS	=   $(OBJ_DIR)\main.o  	  \
	            $(OBJ_DIR)\init.o     \
		    $(OBJ_DIR)\prog.o          

################################################################################
#
# Compiling rules
#
###############################################################################


### This defines the aims of the make process (default)

all:	dirs $(OBJECTS) link


### Creating output directories

dirs:
	$(MKDIR) $(BASE_DIR)
	$(MKDIR) $(OBJ_DIR)
	$(MKDIR) $(LST_DIR)
	$(MKDIR) $(EXE_DIR)

### Compilation rules and dependencies

$(OBJ_DIR)\main.o : main.c prog.h
	$(CC) -c $(CPFLAGS) $(INCDIRS) $< -o $@
$(OBJ_DIR)\init.o : init.s
	$(AS) -c $(ASFLAGS) $(INCDIRS) $< -o $@
$(OBJ_DIR)\prog.o : prog.s prog.h
	$(AS) -c $(ASFLAGS) $(INCDIRS) $< -o $@


### Link: 
#
#     Instructions to:
#       - create elf output file from object files
#       - create bin (ihex, srec) file from elf output file
#       - If all other steps compile ok then echo "Errors: none". ( for AVR studio)

link: $(OBJECTS)
	$(CC)    $(OBJECTS) $(LIB) $(LDFLAGS) -o $(EXE_DIR)\$(TRG).elf	
	$(BIN)  -O $(FORMAT) -R .eeprom $(EXE_DIR)\$(TRG).elf $(EXE_DIR)\$(TRG).rom
	$(SIZE)  $(EXE_DIR)\$(TRG).elf
	@echo "Errors: none" 

### make instruction to delete created files

clean:
	$(RM) $(OBJECTS)
	$(RM) $(LST_DIR)\*.lst
	$(RM) $(LST_DIR)\*.map
	$(RM) $(EXE_DIR)\*.elf
	$(RM) $(EXE_DIR)\*.rom

### display size info

size:
	$(SIZE) $(EXE_DIR)\$(TRG).elf







