<h1>vscp_firmware</h1>

<img src="http://www.vscp.org/images/vscp_logo.jpg"></img>

VSCP firmware contains examples for different hardware architectures of how to
implement the Very Simple Control Protocol (VSCP, http://www.vscp.org).

Files are organized under hardware architecture with common file folders on
each level where common files for vscp (folder /common) and common
files for a specific architecture (folder /{arch}/common) reside. 

Most important files for implementers are:

/common/vscp_firmware.* (for Level I nodes) and<br>
/common/vscp_firmware_level2.* (forLevel II nodes)

which implements the VSCP protocol. There are also some references to
vscp_class.h and vscp_type.h which reside in the vscp_software repository
in the path /src/vscp/common. It is preferred to include these files from
that second repository (and not to make redundant copies if possible).

Documentation on how to build your own nodes from this code is here 
http://www.vscp.org/docs/vscpfirmware/doku.php?id=start


The main reference implementations for VSCP for CAN is the Paris and the Kelvin modules
which both can find here http://www.grodansparadis.com/products.html  Both are available as 
ready made modules, bare pcb's and software and hardware is fully open and free.

Ake Hedman <akhe@grodansparadis.com>, Grodans Paradis AB
