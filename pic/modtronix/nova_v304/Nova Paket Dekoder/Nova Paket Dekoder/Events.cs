#region ---( Includes )---
using System;
using System.Net;
using System.Text;
using System.Diagnostics;
using System.Globalization;
using System.Text.RegularExpressions;

using System.Runtime.InteropServices;
#endregion

namespace Xedia.Protocol.VSCP
{
	#region ---( EventHelper Class )---
	public class EventHelper 
	{
		#region ---( Variables )---
		private static CRCTool _crc;
		private static readonly Encoding _encoder = Encoding.Unicode;
		private static readonly int _sizeofLong = Marshal.SizeOf(typeof(long));
		#endregion

		#region ---( Static Methods )---
		public static byte GetByte (long value, int index) 
		{
			return (byte)(value >> (index * 8));
		}
		/// <summary>
		/// Check to see if the specified zone
		/// covers the main and sub zones
		/// If zone is 0xFF then it encompases all zones
		/// </summary>
		/// <param name="zone"></param>
		/// <param name="mainZone"></param>
		/// <param name="subZone"></param>
		/// <returns></returns>
		public static bool InZone (long zone, byte mainZone, byte subZone) 
		{
			byte zSubZone;
			byte zMainZone;
			
			zMainZone = GetByte (zone, 2);
			zSubZone  = GetByte (zone, 1);

			if ((zMainZone != 0xFF) && (zMainZone != mainZone))
				return false;

			if ((zSubZone != 0xFF) && (zSubZone != subZone))
				return false;

			return true;
		}
		public static string ToHex (byte [] byteArray, int offset, int length) 
		{
			if ((byteArray == null) || (byteArray.Length < (offset + length)))
				return string.Empty;

			StringBuilder sb = new StringBuilder();

			for (int i = 0; i < length; i++)
			{
				sb.Append(byteArray[offset + i].ToString("X2"));
			}

			return sb.ToString();
		}
		public static byte [] FromHex (string hexString)
		{
			hexString = Regex.Replace(hexString.ToUpper(), "[^0-9A-F]", "");

			byte[] b = new byte[hexString.Length / 2];
			for (int i = 0; i < hexString.Length; i += 2) 
			{
				b[i / 2] = byte.Parse(hexString.Substring(i, 2),					
					NumberStyles.AllowHexSpecifier);
			}

			return b;
		}
		public static bool HasCRC (byte[] packet) 
		{
			byte head = packet[EventMessage.VSCP_POS_HEAD];

			return ! ((head & EventMessage.VSCP_NO_CRC_CALC) == EventMessage.VSCP_NO_CRC_CALC);
		}
		public static bool IsValidCRC (byte[] packet)
		{
			short crc;
			short expectedCRC;

			if (HasCRC (packet) == false)
				return true;

			crc = GetCRC (packet);
			expectedCRC = CalcCRC (packet, packet.Length - EventMessage.VSCP_SIZE_CRC);

			if (crc != expectedCRC)
				Debug.WriteLine (string.Format("Invalid CRC '{0}' instead of {1}", crc, expectedCRC));

			return (crc == expectedCRC);
		}
		public static short CalcCRC (byte[] packet, int length)
		{
			if (_crc == null) 
			{
				_crc = new CRCTool ();
				_crc.Init (CRCTool.CRCCode.CRC_CCITT);
			}

			return (short)_crc.crctablefast (packet, length);
		}
		public static short GetCRC (byte [] packet)
		{
			short crc;
			int dataSize;
			int msgLength;

			dataSize = EventHelper.GetDataSize (packet);

			msgLength = dataSize + EventMessage.VSCP_HEADER_SIZE;

			crc = BitConverter.ToInt16 (packet, msgLength);
			crc = System.Net.IPAddress.NetworkToHostOrder (crc);

			return crc;
		}
		public static byte[] EncodeData (long value, string data, byte[] dataBytes) 
		{			
			byte [] firstPart = EncodeData (value, data);

			if (dataBytes == null)
				return firstPart;

			byte [] packet = new byte [firstPart.Length + dataBytes.Length];
			
			Buffer.BlockCopy (firstPart,  0, packet, 0, firstPart.Length);
			Buffer.BlockCopy (dataBytes,  0, packet, firstPart.Length, dataBytes.Length);

			return packet;
		}
		public static byte[] EncodeData (long value, string data) 
		{
			if ((data == null) || (data.Length < 1))
				return BitConverter.GetBytes (value);

			return EncodeData(value, _encoder.GetBytes (data));
		}
		public static byte[] EncodeData (long value, byte[] dataBytes) 
		{
			if (dataBytes == null)
				return BitConverter.GetBytes (value);

			byte [] valueBytes = BitConverter.GetBytes (value);	

			byte [] packet = new byte[dataBytes.Length + valueBytes.Length];
			Buffer.BlockCopy (valueBytes, 0, packet, 0, valueBytes.Length);
			Buffer.BlockCopy (dataBytes,  0, packet, valueBytes.Length, dataBytes.Length);

			return packet;
		}
		public static object[] SplitData (string data, int unitSize) 
		{
			int totalUnits;
			byte [][] result;
			byte [] dataBytes;			

			dataBytes = _encoder.GetBytes (data);
			totalUnits = (int)Math.Ceiling ((double)dataBytes.Length / unitSize);

			if (totalUnits < 1)
				return new byte[0][];

			result = new byte [totalUnits][];
			for (int index = 0; index < totalUnits; index++) 
			{
				int size = unitSize;
				int offset = index * unitSize;
					
				if (dataBytes.Length < (offset + unitSize))
					size = dataBytes.Length - offset;

				byte [] unit = new Byte [size];
				Buffer.BlockCopy (dataBytes, offset, unit, 0, size);
				
				result[index] = unit;
			}

			return result;
		}
		public static string ReadAddress (byte [] packet) 
		{
			return ToHex (packet,
				EventMessage.VSCP_POS_ADDRESS, 
				EventMessage.VSCP_SIZE_ADDRESS);
		}
		public static EventCode GetEventCode (byte [] packet) 
		{
			int eventCode;
			short typeCode;
			short classCode;

			typeCode  = BitConverter.ToInt16 (packet, EventMessage.VSCP_POS_TYPE);
			typeCode  = IPAddress.NetworkToHostOrder (typeCode);
			classCode = BitConverter.ToInt16 (packet, EventMessage.VSCP_POS_CLASS);
			classCode = IPAddress.NetworkToHostOrder (classCode);

			eventCode = classCode;
			eventCode <<= 16;
			eventCode  += typeCode;

			return (EventCode)eventCode;
		}
		public static EVENT_PRIORITY ReadPriority (byte [] packet) 
		{
			byte head = packet [EventMessage.VSCP_POS_HEAD];

			return (EVENT_PRIORITY)(head >> 5);
		}
		public static short GetDataSize (byte [] packet)
		{
			short dataSize = BitConverter.ToInt16(packet, EventMessage.VSCP_POS_DATA_SIZE);
			dataSize = System.Net.IPAddress.NetworkToHostOrder (dataSize);

			return dataSize;
		}
		public static byte [] GetDataBuffer (byte [] packet) 
		{
			byte [] data;
			short dataSize;

			dataSize = EventHelper.GetDataSize (packet);
			data = new Byte [dataSize];

			Buffer.BlockCopy (packet, EventMessage.VSCP_POS_DATA,
				data, 0, dataSize);
			
			return data;
		}
		public static long GetDataCode (byte [] packet) 
		{
			int size;
			byte[] buffer;
			
			size = EventHelper.GetDataSize (packet);

			if (packet.Length < (size + EventMessage.VSCP_POS_DATA))
				return 0;

			size = (size < _sizeofLong) ? size : _sizeofLong;
			buffer = new byte[_sizeofLong];
			
			Buffer.BlockCopy (packet, EventMessage.VSCP_POS_DATA, buffer, 0, size);

			return BitConverter.ToInt64 (buffer, 0);
		}
		public static string GetDataString (byte [] packet) 
		{
			short dataSize;
			
			dataSize = EventHelper.GetDataSize (packet);

			if ((dataSize <= _sizeofLong) || (packet.Length < (EventMessage.VSCP_POS_DATA + dataSize)))
				return String.Empty;
				
			return _encoder.GetString (packet,
				EventMessage.VSCP_POS_DATA + _sizeofLong,
				dataSize - _sizeofLong);			
		}
		#endregion
	}
	#endregion

	#region ---( EventMessage Class )---
	/// <summary>
	/// Summary description for Events.
	/// </summary>
    ///    public class EventMessage : IEventMessage, IDisposable
     public class EventMessage : IDisposable
     {
		#region ---( Constants )---		
		public static readonly int VSCP_SIZE_HEAD = 1;
		public static readonly int VSCP_SIZE_CRC = 2;
		public static readonly int VSCP_SIZE_CLASS = 2;
		public static readonly int VSCP_SIZE_TYPE = 2;
		// MAC address is 6 bytes
		public static readonly int VSCP_SIZE_ADDRESS = 16;
		public static readonly int VSCP_SIZE_DATA_SIZE = 2;	

		public static readonly int VSCP_POS_HEAD = 0;
		public static readonly int VSCP_POS_CLASS = 1;
		public static readonly int VSCP_POS_TYPE = 3;
		public static readonly int VSCP_POS_ADDRESS = 5;
		public static readonly int VSCP_POS_DATA_SIZE = 21;
		public static readonly int VSCP_POS_DATA = 23;

		public static readonly int VSCP_MAX_PACKET_SIZE = 512;

		public static readonly int VSCP_NO_CRC_CALC = 0x4;

		public static readonly int VSCP_HEADER_SIZE = VSCP_POS_DATA;
		public static readonly int VSCP_MAX_DATA_SIZE = VSCP_MAX_PACKET_SIZE - (VSCP_HEADER_SIZE + VSCP_SIZE_CRC);
		#endregion

		#region ---( Variables )---
		EventCode _event;
		private bool _ignoreCRC;
		private Byte[] _address;
		private EVENT_PRIORITY _priority;
		private Byte[] _data = new Byte[0];		
		private static readonly Encoding _encoder = Encoding.Unicode;
		#endregion

		#region ---( Constructors and Destructors )---
		public EventMessage (byte[] sender, EventCode eventCode, EVENT_PRIORITY priority, byte [] data, bool calcCRC) 
		{
			if ((sender == null) || (sender.Length != EventMessage.VSCP_SIZE_ADDRESS))
				_address = new byte [EventMessage.VSCP_SIZE_ADDRESS];
			else
				_address = sender;

			_event = eventCode;
			_priority = priority;
			_ignoreCRC = !calcCRC;

			if ((data == null) || (data.Length > VSCP_MAX_DATA_SIZE))
				_data = new Byte[0];
			else
				_data = data.Clone () as byte [];
		}
		public EventMessage (byte[] msgPacket)
		{
			short msgCRC;
			short expectedCRC;

			if (msgPacket == null)
				throw new ArgumentNullException ("msgPacket");

			byte head = msgPacket[VSCP_POS_HEAD];
			_ignoreCRC = ((head & VSCP_NO_CRC_CALC) == VSCP_NO_CRC_CALC);
            _ignoreCRC = true;

            int incomming_packet_length = msgPacket.Length;
			short dataSize = EventHelper.GetDataSize (msgPacket);

            int msgLength = msgPacket.Length + VSCP_HEADER_SIZE;
			msgLength += (_ignoreCRC) ? 0 : VSCP_SIZE_CRC;

			if (msgLength > VSCP_MAX_DATA_SIZE)
				throw new ApplicationException(string.Format("Invalid event message length: {0}", msgLength));
			
			_priority = (EVENT_PRIORITY)(head >> 5);

			if (_ignoreCRC == false) 
			{
				msgCRC = EventHelper.GetCRC (msgPacket);
				expectedCRC = EventHelper.CalcCRC (msgPacket, msgLength - VSCP_SIZE_CRC);

				if (msgCRC != expectedCRC)
					throw new ApplicationException(string.Format("Invalid event message CRC '{0}' instead of {1}", msgCRC, expectedCRC));
			}

			_event = EventHelper.GetEventCode (msgPacket);
					
			_address = new byte [EventMessage.VSCP_SIZE_ADDRESS];
			Buffer.BlockCopy (msgPacket, VSCP_POS_ADDRESS, _address, 0, VSCP_SIZE_ADDRESS);			

			_data = new Byte [dataSize];
			Buffer.BlockCopy (msgPacket, VSCP_POS_DATA, _data, 0, dataSize);
		}
		~EventMessage () 
		{
			Dispose ();
		}
		public void Dispose() 
		{
			_data = null;
			_address = null;

			// Remove from the finalizer queue
			GC.SuppressFinalize(this);
		}
		#endregion

		#region ---( Public Properties )---
		public EVENT_PRIORITY Priority
		{
			get { return _priority; }
		}
		public EventCode EventCode 
		{
			get { return _event; }
		}
		private short EventClass 
		{
			get { return (short)((int)_event >> 16); }
		}
		private short EventType 
		{
			get { return (short)_event; }
		}
		public byte HeadByte
		{ 
			get
			{
				return (byte)
					(((byte)_priority << 5) |
					(_ignoreCRC ? VSCP_NO_CRC_CALC : 0)); 
			}
		}
		public int Length 
		{
			get 
			{
				int length = this.DataSize + VSCP_HEADER_SIZE;
				length += (_ignoreCRC) ? 0 : VSCP_SIZE_CRC;

				return length;
			}
		}
		public byte[] AddressBytes
		{ 
			get { return _address;  }
			set 
			{
				if ((value == null) || (_address == null))
					_address = new Byte [VSCP_SIZE_ADDRESS];

				Buffer.BlockCopy(value, 0, _address, 0, VSCP_SIZE_ADDRESS);
			}
		}
		public byte[] DataBytes
		{ 
			get { return (_data != null) ? _data : new Byte [0]; }
			set 
			{
				if (value == null)
					_data = new Byte [0];
				else 
					_data = value;
			}
		}		
		public short DataSize
		{
			get 
			{
				return (short)_data.Length;
			}
		}
		public byte[] DataSizeBytes
		{
			get { return BitConverter.GetBytes(IPAddress.HostToNetworkOrder(this.DataSize));  }
		}
		#endregion

		#region ---( GetBytes Method )---
		public Byte[] GetBytes() 
		{
			short crc;
			Byte[] packet;
			
			packet = new Byte[this.Length];

			// Head
			packet[VSCP_POS_HEAD]= this.HeadByte;

			// Event Class and Type
			Buffer.BlockCopy (
				BitConverter.GetBytes (IPAddress.HostToNetworkOrder(this.EventClass)),
				0, packet,
				EventMessage.VSCP_POS_CLASS,
				EventMessage.VSCP_SIZE_CLASS);

			Buffer.BlockCopy (
				BitConverter.GetBytes (IPAddress.HostToNetworkOrder(this.EventType)),
				0, packet,
				EventMessage.VSCP_POS_TYPE,
				EventMessage.VSCP_SIZE_TYPE);


			// Address 
			Buffer.BlockCopy (this.AddressBytes, 0, packet, VSCP_POS_ADDRESS, VSCP_SIZE_ADDRESS);

			// Data Size
			Buffer.BlockCopy (this.DataSizeBytes, 0, packet, VSCP_POS_DATA_SIZE, VSCP_SIZE_DATA_SIZE);

			// Data - limited to 512-25=487 bytes
			Buffer.BlockCopy (this.DataBytes, 0, packet, VSCP_POS_DATA, this.DataSize);

			if (! _ignoreCRC)
			{
				crc = EventHelper.CalcCRC(packet, packet.Length - VSCP_SIZE_CRC);
				Buffer.BlockCopy (
					BitConverter.GetBytes(IPAddress.HostToNetworkOrder(crc)),
					0, packet, packet.Length - VSCP_SIZE_CRC, VSCP_SIZE_CRC);
			}

			return packet;
		}
		#endregion

		#region ---( ToString )---
		public override string ToString () 
		{
            StringBuilder sb = new StringBuilder();
            string sBytes = "{";
            for (int i = 0; i < GetBytes().Length; i++ )
            {
                sb.AppendFormat("{0:X2} ", GetBytes()[i]);
            }

            sBytes += sb.ToString();
            // Kosmetik
            sBytes.TrimEnd(new char[] { ' ' });
            sBytes += "}";

            return string.Format("Class={0}, Type={1}, Priority={2}, Head={3}, Data={4}", EventClass, EventType, Priority, HeadByte, sBytes);
		}
		#endregion
	}
	#endregion
}