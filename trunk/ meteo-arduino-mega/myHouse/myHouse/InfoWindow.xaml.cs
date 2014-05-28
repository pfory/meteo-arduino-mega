using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace myHouse
{
  /// <summary>
  /// Interaction logic for Window1.xaml
  /// </summary>
  public partial class Window1 : Window,IDisposable
  {
    service srv = new service();

    public Window1()
    {
      InitializeComponent();

      //this.Language = XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag);
      DataContext = srv.hd;
      srv.showDataSolar();
      //srv.hd.sendEmail(2);
    }

    public void Dispose() {
      srv.Dispose();
      GC.SuppressFinalize(this);
    }
  }
}
