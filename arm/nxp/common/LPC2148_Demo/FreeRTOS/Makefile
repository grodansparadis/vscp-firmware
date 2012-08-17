#
#  $Id: Makefile 156 2008-10-15 07:05:54Z akhe $
#  $Revision: 156 $
#  $Author: akhe $
#  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
#  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/FreeRTOS/Makefile $
#

SRC_FILES=croutine.c list.c queue.c tasks.c

SUBDIRS=portable

#
# Define all object files.
#
ARM_OBJ = $(SRC_FILES:.c=.o)

.PHONY: all
all : subdirs $(ARM_OBJ)

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

$(ARM_OBJ) : %.o : %.c Makefile .depend
	$(CC) -c $(CFLAGS) -Wno-cast-align -Wno-strict-aliasing $< -o $@
	$(AR) rc $(COMMON)/common.a $@

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
			$(CC) $(CFLAGS) -M $(SRC_FILES) > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
