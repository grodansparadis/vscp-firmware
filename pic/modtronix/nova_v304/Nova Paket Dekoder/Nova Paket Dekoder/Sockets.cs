#region ---( Includes )---
using System;
using System.Net;
using System.Threading;
using System.Net.Sockets;
using System.Diagnostics;
using System.Runtime.InteropServices;
#endregion

namespace Xedia.Net.Sockets
{
	#region ---( UDPMessageListener Class )---
	/// <summary>
	/// Summary description for MessageListner.
	/// </summary>
	public sealed class UDPMessageListener : IDisposable
	{
		#region ---( DllImports )---
#if NETCF
		private const string _winsockDLL = "winsock.dll";
		private const string _kernelDLL = "coredll.dll";
#else
		private const string _winsockDLL = "Ws2_32.dll";
		private const string _kernelDLL = "kernel32.dll";		
#endif		

		[DllImport(_winsockDLL, SetLastError=true)]
		internal static extern int setsockopt(
			[In] IntPtr socketHandle,
			[In] SocketOptionLevel optionLevel,
			[In] SocketOptionName optionName,
			[In] ref int optionValue,
			[In] int optionLength);
		/// <summary>
		/// The LoadLibrary function maps the specified executable module into the address space of the calling process.
		/// </summary>
		/// <param name="lpLibFileName">Pointer to a null-terminated string that names the executable module (either a .dll or .exe file). The name specified is the file name of the module and is not related to the name stored in the library module itself, as specified by the LIBRARY keyword in the module-definition (.def) file.</param>
		/// <returns>If the function succeeds, the return value is a handle to the module.<br></br><br>If the function fails, the return value is NULL. To get extended error information, call Marshal.GetLastWin32Error.</br></returns>
		[ DllImport(_kernelDLL)]
		private static extern IntPtr LoadLibrary(string lpLibFileName);
		/// <summary>
		/// The FreeLibrary function decrements the reference count of the loaded dynamic-link library (DLL). When the reference count reaches zero, the module is unmapped from the address space of the calling process and the handle is no longer valid.
		/// </summary>
		/// <param name="hLibModule">Handle to the loaded DLL module. The LoadLibrary or GetModuleHandle function returns this handle.</param>
		/// <returns>If the function succeeds, the return value is nonzero.<br></br><br>If the function fails, the return value is zero. To get extended error information, call Marshal.GetLastWin32Error.</br></returns>
		[ DllImport(_kernelDLL)]
		private static extern int FreeLibrary(IntPtr hLibModule);
		/// <summary>
		/// The GetProcAddress function retrieves the address of an exported function or variable from the specified dynamic-link library (DLL).
		/// </summary>
		/// <param name="hModule">Handle to the DLL module that contains the function or variable. The LoadLibrary or GetModuleHandle function returns this handle.</param>
		/// <param name="lpProcName">Pointer to a null-terminated string containing the function or variable name, or the function's ordinal value. If this parameter is an ordinal value, it must be in the low-order word; the high-order word must be zero.</param>
		/// <returns>If the function succeeds, the return value is the address of the exported function or variable.<br></br><br>If the function fails, the return value is NULL. To get extended error information, call Marshal.GetLastWin32Error.</br></returns>
		[ DllImport(_kernelDLL)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);
		#endregion

		#region ---( Delegates )---
		public delegate void OnMessageCallback (object msg);
		#endregion

		#region ---( Variables )---
		private Socket _socket;
		private bool _connectionless;
		private EndPoint _remoteEndPoint;
		private IAsyncResult _asyncResult;
		private AsyncCallback _pfnCallBack;
		private OnMessageCallback _onMessage;		
		#endregion

		#region ---( Constructors and Destructor )---
		public UDPMessageListener (int portNumber, ProtocolType type, bool connectionless)
		{
			_connectionless = connectionless;

			_remoteEndPoint = new IPEndPoint(IPAddress.Any, 0);

			//Create a UDP socket.
			_socket = new Socket(
				_remoteEndPoint.AddressFamily, 
				(connectionless) ? SocketType.Dgram : SocketType.Stream, type);

			SocketHelper.SetAddressReuseFlag (_socket, true);

			IPEndPoint receiveEP = new IPEndPoint(IPAddress.Any, portNumber);
			_socket.Bind(receiveEP);

			//			// A socket needs to be bound to an address before call to SetSocketOption
			//			socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastTimeToLive, ttl);
			//			socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership, new MulticastOption(message.Address.Address));
			//			// MulticastOption should have the Group property set
			//			MulticastOption opt = new MulticastOption("192.168.0.1");
			//			opt.Group = IPAddress.Parse("239.255.255.254");
			//			socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership, opt);

			if (connectionless)
				WaitForData ();
			else
			{
				_socket.Listen (1000);
				WaitForConnection ();
			}
		}
		~UDPMessageListener () 
		{
			this.Dispose ();
		}
		public void Dispose ()
		{
			if (_socket != null)
				_socket.Close ();

			_socket = null;
		}
		#endregion

		#region ---( Public Properties )---
		public OnMessageCallback OnMessage 
		{
			get { return _onMessage;  }
			set { _onMessage = value; }
		}
		public WaitHandle AsyncWaitHandle
		{
			get { return (_asyncResult != null) ? _asyncResult.AsyncWaitHandle  : null; }
		}
		#endregion

		#region ---( Private Methods )---
		private void OnConnectionReceived (IAsyncResult ar) 
		{
			// Retrieve the state object and the handler socket
			// from the async state object.
			Socket socket = ar.AsyncState as Socket;

			if (socket == null)
				return;

			// Get the socket that handles the client request.
			_socket = socket.EndAccept(ar);

			WaitForData ();
		}
		private void WaitForConnection () 
		{
			if (_socket == null)
				return;

			_asyncResult = _socket.BeginAccept (new AsyncCallback (this.OnConnectionReceived), _socket);
		}
		private void WaitForData()
		{
			try
			{
				if  (_pfnCallBack == null ) 
					_pfnCallBack = new AsyncCallback (OnDataReceived);

				SocketPacket theSocPkt = new SocketPacket (_socket);

				// Start listening to the m_data asynchronously
				_asyncResult = _socket.BeginReceiveFrom (theSocPkt.Buffer,
					0, theSocPkt.BufferSize,
					SocketFlags.None,
					ref _remoteEndPoint,
					_pfnCallBack, theSocPkt);
			}
			catch(SocketException exc)
			{
				Debug.WriteLine ("Error waiting for m_data: " + exc.ToString ());
			}

		}
		/// <summary>
		/// Call back method to handle incoming m_data.
		/// </summary>
		/// <param name="ar">Status of an asynchronous operation.</param>
		private void OnDataReceived (IAsyncResult ar)
		{
			int bytesRead = 0;
			byte [] readData = null;

			// Retrieve the state object and the handler socket
			// from the async state object.
			SocketPacket theSockId = ar.AsyncState as SocketPacket;

			if ((theSockId == null) || (theSockId.Socket == null))
				return;
 
			try
			{
				// Read m_data from the client socket.
				bytesRead  = theSockId.Socket.EndReceiveFrom (ar, ref _remoteEndPoint);				

				if (bytesRead > 0)
					readData = theSockId.GetBufferBytes (bytesRead);

				WaitForData();
			}
			catch (ObjectDisposedException)
			{
				Debug.WriteLine ("OnDataReceived: Socket has been closed");
			}
			catch(SocketException exc)
			{
				Debug.WriteLine ("Error receiving m_data: " + exc.ToString ());

				// If this is this is a connection socket then
				// re-use it
				if (_connectionless == false)
					WaitForConnection ();
			}
			if (readData != null)
			{
				OnMessageCallback msgCallback = _onMessage;
			
				// Processing after restarting another recieve call
				// just to speed things up
				if (msgCallback != null)
					msgCallback (readData);
			}
		}	
		#endregion

		#region ---( SocketPacket Class )---
		private class SocketPacket
		{
			#region ---( Variables )---
			private Socket _socket;
			private byte [] _buffer;			
			// Size of receive buffer.
			public static readonly int _bufferSize = 4096;
			#endregion

			#region ---( Constructor )---
			public SocketPacket (Socket socket)
			{
				_socket = socket;
				_buffer = new byte[BufferSize];
			}
			#endregion

			#region ---( Public Properties )---
			public int BufferSize 
			{
				get { return _bufferSize; }
			}
			public byte [] Buffer
			{
				get { return _buffer; }
			}
			public Socket Socket
			{
				get { return _socket; }
			}
			#endregion
			
			#region ---( Public Methods )---
			public byte [] GetBufferBytes (int len)
			{
				if (len > this.BufferSize)  
					throw new ArgumentException ("Requested length longer than expected");

				lock (_buffer)
				{
					byte [] data = new byte [len];

					System.Buffer.BlockCopy (_buffer, 0, data, 0, len);
										
					return data;
				}
			}
			#endregion
		}
		#endregion
	}
	#endregion

	#region ---( UDPClient Class )---
	public class UDPClient : UdpClient, IDisposable
	{
		#region ---( Variables )---
		private bool _throwOnError;
		#endregion

		#region ---( Constructors and Destructors )---
		public UDPClient() : this (0)
		{}
		public UDPClient(int port) : this(SocketHelper.CreateLocalEndPoint(port))
		{}
		public UDPClient(IPEndPoint localEP) : base(localEP)
		{
			if(localEP == null)
				throw new ArgumentNullException("localEP");

			SetToBroadcastMode ();
		}
		~UDPClient () 
		{
			Dispose ();
		}
		public void Dispose () 
		{
			base.Close ();
		}
		#endregion

		#region ---( Public Methods )---
		public int Send (byte [] dgram, IPEndPoint endPoint) 
		{
			if (dgram == null)
			{
				if (_throwOnError)
					throw new ArgumentNullException ("dgram");
				else
					Debug.WriteLine ("Cannot send null datagram");

				return 0;
			}

			return this.Send (dgram, dgram.Length, endPoint);
		}
		public new int Send (byte [] dgram, int length) 
		{
			int bytesSent = 0;

			if (dgram == null)
			{
				if (_throwOnError)
					throw new ArgumentNullException ("dgram");
				else
					Debug.WriteLine ("Cannot send null datagram");
			}
			else if (this.Socket != null)
				bytesSent = base.Send (dgram, length);

			return bytesSent;
		}
		public new int Send (byte [] dgram, int length, IPEndPoint endPoint)
		{
			int bytesSent = 0;

			if (dgram == null)
			{
				if (_throwOnError)
					throw new ArgumentNullException ("dgram");
				else
					Debug.WriteLine ("Cannot send null datagram");
			}
			else if (this.Socket != null)
				bytesSent = base.Send (dgram, length, endPoint);

			return bytesSent;
		}
		public byte[] SendReceive(byte[] dgram, IPEndPoint destEP, ref IPEndPoint
			remoteEP)
		{
			byte[] receive;

			if (this.Send(dgram, destEP) > 0)
				receive = this.Receive(ref remoteEP);
			else
				receive = new byte [0];

			return receive;
		}
		// This is needed to send broadcast packet
		public void SetToBroadcastMode()    
		{
			if (this.Active)
			{
				Socket.SetSocketOption (
					SocketOptionLevel.Socket, 
					SocketOptionName.Broadcast, 
					Convert.ToInt32(true));
			}
		}
		#endregion

		#region ---( Public Properties )---
		public bool ThrowOnError 
		{
			get { return _throwOnError;  }
			set { _throwOnError = value; }
		}
		public bool ReuseAddress
		{
			get
			{
				object tmpO = Socket.GetSocketOption(SocketOptionLevel.Socket,
					SocketOptionName.ReuseAddress);
				return Convert.ToBoolean(tmpO);
			}
			set
			{
				SocketHelper.SetAddressReuseFlag (Socket, value);
			}
		}

		public int ReceiveTimeout
		{
			get
			{
				return (int)Socket.GetSocketOption(SocketOptionLevel.Socket,
					SocketOptionName.ReceiveTimeout);
			}
			set
			{
				if ( value < 0 )
					value = 0;

				Socket.SetSocketOption(SocketOptionLevel.Socket,
					SocketOptionName.ReceiveTimeout, value);
			}
		}

		public int SendTimeout
		{
			get
			{
				return (int)Socket.GetSocketOption(SocketOptionLevel.Socket,
					SocketOptionName.SendTimeout);
			}
			set
			{
				if ( value < 0 )
					value = 0;

				//Uses the Socket returned by Client to set an option that is not available using UdpClient.
				Socket.SetSocketOption(SocketOptionLevel.Socket,
					SocketOptionName.SendTimeout, value);
			}
		}

		public Socket Socket
		{
			get { return base.Client; }
			set
			{
				if ( value != null )				
					base.Client = value;
			}
		}

		public bool Connected
		{
			get
			{
				return Socket.Connected;
			}
		}

		public bool IsActive
		{
			get { return this.Active; }
		}

		public IPEndPoint RemoteIPEndPoint
		{
			get
			{
				return (IPEndPoint)Socket.RemoteEndPoint;
			}
		}

		public IPEndPoint LocalIPEndPoint
		{
			get
			{
				return (IPEndPoint)Socket.LocalEndPoint;
			}
		}
		#endregion

	} //End UDPClient Class
	#endregion

	#region ---( SocketHelper Class )---
	/// <summary>
	/// Socket helper methods, most of which are included for the purpose or resolving
	/// compatibility issuers with the "Compact Framework"
	/// </summary>
	public class SocketHelper 
	{
		#region ---( DllImports )---
#if NETCF
		private const string _winsockDLL = "winsock.dll";
		private const string _kernelDLL = "coredll.dll";
#else
		private const string _winsockDLL = "Ws2_32.dll";
		private const string _kernelDLL = "kernel32.dll";		
#endif		

		[DllImport(_winsockDLL, SetLastError=true)]
		private static extern int setsockopt(
			[In] IntPtr socketHandle,
			[In] SocketOptionLevel optionLevel,
			[In] SocketOptionName optionName,
			[In] ref int optionValue,
			[In] int optionLength);
		/// <summary>
		/// The LoadLibrary function maps the specified executable module into the address space of the calling process.
		/// </summary>
		/// <param name="lpLibFileName">Pointer to a null-terminated string that names the executable module (either a .dll or .exe file). The name specified is the file name of the module and is not related to the name stored in the library module itself, as specified by the LIBRARY keyword in the module-definition (.def) file.</param>
		/// <returns>If the function succeeds, the return value is a handle to the module.<br></br><br>If the function fails, the return value is NULL. To get extended error information, call Marshal.GetLastWin32Error.</br></returns>
		[ DllImport(_kernelDLL)]
		private static extern IntPtr LoadLibrary(string lpLibFileName);
		/// <summary>
		/// The FreeLibrary function decrements the reference count of the loaded dynamic-link library (DLL). When the reference count reaches zero, the module is unmapped from the address space of the calling process and the handle is no longer valid.
		/// </summary>
		/// <param name="hLibModule">Handle to the loaded DLL module. The LoadLibrary or GetModuleHandle function returns this handle.</param>
		/// <returns>If the function succeeds, the return value is nonzero.<br></br><br>If the function fails, the return value is zero. To get extended error information, call Marshal.GetLastWin32Error.</br></returns>
		[ DllImport(_kernelDLL)]
		private static extern int FreeLibrary(IntPtr hLibModule);
		/// <summary>
		/// The GetProcAddress function retrieves the address of an exported function or variable from the specified dynamic-link library (DLL).
		/// </summary>
		/// <param name="hModule">Handle to the DLL module that contains the function or variable. The LoadLibrary or GetModuleHandle function returns this handle.</param>
		/// <param name="lpProcName">Pointer to a null-terminated string containing the function or variable name, or the function's ordinal value. If this parameter is an ordinal value, it must be in the low-order word; the high-order word must be zero.</param>
		/// <returns>If the function succeeds, the return value is the address of the exported function or variable.<br></br><br>If the function fails, the return value is NULL. To get extended error information, call Marshal.GetLastWin32Error.</br></returns>
		[ DllImport(_kernelDLL)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);
		#endregion

		#region ---( Public Static Methods )---
		public static IPEndPoint CreateLocalEndPoint (int port) 
		{
			if (port < IPEndPoint.MinPort || port > IPEndPoint.MaxPort)
				throw new ArgumentOutOfRangeException ("port");

			//You must create two endpoints, one for the server that you're trying
			//to connect to, one for the local IP address that you're connecting from.
			//If you do not specify the local IP address/port that you're connecting
			//from, then you will get weird errors when using the compact framework.
			//Also note, Activesync has it's own IP address when you sync, and that 
			//can sometimes overwrite the normal IP address. So if you're getting weird
			//errors, look at what local ip address you're trying to connect from, if 
			//it's 127.0.0.1, that usually won't work.

			//Creates a UDPClient using a local end point.
            IPAddress ipAddress = Dns.GetHostEntry(Dns.GetHostName()).AddressList[0];

			if (ipAddress == IPAddress.Parse("127.0.0.1"))
				throw new Exception ("Cannot connect to network");

			IPEndPoint ipLocalEndPoint = new IPEndPoint (ipAddress, port);

			return ipLocalEndPoint;
		}
		public static void SetAddressReuseFlag (Socket socket, bool optionValue)
		{
#if (! NETCF)
			// Enable the address-reuse option so the server can be stopped and re-started without the TIME_WAIT delay.			
			socket.SetSocketOption (SocketOptionLevel.Socket,
				SocketOptionName.ReuseAddress, Convert.ToInt32(optionValue));
#else
			if (CheckEntryPoint(_winsockDLL, "setsockopt") == true) 
			{
				int option = Convert.ToInt32(optionValue);
				setsockopt(socket.Handle,
					SocketOptionLevel.Socket,
					SocketOptionName.ReuseAddress,
					ref option, 4);
			}
			else
				Debug.WriteLine ("Cannot set socket ReuseAddress option");
#endif
		}
		/// <summary>
		/// Checks whether a specified method exists on the local computer.
		/// </summary>
		/// <param name="library">The library that holds the method.</param>
		/// <param name="method">The entry point of the requested method.</param>
		/// <returns>True if the specified method is present, false otherwise.</returns>
		private static bool CheckEntryPoint(string library , string method ) 
		{
			IntPtr  libPtr = LoadLibrary(library);
			if (!libPtr.Equals(IntPtr.Zero)) 
			{
				if (!GetProcAddress(libPtr, method).Equals(IntPtr.Zero)) 
				{
					FreeLibrary(libPtr);
					return true;
				}
				FreeLibrary(libPtr);
			}
			return false;
		}
		#endregion
	}
	#endregion
}