using System;
using System.Globalization;
using System.Net;
using System.Timers;
using System.Windows;
using System.Windows.Media;

namespace SolarSystem
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Timer timer = new Timer(5000);
        //private static String data;
        private float solarInput;
        private float solarOutput;
        private float bojler;
        private float outTemp;
        private float intTemp;
        private SetupData setupData;
        private DateTime lastUpdate;

        public MainWindow()
        {
            InitializeComponent();
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
            lblError.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblError.Content = String.Empty; }));

            Status.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { Status.Fill = new SolidColorBrush(Colors.Yellow); }));
            try
            {
                String data = getData(setupData);
                if (data != String.Empty)
                {
                    parseData(data);
                    lblSolar.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblSolar.Content = data; }));
                }
            }
            catch (Exception ex) {
                lblError.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblError.Content = ex.Message; }));
            }

            SolarInput.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { SolarInput.Content = solarInput.ToString("F1", CultureInfo.CurrentCulture); }));
            SolarOutput.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { SolarOutput.Content = solarOutput.ToString("F1", CultureInfo.CurrentCulture); }));
            bojlerTeplota.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { bojlerTeplota.Content = bojler.ToString("F1", CultureInfo.CurrentCulture); }));
            venkovniTeplota.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { venkovniTeplota.Content = outTemp.ToString("F1", CultureInfo.CurrentCulture); }));
            vnitrniTeplota.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { vnitrniTeplota.Content = intTemp.ToString("F1", CultureInfo.CurrentCulture); }));

            float solarPower = (5f / 1000f / 60f) * (solarOutput - solarInput) * 1054f * 3900f;
            if (solarPower > 0)
            {
                pictureClouds.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { pictureClouds.Visibility = Visibility.Hidden; }));
                pictureSun.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { pictureSun.Visibility = Visibility.Visible; }));
            }
            else
            {
                pictureClouds.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { pictureClouds.Visibility = Visibility.Visible; }));
                pictureSun.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { pictureSun.Visibility = Visibility.Hidden; }));
            }
            SolarPower.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { SolarPower.Content = solarPower.ToString("F3", CultureInfo.CurrentCulture); }));
            Status.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { Status.Fill = new SolidColorBrush(Colors.Green); }));
            lblDateTime.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblDateTime.Content = lastUpdate.ToLongDateString() + "\n" + lastUpdate.ToLongTimeString(); }));
        }

        private void parseData(string data)
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
            string[] radka = data.Split(new Char[] { '\n' });
            foreach (string s in radka)
            {
                string[] column = s.Split(new Char[] { ',' });
                if (column[0] == setupData.solarVstup)
                    solarInput = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.solarVystup)
                    solarOutput = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.bojler)
                    bojler = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.venkovni)
                    outTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == setupData.vnitrni)
                    intTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                
                lastUpdate = getDateTimeFromCosmString(column[1]+column[2]);
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

        static private String getData(SetupData setupData)
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
                htmlCode = client.DownloadString(url);
                return htmlCode;
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
}
