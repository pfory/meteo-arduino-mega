using System;
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
    public partial class MainWindow : Window, IDisposable
    {


      service srv = new service();

      public MainWindow()
      {
        InitializeComponent();

        this.Language = XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag);
        DataContext = srv.hd;
        srv.showDataMeteo();
        srv.showDataHouse();
        srv.showDataSolar();
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

      public void Dispose() {
        srv.Dispose();
        GC.SuppressFinalize(this);
      }
    }
}
