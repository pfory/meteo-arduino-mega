/*PINOUT

D0 Rx
D1 Tx
D2 DHT sensor 
D3 DALLAS temperature
D4-D9 display
D10 Ethernet shield
D11
D12
D13
A0 free reserved for rain sensor
A1 free reserved for anemometer (current sensor Holcik)
A2 free reserved for anemometer (voltage sensor Holcik)
A3 free reserved for light sensor
A4-SDA for BMP085 ATMEGA328  D20 for BMP085 ATMEGA2560
A5-SCL for BMP085 ATMEGA328  D21 for BMP085 ATMEGA2560


*/


#define DALLASdef //1702 bytes
#define BMP085def //3220 bytes for standard library
#define DHTdef //1052 bytes
#define UDPdef //672 bytes
#define Ethernetdef //10614 bytes
#define LCDdef
#define Anemodef //1454 bytes
#define SDdef 

#ifdef Anemodef
#include <ooPinChangeInt.h>
#include <ByteBuffer.h>
#include "pushbuttonswitch.h" // How do you subclass?  See the pushbuttonswitch.h file:
#endif

#include <limits.h>
#include <MemoryFree.h>
#include "Arduino.h"

#ifdef Ethernetdef
#include <Ethernet.h>
// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 0x00, 0xE0, 0x07D, 0xCE, 0xC6, 0x6F};
// fill in an available IP address on your network here,
// for manual configuration:
//IPAddress ip(192,168,1,1);
// initialize the library instance:
EthernetClient client;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(216,52,233,121);      // numeric IP for api.cosm.com
char server[] = "api.cosm.com";   // name address for cosm API
#endif

#ifdef Ethernetdef
#ifdef UDPdef
EthernetUDP Udp;
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServer(192, 43, 244, 18); // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
// A UDP instance to let us send and receive packets over UDP
byte SNTP_server_IP[]    = { 192, 43, 244, 18}; // time.nist.gov
//byte SNTP_server_IP[] = { 130,149,17,21};    // ntps1-0.cs.tu-berlin.de
//byte SNTP_server_IP[] = { 192,53,103,108};   // ptbtime1.ptb.de
#include <Time.h>

#define DATE_DELIMITER "."
#define TIME_DELIMITER ":"
#define DATE_TIME_DELIMITER " "

#endif
#endif

#ifdef Ethernetdef
//COSM
#define APIKEY         "HyVsT65CnEPitk6vML664llGUZCSAKx0aXFocmJJVHBUVT0g" // your cosm api key
#define FEEDID         75618 // your feed ID
#define USERAGENT      "Solar" // user agent is the project name
#endif

#ifdef DALLASdef
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 12
OneWire onewire(ONE_WIRE_BUS); // pin for onewire DALLAS bus
DallasTemperature sensors(&onewire);
DeviceAddress tempDeviceAddress;
//int  resolution = 12;
int  delayInMillis = 1000;
int numberOfDevices; // Number of temperature devices found
unsigned long lastDisplayTempTime;
unsigned int displayTempDelay=1000; //in ms
#endif


int sensorReading = INT_MIN;

#ifdef BMP085def
#include <Wire.h>
//#include <BMP085.h> //558 bytes +
#include <swI2C_BMP085.h>
#include <I2cMaster.h>
//BMP085 dps = BMP085();      // Digital Pressure Sensor 
swI2C_BMP085 bmp;
//#define HIGH_ABOVE_SEA 34700 //in m
long high_above_sea = 34700;  //in cm
unsigned long lastDisplayBMPTime;
unsigned int displayBMPDelay=5000; //in ms
#endif
long Temperature = 0, Pressure = 0;//, Altitude = 0;


#ifdef DHTdef
#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastDHTMeasTime = 0;
unsigned long lastDisplayDHTTime;
unsigned int displayDHTDelay = 4000; //in ms
boolean isHumidity=true;
#endif
int humidity = 0;
int tempDHT = 0;

#ifdef LCDdef
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define humidityPosR 1 
#define humidityPosC 0 
#define humidityLen 4
#define pressPosR 1
#define pressPosC 6 
#define pressLen 10 
#define temp1R 0 
#define temp1C 0 
#define temp1Len 7 
#define temp2R 0 
#define temp2C 9 
#define temp2Len 7 
#endif


unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 20*1000; //delay between updates to Cosm.com
long dsLastMeasTime = 0;
byte sample=0;

#ifdef Anemodef
//ByteBuffer printBuffer(80);
//long anemoLastMeasTime = 0;
//pushbuttonswitch anemoswitch=pushbuttonswitch(A2, "anemoswitch");
char windDirection[2];
#define anemoDirectioPin 1
long windDirection20=0;
int anemoCountDirectionSamples=0;

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
#endif

unsigned long dsLastPrintTime = 0;
String versionSW("METEOv0.72"); //SW name & version


//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(115200);
  Serial.println(versionSW);

  Serial.println("SW inicialization");
  
  #ifdef LCDdef
  lcd.begin(16, 2);              // start the library
  lcdPrintVersion();
  #endif
  
  Serial.print("Free mem: ");
  Serial.print(freeMemory());
  Serial.println(" bytes");
  
  #ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("Free:");
  lcd.print(freeMemory());
  lcd.print(" bytes");
  
  delay(2000);
  
  lcd.clear();
  lcdPrintVersion();
  #endif

  /*pinMode(10, OUTPUT);
  pinMode(chipSelect, OUTPUT);
  digitalWrite(10,HIGH);
  digitalWrite(chipSelect,LOW);
  */
  
  #ifdef Ethernetdef
  Serial.print("waiting for net...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed using DHCP");
    // DHCP failed, so use a fixed IP address:
    //Ethernet.begin(mac, ip);
  }
  Serial.print("Ethernet OK");

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:

  #ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("IP:");
  lcd.print(Ethernet.localIP());
  delay(2000);
  #endif
  Serial.print(" IP:");
  Serial.println(Ethernet.localIP());
  #endif
  
  #ifdef UDPdef
  Udp.begin(localPort);
  Serial.print("waiting 20s for time sync...");
  setSyncProvider(getNtpTime);

  dsLastMeasTime=millis();
  while(timeStatus()==timeNotSet && millis()<dsLastMeasTime+20000); // wait until the time is set by the sync provider, timeout 20sec
  Serial.println("Time sync interval is set to 3600 second.");
  setSyncInterval(3600); //sync each 1 hour
  
  Serial.print("Now is ");
  printDateTime(0);
  Serial.println(" UTC.");

  #ifdef LCDdef
  lcd.clear();
  lcdPrintVersion();
  lcd.setCursor(0,1);
  printDateTime(1);
  delay(2000);
  #endif
  #endif

  #ifdef SDdef
  Serial.print("Initializing SD card...");

  bCardOK = true;
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("card failed, or not present");
    // don't do anything more:
    bCardOK = false;
  }
  else {
    Serial.println("card initialized.");
    cardInfo();
  }

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
  #endif
  
  #ifdef BMP085def
  bmp085Init();
  lastDisplayBMPTime = millis();
  #endif

  #ifdef DHTdef
  dhtInit();
  lastDHTMeasTime=millis();
  dht.startMeas();
  lastDisplayDHTTime = millis();
  #endif

  lcd.clear();

  Serial.println("End of SW initialization phase, I am starting measuring.");
 
}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {
  #ifdef Anemodef
  int val = analogRead(anemoDirectioPin);    // read the input pin
  //lcd.setCursor(0,1);
  //lcd.print(getWindDirection(val));
  getWindDirectionStr(val);
  //lcd.print("[");
  //lcd.print(windDirection);
  //lcd.print("]");
  windDirection20+=val;
  anemoCountDirectionSamples++;
  
/*  if (millis() - anemoLastMeasTime > 1000) {
    int val = analogRead(1);    // read the input pin
    Serial.print("Analog value ");
    Serial.println(val);
    lcd.setCursor(0,1);
    lcd.print(val);
    anemoLastMeasTime=millis();
    Serial.print("Count per sec ");
    uint8_t count;
    count=anemoswitch.getCount();
    Serial.println(count, DEC);
    lcd.setCursor(0,1);
    lcd.print(count);
    anemoswitch.reset();
  }*/
  #endif
  

  #ifdef DALLASdef    
  if (millis() - dsLastMeasTime > delayInMillis) {
    float temperature1 = sensors.getTempCByIndex(0);
    float temperature2 = sensors.getTempCByIndex(2);

    #ifdef LCDdef
    if (millis() - lastDisplayTempTime > displayTempDelay) {
      lastDisplayTempTime = millis();
      lcd.setCursor(temp1C, temp1R);
      for (byte i=0; i<temp1Len; i++) {
        lcd.print(" ");
      }
      lcd.setCursor(temp1C, temp1R);
      lcd.print(temperature1,1);
      lcd.print("C");
      
      lcd.setCursor(temp2C, temp2R);
      for (byte i=0; i<temp1Len; i++) {
        lcd.print(" ");
      }
      lcd.setCursor(temp2C, temp2R);
      lcd.print(temperature2,1);
      lcd.print("C");
    }
    #endif
 
    sensors.requestTemperatures(); 
    delayInMillis = 750 / (1 << (12 - TEMPERATURE_PRECISION));
    dsLastMeasTime = millis(); 
  }
  #endif
  
  
  #ifdef BMP085def
  Pressure = bmp.readPressure();
  Pressure = getRealPressure(Pressure, high_above_sea);
  #else
  Pressure=101325;
  #endif

  #ifdef LCDdef
  if (millis() - lastDisplayBMPTime > displayBMPDelay) {
    lastDisplayBMPTime = millis();
    lcd.setCursor(pressPosC, pressPosR);
    for (byte i=0; i<pressLen; i++) {
      lcd.print(" ");
    }
    lcd.setCursor(pressPosC, pressPosR);
    lcd.print((int)(Pressure/100));
    lcd.print(".");
    if (Pressure%100<10) lcd.print("0");
    lcd.print((int)(Pressure%100));
    lcd.print("hPa");
  }
  #endif

  #ifdef DHTdef
  
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  if (millis() - lastDHTMeasTime > 250) {
    if (isHumidity) {
      humidity = dht.readHumidity();
      isHumidity = false;
    }
    else {
      tempDHT = dht.readTemperature();
      isHumidity = true;;
    }
      dht.startMeas();
    lastDHTMeasTime=millis();
  }
  #endif

  #ifdef LCDdef
  if (millis() - lastDisplayDHTTime > displayDHTDelay) {
    lastDisplayDHTTime = millis();
    lcd.setCursor(humidityPosC, humidityPosR);
    for (byte i=0; i<humidityLen; i++) {
      lcd.print(" ");
    }
    lcd.setCursor(humidityPosC, humidityPosR);
    lcd.print(humidity);
    lcd.print("%");
    /*lcd.setCursor(0,0);
    lcd.print(tempDHT);
    lcd.print("C");*/
  }
  #endif

  if (millis() - dsLastPrintTime > 1000) {

    Serial.println();
    printDateTime(0);
    Serial.println();
    printTemperatureAll();
    sample++;

  
    //Serial.print(" Alt(cm):");
    //Serial.print(Altitude);
    Serial.print("Press(Pa):");
    Serial.print(Pressure);

    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(tempDHT) || isnan(humidity)) {
      Serial.println("DHT fail.");
    } else {
      Serial.print(" Humidity DHT(%): "); 
      Serial.print(humidity);
      Serial.print(" Temp DHT(C): "); 
      Serial.print(tempDHT);
    }
    
    Serial.print(" Dew point: "); 
    Serial.print(calcDewPoint(humidity, tempDHT));
 
    Serial.println("");
    
    
    dsLastPrintTime = millis(); 
  }
  
  if (sample==5) {
    #ifdef Ethernetdef
    client.stop();
    #endif
  }

  if(
  #ifdef Ethernetdef
  !client.connected() &&
  #endif
  (millis() - lastConnectionTime > postingInterval)) {

    lastConnectionTime = millis();

    String dataString = "";
    char buffer[16];
    //temperature from DALLAS
    for(byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
      if(sensors.getAddress(tempDeviceAddress, i)) {
        dataString += "T";
        for (byte i = 0; i < 8; i++) {
          if (tempDeviceAddress[i] < 16) dataString += "0";
          sprintf (buffer, "%X", tempDeviceAddress[i]);
          dataString += buffer;
        }
        dataString += ",";
        int t = (int)(sensors.getTempCByIndex(i)*10);
        dataString += t/10;
        dataString += ".";
        dataString += abs(t%10);

      }
      dataString += "\n";
    }
    //Pressure
    dataString += "Press,";
    dataString += Pressure;

    //Humidity
    dataString += "\nHumidity,";
    dataString += humidity;

    //temperature from DHT11
    dataString += "\nTempDHT,";
    dataString += tempDHT;

    dataString += "\nDewPoint,";
    dataString += (int)calcDewPoint(humidity, tempDHT);

    dataString += "\nWindDirection,";
    dataString += (int)windDirection20/anemoCountDirectionSamples;

    windDirection20=0;
    anemoCountDirectionSamples=0;
  
    #ifdef Ethernetdef
    sendData(dataString);
    Serial.println("DATA:");
    Serial.println(dataString);
    Serial.println();
    #endif

    #ifdef SDdef
    //save data to SD card
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
    Serial.print("\nSaving data to file:");
    Serial.print(fileName);
    Serial.print("...");
    
    char cFileName[13];
    fileName.toCharArray(cFileName, 13);    
    File dataFile = SD.open(cFileName, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
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

      dataFile.print(";");
      //temperature from DALLAS
      for(byte i=0;i<numberOfDevices; i++) {
        int t = (int)(sensors.getTempCByIndex(i)*10);
        dataFile.print(t/10);
        dataFile.print(",");
        dataFile.print(abs(t%10));
        dataFile.print(";");
      }
      
      //Pressure
      dataFile.print(Pressure);

      //Humidity
      dataFile.print(";");
      dataFile.print(humidity);

      //temperature from DHT11
      dataFile.print(";");
      dataFile.print(tempDHT);

      dataFile.print(";");
      //dataFile.print(calcDewPoint(humidity, tempDHT));
      int t = (int)(calcDewPoint(humidity, tempDHT)*10);
      dataFile.print(t/10);
      dataFile.print(",");
      dataFile.print(abs(t%10));

      
      dataFile.print(";");
      dataFile.print(windDirection20/anemoCountDirectionSamples);
      dataFile.print("\n");
      
      dataFile.close();
      Serial.println("data saved.");
    }  
    // if the file isn't open, pop up an error:
    else {
      Serial.print("error opening ");
      Serial.println(fileName);
    } 
    #endif

    sample=0;
  }
}

//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------


#ifdef Ethernetdef
void sendData(String thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println();
    printDateTime(0);
    Serial.print(" connecting to COSM [FEEDID=");
    Serial.print(FEEDID);
    Serial.println("]");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.cosm.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print(thisData);
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
   // note the time that the connection was made or attempted:
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

void printDateTime(byte toLCD) {
  if (toLCD==1) {
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
  if(digits < 10)
    if (toLCD==1)
      lcd.print('0');
    else
      Serial.print('0');
      
  if (toLCD==1)
    lcd.print(digits);
  else
    Serial.print(digits);
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
  lcd.print(versionSW);
}
#endif

int getWindDirection(int analogValue) {
  return analogValue / 2.844;
}

//calculate wind direction
void getWindDirectionStr(uint16_t adcValue)
{
  if (adcValue>0 && adcValue<64)
    strcpy(windDirection,"V");
  if (adcValue>=64 && adcValue<192)
    strcpy(windDirection,"SV");
  if (adcValue>=192 && adcValue<320)
    strcpy(windDirection,"S");
  if (adcValue>=320 && adcValue<448)
    strcpy(windDirection,"SZ");
  if (adcValue>=448 && adcValue<576)
    strcpy(windDirection,"Z");
  if (adcValue>=576 && adcValue<704)
    strcpy(windDirection,"JZ");
  if (adcValue>=704 && adcValue<832)
    strcpy(windDirection,"J");
  if (adcValue>=832 && adcValue<960)
    strcpy(windDirection,"JV");
  if (adcValue>=960)
    strcpy(windDirection,"V");
  return;
}

#ifdef DALLASdef

void dsInit(void) {
  sensors.begin();
  Serial.println();
  Serial.print("DALLAS Library version:");
  Serial.println(DALLASTEMPLIBVERSION);
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      for (byte i = 0; i < 8; i++) {
        if (tempDeviceAddress[i] < 16) Serial.print("0");
        Serial.print(tempDeviceAddress[i], HEX);
      }
      Serial.println();
      
      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);
      
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      
       Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

void printTemperatureAll() {
  // Loop through each device, print out temperature data
  for(byte i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("T");
      Serial.print(i, DEC);
      Serial.print("[");
      for (byte i = 0; i < 8; i++) {
        if (tempDeviceAddress[i] < 16) Serial.print("0");
        Serial.print(tempDeviceAddress[i], HEX);
      }
      Serial.print("]");

      // It responds almost immediately. Let's print out the data
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print(tempC);
      Serial.println(" C ");
    } 
	//else ghost device! Check your power requirements and cabling
  }
}


#endif

#ifdef BMP085def
void bmp085Init() {
  Serial.println("\nBMP085 setup");
  Serial.print("High above sea level:");
  Serial.println(high_above_sea);
  bmp.begin();
  Serial.println("BMP software on PIN A4,A5 OK");
  #ifdef LCDdef
  lcd.setCursor(0,1);
  lcd.print("High:");
  lcd.print(high_above_sea);
  delay(1000);
  #endif
}
#else
Serial.println("N/A");
#endif

long getRealPressure(long TruePressure, long _param_centimeters) {
  return TruePressure / pow((1 - (float)_param_centimeters / 4433000), 5.255); // + 101325;
}

#ifdef DHTdef
void dhtInit() {
  dht.begin();
  Serial.println("DHT OK");
  #else
  Serial.println("DHT N/A");
  #endif
}


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

