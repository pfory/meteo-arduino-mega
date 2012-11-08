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
using System.Globalization;


namespace cosmDataDownloader
{
  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();
    }

    private bool writeHeader = true;
    StreamWriter sw;
    char ODDELOVAC = ';';

    /*T28A6B0410400004E,2012-11-06T10:47:59.200814Z,11.7
T28CEB0410400002C,2012-11-06T10:47:59.200814Z,6.3
T28C9B84104000097,2012-11-06T10:47:59.200814Z,8.5
T28E8B84104000016,2012-11-06T10:48:19.189317Z,6.3
T28A6B0410400004E,2012-11-06T10:48:19.189317Z,11.7
    */

    private void button1_Click(object sender, EventArgs e)
    {
      DateTime start = new DateTime(dateTimePickerStart.Value.Year, dateTimePickerStart.Value.Month, dateTimePickerStart.Value.Day, 0,0,0);
      DateTime startOld = new DateTime();
      DateTime end = new DateTime(dateTimePickerEnd.Value.Year, dateTimePickerEnd.Value.Month, dateTimePickerEnd.Value.Day, 0, 0, 0);

      tbResult.Text = String.Empty;
      String fileName = start.Year.ToString("D4") + start.Month.ToString("D2") + start.Day.ToString("D2") + ".csv";

      if (checkBoxVytvaretSoubor.Checked)
      {
        writeHeader = true;
        sw = new StreamWriter(fileName, false, Encoding.Default);
      }

      while (true) {
        bool chyba = false;
        start = nactiDavku(start, end, tbFeedId.Text, ref chyba);
        if (start >= end)
          break;
        if (chyba) 
          continue;
        if (startOld == start)
          break;
        else
        {
          start = start.AddSeconds(1);
          startOld = start;
        }
        sw.Flush();
      }
      sw.Close();

    }

    private DateTime nactiDavku(DateTime start, DateTime end, string feedId, ref bool chyba)
    {
      string url = getUrlString(start, feedId);

      string htmlCode = String.Empty;
      try
      {
        // Create Request
        HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);

        // Create Client
        WebClient client = new WebClient();

        // Assign Credentials
        client.Credentials = new NetworkCredential(textBoxUserName.Text, textBoxPassw.Text);

        // Grab Data
        htmlCode = client.DownloadString(url);

        // Display Data
        //MessageBox.Show(htmlCode);
      }
      catch (WebException ex)
      {
        //MessageBox.Show(ex.ToString());
        chyba = true;
      }

      string[] rozsekany = htmlCode.Split(new Char[] { '\n' });
      if (htmlCode == String.Empty)
        return start;

      StringBuilder sb = new StringBuilder();
      StringBuilder header = new StringBuilder("Datum");
      StringBuilder data = new StringBuilder();
      string lastDateTime = string.Empty;
      int i = 0;
      /*string[] dateAndTime;
      string[] date;
      string[] time;
       */
      foreach (String s in rozsekany)
      {
        string[] radka = s.Split(new Char[] { ',' });

        DateTime dateTime = getDateTimeFromCosmString(radka[1]);
        if (dateTime <= end)
        {
          if (i < numericUpDown1.Value)
          {
<<<<<<< .mine
            header.Append(";");
            header.Append(radka[0]);
=======
            if (writeHeader)
            {
              header.Append(ODDELOVAC);
              header.Append(radka[0]);
            }

            if (i == 0)
              data.Append(dateTime.ToShortDateString() + " " + dateTime.ToLongTimeString());
            data.Append(ODDELOVAC);
            data.Append(radka[2].Replace('.', ','));
>>>>>>> .r76
          }
<<<<<<< .mine

          if (i == 0)
            data.Append(dateTime.ToShortDateString()+" "+dateTime.ToLongTimeString());
          data.Append(";");
          data.Append(radka[2]);
        }
        i++;
        if (i == numericUpDown1.Value)
        {
          i = 0;
          if (writeHeader)
=======
          i++;
          if (i == numericUpDown1.Value)
>>>>>>> .r76
          {
            i = 0;
            if (writeHeader)
            {
              writeHeader = false;
              sw.WriteLine(header);
              header.Clear();
            }
            sw.WriteLine(data);
            data.Clear();
          }

          lastDateTime = radka[1];
          sb.AppendLine(s);
        }
      }
      sb.AppendLine("----------------------------------");
      tbResult.Text += sb;
      //2012-11-06T01:23:09.315392Z
      DateTime retDateTime = getDateTimeFromCosmString(lastDateTime);
      return retDateTime;
    }

    private string getUrlString(DateTime start, string feedId)
    {
      DateTime end = start.AddHours(5);
      end = end.AddMinutes(59);
      end = end.AddSeconds(59);

      string s = start.ToShortDateString() + "T" + start.ToShortTimeString();
      string e = end.ToShortDateString() + "T" + end.ToShortTimeString();

      string url = String.Empty;
      url += "http://api.cosm.com/v2/feeds/";
      url += feedId;
      url += ".csv?start=";
      url += start;
      url += "&end=";
      url += end;
      url += "&interval=0&limit=1000";
      return url;
    }


    private DateTime getDateTimeFromCosmString(string s)
    {
      if (s == string.Empty)
        return new DateTime();
      String[] dateAndTime = s.Split(new Char[] { 'T' });
      String[] date = dateAndTime[0].Split(new Char[] { '-' });
      String[] time = dateAndTime[1].Split(new Char[] { ':' });

      return new DateTime(Convert.ToInt16(date[0]), Convert.ToInt16(date[1]), Convert.ToInt16(date[2]), Convert.ToInt16(time[0]), Convert.ToInt16(time[1]), Convert.ToInt16(time[2].Substring(0, time[2].IndexOf('.'))));

    }
  }
}
