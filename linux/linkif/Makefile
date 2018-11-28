# =========================================================================
#                      
# =========================================================================

CC = gcc

CFLAGS =  -g -O0 -I. -I../../common
LDFLAGS = 
EXTRALIBS = 

srcdir = .
top_srcdir = .
top_builddir =
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
datadir = ${prefix}/share
includedir = ${prefix}/include
DLLPREFIX = lib

### Variables: ###

TESTIF_OBJECTS = testif.o\
	vscp_link.o\

### Targets: ###

all: testif

testif:  $(TESTIF_OBJECTS)
	$(CC) -o testif $(TESTIF_OBJECTS) $(LDFLAGS) $(EXTRALIBS)

testif.o: testif.c testif.h
	$(CC) $(CFLAGS)  -c testif.c -o $@

vscp_link.o: ../../common/vscp_link.c ../../common/vscp_link.h
	$(CC) $(CFLAGS) -c ../../common/vscp_link.c -o $@
	
install: all
	$(INSTALL_PROGRAM) -d $(VSCP_PROJ_BASE_DIR)

uninstall:

install-strip: install

clean:
	rm -rf testif
	rm -rf ./.deps ./.pch
	rm -f ./*.o
	rm -f ../../common/*.o
	rm -rf *~
	rm -f *.gz
	rm -f *.deb

distclean: clean
	rm -f config.cache config.log config.status bk-deps bk-make-pch shared-ld-sh

$(ALWAYS_BUILD):  .FORCE

.FORCE:

# Include dependency info, if present:
-include .deps/*.d

.PHONY: all install uninstall clean distclean data .FORCE
