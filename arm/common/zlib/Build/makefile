#
# Copyright (C) ARM Limited 1998. All rights reserved.
# FlashLibrary makefile
#
include ../environ.in

include $(UHAL_BASE)/rules.in

#************************************************************
#  Make targets
#************************************************************
#
all:	
	$(MAKE) TARGET=zlib zlib_all

include ./common.make

#************************************************************
#  clean up the development tree.
#************************************************************
clean:	# clean up
	$(MAKE) TARGET=zlib zlib_clean

