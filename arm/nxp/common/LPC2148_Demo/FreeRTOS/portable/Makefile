#
#  $Id: Makefile 156 2008-10-15 07:05:54Z akhe $
#  $Revision: 156 $
#  $Author: akhe $
#  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
#  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/FreeRTOS/portable/Makefile $
#

#
# Define all object files.
#
SUBDIRS=GCC MemMang

.PHONY: all
all : subdirs

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
