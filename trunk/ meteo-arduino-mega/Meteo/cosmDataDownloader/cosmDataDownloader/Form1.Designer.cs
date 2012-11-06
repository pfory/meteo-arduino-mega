namespace cosmDataDownloader
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
      this.dateTimePickerStart = new System.Windows.Forms.DateTimePicker();
      this.tbFeedId = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      this.dateTimePickerEnd = new System.Windows.Forms.DateTimePicker();
      this.button1 = new System.Windows.Forms.Button();
      this.tbResult = new System.Windows.Forms.TextBox();
      this.label5 = new System.Windows.Forms.Label();
      this.textBoxUserName = new System.Windows.Forms.TextBox();
      this.textBoxPassw = new System.Windows.Forms.TextBox();
      this.label6 = new System.Windows.Forms.Label();
      this.label7 = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // dateTimePickerStart
      // 
      this.dateTimePickerStart.Location = new System.Drawing.Point(55, 10);
      this.dateTimePickerStart.Name = "dateTimePickerStart";
      this.dateTimePickerStart.Size = new System.Drawing.Size(156, 20);
      this.dateTimePickerStart.TabIndex = 0;
      // 
      // tbFeedId
      // 
      this.tbFeedId.Location = new System.Drawing.Point(276, 7);
      this.tbFeedId.Name = "tbFeedId";
      this.tbFeedId.Size = new System.Drawing.Size(63, 20);
      this.tbFeedId.TabIndex = 1;
      this.tbFeedId.Text = "63310";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(90, 62);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(647, 15);
      this.label1.TabIndex = 2;
      this.label1.Text = "http://api.cosm.com/v2/feeds/75618.csv?start=2012-11-06T00:00:00Z&end=2012-11-06T" +
          "05:59:59Z&interval=0&limit=1000";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(226, 10);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(48, 15);
      this.label2.TabIndex = 3;
      this.label2.Text = "Feed #:";
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(13, 12);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(35, 15);
      this.label3.TabIndex = 4;
      this.label3.Text = "Start:";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(13, 37);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(32, 15);
      this.label4.TabIndex = 5;
      this.label4.Text = "End:";
      // 
      // dateTimePickerEnd
      // 
      this.dateTimePickerEnd.Location = new System.Drawing.Point(55, 36);
      this.dateTimePickerEnd.Name = "dateTimePickerEnd";
      this.dateTimePickerEnd.Size = new System.Drawing.Size(156, 20);
      this.dateTimePickerEnd.TabIndex = 6;
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(689, 7);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(75, 23);
      this.button1.TabIndex = 8;
      this.button1.Text = "Stáhnout";
      this.button1.UseVisualStyleBackColor = true;
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // tbResult
      // 
      this.tbResult.Location = new System.Drawing.Point(12, 78);
      this.tbResult.Multiline = true;
      this.tbResult.Name = "tbResult";
      this.tbResult.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.tbResult.Size = new System.Drawing.Size(923, 398);
      this.tbResult.TabIndex = 9;
      // 
      // label5
      // 
      this.label5.AutoSize = true;
      this.label5.Location = new System.Drawing.Point(13, 492);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(41, 15);
      this.label5.TabIndex = 10;
      this.label5.Text = "label5";
      // 
      // textBoxUserName
      // 
      this.textBoxUserName.Location = new System.Drawing.Point(393, 7);
      this.textBoxUserName.Name = "textBoxUserName";
      this.textBoxUserName.Size = new System.Drawing.Size(100, 20);
      this.textBoxUserName.TabIndex = 11;
      this.textBoxUserName.Text = "datel";
      // 
      // textBoxPassw
      // 
      this.textBoxPassw.Location = new System.Drawing.Point(570, 9);
      this.textBoxPassw.Name = "textBoxPassw";
      this.textBoxPassw.Size = new System.Drawing.Size(100, 20);
      this.textBoxPassw.TabIndex = 12;
      this.textBoxPassw.Text = "mrdatel";
      this.textBoxPassw.UseSystemPasswordChar = true;
      // 
      // label6
      // 
      this.label6.AutoSize = true;
      this.label6.Location = new System.Drawing.Point(351, 10);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(36, 15);
      this.label6.TabIndex = 13;
      this.label6.Text = "User:";
      // 
      // label7
      // 
      this.label7.AutoSize = true;
      this.label7.Location = new System.Drawing.Point(500, 10);
      this.label7.Name = "label7";
      this.label7.Size = new System.Drawing.Size(64, 15);
      this.label7.TabIndex = 14;
      this.label7.Text = "Password:";
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(947, 524);
      this.Controls.Add(this.label7);
      this.Controls.Add(this.label6);
      this.Controls.Add(this.textBoxPassw);
      this.Controls.Add(this.textBoxUserName);
      this.Controls.Add(this.label5);
      this.Controls.Add(this.tbResult);
      this.Controls.Add(this.button1);
      this.Controls.Add(this.dateTimePickerEnd);
      this.Controls.Add(this.label4);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.tbFeedId);
      this.Controls.Add(this.dateTimePickerStart);
      this.Name = "Form1";
      this.Text = "Form1";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.DateTimePicker dateTimePickerStart;
    private System.Windows.Forms.TextBox tbFeedId;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.DateTimePicker dateTimePickerEnd;
    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.TextBox tbResult;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.TextBox textBoxUserName;
    private System.Windows.Forms.TextBox textBoxPassw;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.Label label7;
  }
}

