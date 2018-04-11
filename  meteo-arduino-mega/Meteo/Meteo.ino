/*PINOUT

D0 Rx
D1 Tx
D2 DHT sensor 
D3 DALLAS temperature
D4 SS for SD
D5-D9 display
D10 Ethernet shield
D11 display
D12 free
D13 free
A0 free
A1 free 
A2 free
A3 free
A4-SDA for BMP085
A5-SCL for BMP085
*/

// Contains EEPROM.read() and EEPROM.write()
#include <EEPROM.h>
#define DALLASdef
#define DHTdef1
#define DHTdef2
#define UDPdef 
#define Ethernetdef 
#define BMP085def
#define LCDdef
#define SDdef 
#include <TimeLib.h>

#include <limits.h>
#include <MemoryFree.h>
#include "Arduino.h"

#define verbose
#ifdef verbose
 #define DEBUG_PRINT(x)         Serial.print (x)
 #define DEBUG_PRINTDEC(x)      Serial.print (x, DEC)
 #define DEBUG_PRINTHEX(x)      Serial.print (x, HEX)
 #define DEBUG_PRINTLN(x)       Serial.println (x)
 #define DEBUG_PRINTLNDEC(x)    Serial.println (x, DEC)
 #define DEBUG_PRINTF(x, y)     Serial.printf (x, y)
 #define PORTSPEED 115200
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTHEX(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINTLNDEC(x)
 #define DEBUG_PRINTF(x, y)
#endif 


#ifdef Ethernetdef
#include <Ethernet.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 0x00, 0xE0, 0x07D, 0xCE, 0xC6, 0x6F};
// initialize the library instance:
EthernetClient client;
char dataString[400];

unsigned long lastSendTime;

#ifdef UDPdef
EthernetUDP Udp;
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServer(195, 113, 144, 201); // tik.cesnet.cz
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
#include <Time.h>

#define DATE_DELIMITER "."
#define TIME_DELIMITER ":"
#define DATE_TIME_DELIMITER " "

#endif

#define AIO_SERVER      "178.77.238.20"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "datel"
#define AIO_KEY         "hanka12"


Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
Adafruit_MQTT_Publish _temperature1            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature1");
Adafruit_MQTT_Publish _temperature2            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature2");
Adafruit_MQTT_Publish _temperature3            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature3");
Adafruit_MQTT_Publish _temperature4            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature4");
Adafruit_MQTT_Publish _temperature5            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature5");
// Adafruit_MQTT_Publish _temperature6            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature6");
// Adafruit_MQTT_Publish _temperature7            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature7");
// Adafruit_MQTT_Publish _temperature8            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature8");
// Adafruit_MQTT_Publish _temperature9            = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature9");
// Adafruit_MQTT_Publish _temperature10           = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temperature10");
Adafruit_MQTT_Publish _pressure                = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Press");
Adafruit_MQTT_Publish _temperature085          = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Temp085");
Adafruit_MQTT_Publish _temperatureDHT1         = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/TempDHT1");
Adafruit_MQTT_Publish _temperatureDHT2         = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/TempDHT2");
Adafruit_MQTT_Publish _humidity1               = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Humidity1");
Adafruit_MQTT_Publish _humidity2               = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/Humidity2");
Adafruit_MQTT_Publish _dewpoint1               = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/DewPoint1");
Adafruit_MQTT_Publish _dewpoint2               = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/DewPoint2");
Adafruit_MQTT_Publish _versionSW               = Adafruit_MQTT_Publish(&mqtt, "/holcik/Meteo/VersionSW");


#endif

#ifdef DALLASdef
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5
#define TEMPERATURE_PRECISION 12
OneWire onewire(ONE_WIRE_BUS); // pin for onewire DALLAS bus
DallasTemperature dsSensors(&onewire);
DeviceAddress tempDeviceAddress;
#ifndef NUMBER_OF_DEVICES
#define NUMBER_OF_DEVICES 20
#endif
DeviceAddress tempDeviceAddresses[NUMBER_OF_DEVICES];
//int  resolution = 12;
unsigned int numberOfDevices; // Number of temperature devices found
unsigned long lastDisplayTempTime;
unsigned long lastDsMeasStartTime;
bool dsMeasStarted=false;
float sensor[NUMBER_OF_DEVICES];
#ifdef LCDdef
byte currentTempDevice4Display=0;
#endif
#endif


int sensorReading = INT_MIN;

#ifdef BMP085def
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
unsigned long lastDisplayBMPTime;
unsigned long avgPressure=0;
unsigned long lastAvgPressure=0;
unsigned int numberOfSamples=0;
unsigned long lastPressureTime=0;
#define PRESSNOCHANGE 0
#define PRESSUP       1
#define PRESSDOWN     2
byte pressureChange=PRESSNOCHANGE;
#endif
float Temperature = 0;
float Pressure = 0;//, Altitude = 0;
bool                  BMP085Present       = false;


#ifdef DHTdef1
#include "DHT.h"
#define DHTPIN1 2     // what pin we're connected to
// Uncomment whatever type you're using!
#define DHTTYPE1 DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht1(DHTPIN1, DHTTYPE1);
unsigned long lastDHTMeasTime;
unsigned long lastDisplayDHTTime;
//boolean isHumidity=true;
#endif

#ifdef DHTdef2
#include "DHT.h"
#define DHTPIN2 12     // what pin we're connected to
// Uncomment whatever type you're using!
#define DHTTYPE2 DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht2(DHTPIN2, DHTTYPE2);
#endif


float humidity1 = 0;
int tempDHT1 = 0;
float humidity2 = 0;
int tempDHT2 = 0;
byte lastDHTShows=0;

#ifdef LCDdef
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 11, 5, 6, 7);
#define humidityPosR 1 
#define humidityPosC 0 
#define humidityLen 4
#define pressPosR 1
#define pressPosC 4 
#define pressLen 10 
#define tempR 0 
#define tempC 0 
#define tempLen 11
#define hourC 11
#define hourR 0
byte deg[8] = {
  B00100,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte send[8] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B01110,
  B11111,
};
byte save[8] = {
  B11111,
  B01110,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
};
#define DISPLAY_BACKLIGHT 150
#endif

#ifdef SDdef
#include <SD.h>
const int chipSelect = 4;
File myFile;
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
bool bCardOK = false;
unsigned long lastSaveTime;
#endif

unsigned long start, stop;
unsigned int sample=0;

unsigned long lastMeasTime;
unsigned long dsLastPrintTime;
float versionSW=1.2;
char versionSWString[] = "METEO v"; //SW name

// ID of the settings block
#define CONFIG_VERSION "ls3"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

struct StoreStruct {
  // This is for mere detection if they are your settings
  char                version[4];
  // The variables of your settings
  unsigned int        stamp;
  signed long         high_above_sea;
  unsigned int        measDelay;
  unsigned int        displayTempDelay;
  unsigned int        displayBMPDelay;
  unsigned int        displayDHTDelay;
  unsigned long       saveInterval;
  unsigned long       sendInterval;
  unsigned long       BMPPressInterval;
  char                apiKey[50];
  unsigned long       feedId;
  char                userAgent[50];
}
storage = {
  CONFIG_VERSION,
  // The default values
  0,                  //last configuration stamp
  53500,              //high_above_sea in cm
  4000,               //measDelay im ms;
  1000,               //displayTempDelay in ms
  5000,               //displayBMPDelay ms
  4000,               //displayDHTDelay in ms
  10000,              //saveInterval in ms
  20000,              //interval between updates to Cosm.com in ms    
  1800000,            //interval for check press trend in ms
  "HyVsT65CnEPitk6vML664llGUZCSAKx0aXFocmJJVHBUVT0g", //cosm key
  75618,                                              //cosm ID
  "Solar JH"                                          //cosm project name
};

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
#ifdef verbose  
  Serial.begin(PORTSPEED);
#endif
  Serial.println("START");
  DEBUG_PRINT(versionSWString);
  DEBUG_PRINTLN(versionSW);

  loadConfig(); //load values from EEPROM
  
  DEBUG_PRINTLN("SW inicialization");
  DEBUG_PRINT("Free mem: ");
  DEBUG_PRINT(freeMemory());
  DEBUG_PRINTLN(" bytes");

#ifdef LCDdef
  lcd.createChar(3, save);
  lcd.createChar(2, deg);
  lcd.createChar(1, send);
  lcd.begin(16, 2);              // start the library
  lcdPrintVersion();
  lcd.setCursor(0,1);
  lcd.print("Free:");
  lcd.print(freeMemory());
  lcd.print(" bytes");
  analogWrite(12,DISPLAY_BACKLIGHT);
  
  delay(2000);
  
  lcd.clear();
  lcdPrintVersion();
#endif
 
  
#ifdef SDdef
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(53, OUTPUT);
  digitalWrite(53,HIGH);

  DEBUG_PRINT("Initializing SD card...");

  bCardOK = true;
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    DEBUG_PRINTLN("card failed, or not present");
    bCardOK = false;
    #ifdef LCDdef
    lcd.setCursor(0,1);
    lcd.print("SD card failed");
    delay(1000);
    eraseRow(1);
    #endif
  }
  else {
    DEBUG_PRINTLN("card initialized.");
#ifdef LCDdef
    lcd.setCursor(0,1);
    lcd.print("SD card OK");
    delay(1000);
    eraseRow(1);
#endif

    cardInfo();
  }
#endif

#ifdef Ethernetdef
  DEBUG_PRINT("waiting for net connection...");
#ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("waiting for net");
#endif
  if (Ethernet.begin(mac) == 0) {
    DEBUG_PRINTLN("Failed using DHCP");
    // DHCP failed, so use a fixed IP address:
    //Ethernet.begin(mac, ip);
  }
  digitalWrite(53,HIGH);

#ifdef LCDdef
  eraseRow(1);
#endif
  DEBUG_PRINTLN("Ethernet OK");
  
#ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("IP:");
  lcd.print(Ethernet.localIP());
  delay(2000);
  eraseRow(1);
  #endif
  DEBUG_PRINT("\nIP:");
  DEBUG_PRINTLN(Ethernet.localIP());
  DEBUG_PRINT("Mask:");
  DEBUG_PRINTLN(Ethernet.subnetMask());
  DEBUG_PRINT("Gateway:");
  DEBUG_PRINTLN(Ethernet.gatewayIP());
  DEBUG_PRINT("DNS:");
  DEBUG_PRINTLN(Ethernet.dnsServerIP());
  DEBUG_PRINTLN();
#endif

  if (storage.stamp==0) {
    //load config from web
  }

  
#ifdef UDPdef
  Udp.begin(localPort);
  DEBUG_PRINT("waiting 20s for time sync...");
  setSyncProvider(getNtpTime);

  lastMeasTime=millis();
  while(timeStatus()==timeNotSet && millis()<lastMeasTime+20000); // wait until the time is set by the sync provider, timeout 20sec
  DEBUG_PRINTLN("Time sync interval is set to 3600 second.");
  setSyncInterval(3600); //sync each 1 hour
  
  DEBUG_PRINT("Now is ");
  printDateTime(0);
  DEBUG_PRINTLN(" UTC.");

#ifdef LCDdef
  lcd.setCursor(0,1);
  printDateTime(1);
  delay(2000);
  eraseRow(1);
#endif
#endif

#ifdef SDdef
  String fileName = String(year());
  if (month()<10) fileName+="0";
  fileName+=String(month());
  if (day()<10) fileName+="0";
  fileName+=String(day());
  fileName+=".csv";
  DEBUG_PRINT("Filename:");
  DEBUG_PRINTLN(fileName);
#endif
  
 
#ifdef DALLASdef    
  dsInit();
  lastDisplayTempTime = millis();
  dsSensors.setResolution(12);
  dsSensors.setWaitForConversion(false);
#endif
  
#ifdef BMP085def
  bmp085Init();
  lastDisplayBMPTime = millis();
#endif

#ifdef DHTdef1
  dhtInit(1);
  lastDHTMeasTime=millis();
  //dht1.startMeas();
  dht1.begin();
  lastDisplayDHTTime = millis();
#else
  DEBUG_PRINTLN("DHT1 N/A");
#endif

#ifdef DHTdef2
  dhtInit(2);
  //dht2.startMeas();
  dht2.begin();
  #else
  DEBUG_PRINTLN("DHT2 N/A");
#endif

  
#ifdef Ethernetdef
  DEBUG_PRINT("Sending interval [ms]:");
  DEBUG_PRINTLN(storage.sendInterval);
  lastSendTime = millis();
#endif
  
#ifdef SDdef
  DEBUG_PRINT("Saving interval [ms]:");
  DEBUG_PRINTLN(storage.saveInterval);
  lastSaveTime = millis();
#endif

  dsLastPrintTime = millis();
  lastMeasTime = millis();
  dsSensors.requestTemperatures(); 
  
  DEBUG_PRINTLN("End of SW initialization phase, I am starting measuring.");

#ifdef LCDdef
  lcd.clear();
#endif
}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {

  /*while (clientConfig.connected()) {
   if (clientConfig.available()) {
     char c = clientConfig.read();
     Serial.write(c);
    }
  }*/

  
  if (millis() - lastMeasTime > storage.measDelay) {
    sample++;
    lastMeasTime = millis();
#ifdef DALLASdef    
    dsSensors.requestTemperatures(); 
    lastDsMeasStartTime=millis();
    dsMeasStarted=true;
#endif
    
   
#ifdef DHTdef1
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity1 = dht1.readHumidity();
    tempDHT1 = dht1.readTemperature();
#endif

#ifdef DHTdef2
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity2 = dht2.readHumidity();
    tempDHT2 = dht2.readTemperature();
#else
      humidity2 = humidity1;
      tempDHT2 = tempDHT1;
#endif
    
    
#ifdef BMP085def
    if (BMP085Present) {
      DEBUG_PRINT("Temperature BMP085: ");
      Temperature = bmp.readTemperature();
      Pressure = bmp.readSealevelPressure(535.0);
      DEBUG_PRINT(Temperature);
      DEBUG_PRINTLN(" *C");
      DEBUG_PRINT("Pressure: ");
      DEBUG_PRINT(Pressure);
      DEBUG_PRINTLN(" Pa");
    } else {
      Temperature = 77.7;  //dummy
      Pressure = 123456;     //Pa - dummy
    }
#else
    Pressure=101325;
    Temperature=0;
#endif
   
  }
  
#ifdef DALLASdef  
  if (dsMeasStarted) {
    if (millis() - lastDsMeasStartTime>750) {
      dsMeasStarted=false;
      //saving temperatures into variables
      for (byte i=0;i<numberOfDevices; i++) {
        float tempTemp=-126;
        for (byte j=0;j<10;j++) { //try to read temperature ten times
          tempTemp = dsSensors.getTempC(tempDeviceAddresses[i]);
          if (tempTemp>=-55) {
            break;
          }
        }
        sensor[i]=tempTemp;
      } 
    }
  }
  #endif
  
 
#ifdef LCDdef
  lcd.setCursor(hourC, hourR);
#ifdef Ethernetdef
  printDigits(hour(),1);
  if((millis()/1000)%2)
    lcd.print(TIME_DELIMITER);
  else
    lcd.print(" ");
  printDigits(minute(),1);
#endif    
  if (millis() - lastDisplayTempTime > storage.displayTempDelay) {
    lastDisplayTempTime = millis();
    lcd.setCursor(tempC, tempR);
    for (byte i=0; i<tempLen; i++) {
      lcd.print(" ");
    }
    lcd.setCursor(tempC, tempR);
    if (currentTempDevice4Display<10) lcd.print(" ");
    lcd.print(currentTempDevice4Display+1);
    lcd.print(":");
    lcd.print(" ");
    if (abs(sensor[currentTempDevice4Display]<10)) lcd.print(" ");
    lcd.print(sensor[currentTempDevice4Display],1);
    lcd.write(2);
    lcd.print("C");
    
    currentTempDevice4Display++;
    if (currentTempDevice4Display >= numberOfDevices)
      currentTempDevice4Display = 0;
  }

  if (millis() - lastDisplayBMPTime > storage.displayBMPDelay) {
    lastDisplayBMPTime = millis();
    lcd.setCursor(pressPosC, pressPosR);
    for (byte i=0; i<pressLen; i++) {
      lcd.print(" ");
    }
    lcd.setCursor(pressPosC, pressPosR);
    
    
    if (Pressure<100000) lcd.print(" ");

    if (Pressure>0) {
      numberOfSamples++;
      avgPressure+=Pressure;
      DEBUG_PRINT("Average pressure ");
      DEBUG_PRINTLN(avgPressure/numberOfSamples);
    }
    
    if (millis() - lastPressureTime > storage.BMPPressInterval) {
      lastPressureTime = millis();
    
      avgPressure=avgPressure/numberOfSamples;

      DEBUG_PRINT("Last average pressure=");
      DEBUG_PRINTLN(lastAvgPressure);
      
      if (lastAvgPressure>0) {
        if (avgPressure>lastAvgPressure) {
          pressureChange=PRESSUP;
          DEBUG_PRINTLN("Pressure change UP.");
        }
        else if (avgPressure<lastAvgPressure) {
          pressureChange=PRESSDOWN;
          DEBUG_PRINTLN("Pressure change DOWN.");
        }
        else {
          pressureChange=PRESSNOCHANGE;
          DEBUG_PRINTLN("No pressure change.");
        }
      }
      
      lastAvgPressure=avgPressure;
      avgPressure=0;
      numberOfSamples=0;
    }
    
    if (pressureChange==PRESSNOCHANGE) { //no press change
      lcd.print(" ");
    }
    else if (pressureChange==PRESSUP) {  //press increasing
      lcd.write(1);
    }
    else if (pressureChange==PRESSDOWN) { //press decreasing  
      lcd.write(3);
    }
    
    if (Pressure>0) {
      lcd.print(Pressure/100);
      lcd.print("hPa");
    }
  }

  if (millis() - lastDisplayDHTTime > storage.displayDHTDelay) {
    lastDisplayDHTTime = millis();
    lcd.setCursor(humidityPosC, humidityPosR);
    for (byte i=0; i<humidityLen; i++) {
      lcd.print(" ");
    }
    
    byte tempHum;
    lcd.setCursor(humidityPosC, humidityPosR);
    if (lastDHTShows==0) {
      tempHum=humidity1;
      lastDHTShows=1;
    }
    else {
      tempHum=humidity2;
      lastDHTShows=0;
    }
   
    if (tempHum<10) lcd.print(" ");
    lcd.print(tempHum);
    lcd.print("%");
  }
#endif

  if (millis() - dsLastPrintTime > 1000) {

    DEBUG_PRINTLN();
#ifdef Ethernetdef
    printDateTime(0);
#endif
    DEBUG_PRINTLN();
    printTemperatureAll();
  
    DEBUG_PRINT("Press(Pa):");
    DEBUG_PRINT(Pressure);

    DEBUG_PRINT(" Temperature BMP085(C):");
    DEBUG_PRINT(Temperature);

    #ifdef DHTdef1
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(tempDHT1) || isnan(humidity1)) {
      DEBUG_PRINTLN("DHT1 fail.");
    } else {
      DEBUG_PRINT(" Humidity DHT1(%): "); 
      DEBUG_PRINT(humidity1);
      DEBUG_PRINT(" Temp DHT1(C): "); 
      DEBUG_PRINT(tempDHT1);
    }
    #endif

    #ifdef DHTdef2
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(tempDHT2) || isnan(humidity2)) {
      DEBUG_PRINTLN("DHT2 fail.");
    } else {
      DEBUG_PRINT(" Humidity DHT2(%): "); 
      DEBUG_PRINT(humidity2);
      DEBUG_PRINT(" Temp DHT2(C): "); 
      DEBUG_PRINT(tempDHT2);
    }
    #endif
    
    #ifdef DHTdef1
    DEBUG_PRINT(" Dew point DHT1: "); 
    DEBUG_PRINT(calcDewPoint(humidity1, tempDHT1));
    #endif
    
    #ifdef DHTdef2
    DEBUG_PRINT(" Dew point DHT2: "); 
    DEBUG_PRINT(calcDewPoint(humidity2, tempDHT2));
    #endif
   
    DEBUG_PRINTLN("");
    dsLastPrintTime = millis(); 
  }
  
  #ifdef Ethernetdef
  if (sample==2) {
    client.stop();
  }

  //if (sample==5 && checkConfigFlag == false) {
    //checkConfig();
  //}

  /*if (sample==8) {
    clientConfig.stop();
    checkConfigFlag = false;
  }*/

  #endif
  
  #ifdef Ethernetdef
  if(!client.connected() && (millis() - lastSendTime > storage.sendInterval)) {
    lastSendTime = millis();
    sendData();
    sample=0;
  }
  #endif
  
  #ifdef SDdef
  if (millis() - lastSaveTime > storage.saveInterval) {
    lastSaveTime=millis();
    saveDataToSD(false);
  }
  #endif

}

//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
#ifdef Ethernetdef
void sendData() {

  #ifdef LCDdef
  lcd.setCursor(15, 1);
  lcd.write(1);
  #endif
  
  printSystemTime();
  DEBUG_PRINTLN(" I am sending data from Meteo unit to HomeAssistant");
  MQTT_connect();
  if (! _temperature1.publish(sensor[0])) {
    DEBUG_PRINTLN("Temperature1 failed");
  } else {
    DEBUG_PRINTLN("Temperature1 OK!");
  }  
  if (! _temperature2.publish(sensor[1])) {
    DEBUG_PRINTLN("Temperature2 failed");
  } else {
    DEBUG_PRINTLN("Temperature2 OK!");
  }  
  if (! _temperature3.publish(sensor[2])) {
    DEBUG_PRINTLN("Temperature3 failed");
  } else {
    DEBUG_PRINTLN("Temperature3 OK!");
  }  
  if (! _temperature4.publish(sensor[3])) {
    DEBUG_PRINTLN("Temperature4 failed");
  } else {
    DEBUG_PRINTLN("Temperature4 OK!");
  }  
  if (! _temperature5.publish(sensor[4])) {
    DEBUG_PRINTLN("Temperature5 failed");
  } else {
    DEBUG_PRINTLN("Temperature5 OK!");
  }  


  if (! _pressure.publish(Pressure)) {
    DEBUG_PRINTLN("Pressure failed");
  } else {
    DEBUG_PRINTLN("Pressure OK!");
  }  
  if (! _temperature085.publish(Temperature)) {
    DEBUG_PRINTLN("Temperature085 failed");
  } else {
    DEBUG_PRINTLN("Temperature085 OK!");
  }  
  if (! _humidity1.publish(humidity1)) {
    DEBUG_PRINTLN("Humidity1 failed");
  } else {
    DEBUG_PRINTLN("Humidity1 OK!");
  }  
  if (! _humidity2.publish(humidity2)) {
    DEBUG_PRINTLN("Humidity2 failed");
  } else {
    DEBUG_PRINTLN("Humidity2 OK!");
  }  
  if (! _dewpoint1.publish(calcDewPoint(humidity1, tempDHT1))) {
    DEBUG_PRINTLN("DewPoint1 failed");
  } else {
    DEBUG_PRINTLN("DewPoint1 OK!");
  }  
  if (! _dewpoint2.publish(calcDewPoint(humidity2, tempDHT1))) {
    DEBUG_PRINTLN("DewPoint2 failed");
  } else {
    DEBUG_PRINTLN("DewPoint2 OK!");
  }  
  if (! _versionSW.publish(versionSW)) {
    DEBUG_PRINTLN("Version SW failed");
  } else {
    DEBUG_PRINTLN("Version SW OK!");
  }    
  #ifdef LCDdef
  lcd.setCursor(15, 1);
  lcd.print(" ");
  #endif
  
}

void printSystemTime(){
  DEBUG_PRINT(day());
  DEBUG_PRINT(".");
  DEBUG_PRINT(month());
  DEBUG_PRINT(".");
  DEBUG_PRINT(year());
  DEBUG_PRINT(" ");
  DEBUG_PRINT(hour());
  printDigits(minute(), 0);
  printDigits(second(), 0);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  DEBUG_PRINT("Connecting to MQTT... ");

uint8_t retries = 3;
while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
     DEBUG_PRINTLN(mqtt.connectErrorString(ret));
     DEBUG_PRINTLN("Retrying MQTT connection in 5 seconds...");
     mqtt.disconnect();
     delay(5000);  // wait 5 seconds
     retries--;
     if (retries == 0) {
       // basically die and wait for WDT to reset me
       while (1);
     }
}
  DEBUG_PRINTLN("MQTT Connected!");
}

/*void checkConfig() {
  checkConfigFlag = true;
  if (clientConfig.connect(serverConfig, 80)) {
    DEBUG_PRINTLN("Connected to config server.");
    // Make a HTTP request:
    clientConfig.println("GET /getConfigData.aspx HTTP/1.0");
    clientConfig.println();
  } 
  else
  {
    // if you didn't get a connection to the server:
    DEBUG_PRINTLN("Connection to config server failed.");
  }
}
*/


#endif



#ifdef SDdef
void saveDataToSD(bool rep) {
  //save data to SD card
  #ifdef LCDdef
  lcd.setCursor(15, 1);
  lcd.write(3);
  #endif
  String tMonth = "";
  String tDay = "";
  byte temp = month();
  if (temp<10) tMonth = "0";
  tMonth += String(month());
  temp = day();
  if (temp<10) tDay = "0";
  tDay += String(day());
    
  String fileName = String(year());
  if (month()<10) fileName+="0";
  fileName+=String(month());
  if (day()<10) fileName+="0";
  fileName+=String(day());
  fileName+=".csv";

  DEBUG_PRINTLN();
#ifdef Ethernetdef
  printDateTime(0);
#endif
  DEBUG_PRINT("\nSaving data to file:");
  DEBUG_PRINT(fileName);
  DEBUG_PRINT("...");
  
  char cFileName[13];
  fileName.toCharArray(cFileName, 13);    
  File dataFile = SD.open(cFileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
#ifdef Ethernetdef    
    dataFile.print(day());
    dataFile.print(DATE_DELIMITER);
    dataFile.print(month());
    dataFile.print(DATE_DELIMITER);
    dataFile.print(year());
    dataFile.print(DATE_TIME_DELIMITER);
    if(hour() < 10)
      dataFile.print('0');
    dataFile.print(hour());
    dataFile.print(TIME_DELIMITER);
    if(minute() < 10)
      dataFile.print('0');
    dataFile.print(minute());
    dataFile.print(TIME_DELIMITER);
    if(second() < 10)
      dataFile.print('0');
    dataFile.print(second());
#endif
#ifdef DALLASdef
    dataFile.print(";");
    //temperature from DALLAS
    for(byte i=0;i<numberOfDevices; i++) {
      int t = (int)(sensor[i]*10);
      if (t<0&&t>-10) {
        dataFile.print("-");
      }
      dataFile.print(t/10);
      dataFile.print(",");
      dataFile.print(abs(t%10));
      dataFile.print(";");
    }
#endif
    
#ifdef BMP085def
#ifndef DALLASdef
    dataFile.print(";");
#endif
    //Pressure
    dataFile.print(Pressure);
#endif
    
#ifdef DHTdef1
    //DHT1
    //Humidity from DHT
    dataFile.print(";");
    dataFile.print(humidity1);

    //temperature from DHT
    dataFile.print(";");
    dataFile.print(tempDHT1);

    dataFile.print(";");
    int t = (int)(calcDewPoint(humidity1, tempDHT1)*10);
    dataFile.print(t/10);
    dataFile.print(",");
    dataFile.print(abs(t%10));
    #endif
    
    #ifdef DHTdef2
    //DHT2
    //Humidity from DHT
    dataFile.print(";");
    dataFile.print(humidity2);

    //temperature from DHT
    dataFile.print(";");
    dataFile.print(tempDHT2);

    dataFile.print(";");
    t = (int)(calcDewPoint(humidity2, tempDHT2)*10);
    dataFile.print(t/10);
    dataFile.print(",");
    dataFile.print(abs(t%10));
    #endif
    
    dataFile.print("\n");
      
    dataFile.close();
    DEBUG_PRINTLN("data saved.");
  }  
  // if the file isn't open, pop up an error:
  else {
    DEBUG_PRINT("error opening ");
    DEBUG_PRINTLN(fileName);
    DEBUG_PRINTLN("Try SD card reinit.");
    SD.begin(chipSelect);
    if (!rep) {
      saveDataToSD(true);
    }
  } 

  #ifdef LCDdef
  lcd.setCursor(15, 1);
  lcd.print(" ");
  #endif
}
#endif


#ifdef UDPdef
unsigned long getNtpTime(void) {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

    // wait to see if a reply is available
  delay(1000);
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
//    DEBUG_PRINT("Seconds since Jan 1 1900 = " );
//    DEBUG_PRINTLN(secsSince1900);               

    // now convert NTP time into everyday time:
//    DEBUG_PRINT("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    //DEBUG_PRINTLN(epoch);                               


    // print the hour, minute and second:
    DEBUG_PRINT("UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    DEBUG_PRINT((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    DEBUG_PRINT(':');  
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      DEBUG_PRINT('0');
    }
    DEBUG_PRINT((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    DEBUG_PRINT(':'); 
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      DEBUG_PRINT('0');
    }
    DEBUG_PRINTLN(epoch %60); // print the second
    return epoch;
  }
  return 0;
 
}

#ifdef Ethernetdef
// send an NTP request to the time server at the given address 
void sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 		   
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}
#endif

void printDateTime(byte toLCD) {
  if (toLCD==1) {
#ifdef LCDdef
    lcd.print(day());
    lcd.print(DATE_DELIMITER);
    lcd.print(month());
    lcd.print(DATE_DELIMITER);
    lcd.print(year());
    lcd.print(DATE_TIME_DELIMITER);
    printDigits(hour(),toLCD);
    lcd.print(TIME_DELIMITER);
    printDigits(minute(),toLCD);
    //lcd.print(TIME_DELIMITER);
    //printDigits(second(),toLCD);
#endif
  }
  else {
    DEBUG_PRINT(day());
    DEBUG_PRINT(DATE_DELIMITER);
    DEBUG_PRINT(month());
    DEBUG_PRINT(DATE_DELIMITER);
    DEBUG_PRINT(year());
    DEBUG_PRINT(DATE_TIME_DELIMITER);
    printDigits(hour(),toLCD);
    DEBUG_PRINT(TIME_DELIMITER);
    printDigits(minute(),toLCD);
    DEBUG_PRINT(TIME_DELIMITER);
    printDigits(second(),toLCD);
  }
}

void printDigits(int digits, byte toLCD){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10) {
    //#ifdef LCDdef
    if (toLCD==1) {
      lcd.print('0');
    }
    else {
    //#endif
      DEBUG_PRINT('0');
    //#ifdef LCDdef
    }
    //#endif
  }
  //#ifdef LCDdef
  if (toLCD==1) {
    lcd.print(digits);
  }
  else {
  //#endif
    DEBUG_PRINT(digits);
  //#ifdef LCDdef
  }
  //#endif
}

#endif

float calcDewPoint (int humidity, int temperature)  
{  
    float logEx;  
    logEx = 0.66077 + (7.5 * temperature) / (237.3 + temperature)  
            + (log10(humidity) - 2);  
    return (logEx - 0.66077) * 237.3 / (0.66077 + 7.5 - logEx);  
} 

#ifdef LCDdef
void lcdPrintVersion() {
  lcd.setCursor(0,0);
  lcd.print(versionSWString);
  lcd.print(versionSW);
}

void eraseRow(byte r) {
  lcd.setCursor(0, r);
  for (byte i=0; i<16; i++) {
    lcd.print(" ");
  }
  lcd.setCursor(0, r);
}
#endif

#ifdef DALLASdef

void printTemperatureAll() {
  // Loop through each device, print out temperature data
  for(byte i=0;i<numberOfDevices; i++) {
    // Search the wire for address
      DEBUG_PRINT("T");
      DEBUG_PRINTDEC(i);
      DEBUG_PRINT("[");
      for (byte j = 0; j < 8; j++) {
        if (tempDeviceAddresses[i][j] < 16) DEBUG_PRINT("0");
        DEBUG_PRINTHEX(tempDeviceAddresses[i][j]);
      }
      DEBUG_PRINT("]");

      DEBUG_PRINT(sensor[i]);
      DEBUG_PRINTLN(" C ");
  }
}



void dsInit(void) {
  dsSensors.begin();
  DEBUG_PRINTLN();
  DEBUG_PRINT("DALLAS Library version:");
  DEBUG_PRINTLN(DALLASTEMPLIBVERSION);
  // Grab a count of devices on the wire
  numberOfDevices = dsSensors.getDeviceCount();

  // locate devices on the bus
  DEBUG_PRINT("Locating devices...");
  
  DEBUG_PRINT("Found ");
  DEBUG_PRINTDEC(numberOfDevices);
  DEBUG_PRINTLN(" devices.");

  // report parasite power requirements
  DEBUG_PRINT("Parasite power is: "); 
  if (dsSensors.isParasitePowerMode()) {
    DEBUG_PRINTLN("ON");
  } else {
    DEBUG_PRINTLN("OFF");
  }
  
  // Loop through each device, print out address
  for(byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if(dsSensors.getAddress(tempDeviceAddress, i)) {
      DEBUG_PRINT("Found device ");
      DEBUG_PRINTDEC(i);
      DEBUG_PRINT(" with address: ");
      for (byte j=0; j<8;j++) {
        if (tempDeviceAddress[j] < 16) DEBUG_PRINT("0");
        DEBUG_PRINTHEX(tempDeviceAddress[j]);
      }
      memcpy(tempDeviceAddresses[i],tempDeviceAddress,8);
      DEBUG_PRINTLN();
      
      DEBUG_PRINT("Setting resolution to ");
      DEBUG_PRINTLNDEC(TEMPERATURE_PRECISION);
      
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      dsSensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      
      DEBUG_PRINT("Resolution actually set to: ");
      DEBUG_PRINTDEC(dsSensors.getResolution(tempDeviceAddress)); 
      DEBUG_PRINTLN();
    } else {
      DEBUG_PRINT("Found ghost device at ");
      DEBUG_PRINTDEC(i);
      DEBUG_PRINT(" but could not detect address. Check power and cabling");
    }
  }

  #ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("#DALLAS sens:");
  lcd.print(numberOfDevices, DEC);
  delay(1000);
  eraseRow(1);
  #endif

  
  DEBUG_PRINT("DALLAS on pin D");
  DEBUG_PRINT(ONE_WIRE_BUS);
  DEBUG_PRINTLN(" OK");
}
#endif

#ifdef BMP085def
void bmp085Init() {
  DEBUG_PRINT("Probe BMP085: ");
  if (!bmp.begin()) {
    DEBUG_PRINTLN("Sensor missing!!!");
  } else {
    BMP085Present = true;
    DEBUG_PRINTLN("Sensor found.");
    DEBUG_PRINT("Temperature: ");
    DEBUG_PRINTLN(bmp.readTemperature());
  }
  DEBUG_PRINTLN("BMP software on PIN A4,A5 OK");
#ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("High:");
  lcd.print(storage.high_above_sea/100);
  lcd.print(".");
  lcd.print(storage.high_above_sea%100);
  lcd.print(" masl");
  delay(1000);
  eraseRow(1);
#endif
}

// long getRealPressure(long TruePressure, long _param_centimeters) {
  // return TruePressure / pow((1 - (float)_param_centimeters / 4433000), 5.255); // + 101325;
// }
#endif

#if defined DHTdef1 || defined DHTdef2
void dhtInit(byte sensor) {
  DEBUG_PRINTLN("\nDHT setup");
  if (sensor==1) {
    dht1.begin();
    DEBUG_PRINT("DHT1 software on PIN D");
    DEBUG_PRINT(DHTPIN1);
    DEBUG_PRINTLN(" OK");
    #ifdef LCDdef
    lcd.setCursor(0,1);
    lcd.print("DHT1");
    delay(1000);
    eraseRow(1);
    #endif

  }
  else if (sensor==2) {
    #ifdef DHTdef2
    dht2.begin();
    DEBUG_PRINT("DHT2 software on PIN D");
    DEBUG_PRINT(DHTPIN2);
    DEBUG_PRINTLN(" OK");
    #ifdef LCDdef
    lcd.setCursor(0,1);
    lcd.print("DHT2");
    delay(1000);
    eraseRow(1);
    #endif
    #endif
  }
}
#endif

#ifdef SDdef
void cardInfo() {
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    DEBUG_PRINTLN("initialization failed. Things to check:");
    DEBUG_PRINTLN("* is a card is inserted?");
    DEBUG_PRINTLN("* Is your wiring correct?");
    DEBUG_PRINTLN("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   DEBUG_PRINTLN("Wiring is correct and a card is present."); 
  }

  // print the type of card
  DEBUG_PRINT("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      DEBUG_PRINTLN("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      DEBUG_PRINTLN("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      DEBUG_PRINTLN("SDHC");
      break;
    default:
      DEBUG_PRINTLN("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    DEBUG_PRINTLN("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  DEBUG_PRINT("\nVolume type is FAT");
  DEBUG_PRINTLNDEC(volume.fatType());
  DEBUG_PRINTLN();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  DEBUG_PRINT("Volume size (bytes): ");
  DEBUG_PRINTLN(volumesize);
  DEBUG_PRINT("Volume size (Kbytes): ");
  volumesize /= 1024;
  DEBUG_PRINTLN(volumesize);
  DEBUG_PRINT("Volume size (Mbytes): ");
  volumesize /= 1024;
  DEBUG_PRINTLN(volumesize);

  
  DEBUG_PRINTLN("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  DEBUG_PRINTLN();
  DEBUG_PRINTLN();
}


#endif

void startTimer() {
  start=millis();
}

void stopTimer() {
  stop=millis();
}

void printTimer(char* message) {
  DEBUG_PRINT(message);
  DEBUG_PRINT(stop-start);
  DEBUG_PRINTLN(" ms");
}

///////EEPROM functions
void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]) {
    for (unsigned int t=0; t<sizeof(storage); t++) {
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
    }
  }
}

void saveConfig() {
  for (unsigned int t=0; t<sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
}
