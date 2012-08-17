#
# nmake - Makefile for AVR application stub
#
NAME    = test
#MCU     = at90s8515
#MCU     = atmega161
MCU     = at90s2313

# check for a local environmental include file
#!if exist( environ.mak )
#!include environ.mak
#!else
# put default enviornmental variables (ROOTDIR,...) here
ROOTDIR = c:\avrgcc
#!endif

AVR      = $(ROOTDIR)
PATH     = $(AVR)\bin;$(PATH)

CPFLAGS  = -mmcu=$(MCU) -g -O2 -Wall -Wcast-align -Wcast-qual -Wimplicit \
           -Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
           -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow \
           -Wstrict-prototypes -Wswitch -Wunused
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$*.map,--cref

#############################################################################

CC       = avr-gcc
LD       = avr-ld
AR       = avr-ar
AS       = avr-gcc
GASP     = avr-gasp
NM       = avr-nm
OBJCOPY  = avr-objcopy
RANLIB   = avr-ranlib
STRIP    = avr-strip
CP       = copy /Y
RM       = del /Q
ARCHIVE  = zip
MV       = ren
ELF2COFF = elfcoff

#############################################################################

.SUFFIXES :
.SUFFIXES : .o .c .s .elf .bin .hex .asm

.c.o:
	@$(CC) $(CPFLAGS) $< -c -o $@

.c.s:    
	@$(CC) $(CPFLAGS) $< -S -o $@

.c.asm:  
	@$(CC) $(CPFLAGS) $< -S -o $@

.s.o:    
	@$(CC) $(ASFLAGS) $< -c -o $@

.elf.bin:
	@$(OBJCOPY) $*.elf -O binary $*.bin

##
# if $(RELEASE) is defined, "mark" the S0 record of the .S19 file with
# the name of the release, then rename the .S19 file back to the original.
# Otherwise the contents of the S0 record don't matter.
##
.elf.s19:
!if defined(RELEASE)
  @$(OBJCOPY) $*.elf -O srec $(RELEASE).s19
  @$(MV) $(RELEASE).s19 $*.s19
!else
  @$(OBJCOPY) $*.elf -O srec $*.s19
!endif

.elf.hex:
  @$(OBJCOPY) $*.elf -O ihex $*.hex

##############################################################################

## these next conditionals must be before the 'all:' target

##
# 'nmake release=<version_string>' sets the version string in version.c
# to "<version_string>", builds the output images without the GDB stub.
##
!ifdef RELEASE
VERSION  = $(RELEASE)
ZIP      = $(RELEASE)
do_release: do_version scratch
!endif

##
# 'nmake version=<version_string>' sets the version string in version.c
# to "<version_string>".
##
!ifdef VERSION
do_version:
      @echo #include "version.h">version.c
      @echo const char version[] = "$(VERSION)";>>version.c
      @echo Project marked "$(VERSION)" in 'version.c'.
!endif

##
# 'nmake zip=<file_name>' creates a <file_name>.zip archive.
##
!ifdef ZIP
ZIPFILE = $(ZIP)
do_zip: zip
!else
ZIPFILE = $(NAME).zip
!endif

##############################################################################

ASRCS = 

CSRCS = test.c uart.c

OBJS = $(ASRCS:.s=.o) $(CSRCS:.c=.o)

##############################################################################
all: $(NAME).hex

$(NAME).elf: $(OBJS) Makefile
      @$(CC) $(OBJS) $(LDFLAGS) -o $@
#      @$(STRIP) --discard-locals -o $(NAME).es $(NAME).elf
#      @$(NM) $(NAME).es >$(NAME).syA
#      @$(NM) -n $(NAME).es >$(NAME).sy1
#      @$(RM) $(NAME).es
      @$(ELF2COFF) $(NAME).elf debug $(NAME).cof

depend: clean
      @$(CC) -M -mmcu=$(MCU) $(CSRCS) >depend.mak

scratch::
      -@$(RM) *.hex     >NUL 2>&1

scratch:: clean all

kill: clean
      -@$(RM) *.c       >NUL 2>&1
      -@$(RM) *.h       >NUL 2>&1
      -@$(RM) *.s       >NUL 2>&1
      -@$(RM) *.txt     >NUL 2>&1
      -@$(RM) Makefile  >NUL 2>&1

clean:
	-@$(RM) *.o       >NUL 2>&1
	-@$(RM) *.asm     >NUL 2>&1
	-@$(RM) *.bin     >NUL 2>&1
	-@$(RM) *.elf     >NUL 2>&1
	-@$(RM) *.cof     >NUL 2>&1
	-@$(RM) *.map     >NUL 2>&1
	-@$(RM) *.bak     >NUL 2>&1
	-@$(RM) *.syA     >NUL 2>&1
	-@$(RM) *.sy1     >NUL 2>&1
	-@$(CP) debug\$(NAME).avd . >NUL 2>&1
	-@$(RM) debug\*.* >NUL 2>&1
	-@$(CP) $(NAME).avd debug   >NUL 2>&1

zip:
	@$(ARCHIVE) $(ZIPFILE) $(ASRCS) $(CSRCS) *.h Makefile \
		$(NAME).map $(NAME).sy? $(NAME).hex
	@$(CP) $(ZIPFILE).zip zips
	@$(RM) $(ZIPFILE).zip

#!if exist(depend.mak)
#!include depend.mak
#!else
#!message dependency file missing, run 'nmake depend' to generate.
#!message ignore this message if running 'nmake depend'.
#!endif








