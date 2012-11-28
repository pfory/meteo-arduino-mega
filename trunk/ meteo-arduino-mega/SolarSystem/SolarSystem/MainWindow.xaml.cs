using System;
using System.ComponentModel;
using System.Globalization;
using System.Net;
using System.Timers;
using System.Windows;
using System.Windows.Markup;
using System.Windows.Media;

namespace SolarSystem
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Timer timer = new Timer(5000);
        private SetupData setupData;
        SolarData sd = new SolarData();


        public MainWindow()
        {
            InitializeComponent();
            this.Language = XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag);
            DataContext = sd;
            timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);
            timer.Enabled = true;
        }

        private void Window_Loaded_1(object sender, RoutedEventArgs e)
        {
            setupData = new SetupData();
            showData(setupData);
        }

        private void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            showData(setupData);
        }

        private void showData(SetupData setupData)
        {
            sd.statusLEDColor = Colors.Yellow.ToString();
            try
            {
                getData(setupData);
                if (sd.rawData != String.Empty)
                {
                    parseData();
                }
            }
            catch (Exception ex) {
                sd.error = ex.Message;
            }

            sd.statusLEDColor = Colors.Green.ToString();
        }

        private void parseData()
        {
            /*  DewPoint1,2012-11-26T09:31:57.811788Z,3
                DewPoint2,2012-11-26T09:31:57.811788Z,6
                Humidity1,2012-11-26T09:31:57.811788Z,95
                Humidity2,2012-11-26T09:31:57.811788Z,64
                Press,2012-11-26T09:31:57.811788Z,101190
                T2828C6F3010000E5,2012-11-26T09:31:57.811788Z,21.8
                T28446CF3010000D5,2012-11-26T09:31:57.811788Z,5.5
                T2890A5F30100001C,2012-11-26T09:31:57.811788Z,21.0
                T289879F3010000C2,2012-11-26T09:31:57.811788Z,14.5
                T28D376F301000020,2012-11-26T09:31:57.811788Z,47.2
                TempDHT1,2012-11-26T09:31:57.811788Z,4
                TempDHT2,2012-11-26T09:31:57.811788Z,13
             */
            string[] radka = sd.rawData.Split(new Char[] { '\n' });
            foreach (string s in radka)
            {
                string[] column = s.Split(new Char[] { ',' });
                if (column[0] == setupData.solarVstup)
                    sd.solarInput = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.solarVystup)
                    sd.solarOutput = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.bojler)
                    sd.bojler = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.venkovni)
                    sd.outTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.vnitrni)
                    sd.intTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                
                sd.lastUpdate = getDateTimeFromCosmString(column[1]+column[2]);
            }
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

        private void getData(SetupData setupData)
        {
            string url = "http://api.cosm.com/v2/feeds/" + setupData.cosmID + ".csv";

            string htmlCode = String.Empty;
            try
            {
                // Create Request
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);

                // Create Client
                WebClient client = new WebClient();

                // Assign Credentials
                client.Credentials = new NetworkCredential(setupData.userName, setupData.password);

                // Grab Data
                sd.rawData = client.DownloadString(url);
            }
            catch (WebException ex)
            {
                throw ex;
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Setup win = new Setup();
            win.ShowDialog();
            setupData.loadData();
            showData(setupData);
        }
    }

    public class SolarData : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(info));
        }
        public SolarData()
        {
        }

        private float _solarInput;
        private float _solarOutput;
        private float _bojler;
        private float _outTemp;
        private float _intTemp;
        private float _solarPower;
        private string _statusLEDColor;
        private DateTime _lastUpdate;
        private string _rawData;
        private Visibility _pictureClouds;
        private Visibility _pictureSun;
        
        private string _error;

        public float solarInput { get { return _solarInput; } set { _solarInput = value; solarPower = 0; NotifyPropertyChanged("solarInput"); }}
        public float solarOutput { get { return _solarOutput; } set { _solarOutput = value; solarPower = 0; NotifyPropertyChanged("solarOutput"); }}
        public float bojler { get { return _bojler; } set { _bojler = value; NotifyPropertyChanged("bojler"); }}
        public float outTemp { get { return _outTemp; } set { _outTemp = value; NotifyPropertyChanged("outTemp"); }}
        public float intTemp { get { return _intTemp; } set { _intTemp = value; NotifyPropertyChanged("intTemp"); }}
        
        public float solarPower { get { return _solarPower; } 
            set { 
                _solarPower = (5f / 1000f / 60f) * (_solarOutput - _solarInput) * 1054f * 3900f; NotifyPropertyChanged("solarPower");
                pictureClouds = Visibility.Hidden;
                pictureSun = Visibility.Hidden;
            }
        }

        public string error { get { return _error; } set { _error = value; NotifyPropertyChanged("error"); }}

        public string statusLEDColor { get { return _statusLEDColor; } set { _statusLEDColor = value; NotifyPropertyChanged("statusLEDColor"); }}
        public DateTime lastUpdate { get { return _lastUpdate; } set { _lastUpdate = value; NotifyPropertyChanged("lastUpdate"); }}
        public string rawData { get { return _rawData; } set { _rawData = value; NotifyPropertyChanged("rawData"); }}

        public Visibility pictureClouds { 
            get { 
                return _pictureClouds; } 
            set {
                if (solarPower > 0)
                    _pictureClouds = Visibility.Hidden;
                else
                    _pictureClouds = Visibility.Visible;
                NotifyPropertyChanged("pictureClouds");
            }
        }
        public Visibility pictureSun {
            get {
                return _pictureSun; }
            set {
                if (solarPower > 0)
                    _pictureSun = Visibility.Visible;
                else
                    _pictureSun = Visibility.Hidden;
                NotifyPropertyChanged("pictureSun");
            } 
        }
    }
}
