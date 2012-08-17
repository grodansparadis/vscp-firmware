MiWi PIC16 implementation history

Revision 1.0
=====================================================================

Change History
----------------------------------------------
Initial release

Flash Requirements
----------------------------------------------
Basic Stack                      3204 bytes (includes .cinit, 
                                      reset vector, and stack)
Socket support                    344 bytes                       
EUI address search support        288 bytes                                  
Demo Code                         736 bytes (include application,
                                      interrupt vector, and 
                                      interrupt handler)
Total                            4572 bytes              

RAM Requirements
----------------------------------------------
Basic Stack                        78 bytes + RxBufferSize
                                            + TxBufferSize
                                     + 13*NumberOfNetworkTableEntries
                                            = (312 as provided)
Socket support	                    2 bytes                      
EUI address search support          1 bytes                         
Demo Code                           2 bytes                
Total                             317 bytes (as provided)

Features
----------------------------------------------			
Socket support				
EUI adress search support					

Limitations
----------------------------------------------
Security not implemented yet
Network Table must reside in Banks 0 or 1
RX and TX Buffers must reside in Banks 2 and 3
P2P devices not yet supported
No ZENA support for MiWiDefs.inc generation
No POR failure recovery mechanism is provided.
  NetworkTables, sockets, node, and parent information is lost on a
  power failure unless the application saves and restores them. 
