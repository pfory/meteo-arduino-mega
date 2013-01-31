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
        private static int downloadInterval = 20000; //in ms
        private Timer timer = new Timer(downloadInterval);
        HouseData hd = new HouseData();

        public MainWindow()
        {
            InitializeComponent();
            this.Language = XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag);
            DataContext = hd;
            timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);
            timer.Enabled = true;
            showData();
        }

        private void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            showData();
        }

        private void showData()
        {
            hd.statusLEDColor = Colors.Yellow.ToString();
            parseData(getData());
            hd.statusLEDColor = Colors.Green.ToString();
        }


        static private String getData()
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

        private void parseData(string data)
        {
            /*  
            
             */
            string[] radka = data.Split(new Char[] { '\n' });
            hd.resetTemperatures();
            foreach (string s in radka)
            {
                string[] column = s.Split(new Char[] { ',' });
                try
                {
                    if (column[0] == "T28C9B84104000097")
                    {
                        hd.corridorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                        hd.corridorColor = getRoomColor(hd.corridorTemp);
                    }
                    if (column[0] == "T28AEC04104000082")
                    {
                        hd.hallTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                        hd.hallColor = getRoomColor(hd.hallTemp);
                    }
                    if (column[0] == "T28A6B0410400004E")
                    {
                        hd.livingRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                        hd.livingRoomColor = getRoomColor(hd.livingRoomTemp);
                    }
                    if (column[0] == "T28CEB0410400002C")
                    {
                        hd.workRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                        hd.workRoomColor = getRoomColor(hd.workRoomTemp);
                    }
                    if (column[0] == "T28E8B84104000016")
                    {
                        hd.bedRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                        hd.bedRoomColor = getRoomColor(hd.workRoomTemp);
                    }
                    if (column[0] == "T285DF3CF0200007E")
                    {
                      hd.bojlerTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                      hd.bojlerColor = getRoomColor(hd.bojlerTemp);
                    }
                    if (column[0] == "T2899BDCF02000076")
                    {
                      hd.outdoorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                      hd.outdoorColor = getRoomColor(hd.outdoorTemp);
                    }
                    if (column[0] == "Humidity")
                    {
                      hd.humidity = (float)Convert.ToDecimal(column[2].Replace(".",","));
                    }
                    if (column[0] == "Press")
                    {
                      hd.pressure = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                    }
                    if (column[0] == "Temp085")
                    {
                      hd.temp085 = (float)Convert.ToDecimal(column[2].Replace(".",","));
                    }
                    if (column[0] == "TempDHT")
                    {
                      hd.tempDHT = (float)Convert.ToDecimal(column[2].Replace(".",","));
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

                    hd.lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);

                }
                catch (FormatException ex)
                {

                }
                //lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
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
            tbBedRoomTemp.Visibility = System.Windows.Visibility.Visible;
          }
          else
          {
            tbBedRoomTemp.Visibility = System.Windows.Visibility.Hidden;
          }
          lblBedroom.Visibility = tbBedRoomTemp.Visibility;
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
        private float     _bedRoomTemp;
        private float     _bojlerTemp;
        private float     _outdoorTemp;
        private string    _corridorColor;
        private string    _hallColor;
        private string    _livingRoomColor;
        private string    _workRoomColor;
        private string    _bedRoomColor;
        private string    _bojlerColor;
        private string    _outdoorColor;
        private string    _statusLEDColor;
        private DateTime  _lastUpdate;
        private float     _humidity;
        private float     _temp085;
        private float     _tempDHT;
        private float      _pressure;
        private int       _windSpeed;
        private int       _windSpeedMax;
        private int       _windDirection;
        private const int _pulsesPerRevolution=5;

        public HouseData()
        {
        }

        public void resetTemperatures() {
            bedRoomTemp = corridorTemp = hallTemp = workRoomTemp = livingRoomTemp = _bojlerTemp = _outdoorTemp = -127;
            bedRoomColor = corridorColor = hallColor = workRoomColor = livingRoomColor = outdoorColor = Colors.Red.ToString();
        }

        public float corridorTemp     { get { return _corridorTemp; }               set { _corridorTemp       = value; NotifyPropertyChanged("corridorTemp"); }}
        public float hallTemp         { get { return _hallTemp; }                   set { _hallTemp           = value; NotifyPropertyChanged("hallTemp"); }}
        public float livingRoomTemp   { get { return _livingRoomTemp; }             set { _livingRoomTemp     = value; NotifyPropertyChanged("livingRoomTemp"); }}
        public float workRoomTemp     { get { return _workRoomTemp; }               set { _workRoomTemp       = value; NotifyPropertyChanged("workRoomTemp"); }}
        public float bedRoomTemp      { get { return _bedRoomTemp; }                set { _bedRoomTemp        = value; NotifyPropertyChanged("bedRoomTemp"); }}
        public float bojlerTemp       { get { return _bojlerTemp; }                 set { _bojlerTemp         = value; NotifyPropertyChanged("bojlerTemp"); } }
        public float outdoorTemp      { get { return _outdoorTemp; }                set { _outdoorTemp        = value; NotifyPropertyChanged("outdoorTemp"); } }

        public string corridorColor   { get { return _corridorColor.ToString(); }   set { _corridorColor      = value; NotifyPropertyChanged("corridorColor"); }}
        public string hallColor       { get { return _hallColor.ToString(); }       set { _hallColor          = value; NotifyPropertyChanged("hallColor"); }}
        public string livingRoomColor { get { return _livingRoomColor.ToString(); } set { _livingRoomColor    = value; NotifyPropertyChanged("livingRoomColor"); }}
        public string workRoomColor   { get { return _workRoomColor.ToString(); }   set { _workRoomColor      = value; NotifyPropertyChanged("workRoomColor"); }}
        public string bedRoomColor    { get { return _bedRoomColor.ToString(); }    set { _bedRoomColor       = value; NotifyPropertyChanged("bedRoomColor"); }}
        public string bojlerColor     { get { return _bojlerColor.ToString(); }     set { _bojlerColor        = value; NotifyPropertyChanged("bojlerColor"); } }
        public string outdoorColor    { get { return _outdoorColor.ToString(); }    set { _outdoorColor       = value; NotifyPropertyChanged("outdoorColor"); } }

        public string statusLEDColor  { get { return _statusLEDColor; }             set { _statusLEDColor     = value; NotifyPropertyChanged("statusLEDColor");}}

        public float humidity         { get { return _humidity; }                   set { _humidity           = value; NotifyPropertyChanged("humidity"); }}
        public float temp085          { get { return _temp085; }                    set { _temp085            = value; NotifyPropertyChanged("temp085"); }}
        public float tempDHT          { get { return _tempDHT; }                    set { _tempDHT            = value; NotifyPropertyChanged("tempDHT"); }}
        public float pressure         { get { return _pressure / 100; }             set { _pressure           = value; NotifyPropertyChanged("pressure"); }}

        public int windSpeed          { get { return (_windSpeed/_pulsesPerRevolution) * 60; }
                                                                                    set { _windSpeed          = value; NotifyPropertyChanged("windSpeed"); } }
        public int windSpeedMax       { get { return (_windSpeedMax/_pulsesPerRevolution) * 60; }
                                                                                    set { _windSpeedMax       = value; NotifyPropertyChanged("windSpeedMax"); } }
        public int windDirection      { get { return _windDirection; }              set { _windDirection      = value; NotifyPropertyChanged("windDirection"); } }

        public DateTime lastUpdate    { get { return _lastUpdate; }                 set { _lastUpdate         = value; NotifyPropertyChanged("lastUpdate"); } }

    }
}
