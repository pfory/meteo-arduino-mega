using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Net.Mail;
using System.Net;
namespace myHouse {
  public class HouseData : INotifyPropertyChanged {
    public event PropertyChangedEventHandler PropertyChanged;
    private void NotifyPropertyChanged(string info) {
      if (PropertyChanged != null)
        PropertyChanged(this, new PropertyChangedEventArgs(info));
    }

    private float _corridorTemp;
    private float _hallTemp;
    private float _livingRoomTemp;
    private float _workRoomTemp;
    private float _bedRoomTempOld;
    private float _bedRoomTempNew;
    private float _bojlerTemp;
    private float _bojler2Temp;
    private float _bojler2TempDiff;
    private float _outdoorTemp;
    private string _corridorColor;
    private string _hallColor;
    private string _livingRoomColor;
    private string _workRoomColor;
    private string _bedRoomOldColor;
    private string _bedRoomNewColor;
    private string _bojlerColor;
    private string _bojler2Color;
    private string _outdoorColor;
    private string _statusLEDHouseColor;
    private string _statusLEDMeteoColor;
    private string _statusLEDSolarColor;
    private DateTime _lastUpdateHouse;
    private DateTime _lastUpdateSolar;
    private int _lastUpdateSolarDiff;
    private DateTime _lastUpdateMeteo;
    private float _humidity;
    private float _temp085;
    private float _tempDHT;
    private float _pressure;
    private int _windSpeed;
    private int _windSpeedMax;
    private int _windDirection;
    private const int _pulsesPerRevolution = 5;
    private float _solarINTempDiff;
    private float _solarOUTTempDiff;
    private float _solarROOMTempDiff;
    private float _solarPower;
    private float _energyADay;
    private float _energyTotal;
    private float _difON;
    private float _difOFF;
    private TimeZoneInfo tzi = TimeZoneInfo.Local;
    private TimeSpan _timespan;
    public HouseData() {
      _timespan = tzi.GetUtcOffset(DateTime.Now);
    }

    public void resetHouse() {
      bedRoomTempOld = corridorTemp = hallTemp = workRoomTemp = livingRoomTemp = bojlerTemp = outdoorTemp = bedRoomTempNew = -127;
      bedRoomOldColor = bedRoomNewColor = corridorColor = hallColor = workRoomColor = livingRoomColor = outdoorColor = bojlerColor = Colors.Red.ToString();
    }
    public void resetMeteo() {
    }
    public void resetSolar() {
      //bojler2Temp = -127;
    }

    public float corridorTemp { get { return _corridorTemp; } set { _corridorTemp = value; NotifyPropertyChanged("corridorTemp"); } }
    public float hallTemp { get { return _hallTemp; } set { _hallTemp = value; NotifyPropertyChanged("hallTemp"); } }
    public float livingRoomTemp { get { return _livingRoomTemp; } set { _livingRoomTemp = value; NotifyPropertyChanged("livingRoomTemp"); } }
    public float workRoomTemp { get { return _workRoomTemp; } set { _workRoomTemp = value; NotifyPropertyChanged("workRoomTemp"); } }
    public float bedRoomTempOld { get { return _bedRoomTempOld; } set { _bedRoomTempOld = value; NotifyPropertyChanged("bedRoomTempOld"); } }
    public float bedRoomTempNew { get { return _bedRoomTempNew; } set { _bedRoomTempNew = value; NotifyPropertyChanged("bedRoomTempNew"); } }
    public float bojlerTemp { get { return _bojlerTemp; } set { _bojlerTemp = value; NotifyPropertyChanged("bojlerTemp"); } }
    public float outdoorTemp { get { return _outdoorTemp; } set { _outdoorTemp = value; NotifyPropertyChanged("outdoorTemp"); } }

    private float _solarINTemp = float.MinValue;
    public float solarINTemp {
      get { return _solarINTemp; }
      set {
        solarINTempDiff = value - solarINTemp;
        _solarINTemp = value;
        dON = float.NaN;
        dOFF = float.NaN;
        NotifyPropertyChanged("solarINTemp");
        NotifyPropertyChanged("solarINTempForeGround");
      }
    }
    private Brush _solarINTempForeGround = new SolidColorBrush(Colors.Black);
    public Brush solarINTempForeGround {
      get {
        Brush oldBrush = _solarINTempForeGround;
        if (solarINTempDiff > 0.01) {
          _solarINTempForeGround = new SolidColorBrush(Colors.Red);
        } else if (solarINTempDiff < -0.01) {
          _solarINTempForeGround = new SolidColorBrush(Colors.Blue);
        } else { _solarINTempForeGround = oldBrush; }
        return _solarINTempForeGround;
      }
      set {
        _solarINTempForeGround = value;
      }
    }

    private Brush _solarOUTTempForeGround = new SolidColorBrush(Colors.Black);
    public Brush solarOUTTempForeGround {
      get {
        Brush oldBrush = _solarOUTTempForeGround;
        if (solarOUTTempDiff > 0.01) {
          _solarOUTTempForeGround = new SolidColorBrush(Colors.Red);
        } else if (solarOUTTempDiff < -0.01) {
          _solarOUTTempForeGround = new SolidColorBrush(Colors.Blue);
        } else { _solarOUTTempForeGround = oldBrush; }
        return _solarOUTTempForeGround;
      }
      set {
        _solarOUTTempForeGround = value;
      }
    }

    private float _solarOUTTemp = float.NaN;
    public float solarOUTTemp {
      get { return _solarOUTTemp; }
      set {
        solarOUTTempDiff = value - solarOUTTemp;
        _solarOUTTemp = value;
        dON = float.NaN;
        dOFF = float.NaN;
        NotifyPropertyChanged("solarOUTTemp");
        NotifyPropertyChanged("solarOUTTempForeGround");
      }
    }
    private Brush _solarROOMTempForeGround = new SolidColorBrush(Colors.Black);
    public Brush solarROOMTempForeGround {
      get {
        Brush oldBrush = _solarROOMTempForeGround;
        if (solarROOMTempDiff > 0.01) {
          _solarROOMTempForeGround = new SolidColorBrush(Colors.Red);
        } else if (solarROOMTempDiff < -0.01) {
          _solarROOMTempForeGround = new SolidColorBrush(Colors.Blue);
        } else { _solarROOMTempForeGround = oldBrush; }
        return _solarROOMTempForeGround;
      }
      set {
        _solarROOMTempForeGround = value;
      }
    }

    private float _solarROOMTemp = float.NaN;
    public float solarROOMTemp {
      get { return _solarROOMTemp; }
      set {
        solarROOMTempDiff = value - solarROOMTemp;
        _solarROOMTemp = value;
        NotifyPropertyChanged("solarROOMTemp");
        NotifyPropertyChanged("solarROOMTempForeGround");
      }
    }
    private Brush _bojler2TempForeGround = new SolidColorBrush(Colors.Black);
    public Brush bojler2TempForeGround {
      get {
        Brush oldBrush = _bojler2TempForeGround;
        if (bojler2TempDiff > 0.01) {
          _bojler2TempForeGround = new SolidColorBrush(Colors.Red);
        } else if (bojler2TempDiff < -0.01) {
          _bojler2TempForeGround = new SolidColorBrush(Colors.Blue);
        } else { _bojler2TempForeGround = oldBrush; }
        return _bojler2TempForeGround;
      }
      set {
        _bojler2TempForeGround = value;
      }
    }
    public float bojler2Temp {
      get { return _bojler2Temp; }
      set {
        bojler2TempDiff = value - bojler2Temp;
        _bojler2Temp = value;
        dON = float.NaN;
        dOFF = float.NaN;
        NotifyPropertyChanged("bojler2Temp");
        NotifyPropertyChanged("bojler2TempForeGround");
      }
    }

    private System.Windows.Visibility _tbOnVisible = System.Windows.Visibility.Hidden;
    public System.Windows.Visibility tbOnVisible {
      set { _tbOnVisible = value; }
      get { return _tbOnVisible; }
    }

    private System.Windows.Visibility _tbOffVisible = System.Windows.Visibility.Hidden;
    public System.Windows.Visibility tbOffVisible {
      set { _tbOffVisible = value; }
      get { return _tbOffVisible; }
    }

    private float _dON = 0;
    public float dON {
      get { return _dON; }
      set {
        _dON = difON + bojler2Temp - service.max(solarOUTTemp, solarINTemp);
        NotifyPropertyChanged("dON");
      }
    }

    private float _dOFF = 0;
    public float dOFF {
      get { return _dOFF; }
      set {
        _dOFF = difOFF + bojler2Temp - solarOUTTemp;
        NotifyPropertyChanged("dOFF");
      }
    }

    public float bojler2TempDiff {
      get { return _bojler2TempDiff; }
      set { _bojler2TempDiff = value; }
    }

    public float solarROOMTempDiff {
      get { return _solarROOMTempDiff; }
      set { _solarROOMTempDiff = value; }
    }

    public float solarOUTTempDiff {
      get { return _solarOUTTempDiff; }
      set { _solarOUTTempDiff = value; }
    }

    public float solarINTempDiff {
      get { return _solarINTempDiff; }
      set { _solarINTempDiff = value; }
    }

    public string corridorColor { get { return _corridorColor.ToString(); } set { _corridorColor = value; NotifyPropertyChanged("corridorColor"); } }
    public string hallColor { get { return _hallColor.ToString(); } set { _hallColor = value; NotifyPropertyChanged("hallColor"); } }
    public string livingRoomColor { get { return _livingRoomColor.ToString(); } set { _livingRoomColor = value; NotifyPropertyChanged("livingRoomColor"); } }
    public string workRoomColor { get { return _workRoomColor.ToString(); } set { _workRoomColor = value; NotifyPropertyChanged("workRoomColor"); } }
    public string bedRoomOldColor { get { return _bedRoomOldColor.ToString(); } set { _bedRoomOldColor = value; NotifyPropertyChanged("bedRoomOldColor"); } }
    public string bedRoomNewColor { get { return _bedRoomNewColor.ToString(); } set { _bedRoomNewColor = value; NotifyPropertyChanged("bedRoomNewColor"); } }
    public string bojlerColor { get { return _bojlerColor.ToString(); } set { _bojlerColor = value; NotifyPropertyChanged("bojlerColor"); } }
    public string bojler2Color { get { return _bojler2Color.ToString(); } set { _bojler2Color = value; NotifyPropertyChanged("bojler2Color"); } }
    public string outdoorColor { get { return _outdoorColor.ToString(); } set { _outdoorColor = value; NotifyPropertyChanged("outdoorColor"); } }

    public string statusLEDHouseColor { get { return _statusLEDHouseColor; } set { _statusLEDHouseColor = value; NotifyPropertyChanged("statusLEDHouseColor"); } }
    public string statusLEDSolarColor { get { return _statusLEDSolarColor; } set { _statusLEDSolarColor = value; NotifyPropertyChanged("statusLEDSolarColor"); } }
    public string statusLEDMeteoColor { get { return _statusLEDMeteoColor; } set { _statusLEDMeteoColor = value; NotifyPropertyChanged("statusLEDMeteoColor"); } }

    public float humidity { get { return _humidity; } set { _humidity = value; NotifyPropertyChanged("humidity"); } }
    public float temp085 { get { return _temp085; } set { _temp085 = value; NotifyPropertyChanged("temp085"); } }
    public float tempDHT { get { return _tempDHT; } set { _tempDHT = value; NotifyPropertyChanged("tempDHT"); } }
    public float pressure { get { return _pressure / 100; } set { _pressure = value; NotifyPropertyChanged("pressure"); } }

    public int windSpeed {
      get { return (_windSpeed / _pulsesPerRevolution) * 60; }
      set { _windSpeed = value; NotifyPropertyChanged("windSpeed"); }
    }
    public int windSpeedMax {
      get { return (_windSpeedMax / _pulsesPerRevolution) * 60; }
      set { _windSpeedMax = value; NotifyPropertyChanged("windSpeedMax"); }
    }
    public int windDirection { get { return _windDirection; } set { _windDirection = value; NotifyPropertyChanged("windDirection"); } }

    public DateTime lastUpdateHouse { get { return _lastUpdateHouse; } set { _lastUpdateHouse = value; NotifyPropertyChanged("lastUpdateHouse"); } }
    public DateTime lastUpdateMeteo { get { return _lastUpdateMeteo; } set { _lastUpdateMeteo = value; NotifyPropertyChanged("lastUpdateMeteo"); } }
    public DateTime lastUpdateSolar { get { return _lastUpdateSolar; } set { _lastUpdateSolar = value.Add(_timespan); lastUpdateSolarDiff = 0; NotifyPropertyChanged("lastUpdateSolar"); } }
    public int lastUpdateSolarDiff { get { return _lastUpdateSolarDiff; } 
      set { 
        TimeSpan t = lastUpdateSolar - DateTime.Now;
        _lastUpdateSolarDiff = t.Seconds;
        NotifyPropertyChanged("lastUpdateSolarDiff");
        NotifyPropertyChanged("solarTitle");
      } 
    }
    public String solarTitle
    {
      get
      {
        return lastUpdateSolar.ToString("dddd dd.MM.yyyy HH:mm:ss") + " dif:" + lastUpdateSolarDiff + "s";
      }
    }

    public float solarPower { get { return _solarPower; } set { _solarPower = value; NotifyPropertyChanged("solarPower"); } }
    public float energyADay { get { return _energyADay; } set { _energyADay = value; NotifyPropertyChanged("energyADay"); } }
    public float energyTotal { get { return _energyTotal; } set { _energyTotal = value; NotifyPropertyChanged("energyTotal"); } }
    public float difOFF { get { return _difOFF; } set { _difOFF = value; NotifyPropertyChanged("difOFF"); } }
    public float difON { get { return _difON; } set { _difON = value; NotifyPropertyChanged("difON"); } }

    private byte _statusSolar;
    public string statusSolar {
      get { if (_statusSolar == 0) { return "OFF"; } else { return "ON"; } }
      set {
        if (value != _statusSolar.ToString()) {
          //sendEmail(_statusSolar);
        }
        if (value == "0") { _statusSolar = 0; tbOnVisible = System.Windows.Visibility.Visible; tbOffVisible = System.Windows.Visibility.Hidden; } else { _statusSolar = 1; tbOffVisible = System.Windows.Visibility.Visible; tbOnVisible = System.Windows.Visibility.Hidden; }
        NotifyPropertyChanged("statusSolar");
        NotifyPropertyChanged("statusSolarBackGround");
        NotifyPropertyChanged("statusSolarForeGround");
        NotifyPropertyChanged("tbOnVisible");
        NotifyPropertyChanged("tbOffVisible");
      }
    }

    //public void sendEmail(byte status) {
    //  MailAddress address = new MailAddress("pfory@seznam.cz");
    //  StringBuilder zprava = new StringBuilder();
    //  MailMessage msg = new MailMessage();
    //  String mailServer = String.Empty;
    //  Int16 mailPort = 0;
    //  String mailUserName = String.Empty;
    //  String mailPassword = String.Empty;
    //  bool enableSSL = false;

    //  String mailType = "GMAIL";

    //  if (mailType == "GMAIL")
    //  {
    //    mailServer = "smtp.gmail.com";
    //    mailPort = 587;
    //    mailUserName = "mr.datel@gmail.com";
    //    mailPassword = "hanka123";
    //    enableSSL = true;

    //  }
    //  if (mailType == "SEZNAM")
    //  {
    //    mailServer = "smtp.seznam.cz";
    //    mailPort = 465;
    //    mailUserName = "pfory";
    //    mailPassword = "hanka123";
    //    enableSSL = true;
    //  }
    //  SmtpClient client = new SmtpClient(mailServer, mailPort)
    //  //SmtpClient client = new SmtpClient()
    //  {
    //    Credentials = new NetworkCredential(mailUserName, mailPassword),
    //    EnableSsl = enableSSL,
    //    UseDefaultCredentials = false,
    //    //DeliveryMethod = SmtpDeliveryMethod.PickupDirectoryFromIis,
    //    //PickupDirectoryLocation = "..."
    //  };
    //  msg.From = new MailAddress("pfory@seznam.cz");
    //  if (status == 0) {
    //    msg.Subject = "Solar VYP";
    //  } else {
    //    msg.Subject = "Solar ZAP";
    //  }
    //  zprava.AppendLine("Tesovací mail Solar");
    //  msg.Body = zprava.ToString();
    //  msg.To.Add("pfory@seznam.cz");
    //  msg.IsBodyHtml = true;
    //  msg.Priority = MailPriority.High;
    //  try {
    //    client.Send(msg);
    //  } catch (Exception ex) {
    //    throw ex;
    //  }
    //}

       public void sendEmail(byte status) {
      MailAddress address = new MailAddress("pfory@seznam.cz");
      StringBuilder zprava = new StringBuilder();
      MailMessage msg = new MailMessage();
      SmtpClient client = new SmtpClient();
      //SmtpClient client = new SmtpClient("smtp.bcas.cz", 25) {
        //Credentials = new NetworkCredential("petr.fory@bcas.cz", "eljo3v"),
        //EnableSsl = true
      //};
      msg.From = new MailAddress("pfory@seznam.cz");
      if (status == 0) {
        msg.Subject = "Solar VYP";
      } else {
        msg.Subject = "Solar ZAP";
      }
      msg.Body = zprava.ToString();
      msg.To.Add("pfory@seznam.cz");
      msg.IsBodyHtml = true;
      msg.Priority = MailPriority.High;
      zprava.AppendLine("<h3>Temperatures:</h3>");
      zprava.AppendLine("<b>Bojler:</b>" + bojler2Temp + "°C</br>");
      zprava.AppendLine("<b>IN:</b>" + solarINTemp + "°C</br>");
      zprava.AppendLine("<b>OUT:</b>" + solarOUTTemp + "°C</br>");
      zprava.AppendLine("<b>Room:</b>" + solarROOMTemp + "°C</br>");
      zprava.AppendLine("<h3>Energy:</h3>");
      zprava.AppendLine("<b>Energy a day:</b>" + energyADay + "kWh</br>");
      zprava.AppendLine("<b>Energy total:</b>" + energyTotal + "kWh</br>");
      msg.Body = zprava.ToString();

      try {
        client.Send(msg);
      } catch (Exception ex) {
        throw ex;
      }
    }

    private Brush _statusSolarBackGround;
    public Brush statusSolarBackGround {
      get { if (statusSolar == "OFF") { return new SolidColorBrush(Colors.Red); } else { return new SolidColorBrush(Colors.Green); } }
      set {
        _statusSolarBackGround = value;
      }
    }
    private Brush _statusSolarForeGround;
    public Brush statusSolarForeGround {
      get { if (statusSolar == "ON") { return new SolidColorBrush(Colors.White); } else { return new SolidColorBrush(Colors.White); } }
      set {
        _statusSolarForeGround = value;
      }
    }

  }
}
