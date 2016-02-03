Title:		Malloc - Memory allocation library for fedC and wizC.
Version:	2005-04-08

Copyright:	(c) 2005, HMCS, Marcel van Lieshout

License:	This software is released to the public domain and comes
		without	warranty and/or guarantees of any kind. You have
		the right to use, copy, modify and/or (re-)distribute the
		software as long as the reference to the author is
		maintained in the software (in it's original or in a
                modified form).

Overview:	This library implements a fully functional dynamic
		memory allocation and de-allocation scheme for
		Fed's fedC and wizC. A limited, but very memory-efficient, model
		and an unlimited version are available. The limited model
		can allocate segments with a maximum size of 127 bytes
		each. The unlimited model can hold segments with 
		a size up to the size of the heap. For both models, the size of
                the heap is limited by available ram only (PIC18) or the size
                of a memory-page (PIC12/14/16).
		See Malloc.htm for more information.

Installation:	At least version 11.03 of fedC or WizC is required.
		Unzip the downloadfile into a temporary directory.
		Doubleclick install.bat and follow the directions.
		If your fedC or wizC installpath differs from the default,
		you must change the default-paths in install.bat.
