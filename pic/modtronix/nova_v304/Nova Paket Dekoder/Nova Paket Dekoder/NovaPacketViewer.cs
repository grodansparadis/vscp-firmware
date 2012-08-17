using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

// This is a new namespace in .NET 2.0 that contains the SerialPort class
using System.IO;
using System.IO.Ports;

using BRM_Tools;
using BRM_vscp_message;
using PersistentForms;
using Xedia.Net.Sockets;
using Xedia.Protocol.VSCP;

/// Tool to display messages created by Nova: http://www.vscp.org/wiki/doku.php?id=nova
/// 
/// ***************************************************************************
/// Copyright (c) 2007 and beyond Robert M. Bouwens, CH-5425 Schneisingen, Switzerland.
/// 
/// Written by Robert M. Bouwens, 
/// 
/// This code is free for private use.
/// 
/// For comments mailto:bouwens.mehl@bluewin.com

namespace Nova_Paket_Dekoder
{
    public partial class NovaPacketViewer : Form
    {
        private UDPMessageListener m_packet_listener;
        private CTracer m_tracer;
        private PersistentForm persistentForm;
        private message_frame serial_vscp_message = new message_frame();
    	private string m_sFileName = "tcp_logger.txt";

        private CVscpMsgMap m_msgCracker;
        protected Dictionary<string, EventCode> m_eventCodeNameToVal;
        protected Dictionary<string, EVENT_PRIORITY> m_eventPriorityToval;

        public NovaPacketViewer()
        {
            InitializeComponent();
            persistentForm = new PersistentForm(this, "Bouwens Engineering\\Nova Packet Viewer\\Settings");
            m_msgCracker = new CVscpMsgMap();
            m_eventCodeNameToVal = new Dictionary<string,EventCode>();
            m_eventPriorityToval = new Dictionary<string,EVENT_PRIORITY>();
        }

        private void OnFormLoad(object sender, EventArgs e)
        {
            foreach (string sPort in SerialPort.GetPortNames())
                m_cbx_serialPort.Items.Add(sPort);

            if (m_cbx_serialPort.Items.Count > 0)
            {
                m_cbx_serialPort.SelectedIndex = 0;
            }

            // eventcode
            foreach (EventCode event_code in Enum.GetValues(typeof(EventCode)))
            {
                Console.WriteLine(event_code);
                cbxEventCode.Items.Add(event_code);
                m_eventCodeNameToVal.Add( event_code.ToString(), event_code);
            }
            cbxEventCode.SelectedIndex = 2;


            // priority
            foreach (EVENT_PRIORITY priority in Enum.GetValues(typeof(EVENT_PRIORITY)))
            {
                Console.WriteLine(priority);
                cbxPriority.Items.Add(priority);
                m_eventPriorityToval.Add(priority.ToString(), priority);
            }
            cbxPriority.SelectedIndex = 3;

            for (int i = 0; i < cbxBaud.Items.Count; i++)
            {
                if ( "115200" == cbxBaud.Items[i].ToString())
                {
                    cbxBaud.SelectedIndex = i;
                    break;
                }
            }
        }

        private void btn_start_Click(object sender, EventArgs e)
        {
            if ( "Start capturing"  == btn_start.Text )
            {
                m_packet_listener = new UDPMessageListener(int.Parse(tb_port.Text), System.Net.Sockets.ProtocolType.Udp, true);
                m_tracer = new CTracer(m_sFileName, true);
                m_packet_listener.OnMessage += new UDPMessageListener.OnMessageCallback(OnReceivedMessage);
                btn_start.Text = "Stop capturing";

                m_serialPort.PortName = m_cbx_serialPort.SelectedItem.ToString();
                m_serialPort.BaudRate = int.Parse(cbxBaud.Text);
                m_serialPort.Open();
            }
            else
            {
                m_tracer.Close();
                m_packet_listener = null;
                m_tracer = null;
                btn_start.Text = "Start capturing";
                m_serialPort.Close();
            }
        }

        private void OnReceivedMessage( object i_msg )
        {
            DateTime now = DateTime.Now;
            // Create message so CRC is checked if needed
            EventMessage msg = new EventMessage(i_msg as byte[]);
            string sMsg = msg.ToString();

            string sTime = now.ToLongTimeString() + ":" + now.Millisecond;
            if ( null != m_tracer )
            {
                m_tracer.Write(sTime + " " + sMsg);

                ListViewItem item = new ListViewItem(sTime, sMsg );
                item.SubItems.Add(msg.ToString());
                m_listView.AddItem(item);

                string sCrack = m_msgCracker.Decode(msg.EventCode.ToString(), msg.DataBytes);
                if (sCrack.Length > 0)
                {
                    ListViewItem item2 = new ListViewItem(sTime, sMsg);
                    item2.SubItems.Add(sCrack);
                    m_listView.AddItem(item2);
                }
            }
        }

        private void OnDataReceive_serialPort(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            try
            {
                // binary mode 
                int bytesAvailable = m_serialPort.BytesToRead;
                byte[] data = new byte[bytesAvailable];
                m_serialPort.Read(data, 0, data.Length);
                ExtractSerialVSCPMessage(bytesAvailable, data);
            }
            catch (IOException ex)
            {
                Debug.WriteLine("IOException: " + ex.Message);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                //MessageBox.Show(ex.Message);
            }
        }

        private void ExtractSerialVSCPMessage(int i_length, byte[] i_data)
        {
            //lock (this)
            {
                // If Receive RS-232 m_data available
                for (int i = 0; i < i_length; i++)
                {
                    Byte c = i_data[i];
                    if (serial_vscp_message.AddByte(c))
                    {
                        // a new serial vscp message
                        DateTime now = DateTime.Now;
                        // Create message so CRC is checked if needed
                        SerialEventMessage msg = new SerialEventMessage(serial_vscp_message);
                        string sMsg = msg.ToString();

                        string sTime = now.ToLongTimeString() + ":" + now.Millisecond;
                        if (null != m_tracer)
                        {
                            m_tracer.Write(sTime + " " + sMsg);

                            ListViewItem item = new ListViewItem(sTime, sMsg);
                            item.SubItems.Add(msg.ToString());
                            m_listView.AddItem(item);

                            string sEventName = msg.m_event.ToString();
                            byte[] dataBytes = msg.GetDataBytes();
                            string sCrack = m_msgCracker.Decode(sEventName, dataBytes);
                            if (sCrack.Length > 0)
                            {
                                ListViewItem item2 = new ListViewItem(sTime, sMsg);
                                item2.SubItems.Add(sCrack);
                                m_listView.AddItem(item2);
                            }
                        }
                    }
                }
            }
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            try
            {
                // the the message to the serias interface
                char[] aSplit = { ',' };
                string sDataBytes = txtData.Text;
                string[] asData = sDataBytes.Split(aSplit);
                EventCode chosenEvent = m_eventCodeNameToVal[cbxEventCode.SelectedItem.ToString()];
                EVENT_PRIORITY chosenPriority = m_eventPriorityToval[cbxPriority.SelectedItem.ToString()];

                if (asData.Length == 1 && asData[0].Length == 0)
                {
                    asData = new string[0];
                }
                // length: dle, stx, flags, 4 bytes header, data bytes, dle, etx 
                int len = 9 + asData.Length;
                byte[] aVscpMsg = new byte[len];
                aVscpMsg[0] = message_frame.DLE;
                aVscpMsg[1] = message_frame.STX;
                int iTemp = (int)chosenPriority;
                aVscpMsg[2] = (byte) (iTemp << 5);
                
                iTemp = (int)chosenEvent;
                int iVal = (iTemp >> 24) & 0xFF;
                aVscpMsg[3] = (byte)iVal;

                iVal = (iTemp >> 16) & 0xFF;
                aVscpMsg[4] = (byte)iVal;

                iVal = iTemp & 0xFF;
                aVscpMsg[5] = (byte)iVal;

                // nickname is zero
                aVscpMsg[6] = 0;

                int i = 0;
                for (; i < asData.Length; i++)
                {
                    string sNumber = asData[i];
                    if (sNumber.Length > 0)
                    {
                        aVscpMsg[7 + i] = (byte)Int32.Parse(sNumber, System.Globalization.NumberStyles.HexNumber);
                    }
                    else
                    {
                        aVscpMsg[7 + i] = 0;
                    }
                }
                aVscpMsg[7 + i] = message_frame.DLE;
                i++;
                aVscpMsg[7 + i] = message_frame.ETX;

                m_serialPort.Write(aVscpMsg, 0, aVscpMsg.Length);
                message_frame generated_vscp_message = new message_frame();
                for (int j = 0; j < aVscpMsg.Length; j++)
                {
                    if (generated_vscp_message.AddByte(aVscpMsg[j]))
                    {
                        // a new serial vscp message
                        DateTime now = DateTime.Now;
                        // Create message so CRC is checked if needed
                        SerialEventMessage msg = new SerialEventMessage(generated_vscp_message);
                        string sMsg = msg.ToString();

                        string sTime = now.ToLongTimeString() + ":" + now.Millisecond;
                        if (null != m_tracer)
                        {
                            m_tracer.Write(sTime + " " + sMsg);

                            ListViewItem item = new ListViewItem(sTime, sMsg);
                            item.SubItems.Add(msg.ToString());
                            m_listView.AddItem(item);

                            string sEventName = msg.m_event.ToString();
                            byte[] dataBytes = msg.GetDataBytes();
                            string sCrack = m_msgCracker.Decode(sEventName, dataBytes);
                            if (sCrack.Length > 0)
                            {
                                ListViewItem item2 = new ListViewItem(sTime, sMsg);
                                item2.SubItems.Add(sCrack);
                                m_listView.AddItem(item2);
                            }
                        }
                    }
                }
            }
            catch (IOException ex)
            {
                Debug.WriteLine("IOException: " + ex.Message);
                MessageBox.Show(ex.Message);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                MessageBox.Show(ex.Message);
            }
        }

        private void btnClearListView_Click(object sender, EventArgs e)
        {
            m_listView.Items.Clear();
        }
    }
}