##############################################################################
# Makefile
# 

PATH		:= $(PATH)

# Tools
CC			:= arm-thumb-elf-gcc
CPP			:= arm-thumb-elf-g++
OBJCOPY		:= arm-thumb-elf-objcopy
INSIGHT		:= arm-thumb-elf-insight

# Tool locations
PATH		:= /cygdrive/c/cygwin/arm/bin:$(PATH)

# Project files
PPR			:= $(wildcard *.ppr)
NAME		:= $(basename $(PPR))
BIN			:= $(NAME).bin
ELF			:= $(NAME).elf
MAP			:= $(NAME).map
LNK			:= Linkerscript

# Options
CFLAGS		:= -g -pipe -Wall -mcpu=arm7tdmi -mtune=arm7tdmi -mstructure-size-boundary=32 -Wno-multichar -fno-builtin $(INCPATHS)
LDFLAGS		:= -Wl,-Map,$(MAP) -T$(LNK) -nostartfiles

# Extract source files from PSPad project file
FILES		:= $(shell	\
	gawk "	\
		/^\t\t[^\t]/		{ OK=0 }	\
		//					{ if (OK==1) print $0 }	\
		/^\t\t\+compile$$/	{ OK=1 }	\
	" $(PPR))

# Which object files should be built from the source files?
ALL_OBJS	:=	$(patsubst %.cpp,%.o,\
				$(patsubst %.s,%.o,\
				$(patsubst %.S,%.o,\
				$(FILES))))

# Which files can be cleaned?
CLEAN		+= $(ALL_OBJS) $(BIN) $(ELF) $(MAP)

# 
.PHONY: all
all: $(NAME)

.PHONY: $(NAME)
$(NAME): $(ELF)	#$(BIN)

.PHONY: debug
debug: $(ELF)
	$(if $(shell pslist | grep -i OcdLib), , cmd /c start OcdLibRemote --cpu ARM7 --device WIGGLER)
	$(INSIGHT) $(ELF)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)

$(ELF): $(ALL_OBJS) $(LNK)
	$(CPP) $(CFLAGS) $(LDFLAGS) -o $@ $(filter-out %crt0.o,$(ALL_OBJS))

# Common include file dependecies
%.c:: %.h
%.cpp:: %.h

# How to make object files?
%.o: %.c; $(CC) $(CFLAGS) -c -o $@ $<
%.o: %.cpp; $(CPP) $(CFLAGS) -c -o $@ $<
%.o: %.s; $(CC) $(CFLAGS) -c -o $@ $<
%.o: %.S; $(CC) $(CFLAGS) -c -o $@ $<
%.o: %.bin; cd $(@D); $(OBJCOPY) -I binary -O elf32-little --rename-section .data=.rodata $(<F) $(@F)

# Remove output files
.PHONY: clean
clean:
	-$(RM) $(CLEAN)
          
