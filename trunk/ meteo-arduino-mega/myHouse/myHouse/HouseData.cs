using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;

namespace myHouse
{
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
    private float _bedRoomTempOld;
    private float _bedRoomTempNew;
    private float _bojlerTemp;
    private float _bojler2Temp;
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
    private DateTime _lastUpdateMeteo;
    private float _humidity;
    private float _temp085;
    private float _tempDHT;
    private float _pressure;
    private int _windSpeed;
    private int _windSpeedMax;
    private int _windDirection;
    private const int _pulsesPerRevolution = 5;
    private float _solarINTemp;
    private float _solarOUTTemp;
    private float _solarROOMTemp;
    private float _solarPower;
    private float _energyADay;
    private float _energyTotal;
    private float _difON;
    private float _difOFF;

    public HouseData()
    {
    }

    public void resetHouse()
    {
      bedRoomTempOld = corridorTemp = hallTemp = workRoomTemp = livingRoomTemp = _bojlerTemp = _outdoorTemp = bedRoomTempNew = _bojler2Temp = -127;
      bedRoomOldColor = bedRoomNewColor = corridorColor = hallColor = workRoomColor = livingRoomColor = outdoorColor = _bojlerColor = _bojler2Color = Colors.Red.ToString();
    }
    public void resetMeteo()
    {
    }
    public void resetSolar()
    {
    }

    public float corridorTemp { get { return _corridorTemp; } set { _corridorTemp = value; NotifyPropertyChanged("corridorTemp"); } }
    public float hallTemp { get { return _hallTemp; } set { _hallTemp = value; NotifyPropertyChanged("hallTemp"); } }
    public float livingRoomTemp { get { return _livingRoomTemp; } set { _livingRoomTemp = value; NotifyPropertyChanged("livingRoomTemp"); } }
    public float workRoomTemp { get { return _workRoomTemp; } set { _workRoomTemp = value; NotifyPropertyChanged("workRoomTemp"); } }
    public float bedRoomTempOld { get { return _bedRoomTempOld; } set { _bedRoomTempOld = value; NotifyPropertyChanged("bedRoomTempOld"); } }
    public float bedRoomTempNew { get { return _bedRoomTempNew; } set { _bedRoomTempNew = value; NotifyPropertyChanged("bedRoomTempNew"); } }
    public float bojlerTemp { get { return _bojlerTemp; } set { _bojlerTemp = value; NotifyPropertyChanged("bojlerTemp"); } }
    public float bojler2Temp { get { return _bojler2Temp; } set { _bojler2Temp = value; NotifyPropertyChanged("bojler2Temp"); } }
    public float outdoorTemp { get { return _outdoorTemp; } set { _outdoorTemp = value; NotifyPropertyChanged("outdoorTemp"); } }
    public float solarINTemp { get { return _solarINTemp; } set { _solarINTemp = value; NotifyPropertyChanged("solarINTemp"); } }
    public float solarOUTTemp { get { return _solarOUTTemp; } set { _solarOUTTemp = value; NotifyPropertyChanged("solarOUTTemp"); } }
    public float solarROOMTemp { get { return _solarROOMTemp; } set { _solarROOMTemp = value; NotifyPropertyChanged("solarROOMTemp"); } }

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

    public int windSpeed
    {
      get { return (_windSpeed / _pulsesPerRevolution) * 60; }
      set { _windSpeed = value; NotifyPropertyChanged("windSpeed"); }
    }
    public int windSpeedMax
    {
      get { return (_windSpeedMax / _pulsesPerRevolution) * 60; }
      set { _windSpeedMax = value; NotifyPropertyChanged("windSpeedMax"); }
    }
    public int windDirection { get { return _windDirection; } set { _windDirection = value; NotifyPropertyChanged("windDirection"); } }

    public DateTime lastUpdateHouse { get { return _lastUpdateHouse; } set { _lastUpdateHouse = value; NotifyPropertyChanged("lastUpdateHouse"); } }
    public DateTime lastUpdateMeteo { get { return _lastUpdateMeteo; } set { _lastUpdateMeteo = value; NotifyPropertyChanged("lastUpdateMeteo"); } }
    public DateTime lastUpdateSolar { get { return _lastUpdateSolar; } set { _lastUpdateSolar = value; NotifyPropertyChanged("lastUpdateSolar"); } }

    public float solarPower { get { return _solarPower; } set { _solarPower = value; NotifyPropertyChanged("solarPower"); } }
    public float energyADay { get { return _energyADay; } set { _energyADay = value; NotifyPropertyChanged("energyADay"); } }
    public float energyTotal { get { return _energyTotal; } set { _energyTotal = value; NotifyPropertyChanged("energyTotal"); } }
    public float difOFF { get { return _difOFF; } set { _difOFF = value; NotifyPropertyChanged("difOFF"); } }
    public float difON { get { return _difON; } set { _difON = value; NotifyPropertyChanged("difON"); } }

    private byte _statusSolar;
    public string statusSolar
    {
      get { if (_statusSolar == 0) { return "OFF"; } else { return "ON"; } }
      set
      {
        if (value == "0") { _statusSolar = 0; } else { _statusSolar = 1; };
        NotifyPropertyChanged("statusSolar");
      }
    }
    private Brush _statusSolarBackGround;
    public Brush statusSolarBackGround
    {
      get { if (_statusSolar == 0) { return new SolidColorBrush(Colors.Red); } else { return new SolidColorBrush(Colors.Green); } }
      set
      {
        _statusSolarBackGround = value;
        NotifyPropertyChanged("statusSolarBackGround");
      }
    }
    private Brush _statusSolarForeGround;
    public Brush statusSolarForeGround
    {
      get { if (_statusSolar == 0) {return new SolidColorBrush(Colors.White); } else { return new SolidColorBrush(Colors.Black); } }
      set
      {
        _statusSolarForeGround = value;
        NotifyPropertyChanged("statusSolarForeGround");
      }
    }
  }
}
