using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows.Media;

namespace myHouse
{

  class service
  {
    public HouseData hd;
    public int downloadIntervalMeteo = 20000; //in ms
    public int downloadIntervalHouse = 15000; //in ms
    public int downloadIntervalSolar = 5000; //in ms

    private Timer timerMeteo;
    private Timer timerHouse;
    private Timer timerSolar;


    public service()
    {
      hd = new HouseData();
      timerMeteo = new Timer(downloadIntervalMeteo);
      timerMeteo.Elapsed += new ElapsedEventHandler(OnTimedEventMeteo);
      timerMeteo.Enabled = true;
      timerHouse = new Timer(downloadIntervalHouse);
      timerHouse.Elapsed += new ElapsedEventHandler(OnTimedEventHouse);
      timerHouse.Enabled = true;
      timerSolar = new Timer(downloadIntervalSolar);
      timerSolar.Elapsed += new ElapsedEventHandler(OnTimedEventSolar);
      timerSolar.Enabled = true;

    }

    private void OnTimedEventMeteo(object source, ElapsedEventArgs e)
    {
      showDataMeteo();
    }
    private void OnTimedEventHouse(object source, ElapsedEventArgs e)
    {
      showDataHouse();
    }
    private void OnTimedEventSolar(object source, ElapsedEventArgs e)
    {
      showDataSolar();
    }



    public String getDataHouse()
    {
      string url = "https://api.cosm.com/v2/feeds/" + "740319992" + ".csv";

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
        return htmlCode;
      }
      catch (WebException ex)
      {
        throw ex;
      }
    }

    public String getDataMeteo()
    {
      string url = "https://api.cosm.com/v2/feeds/" + "63310" + ".csv";

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
        return htmlCode;
      }
      catch (WebException ex)
      {
        throw ex;
      }
    }

    public String getDataSolar()
    {
      string url = "https://api.cosm.com/v2/feeds/" + "538561447" + ".csv";

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
        return htmlCode;
      }
      catch (WebException ex)
      {
        throw ex;
      }
    }


    public void parseDataHouse(string data)
    {
      string[] radka = data.Split(new Char[] { '\n' });
      hd.resetHouse();
      foreach (string s in radka)
      {
        string[] column = s.Split(new Char[] { ',' });
        try
        {
          if (column[0] == "Corridor")
          {
            hd.corridorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.corridorColor = getRoomColor(hd.corridorTemp);
          }
          if (column[0] == "Hall")
          {
            hd.hallTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.hallColor = getRoomColor(hd.hallTemp);
          }
          if (column[0] == "LivingRoom")
          {
            hd.livingRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.livingRoomColor = getRoomColor(hd.livingRoomTemp);
          }
          if (column[0] == "WorkRoom")
          {
            hd.workRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.workRoomColor = getRoomColor(hd.workRoomTemp);
          }
          if (column[0] == "BedRoomOld")
          {
            hd.bedRoomTempOld = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.bedRoomOldColor = getRoomColor(hd.bedRoomTempOld);
          }
          if (column[0] == "BedRoomNew")
          {
            hd.bedRoomTempNew = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.bedRoomNewColor = getRoomColor(hd.bedRoomTempNew);
          }
          if (column[0] == "Bojler")
          {
            hd.bojlerTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.bojlerColor = getRoomColor(hd.bojlerTemp);
          }
          hd.lastUpdateHouse = getDateTimeFromCosmString(column[1] + column[2]);
        }
        catch (FormatException ex)
        {

        }
        //lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
      }
    }

    public void parseDataMeteo(string data)
    {
      /*  
            
       */
      string[] radka = data.Split(new Char[] { '\n' });
      hd.resetMeteo();
      foreach (string s in radka)
      {
        string[] column = s.Split(new Char[] { ',' });
        try
        {
          if (column[0] == "T2899BDCF02000076")
          {
            hd.outdoorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.outdoorColor = getRoomColor(hd.outdoorTemp);
          }
          if (column[0] == "Humidity")
          {
            hd.humidity = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "Press")
          {
            hd.pressure = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "Temp085")
          {
            hd.temp085 = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "TempDHT")
          {
            hd.tempDHT = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "WindS")
          {
            hd.windSpeed = (int)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "WindSM")
          {
            hd.windSpeedMax = (int)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "WindD")
          {
            hd.windDirection = (int)Convert.ToDecimal(column[2].Replace(".", ","));
          }

          hd.lastUpdateMeteo = getDateTimeFromCosmString(column[1] + column[2]);

        }
        catch (FormatException ex)
        {

        }
        //lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
      }
    }

    public void parseDataSolar(string data)
    {
      string[] radka = data.Split(new Char[] { '\n' });
      hd.resetSolar();
      foreach (string s in radka)
      {
        string[] column = s.Split(new Char[] { ',' });
        try
        {
          if (column[0] == "IN")
          {
            hd.solarINTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            //hd.corridorColor = getRoomColor(hd.corridorTemp);
          }
          if (column[0] == "OUT")
          {
            hd.solarOUTTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "ROOM")
          {
            hd.solarROOMTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "Power")
          {
            hd.solarPower = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "EnergyAday")
          {
            hd.energyADay = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "EnergyTotal")
          {
            hd.energyTotal = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "DifON")
          {
            hd.difON = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "DifOFF")
          {
            hd.difOFF = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "Status")
          {
            hd.statusSolar = column[2];
          }
          if (column[0] == "Bojler2")
          {
            hd.bojler2Temp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }

          hd.lastUpdateSolar = getDateTimeFromCosmString(column[1] + column[2]);
        }
        catch (FormatException ex)
        {

        }
      }
    }

    private string getRoomColor(float temperature)
    {
      if (temperature > 25)
        return Colors.Red.ToString();
      else if (temperature > 10)
        return Colors.Green.ToString();
      else if (temperature > 0)
        return Colors.AliceBlue.ToString();
      else
        return Colors.Blue.ToString();
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

    public void showDataMeteo()
    {
      hd.statusLEDMeteoColor = Colors.Yellow.ToString();
      parseDataMeteo(getDataMeteo());
      hd.statusLEDMeteoColor = Colors.Green.ToString();
    }
    public void showDataHouse()
    {
      hd.statusLEDHouseColor = Colors.Yellow.ToString();
      parseDataHouse(getDataHouse());
      hd.statusLEDHouseColor = Colors.Green.ToString();
    }
    public void showDataSolar()
    {
      hd.statusLEDSolarColor = Colors.Yellow.ToString();
      parseDataSolar(getDataSolar());
      hd.statusLEDSolarColor = Colors.Green.ToString();
    }

  }
}
