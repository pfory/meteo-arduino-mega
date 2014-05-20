using System;
using System.Net;
using System.Timers;
using System.Windows.Media;

namespace myHouse {

  class service {
    public HouseData hd;
    public int downloadIntervalMeteo = 20000; //in ms
    public int downloadIntervalHouse = 15000; //in ms
    public int downloadIntervalSolar = 5000; //in ms

    private String apiURLbase = "https://api.cosm.com/v2/feeds/";
    private long houseFeedID = 740319992;
    private long meteoFeedID = 63310;
    private long solarFeedID = 538561447;

    private Timer timerMeteo;
    private Timer timerHouse;
    private Timer timerSolar;


    public service() {
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

    private void OnTimedEventMeteo(object source, ElapsedEventArgs e) {
      timerMeteo.Enabled = false;
      showDataMeteo();
      timerMeteo.Enabled = true;
    }
    private void OnTimedEventHouse(object source, ElapsedEventArgs e) {
      timerHouse.Enabled = false;
      showDataHouse();
      timerHouse.Enabled = true;
    }
    private void OnTimedEventSolar(object source, ElapsedEventArgs e) {
      timerSolar.Enabled = false;
      showDataSolar();
      timerSolar.Enabled = true;
    }

    private String getDataHouse() {
      return getData(houseFeedID.ToString());
    }

    private String getDataMeteo() {
      return getData(meteoFeedID.ToString());
    }

    private String getDataSolar() {
      return getData(solarFeedID.ToString());
    }

    public String getDataSolarHist(DateTime dt) {
      return getData(solarFeedID.ToString(), dt);
    }

    private String getData(String feedID) {
      return getData(feedID, DateTime.MaxValue);
    }

    private String getData(String feedID, DateTime dt) {
      String url = apiURLbase + feedID.ToString() + ".csv";
      if (dt != DateTime.MaxValue) {
        dt = dt.Add(-hd.timeSpan);
        url += "?start=" + dt.ToString("yyyy-MM-ddThh:mm:ssZ") + "&end=" + dt.AddSeconds(5).ToString("yyyy-MM-ddThh:mm:ssZ");
      }

      string htmlCode = String.Empty;
      try {
        HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
        WebClient client = new WebClient();
        client.Credentials = new NetworkCredential("datel", "mrdatel");
        htmlCode = client.DownloadString(url);
        return htmlCode;
      } catch (WebException ex) {
        return "Error fetch data " + ex.Message;
      }
    }

    public void parseDataHouse(string data) {
      string[] radka = data.Split(new Char[] { '\n' });
      hd.resetHouse();
      foreach (string s in radka) {
        string[] column = s.Split(new Char[] { ',' });
        try {
          if (column[0] == "Corridor") {
            hd.corridorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.corridorColor = getRoomColor(hd.corridorTemp);
          }
          if (column[0] == "Hall") {
            hd.hallTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.hallColor = getRoomColor(hd.hallTemp);
          }
          if (column[0] == "LivingRoom") {
            hd.livingRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.livingRoomColor = getRoomColor(hd.livingRoomTemp);
          }
          if (column[0] == "WorkRoom") {
            hd.workRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.workRoomColor = getRoomColor(hd.workRoomTemp);
          }
          if (column[0] == "BedRoomOld") {
            hd.bedRoomTempOld = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.bedRoomOldColor = getRoomColor(hd.bedRoomTempOld);
          }
          if (column[0] == "BedRoomNew") {
            hd.bedRoomTempNew = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.bedRoomNewColor = getRoomColor(hd.bedRoomTempNew);
          }
          if (column[0] == "Bojler") {
            hd.bojlerTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.bojlerColor = getRoomColor(hd.bojlerTemp);
          }
          hd.lastUpdateHouse = getDateTimeFromCosmString(column[1] + column[2]);
        } catch (FormatException ex) {

        }
        //lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
      }
    }

    public void parseDataMeteo(string data) {
      /*  
            
       */
      string[] radka = data.Split(new Char[] { '\n' });
      hd.resetMeteo();
      foreach (string s in radka) {
        string[] column = s.Split(new Char[] { ',' });
        try {
          if (column[0] == "T2899BDCF02000076") {
            hd.outdoorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            hd.outdoorColor = getRoomColor(hd.outdoorTemp);
          }
          if (column[0] == "Humidity") {
            hd.humidity = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "Press") {
            hd.pressure = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "Temp085") {
            hd.temp085 = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "TempDHT") {
            hd.tempDHT = (float)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "WindS") {
            hd.windSpeed = (int)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "WindSM") {
            hd.windSpeedMax = (int)Convert.ToDecimal(column[2].Replace(".", ","));
          }
          if (column[0] == "WindD") {
            hd.windDirection = (int)Convert.ToDecimal(column[2].Replace(".", ","));
          }

          hd.lastUpdateMeteo = getDateTimeFromCosmString(column[1] + column[2]);

        } catch (FormatException ex) {

        }
        //lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
      }
    }

    public void parseDataSolar(string data) {
      parseDataSolar(data, false);
    }

    public void parseDataSolar(string data, bool hist) {
      string[] radka = data.Split(new Char[] { '\n' });
      hd.resetSolar();
      foreach (string s in radka) {
        string[] column = s.Split(new Char[] { ',' });
        try {
          if (column[0] == "IN") {
            if (hist) {
              hd.solarINTempHist = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            } else {
              hd.solarINTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "OUT") {
            if (hist) {
              hd.solarOUTTempHist = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            } else {
              hd.solarOUTTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "ROOM") {
            if (hist) {
              hd.solarROOMTempHist = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            } else {
              hd.solarROOMTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "Bojler2") {
            if (hist) {
              hd.bojler2TempHist = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            } else {
              hd.bojler2Temp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }

          if (column[0] == "Power") {
            if (hist) {
            } else {
              hd.solarPower = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "EnergyAday") {
            if (hist) {
            } else {
              hd.energyADay = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "EnergyTotal") {
            if (hist) {
            } else {
              hd.energyTotal = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "DiffON") {
            if (hist) {
            } else {
              hd.difON = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "DiffOFF") {
            if (hist) {
            } else {
              hd.difOFF = (float)Convert.ToDecimal(column[2].Replace(".", ","));
            }
          }
          if (column[0] == "Status") {
            if (hist) {
            } else {
              hd.statusSolar = column[2];
            }
          }

          if (!hist) {
            hd.lastUpdateSolar = getDateTimeFromCosmString(column[1] + column[2]);
          }
        } catch (FormatException ex) {

        }
      }
    }

    private string getRoomColor(float temperature) {
      if (temperature > 25)
        return Colors.Red.ToString();
      else if (temperature > 10)
        return Colors.Green.ToString();
      else if (temperature > 0)
        return Colors.AliceBlue.ToString();
      else
        return Colors.Blue.ToString();
    }

    private DateTime getDateTimeFromCosmString(string s) {
      if (s == string.Empty)
        return new DateTime();
      String[] dateAndTime = s.Split(new Char[] { 'T' });
      String[] date = dateAndTime[0].Split(new Char[] { '-' });
      String[] time = dateAndTime[1].Split(new Char[] { ':' });

      return new DateTime(Convert.ToInt16(date[0]), Convert.ToInt16(date[1]), Convert.ToInt16(date[2]), Convert.ToInt16(time[0]), Convert.ToInt16(time[1]), Convert.ToInt16(time[2].Substring(0, time[2].IndexOf('.'))));
    }

    public void showDataMeteo() {
      hd.statusLEDMeteoColor = Colors.Yellow.ToString();
      parseDataMeteo(getDataMeteo());
      hd.statusLEDMeteoColor = Colors.Green.ToString();
    }
    public void showDataHouse() {
      hd.statusLEDHouseColor = Colors.Yellow.ToString();
      parseDataHouse(getDataHouse());
      hd.statusLEDHouseColor = Colors.Green.ToString();
    }
    public void showDataSolar() {
      hd.statusLEDSolarColor = Colors.GreenYellow.ToString();
      parseDataSolar(getDataSolar());
      parseDataSolar(getDataSolarHist(DateTime.Now.AddHours(-1)),true);
      hd.statusLEDSolarColor = Colors.Green.ToString();
    }

    internal static float max(float a, float b) {
      if (a > b) return a;
      else return b;
    }
  }
}
