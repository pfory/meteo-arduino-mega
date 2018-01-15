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
#define BMP085def
#define DHTdef1
#define DHTdef2
//#define UDPdef 
//#define Ethernetdef 
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
 #define DEBUG_PRINTLN(x)       Serial.println (x)
 #define DEBUG_PRINTF(x, y)     Serial.printf (x, y)
 #define PORTSPEED 115200
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINTF(x, y)
#endif 


#ifdef Ethernetdef
#include <Ethernet.h>
// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 0x00, 0xE0, 0x07D, 0xCE, 0xC6, 0x6F};
// initialize the library instance:
EthernetClient client;
//EthernetClient clientConfig;
char server[] = "api.cosm.com";   // name address for cosm API
char serverConfig[] = "datel.asp2.cz"; //config server
//bool checkConfigFlag = false;
char dataString[400];

unsigned long lastSendTime;

#ifdef UDPdef
EthernetUDP Udp;
unsigned int localPort = 8888;      // local port to listen for UDP packets
//IPAddress timeServer(192, 43, 244, 18); // time.nist.gov NTP server
IPAddress timeServer(130,149,17,21); // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
#include <Time.h>

#define DATE_DELIMITER "."
#define TIME_DELIMITER ":"
#define DATE_TIME_DELIMITER " "

#endif
#endif

#ifdef DALLASdef
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
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
//#include <Wire.h>
#include <Adafruit_BMP085.h>
//#include <BMP085.h> //558 bytes +
//#include <swI2C_BMP085.h>
//#include <I2cMaster.h>
//BMP085 dps = BMP085();      // Digital Pressure Sensor 
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


int humidity1 = 0;
int tempDHT1 = 0;
int humidity2 = 0;
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
char versionSW[]="0.98";
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
  Serial.begin(115200);
  Serial.print(versionSWString);
  Serial.println(versionSW);

  loadConfig(); //load values from EEPROM
  
  Serial.println("SW inicialization");
  Serial.print("Free mem: ");
  Serial.print(freeMemory());
  Serial.println(" bytes");

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

  Serial.print("Initializing SD card...");

  bCardOK = true;
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("card failed, or not present");
    bCardOK = false;
    #ifdef LCDdef
    lcd.setCursor(0,1);
    lcd.print("SD card failed");
    delay(1000);
    eraseRow(1);
    #endif
  }
  else {
    Serial.println("card initialized.");
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
  Serial.print("waiting for net connection...");
#ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("waiting for net");
#endif
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed using DHCP");
    // DHCP failed, so use a fixed IP address:
    //Ethernet.begin(mac, ip);
  }
  digitalWrite(53,HIGH);

#ifdef LCDdef
  eraseRow(1);
#endif
  Serial.println("Ethernet OK");
  
#ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("IP:");
  lcd.print(Ethernet.localIP());
  delay(2000);
  eraseRow(1);
  #endif
  Serial.print("\nIP:");
  Serial.println(Ethernet.localIP());
  Serial.print("Mask:");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway:");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS:");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println();
#endif

  if (storage.stamp==0) {
    //load config from web
  }

  
#ifdef UDPdef
  Udp.begin(localPort);
  Serial.print("waiting 20s for time sync...");
  setSyncProvider(getNtpTime);

  lastMeasTime=millis();
  while(timeStatus()==timeNotSet && millis()<lastMeasTime+20000); // wait until the time is set by the sync provider, timeout 20sec
  Serial.println("Time sync interval is set to 3600 second.");
  setSyncInterval(3600); //sync each 1 hour
  
  Serial.print("Now is ");
  printDateTime(0);
  Serial.println(" UTC.");

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
  Serial.print("Filename:");
  Serial.println(fileName);
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
  Serial.println("DHT1 N/A");
#endif

#ifdef DHTdef2
  dhtInit(2);
  //dht2.startMeas();
  dht2.begin();
  #else
  Serial.println("DHT2 N/A");
#endif

  
#ifdef Ethernetdef
  Serial.print("Sending interval [ms]:");
  Serial.println(storage.sendInterval);
  lastSendTime = millis();
#endif
  
#ifdef SDdef
  Serial.print("Saving interval [ms]:");
  Serial.println(storage.saveInterval);
  lastSaveTime = millis();
#endif

  dsLastPrintTime = millis();
  lastMeasTime = millis();
  dsSensors.requestTemperatures(); 
  
  Serial.println("End of SW initialization phase, I am starting measuring.");

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
      DEBUG_PRINT(temperature085);
      DEBUG_PRINTLN(" *C");
      DEBUG_PRINT("Pressure: ");
      DEBUG_PRINT(pressure);
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
      Serial.print("Average pressure ");
      Serial.println(avgPressure/numberOfSamples);
    }
    
    if (millis() - lastPressureTime > storage.BMPPressInterval) {
      lastPressureTime = millis();
    
      avgPressure=avgPressure/numberOfSamples;

      Serial.print("Last average pressure=");
      Serial.println(lastAvgPressure);
      
      if (lastAvgPressure>0) {
        if (avgPressure>lastAvgPressure) {
          pressureChange=PRESSUP;
          Serial.println("Pressure change UP.");
        }
        else if (avgPressure<lastAvgPressure) {
          pressureChange=PRESSDOWN;
          Serial.println("Pressure change DOWN.");
        }
        else {
          pressureChange=PRESSNOCHANGE;
          Serial.println("No pressure change.");
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

    Serial.println();
#ifdef Ethernetdef
    printDateTime(0);
#endif
    Serial.println();
    printTemperatureAll();
  
    Serial.print("Press(Pa):");
    Serial.print(Pressure);

    Serial.print(" Temperature BMP085(C):");
    Serial.print(Temperature);

    #ifdef DHTdef1
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(tempDHT1) || isnan(humidity1)) {
      Serial.println("DHT1 fail.");
    } else {
      Serial.print(" Humidity DHT1(%): "); 
      Serial.print(humidity1);
      Serial.print(" Temp DHT1(C): "); 
      Serial.print(tempDHT1);
    }
    #endif

    #ifdef DHTdef2
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(tempDHT2) || isnan(humidity2)) {
      Serial.println("DHT2 fail.");
    } else {
      Serial.print(" Humidity DHT2(%): "); 
      Serial.print(humidity2);
      Serial.print(" Temp DHT2(C): "); 
      Serial.print(tempDHT2);
    }
    #endif
    
    #ifdef DHTdef1
    Serial.print(" Dew point DHT1: "); 
    Serial.print(calcDewPoint(humidity1, tempDHT1));
    #endif
    
    #ifdef DHTdef2
    Serial.print(" Dew point DHT2: "); 
    Serial.print(calcDewPoint(humidity2, tempDHT2));
    #endif
   
    Serial.println("");
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

  int n; //data length

  #ifdef LCDdef
  lcd.setCursor(15, 1);
  lcd.write(1);
  #endif
  
  //prepare data to send
  char buffer[3];
  //temperature from DALLAS

  sprintf(dataString,"Version,%s\n",versionSW);
  
  #ifdef DALLASdef
  for(byte i=0;i<numberOfDevices; i++) {
    sprintf(dataString,"%sT",dataString);

    for (byte j = 0; j < 8; j++) {
      if (tempDeviceAddresses[i][j] < 16) {
        sprintf(dataString,"%s0",dataString);
      }
      sprintf (dataString, "%s%X", dataString, tempDeviceAddresses[i][j]);
    }

    sprintf(dataString,"%s,",dataString);
    int t = (int)(sensor[i]*10);
    
    if (t<0&&t>-10) {
      sprintf(dataString,"%s-",dataString);
    }
    //sprintf(dataString,"%s%u.%u\n",dataString,t/10,abs(t%10));
    sprintf(dataString,"%s%d.%d\n",dataString,t/10,abs(t%10));
  }
  #endif

  #ifdef BMP085def
  //Pressure
  sprintf(dataString,"%sPress,%ld\n",dataString,Pressure);

  //Temperature
  int temp1 = (Temperature - (int)Temperature) * 100;
  sprintf(dataString,"%sTemp085,%0d.%d\n",dataString,(int)Temperature,temp1);
  #endif

  #ifdef DHTdef1
  //DHT1
  //Humidity and temp
  sprintf(dataString,"%sHumidity1,%u\nTempDHT1,%u\n", dataString,humidity1,tempDHT1);
  //Dew Point
  n=sprintf(dataString,"%sDewPoint1,%u\n", dataString,(int)calcDewPoint(humidity1, tempDHT1));
  #endif

  #ifdef DHTdef2
  //DHT2
  //Humidity and temp
  sprintf(dataString,"%sHumidity2,%u\nTempDHT2,%u\n", dataString,humidity2,tempDHT2);
  //Dew Point
  n=sprintf(dataString,"%sDewPoint2,%u", dataString,(int)calcDewPoint(humidity2, tempDHT2));
  #endif
    
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println();
    printDateTime(0);
    Serial.print(" connecting to COSM [FEEDID=");
    Serial.print(storage.feedId);
    Serial.println("]");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(storage.feedId);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.cosm.com");
    client.print("X-ApiKey: ");
    client.println(storage.apiKey);
    client.print("User-Agent: ");
    client.println(storage.userAgent);
    client.print("Content-Length: ");
    client.println(n);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print(dataString);
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
  #ifdef LCDdef
  lcd.setCursor(15, 1);
  lcd.print(" ");
  #endif
  
  Serial.println("\nDATA:");
  Serial.println(dataString);

  Serial.print("\nDATA sentence length=");
  Serial.println(n);
  
  // note the time that the connection was made or attempted:
}

/*void checkConfig() {
  checkConfigFlag = true;
  if (clientConfig.connect(serverConfig, 80)) {
    Serial.println("Connected to config server.");
    // Make a HTTP request:
    clientConfig.println("GET /getConfigData.aspx HTTP/1.0");
    clientConfig.println();
  } 
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("Connection to config server failed.");
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

  Serial.println();
#ifdef Ethernetdef
  printDateTime(0);
#endif
  Serial.print("\nSaving data to file:");
  Serial.print(fileName);
  Serial.print("...");
  
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
    Serial.println("data saved.");
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(fileName);
    Serial.println("Try SD card reinit.");
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
//    Serial.print("Seconds since Jan 1 1900 = " );
//    Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
//    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    //Serial.println(epoch);                               


    // print the hour, minute and second:
    Serial.print("UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');  
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':'); 
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch %60); // print the second
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
    Serial.print(day());
    Serial.print(DATE_DELIMITER);
    Serial.print(month());
    Serial.print(DATE_DELIMITER);
    Serial.print(year());
    Serial.print(DATE_TIME_DELIMITER);
    printDigits(hour(),toLCD);
    Serial.print(TIME_DELIMITER);
    printDigits(minute(),toLCD);
    Serial.print(TIME_DELIMITER);
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
      Serial.print('0');
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
    Serial.print(digits);
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
      Serial.print("T");
      Serial.print(i, DEC);
      Serial.print("[");
      for (byte j = 0; j < 8; j++) {
        if (tempDeviceAddresses[i][j] < 16) Serial.print("0");
        Serial.print(tempDeviceAddresses[i][j], HEX);
      }
      Serial.print("]");

      Serial.print(sensor[i]);
      Serial.println(" C ");
  }
}



void dsInit(void) {
  dsSensors.begin();
  Serial.println();
  Serial.print("DALLAS Library version:");
  Serial.println(DALLASTEMPLIBVERSION);
  // Grab a count of devices on the wire
  numberOfDevices = dsSensors.getDeviceCount();

  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (dsSensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if(dsSensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      for (byte j=0; j<8;j++) {
        if (tempDeviceAddress[j] < 16) Serial.print("0");
        Serial.print(tempDeviceAddress[j], HEX);
      }
      memcpy(tempDeviceAddresses[i],tempDeviceAddress,8);
      Serial.println();
      
      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);
      
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      dsSensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      
       Serial.print("Resolution actually set to: ");
      Serial.print(dsSensors.getResolution(tempDeviceAddress), DEC); 
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }

  #ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("#DALLAS sens:");
  lcd.print(numberOfDevices, DEC);
  delay(1000);
  eraseRow(1);
  #endif

  
  Serial.print("DALLAS on pin D");
  Serial.print(ONE_WIRE_BUS);
  Serial.println(" OK");
}
#endif

#ifdef BMP085def
void bmp085Init() {
  DEBUG_PRINT("Probe BMP085: ");
  if (bmp.begin()==1) {
    BMP085Present = true;
    DEBUG_PRINTLN("Sensor found.");
    } else {
    DEBUG_PRINTLN("Sensor missing!!!");
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
  Serial.println("\nDHT setup");
  if (sensor==1) {
    dht1.begin();
    Serial.print("DHT1 software on PIN D");
    Serial.print(DHTPIN1);
    Serial.println(" OK");
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
    Serial.print("DHT2 software on PIN D");
    Serial.print(DHTPIN2);
    Serial.println(" OK");
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
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   Serial.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  Serial.println();
  Serial.println();
}


#endif

void startTimer() {
  start=millis();
}

void stopTimer() {
  stop=millis();
}

void printTimer(char* message) {
  Serial.print(message);
  Serial.print(stop-start);
  Serial.println(" ms");
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
