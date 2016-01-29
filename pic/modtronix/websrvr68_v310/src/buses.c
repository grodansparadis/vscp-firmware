 /**
 * @brief           Serial Buses
 * @file            buses.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2006-12-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#define THIS_IS_BUSES_MODULE

#include "projdefs.h"
#include "buses.h"
#include "debug.h"
#include "appcfg.h"
#include "busser1.h"
#if defined(BRD_SBC65EC)
#include "busser2.h"
#endif
#include "busi2c.h"

#include "net\udp.h"
#include "net\helpers.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD1, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD1, msgCode, msgStr)


#if defined(MCHP_C18)
    #pragma udata BUS_BUFFER_SEG
    BYTE    busBuffer[BUSBUFFER_SIZE];
    #pragma udata   //Return to default section
#endif

#if defined(HITECH_C18)
    BYTE    busBuffer[BUSBUFFER_SIZE];
#endif

//Bus info structure
BUS_INFO   busInfo;

//Create a UDP socket for the UDP1 port
static UDP_SOCKET udpBus1 = INVALID_UDP_SOCKET;

static UDP_SOCKET udpBus2 = INVALID_UDP_SOCKET;



/**
 * Initialization code.
 */
void busInit(void) {
    BYTE i;
    
    //Initialize busInfo structure with configured values from EEPROM
    memclr(&busInfo, sizeof(busInfo));      //Clear busInfo structure
    busInfoInit();
    
    //Initialze serial buses
    serInit();
    #if defined(BRD_SBC65EC)
    ser2Init();
    #endif
    i2cBusInit();

    /*
    debugPutGenMsg(2);     //@mxd:2:%s
    debugPutRomStringXNull((ROM char*)"Bus Ser1 Txbuf = 0x");
    debugPutByteHex( (BYTE)(((WORD)busInfo.buf[BUSID_SER1].txBuf)>>8) );
    debugPutByteHex( (BYTE)busInfo.buf[BUSID_SER1].txBuf );
    debugPutByte(0);    //Null terminate string

    debugPutGenMsg(2);     //@mxd:2:%s
    debugPutRomStringXNull((ROM char*)"Bus Ser1 Rxbuf = 0x");
    debugPutByteHex( (BYTE)(((WORD)busInfo.buf[BUSID_SER1].rxBuf)>>8) );
    debugPutByteHex( (BYTE)busInfo.buf[BUSID_SER1].rxBuf );
    debugPutByte(0);    //Null terminate string
    */

    #if (DEBUG_BUS >= LOG_ERROR)
    debugPutMsg(1);     //@mxd:1:Initialized Serial Buses
    #endif
}


/**
 * Initializes the busInfo structure with pointers to all buffers.
 */
void busInfoInit(void) {
    BYTE busId;
    WORD totalBufSize = 0;

    //Clear BUS_BUFFER structure for all busses
    memclr((void *)&busInfo.buf, (WORD)(sizeof(BUS_BUFFER) * BUS_COUNT) );
    
    //Initialze all bus TX and RX buffer sizes
    for (busId=0; busId<BUS_COUNT; busId++) {
        busInfo.buf[busId].txBufSize = appcfgGetc(BUSCFG_BUS1_TXBUFSIZE + (busId*2));
        totalBufSize += busInfo.buf[busId].txBufSize;
        busInfo.buf[busId].rxBufSize = appcfgGetc(BUSCFG_BUS1_TXBUFSIZE + (busId*2) + 1);
        totalBufSize += busInfo.buf[busId].rxBufSize;
    }

    //Do check that total buffer size is not larger than BUSBUFFER_SIZE
    //If it is, set all buffers to default values
    if ( totalBufSize > BUSBUFFER_SIZE)
    {
        busInfo.buf[BUSID_SER1].txBufSize = BUS_SER1_TXBUFSIZE;
        busInfo.buf[BUSID_SER1].rxBufSize = BUS_SER1_RXBUFSIZE;

        #if defined(BRD_SBC65EC)
        busInfo.buf[BUSID_SER2].txBufSize = BUS_SER2_TXBUFSIZE;
        busInfo.buf[BUSID_SER2].rxBufSize = BUS_SER2_RXBUFSIZE;
		#else
        busInfo.buf[BUSID_SER2].txBufSize = 0;
        busInfo.buf[BUSID_SER2].rxBufSize = 0;
        #endif

        busInfo.buf[BUSID_I2C1].txBufSize = BUS_I2C1_TXBUFSIZE;
        busInfo.buf[BUSID_I2C1].rxBufSize = BUS_I2C1_RXBUFSIZE;

        busInfo.buf[BUSID_SPI1].txBufSize = BUS_SPI1_TXBUFSIZE;
        busInfo.buf[BUSID_SPI1].rxBufSize = BUS_SPI1_RXBUFSIZE;

        //Set all bus TX and RX buffers to default sizes
        for (busId=0; busId<BUS_COUNT; busId++) {
            //Set TX buffer size
            appcfgPutc(BUSCFG_BUS1_TXBUFSIZE + (busId*2),     busInfo.buf[busId].txBufSize);
            //Set RX buffer size, always follows TX buffer in application configuration structure
            appcfgPutc(BUSCFG_BUS1_TXBUFSIZE + (busId*2) + 1, busInfo.buf[busId].rxBufSize);
        }
    }

    //Initialize buffer pointers    
    busInfo.buf[BUSID_SER1].txBuf = &busBuffer[0];
    busInfo.buf[BUSID_SER1].rxBuf = busInfo.buf[BUSID_SER1].txBuf + busInfo.buf[BUSID_SER1].txBufSize;
    
    busInfo.buf[BUSID_SER2].txBuf = busInfo.buf[BUSID_SER1].rxBuf + busInfo.buf[BUSID_SER1].rxBufSize;
    busInfo.buf[BUSID_SER2].rxBuf = busInfo.buf[BUSID_SER2].txBuf + busInfo.buf[BUSID_SER2].txBufSize;

    busInfo.buf[BUSID_I2C1].txBuf = busInfo.buf[BUSID_SER2].rxBuf + busInfo.buf[BUSID_SER2].rxBufSize;
    busInfo.buf[BUSID_I2C1].rxBuf = busInfo.buf[BUSID_I2C1].txBuf + busInfo.buf[BUSID_I2C1].txBufSize;

    busInfo.buf[BUSID_SPI1].txBuf = busInfo.buf[BUSID_I2C1].rxBuf + busInfo.buf[BUSID_I2C1].rxBufSize;
    busInfo.buf[BUSID_SPI1].rxBuf = busInfo.buf[BUSID_SPI1].txBuf + busInfo.buf[BUSID_SPI1].txBufSize;

    busInfo.bufEnd                = busInfo.buf[BUSID_SPI1].rxBuf + busInfo.buf[BUSID_SPI1].rxBufSize;

    /*
    debugPutGenMsg(2);     //@mxd:2:%s
    debugPutRomStringXNull((ROM char*)"Bus Ser1 Txbuf = 0x");
    debugPutByteHex( (BYTE)(((WORD)busInfo.buf[BUSID_SER1].txBuf)>>8) );
    debugPutByteHex( (BYTE)busInfo.buf[BUSID_SER1].txBuf );
    debugPutByte(0);    //Null terminate string

    debugPutGenMsg(2);     //@mxd:2:%s
    debugPutRomStringXNull((ROM char*)"Bus Ser1 Rxbuf = 0x");
    debugPutByteHex( (BYTE)(((WORD)busInfo.buf[BUSID_SER1].rxBuf)>>8) );
    debugPutByteHex( (BYTE)busInfo.buf[BUSID_SER1].rxBuf );
    debugPutByte(0);    //Null terminate string
    */
}

/**
 * Initialize bus network components
 *
 * @preCondition Do NOT call this function before stackInit() has been called!
 */
void busNetInit(void) {
    BYTE cfg;
    NODE_INFO udpServerNode;

    //Initialize remote IP and MAC address of udpServerNode with 0, seeing that we don't know them for the node
    //that will send us an UDP message. The first time a message is received addressed to this port, the
    //remote IP and MAC addresses are automatically updated with the addresses of the remote node.
    memclr(&udpServerNode, sizeof(udpServerNode));

    //Get UDP Port 1 configuration, is a BUSSTAT_UDP_XXX define
    cfg = appcfgGetc(BUSCFG_UDP1_CFG);

    if (cfg & BUSSTAT_UDP_ENABLE) {
        //Only continue if udpBus1 has not already been initialized
        if (udpBus1 == INVALID_UDP_SOCKET) {
            //Open for configured local port, and INVALID_UDP_PORT remote port. This opens the socket to
            //listen on the given port.
            udpBus1 = UDPOpen(BUS_UDP1PORT, &udpServerNode, INVALID_UDP_PORT);

            //An error occurred during the UDPOpen() function
            if (udpBus1 == INVALID_UDP_SOCKET) {
                #if (DEBUG_BUS >= LOG_ERROR)
                debugPutMsg(2);     //@mxd:2:Could not open UDP1 port
                #endif
            }
        }
    }
    //Disable udpBus1
    else {
        //If udpBus1 currently open, close it
        if (udpBus1 != INVALID_UDP_SOCKET) {
            UDPClose(udpBus1);
            udpBus1 = INVALID_UDP_SOCKET;
        }
    }


    //Get UDP Port 1 configuration, is a BUSSTAT_UDP_XXX define
    cfg = appcfgGetc(BUSCFG_UDP2_CFG);

    if (cfg & BUSSTAT_UDP_ENABLE) {
        //Only continue if udpBus2 has not already been initialized
        if (udpBus2 == INVALID_UDP_SOCKET) {
            //Open for configured local port, and INVALID_UDP_PORT remote port. This opens the socket to
            //listen on the given port.
            udpBus2 = UDPOpen(BUS_UDP2PORT, &udpServerNode, INVALID_UDP_PORT);
    
            //An error occurred during the UDPOpen() function
            if (udpBus2 == INVALID_UDP_SOCKET) {
                #if (DEBUG_BUS >= LOG_ERROR)
                debugPutMsg(3);     //@mxd:2:Could not open UDP2 port
                #endif
            }
        }
    }
    //Disable udpBus2
    else {
        //If udpBus2 currently open, close it
        if (udpBus2 != INVALID_UDP_SOCKET) {
            UDPClose(udpBus2);
            udpBus2 = INVALID_UDP_SOCKET;
        }
    }
}

/**
 * Must be called every couple of ms
 *
 */
void busTask(void) {
    BYTE c;
    BYTE bytesRead;
    BYTE busId;
    BYTE linkedMask;

    /////////////////////////////////////////////////
    //UDP 1
    
    //Get bus to link to UDP 1
    busId = appcfgGetc(BUSCFG_UDP1_LINK);

    #if defined(BRD_SBC65EC)    
    //Currently UDP 1 can only be linked to Serial 1, Serial 2 and I2C for SBC65EC
    if ((busId >= BUSID_SER1) && (busId <= BUSID_I2C1) && (udpBus1 != INVALID_UDP_SOCKET)) {
    #else
    //Currently UDP 1 can only be linked to Serial 1 and I2C for SBC68EC
    if ( ((busId == BUSID_SER1) || (busId == BUSID_I2C1)) && (udpBus1 != INVALID_UDP_SOCKET)) {
    #endif
    
        //Is there any data waiting for us on this UDP port?
        if (UDPIsGetReady(udpBus1)) {

            if (busIsEnabled(busId)) {

                //Read bytes
                while (UDPGet(&c)) {

                    //Wait until a byte is transmitted by the interrupt routine and buffer has place again.
                    while (busIsTxBufFull(busId)) {

                        busService(busId);
                        FAST_USER_PROCESS();
                    }
                    //Add byte to TX buffer, and update buffer pointers
                    busPutByteTxBuf(busId, c);
                }

                //Initiate transmitting if not already transmitting
                if (!busIsTxing(busId)) {

                    busService(busId);
                }
            }

            //Discard the socket buffer.
            UDPDiscard();
        }

        //Is there any data to send on UDP port
        if(busRxBufHasData(busId)) {

            if (UDPIsPutReady(udpBus1)) {

                while(busRxBufHasData(busId)) {
                
                    //Read and remove byte from buffer
                    c = busPeekByteRxBuf(busId);
                    busRemoveByteRxBuf(busId);
                    
                    UDPPut(c);
                }
            
                // Now transmit it.
                UDPFlush();
            }
        }
    }


    /////////////////////////////////////////////////
    //UDP 2
    
    //Get bus to link to UDP 2
    busId = appcfgGetc(BUSCFG_UDP2_LINK);

    #if defined(BRD_SBC65EC)    
    //Currently UDP 1 can only be linked to Serial 1, Serial 2 and I2C for SBC65EC
    if ((busId >= BUSID_SER1) && (busId <= BUSID_I2C1) && (udpBus1 != INVALID_UDP_SOCKET)) {
    #else
    //Currently UDP 1 can only be linked to Serial 1 and I2C for SBC68EC
    if ( ((busId == BUSID_SER1) || (busId == BUSID_I2C1)) && (udpBus1 != INVALID_UDP_SOCKET)) {
    #endif
        //Is there any data waiting for us on this UDP port?
        if (UDPIsGetReady(udpBus2)) {

            if (busIsEnabled(busId)) {
                //Read bytes
                while (UDPGet(&c)) {
                    //Wait until a byte is transmitted by the interrupt routine and buffer has place again.
                    while (busIsTxBufFull(busId)) {
                        busService(busId);
                        FAST_USER_PROCESS();
                    }
                    //Add byte to TX buffer, and update buffer pointers
                    busPutByteTxBuf(busId, c);
                }

                //Initiate transmitting if not already transmitting
                if (!busIsTxing(busId)) {
                    busService(busId);
                }
            }

            //Discard the socket buffer.
            UDPDiscard();
        }

        //Is there any data to send on UDP port
        if(busRxBufHasData(busId)) {
            if (UDPIsPutReady(udpBus2)) {

                while(busRxBufHasData(busId)) {
                    //Read and remove byte from buffer
                    c = busPeekByteRxBuf(busId);
                    busRemoveByteRxBuf(busId);
            
                    UDPPut(c);
                }
            
                // Now transmit it.
                UDPFlush();
            }
        }
    }


    /////////////////////////////////////////////////
    //Service all serial buses
    for (busId=0; busId<BUS_COUNT; busId++) {
        busService(busId);
    }
}


/**
 * Service the given bus. If our code has a section where it has to wait for the transmit
 * buffer of a bus to be empties, it should call this function while waiting.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 */
void busService(BYTE busId) {
    switch(busId) {
        case BUSID_SER1:
            serService();
        break;
        
        #if defined(BRD_SBC65EC)
        case BUSID_SER2:
            ser2Service();
        break;
        #endif
        
        case BUSID_I2C1:
            i2cBusService();
        break;
        
        case BUSID_SPI1:
            NOP();
        break;
    }
}


/**
 * Gets a byte at the given offset from the Transmit Buffer, without removing it.
 * The byte is NOT removed from the buffer, and the buffer pointers are NOT updated! 
 * The byte at the given offset it returned. The offset is how deep the byte is in
 * the buffer. For example, 0 will return first byte in buffer, 5 will return the 6th
 * byte in the buffer.
 *
 * @preCondition Ensure offset parameter is not larger than current number of bytes
 * contained in buffer. Call busGetTxBufCount() to get current number of bytes in buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * @param offset Offset of byte to return. Is a value from 0-n, where n = (busGetTxBufCount() - 1)
 *
 * @return Returns the byte at the given offset in the given bus's Transmit buffer.
 */
BYTE busPeekByteTxBufAt(BYTE busId, BYTE offset) {
	BYTE ofst;

	ofst = busInfo.buf[busId].getTx + offset;

	//Check if offset wrap around
	if (ofst >= busInfo.buf[busId].txBufSize) {
		ofst = ofst - busInfo.buf[busId].txBufSize;
	}

	return *(busInfo.buf[busId].txBuf + ofst);

}


/**
 * Gets a byte at the given offset from the Receive Buffer, without removing it.
 * The byte is NOT removed from the buffer, and the buffer pointers are NOT updated! 
 * The byte at the given offset it returned. The offset is how deep the byte is in
 * the buffer. For example, 0 will return first byte in buffer, 5 will return the 6th
 * byte in the buffer.
 *
 * @preCondition Ensure offset parameter is not larger than current number of bytes
 * contained in buffer. Call busGetRxBufCount() to get current number of bytes in buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * @param offset Offset of byte to return. Is a value from 0-n, where n = (busGetRxBufCount() - 1)
 *
 * @return Returns the byte at the given offset in the given bus's Receive buffer.
 */
BYTE busPeekByteRxBufAt(BYTE busId, BYTE offset) {
	BYTE ofst;

	ofst = busInfo.buf[busId].getRx + offset;

	//Check if offset wrap around
	if (ofst >= busInfo.buf[busId].rxBufSize) {
		ofst = ofst - busInfo.buf[busId].rxBufSize;
	}

	return *(busInfo.buf[busId].rxBuf + ofst);
}


/**
 * Gets a byte from the Transmit Buffer, and removes it from buffer.
 *
 * @preCondition busIsTxBufEmpty() must have been called to confirm the TX Buffer has data!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte in the given bus's TX buffer.
 */
BYTE busGetByteTxBuf(BYTE busId) {
	BYTE ret;

	//Get next byte in buffer
	ret = *(busInfo.buf[busId].txBuf + busInfo.buf[busId].getTx);

	if((++busInfo.buf[busId].getTx)==busInfo.buf[busId].txBufSize) {
		busInfo.buf[busId].getTx = 0;
	}
	
	// Update available data left in tx buffer
	busInfo.buf[busId].txCount--; 

	return ret;
}


/**
 * Gets a byte from the Receive Buffer, and removes it from buffer.
 *
 * @preCondition busIsRxBufEmpty() must have been called to confirm the RX Buffer has data!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte in the given bus's RX buffer.
 */
BYTE busGetByteRxBuf(BYTE busId) {
	BYTE ret;

	//Get next byte in buffer
	ret = *(busInfo.buf[busId].rxBuf + busInfo.buf[busId].getRx);

	if((++busInfo.buf[busId].getRx)==busInfo.buf[busId].rxBufSize) {
		busInfo.buf[busId].getRx = 0;
	}
	
	// Update available data left in rx buffer
	busInfo.buf[busId].rxCount--; 

	return ret;
}


