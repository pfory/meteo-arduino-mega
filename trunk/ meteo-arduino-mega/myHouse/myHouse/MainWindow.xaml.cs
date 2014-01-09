using System;
using System.Net;
using System.Windows;
using System.Timers;
using System.ComponentModel;
using System.Windows.Media;
using System.Windows.Markup;
using System.Globalization;
using System.Windows.Controls;

namespace myHouse
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static int downloadIntervalMeteo = 20000; //in ms
        private static int downloadIntervalHouse = 15000; //in ms
        private static int downloadIntervalSolar = 5000; //in ms
        private Timer timerMeteo = new Timer(downloadIntervalMeteo);
        private Timer timerHouse = new Timer(downloadIntervalHouse);
        private Timer timerSolar = new Timer(downloadIntervalSolar);
        HouseData hd = new HouseData();

        public MainWindow()
        {
            InitializeComponent();
            this.Language = XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag);
            DataContext = hd;
            timerMeteo.Elapsed += new ElapsedEventHandler(OnTimedEventMeteo);
            showDataMeteo();
            timerMeteo.Enabled = true;
            timerHouse.Elapsed += new ElapsedEventHandler(OnTimedEventHouse);
            showDataHouse();
            timerHouse.Enabled = true;
            timerSolar.Elapsed += new ElapsedEventHandler(OnTimedEventSolar);
            showDataSolar();
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

        private void showDataMeteo()
        {
            hd.statusLEDMeteoColor = Colors.Yellow.ToString();
            parseDataMeteo(getDataMeteo());
            hd.statusLEDMeteoColor = Colors.Green.ToString();
        }
        private void showDataHouse()
        {
          hd.statusLEDHouseColor = Colors.Yellow.ToString();
          parseDataHouse(getDataHouse());
          hd.statusLEDHouseColor = Colors.Green.ToString();
        }
        private void showDataSolar()
        {
          hd.statusLEDSolarColor = Colors.Yellow.ToString();
          parseDataSolar(getDataSolar());
          hd.statusLEDSolarColor = Colors.Green.ToString();
        }


        static private String getDataHouse()
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

        static private String getDataMeteo()
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

        static private String getDataSolar()
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


        private void parseDataHouse(string data)
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

        private void parseDataMeteo(string data)
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

        private void parseDataSolar(string data)
        {
          string[] radka = data.Split(new Char[] { '\n' });
          hd.resetSolar();
          foreach (string s in radka)
          {
            string[] column = s.Split(new Char[] { ',' });
            try
            {
              if (column[0] == "IN") {
                hd.solarINTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                //hd.corridorColor = getRoomColor(hd.corridorTemp);
              }
              if (column[0] == "OUT") {
                hd.solarOUTTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
              }
              if (column[0] == "ROOM") {
                hd.solarROOMTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
              }
              if (column[0] == "Power") {
                hd.solarPower = (float)Convert.ToDecimal(column[2].Replace(".", ","));
              }
              if (column[0] == "EnergyAday") {
                hd.energyADay = (float)Convert.ToDecimal(column[2].Replace(".", ","));
              }
              if (column[0] == "EnergyTotal") {
                hd.energyTotal = (float)Convert.ToDecimal(column[2].Replace(".", ","));
              }
              if (column[0] == "DifON") {
                hd.difON = (float)Convert.ToDecimal(column[2].Replace(".", ","));
              }
              if (column[0] == "DifOFF") {
                hd.difOFF = (float)Convert.ToDecimal(column[2].Replace(".", ","));
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
            if (temperature>25)
                return Colors.Red.ToString();
            else if (temperature>10)
                return Colors.Green.ToString();
            else if (temperature>0)
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


        private void cb1Floor_Click(object sender, RoutedEventArgs e)
        {
          CheckBox cb = (CheckBox)sender;
          if (cb.IsChecked == true)
          {
            tbHallTemp.Visibility = tbLivingRoomTemp.Visibility = tbWorkroomTemp.Visibility = tbCorridorTemp.Visibility = 
              tbOutdoorTemp.Visibility =
              lblPressure.Visibility = lblTemp085.Visibility = lblTempDHT.Visibility = lblHumidity.Visibility = 
              tbBojlerTemp.Visibility = lblBojler.Visibility = lblOutdoor.Visibility = System.Windows.Visibility.Visible;
            lblHall.Visibility = lblLivingRoom.Visibility = lblWorkRoom.Visibility = lblCorridor.Visibility =
              tbHumidity.Visibility = tbPressure.Visibility = tbTempDHT.Visibility = tbTemp085.Visibility = System.Windows.Visibility.Visible;
          }
          else
          {
            tbHallTemp.Visibility = tbLivingRoomTemp.Visibility = tbWorkroomTemp.Visibility = tbCorridorTemp.Visibility =
              tbOutdoorTemp.Visibility =
              lblPressure.Visibility = lblTemp085.Visibility = lblTempDHT.Visibility = lblHumidity.Visibility = 
              tbBojlerTemp.Visibility = lblBojler.Visibility = lblOutdoor.Visibility = System.Windows.Visibility.Hidden;
            lblHall.Visibility = lblLivingRoom.Visibility = lblWorkRoom.Visibility = lblCorridor.Visibility =
              tbHumidity.Visibility = tbPressure.Visibility = tbTempDHT.Visibility = tbTemp085.Visibility = System.Windows.Visibility.Hidden;
          }
        }

        private void cb2Floor_Click(object sender, RoutedEventArgs e)
        {
          CheckBox cb = (CheckBox)sender;
          if (cb.IsChecked == true)
          {
            tbBedRoomTempOld.Visibility = System.Windows.Visibility.Visible;
            tbBedRoomTempNew.Visibility = System.Windows.Visibility.Visible;
          }
          else
          {
            tbBedRoomTempOld.Visibility = System.Windows.Visibility.Hidden;
            tbBedRoomTempNew.Visibility = System.Windows.Visibility.Hidden;
          }
          lblBedroomOld.Visibility = tbBedRoomTempOld.Visibility;
          lblBedroomNew.Visibility = tbBedRoomTempNew.Visibility;
        }

      private void roof_Click(object sender, RoutedEventArgs e)
        {
          CheckBox cb = (CheckBox)sender;
          if (cb.IsChecked == true)
          {
            lblWindSpeed.Visibility = lblWindSpeedMax.Visibility = lblWindDirection.Visibility = Visibility.Visible;
            tbWindSpeed.Visibility = tbWindSpeedMax.Visibility = tbWindDirection.Visibility = Visibility.Visible;
          }
          else
          {
            lblWindSpeed.Visibility = lblWindSpeedMax.Visibility = lblWindDirection.Visibility = Visibility.Hidden;
            tbWindSpeed.Visibility = tbWindSpeedMax.Visibility = tbWindDirection.Visibility = Visibility.Hidden;
          }
        }

    }


    public class HouseData : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(info));
        }

        private float     _corridorTemp;
        private float     _hallTemp;
        private float     _livingRoomTemp;
        private float     _workRoomTemp;
        private float     _bedRoomTempOld;
        private float     _bedRoomTempNew;
        private float     _bojlerTemp;
        private float     _outdoorTemp;
        private string    _corridorColor;
        private string    _hallColor;
        private string    _livingRoomColor;
        private string    _workRoomColor;
        private string    _bedRoomOldColor;
        private string    _bedRoomNewColor;
        private string    _bojlerColor;
        private string    _outdoorColor;
        private string    _statusLEDHouseColor;
        private string    _statusLEDMeteoColor;
        private string    _statusLEDSolarColor;
        private DateTime  _lastUpdateHouse;
        private DateTime  _lastUpdateSolar;
        private DateTime  _lastUpdateMeteo;
        private float     _humidity;
        private float     _temp085;
        private float     _tempDHT;
        private float     _pressure;
        private int       _windSpeed;
        private int       _windSpeedMax;
        private int       _windDirection;
        private const int _pulsesPerRevolution=5;
        private float     _solarINTemp;
        private float     _solarOUTTemp;
        private float     _solarROOMTemp;
        private float     _solarPower;
        private float     _energyADay;
        private float     _energyTotal;
        private float     _difON;
        private float     _difOFF;

        public HouseData()
        {
        }

        public void resetHouse() {
          bedRoomTempOld = corridorTemp = hallTemp = workRoomTemp = livingRoomTemp = _bojlerTemp = _outdoorTemp = bedRoomTempNew = -127;
          bedRoomOldColor = bedRoomNewColor = corridorColor = hallColor = workRoomColor = livingRoomColor = outdoorColor = Colors.Red.ToString();
        }
        public void resetMeteo() {
        }
        public void resetSolar() {
        }

        public float corridorTemp     { get { return _corridorTemp; }               set { _corridorTemp       = value; NotifyPropertyChanged("corridorTemp"); }}
        public float hallTemp         { get { return _hallTemp; }                   set { _hallTemp           = value; NotifyPropertyChanged("hallTemp"); }}
        public float livingRoomTemp   { get { return _livingRoomTemp; }             set { _livingRoomTemp     = value; NotifyPropertyChanged("livingRoomTemp"); }}
        public float workRoomTemp     { get { return _workRoomTemp; }               set { _workRoomTemp       = value; NotifyPropertyChanged("workRoomTemp"); }}
        public float bedRoomTempOld   { get { return _bedRoomTempOld; }             set { _bedRoomTempOld     = value; NotifyPropertyChanged("bedRoomTempOld"); }}
        public float bedRoomTempNew   { get { return _bedRoomTempNew; }             set { _bedRoomTempNew     = value; NotifyPropertyChanged("bedRoomTempNew"); } }
        public float bojlerTemp       { get { return _bojlerTemp; }                 set { _bojlerTemp         = value; NotifyPropertyChanged("bojlerTemp"); } }
        public float outdoorTemp      { get { return _outdoorTemp; }                set { _outdoorTemp        = value; NotifyPropertyChanged("outdoorTemp"); } }
        public float solarINTemp      { get { return _solarINTemp; }                set { _solarINTemp        = value; NotifyPropertyChanged("solarINTemp"); } }
        public float solarOUTTemp     { get { return _solarOUTTemp; }               set { _solarOUTTemp       = value; NotifyPropertyChanged("solarOUTTemp"); } }
        public float solarROOMTemp    { get { return _solarROOMTemp; }              set { _solarROOMTemp      = value; NotifyPropertyChanged("solarROOMTemp"); } }

        public string corridorColor   { get { return _corridorColor.ToString(); }   set { _corridorColor      = value; NotifyPropertyChanged("corridorColor"); }}
        public string hallColor       { get { return _hallColor.ToString(); }       set { _hallColor          = value; NotifyPropertyChanged("hallColor"); }}
        public string livingRoomColor { get { return _livingRoomColor.ToString(); } set { _livingRoomColor    = value; NotifyPropertyChanged("livingRoomColor"); }}
        public string workRoomColor   { get { return _workRoomColor.ToString(); }   set { _workRoomColor      = value; NotifyPropertyChanged("workRoomColor"); }}
        public string bedRoomOldColor { get { return _bedRoomOldColor.ToString(); } set { _bedRoomOldColor    = value; NotifyPropertyChanged("bedRoomOldColor"); }}
        public string bedRoomNewColor { get { return _bedRoomNewColor.ToString(); } set { _bedRoomNewColor    = value; NotifyPropertyChanged("bedRoomNewColor"); } }
        public string bojlerColor     { get { return _bojlerColor.ToString(); }     set { _bojlerColor        = value; NotifyPropertyChanged("bojlerColor"); } }
        public string outdoorColor    { get { return _outdoorColor.ToString(); }    set { _outdoorColor       = value; NotifyPropertyChanged("outdoorColor"); } }

        public string statusLEDHouseColor   { get { return _statusLEDHouseColor; }  set { _statusLEDHouseColor= value; NotifyPropertyChanged("statusLEDHouseColor");}}
        public string statusLEDSolarColor   { get { return _statusLEDSolarColor; }  set { _statusLEDSolarColor= value; NotifyPropertyChanged("statusLEDSolarColor"); } }
        public string statusLEDMeteoColor   { get { return _statusLEDMeteoColor; }  set { _statusLEDMeteoColor= value; NotifyPropertyChanged("statusLEDMeteoColor"); } }

        public float humidity         { get { return _humidity; }                   set { _humidity           = value; NotifyPropertyChanged("humidity"); }}
        public float temp085          { get { return _temp085; }                    set { _temp085            = value; NotifyPropertyChanged("temp085"); }}
        public float tempDHT          { get { return _tempDHT; }                    set { _tempDHT            = value; NotifyPropertyChanged("tempDHT"); }}
        public float pressure         { get { return _pressure / 100; }             set { _pressure           = value; NotifyPropertyChanged("pressure"); }}

        public int windSpeed          { get { return (_windSpeed/_pulsesPerRevolution) * 60; }
                                                                                    set { _windSpeed          = value; NotifyPropertyChanged("windSpeed"); } }
        public int windSpeedMax       { get { return (_windSpeedMax/_pulsesPerRevolution) * 60; }
                                                                                    set { _windSpeedMax       = value; NotifyPropertyChanged("windSpeedMax"); } }
        public int windDirection      { get { return _windDirection; }              set { _windDirection      = value; NotifyPropertyChanged("windDirection"); } }

        public DateTime lastUpdateHouse    { get { return _lastUpdateHouse; }       set { _lastUpdateHouse    = value; NotifyPropertyChanged("lastUpdateHouse"); } }
        public DateTime lastUpdateMeteo    { get { return _lastUpdateMeteo; }       set { _lastUpdateMeteo    = value; NotifyPropertyChanged("lastUpdateMeteo"); } }
        public DateTime lastUpdateSolar    { get { return _lastUpdateSolar; }       set { _lastUpdateSolar    = value; NotifyPropertyChanged("lastUpdateSolar"); } }

        public float solarPower       { get { return _solarPower; }                 set { _solarPower         = value; NotifyPropertyChanged("solarPower"); } }
        public float energyADay       { get { return _energyADay; }                 set { _energyADay         = value; NotifyPropertyChanged("energyADay"); } }
        public float energyTotal      { get { return _energyTotal; }                set { _energyTotal        = value; NotifyPropertyChanged("energyTotal"); } }
        public float difOFF           { get { return _difOFF; }                     set { _difOFF             = value; NotifyPropertyChanged("difOFF"); } }
        public float difON            { get { return _difON; }                      set { _difON              = value; NotifyPropertyChanged("difON"); } }


    }
}
