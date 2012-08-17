# Makefile for Pic 18f* compilation w/ SDCC.
PIC_CHIP=18f2620
PROG_NAME=ratbehav.hex
OBJS=main.o loop.o prefs.o

# Include and Link paths
INCLUDE=-I. -I/usr/local/share/sdcc/include/pic16
LDFLAGS=-w -I/usr/local/share/sdcc/lib/pic16 -c -s /usr/local/share/gputils/lkr/${PIC_CHIP}.lkr 

# Libraries
LIBS=crt0i.o pic${PIC_CHIP}.lib libsdcc.lib libio${PIC_CHIP}.lib libc18f.lib

# Flags
PROCESSOR_FLAGS=-mpic16 -p${PIC_CHIP}
PROGRAMMER_FLAGS=-device="$(PIC_CHIP)" -writeprog
CFLAGS=-V -S

# Programs
SDCC=sdcc
GPASM=gpasm
GPLINK=gplink
PROGRAMMER=pk2



all: $(PROG_NAME) 

# Compile the .c sources into object files
%.o: %.c 
	$(SDCC) $(INCLUDE) $(PROCESSOR_FLAGS) $(CFLAGS) $<  
	$(GPASM) -c $*.asm

# Link the .o object files into a hex file
$(PROG_NAME): $(OBJS)
	$(GPLINK) $(LDFLAGS) -o $(PROG_NAME) $(OBJS) $(LIBS)

# Write the hex file to the chip
burn: $(PROG_NAME)
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) $(PROG_NAME)

# Remove all intermediate files
clean:
	rm -f *.cof *.adb *.asm *.cod *.lst *.p *.d *.lnk *.o *~

