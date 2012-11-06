using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;

namespace cosmDataDownloader
{
  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();
    }

    private void button1_Click(object sender, EventArgs e)
    {
      string url = getUrlString(dateTimePickerStart.Value.ToShortDateString());

      string htmlCode = String.Empty;
      try
      {
        // Create Request
        HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);

        // Create Client
        WebClient client = new WebClient();

        // Assign Credentials
        client.Credentials = new NetworkCredential("datel", "mrdatel");

        // Grab Data
        htmlCode = client.DownloadString(url);

        // Display Data
        //MessageBox.Show(htmlCode);
      }
      catch (WebException ex)
      {
        MessageBox.Show(ex.ToString());
      }

      string[] rozsekany = htmlCode.Split(new Char[] { '\n' });

      StringBuilder sb = new StringBuilder();
      string lastDateTime = string.Empty;
      foreach (String s in rozsekany)
      {
        string[] radka = s.Split(new Char[] { ',' });
        lastDateTime = radka[1];
        sb.AppendLine(s);
      }
      tbResult.Text += sb;

    }


    private string getUrlString(string start)
    {
      string url = String.Empty;
      url += "http://api.cosm.com/v2/feeds/";
      url += tbFeedId.Text;
      url += ".csv?start=";
      url += start;
      url += "T00:00:00";
      url += "&end=";
      url += dateTimePickerStart.Value.ToShortDateString();
      url += "T05:59:59";
      url += "&interval=0&limit=1000";
      return url;
    }
  }
}
