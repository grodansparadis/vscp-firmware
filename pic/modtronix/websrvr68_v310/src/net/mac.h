/**
 * @brief           MAC Module for Modtronix TCP/IP Stack
 * @file            mac.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    string.h, stacktsk.h,  helpers.h, mac.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *                  IMPORTANT!
 *                  For speed, disable the "Procedural abstraction" optimization when using
 *                  the MPLAB C18 compiler!
 *
 * @ingroup         mod_tcpip_base_mac
 *
 *
 * @section description Description
 **********************************
 * The MAC module contains the driver software for the RTL8019AS NIC chip. It provides
 * functions for reading and writing data from and to it.
 * <br>For a detailed description, see the @ref mod_tcpip_base_mac section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP Base Protocols].
 * 
 *
 * @section mac_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //--------------------  Mac Configuration --------------------
 //*********************************************************************
 //When STACK_USE_FAST_NIC is defined, a bit longer, but faster code is used.
 #define STACK_USE_FAST_NIC

 //When defined, the code will be compiled for optimal speed. If not defined, code is defined for smallest size.
 #define MAC_SPEED_OPTIMIZE

 //STACK_DISABLES_INTS can be defined if interrupts are to be disabled during the MAC access routines
 //#define STACK_DISABLES_INTS

 //Only valid for the SBC44EC board! If defined, port B6 and B7 will be available for general purpose I/O.
 //The Ethernet port will however be a bit slower.
 #define NIC_ENABLE_B6B7

 //NIC_DISABLE_INT0 can be defined if the MAC should NOT use INT0 (connected to PIC port RB0) for it's
 //interrupt request status line. When defined, INT0 is tri-stated, and the PIC port pin connected to
 //it can be used as a general purpose user IO pin. The PIC port pin that becomes available is:
 // - For SBC44EC this has no affect - no PIC pins are connected to the interrupt pins
 // - For SBC45EC this has no affect - no PIC pins are connected to the interrupt pins
 // - For SBC65EC and SBC68EC this frees up PIC pin RB0.
 #define NIC_DISABLE_INT0

 //NIC_DISABLE_IOCHRDY can be defined if the MAC should NOT use the IOCHRDY signal on the RTL8019AS chip. This
 //will mean that an additional PIC pin will be available for general purpose use. To use this port pin, the
 //connection to the IOCHRDY signal on RTL8019AS must be broken. This can be done via solder jumpers on certian
 //SBC boards.
 // - For SBC44EC PIC port pin B5 will be available for user IO. Solder jumper SJ5 must be opened!
 // - For SBC45EC PIC port pin A4 will be available for user IO. Solder jumper SJ5 must be opened!
 // - For SBC65EC and SBC68EC this frees up PIC pin RG4. This pin is currently however not routed to an connectors
 #define NIC_DISABLE_IOCHRDY

 //Keep a count of CNTR1 - CNTR3 registers. This can be used for debug purposes, and can be disabled for
 //release code.
 //#define MAC_CNTR1_3

 //Use access RAM variables to optiomize speed and code size. There are only a limited amount of access RAM
 //registers in the PIC processor. If they are not used by any other code modules, this define should be enabled
 //seeing that it will speed up the MAC module quite a bit. If they are not available, an error message will be
 //displayed during compilation.
 //#define MAC_USE_ACCESSRAM

 //This define must be define when using this MAC
 #define MAC_RTL8019AS

 @endcode
 *********************************************************************/

 /*********************************************************************
 * Timing
 * ======
 * At this stage it is not known if the RealTek chip applies IOCHRDY wait
 * states to ordinary NIC register access. It must be assumed
 * as the NE1000 does insert wait cycles upon register access while a
 * local DMA is in progress (It must, there is only one data bus for all).
 * - Register Read (as per NatSemi DP8390C datasheet)
 *   Min. time of 10 ns to latch address before IOR shall be asserted.
 *   Min time of 55 ns after IOCHRDY is asserted before data is valid.
 *   NatSemi DP9390C datasheet states that it can take
 *   max. 55 ns + 30 ns * n*50 ns defore data is valid after falling edge
 *   of /RD. The time n*bcyc is not specified, it's an integral number.
 *   Additional bcyc are inserted if a local or remote DMA takes place.
 *   Assuming that n corresponds to cycles needed for the DMA bursts
 *   this totals to 55 + 30 + (2 * 4 * 50) = 485 ns for 2 byte bursts.
 * - Register Write (as per NatSemi DP8390C datasheet)
 *   Min. time of 15 ns to latch address before IOW shall be asserted.
 *   Min. time of 50 ns after IOCHRDY is asserted before data is sampled.
 *   NatSemi DP9390C datasheet states that it can take
 *   max. 50 + 30 ns * n*50 ns defore data is sampled after falling edge
 *   of /WR. The time n*bcyc is not specified, it's an integral number.
 *   Additional bcyc are inserted if a local or remote DMA takes place.
 *   Assuming that n corresponds to cycles needed for the DMA bursts
 *   this totals to 50 + 30 + (2 * 4 * 50) = 480 ns for 2 byte bursts.<br><br>
 *
 * Known Issues:
 * =============
 * - When writing into the NIC transmit buffer we must assure that we are not
 *   overwring data of a currently transmitting packet. Usually this
 *   will not happen because we are writing slower to the NIC then
 *   a packet will leave the NIC. However if transmission is delayed because
 *   of collisions then this assumption is not correct anymore and we will
 *   overwrite the currently buffered packet with a new one.
 *   This issue can be avoided by improving the TX buffer management
 *   and making sure before allocating a TX buffer that there is no TX in
 *   progress, e.g. doing "while (NICGet(CMDR) & 0x04);" in MACReserveTxBuffer.
 *   However currently only TCP is using MACReserveTxBuffer.
 *   UDPIsPutReady, ARPIsTxReady, ICMPIsTxReady eventually call MACIsTXReady.
 *   Some more thoughts are needed to resolve this issue satisfactory.
 *
 * RTL8019AS Errata
 * ================
 * The RTL8019AS has the following problem as used in the SBCxxEC boards:
 * When doing consecutive Remote DMA reads without updating the "Remote start
 * address" registers, the Remote DMA unit will:
 * - Wrongly set and read the last address location of the RAM.
 * - Wrongly increment from last byte in buffer to first byte
 * For example, if we set the Receive buffer pages from 0x44 to 0x60. If we then
 * set the RSAR to 0x5ffd, and do 5 consecutive Remote DMA reads, the following will happen.
 * 0x5ffd is read by first read = OK
 * 0x5ffe is read by next read = OK
 * 0x44ff is read by next read = Wronge, should be 0x5fff
 * 0x4500 is read by next read = Wronge, should be 0x4400 = first byte of RX Buffer
 * 0x4501 is read by next read = Wronge, should be 0x4401
 * To correct this, we do a check each time we read a byte via Remote DMA for:
 * - If current Remote DMA address is last byte in RX Buffer, set the current address again
 * - If last byte in RX buffer was just read, set Remote DMA address to start of Receive Buffer
 *********************************************************************/


 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************/


/////////////////////////////////////////////////
// Ethernet - documentation module
/**
@defgroup mod_tcpip_base_mac Ethernet
@ingroup mod_tcpip_base

@section mod_tcpip_base_mac_description Description
The MAC module contains the driver software for the RTL8019AS NIC chip. It provides functions
for reading and writing data from and to it.
The RTL8019AS is a NE2000 compatible NIC, that implements both the Ethernet physical (PHY) and MAC layers.
If a different NIC is to be used, users will need to modify or create a new "mac.c" file to implement
access. As long as services provided by "mac.c" are not changed, all other modules will remain unchanged.

The stack utilizes the on-chip SRAM available on the NIC as a holding buffer, until a higher level module
reads it. It also performs the necessary IP checksum calculations in the NIC’s SRAM buffer. In addition
to the receive FIFO buffer managed by the NIC itself, the MAC layer manages its own transmit queue.
Using this queue, the caller can transmit a message and request the MAC to reserve it so that the
same message can be retransmitted, if required. The user can specify sizes for the transmit buffer,
transmit queue and receive queue using ‘C’ defines - see projdefs.h for details.


@section mod_tcpip_base_mac_buffers RTL8019AS Receive and Transmit buffers
The NIC has 8kbytes of RAM, devided into 256byte pages. At initialization, a group of these
pages will be configured to be the RX buffer. The remaining pages are used for user RAM and
for the TX buffer.
<br><b>Receive</b><br>
During reception, the NIC will transfer the received packet via Local DMA to the RX buffer.
The user has to check if the RX buffer contains any packets, and if so, remove them via Remote
DMA access (MACGetRxbuf and MACGetArray functions). One of two RTL8019AS commands can be
used to retrieve received packets from the RX buffer:
@li Remote read command. The user has to set the "Remote Start Registers" and the lenght. The
User also has to remove the packet from the RX buffer once read.
@li Send Packet command. The Remote SMA registers are automatically configured to read the next
packet from the RX buffer. After it has been read, it ia automatically removed too.

<br><b>Transmit</b><br>
Transmission is done via writing the TX packet to a location in RAM via Remote DMA, and than
setting registers in the NIC to start transmission.


@section mod_tcpip_base_mac_ring MAC Receive Ring Buffer
The MAC makes all received packets available to the user via the MACGetRxbuf() and
MACGetArray() functions. These functions will read the given amount of bytes from the
current RX Buffer. The current receive buffer is the one we are currently reading from. The
MACDiscardRx() function will free the current receive buffer.


@section mod_tcpip_user_mac_conf Configuration
For details on configuring this module, see @ref mac_conf section of mac.h file.
*/

#ifndef MAC_H
#define MAC_H

#define MAC_IP      (0ul)
#define MAC_ARP     (0x6ul)
#define MAC_VSCP	(0x7eul)
#define MAC_UNKNOWN (0x0fful)

#define INVALID_BUFFER  (0xfful)

/**
 * TCPGetArrayChr return codes
 */
#define MAC_GETARR_ALL          (0x00ul)
#define MAC_GETARR_TRM          (0x20ul)
#define MAC_GETARR_RETMASK      (0xF0ul)


/** Structure for storing Ethernet address */
typedef struct _MAC_ADDR
{
    BYTE v[6];
} MAC_ADDR;

/**
 * Reset the NIC
 */
void NICReset(void);


/**
 * Function for performing MAC related tasks
 *
 * @preCondition:    Must be called every couple of ms
 */
void    MACTask(void);

void    MACInit(void);

#if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
void    MACInitEepromRegs(void);
#endif

/**
 * Check if ready for next transmission.
 *
 * @param HighPriority Not used for this implementation
 *                     High priority messages are those that don't need to be acknowledged before
 *                     being discarded (TCP control packets, all ICMP packets, all UDP packets, etc.)
 *
 * @return      TRUE if transmit buffer is empty <br>
 *              FALSE if transmit buffer is not empty
 *
 */
BOOL    MACIsTxReady(BOOL HighPriority);

/**
 * Check if the MAC Receive Buffer has any received packets.
 * Reads the following data from the next available packet in the RX buffer: <ul>
 *  <li> The MAC 4 bytes RX packet header (status, nex receive packet, length) </li>
 *  <li> The 14 byte Ethernet header </li></ul>
 * Once a data packet is fetched by calling MACGetHeader, the complete data
 * packet must be fetched (using MACGet) and discarded (using MACDiscardRx).
 * Users cannot call MACGetHeader multiple times to receive multiple packets
 * and fetch data later on.
 *
 * @param[out] remote   Pointer to a MAC_ADDR structure. This function will write the MAC address of the
 *                      node who sent this packet to this structure
 * @param[out] type     Pointer to byte. This function will write the type of header to this variable.
 *                      Can be ETHER_IP, ETHER_ARP or MAC_UNKNOWN
 *
 * @return              True if RX Buffer contained a packet and it's header was read, False if not
 *
 */
BOOL    MACGetHeader(MAC_ADDR *remote, BYTE* type);

/**
 * Reads a single byte via Remote DMA from the current MAC Receive buffer.
 * If the last byte of the RX Buffer was read, this function automatically
 * updates the Remote DMA read address to the first page of the RX Buffer.
 * See PreConditions for more info.
 *
 * @preCondition    Remote DMA address has to be set up prior to calling this function.
 *                  The Remote DMA registers are NOT configured by this function,
 *                  and simply continue reading from the current "Remote DMA Address". A function
 *                  like MACGetHeader() can be called prior to this function to configure Remote
 *                  DMA to read the next packet in RX Buffer (situated in Remote DMA RAM)
 *
 * @return          Read Byte
 */
BYTE    MACGet(void);

/**
 * Reads the given amount of bytes via Remote DMA from the current MAC Receive buffer.
 * If the end of the RX Buffer is reached, this function automatically rolls over to
 * the first page of the RX Ring Buffer. See PreConditions below for more info.
 *
 * @preCondition    Remote DMA address has to be set up prior to calling this function.
 *                  The Remote DMA registers are NOT configured by this function,
 *                  and simply continue reading from the current "Remote DMA Address". A function
 *                  like MACGetHeader() can be called prior to this function to configure Remote
 *                  DMA to read the next packet in RX Buffer (situated in Remote DMA RAM)
 *
 * @param len       Length of array to be read
 * @param val       Buffer to read packet into
 *
 * @return          Number of bytes read
 *
 */
WORD    MACGetArray(BYTE *val, WORD len);

/**
 * Reads the given amount of bytes via Remote DMA from the current MAC Receive buffer,
 * and copies them to the given destination buffer until:
 * - The given buffer for returning the read array is full
 * - The given Chr terminating character in found (is also copied to destination buffer)
 * If the end of the RX Buffer is reached, this function automatically rolls over to
 * the first page of the RX Ring Buffer. See PreConditions below for more info.
 *
 * If the termination character is found, it is also copied to the destination folder.
 * In this case the returned value indicating the number of bytes read will also include
 * this byte.
 *
 * @preCondition    Remote DMA address has to be set up prior to calling this function.
 *                  The Remote DMA registers are NOT configured by this function,
 *                  and simply continue reading from the current "Remote DMA Address". A function
 *                  like MACGetHeader() can be called prior to this function to configure Remote
 *                  DMA to read the next packet in RX Buffer (situated in Remote DMA RAM)
 *
 * @param[out] val  Buffer to read packet into. If NULL, nothing is written to the buffer.
 * @param len       Length of array to be read, maximum length is 255
 * @param chr       Character to look for, that will terminate read
 *
 * @return          The LSB (Bits 0-7) gives number of bytes loaded into buffer, including the
 *                  terminating character if found!
 *                  The MSB (bit 8-15) is a return code.
 *                  The return code in the MSB has the following meaning:
 *                  - MAC_GETARR_ALL: All requested bytes were read, buffer was filled. The LSB will
 *                    contain the number of bytes read = original length we passed this function.
 *                  - MAC_GETARR_TRM: The given terminating character was found. The LSB will
 *                    contain the number of byres read, including the terminating character.
 *
 */
WORD MACGetArrayChr(BYTE *val, BYTE len, BYTE chr);

/**
 * Discard the contents of the current RX buffer. 
 */
void    MACDiscardRx(void);

/**
 * This function writes the MAC header (Ethernet header) part of a packet that
 * has to be transmitted to the current TX buffer (page in NIC RAM).
 * After this function, the data must still be written to the TX buffer.
 * After both header and data has been written, bits are set to instruct
 * NIC to transmit transmit buffer. This function does the following: <ul>
 *  <li> Reset the NIC Remote DMA write pointer to the first byte of the current TX Buffer </li>
 *  <li> Write the given header to the current TX Buffer </li>
 *  <li> Set the NIC Remote DMA byte count to the given len. This configures the
 *    Remote DMA to receive the given number of bytes. </li></ul>
 *
 * @param[out] remote   Pointer to a MAC_ADDR structure. This function will write the MAC address of the
 *                      node who sent this packet to this structure
 * @param type          Type of header. Can be ETHER_IP, ETHER_ARP or MAC_UNKNOWN
 * @param dataLen       Size of the Data of this Ethernet packet. A Ethernet packet consists of: <br>
 *                      - 6 Bytes = Desitination MAC address <br>
 *                      - 6 Bytes = Source MAC address <br>
 *                      - 2 Bytes = Type field, currently only IP or ARP <br>
 *                      - n Bytes = Data. Minimum length of 46 bytes <br><br>
 *                      The data will be written to the TX buffer following this command.
 *
 * @return              True if header was read, False if not
 */
void    MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen);

/**
 * Write a single byte to the Remote DMA. The byte is written to the current Remote DMA address.
 * 
 * @param val Byte to write to Remote DMA
 */
void    MACPut(BYTE val);

/**
 * Write given array of bytes to NIC's RAM
 *
 * @param val Pointer to byte array of bytes that are to be written to the NIC's RAM
 * @param len Lengh of array given in val
 */

void    MACPutArray(BYTE *val, WORD len);

/**
 * Flush the MAC
 */
void    MACFlush(void);

/**
 * Discard the given transmit buffer
 *
 * @param buffer Buffer identifier, is a number from 0-255 identifying the buffer
 */
void    MACDiscardTx(BUFFER buffer);

/**
 * This function sets the access location for the active receive buffer,
 * relative to the first byte following the Ethernet header. If the given
 * offset is thus 0, the access location will be set to the first byte of
 * whatever follows the Ethernet header, for example the IP packet or ARP packet.
 * Users must make sure that the supplied offset does not go beyond
 * current receive buffer content. If offset overruns the current receive
 * buffer, all subsequent access to that buffer would yield invalid data.
 *
 * @param offset    Location (with respect to first byte following Ethernet header)
 *                  where next access is to occur.
 *
 */
void    MACSetRxBuffer(WORD offset);

/**
 * This function makes the given transmit buffer active, and sets it's access
 * pointer to be: <br>
 * - At the given offset after the Ethernet header <br>
 * - In the given TX Buffer <br><br>
 * So, if we pass 0 in as the offset, we will set the pointer to the
 * first byte of after the Ethernet header.
 * Users must make sure that the supplied
 * offset does not go beyond current transmit buffer content.
 * If offset overruns the current transmit buffer, all subsequent access to
 * that buffer would yield invalid data.
 *
 * @param buffer    A transmit buffer where this access offset be applied
 * @param offset    Location (with respect to beginning of buffer) where next access is to occur
 *
 */
void    MACSetTxBuffer(BUFFER buffer, WORD offset);

/**
 * This function reserves a given transmit buffer and marks it as unavailable. This function
 * is useful for TCP layer where a message would be queued until it is correctly
 * acknowledged by remote host.
 *
 * @param buffer Buffer identifier, is a number from 0-255 identifying the buffer
 */
void    MACReserveTxBuffer(BUFFER buffer);

/**
 * Get the current Remote DMA address. This is the address set by the NICSetAddr register.
 * It will however be incremented after each Remote DMA read.
 * The MACGet() function for example does a remote DMA read.
 *
 * @return The current Remote DMA address.
 */ 
WORD    MACGetNICAddr(void);

/**
 * This function returns total receive buffer size available for future data packets.
 */
WORD    MACGetFreeRxSize(void);

/**
 * Check if the MAC is linked
 *
 * @return TRUE if linked, else FALSE
 */
BOOL MACIsLinked(void);

/**
 * Get handle of current transmit buffer. Is a number from 0-254. The current transmit
 * buffer will automatically be set to the next available buffer after a MACFlush() function.
 *
 * @param HighPriority Not used for this (RTL8019AS) MAC.
 *                     High priority messages are those that don't need to be acknowledged before
 *                     being discarded (TCP control packets, all ICMP packets, all UDP packets, etc.)
 *
 * @return Handle of current transmit buffer. Is a number from 0-254, or INVALID_BUFFER is nothing available.
 */
BUFFER MACGetTxBuffer(BOOL HighPriority);

#endif
