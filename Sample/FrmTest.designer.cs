namespace DCTDenoiseTest
{
    unsafe partial class FrmTest
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up all resources that are in use.
        /// </summary>
        /// <param name="disposing">True if the managed resource should be released; otherwise false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form designer generated code

        /// <summary>
        /// The designer supports the required method - do not use the code editor to modify the content of this method.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmTest));
            this.CmdOpen = new System.Windows.Forms.Button();
            this.CmdSave = new System.Windows.Forms.Button();
            this.PicSrc = new System.Windows.Forms.PictureBox();
            this.CmdProcess = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.SigmaC = new System.Windows.Forms.HScrollBar();
            this.label2 = new System.Windows.Forms.Label();
            this.LblInfo = new System.Windows.Forms.Label();
            this.PicDest = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.PicSrc)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PicDest)).BeginInit();
            this.SuspendLayout();
            // 
            // CmdOpen
            // 
            this.CmdOpen.Location = new System.Drawing.Point(4, 7);
            this.CmdOpen.Name = "CmdOpen";
            this.CmdOpen.Size = new System.Drawing.Size(75, 31);
            this.CmdOpen.TabIndex = 21;
            this.CmdOpen.Text = "Open image";
            this.CmdOpen.UseVisualStyleBackColor = true;
            this.CmdOpen.Click += new System.EventHandler(this.CmdOpen_Click);
            // 
            // CmdSave
            // 
            this.CmdSave.Location = new System.Drawing.Point(85, 7);
            this.CmdSave.Name = "CmdSave";
            this.CmdSave.Size = new System.Drawing.Size(75, 31);
            this.CmdSave.TabIndex = 35;
            this.CmdSave.Text = "Save image";
            this.CmdSave.UseVisualStyleBackColor = true;
            this.CmdSave.Click += new System.EventHandler(this.CmdSave_Click);
            // 
            // PicSrc
            // 
            this.PicSrc.Image = ((System.Drawing.Image)(resources.GetObject("PicSrc.Image")));
            this.PicSrc.Location = new System.Drawing.Point(12, 47);
            this.PicSrc.Name = "PicSrc";
            this.PicSrc.Size = new System.Drawing.Size(774, 518);
            this.PicSrc.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PicSrc.TabIndex = 33;
            this.PicSrc.TabStop = false;
            // 
            // CmdProcess
            // 
            this.CmdProcess.Location = new System.Drawing.Point(166, 7);
            this.CmdProcess.Name = "CmdProcess";
            this.CmdProcess.Size = new System.Drawing.Size(75, 31);
            this.CmdProcess.TabIndex = 37;
            this.CmdProcess.Text = "Process";
            this.CmdProcess.UseVisualStyleBackColor = true;
            this.CmdProcess.Click += new System.EventHandler(this.CmdProcess_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(270, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 38;
            this.label1.Text = "Sigma";
            // 
            // SigmaC
            // 
            this.SigmaC.Location = new System.Drawing.Point(326, 15);
            this.SigmaC.Maximum = 300;
            this.SigmaC.Minimum = 1;
            this.SigmaC.Name = "SigmaC";
            this.SigmaC.Size = new System.Drawing.Size(461, 17);
            this.SigmaC.TabIndex = 39;
            this.SigmaC.Value = 80;
            this.SigmaC.Scroll += new System.Windows.Forms.ScrollEventHandler(this.SigmaC_Scroll);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(792, 17);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(19, 13);
            this.label2.TabIndex = 40;
            this.label2.Text = "10";
            // 
            // LblInfo
            // 
            this.LblInfo.AutoSize = true;
            this.LblInfo.Location = new System.Drawing.Point(826, 16);
            this.LblInfo.Name = "LblInfo";
            this.LblInfo.Size = new System.Drawing.Size(0, 13);
            this.LblInfo.TabIndex = 301;
            // 
            // PicDest
            // 
            this.PicDest.Image = ((System.Drawing.Image)(resources.GetObject("PicDest.Image")));
            this.PicDest.Location = new System.Drawing.Point(795, 47);
            this.PicDest.Name = "PicDest";
            this.PicDest.Size = new System.Drawing.Size(774, 518);
            this.PicDest.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PicDest.TabIndex = 302;
            this.PicDest.TabStop = false;
            // 
            // FrmTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1577, 573);
            this.Controls.Add(this.PicDest);
            this.Controls.Add(this.LblInfo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.SigmaC);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.CmdProcess);
            this.Controls.Add(this.CmdSave);
            this.Controls.Add(this.PicSrc);
            this.Controls.Add(this.CmdOpen);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FrmTest";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "DCT denoising algorithm test";
            this.Load += new System.EventHandler(this.FrmTest_Load);
            ((System.ComponentModel.ISupportInitialize)(this.PicSrc)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PicDest)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button CmdOpen;
        private System.Windows.Forms.PictureBox PicSrc;
        private System.Windows.Forms.Button CmdSave;
        private System.Windows.Forms.Button CmdProcess;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.HScrollBar SigmaC;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label LblInfo;
        private System.Windows.Forms.PictureBox PicDest;
    }
}

