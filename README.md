vscp_firmware
=============

vscp firmware contains examples for different hardware architectures how to
implement the Very Simple Control Protocol (VSCP, http://www.vscp.org).

Files are organized under hardware architecture with common file folders on
each level where common files for vscp (folder /common) and common
files for a specific architecture (folder /{arch}/common) reside. 

Most important files for implementers are:

/common/vscp_firmware.* (for Level I nodes) and
/common/vscp_firmware_level2.* (forLevel II nodes)

which implements the VSCP protocol. There are also some references to
vscp_class.h and vscp_type.h which reside in the vscp_software repository
in the path /src/vscp/common. It is preferred to include these files from
that second repository (and not to make redundant copies if possible).

Ake Hedman <akhe@grodansparadis.com>, Grodans Paradis AB
