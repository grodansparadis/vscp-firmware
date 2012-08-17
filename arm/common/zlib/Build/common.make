#
# Copyright ARM Limited 1999-2000. All rights reserved.
# ARM implementation of zlib
# File: common.make
# Assumes that uHAL/rules.in are somehow included 
#
#	$Id: common.make,v 1.1 2005/12/19 11:46:07 akhe Exp $
#

LIBOBJECTS = $(TARGET)/adler32.o $(TARGET)/compress.o $(TARGET)/crc32.o \
	$(TARGET)/deflate.o $(TARGET)/infblock.o $(TARGET)/infcodes.o \
	$(TARGET)/inffast.o $(TARGET)/inflate.o $(TARGET)/inftrees.o \
	$(TARGET)/infutil.o $(TARGET)/trees.o $(TARGET)/uncompr.o \
	$(TARGET)/zutil.o $(TARGET)/unzip.o
	 
SOURCES = ../Sources
FINCLUDE = ../h
INCLUDEFILES = $(FINCLUDE)/zlib.h $(FINCLUDE)/armzip.h  
INCL := -I$(FINCLUDE)

zlib_all: 	$(TARGET)/zlib.$(LIB_EXT)

#------------------------------------------------------------
# The Library
#------------------------------------------------------------
$(TARGET)/adler32.o:	$(SOURCES)/adler32.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/compress.o:	$(SOURCES)/compress.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/crc32.o:	$(SOURCES)/crc32.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/deflate.o:	$(SOURCES)/deflate.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/infblock.o:	$(SOURCES)/infblock.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/infcodes.o:	$(SOURCES)/infcodes.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/inffast.o:	$(SOURCES)/inffast.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/inflate.o:	$(SOURCES)/inflate.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/inftrees.o:	$(SOURCES)/inftrees.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/infutil.o:	$(SOURCES)/infutil.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/trees.o:	$(SOURCES)/trees.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/uncompr.o:	$(SOURCES)/uncompr.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/zutil.o:	$(SOURCES)/zutil.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o

$(TARGET)/unzip.o:	$(SOURCES)/unzip.c $(INCLUDEFILES)
	$(ARMCC) $(CFLAGS) -c $(INCL) $< -o $*.o


$(TARGET)/zlib.$(LIB_EXT):	$(LIBOBJECTS)
	$(AR) $(ARFLAGS) $(TARGET)/zlib.$(LIB_EXT) $(LIBOBJECTS)

#************************************************************
#  clean up the development tree.
#************************************************************
$(TARGET)_clean:	# clean up
	rm -f *~
	rm -f $(TARGET)/*.o
	rm -f $(TARGET)/*.axf
	rm -f $(TARGET)/*.sym
	rm -f $(TARGET)/*.elf
	rm -f $(TARGET)/*.bin
	rm -f $(TARGET)/*.$(LIB_EXT)

