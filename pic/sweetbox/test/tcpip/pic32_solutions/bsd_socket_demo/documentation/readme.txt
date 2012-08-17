
The pic32 socket demo program creates a UDP socket at port 6500
and listen for incomming packets.  It acts as server and echoes back 
the data included in the packet to the client.  The message may 
contain upto 50 byts of data.

The PC side code acts as a client and open the socket at port 6500.

To run the demo code, connect the pic32 board to the network and make sure
the link light is on.  The LCD display shows the ip address assigned to the
board by dhcp server.  lauch the pc executible and use this ip address in
the command line. eg

c:>udp_demo 10.10.33.204

the pc application send a string to the board and wait for the echo.
when it receives the string, it increments and prints the echo counter
value.  To stop the program, press any key.




