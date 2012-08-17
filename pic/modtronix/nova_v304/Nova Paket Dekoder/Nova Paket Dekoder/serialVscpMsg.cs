/// 
/// ***************************************************************************
/// Copyright (c) 2007 and beyond Robert M. Bouwens, CH-5425 Schneisingen, Switzerland. Allrights reserved.
/// 
/// Written by Robert M. Bouwens, 
/// 
/// This code is free for private use.
/// Use in commercial applications requires written permission
/// This software is provided "as is", with no warranty.
/// 
/// For comments mailto:bouwens.mehl@bluewin.com
///
using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
using System.Net;
using System.Diagnostics;

using Xedia.Protocol.VSCP;

namespace BRM_vscp_message
{
    // Incoming CAN Frame under construction (from RS-232)
    //
    // Flags
    // =====
    // Bit 7 - Extended identifier
    // Bit 6 - RTR 
    // Bit 4-5	Package type
    //				000 Data packet.
    //				001 Error packet.
    //				010 Response ACK.
    //				011	Response NACK.
    //				100 Command.
    //				The rest are reserved.
    // Bit 0-3	Number of databytes
    //

    class message_frame 
    {
	    public byte[]   msg; // msg_flag, 4 bytes id,+ 8 bytes m_data
	    public byte     size;
	    public byte     msgflags;

        // statemachine variables
        private byte    state_incoming;
        private byte    substate_incoming;

        const byte MAIN_STATE_NONE = 0;  	// waiting for "AT"
        const byte MAIN_STATE_ATTENTION = 1;	// "A" received, waiting for "T"
        const byte MAIN_STATE_COMMAND = 2;	// "AT" received

        public const byte DLE = 0x10;
        public const byte STX = 0x02;
        public const byte ETX = 0x03;

        const byte INCOMING_STATE_NONE = 0;	// Waiting for <STX>
        const byte INCOMING_STATE_STX = 1;	// Reading m_data
        const byte INCOMING_STATE_ETX = 2;	// <ETX> has been received

        const byte INCOMING_SUBSTATE_NONE = 0;	// Idle
        const byte INCOMING_SUBSTATE_DLE = 1;	// <DLE> received
         
        public message_frame()
        {
	        msg = new byte[ 13 ]; // msg_flag, 4 bytes id,+ 8 bytes m_data
	        size = 0;
	        msgflags = 0;

            state_incoming = INCOMING_STATE_NONE;
            substate_incoming = INCOMING_SUBSTATE_NONE;
        }

        public bool AddByte(byte i_byte)
        {
            string Ls_string = i_byte.ToString() + " ";
            Debug.Write(Ls_string);
            switch (state_incoming)
            {
                case INCOMING_STATE_NONE:
                    if (INCOMING_SUBSTATE_NONE == substate_incoming && DLE == i_byte)
                    {
                        substate_incoming = INCOMING_SUBSTATE_DLE;
                    }
                    else if (INCOMING_SUBSTATE_DLE == substate_incoming && STX == i_byte)
                    {
                        state_incoming = INCOMING_STATE_STX;
                        substate_incoming = INCOMING_SUBSTATE_NONE;
                        size = 0;
                    }
                    else
                    {
                        state_incoming = INCOMING_STATE_NONE;
                        substate_incoming = INCOMING_SUBSTATE_NONE;
                    }
                    break;


                case INCOMING_STATE_STX:
                    if (INCOMING_SUBSTATE_NONE == substate_incoming && DLE == i_byte)
                    {
                        substate_incoming = INCOMING_SUBSTATE_DLE;
                    }
                    else if (INCOMING_SUBSTATE_DLE == substate_incoming && STX == i_byte)
                    {
                        // This is strange as a DEL STX is not expected here
                        // We try to sync up again...
                        state_incoming = INCOMING_STATE_STX;
                        substate_incoming = INCOMING_SUBSTATE_NONE;
                        size = 0;
                    }
                    else if (INCOMING_SUBSTATE_DLE == substate_incoming && ETX == i_byte)
                    {
                        // We have a packet received
                        state_incoming = INCOMING_STATE_NONE;
                        substate_incoming = INCOMING_SUBSTATE_NONE;

                        msgflags = msg[0];

                        // We must at least got the id == four bytes
                        if (size >= 4)
                        {
                            Debug.WriteLine("");
                            Debug.Write("New vscp message:");
                            return true;
                        }
                    }
                    // We come here if m_data is received
                    // but also for DLE DEL
                    else
                    {
                        substate_incoming = INCOMING_SUBSTATE_NONE;
                        if (size < msg.Length)
                        {
                            msg[size++] = i_byte;
                        }
                        else
                        {
                            // This packet has wrong format as it have
                            // to many databytes - start over!
                            state_incoming = INCOMING_STATE_STX;
                            substate_incoming = INCOMING_SUBSTATE_NONE;
                        }
                    }
                    break;

                case INCOMING_STATE_ETX:
                    break;
            }
            return false;
        }
    }

    class SerialEventMessage
    {
	    byte[]      m_data;			// Pointer to m_data. Max 487 (512- 25) bytes
	    byte        m_head;		  	// bit 765 prioriy, Priority 0-7 where 0 is highest.
							        // bit 4 = hardcoded, true for a hardcoded device.
							        // bit 3 = Dont calculate CRC, false for CRC usage.
        byte        m_addr;         // Nickname, aka address
	    short       m_vscp_class;	// VSCP class
	    short       m_vscp_type;	// VSCP type
	    byte        m_sizeData;		// Number of valid m_data bytes	

        private EVENT_PRIORITY  m_priority;
        public EventCode       m_event;

        private const int VSCP_SIZE_HEAD = 1;
        private const int VSCP_SIZE_CLASS = 2;
        private const int VSCP_SIZE_TYPE = 2;

        private const int VSCP_POS_HEAD = 0;
        private const int VSCP_POS_CLASS = 1;
        private const int VSCP_POS_TYPE = 2;
        private const int VSCP_POS_DATA = 5;

        public SerialEventMessage()
        {
	        m_data = new byte[8];
	        m_head = 0;
            m_addr = 0;
	        m_vscp_class = 0;
	        m_vscp_type = 0;
	        m_sizeData = 0;
        }

        public SerialEventMessage( message_frame i_msg)
        {
            m_data = new byte[i_msg.size];
            Buffer.BlockCopy(i_msg.msg, 0, m_data, 0, i_msg.size);
            m_head = i_msg.msgflags;
            m_sizeData = i_msg.size;

            m_priority = (EVENT_PRIORITY)(m_head >> 5);
            m_event = GetEventCode(i_msg.msg);
        }

        // the helpers
        private EventCode GetEventCode(byte[] packet)
        {
            m_addr = packet[4];
            m_vscp_type = BitConverter.ToInt16(packet, VSCP_POS_TYPE);
            m_vscp_type = IPAddress.NetworkToHostOrder(m_vscp_type);
            m_vscp_type &= 0xff;
            m_vscp_class = BitConverter.ToInt16(packet, VSCP_POS_CLASS);
            m_vscp_class = IPAddress.NetworkToHostOrder(m_vscp_class);
            m_vscp_class &= 0x1ff;

            int eventCode = m_vscp_class;
            eventCode <<= 16;
            eventCode += m_vscp_type;

            return (EventCode)eventCode;
        }

        public int GetDataSize()
        {
            return m_data.Length - VSCP_POS_DATA;
         }

        public byte[] GetDataBytes()
        {
            short dataSize = (short)GetDataSize();
            byte[] data = new Byte[dataSize];

            Buffer.BlockCopy(m_data, VSCP_POS_DATA, data, 0, dataSize);

            return data;
        }
        /*
                private EVENT_PRIORITY ReadPriority(byte[] packet)
                {
                    byte head = packet[VSCP_POS_HEAD];

                    return (EVENT_PRIORITY)(head >> 5);
                }
        */
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            string sBytes = "{";
            for (int i = 0; i < m_sizeData; i++)
            {
                sb.AppendFormat("{0:X2} ", m_data[i]);
            }

            sBytes += sb.ToString();
            // Kosmetik
            sBytes.TrimEnd(new char[] { ' ' });
            sBytes += "}";
            return string.Format("EventCode={0}, Class={1}, Type={2}, Priority={3}, Head={4}, Nickname={5}, Data={6}", m_event, m_vscp_class, m_vscp_type, m_priority, m_head, m_addr, sBytes);
        }
    }
}
