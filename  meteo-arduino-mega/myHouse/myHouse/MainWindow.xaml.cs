using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Timers;
using System.Globalization;

namespace myHouse
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private float corridorTemp;
        private float hallTemp;
        private float livingRoomTemp;
        private float workRoomTemp;
        private Timer timer = new Timer(5000);

        public MainWindow()
        {
            InitializeComponent();
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
            /*corridorTemp = 10.5F;
            hallTemp = 20.8F;
            livingRoomTemp = 35.8F;
            workRoomTemp = -5.6F;
            */
            /*lblCorridorTemp.Content = corridorTemp;
            lblHallTemp.Content = hallTemp;
            lblLivingRoomTemp.Content = livingRoomTemp;
            lblWorkroomTemp.Content = workRoomTemp;
            */
            status.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { status.Fill = new SolidColorBrush(Colors.Yellow); }));
            parseData(getData());


            lblCorridorTemp.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblCorridorTemp.Content = corridorTemp.ToString("F1", CultureInfo.CurrentCulture); }));
            lblHallTemp.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblHallTemp.Content = hallTemp.ToString("F1", CultureInfo.CurrentCulture); }));
            lblLivingRoomTemp.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblLivingRoomTemp.Content = livingRoomTemp.ToString("F1", CultureInfo.CurrentCulture); }));
            lblWorkroomTemp.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { lblWorkroomTemp.Content = workRoomTemp.ToString("F1", CultureInfo.CurrentCulture); }));

            corridorRect.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { corridorRect.Fill = getRoomColor(corridorTemp) ; corridorRect.Opacity = 0.1; }));
            hallRect.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { hallRect.Fill = getRoomColor(hallTemp); hallRect.Opacity = 0.1; }));
            livingRoomRect.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { livingRoomRect.Fill = getRoomColor(livingRoomTemp); livingRoomRect.Opacity = 0.1; }));
            workRoomRect.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { workRoomRect.Fill = getRoomColor(workRoomTemp); workRoomRect.Opacity = 0.1; }));

            status.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal, new Action(delegate() { status.Fill = new SolidColorBrush(Colors.Green); }));
        }

        private Brush getRoomColor(float temperature) {
            SolidColorBrush brush = new SolidColorBrush();

            if (temperature < 8)
            {
                brush.Color = Colors.Blue;
            }
            else if (temperature > 25)
            {
                brush.Color = Colors.Red;
            }

            return brush;
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
                    corridorTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == "T28AEC04104000082")
                    hallTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == "T28A6B0410400004E")
                    livingRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
                if (column[0] == "T28CEB0410400002C")
                    workRoomTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));

                //lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
            }
        }
    }
}
