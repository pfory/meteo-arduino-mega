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
      this.dateTimePicker2 = new System.Windows.Forms.DateTimePicker();
      this.button1 = new System.Windows.Forms.Button();
      this.tbResult = new System.Windows.Forms.TextBox();
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
      this.tbFeedId.Location = new System.Drawing.Point(535, 9);
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
      this.label1.Size = new System.Drawing.Size(575, 13);
      this.label1.TabIndex = 2;
      this.label1.Text = "http://api.cosm.com/v2/feeds/75618.csv?start=2012-11-06T00:00:00Z&end=2012-11-06T" +
    "05:59:59Z&interval=0&limit=1000";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(485, 12);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(44, 13);
      this.label2.TabIndex = 3;
      this.label2.Text = "Feed #:";
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(13, 12);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(32, 13);
      this.label3.TabIndex = 4;
      this.label3.Text = "Start:";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(13, 37);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(29, 13);
      this.label4.TabIndex = 5;
      this.label4.Text = "End:";
      // 
      // dateTimePicker2
      // 
      this.dateTimePicker2.Location = new System.Drawing.Point(55, 36);
      this.dateTimePicker2.Name = "dateTimePicker2";
      this.dateTimePicker2.Size = new System.Drawing.Size(156, 20);
      this.dateTimePicker2.TabIndex = 6;
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(695, 10);
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
      this.tbResult.Size = new System.Drawing.Size(923, 449);
      this.tbResult.TabIndex = 9;
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(947, 553);
      this.Controls.Add(this.tbResult);
      this.Controls.Add(this.button1);
      this.Controls.Add(this.dateTimePicker2);
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
    private System.Windows.Forms.DateTimePicker dateTimePicker2;
    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.TextBox tbResult;
  }
}

