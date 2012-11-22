using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Timers;

namespace WindowTransparency
{
  /// <summary>
  /// Interaction logic for Window2.xaml
  /// </summary>
  public partial class Window2 : Window
  {
    Timer t = new Timer(1000);
    
    public Window2()
    {
      InitializeComponent();
      t.Start();
    }


    private void rectangle1_MouseEnter(object sender, MouseEventArgs e)
    {
      //Rectangle rec = (Rectangle)sender;
      //string name = rec.Name;
      rectangle1.Height = 250;
    }

    private void rectangle1_MouseLeave(object sender, MouseEventArgs e)
    {
      rectangle1.Height = 50;

    }

  
  }
}
