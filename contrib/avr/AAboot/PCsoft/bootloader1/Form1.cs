using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;



namespace VSCP
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        public TcpClient VSCPServer;
        //private StreamReader Dreader;  //Daemon reader
        byte[] WriteBuffer = new byte[1024];
        //string ReadBuffer = " ";
        ASCIIEncoding enc = new System.Text.ASCIIEncoding();
        VSCP.daemon myDaemon = new VSCP.daemon();

        private void buttonConnect_Click(object sender, EventArgs e)
        {
            if (myDaemon.Connect("127.0.0.1"))
            {
                button_version.Enabled = true;
                buttonConnect.Enabled = false;
                listBox1.Items.Add("TCP connected");
            }
            else MessageBox.Show("Connection failed");
        }

        private void buttonDisconnect_Click(object sender, EventArgs e)
        {
            if (myDaemon.Disconnect())
            {
                button_version.Enabled = false;
                buttonConnect.Enabled = true;
                listBox1.Items.Add("TCP disconnected");
            }
        }

        private void button_version_Click(object sender, EventArgs e)
        {
            listBox1.Items.Add("Daemon Version: " + myDaemon.Do("vers"));

        }

        private void button_event_Click(object sender, EventArgs e)
        {
            myDaemon.type = VSCP.events.PROTOCOL_NEW_NODE_ONLINE;
            myDaemon.L1class = VSCP.L1classes.CLASS1_PROTOCOL;
            myDaemon.data = nodeIdBox.SelectedItem.ToString();
            listBox1.Items.Add(myDaemon.buffer);
            myDaemon.EventSend();
            System.Threading.Thread.Sleep(1000);


            if (myDaemon.EventAvailable() != 0)
            {
                string s;
                s = (myDaemon.RecEvent());
                listBox1.Items.Add(s);
                string[] sa = s.Split(',');
                //char[] c = s.ToCharArray();
                if ((sa[1].Equals("0")) & (sa[2].Equals("3")))
                {
                    MessageBox.Show("node " + nodeIdBox.SelectedItem.ToString() + " ACK");
                }

            }



        }

        private void button_scan_Click(object sender, EventArgs e)
        {
            nodeIdBox.Items.Clear();
            for (int j=0 ; j < 25 ; j++ )
            {
                myDaemon.type = VSCP.events.PROTOCOL_NEW_NODE_ONLINE;
                myDaemon.L1class = VSCP.L1classes.CLASS1_PROTOCOL;
                myDaemon.data = j.ToString();
                listBox1.Items.Add(myDaemon.buffer);
                myDaemon.EventSend();
                System.Threading.Thread.Sleep(1000);
                
                if (myDaemon.EventAvailable() != 0)
                {
                    string s;
                    s =(myDaemon.RecEvent());
                    listBox1.Items.Add(s);
                    string[] sa = s.Split(',');
                    //char[] c = s.ToCharArray();
                    if ((sa[1].Equals("0")) & (sa[2].Equals("3")))
                    {
                        MessageBox.Show("node " + j.ToString() +  " ACK");
                        nodeIdBox.Items.Add(j.ToString());
                    }

                }
           }
    }

 

        private void openHEX_Click(object sender, EventArgs e)
        {
            openHEXFileDialog.ShowDialog();
            try
            {
                File.OpenRead(openHEXFileDialog.FileName);
            }
            catch (Exception)
            {
                MessageBox.Show ("no file open");
            }
        }
        public string FilebufferString;
        private void openHEXFileDialog_FileOk(object sender, CancelEventArgs e)
        {
            // todo: check if file is a valid hex file
            byte[] Filebuffer;
            //char[] FilebufferChar;
            FileStream fs = new FileStream(openHEXFileDialog.FileName, FileMode.Open, FileAccess.Read, FileShare.None);
            // read file into array of bytes
            Filebuffer = new byte[fs.Length];
            fs.Read(Filebuffer, 0, Filebuffer.Length);
            fs.Close();
            char[] FilebufferChar = new char[Filebuffer.Length];
            // Convert the new byte[] into a char[]
            UTF8Encoding.UTF8.GetChars(Filebuffer, 0, Filebuffer.Length, FilebufferChar, 0 );
            // and now to string
            FilebufferString = new string(FilebufferChar);
        }

        private void ButtonUpload_Click(object sender, EventArgs e)
        {
            char[] tempchar = new char[18];
            string tempstring;
            int succesfulblocks = 0;
            int expectedblocks = 0;

            myDaemon.type = VSCP.events.PROTOCOL_ENTER_BOOT_LOADER;
            myDaemon.L1class = VSCP.L1classes.CLASS1_PROTOCOL;
            tempstring = nodeIdBox.SelectedItem.ToString();
            tempchar[0] = Convert.ToChar(tempstring);
            tempchar[1] = ',';
            tempstring = algorithm.SelectedItem.ToString();
            tempchar[2] = Convert.ToChar(tempstring);
            tempchar[3] = ',';
            tempstring = GUID0.Value.ToString();
            tempchar[4] = Convert.ToChar(tempstring);
            tempchar[5] = ',';
            tempstring = GUID3.Value.ToString();
            tempchar[6] = Convert.ToChar(tempstring);
            tempchar[7] = ',';
            tempstring = GUID5.Value.ToString();
            tempchar[8] = Convert.ToChar(tempstring);
            tempchar[9] = ',';
            tempstring = GUID7.Value.ToString();
            tempchar[10] = Convert.ToChar(tempstring);
            tempchar[11] = ',';
            tempstring = REGx92.Value.ToString();
            tempchar[12] = Convert.ToChar(tempstring);
            tempchar[13] = ',';
            tempstring = REGx93.Value.ToString();
            tempchar[14] = Convert.ToChar(tempstring);
            tempchar[15] = ',';
            tempchar[16] = '0';
            tempchar[17] = '9';
            tempstring = new string(tempchar);
            myDaemon.data = tempstring;
            myDaemon.EventSend();
            System.Threading.Thread.Sleep(1000);
            listBox1.Items.Add("waiting");

            bool REC_ACK = false; //! set to true only to check file processing
            //wait for BOOT_ACK
            while (REC_ACK == false) //todo.here should be a timeout
            {
                if (myDaemon.EventAvailable() != 0)
                {
                    listBox1.Items.Add("event");
                    string s;
                    s = (myDaemon.RecEvent());
                    listBox1.Items.Add(s);
                    string[] sa = s.Split(',');
                    if ((sa[1].Equals("0")) & (sa[2].Equals("13")))
                    {
                        REC_ACK = true;
                        string blocksizeS = sa[8] + sa[9];
                        listBox1.Items.Add("blocksize = " + blocksizeS);
                        expectedblocks = Convert.ToInt32(sa[13]);
                        listBox1.Items.Add("nr of blocks = " + expectedblocks);
                    }
                } 
            }
            System.Threading.Thread.Sleep(500); // value correct?

            //start sending pages, divided in blocks of 8bytes
            string page = "";
            int nroflines = 0;
            //determine nr of lines, based on Start of line char ":"
            for (int i = 0; i < FilebufferString.Length-1; i++)
            {
                if (FilebufferString.Substring(i, 1) == ":")
                {
                    nroflines += 1;
                }

            }
            
            //create array with an element for each line
            string[] linebuffer = new string[nroflines];

            // convert the filebuffer into the linebuffer
            int p = 1;
            int lp = 0;
            for (uint l = 0; l < nroflines; l++)
            {
                if (l+1 < nroflines)
                {
                    while (FilebufferString.Substring(lp + p + 1, 1) != ":")
                    {
                        linebuffer[l] = FilebufferString.Substring(lp, p++);
                    }
                    lp = lp + p + 1;
                    p = 1;
                }
                if (l+1 == nroflines)
                {
                    while (lp+p+1 < FilebufferString.Length)
                    {
                        linebuffer[l] = FilebufferString.Substring(lp, p++);
                    }
                }
            }
            //equalize line lengths
            //all lines should be 16 data-bytes long.
            //this helps to reduce bootloader size in avr
            for (int l = 0; l < nroflines; l++)
            {
                if (linebuffer[l].Substring(1, 2) != "10")
                {

                    int length = (Int32.Parse(linebuffer[l].Substring(1, 2), System.Globalization.NumberStyles.HexNumber));
                    for (; length < 16; length++)
                    {
                        linebuffer[l] = linebuffer[l].Insert((9 + (length * 2)), "FF");
                    }
                    //todo recalculate new CRC at end of line
                }
            }
            Boolean EOFreached = false;
            for (int l = 0; l < nroflines; l++)
            {
                //chop up in pages
                // if pages are not complete, fill them up with 0xFF
                //todo check if firstline start with xx0 as addres, if not add blanks berfore it
                string firstline = linebuffer[l];
                page = "0" + firstline.Substring(2, 3);
                
                string pagebuffer = "";// firstline.Substring(9, 32);  //add line to buffer
                int firstadress = (Int32.Parse(firstline.Substring(3, 3), System.Globalization.NumberStyles.HexNumber));
                string nextline = linebuffer[l + 1];
                int nextadress = (Int32.Parse(nextline.Substring(3, 3), System.Globalization.NumberStyles.HexNumber));

                while (nextadress <= (firstadress + 0x0F))
                {
                    string newline = linebuffer[l];
                    if (newline.Substring(0,9) != ":00000001") //EOF reached?
                    {
                        pagebuffer += newline.Substring(9, 32);
                        l++;
                        nextline = linebuffer[l];
                        if (nextline.Substring(0, 9) != ":00000001")
                        {
                            nextadress = (Int32.Parse(nextline.Substring(3, 3), System.Globalization.NumberStyles.HexNumber));
                        }
                        else nextadress++;
                    }
                    else //add blank lines after EOF
                    {
                        EOFreached = true;
                        pagebuffer += "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
                        nextadress++;
                    }
                }
                if (!EOFreached) l--;
                


                // now send the page over
                pagesender(page, pagebuffer);
                //wait for confirm_block event
                bool blockrec = false;
                while (blockrec != true) //todo timeout
                {
                    if (myDaemon.EventAvailable() != 0)
                    {
                        string s;
                        s = (myDaemon.RecEvent());
                        string[] sa = s.Split(',');
                        //char[] c = s.ToCharArray();
                        if ((sa[1].Equals("0")) & (sa[2].Equals("17")))
                        {
                            blockrec = true;
                        }
                    }
                }
                //send program block event
                myDaemon.EventSender(VSCP.L1classes.CLASS1_PROTOCOL, VSCP.events.PROTOCOL_PROGRAM_BLOCK_DATA, "0");
                //wait for program block_data_ack
                blockrec = false;
                while (blockrec != true) //todo timeout
                {
                    if (myDaemon.EventAvailable() != 0)
                    {
                        string s;
                        s = (myDaemon.RecEvent());
                        string[] sa = s.Split(',');
                        //char[] c = s.ToCharArray();
                        if ((sa[1].Equals("0")) & (sa[2].Equals("20")))
                        {
                            blockrec = true;
                            succesfulblocks++;
                        }
                    }
                }
            }


            //check if blocks sent are equal to expected nr of blocks
            // if not: send empty blocks until equal
            while (succesfulblocks < expectedblocks)
            {
                string pagebuffer = "";
                for (int i = 0; i <= 0x0F; i++)
                {
                    pagebuffer += "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
                }
                //page = System.Convert.ToString(succesfulblocks, 16);
                page = (succesfulblocks.ToString("X4"));

                //pagesender(page, pagebuffer);

                // now send the page over
                pagesender(page, pagebuffer);
                //wait for confirm_block event
                bool blockrec = false;
                while (blockrec != true) //todo timeout
                {
                    if (myDaemon.EventAvailable() != 0)
                    {
                        string s;
                        s = (myDaemon.RecEvent());
                        string[] sa = s.Split(',');
                        //char[] c = s.ToCharArray();
                        if ((sa[1].Equals("0")) & (sa[2].Equals("17")))
                        {
                            blockrec = true;
                        }
                    }
                }
                //send program block event
                myDaemon.EventSender(VSCP.L1classes.CLASS1_PROTOCOL, VSCP.events.PROTOCOL_PROGRAM_BLOCK_DATA, "0");
                //wait for program block_data_ack
                blockrec = false;
                while (blockrec != true) //todo timeout
                {
                    if (myDaemon.EventAvailable() != 0)
                    {
                        string s;
                        s = (myDaemon.RecEvent());
                        string[] sa = s.Split(',');
                        //char[] c = s.ToCharArray();
                        if ((sa[1].Equals("0")) & (sa[2].Equals("20")))
                        {
                            blockrec = true;
                            succesfulblocks++;
                        }
                    }
                }
            



            }

                        
            //send activate image
            myDaemon.EventSender(VSCP.L1classes.CLASS1_PROTOCOL, VSCP.events.PROTOCOL_ACTIVATE_NEW_IMAGE, "0");
            MessageBox.Show("UPLOAD COMPLETE");
                
    }
private void pagesender(string _page, string _buffer)
{
    string tempblockbuffer = "";
    string blockbuffer = "";
    string temppage = "";
    temppage = string.Concat("0x", _page.Substring(0, 2), ",0x", _page.Substring(2, 2));
    myDaemon.EventSender(VSCP.L1classes.CLASS1_PROTOCOL, VSCP.events.PROTOCOL_START_BLOCK, temppage);
    System.Threading.Thread.Sleep(250); // value correct?
    //page should be sent in blocks of 8 bytes
    for (int p = 0; (p+16) <= _buffer.Length; p += 16)
    {
        tempblockbuffer = _buffer.Substring(p, 16); //take 16 char (=8byte) 
        // chop it up
        blockbuffer = "";
        for (int x = 0; x < 8; x++)
        {
            blockbuffer = string.Concat(blockbuffer, "0x", (tempblockbuffer.Substring(x * 2, 2)), ",");
        }
        //chop off last comma
        blockbuffer = blockbuffer.Substring(0, (blockbuffer.Length - 1));
        myDaemon.EventSender(VSCP.L1classes.CLASS1_PROTOCOL, VSCP.events.PROTOCOL_BLOCK_DATA, blockbuffer);
        System.Threading.Thread.Sleep(125); // value correct?
    }
    }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }
    }

}