namespace VSCP
{
    partial class Form1
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
            this.buttonConnect = new System.Windows.Forms.Button();
            this.buttonDisconnect = new System.Windows.Forms.Button();
            this.button_version = new System.Windows.Forms.Button();
            this.button_event = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.nodeIdBox = new System.Windows.Forms.ComboBox();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.button_scan = new System.Windows.Forms.Button();
            this.openHEX = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.openHEXFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.ButtonUpload = new System.Windows.Forms.Button();
            this.GUID0 = new System.Windows.Forms.NumericUpDown();
            this.GUID3 = new System.Windows.Forms.NumericUpDown();
            this.GUID5 = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.GUID7 = new System.Windows.Forms.NumericUpDown();
            this.algorithm = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.REGx93 = new System.Windows.Forms.NumericUpDown();
            this.REGx92 = new System.Windows.Forms.NumericUpDown();
            ((System.ComponentModel.ISupportInitialize)(this.GUID0)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GUID3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GUID5)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GUID7)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.REGx93)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.REGx92)).BeginInit();
            this.SuspendLayout();
            // 
            // buttonConnect
            // 
            this.buttonConnect.Location = new System.Drawing.Point(60, 29);
            this.buttonConnect.Margin = new System.Windows.Forms.Padding(2);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(107, 19);
            this.buttonConnect.TabIndex = 0;
            this.buttonConnect.Text = "connect daemon";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
            // 
            // buttonDisconnect
            // 
            this.buttonDisconnect.Location = new System.Drawing.Point(60, 70);
            this.buttonDisconnect.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDisconnect.Name = "buttonDisconnect";
            this.buttonDisconnect.Size = new System.Drawing.Size(107, 19);
            this.buttonDisconnect.TabIndex = 1;
            this.buttonDisconnect.Text = "disconnect daemon";
            this.buttonDisconnect.UseVisualStyleBackColor = true;
            this.buttonDisconnect.Click += new System.EventHandler(this.buttonDisconnect_Click);
            // 
            // button_version
            // 
            this.button_version.Enabled = false;
            this.button_version.Location = new System.Drawing.Point(223, 28);
            this.button_version.Margin = new System.Windows.Forms.Padding(2);
            this.button_version.Name = "button_version";
            this.button_version.Size = new System.Drawing.Size(56, 19);
            this.button_version.TabIndex = 3;
            this.button_version.Text = "version";
            this.button_version.UseVisualStyleBackColor = true;
            this.button_version.Click += new System.EventHandler(this.button_version_Click);
            // 
            // button_event
            // 
            this.button_event.Location = new System.Drawing.Point(333, 29);
            this.button_event.Margin = new System.Windows.Forms.Padding(2);
            this.button_event.Name = "button_event";
            this.button_event.Size = new System.Drawing.Size(67, 19);
            this.button_event.TabIndex = 4;
            this.button_event.Text = "scan id";
            this.button_event.UseVisualStyleBackColor = true;
            this.button_event.Click += new System.EventHandler(this.button_event_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(435, 7);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(40, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "nodeId";
            // 
            // nodeIdBox
            // 
            this.nodeIdBox.FormattingEnabled = true;
            this.nodeIdBox.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10"});
            this.nodeIdBox.Location = new System.Drawing.Point(437, 35);
            this.nodeIdBox.Margin = new System.Windows.Forms.Padding(2);
            this.nodeIdBox.Name = "nodeIdBox";
            this.nodeIdBox.Size = new System.Drawing.Size(92, 21);
            this.nodeIdBox.TabIndex = 6;
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.HorizontalScrollbar = true;
            this.listBox1.Location = new System.Drawing.Point(16, 132);
            this.listBox1.Margin = new System.Windows.Forms.Padding(2);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(152, 134);
            this.listBox1.TabIndex = 7;
            // 
            // button_scan
            // 
            this.button_scan.Location = new System.Drawing.Point(333, 70);
            this.button_scan.Margin = new System.Windows.Forms.Padding(2);
            this.button_scan.Name = "button_scan";
            this.button_scan.Size = new System.Drawing.Size(67, 19);
            this.button_scan.TabIndex = 8;
            this.button_scan.Text = "ScanNodes";
            this.button_scan.UseVisualStyleBackColor = true;
            this.button_scan.Click += new System.EventHandler(this.button_scan_Click);
            // 
            // openHEX
            // 
            this.openHEX.Location = new System.Drawing.Point(333, 132);
            this.openHEX.Margin = new System.Windows.Forms.Padding(2);
            this.openHEX.Name = "openHEX";
            this.openHEX.Size = new System.Drawing.Size(67, 45);
            this.openHEX.TabIndex = 9;
            this.openHEX.Text = "select firmware";
            this.openHEX.UseVisualStyleBackColor = true;
            this.openHEX.Click += new System.EventHandler(this.openHEX_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(333, 325);
            this.progressBar1.Margin = new System.Windows.Forms.Padding(2);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(195, 19);
            this.progressBar1.TabIndex = 10;
            // 
            // openHEXFileDialog
            // 
            this.openHEXFileDialog.FileName = "HEXfile";
            this.openHEXFileDialog.Filter = "\"hex-files (*.hex)\"|*.hex";
            this.openHEXFileDialog.Title = "HEX";
            this.openHEXFileDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.openHEXFileDialog_FileOk);
            // 
            // ButtonUpload
            // 
            this.ButtonUpload.Location = new System.Drawing.Point(437, 132);
            this.ButtonUpload.Margin = new System.Windows.Forms.Padding(2);
            this.ButtonUpload.Name = "ButtonUpload";
            this.ButtonUpload.Size = new System.Drawing.Size(56, 45);
            this.ButtonUpload.TabIndex = 11;
            this.ButtonUpload.Text = "upload firmware";
            this.ButtonUpload.UseVisualStyleBackColor = true;
            this.ButtonUpload.Click += new System.EventHandler(this.ButtonUpload_Click);
            // 
            // GUID0
            // 
            this.GUID0.Location = new System.Drawing.Point(547, 36);
            this.GUID0.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.GUID0.Name = "GUID0";
            this.GUID0.Size = new System.Drawing.Size(120, 20);
            this.GUID0.TabIndex = 13;
            this.GUID0.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // GUID3
            // 
            this.GUID3.Location = new System.Drawing.Point(547, 62);
            this.GUID3.Name = "GUID3";
            this.GUID3.Size = new System.Drawing.Size(120, 20);
            this.GUID3.TabIndex = 14;
            // 
            // GUID5
            // 
            this.GUID5.Location = new System.Drawing.Point(547, 88);
            this.GUID5.Name = "GUID5";
            this.GUID5.Size = new System.Drawing.Size(120, 20);
            this.GUID5.TabIndex = 15;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(544, 7);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(70, 13);
            this.label2.TabIndex = 16;
            this.label2.Text = "GUID 0 3 5 7";
            this.label2.Click += new System.EventHandler(this.label2_Click);
            // 
            // GUID7
            // 
            this.GUID7.Location = new System.Drawing.Point(547, 114);
            this.GUID7.Name = "GUID7";
            this.GUID7.Size = new System.Drawing.Size(120, 20);
            this.GUID7.TabIndex = 17;
            // 
            // algorithm
            // 
            this.algorithm.FormattingEnabled = true;
            this.algorithm.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10"});
            this.algorithm.Location = new System.Drawing.Point(438, 245);
            this.algorithm.Margin = new System.Windows.Forms.Padding(2);
            this.algorithm.Name = "algorithm";
            this.algorithm.Size = new System.Drawing.Size(92, 21);
            this.algorithm.TabIndex = 18;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(435, 230);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 13);
            this.label3.TabIndex = 19;
            this.label3.Text = "algorithm";
            this.label3.Click += new System.EventHandler(this.label3_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(544, 164);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 13);
            this.label4.TabIndex = 22;
            this.label4.Text = "REG x92 x93";
            // 
            // REGx93
            // 
            this.REGx93.Location = new System.Drawing.Point(547, 219);
            this.REGx93.Name = "REGx93";
            this.REGx93.Size = new System.Drawing.Size(120, 20);
            this.REGx93.TabIndex = 21;
            // 
            // REGx92
            // 
            this.REGx92.Location = new System.Drawing.Point(547, 193);
            this.REGx92.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.REGx92.Name = "REGx92";
            this.REGx92.Size = new System.Drawing.Size(120, 20);
            this.REGx92.TabIndex = 20;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(666, 356);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.REGx93);
            this.Controls.Add(this.REGx92);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.algorithm);
            this.Controls.Add(this.GUID7);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.GUID5);
            this.Controls.Add(this.GUID3);
            this.Controls.Add(this.GUID0);
            this.Controls.Add(this.ButtonUpload);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.openHEX);
            this.Controls.Add(this.button_scan);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.nodeIdBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button_event);
            this.Controls.Add(this.button_version);
            this.Controls.Add(this.buttonDisconnect);
            this.Controls.Add(this.buttonConnect);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.GUID0)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GUID3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GUID5)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GUID7)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.REGx93)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.REGx92)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.Button buttonDisconnect;
        private System.Windows.Forms.Button button_version;
        private System.Windows.Forms.Button button_event;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox nodeIdBox;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Button button_scan;
        private System.Windows.Forms.Button openHEX;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.OpenFileDialog openHEXFileDialog;
        private System.Windows.Forms.Button ButtonUpload;
        private System.Windows.Forms.NumericUpDown GUID0;
        private System.Windows.Forms.NumericUpDown GUID3;
        private System.Windows.Forms.NumericUpDown GUID5;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown GUID7;
        private System.Windows.Forms.ComboBox algorithm;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown REGx93;
        private System.Windows.Forms.NumericUpDown REGx92;
    }
}

