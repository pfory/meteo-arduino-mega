/*PINOUT

D0 Rx
D1 Tx
D2 free
D3 Anemometer - Wind spped
D4 free
D5-D9 free
D10 Ethernet shield
D11 free
D12
D13
A0 DALLAS temperature
A1 DHT humidity
A2 free
A3 Anemometer - Wind direction
A4 SDA for Pressure BMP085
A5 SCL fpr Pressure BMP085
*/

#define debug
#define Ethernetdef
#define DALLASdef 
#define Anemodef
#define BMP085def
//#define SWI2C
#define DHTdef //1022

#ifdef Anemodef
const byte counterPin = 3; 
const byte counterInterrupt = 1; // = pin D3
volatile unsigned int pulseCount=0;
unsigned int pulseCountAll=0;
unsigned int pulseCountMax=0;
unsigned int windDirectionAll=0;
unsigned long time=0;
byte numberOfWindSamples=0;
#endif

#include <limits.h>
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
bool checkConfigFlag = false;

unsigned long lastSendTime;

//COSM
#define APIKEY         "q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g" // your cosm api key
#define FEEDID         63310 // your feed ID
#define USERAGENT      "Meteo Arduino" // user agent is the project name
#endif

#ifdef DALLASdef
#include <OneWire.h>
#define dallasMinimal //-956 Bytes
#ifdef dallasMinimal
#include <DallasTemperatureMinimal.h>
#else
#include <DallasTemperature.h>
#endif
#define ONE_WIRE_BUS A0
OneWire onewire(ONE_WIRE_BUS); // pin for onewire DALLAS bus
#ifdef dallasMinimal
DallasTemperatureMinimal dsSensors(&onewire);
#else
DallasTemperature dsSensors(&onewire);
#endif
DeviceAddress tempDeviceAddress;
#ifndef NUMBER_OF_DEVICES
#define NUMBER_OF_DEVICES 20
#endif
DeviceAddress tempDeviceAddresses[NUMBER_OF_DEVICES];
//int  resolution = 12;
unsigned int numberOfDevices; // Number of temperature devices found
unsigned long lastDisplayTempTime;
unsigned long lastDsMeasStartTime;
unsigned int const dsMeassureDelay=750; //delay between start meassurement and read valid data in ms
bool dsMeasStarted=false;
float sensor[NUMBER_OF_DEVICES];

int sensorReading = INT_MIN;
#endif
unsigned int const dsMeassureInterval=4000; //inteval between meassurements
unsigned long lastMeasTime;
unsigned long dsLastPrintTime;
unsigned int sample=0;
unsigned int const dsPrintTimeDelay=4000; //interval to show results
unsigned int const sendTimeDelay=20000; //to send to cosm.com


#ifdef BMP085def
#include <Wire.h>
#ifdef SWI2C
#include <swI2C_BMP085.h>
#include <I2cMaster.h>
swI2C_BMP085 bmp;
#else
#include <BMP085.h> //558 bytes +
BMP085 bmp = BMP085();      // Digital Pressure Sensor 
#endif
unsigned long lastDisplayBMPTime;
//unsigned long avgPressure=0;
//unsigned long lastAvgPressure=0;
//unsigned int numberOfSamples=0;
unsigned long lastPressureTime=0;
#define PRESSNOCHANGE 0
#define PRESSUP       1
#define PRESSDOWN     2
//byte pressureChange=PRESSNOCHANGE;
signed long         high_above_sea=36900;
signed long Temperature = 0;
long Pressure = 0;//, Altitude = 0;

#endif

#ifdef DHTdef
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11 
#define DHTPIN A1    // what pin we're connected to

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastDHTMeasTime;
unsigned long lastDisplayDHTTime;
#endif
int humidity = 0;
int tempDHT = 0;

#ifdef Anemodef 
const byte counterPin = 3; 
const byte counterInterrupt = 1; // = pin D3
volatile unsigned int pulseCount=0;
unsigned int pulseCountAll=0;
unsigned int pulseCountMax=0;
unsigned int windDirectionAll=0;
unsigned long time=0;
byte numberOfWindSamples=0;
#define windDirPin A3
#endif 

byte counterOverflow=0;
unsigned int old_value=0;
byte counter=0;
String dataString1 = "";
String dataString2 = "";

char versionSW[]="0.75";
char versionSWString[] = "METEO Simple v"; //SW name & version

//byte ledPin=9;

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(115200);
//  Serial.println();
//  Serial.println(versionSW);
  
  #ifdef debug
  printDebugInfo();
  #endif
  Serial.println(F("SW inicialization"));

  #ifdef Ethernetdef
//  Serial.print("waiting for net connection...");
  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed using DHCP");
    // DHCP failed, so use a fixed IP address:
  }

//  Serial.println("Ethernet OK");
  
//  Serial.print("\nIP:");
//  Serial.println(Ethernet.localIP());
  /*Serial.print("Mask:");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway:");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS:");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println();
  */
  #endif
  
  #ifdef DALLASdef
  dsInit();
  lastDisplayTempTime = millis();
  lastSendTime = dsLastPrintTime = lastMeasTime = millis();
  dsSensors.requestTemperatures(); 
  #endif
  
  #ifdef Anemodef 
  pinMode(counterPin, INPUT);      
  digitalWrite(counterPin, HIGH);
  attachInterrupt(counterInterrupt, counterISR, RISING);
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
  #else
//  Serial.println("DHT N/A");
  #endif

  #ifdef Anemodef
  pinMode(counterPin, INPUT);      
  digitalWrite(counterPin, HIGH);
  attachInterrupt(counterInterrupt, counterISR, RISING);
  #endif

  //pinMode(ledPin, OUTPUT);
  //Serial.println("End of SW initialization phase, I am starting measuring.");

}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {

  //start sampling
  if (millis() - lastMeasTime > dsMeassureInterval) {
    sample++;
    lastMeasTime = millis();
    //startTimer();
    #ifdef DALLASdef
    dsSensors.requestTemperatures(); 
    lastDsMeasStartTime=millis();
    dsMeasStarted=true;
    #endif
    
    #ifdef DHTdef
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    tempDHT = dht.readTemperature();
    #endif

    
    #ifdef BMP085def
    //unsigned long oldPress=Pressure;
    #ifdef SWI2C
    Pressure = bmp.readPressure();
    Pressure = getRealPressure(Pressure, high_above_sea);
    #else
    bmp.getPressure(&Pressure);
    bmp.getTemperature(&Temperature); 
    #endif
    #endif
  }

  #ifdef Anemodef
  if (millis() - time > 1000) {
    numberOfWindSamples++;
    time = millis();
    int val=analogRead(windDirPin);
    windDirectionAll+=calculateWindDirectionDegrees(val);
    pulseCountAll+=pulseCount;
    pulseCountMax = max(pulseCount,pulseCountMax);
    pulseCount=0;
  }
  #endif
  
  #ifdef DALLASdef
  if (dsMeasStarted) {
    if (millis() - lastDsMeasStartTime>dsMeassureDelay) {
      dsMeasStarted=false;
      //saving temperatures into variables
      for (byte i=0;i<numberOfDevices; i++) {
        float tempTemp=-126;
        for (byte j=0;j<10;j++) { //try to read temperature ten times
          //tempTemp = dsSensors.getTempCByIndex(i);
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
  
    
  if (millis() - dsLastPrintTime > dsPrintTimeDelay) {

    /*Serial.println();
    #ifdef DALLASdef
    printTemperatureAll();
    #endif

    #ifdef BMP085def
    Serial.print("Press(Pa):");
    Serial.println(Pressure);
    Serial.print("Temp:");
    Serial.print(Temperature/10);
    Serial.print(".");
    Serial.println(abs(Temperature%10));
    #endif
    #ifdef DHTdef
    Serial.print("Humidity:");
    Serial.println(humidity);
    Serial.print("Temp:");
    Serial.println(tempDHT);
    #endif
    
    Serial.println("");
    */
    dsLastPrintTime = millis(); 
  }
  
  #ifdef Ethernetdef
  if (sample==2) {
    client.stop();
  }

  if (sample==5 && checkConfigFlag == false) {
    //checkConfig();
  }

  if (sample==8) {
    checkConfigFlag = false;
  }

  if(!client.connected() && (millis() - lastSendTime > sendTimeDelay)) {
    lastSendTime = millis();
    //digitalWrite(ledPin, HIGH);
    sendData();
    //digitalWrite(ledPin, LOW);
    sample=0;
  }
  #endif
}

#ifdef Ethernetdef
//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
void sendData() {

  //Serial.println("sending data");
  dataString1="";

  //prepare data to send
  char buffer[3];
  //temperature from DALLAS
  //00 01 02 03 04 05 06 07
  //-----------------------
  //28 C9 B8 41 04 00 00 97

  dataString1 += "V,";
  dataString1 += versionSW;
  dataString1 += "\n";

  #ifdef DALLASdef
  for(byte i=0;i<numberOfDevices; i++) {
    dataString1 += "T";

    for (byte j=0; j<8; j++) {
      sprintf (buffer, "%X", tempDeviceAddresses[i][j]);
      if (tempDeviceAddresses[i][j]<16) {
        dataString1 += "0";
        dataString1 += buffer[0];
      }
      else {
        dataString1 += buffer[0];
        dataString1 += buffer[1];
      }
    }

    dataString1 += ",";
    int t = (int)(sensor[i]*10);

    if (t<0&&t>-10) {
      dataString1 += "-";
    }
    dataString1 += t/10;
    dataString1 += ".";
    dataString1 += abs(t%10);
    dataString1 += "\n";
  }
  #endif

  #ifdef BMP085def
  //Pressure
  dataString1 += "Press,";
  dataString1 += Pressure;
  dataString1 += "\n";
  //Temperature
  dataString1 += "Temp085,";
  dataString1 += Temperature/10;
  dataString1 += ".";
  dataString1 += abs(Temperature%10);
  dataString1 += "\n";
  #endif

  #ifdef DHTdef
  dataString1 += "Humidity,";
  dataString1 += humidity;
  dataString1 += "\n";
  dataString1 += "TempDHT,";
  dataString1 += tempDHT;
  #endif
  
  dataString2 = "";
  #ifdef Anemodef
  dataString2 = "\nWindS,";
  dataString2 += pulseCountAll/numberOfWindSamples;
  dataString2 += "\nWindSM,";
  dataString2 += pulseCountMax;
  dataString2 += "\nWindD,";
  dataString2 += windDirectionAll/numberOfWindSamples;
  pulseCountAll=0;
  pulseCountMax=0;
  windDirectionAll=0;
  numberOfWindSamples=0;
  #endif

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connected");
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
    client.println(dataString1.length()+dataString2.length());
    //client.println(dataString2.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
	//dataString1.trim();
	//dataString2.trim();
    client.print(dataString1);
    client.print(dataString2);
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("failed");
//    Serial.println();
//    Serial.println("disconnecting.");
    client.stop();
  }
 
/*  Serial.println("\nDATA:");
  Serial.println(dataString1);
  Serial.println(dataString2);
  Serial.println();
*/  
  // note the time that the connection was made or attempted:
}
#endif


#ifdef DALLASdef
/*void printTemperatureAll() {
  // Loop through each device, print out temperature data
  for(byte i=0;i<numberOfDevices; i++) {
    // Search the wire for address
      Serial.print("T");
      Serial.print(i, DEC);
      Serial.print("[");
      for (byte j=0; j<8;j++) {
        if (tempDeviceAddresses[i][j] < 16) Serial.print("0");
        Serial.print(tempDeviceAddresses[i][j], HEX);
      }
      Serial.print("]");

      Serial.print(sensor[i]);
      Serial.println(" C ");
  }
}
*/

void dsInit(void) {
  dsSensors.begin();
  numberOfDevices = dsSensors.getDeviceCount();

  // Loop through each device, print out address
  for (byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if (dsSensors.getAddress(tempDeviceAddress, i)) {
      /*for (byte j=0; j<8; j++) {
        if (tempDeviceAddress[j] < 16) Serial.print("0");
      }
      */
      memcpy(tempDeviceAddresses[i],tempDeviceAddress,8);
    }
    else
    {
      //Serial.println("Unable to get device address for sensor " + i);
    }
  }
  #ifndef dallasMinimal
  dsSensors.setResolution(12);
  dsSensors.setWaitForConversion(false);
  #endif
}
#endif

#ifdef BMP085def
void bmp085Init() {
  #ifdef SWI2C
  bmp.begin();
  #else
  Wire.begin();
  delay(1000);
  bmp.init(MODE_ULTRA_HIGHRES, high_above_sea, true);  // 250 meters, true = using meter units
                  // this initialization is useful if current altitude is known,
                  // pressure will be calculated based on TruePressure and known altitude.
  #endif
}

#ifdef SWI2C
long getRealPressure(long TruePressure, long _param_centimeters) {
  return TruePressure / pow((1 - (float)_param_centimeters / 4433000), 5.255); // + 101325;
}
#endif

#endif

#ifdef DHTdef
void dhtInit() {
  //Serial.println("\nDHT setup");
  dht.begin();
  //Serial.print("DHT software on PIN D");
  //Serial.print(DHTPIN);
  //Serial.println(" OK");
}
#endif

#ifdef Anemodef
//calculate wind direction
String calculateWindDirection(int adcValue) {
  String retVal="";
  if (adcValue>0 && adcValue<64)
    retVal="J";
  if (adcValue>=64 && adcValue<192)
    retVal="JZ";
  if (adcValue>=192 && adcValue<320)
    retVal="Z";
  if (adcValue>=320 && adcValue<448)
    retVal="SZ";
  if (adcValue>=448 && adcValue<576)
    retVal="S";
  if (adcValue>=576 && adcValue<704)
    retVal="SV";
  if (adcValue>=704 && adcValue<832)
    retVal="V";
  if (adcValue>=832 && adcValue<960)
    retVal="JV";
  if (adcValue>=960)
    retVal="J";
  return retVal;
}

unsigned int calculateWindDirectionDegrees(int adcValue) {
  return (int)((float)adcValue/1024.f*360.f);
}

void counterISR() { 
  pulseCount++;
}
#endif