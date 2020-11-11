namespace annot
{
    partial class frmannot
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
            this.pic = new System.Windows.Forms.PictureBox();
            this.lblcur = new System.Windows.Forms.Label();
            this.txtcur = new System.Windows.Forms.TextBox();
            this.btnup = new System.Windows.Forms.Button();
            this.btndown = new System.Windows.Forms.Button();
            this.btn0 = new System.Windows.Forms.Button();
            this.btn1 = new System.Windows.Forms.Button();
            this.btn4 = new System.Windows.Forms.Button();
            this.btn2 = new System.Windows.Forms.Button();
            this.btn5 = new System.Windows.Forms.Button();
            this.btn3 = new System.Windows.Forms.Button();
            this.btn6 = new System.Windows.Forms.Button();
            this.btn16 = new System.Windows.Forms.Button();
            this.btn15 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.btn11 = new System.Windows.Forms.Button();
            this.btn12 = new System.Windows.Forms.Button();
            this.btn13 = new System.Windows.Forms.Button();
            this.btn17 = new System.Windows.Forms.Button();
            this.btn8 = new System.Windows.Forms.Button();
            this.btn10 = new System.Windows.Forms.Button();
            this.lblneck = new System.Windows.Forms.Label();
            this.lblhead = new System.Windows.Forms.Label();
            this.lblloc = new System.Windows.Forms.Label();
            this.txtcurjt = new System.Windows.Forms.TextBox();
            this.lbljt = new System.Windows.Forms.Label();
            this.btndone = new System.Windows.Forms.Button();
            this.btndraw = new System.Windows.Forms.Button();
            this.head1 = new System.Windows.Forms.Label();
            this.head2 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btndraw2 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pic)).BeginInit();
            this.SuspendLayout();
            // 
            // pic
            // 
            this.pic.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pic.Location = new System.Drawing.Point(118, 39);
            this.pic.Name = "pic";
            this.pic.Size = new System.Drawing.Size(784, 784);
            this.pic.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pic.TabIndex = 0;
            this.pic.TabStop = false;
            this.pic.Click += new System.EventHandler(this.pic_Click);
            this.pic.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pic_MouseClick);
            this.pic.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pic_MouseDown);
            this.pic.MouseHover += new System.EventHandler(this.pic_MouseHover);
            this.pic.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pic_MouseMove);
            this.pic.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pic_MouseUp);
            this.pic.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.pic_PreviewKeyDown);
            // 
            // lblcur
            // 
            this.lblcur.AutoSize = true;
            this.lblcur.Font = new System.Drawing.Font("Pill Gothic 600mg Semibd", 16.2F, System.Drawing.FontStyle.Bold);
            this.lblcur.Location = new System.Drawing.Point(922, 21);
            this.lblcur.Name = "lblcur";
            this.lblcur.Size = new System.Drawing.Size(254, 28);
            this.lblcur.TabIndex = 1;
            this.lblcur.Text = "Current Image Index";
            // 
            // txtcur
            // 
            this.txtcur.Font = new System.Drawing.Font("Segoe Print", 24F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtcur.Location = new System.Drawing.Point(1206, 21);
            this.txtcur.Name = "txtcur";
            this.txtcur.Size = new System.Drawing.Size(97, 78);
            this.txtcur.TabIndex = 2;
            this.txtcur.Text = "0";
            this.txtcur.TextChanged += new System.EventHandler(this.txtcur_TextChanged);
            this.txtcur.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtcur_KeyDown);
            // 
            // btnup
            // 
            this.btnup.Font = new System.Drawing.Font("MS UI Gothic", 72F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnup.Location = new System.Drawing.Point(1352, 80);
            this.btnup.Name = "btnup";
            this.btnup.Size = new System.Drawing.Size(138, 139);
            this.btnup.TabIndex = 3;
            this.btnup.Text = "+";
            this.btnup.UseVisualStyleBackColor = true;
            this.btnup.Click += new System.EventHandler(this.btnup_Click);
            // 
            // btndown
            // 
            this.btndown.Font = new System.Drawing.Font("Microsoft Sans Serif", 72F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btndown.Location = new System.Drawing.Point(1352, 251);
            this.btndown.Name = "btndown";
            this.btndown.Size = new System.Drawing.Size(138, 139);
            this.btndown.TabIndex = 4;
            this.btndown.Text = "-";
            this.btndown.UseVisualStyleBackColor = true;
            this.btndown.Click += new System.EventHandler(this.btndown_Click);
            // 
            // btn0
            // 
            this.btn0.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn0.Location = new System.Drawing.Point(1077, 524);
            this.btn0.Name = "btn0";
            this.btn0.Size = new System.Drawing.Size(85, 79);
            this.btn0.TabIndex = 5;
            this.btn0.Text = "0";
            this.btn0.UseVisualStyleBackColor = true;
            this.btn0.Click += new System.EventHandler(this.btn0_Click);
            // 
            // btn1
            // 
            this.btn1.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn1.Location = new System.Drawing.Point(928, 524);
            this.btn1.Name = "btn1";
            this.btn1.Size = new System.Drawing.Size(85, 79);
            this.btn1.TabIndex = 6;
            this.btn1.Text = "1";
            this.btn1.UseVisualStyleBackColor = true;
            this.btn1.Click += new System.EventHandler(this.btn1_Click);
            // 
            // btn4
            // 
            this.btn4.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn4.Location = new System.Drawing.Point(1218, 524);
            this.btn4.Name = "btn4";
            this.btn4.Size = new System.Drawing.Size(85, 79);
            this.btn4.TabIndex = 7;
            this.btn4.Text = "4";
            this.btn4.UseVisualStyleBackColor = true;
            this.btn4.Click += new System.EventHandler(this.btn4_Click);
            // 
            // btn2
            // 
            this.btn2.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn2.Location = new System.Drawing.Point(928, 688);
            this.btn2.Name = "btn2";
            this.btn2.Size = new System.Drawing.Size(85, 79);
            this.btn2.TabIndex = 8;
            this.btn2.Text = "2";
            this.btn2.UseVisualStyleBackColor = true;
            this.btn2.Click += new System.EventHandler(this.btn2_Click);
            // 
            // btn5
            // 
            this.btn5.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn5.Location = new System.Drawing.Point(1218, 688);
            this.btn5.Name = "btn5";
            this.btn5.Size = new System.Drawing.Size(85, 79);
            this.btn5.TabIndex = 9;
            this.btn5.Text = "5";
            this.btn5.UseVisualStyleBackColor = true;
            this.btn5.Click += new System.EventHandler(this.btn5_Click);
            // 
            // btn3
            // 
            this.btn3.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn3.Location = new System.Drawing.Point(928, 851);
            this.btn3.Name = "btn3";
            this.btn3.Size = new System.Drawing.Size(85, 79);
            this.btn3.TabIndex = 10;
            this.btn3.Text = "3";
            this.btn3.UseVisualStyleBackColor = true;
            this.btn3.Click += new System.EventHandler(this.btn3_Click);
            // 
            // btn6
            // 
            this.btn6.Font = new System.Drawing.Font("MV Boli", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn6.Location = new System.Drawing.Point(1218, 851);
            this.btn6.Name = "btn6";
            this.btn6.Size = new System.Drawing.Size(85, 79);
            this.btn6.TabIndex = 11;
            this.btn6.Text = "6";
            this.btn6.UseVisualStyleBackColor = true;
            this.btn6.Click += new System.EventHandler(this.btn6_Click);
            // 
            // btn16
            // 
            this.btn16.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn16.Location = new System.Drawing.Point(955, 411);
            this.btn16.Name = "btn16";
            this.btn16.Size = new System.Drawing.Size(100, 79);
            this.btn16.TabIndex = 12;
            this.btn16.Text = "16";
            this.btn16.UseVisualStyleBackColor = true;
            this.btn16.Click += new System.EventHandler(this.btn16_Click);
            // 
            // btn15
            // 
            this.btn15.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn15.Location = new System.Drawing.Point(955, 326);
            this.btn15.Name = "btn15";
            this.btn15.Size = new System.Drawing.Size(100, 79);
            this.btn15.TabIndex = 13;
            this.btn15.Text = "15";
            this.btn15.UseVisualStyleBackColor = true;
            this.btn15.Click += new System.EventHandler(this.btn15_Click);
            // 
            // button3
            // 
            this.button3.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button3.Location = new System.Drawing.Point(955, 241);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(100, 79);
            this.button3.TabIndex = 14;
            this.button3.Text = "14";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // btn11
            // 
            this.btn11.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn11.Location = new System.Drawing.Point(1166, 241);
            this.btn11.Name = "btn11";
            this.btn11.Size = new System.Drawing.Size(100, 79);
            this.btn11.TabIndex = 17;
            this.btn11.Text = "11";
            this.btn11.UseVisualStyleBackColor = true;
            this.btn11.Click += new System.EventHandler(this.btn11_Click);
            // 
            // btn12
            // 
            this.btn12.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn12.Location = new System.Drawing.Point(1166, 326);
            this.btn12.Name = "btn12";
            this.btn12.Size = new System.Drawing.Size(100, 79);
            this.btn12.TabIndex = 16;
            this.btn12.Text = "12";
            this.btn12.UseVisualStyleBackColor = true;
            this.btn12.Click += new System.EventHandler(this.btn12_Click);
            // 
            // btn13
            // 
            this.btn13.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn13.Location = new System.Drawing.Point(1166, 411);
            this.btn13.Name = "btn13";
            this.btn13.Size = new System.Drawing.Size(100, 79);
            this.btn13.TabIndex = 15;
            this.btn13.Text = "13";
            this.btn13.UseVisualStyleBackColor = true;
            this.btn13.Click += new System.EventHandler(this.btn13_Click);
            // 
            // btn17
            // 
            this.btn17.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn17.Location = new System.Drawing.Point(1060, 241);
            this.btn17.Name = "btn17";
            this.btn17.Size = new System.Drawing.Size(100, 79);
            this.btn17.TabIndex = 18;
            this.btn17.Text = "17";
            this.btn17.UseVisualStyleBackColor = true;
            this.btn17.Click += new System.EventHandler(this.btn17_Click);
            // 
            // btn8
            // 
            this.btn8.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn8.Location = new System.Drawing.Point(1062, 154);
            this.btn8.Name = "btn8";
            this.btn8.Size = new System.Drawing.Size(100, 79);
            this.btn8.TabIndex = 19;
            this.btn8.Text = "8";
            this.btn8.UseVisualStyleBackColor = true;
            this.btn8.Click += new System.EventHandler(this.btn8_Click);
            // 
            // btn10
            // 
            this.btn10.Font = new System.Drawing.Font("MV Boli", 28.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btn10.Location = new System.Drawing.Point(1062, 69);
            this.btn10.Name = "btn10";
            this.btn10.Size = new System.Drawing.Size(100, 79);
            this.btn10.TabIndex = 20;
            this.btn10.Text = "10";
            this.btn10.UseVisualStyleBackColor = true;
            this.btn10.Click += new System.EventHandler(this.btn10_Click);
            // 
            // lblneck
            // 
            this.lblneck.AutoSize = true;
            this.lblneck.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.lblneck.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblneck.ForeColor = System.Drawing.Color.MediumBlue;
            this.lblneck.Location = new System.Drawing.Point(1182, 173);
            this.lblneck.Name = "lblneck";
            this.lblneck.Size = new System.Drawing.Size(112, 46);
            this.lblneck.TabIndex = 21;
            this.lblneck.Text = "Neck";
            // 
            // lblhead
            // 
            this.lblhead.AutoSize = true;
            this.lblhead.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.lblhead.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblhead.ForeColor = System.Drawing.Color.MediumBlue;
            this.lblhead.Location = new System.Drawing.Point(1182, 102);
            this.lblhead.Name = "lblhead";
            this.lblhead.Size = new System.Drawing.Size(115, 46);
            this.lblhead.TabIndex = 22;
            this.lblhead.Text = "Head";
            // 
            // lblloc
            // 
            this.lblloc.AutoSize = true;
            this.lblloc.Font = new System.Drawing.Font("Ink Free", 48F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblloc.Location = new System.Drawing.Point(119, 826);
            this.lblloc.Name = "lblloc";
            this.lblloc.Size = new System.Drawing.Size(213, 99);
            this.lblloc.TabIndex = 23;
            this.lblloc.Text = "(0,0)";
            // 
            // txtcurjt
            // 
            this.txtcurjt.Font = new System.Drawing.Font("Segoe Print", 25.8F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtcurjt.Location = new System.Drawing.Point(1582, 21);
            this.txtcurjt.Name = "txtcurjt";
            this.txtcurjt.Size = new System.Drawing.Size(97, 83);
            this.txtcurjt.TabIndex = 26;
            this.txtcurjt.Text = "0";
            // 
            // lbljt
            // 
            this.lbljt.AutoSize = true;
            this.lbljt.Font = new System.Drawing.Font("Pill Gothic 600mg Semibd", 16.2F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbljt.Location = new System.Drawing.Point(1322, 21);
            this.lbljt.Name = "lbljt";
            this.lbljt.Size = new System.Drawing.Size(240, 28);
            this.lbljt.TabIndex = 25;
            this.lbljt.Text = "Current Joint Index";
            // 
            // btndone
            // 
            this.btndone.Font = new System.Drawing.Font("Segoe Script", 72F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btndone.Location = new System.Drawing.Point(1345, 664);
            this.btndone.Name = "btndone";
            this.btndone.Size = new System.Drawing.Size(317, 230);
            this.btndone.TabIndex = 27;
            this.btndone.Text = "OK";
            this.btndone.UseVisualStyleBackColor = true;
            this.btndone.Click += new System.EventHandler(this.btndone_Click);
            // 
            // btndraw
            // 
            this.btndraw.Font = new System.Drawing.Font("Segoe Script", 25.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btndraw.Location = new System.Drawing.Point(1345, 411);
            this.btndraw.Name = "btndraw";
            this.btndraw.Size = new System.Drawing.Size(317, 230);
            this.btndraw.TabIndex = 28;
            this.btndraw.Text = "DRAW\r\nHEAD";
            this.btndraw.UseVisualStyleBackColor = true;
            this.btndraw.Click += new System.EventHandler(this.btndraw_Click);
            // 
            // head1
            // 
            this.head1.AutoSize = true;
            this.head1.Font = new System.Drawing.Font("Pill Gothic 600mg Semibd", 48F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.head1.Location = new System.Drawing.Point(1736, 149);
            this.head1.Name = "head1";
            this.head1.Size = new System.Drawing.Size(205, 80);
            this.head1.TabIndex = 29;
            this.head1.Text = "(0,0)";
            // 
            // head2
            // 
            this.head2.AutoSize = true;
            this.head2.Font = new System.Drawing.Font("Pill Gothic 600mg Semibd", 48F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.head2.Location = new System.Drawing.Point(1736, 310);
            this.head2.Name = "head2";
            this.head2.Size = new System.Drawing.Size(205, 80);
            this.head2.TabIndex = 30;
            this.head2.Text = "(0,0)";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(1792, 249);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(100, 22);
            this.textBox1.TabIndex = 31;
            // 
            // btndraw2
            // 
            this.btndraw2.Font = new System.Drawing.Font("Segoe Script", 25.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btndraw2.Location = new System.Drawing.Point(1699, 411);
            this.btndraw2.Name = "btndraw2";
            this.btndraw2.Size = new System.Drawing.Size(317, 230);
            this.btndraw2.TabIndex = 32;
            this.btndraw2.Text = "DRAW\r\nHEAD2\r\n(corner pt)";
            this.btndraw2.UseVisualStyleBackColor = true;
            this.btndraw2.Click += new System.EventHandler(this.btndraw2_Click);
            // 
            // frmannot
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1924, 1055);
            this.Controls.Add(this.btndraw2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.head2);
            this.Controls.Add(this.head1);
            this.Controls.Add(this.btndraw);
            this.Controls.Add(this.btndone);
            this.Controls.Add(this.txtcurjt);
            this.Controls.Add(this.lbljt);
            this.Controls.Add(this.lblloc);
            this.Controls.Add(this.lblhead);
            this.Controls.Add(this.lblneck);
            this.Controls.Add(this.btn10);
            this.Controls.Add(this.btn8);
            this.Controls.Add(this.btn17);
            this.Controls.Add(this.btn11);
            this.Controls.Add(this.btn12);
            this.Controls.Add(this.btn13);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.btn15);
            this.Controls.Add(this.btn16);
            this.Controls.Add(this.btn6);
            this.Controls.Add(this.btn3);
            this.Controls.Add(this.btn5);
            this.Controls.Add(this.btn2);
            this.Controls.Add(this.btn4);
            this.Controls.Add(this.btn1);
            this.Controls.Add(this.btn0);
            this.Controls.Add(this.btndown);
            this.Controls.Add(this.btnup);
            this.Controls.Add(this.txtcur);
            this.Controls.Add(this.lblcur);
            this.Controls.Add(this.pic);
            this.Name = "frmannot";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Annot";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.frmannot_Load);
            this.Click += new System.EventHandler(this.frmannot_Click);
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.frmannot_KeyPress);
            ((System.ComponentModel.ISupportInitialize)(this.pic)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pic;
        private System.Windows.Forms.Label lblcur;
        private System.Windows.Forms.TextBox txtcur;
        private System.Windows.Forms.Button btnup;
        private System.Windows.Forms.Button btndown;
        private System.Windows.Forms.Button btn0;
        private System.Windows.Forms.Button btn1;
        private System.Windows.Forms.Button btn4;
        private System.Windows.Forms.Button btn2;
        private System.Windows.Forms.Button btn5;
        private System.Windows.Forms.Button btn3;
        private System.Windows.Forms.Button btn6;
        private System.Windows.Forms.Button btn16;
        private System.Windows.Forms.Button btn15;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button btn11;
        private System.Windows.Forms.Button btn12;
        private System.Windows.Forms.Button btn13;
        private System.Windows.Forms.Button btn17;
        private System.Windows.Forms.Button btn8;
        private System.Windows.Forms.Button btn10;
        private System.Windows.Forms.Label lblneck;
        private System.Windows.Forms.Label lblhead;
        private System.Windows.Forms.Label lblloc;
        private System.Windows.Forms.TextBox txtcurjt;
        private System.Windows.Forms.Label lbljt;
        private System.Windows.Forms.Button btndone;
        private System.Windows.Forms.Button btndraw;
        private System.Windows.Forms.Label head1;
        private System.Windows.Forms.Label head2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btndraw2;
    }
}

