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
            this.SuspendLayout();
            // 
            // buttonConnect
            // 
            this.buttonConnect.Location = new System.Drawing.Point(80, 36);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(143, 23);
            this.buttonConnect.TabIndex = 0;
            this.buttonConnect.Text = "connect daemon";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
            // 
            // buttonDisconnect
            // 
            this.buttonDisconnect.Location = new System.Drawing.Point(80, 86);
            this.buttonDisconnect.Name = "buttonDisconnect";
            this.buttonDisconnect.Size = new System.Drawing.Size(143, 23);
            this.buttonDisconnect.TabIndex = 1;
            this.buttonDisconnect.Text = "disconnect daemon";
            this.buttonDisconnect.UseVisualStyleBackColor = true;
            this.buttonDisconnect.Click += new System.EventHandler(this.buttonDisconnect_Click);
            // 
            // button_version
            // 
            this.button_version.Enabled = false;
            this.button_version.Location = new System.Drawing.Point(297, 35);
            this.button_version.Name = "button_version";
            this.button_version.Size = new System.Drawing.Size(75, 23);
            this.button_version.TabIndex = 3;
            this.button_version.Text = "version";
            this.button_version.UseVisualStyleBackColor = true;
            this.button_version.Click += new System.EventHandler(this.button_version_Click);
            // 
            // button_event
            // 
            this.button_event.Location = new System.Drawing.Point(444, 36);
            this.button_event.Name = "button_event";
            this.button_event.Size = new System.Drawing.Size(89, 23);
            this.button_event.TabIndex = 4;
            this.button_event.Text = "scan id";
            this.button_event.UseVisualStyleBackColor = true;
            this.button_event.Click += new System.EventHandler(this.button_event_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(580, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(51, 17);
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
            this.nodeIdBox.Location = new System.Drawing.Point(583, 43);
            this.nodeIdBox.Name = "nodeIdBox";
            this.nodeIdBox.Size = new System.Drawing.Size(121, 24);
            this.nodeIdBox.TabIndex = 6;
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.HorizontalScrollbar = true;
            this.listBox1.ItemHeight = 16;
            this.listBox1.Location = new System.Drawing.Point(22, 163);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(201, 164);
            this.listBox1.TabIndex = 7;
            // 
            // button_scan
            // 
            this.button_scan.Location = new System.Drawing.Point(444, 86);
            this.button_scan.Name = "button_scan";
            this.button_scan.Size = new System.Drawing.Size(89, 23);
            this.button_scan.TabIndex = 8;
            this.button_scan.Text = "ScanNodes";
            this.button_scan.UseVisualStyleBackColor = true;
            this.button_scan.Click += new System.EventHandler(this.button_scan_Click);
            // 
            // openHEX
            // 
            this.openHEX.Location = new System.Drawing.Point(444, 163);
            this.openHEX.Name = "openHEX";
            this.openHEX.Size = new System.Drawing.Size(89, 55);
            this.openHEX.TabIndex = 9;
            this.openHEX.Text = "select firmware";
            this.openHEX.UseVisualStyleBackColor = true;
            this.openHEX.Click += new System.EventHandler(this.openHEX_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(444, 400);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(260, 23);
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
            this.ButtonUpload.Location = new System.Drawing.Point(583, 163);
            this.ButtonUpload.Name = "ButtonUpload";
            this.ButtonUpload.Size = new System.Drawing.Size(75, 55);
            this.ButtonUpload.TabIndex = 11;
            this.ButtonUpload.Text = "upload firmware";
            this.ButtonUpload.UseVisualStyleBackColor = true;
            this.ButtonUpload.Click += new System.EventHandler(this.ButtonUpload_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(888, 438);
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
            this.Name = "Form1";
            this.Text = "Form1";
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
    }
}

