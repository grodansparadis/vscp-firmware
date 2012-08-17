using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Windows.Forms;

namespace VSCP
{
    class daemon
    {
        private TcpClient VSCPServer;
        private StreamReader Dreader;  //Daemon reader
        public List<string> daemonLog = new List<string>();

        byte[] WriteBuffer = new byte[1024];
        string ReadBuffer = " ";
        ASCIIEncoding enc = new System.Text.ASCIIEncoding();
                
        public bool Connect(string IP)
        {
            IPAddress ipAddr;
            ipAddr = IPAddress.Parse(IP);
            VSCPServer = new TcpClient();

            try
            {
                VSCPServer.Connect(ipAddr, 9598);
            }
            catch (SocketException)
            {
                return false;// MessageBox.Show("connection failed");
            }


            if (VSCPServer.Connected)
            {
                NetworkStream VSCPstream = VSCPServer.GetStream();
                Dreader = new StreamReader(VSCPstream);
                //MessageBox.Show("TCP connected");
                ReadBuffer = "";
                while (ReadBuffer != "+OK - Success.")
                {
                    ReadBuffer = Dreader.ReadLine();
                    daemonLog.Add (ReadBuffer);
                }
                //MessageBox.Show("Daemon connected");
                WriteBuffer = enc.GetBytes("user admin\r\n");
                VSCPstream.Write(WriteBuffer, 0, WriteBuffer.Length);
                ReadBuffer = Dreader.ReadLine();
                daemonLog.Add(ReadBuffer);
                if (ReadBuffer.StartsWith("+"))
                {
                    //MessageBox.Show("USER OK");
                    WriteBuffer = enc.GetBytes("pass secret\r\n");
                    VSCPstream.Write(WriteBuffer, 0, WriteBuffer.Length);
                }
                else return false;
                ReadBuffer = Dreader.ReadLine();
                daemonLog.Add(ReadBuffer); 
                if (ReadBuffer.StartsWith("+"))
                {
                    return true;//MessageBox.Show("CONNECTION ESTABLISHED");
                }
                else return false;

            }
            else return false;//MessageBox.Show("connection not ok");
        }
        public bool Disconnect()
        {
            if (VSCPServer.Connected)
            {
                NetworkStream VSCPstream = VSCPServer.GetStream();
                WriteBuffer = enc.GetBytes("quit\r\n");
                VSCPstream.Write(WriteBuffer, 0, WriteBuffer.Length);
                ReadBuffer = Dreader.ReadLine();
                daemonLog.Add(ReadBuffer);
                if (ReadBuffer.StartsWith("+"))
                VSCPServer.Close();
                return true;

            }
            else return false;

        }
        public string Do(string _command)  //perform a command
        {
            string temp = " ";
            NetworkStream VSCPstream = VSCPServer.GetStream();
            //VSCPstream.ReadTimeout = 10;
            while (VSCPstream.DataAvailable)
            {
                temp = Dreader.ReadLine();
            }
            WriteBuffer = enc.GetBytes(_command + "\r\n");
            VSCPstream.Write(WriteBuffer, 0, WriteBuffer.Length);
            temp = Dreader.ReadLine();
            ReadBuffer = Dreader.ReadLine();
            return temp; 
            //return ReadBuffer;
            //listBox1.Items.Add(ReadBuffer);
        }

        public string RecEvent()
        {
            //NetworkStream VSCPstream = VSCPServer.GetStream();
            return Do("RETR");
        }

        public int EventAvailable()
        {
            int i = 0;
            string temp = (Do("CDTA"));
            try
            {
                i = int.Parse(temp);
            }
            catch {return 0;}
            return i;

 
            
        }

        public void EventSend()
        {
            NetworkStream VSCPstream = VSCPServer.GetStream();
            //MessageBox.Show(buffer);
            WriteBuffer = enc.GetBytes("send " + buffer + "\r\n");
            VSCPstream.Write(WriteBuffer, 0, WriteBuffer.Length);
            //WriteBuffer = enc.GetBytes("send " + buffer);
        }

        public void EventSender( int _class, int _type, string _data)
        {
            NetworkStream VSCPstream = VSCPServer.GetStream();
            //convert char array to string
            //string tempdata = string.co
            string writestring = string.Concat("0,", _class, ",", _type, ",0,0,-,", _data);
            WriteBuffer = enc.GetBytes(string.Concat("send 0,", _class, ",", _type, ",0,0,-,", _data, "\r\n"));
            VSCPstream.Write(WriteBuffer, 0, WriteBuffer.Length);
            ReadBuffer = Dreader.ReadLine(); //only added to process the +ok...
            daemonLog.Add(ReadBuffer);
        }


        #region event data
        //private int _priority;
        private int _class;
        private int _type;
        //private int _oadrr;
        private string _data;
        
        /*public int priority
        {
            get { return _priority; }
            set { _priority = value; }
        }*/
        public int L1class
        {
            get { return _class; }
            set { _class = value; }
        }
        public int type
        {
            get { return _type; }
            set { _type = value; }
        }
        /*public int oadrr
        {
            get { return _oadrr; }
            set { _oadrr = value; }
        }*/
        public string buffer
        {
            get
            {
                return (string.Concat("0,", _class, ",", _type, ",0,0,-,", _data));
            }
        }
        public string data
        {
            get { return _data; }
            set { _data = value; }
        }





}
#endregion

    // based lists upon \m2m\trunk\src\vscp\common\vscp_class.h
    class L1classes
    {
        public const int CLASS1_PROTOCOL          = 0;
        public const int CLASS1_ALARM             = 1;
        public const int CLASS1_SECURITY          = 2;
        public const int CLASS1_MEASUREMENT       = 10;
        public const int CLASS1_DATA              = 15;
        public const int CLASS1_INFORMATION       = 20;
        public const int CLASS1_CONTROL           = 30;
        public const int CLASS1_MULTIMEDIA        = 40;
        public const int CLASS1_AOL               = 50;
        public const int CLASS1_PHONE             = 100;
        public const int CLASS1_LIN               = 101;
        public const int CLASS1_DISPLAY           = 102;
        public const int CLASS1_RC5               = 110;
        public const int CLASS1_ONEWIRE           = 200;
        public const int CLASS1_X10               = 201;
        public const int CLASS1_LON               = 202;
        public const int CLASS1_EIB               = 203;
        public const int CLASS1_SNAP              = 204;
        public const int CLASS1_MUMIN             = 205;
        public const int CLASS1_LOG               = 509;
        public const int CLASS1_LAB               = 510;
        public const int CLASS1_LOCAL             = 511;
    }

    class events
    {
        public const int PROTOCOL_SEGCTRL_HEARTBEAT         = 1;
        public const int PROTOCOL_NEW_NODE_ONLINE           = 2;
        public const int PROTOCOL_PROBE_ACK                 = 3;
        public const int PROTOCOL_SET_NICKNAME              = 6;
        public const int PROTOCOL_NICKNAME_ACCEPTED         = 7;
        public const int PROTOCOL_DROP_NICKNAME             = 8;
        public const int PROTOCOL_READ_REGISTER             = 9;
        public const int PROTOCOL_RW_RESPONSE               = 10;
        public const int PROTOCOL_WRITE_REGISTER            = 11;
        public const int PROTOCOL_ENTER_BOOT_LOADER         = 12;
        public const int PROTOCOL_ACK_BOOT_LOADER           = 13;
        public const int PROTOCOL_NACK_BOOT_LOADER          = 14;
        public const int PROTOCOL_START_BLOCK               = 15;
        public const int PROTOCOL_BLOCK_DATA                = 16;
        public const int PROTOCOL_BLOCK_DATA_ACK            = 17;
        public const int PROTOCOL_BLOCK_DATA_NACK           = 18;
        public const int PROTOCOL_PROGRAM_BLOCK_DATA        = 19;
        public const int PROTOCOL_PROGRAM_BLOCK_DATA_ACK    = 20;
        public const int PROTOCOL_PROGRAM_BLOCK_DATA_NACK   = 21;
        public const int PROTOCOL_ACTIVATE_NEW_IMAGE        = 22;
        public const int PROTOCOL_RESET_DEVICE              = 23;
    }
}

// old code
    class Event99
    {
        //private int _priority;
        private int _class;
        private int _type;
        //private int _oadrr;
        private string _data;

         
        /*public int priority
        {
            get { return _priority; }
            set { _priority = value; }
        }*/
        public int L1class
        {
            get { return _class; }
            set { _class = value; }
        }
        public int type
        {
            get { return _type; }
            set { _type = value; }
        }
        /*public int oadrr
        {
            get { return _oadrr; }
            set { _oadrr = value; }
        }*/
        public string buffer
        {
            get
            {
            //string temp = "0,"; //head always zero
            return (string.Concat("0,", _class,",",_type,",0,0,-,",_data,"\r\n"));
                //  return "0,20,3,0,0,0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15,0,1,35\r\n"; 
                //temp += ToString(_class);
            }
        }
        public string data
        {
            get {return _data; }
            set {_data = value; }
        }
  /*      public string send()
        {
            NetworkStream VSCPstream = VSCPServer.GetStream();
            WriteBuffer = enc.GetBytes("send " + myEvent.buffer);
        }
        */
    }

