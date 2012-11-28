﻿using System;
using System.Net;
using System.Windows;
using System.Timers;
using System.ComponentModel;
using System.Windows.Media;

namespace myHouse
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static int downloadInterval = 5000; //in ms
        private Timer timer = new Timer(downloadInterval);
        HouseData hd = new HouseData();

        public MainWindow()
        {
            InitializeComponent();
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
            string url = "http://api.cosm.com/v2/feeds/" + "63310" + ".csv";

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
            foreach (string s in radka)
            {
                string[] column = s.Split(new Char[] { ',' });
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
    }


    public class HouseData : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(info));
        }

        private float _corridorTemp;
        private float _hallTemp;
        private float _livingRoomTemp;
        private float _workRoomTemp;
        private string _corridorColor;
        private string _hallColor;
        private string _livingRoomColor;
        private string _workRoomColor;
        private string _statusLEDColor;

        public HouseData()
        {
        }

        public float corridorTemp { get { return _corridorTemp; } set { _corridorTemp = value; NotifyPropertyChanged("corridorTemp");}}
        public float hallTemp { get { return _hallTemp; } set { _hallTemp = value; NotifyPropertyChanged("hallTemp"); } }
        public float livingRoomTemp { get { return _livingRoomTemp; } set { _livingRoomTemp = value; NotifyPropertyChanged("livingRoomTemp"); } }
        public float workRoomTemp { get { return _workRoomTemp; } set { _workRoomTemp = value; NotifyPropertyChanged("workRoomTemp"); } }

        public string corridorColor { get { return _corridorColor.ToString(); } set { _corridorColor = value; NotifyPropertyChanged("corridorColor"); } }
        public string hallColor { get { return _hallColor.ToString(); } set { _hallColor = value; NotifyPropertyChanged("hallColor"); } }
        public string livingRoomColor { get { return _livingRoomColor.ToString(); } set { _livingRoomColor = value; NotifyPropertyChanged("livingRoomColor"); } }
        public string workRoomColor { get { return _workRoomColor.ToString(); } set { _workRoomColor = value; NotifyPropertyChanged("workRoomColor"); } }

        public string statusLEDColor { get { return _statusLEDColor; } set { _statusLEDColor = value; NotifyPropertyChanged("statusLEDColor");}}
    }
}