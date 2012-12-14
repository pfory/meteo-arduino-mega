using System;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Net;
using System.Timers;
using System.Windows;
using System.Windows.Markup;
using System.Windows.Media;
using System.Xml;

namespace SolarSystem
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    private Timer timer = new Timer();
    SolarData sd = new SolarData();
    private int oldDay = DateTime.Now.Day;

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
      sd.loadData();
      sd.odchylkaOdJihu = -29;
      sd.casovePasmo = 1;
      sd.azimutKolektoru = 151;
      showData(sd);
      sd.energieKolektorDen = 0;
      timer.Interval = sd.intervalNacitani;
    }

    private void OnTimedEvent(object source, ElapsedEventArgs e)
    {
      showData(sd);
      sd.calculateVykonNaKolektor();
      if (oldDay != DateTime.Now.Day)
      {
          sd.energieKolektorDen = 0;
          oldDay = DateTime.Now.Day;
      }
    }

    private void showData(SolarData sd)
    {
      sd.statusLEDColor = Colors.Yellow.ToString();
      try
      {
        getData(sd);
        if (sd.rawData != String.Empty)
        {
          parseData();
        }
      }
      catch (Exception ex)
      {
        sd.error = ex.Message;
      }

      sd.statusLEDColor = Colors.Green.ToString();
    }

    private void parseData()
    {
      string[] radka = sd.rawData.Split(new Char[] { '\n' });
      foreach (string s in radka)
      {
        string[] column = s.Split(new Char[] { ',' });
        if (column[0] == sd.solarVstup)
          sd.solarInput = (float)Convert.ToDecimal(column[2].Replace(".", ","));
        if (column[0] == sd.solarVystup)
          sd.solarOutput = (float)Convert.ToDecimal(column[2].Replace(".", ","));
        if (column[0] == sd.bojler)
          sd.bojlerTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
        if (column[0] == sd.venkovni)
          sd.outTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));
        if (column[0] == sd.vnitrni)
          sd.intTemp = (float)Convert.ToDecimal(column[2].Replace(".", ","));

        sd.lastUpdate = getDateTimeFromCosmString(column[1] + column[2]);
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

    private void getData(SolarData sd)
    {
      string url = "http://api.cosm.com/v2/feeds/" + sd.cosmID + ".csv";

      string htmlCode = String.Empty;
      try
      {
        // Create Request
        HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);

        // Create Client
        WebClient client = new WebClient();

        // Assign Credentials
        client.Credentials = new NetworkCredential(sd.userName, sd.password);

        // Grab Data
        sd.rawData = client.DownloadString(url);
      }
      catch (WebException ex)
      {
        throw ex;
      }
    }


    private void Button_Click_2(object sender, RoutedEventArgs e)
    {
      sd.saveData();
    }

    private void Button_Click_3(object sender, RoutedEventArgs e)
    {
      sd.saveData();
      this.Close();
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
    private float _bojlerTemp;
    private float _outTemp;
    private float _intTemp;
    private float _solarPower;
    private string _statusLEDColor;
    private DateTime _lastUpdate;
    private string _rawData;
    private Visibility _pictureClouds;
    private Visibility _pictureSun;

    private string _error;

    private string configFile = "config.xml";

    private string _cosmID;
    private string _userName;
    private string _password;
    private string _solarVstup;
    private string _solarVystup;
    private string _bojler;
    private string _venkovni;
    private string _vnitrni;
    private long _intervalNacitani;


    private float _zemepisnaSirka;
    private float _zemepisnaDelka;
    private float _naklon;
    private float _odchylka;
    private float _solarniKonstanta;
    private float _koeficientPropustnosti;
    private float _plochaKolektoru;
    private float _vykonKolektor;
    private float _vykonKolektorOld;
    private float _vykonKolektorTrend;
    private float _energieKolektorDen;
    private float _odchylkaOdJihu;
    private float _casovePasmo;
    private float _azimutKolektoru;

    public float solarInput { get { return _solarInput; } set { _solarInput = value; solarPower = 0; NotifyPropertyChanged("solarInput"); } }
    public float solarOutput { get { return _solarOutput; } set { _solarOutput = value; solarPower = 0; NotifyPropertyChanged("solarOutput"); } }
    public float bojlerTemp { get { return _bojlerTemp; } set { _bojlerTemp = value; NotifyPropertyChanged("bojlerTemp"); } }
    public float outTemp { get { return _outTemp; } set { _outTemp = value; NotifyPropertyChanged("outTemp"); } }
    public float intTemp { get { return _intTemp; } set { _intTemp = value; NotifyPropertyChanged("intTemp"); } }
    public float odchylkaOdJihu { get { return _odchylkaOdJihu; } set { _odchylkaOdJihu = value; } }
    public float casovePasmo { get { return _casovePasmo; } set { _casovePasmo = value; } }
    public float azimutKolektoru { get { return _azimutKolektoru; } set { _azimutKolektoru = value; } }
    public float azimutKolektoruRad { get { return degreeToRadian(_azimutKolektoru); } }
    public float energieKolektorDen { get { return _energieKolektorDen / 1000f; } set { _energieKolektorDen = calculateEnergieKolektorDen(); NotifyPropertyChanged("energieKolektorDen"); } }


    public float solarPower
    {
      get { return _solarPower; }
      set
      {
        float sp = (5f / 1000f / 60f) * (_solarOutput - _solarInput) * 1054f * 3900f;
        if (_solarOutput < _solarInput)
          sp = 0;
        if (vykonKolektor < 0)
          sp = 0;
        if (sp > 0)
        {
          _solarPower = sp;
        }
        else {
          _solarPower = 0;
        }
        
        NotifyPropertyChanged("solarPower");
        pictureClouds = Visibility.Hidden;
        pictureSun = Visibility.Hidden;
      }
    }

    public string error { get { return _error; } set { _error = value; NotifyPropertyChanged("error"); } }

    public string statusLEDColor { get { return _statusLEDColor; } set { _statusLEDColor = value; NotifyPropertyChanged("statusLEDColor"); } }
    public DateTime lastUpdate { get { return _lastUpdate; } set { _lastUpdate = value; NotifyPropertyChanged("lastUpdate"); } }
    public string rawData { get { return _rawData; } set { _rawData = value; NotifyPropertyChanged("rawData"); } }

    public Visibility pictureClouds
    {
      get { return _pictureClouds; }
      set
      {
        if (solarPower > 0)
          _pictureClouds = Visibility.Hidden;
        else
          _pictureClouds = Visibility.Visible;
        NotifyPropertyChanged("pictureClouds");
      }
    }
    public Visibility pictureSun
    {
      get { return _pictureSun; }
      set
      {
        if (solarPower > 0)
          _pictureSun = Visibility.Visible;
        else
          _pictureSun = Visibility.Hidden;
        NotifyPropertyChanged("pictureSun");
      }
    }

    public string cosmID { get { return _cosmID; } set { _cosmID = value; NotifyPropertyChanged("cosmID"); } }
    public string userName { get { return _userName; } set { _userName = value; NotifyPropertyChanged("userName"); } }
    public string password { get { return _password; } set { _password = value; NotifyPropertyChanged("password"); } }
    public string solarVstup { get { return _solarVstup; } set { _solarVstup = value; NotifyPropertyChanged("solarVstup"); } }
    public string solarVystup { get { return _solarVystup; } set { _solarVystup = value; NotifyPropertyChanged("solarVystup"); } }
    public string bojler { get { return _bojler; } set { _bojler = value; NotifyPropertyChanged("bojler"); } }
    public string venkovni { get { return _venkovni; } set { _venkovni = value; NotifyPropertyChanged("venkovni"); } }
    public string vnitrni { get { return _vnitrni; } set { _vnitrni = value; NotifyPropertyChanged("vnitrni"); } }
    public long   intervalNacitani { get { return _intervalNacitani; } set { _intervalNacitani = value; NotifyPropertyChanged("intervalNacitani"); } }

    //public float  solarInput             { get { return _solarInput; }               set { _solarInput = value; solarPower = 0; NotifyPropertyChanged("solarInput"); } }

    public float zemepisnaSirka { get { return _zemepisnaSirka; } set { _zemepisnaSirka = value; NotifyPropertyChanged("zemepisnaSirka"); } }
    public float zemepisnaSirkaRad { get { return degreeToRadian(_zemepisnaSirka); }}
    public float zemepisnaDelka { get { return _zemepisnaDelka; } set { _zemepisnaDelka = value; NotifyPropertyChanged("zemepisnaDelka"); } }
    public float naklon { get { return _naklon; } set { _naklon = value; NotifyPropertyChanged("naklon"); } }
    public float naklonRad { get { return degreeToRadian(_naklon); }}
    public float odchylka { get { return _odchylka; } set { _odchylka = value; NotifyPropertyChanged("odchylka"); } }
    public float odchylkaRad { get { return degreeToRadian(_odchylka); }}
    public float solarniKonstanta { get { return _solarniKonstanta; } set { _solarniKonstanta = value; NotifyPropertyChanged("solarniKonstanta"); } }
    public float koeficientPropustnosti { get { return _koeficientPropustnosti; } set { _koeficientPropustnosti = value; NotifyPropertyChanged("koeficientPropustnosti"); } }
    public float plochaKolektoru { get { return _plochaKolektoru; } set { _plochaKolektoru = value; NotifyPropertyChanged("plochaKolektoru"); } }

    public float vykonKolektor { get { return _vykonKolektor; } 
    set {
      if (value >= 0)
      {
        _vykonKolektor = value; NotifyPropertyChanged("vykonKolektor");
        _vykonKolektorTrend = _vykonKolektor - _vykonKolektorOld;
        NotifyPropertyChanged("vykonKolektorTrend");

        _vykonKolektorOld = _vykonKolektor;
      }
      else
        _vykonKolektor = 0;
      }
    }

    public float vykonKolektorTrend { get { return _vykonKolektorTrend; } }

    public void loadData()
    {
      if (File.Exists(configFile))
      {
        XmlTextReader reader = new XmlTextReader(configFile);

        while (reader.Read())
        {
          XmlNodeType nType = reader.NodeType;
          if (nType == XmlNodeType.Element)
          {
            if (reader.Name == "COSM_ID") { reader.Read(); cosmID = reader.Value; }
            if (reader.Name == "UserName") { reader.Read(); userName = reader.Value; }
            if (reader.Name == "Password") { reader.Read(); password = reader.Value; }
            if (reader.Name == "solarInput") { reader.Read(); solarVstup = reader.Value; }
            if (reader.Name == "solarOutput") { reader.Read(); solarVystup = reader.Value; }
            if (reader.Name == "intTemperature") { reader.Read(); vnitrni = reader.Value; }
            if (reader.Name == "outTemperature") { reader.Read(); venkovni = reader.Value; }
            if (reader.Name == "bojler") { reader.Read(); bojler = reader.Value; }
            if (reader.Name == "intervalNacitani") { reader.Read(); intervalNacitani = Convert.ToInt32(reader.Value); }

            if (reader.Name == "lat") { reader.Read(); zemepisnaSirka = (float)Convert.ToDouble(reader.Value); }
            if (reader.Name == "lon") { reader.Read(); zemepisnaDelka = (float)Convert.ToDouble(reader.Value); }
            if (reader.Name == "lambda") { reader.Read(); naklon = (float)Convert.ToDouble(reader.Value); }
            if (reader.Name == "azimut") { reader.Read(); odchylka = (float)Convert.ToDouble(reader.Value); }
            if (reader.Name == "solarConstant") { reader.Read(); solarniKonstanta = (float)Convert.ToDouble(reader.Value); }
            if (reader.Name == "throughpuCoef") { reader.Read(); koeficientPropustnosti = (float)Convert.ToDouble(reader.Value); }
            if (reader.Name == "collectorArea") { reader.Read(); plochaKolektoru = (float)Convert.ToDouble(reader.Value); }
          }

        }

        reader.Close();
      }
      else
      {
        cosmID = "75618";
        userName = "jholcik";
        password = "Jenda888";
        solarVstup = "T2890A5F30100001C";
        solarVystup = "T2828C6F3010000E5";
        bojler = "T28D376F301000020";
        venkovni = "T28446CF3010000D5";
        vnitrni = "T289879F3010000C2";
        intervalNacitani = 20000;

        zemepisnaSirka = 0;
        zemepisnaDelka = 0;
        naklon = 0;
        odchylka = 0;
        solarniKonstanta = 0;
        koeficientPropustnosti = 0;
        plochaKolektoru = 0;

        this.saveData();
      }


    }

    private float getDopoctenaHodina()
    {
        return getDopoctenaHodina(DateTime.Now);
    }

    private float getDopoctenaHodina(DateTime dt)
    {
        return (float)dt.Hour + (float)dt.Minute / 60f + (float)dt.Second / 3600f + (zemepisnaDelka / (casovePasmo * 15f) - 1f);
    }

    private float getDeklinace()
    {
        return getDeklinace(DateTime.Now);
    }

    private float getDeklinace(DateTime dt)
    {
        return radianToDegree(23.45f / 180f * (float)Math.PI * (float)Math.Sin((float)Math.PI * (0.98f / 180f * (float)dt.Day + 29.7f / 180f * (float)dt.Month - 109f / 180f)));
    }

    private float getOdchylkaOdJihu()
    {
        return getOdchylkaOdJihu(DateTime.Now);
    }

    private float getOdchylkaOdJihu(DateTime dt) 
    {
        return (getDopoctenaHodina(dt) - 12) * 15;
    }

    private float getVyskaSlunce()
    {
      return getVyskaSlunce(DateTime.Now);
    }

    private float getVyskaSlunce(DateTime dt)
    {
      return radianToDegree((float)Math.Asin ((float)Math.Sin(degreeToRadian(getDeklinace(dt))) * (float)Math.Sin(zemepisnaSirkaRad) + (float)Math.Cos(degreeToRadian(getDeklinace(dt))) * (float)Math.Cos(zemepisnaSirkaRad) * (float)Math.Cos(degreeToRadian(getOdchylkaOdJihu(dt)))));
    }

    private float getVyskaNadObzorem()
    {
        return getVyskaNadObzorem(DateTime.Now);
    }

    private float getVyskaNadObzorem(DateTime dt)
    {
        return radianToDegree((float)Math.Acos((float)Math.Sin(degreeToRadian(getVyskaSlunce(dt))) * Math.Cos(naklonRad) + Math.Cos(degreeToRadian(getVyskaSlunce(dt))) * Math.Sin(naklonRad) * Math.Cos(degreeToRadian(getAzimutSlunce(dt)) - azimutKolektoruRad)));
    }

    private float degreeToRadian(float angle)
    {
        return (float)Math.PI * angle / 180f;
    }

    private float radianToDegree(float angle)
    {
        return angle * (180f / (float)Math.PI);
    }

    private float getAzimutSlunce()
    {
        return getAzimutSlunce(DateTime.Now);
    }

    private float getAzimutSlunce(DateTime dt)
    {
        return (float)(getOdchylkaOdJihu(dt) + 180f);
    }

    private float getTeoretickyMaximalniVykon()
    {
      return getTeoretickyMaximalniVykon(DateTime.Now);
    }

    private float getTeoretickyMaximalniVykon(DateTime dt)
    {
        return solarniKonstanta * (float)Math.Cos(degreeToRadian(getVyskaNadObzorem(dt)));
    }

    public float calculateVykonNaKolektor()
    {
      return calculateVykonNaKolektor(DateTime.Now);
    }

    public float calculateVykonNaKolektor(DateTime dt)
    {
      return getTeoretickyMaximalniVykon(dt) * plochaKolektoru;
    }

    public float calculateEnergieKolektorDen()
    {
      DateTime dt = DateTime.Now;
      return calculateEnergieKolektorDen(dt.Year, dt.Month, dt.Day);
    }


    public float calculateEnergieKolektorDen(int rok, int mesic, int den) //Wh
    {
      float f = 0;
      DateTime dt = new DateTime(rok, mesic, den);
      for (int i = 0; i < 1440; i++)
      {
        if (Math.Cos(degreeToRadian(getVyskaNadObzorem(dt.AddMinutes(i))))>=0)
          f += calculateVykonNaKolektor(dt.AddMinutes(i)) / 60;
      }
      return f;
    }


    internal void saveData()
    {
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
      textWriter.WriteElementString("intervalNacitani", intervalNacitani.ToString());
      textWriter.WriteEndElement();

      textWriter.WriteStartElement("Collectors");
      textWriter.WriteElementString("lat", zemepisnaSirka.ToString());
      textWriter.WriteElementString("lon", zemepisnaDelka.ToString());
      textWriter.WriteElementString("lambda", naklon.ToString());
      textWriter.WriteElementString("azimut", odchylka.ToString());
      textWriter.WriteElementString("solarConstant", solarniKonstanta.ToString());
      textWriter.WriteElementString("throughpuCoef", koeficientPropustnosti.ToString());
      textWriter.WriteElementString("collectorArea", plochaKolektoru.ToString());
      textWriter.WriteEndElement();

      textWriter.WriteEndElement();

      textWriter.WriteEndDocument();

      textWriter.Close();
    }
  }
}
