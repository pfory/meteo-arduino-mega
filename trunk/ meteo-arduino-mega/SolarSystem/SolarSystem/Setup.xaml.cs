using System.ComponentModel;
using System.IO;
using System.Windows;
using System.Xml;

namespace SolarSystem
{
    /// <summary>
    /// Interaction logic for Setup.xaml
    /// </summary>
    public partial class Setup : Window
    {
        public SetupData setup;
        public Setup()
        {
            InitializeComponent();
            setup = new SetupData();


            this.DataContext = setup;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            setup.saveData();
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            setup.saveData();
            this.Close();
        }


    }

    public class SetupData : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(info));
        }

        private string configFile = "config.xml";

        private string _cosmID;
        private string _userName;
        private string _password;
        private string _solarVstup;
        private string _solarVystup;
        private string _bojler;
        private string _venkovni;
        private string _vnitrni;

        public SetupData()
        {
            loadData();
        }

        public void loadData()
        {
            //cosmID = "75618";
            //userName = "jholcik";
            //password = "Jenda888";
            //solarVstup = "T2890A5F30100001C";
            //solarVystup = "T2828C6F3010000E5";
            //bojler = "T28D376F301000020";
            //venkovni = "T28446CF3010000D5";
            //vnitrni = "T289879F3010000C2";

            if (File.Exists(configFile))
            {
                XmlTextReader reader = new XmlTextReader(configFile);

                while (reader.Read())
                {
                    XmlNodeType nType = reader.NodeType;
                    if (nType == XmlNodeType.Element)
                    {
                        if (reader.Name == "COSM_ID")
                        {
                            reader.Read();
                            cosmID = reader.Value;
                        }
                        if (reader.Name == "UserName")
                        {
                            reader.Read();
                            userName = reader.Value;
                        }
                        if (reader.Name == "Password")
                        {
                            reader.Read();
                            password = reader.Value;
                        }
                        if (reader.Name == "solarInput")
                        {
                            reader.Read();
                            solarVstup = reader.Value;
                        }
                        if (reader.Name == "solarOutput")
                        {
                            reader.Read();
                            solarVystup = reader.Value;
                        }
                        if (reader.Name == "intTemperature")
                        {
                            reader.Read();
                            vnitrni = reader.Value;
                        }
                        if (reader.Name == "outTemperature")
                        {
                            reader.Read();
                            venkovni = reader.Value;
                        }
                        if (reader.Name == "bojler")
                        {
                            reader.Read();
                            bojler = reader.Value;
                        }
                    }

                }

                reader.Close();
            }
            else {
                cosmID = "75618";
                userName = "jholcik";
                password = "Jenda888";
                solarVstup = "T2890A5F30100001C";
                solarVystup = "T2828C6F3010000E5";
                bojler = "T28D376F301000020";
                venkovni = "T28446CF3010000D5";
                vnitrni = "T289879F3010000C2";

                this.saveData();
            }


        }

        public string cosmID
        {
            get { return _cosmID; }
            set {   
                _cosmID = value;
                NotifyPropertyChanged("cosmID");
            }
        }

        public string userName
        {
            get { return _userName; }
            set {
                _userName = value;
                NotifyPropertyChanged("userName");
            }
        }

        public string password
        {
            get { return _password; }
            set { 
                _password = value;
                NotifyPropertyChanged("password");
            }
        }

        public string solarVstup
        {
            get { return _solarVstup; }
            set { 
                _solarVstup = value;
                NotifyPropertyChanged("solarVstup");
            }
        }

        public string solarVystup
        {
            get { return _solarVystup; }
            set { 
                _solarVystup = value;
                NotifyPropertyChanged("solarVystup");
            }
        }

        public string bojler
        {
            get { return _bojler; }
            set { 
                _bojler = value;
                NotifyPropertyChanged("bojler");
            }
        }

        public string venkovni
        {
            get { return _venkovni; }
            set { 
                _venkovni = value;
                NotifyPropertyChanged("venkovni");
            }
        }

        public string vnitrni
        {
            get { return _vnitrni; }
            set { 
                _vnitrni = value;
                NotifyPropertyChanged("vnitrni");
            }
        }


        internal void saveData()
        {
            //MessageBox.Show("Zatím není implementováno.");
            XmlTextWriter textWriter = new XmlTextWriter(configFile, null);
            textWriter.WriteStartDocument();
            textWriter.WriteComment("Solar config file");

            textWriter.WriteStartElement("Data");
            textWriter.WriteStartElement("COSM");
            textWriter.WriteElementString("COSM_ID", cosmID);
            textWriter.WriteElementString("UserName", userName);
            textWriter.WriteElementString("Password", password);
            textWriter.WriteEndElement();

            textWriter.WriteStartElement("Sensors");
            textWriter.WriteElementString("solarInput", solarVstup);
            textWriter.WriteElementString("solarOutput", solarVystup);
            textWriter.WriteElementString("intTemperature", vnitrni);
            textWriter.WriteElementString("outTemperature", venkovni);
            textWriter.WriteElementString("bojler", bojler);
            textWriter.WriteEndElement();

            textWriter.WriteEndElement();

            textWriter.WriteEndDocument();

            textWriter.Close();
        }
    }
}
