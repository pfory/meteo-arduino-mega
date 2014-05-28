using System;
using System.ComponentModel;
using System.Text;
using System.Windows.Media;
using System.Net.Mail;
namespace myHouse {
  /// <summary>
  /// service class
  /// </summary>
  public class HouseData : INotifyPropertyChanged {
    /// <summary>
    /// 
    /// </summary>
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
    private TimeSpan _timeSpan;
    public TimeSpan timeSpan {
      get { return _timeSpan; }
      set { _timeSpan = value; }
    }
    /// <summary>
    /// 
    /// </summary>
    public HouseData() {
      timeSpan = tzi.GetUtcOffset(DateTime.Now);
    }

    /// <summary>
    /// 
    /// </summary>
    public void resetHouse() {
      bedRoomTempOld = corridorTemp = hallTemp = workRoomTemp = livingRoomTemp = bojlerTemp = outdoorTemp = bedRoomTempNew = -127;
      bedRoomOldColor = bedRoomNewColor = corridorColor = hallColor = workRoomColor = livingRoomColor = outdoorColor = bojlerColor = Colors.Red.ToString();
    }
    /// <summary>
    /// 
    /// </summary>
    public void resetMeteo() {
    }
    /// <summary>
    /// 
    /// </summary>
    public void resetSolar() {
      //bojler2Temp = -127;
    }

    /// <summary>
    /// 
    /// </summary>
    public float corridorTemp { get { return _corridorTemp; } set { _corridorTemp = value; NotifyPropertyChanged("corridorTemp"); } }
    /// <summary>
    /// 
    /// </summary>
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
        solarINTempDiff = solarINTemp - value;
        _solarINTemp = value;
        dON = float.NaN;
        dOFF = float.NaN;
        NotifyPropertyChanged("solarINTemp");
        NotifyPropertyChanged("solarINTempHist");
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
        solarOUTTempDiff = solarOUTTemp - value;
        _solarOUTTemp = value;
        dON = float.NaN;
        dOFF = float.NaN;
        NotifyPropertyChanged("solarOUTTemp");
        NotifyPropertyChanged("solarOUTTempHist");
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
        solarROOMTempDiff = solarROOMTemp - value;
        _solarROOMTemp = value;
        NotifyPropertyChanged("solarROOMTemp");
        NotifyPropertyChanged("solarROOMTempHist");
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
        bojler2TempDiff = bojler2Temp - value;
        _bojler2Temp = value;
        dON = float.NaN;
        dOFF = float.NaN;
        NotifyPropertyChanged("bojler2Temp");
        NotifyPropertyChanged("bojler2TempHist");
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

    private float _dDiff = 0;
    public float dDiff {
      get { return _dDiff; }
      set {
        if (statusSolar == "ON") {
          _dDiff = value - dON;
        } else {
          _dDiff = value - dOFF;
        }
        NotifyPropertyChanged("dDiff");
      }
    }

    
    private float _dON = 0;
    public float dON {
      get { return _dON; }
      set {
        if (statusSolar == "ON") {
          dDiff = _dON;
        }
        _dON = difON + bojler2Temp - service.max(solarOUTTemp, solarINTemp);
        NotifyPropertyChanged("dON");
      }
    }

    private float _dOFF = 0;
    public float dOFF {
      get { return _dOFF; }
      set {
        if (statusSolar == "OFF") {
          dDiff = dOFF;
        }
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
    public DateTime lastUpdateSolar { get { return _lastUpdateSolar; } set { _lastUpdateSolar = value.Add(_timeSpan); lastUpdateSolarDiff = 0; NotifyPropertyChanged("lastUpdateSolar"); } }
    public int lastUpdateSolarDiff {
      get { return _lastUpdateSolarDiff; }
      set {
        TimeSpan t = lastUpdateSolar - DateTime.Now;
        _lastUpdateSolarDiff = t.Seconds;
        NotifyPropertyChanged("lastUpdateSolarDiff");
        NotifyPropertyChanged("solarTitle");
      }
    }
    public String solarTitle {
      get {
        return lastUpdateSolar.ToString("dddd dd.MM.yyyy HH:mm:ss") + " dif:" + lastUpdateSolarDiff + "s";
      }
    }

    public float solarPower { get { return _solarPower; } set { _solarPower = value; NotifyPropertyChanged("solarPower"); } }
    public float energyADay { get { return _energyADay; } set { _energyADay = value; NotifyPropertyChanged("energyADay"); } }
    public float energyTotal { get { return _energyTotal; } set { _energyTotal = value; NotifyPropertyChanged("energyTotal"); } }
    public float difOFF { get { return _difOFF; } set { _difOFF = value; NotifyPropertyChanged("difOFF"); } }
    public float difON { get { return _difON; } set { _difON = value; NotifyPropertyChanged("difON"); } }

    private byte _statusSolar = byte.MaxValue;
    public string statusSolar {
      get { if (_statusSolar == 0) { return "OFF"; } else { return "ON"; } }
      set {
        if (_statusSolar != byte.MaxValue && value != _statusSolar.ToString()) {
          sendEmail(Convert.ToByte(value));
        }
        _statusSolar = Convert.ToByte(value);

        if (value == "0") { tbOnVisible = System.Windows.Visibility.Visible; tbOffVisible = System.Windows.Visibility.Hidden; } 
        else { tbOffVisible = System.Windows.Visibility.Visible; tbOnVisible = System.Windows.Visibility.Hidden; }
        
        NotifyPropertyChanged("statusSolar");
        NotifyPropertyChanged("statusSolarBackGround");
        NotifyPropertyChanged("statusSolarForeGround");
        NotifyPropertyChanged("tbOnVisible");
        NotifyPropertyChanged("tbOffVisible");
      }
    }

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
      } else if (status == 1) {
        msg.Subject = "Solar ZAP";
      } else if (status == 2) {
        msg.Subject = "Solar START";
      }
      msg.Body = zprava.ToString();
      msg.To.Add("pfory@seznam.cz");
      msg.IsBodyHtml = true;
      msg.Priority = MailPriority.High;
      zprava.AppendLine("<h3>Temperatures at " + lastUpdateSolar + " :</h3>");
      zprava.AppendFormat("<b>Bojler:</b> {0:F2} &degC</br>", bojler2Temp);
      zprava.AppendLine();
      zprava.AppendFormat("<b>IN:</b> {0:F2} &degC</br>", solarINTemp);
      zprava.AppendLine();
      zprava.AppendFormat("<b>OUT:</b> {0:F2} &degC</br>", solarOUTTemp);
      zprava.AppendLine();
      zprava.AppendFormat("<b>Room:</b> {0:F2} &degC</br>", solarROOMTemp);
      zprava.AppendLine("<h3>Energy:</h3>");
      zprava.AppendFormat("<b>Day:</b> {0:F2} kWh</br>", energyADay);
      zprava.AppendLine();
      zprava.AppendFormat("<b>Total:</b> {0:F2} kWh</br>", energyTotal);
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

    private float _solarINTempHist;
    public float solarINTempHist { get { return _solarINTempHist; } set { _solarINTempHist = value - _solarOUTTemp; } }
    private float _solarOUTTempHist;
    public float solarOUTTempHist { get { return _solarOUTTempHist; } set { _solarOUTTempHist = value - _solarOUTTemp; } }
    private float _solarROOMTempHist;
    public float solarROOMTempHist { get { return _solarROOMTempHist; } set { _solarROOMTempHist = value - _solarROOMTemp; } }
    private float _bojler2TempHist;
    public float bojler2TempHist { get { return _bojler2TempHist; } set { _bojler2TempHist = value - _bojler2Temp; } }
  }
}
