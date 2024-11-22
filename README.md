![License](https://img.shields.io/badge/license-MIT-blue.svg)
[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)


# VCSP Firmware

<img src="http://vscp.org/images/logo.png" width="200px" alt="VSCP logo">

VSCP firmware contains examples for different hardware architectures of how to
implement the Very Simple Control Protocol (VSCP, [http://www.vscp.org](http://www.vscp.org)).

Files are organized under hardware architecture with common file folders on
each level where common files for vscp (folder /common) and common
files for a specific architecture (folder /{arch}/common) reside. 

Most important files for implementers are:

  * /common/vscp_firmware.* (for Level I nodes) and
  * /common/vscp_firmware_level2.* (for Level II (and Level I) nodes)

which both implements the VSCP protocol. The first alternative is created for very space limited devices, the second is for devices with more normal resources. 

There are also some references to
vscp_class.h and vscp_type.h which reside in the vscp_software repository
in the path /src/vscp/common. It is preferred to include these files from
that second repository (and not to make redundant copies if possible).

Documentation on how to build your own nodes from this code is [here](https://grodansparadis.gitbooks.io/vscp-firmware/) 

The main reference implementations for VSCP over CAN is the Paris and the Kelvin modules. Both can found [here](http://www.grodansparadis.com/products.html) along with other open source products. All modules are available as 
ready made modules, bare pcb's and software and hardware is and will always be fully open and free. All design documents are open and available.

Ake Hedman <akhe@grodansparadis.com>, <a href="http://www.grodansparadis.com">Grodans Paradis AB</a>
