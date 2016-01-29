using BRM_Tools;

namespace Nova_Paket_Dekoder
{
    partial class NovaPacketViewer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.m_tabControl = new System.Windows.Forms.TabControl();
            this.tabPage_Nova = new System.Windows.Forms.TabPage();
            this.label8 = new System.Windows.Forms.Label();
            this.cbxBaud = new System.Windows.Forms.ComboBox();
            this.m_cbx_serialPort = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.btn_start = new System.Windows.Forms.Button();
            this.tb_port = new System.Windows.Forms.TextBox();
            this.tb_ip_address = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage_Viewer = new System.Windows.Forms.TabPage();
            this.m_listView = new BRM_Tools.SafeListView();
            this.col_time = new System.Windows.Forms.ColumnHeader();
            this.col_packet = new System.Windows.Forms.ColumnHeader();
            this.tabPage_sendMsg = new System.Windows.Forms.TabPage();
            this.btnSend = new System.Windows.Forms.Button();
            this.txtData = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cbxPriority = new System.Windows.Forms.ComboBox();
            this.cbxEventCode = new System.Windows.Forms.ComboBox();
            this.m_statusStrip = new System.Windows.Forms.StatusStrip();
            this.m_toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.m_serialPort = new System.IO.Ports.SerialPort(this.components);
            this.btnClearListView = new System.Windows.Forms.Button();
            this.m_tabControl.SuspendLayout();
            this.tabPage_Nova.SuspendLayout();
            this.tabPage_Viewer.SuspendLayout();
            this.tabPage_sendMsg.SuspendLayout();
            this.m_statusStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_tabControl
            // 
            this.m_tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tabControl.Controls.Add(this.tabPage_Nova);
            this.m_tabControl.Controls.Add(this.tabPage_Viewer);
            this.m_tabControl.Controls.Add(this.tabPage_sendMsg);
            this.m_tabControl.Location = new System.Drawing.Point(0, 0);
            this.m_tabControl.Name = "m_tabControl";
            this.m_tabControl.SelectedIndex = 0;
            this.m_tabControl.Size = new System.Drawing.Size(1136, 341);
            this.m_tabControl.TabIndex = 0;
            // 
            // tabPage_Nova
            // 
            this.tabPage_Nova.Controls.Add(this.btnClearListView);
            this.tabPage_Nova.Controls.Add(this.label8);
            this.tabPage_Nova.Controls.Add(this.cbxBaud);
            this.tabPage_Nova.Controls.Add(this.m_cbx_serialPort);
            this.tabPage_Nova.Controls.Add(this.label3);
            this.tabPage_Nova.Controls.Add(this.btn_start);
            this.tabPage_Nova.Controls.Add(this.tb_port);
            this.tabPage_Nova.Controls.Add(this.tb_ip_address);
            this.tabPage_Nova.Controls.Add(this.label2);
            this.tabPage_Nova.Controls.Add(this.label1);
            this.tabPage_Nova.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Nova.Name = "tabPage_Nova";
            this.tabPage_Nova.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Nova.Size = new System.Drawing.Size(1128, 315);
            this.tabPage_Nova.TabIndex = 0;
            this.tabPage_Nova.Text = "Nova";
            this.tabPage_Nova.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(18, 113);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(64, 16);
            this.label8.TabIndex = 15;
            this.label8.Text = "Baud";
            this.label8.UseMnemonic = false;
            // 
            // cbxBaud
            // 
            this.cbxBaud.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbxBaud.Items.AddRange(new object[] {
            "75",
            "110",
            "134",
            "150",
            "300",
            "600",
            "1200",
            "1800",
            "2400",
            "4800",
            "7200",
            "9600",
            "14400",
            "19200",
            "38400",
            "57600",
            "115200",
            "128000"});
            this.cbxBaud.Location = new System.Drawing.Point(120, 108);
            this.cbxBaud.Name = "cbxBaud";
            this.cbxBaud.Size = new System.Drawing.Size(100, 21);
            this.cbxBaud.TabIndex = 14;
            // 
            // m_cbx_serialPort
            // 
            this.m_cbx_serialPort.FormattingEnabled = true;
            this.m_cbx_serialPort.Location = new System.Drawing.Point(120, 81);
            this.m_cbx_serialPort.Name = "m_cbx_serialPort";
            this.m_cbx_serialPort.Size = new System.Drawing.Size(100, 21);
            this.m_cbx_serialPort.Sorted = true;
            this.m_cbx_serialPort.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(18, 89);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(58, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Serial Port:";
            // 
            // btn_start
            // 
            this.btn_start.Location = new System.Drawing.Point(120, 144);
            this.btn_start.Name = "btn_start";
            this.btn_start.Size = new System.Drawing.Size(100, 23);
            this.btn_start.TabIndex = 4;
            this.btn_start.Text = "Start capturing";
            this.btn_start.UseVisualStyleBackColor = true;
            this.btn_start.Click += new System.EventHandler(this.btn_start_Click);
            // 
            // tb_port
            // 
            this.tb_port.Location = new System.Drawing.Point(120, 43);
            this.tb_port.Name = "tb_port";
            this.tb_port.Size = new System.Drawing.Size(100, 20);
            this.tb_port.TabIndex = 3;
            this.tb_port.Text = "9598";
            // 
            // tb_ip_address
            // 
            this.tb_ip_address.Location = new System.Drawing.Point(120, 23);
            this.tb_ip_address.Name = "tb_ip_address";
            this.tb_ip_address.Size = new System.Drawing.Size(100, 20);
            this.tb_ip_address.TabIndex = 2;
            this.tb_ip_address.Text = "localhost";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(18, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Port:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(18, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "IP Address:";
            // 
            // tabPage_Viewer
            // 
            this.tabPage_Viewer.Controls.Add(this.m_listView);
            this.tabPage_Viewer.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Viewer.Name = "tabPage_Viewer";
            this.tabPage_Viewer.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Viewer.Size = new System.Drawing.Size(1128, 315);
            this.tabPage_Viewer.TabIndex = 1;
            this.tabPage_Viewer.Text = "Viewer";
            this.tabPage_Viewer.UseVisualStyleBackColor = true;
            // 
            // m_listView
            // 
            this.m_listView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_listView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.col_time,
            this.col_packet});
            this.m_listView.Location = new System.Drawing.Point(0, 3);
            this.m_listView.Name = "m_listView";
            this.m_listView.Size = new System.Drawing.Size(1132, 316);
            this.m_listView.TabIndex = 0;
            this.m_listView.UseCompatibleStateImageBehavior = false;
            this.m_listView.View = System.Windows.Forms.View.Details;
            // 
            // col_time
            // 
            this.col_time.Text = "Time";
            this.col_time.Width = 99;
            // 
            // col_packet
            // 
            this.col_packet.Text = "Packet received";
            this.col_packet.Width = 1010;
            // 
            // tabPage_sendMsg
            // 
            this.tabPage_sendMsg.Controls.Add(this.btnSend);
            this.tabPage_sendMsg.Controls.Add(this.txtData);
            this.tabPage_sendMsg.Controls.Add(this.label7);
            this.tabPage_sendMsg.Controls.Add(this.label6);
            this.tabPage_sendMsg.Controls.Add(this.label5);
            this.tabPage_sendMsg.Controls.Add(this.label4);
            this.tabPage_sendMsg.Controls.Add(this.cbxPriority);
            this.tabPage_sendMsg.Controls.Add(this.cbxEventCode);
            this.tabPage_sendMsg.Location = new System.Drawing.Point(4, 22);
            this.tabPage_sendMsg.Name = "tabPage_sendMsg";
            this.tabPage_sendMsg.Size = new System.Drawing.Size(1128, 315);
            this.tabPage_sendMsg.TabIndex = 2;
            this.tabPage_sendMsg.Text = "Send Msg";
            this.tabPage_sendMsg.UseVisualStyleBackColor = true;
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(159, 142);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(108, 23);
            this.btnSend.TabIndex = 4;
            this.btnSend.Text = "Send Message";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
            // 
            // txtData
            // 
            this.txtData.Location = new System.Drawing.Point(159, 92);
            this.txtData.Name = "txtData";
            this.txtData.Size = new System.Drawing.Size(177, 20);
            this.txtData.TabIndex = 3;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(356, 95);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(306, 13);
            this.label7.TabIndex = 2;
            this.label7.Text = "Comma seperated list of databytes,  hexadecimal representation";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(37, 99);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(33, 13);
            this.label6.TabIndex = 2;
            this.label6.Text = "Data:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(37, 72);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "Priority:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(37, 37);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(66, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "Event Code:";
            // 
            // cbxPriority
            // 
            this.cbxPriority.FormattingEnabled = true;
            this.cbxPriority.Location = new System.Drawing.Point(159, 64);
            this.cbxPriority.Name = "cbxPriority";
            this.cbxPriority.Size = new System.Drawing.Size(177, 21);
            this.cbxPriority.TabIndex = 0;
            // 
            // cbxEventCode
            // 
            this.cbxEventCode.FormattingEnabled = true;
            this.cbxEventCode.Location = new System.Drawing.Point(159, 37);
            this.cbxEventCode.Name = "cbxEventCode";
            this.cbxEventCode.Size = new System.Drawing.Size(177, 21);
            this.cbxEventCode.TabIndex = 0;
            // 
            // m_statusStrip
            // 
            this.m_statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_toolStripStatusLabel});
            this.m_statusStrip.Location = new System.Drawing.Point(0, 344);
            this.m_statusStrip.Name = "m_statusStrip";
            this.m_statusStrip.Size = new System.Drawing.Size(1136, 22);
            this.m_statusStrip.TabIndex = 1;
            this.m_statusStrip.Text = "statusStrip1";
            // 
            // m_toolStripStatusLabel
            // 
            this.m_toolStripStatusLabel.Name = "m_toolStripStatusLabel";
            this.m_toolStripStatusLabel.Size = new System.Drawing.Size(106, 17);
            this.m_toolStripStatusLabel.Text = "No connection yet...";
            // 
            // m_serialPort
            // 
            this.m_serialPort.BaudRate = 115200;
            this.m_serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.OnDataReceive_serialPort);
            // 
            // btnClearListView
            // 
            this.btnClearListView.Location = new System.Drawing.Point(120, 182);
            this.btnClearListView.Name = "btnClearListView";
            this.btnClearListView.Size = new System.Drawing.Size(100, 23);
            this.btnClearListView.TabIndex = 16;
            this.btnClearListView.Text = "Clear listview";
            this.btnClearListView.UseVisualStyleBackColor = true;
            this.btnClearListView.Click += new System.EventHandler(this.btnClearListView_Click);
            // 
            // NovaPacketViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1136, 366);
            this.Controls.Add(this.m_statusStrip);
            this.Controls.Add(this.m_tabControl);
            this.MinimumSize = new System.Drawing.Size(400, 400);
            this.Name = "NovaPacketViewer";
            this.Text = "Nova packet decoder";
            this.Load += new System.EventHandler(this.OnFormLoad);
            this.m_tabControl.ResumeLayout(false);
            this.tabPage_Nova.ResumeLayout(false);
            this.tabPage_Nova.PerformLayout();
            this.tabPage_Viewer.ResumeLayout(false);
            this.tabPage_sendMsg.ResumeLayout(false);
            this.tabPage_sendMsg.PerformLayout();
            this.m_statusStrip.ResumeLayout(false);
            this.m_statusStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl m_tabControl;
        private System.Windows.Forms.TabPage tabPage_Nova;
        private System.Windows.Forms.TabPage tabPage_Viewer;
        private System.Windows.Forms.StatusStrip m_statusStrip;
        private System.Windows.Forms.Button btn_start;
        private System.Windows.Forms.TextBox tb_port;
        private System.Windows.Forms.TextBox tb_ip_address;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolStripStatusLabel m_toolStripStatusLabel;
        private SafeListView m_listView;
        private System.Windows.Forms.ColumnHeader col_time;
        private System.Windows.Forms.ColumnHeader col_packet;
        private System.IO.Ports.SerialPort m_serialPort;
        private System.Windows.Forms.ComboBox m_cbx_serialPort;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TabPage tabPage_sendMsg;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cbxPriority;
        private System.Windows.Forms.ComboBox cbxEventCode;
        private System.Windows.Forms.TextBox txtData;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cbxBaud;
        private System.Windows.Forms.Button btnClearListView;
    }
}

