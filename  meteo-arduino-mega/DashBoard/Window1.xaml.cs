using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Media.Media3D;
using System.Windows.Media.Animation;


namespace WindowTransparency
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(Window1_Loaded); 
            mainViewport.MouseLeftButtonDown += new MouseButtonEventHandler(mainViewport_MouseLeftButtonDown);
            mainViewport.MouseRightButtonDown+=new MouseButtonEventHandler(mainViewport_MouseRightButtonDown);  
        }

        void Window1_Loaded(object sender, RoutedEventArgs e)
        {
            //Storyboard s;

            //s = (Storyboard)this.FindResource("RotateStoryboard");
            //this.BeginStoryboard(s);
        }

        void mainViewport_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {

                this.Close();
 

        }








        void mainViewport_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            DragMove(); 
        }

        void CloseWindow(object sender, RoutedEventArgs e)
        {
            Close();
        }

    }
}