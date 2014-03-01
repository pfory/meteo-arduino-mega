/*PINOUT

D0 Rx
D1 Tx
D2 Rain sensor
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

#include <limits.h>

//#define debug
//#define serial
#define Ethernetdef
#define DALLASdef 
#define Anemodef
#define BMP085def
#define RainSensdef
#define DHTdef //1022

#ifndef dummy //this section prevent from error while program is compiling without Ethernetdef
char a[0]; //do not delete this dummy variable
#endif

#ifdef Ethernetdef
//#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

byte mac[] = { 0x00, 0xE0, 0x07D, 0xCE, 0xC6, 0x6F};

// Your Xively key to let you upload data
char xivelyKey[] = "q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g";
//your xively feed ID
#define xivelyFeed 63310
//datastreams
char VersionID[] = "V";
char StatusID[] = "H";
char DHTHumidityID[] = "Humidity";
char DHTTemperatureID[] = "TempDHT";
char PressID[] = "Press";
char PressTemperatureID[] = "Temp085";
char RainID[] = "Rain";
char TemperatureID[] = "T2899BDCF02000076";
char WindDirectionID[] = "WindD";
char WindSpeedID[] = "WindS";
char WindSpeedmaxID[] = "WindSM";


//TODO add all streams
// Define the strings for our datastream IDs
XivelyDatastream datastreams[] = {
XivelyDatastream(VersionID, strlen(VersionID), DATASTREAM_FLOAT),
XivelyDatastream(StatusID, strlen(StatusID), DATASTREAM_INT),
XivelyDatastream(DHTHumidityID, strlen(DHTHumidityID), DATASTREAM_INT),
XivelyDatastream(DHTTemperatureID, strlen(DHTTemperatureID), DATASTREAM_INT),
XivelyDatastream(PressID, strlen(PressID), DATASTREAM_INT),
XivelyDatastream(PressTemperatureID, strlen(PressTemperatureID), DATASTREAM_INT),
XivelyDatastream(RainID, strlen(RainID), DATASTREAM_INT),
XivelyDatastream(TemperatureID, strlen(TemperatureID), DATASTREAM_FLOAT),
XivelyDatastream(WindDirectionID, strlen(WindDirectionID), DATASTREAM_INT),
XivelyDatastream(WindSpeedID, strlen(WindSpeedID), DATASTREAM_INT),
XivelyDatastream(WindSpeedmaxID, strlen(WindSpeedmaxID), DATASTREAM_INT),
//XivelyDatastream(WindRatioID, strlen(WindRatioID), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(xivelyFeed, datastreams, 11 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

bool checkConfigFlag = false;
unsigned long lastSendTime;
float windRatio = 4;
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
#define NUMBER_OF_DEVICES 2
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
//#include <Wire.h>
#include <BMP085.h> //558 bytes +
#include "I2Cdev.h"
#include "Wire.h"
BMP085 bmp = BMP085();      // Digital Pressure Sensor 
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
float Temperature = 0;
float Pressure = 0;//, Altitude = 0;

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
unsigned int pulseCountPrev=0;
const unsigned int pulseDiff=30;      //rozdil mezi otackami v minule vterine a aktualnimi, pokud je vetsi nez pulseDiff jde o zakmit a otacky se neprictou
unsigned int pulseCountAll=0;
unsigned int pulseCountMax=0;
unsigned int windDirectionAll=0;
unsigned long time=0;
byte numberOfWindSamples=0;
#define windDirPin 3
#endif 

#ifdef RainSensdef
const byte counterPinRain = 2; 
const byte counterInterruptRain = 0; // = pin D2
volatile bool pulseCountRain=false;
unsigned int pulseCountRainAll=0;
#endif

byte counter=0;
byte pila=0;

float versionSW=0.83;
String versionSWString = "METEO Simple v"; //SW name & version

//byte ledPin=9;

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  #ifdef serial
  Serial.begin(19200);
//  Serial.println();
  Serial.println(versionSW);
  #endif
  //Serial.println(F("SW inicialization"));

  #ifdef Ethernetdef
  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
	
	#ifdef serial
  Serial.println("EthOK");
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
  
  lastSendTime = dsLastPrintTime = lastMeasTime = millis();
  lastSendTime = dsLastPrintTime = lastMeasTime = millis();
  #endif
  
  #ifdef DALLASdef
  dsInit();
  lastDisplayTempTime = millis();
  dsSensors.requestTemperatures(); 
  #endif
  
  #ifdef Anemodef 
  pinMode(counterPin, INPUT);      
  digitalWrite(counterPin, HIGH);
  attachInterrupt(counterInterrupt, counterISR, RISING);
  #endif

  #ifdef RainSensdef
  pinMode(counterPinRain, INPUT);      
  //digitalWrite(counterPinRain, HIGH);
  attachInterrupt(counterInterruptRain, counterISRRain, RISING);
  #endif

  #ifdef BMP085def
  bmp085Init();
	Serial.println("BMP085");
  lastDisplayBMPTime = millis();
  #endif
  
  #ifdef DHTdef
  dhtInit();
  lastDHTMeasTime=millis();
  dht.startMeas();
  lastDisplayDHTTime = millis();
	Serial.println("DHT");
  #else
//  Serial.println("DHT N/A");
  #endif

   //pinMode(ledPin, OUTPUT);
  //Serial.println("End of SW initialization phase, I am starting measuring.");

}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {
  //start sampling
  if ((millis()) - lastMeasTime >= dsMeassureInterval) {
    lastMeasTime = millis();
    lastDsMeasStartTime=lastMeasTime;
    sample++;
    //startTimer();
    #ifdef DALLASdef
    dsSensors.requestTemperatures(); 
		dsMeasStarted=true;
    #endif
    
    #ifdef DHTdef
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    datastreams[2].setInt(humidity);
 		Serial.print("Humidity:");
    Serial.println(humidity);
		tempDHT = dht.readTemperature();
    datastreams[3].setInt(tempDHT);
    #endif

    
    #ifdef BMP085def
    //bmpMin.getPressure(&Pressure);
    bmp.setControl(BMP085_MODE_TEMPERATURE);
    // wait appropriate time for conversion (4.5ms delay)
    int32_t lastMicrosBMP = micros();
    while (micros() - lastMicrosBMP < bmp.getMeasureDelayMicroseconds());
    Temperature = bmp.getTemperatureC(); 
    datastreams[5].setInt(Temperature);
    bmp.setControl(BMP085_MODE_PRESSURE_3);
    lastMicrosBMP = micros();
    while (micros() - lastMicrosBMP < bmp.getMeasureDelayMicroseconds());
    Pressure = bmp.getPressure();
    
    Pressure = Pressure / pow((1 - (float)high_above_sea / 4433000), 5.255) + 0;
    datastreams[4].setInt(Pressure);
 		Serial.print("Press:");
    Serial.println(Pressure);
#endif
  }

  #ifdef Anemodef
  if ((millis()) - time >= 1000) {
    time = millis();
    numberOfWindSamples++;
    int val=analogRead(windDirPin);
    /*Serial.print(val);
    Serial.print("-");
    Serial.println(calculateWindDirectionDegrees(val));*/
    windDirectionAll+=calculateWindDirectionDegrees(val);
    if (abs(pulseCount-pulseCountPrev)<pulseDiff) {
      pulseCountAll+=pulseCount;
      pulseCountMax = max(pulseCount,pulseCountMax);
    }
    pulseCountPrev = pulseCount;
    pulseCount=0;
  
    #ifdef RainSensdef
    if (pulseCountRain==true) {
      pulseCountRainAll++;
      //Serial.print(pulseCountRainAll);
      pulseCountRain=false;
    }
    #endif

  }
  #endif

  
  #ifdef DALLASdef
  if (dsMeasStarted) {
    if (millis() - lastDsMeasStartTime >= dsMeassureDelay) {
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
      datastreams[7].setInt(sensor[0]);
			Serial.print("Temp:");
			Serial.println(sensor[0]);
      //obcas se vyskytne chyba a vsechna cidla prestanou merit
      //zkusim restartovat sbernici
      bool reset=true;
      for (byte i=0; i<numberOfDevices; i++) {
        if (sensor[i]!=0.0) {
          reset=false;
        }
      }
      if (reset) {
        pila=2;
        dsInit();
      }
    }
  }
  #endif
    
    
  #ifdef Ethernetdef
  if(!client.connected() && ((millis()) - lastSendTime >= sendTimeDelay)) {
    lastSendTime = millis();
    //digitalWrite(ledPin, HIGH);
    sendData();
    //digitalWrite(ledPin, LOW);
    sample=0;
  }
  #endif
}

//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
#ifdef Ethernetdef
void sendData() {
  datastreams[0].setFloat(versionSW);
  datastreams[1].setInt(pila);
  if (pila>0) pila=0; else pila=1;

  datastreams[2].setInt(0); //Humidity
  datastreams[3].setInt(0); //TempDHT
  datastreams[4].setInt(0); //Press
  datastreams[5].setInt(0); //Temp085
  datastreams[6].setInt(pulseCountRainAll); //Rain
  pulseCountRainAll=0;
  datastreams[7].setFloat(0.0); //T2899BDCF02000076
  datastreams[8].setInt(windDirectionAll/numberOfWindSamples); //WindD
  windDirectionAll=0;
  numberOfWindSamples=0;
  datastreams[9].setInt((float)(pulseCountAll/numberOfWindSamples/windRatio)); //WindS
  datastreams[10].setInt((float)pulseCountMax/windRatio);  //WindSM
  
  pulseCountAll=0;
  pulseCountMax=0;

  
   //send value to xively
  #ifdef serial
	Serial.println("Uploading it to Xively");
	#endif
  int ret = xivelyclient.put(feed, xivelyKey);
  //return message
  #ifdef serial
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");
	#endif
}


 #endif


#ifdef DALLASdef
void dsInit(void) {
  dsSensors.begin();
  numberOfDevices = dsSensors.getDeviceCount();

	#ifdef serial
	Serial.print("DALLAS sensor(s):");
	Serial.println(numberOfDevices);
	#endif
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
  delay(1000);
  bmp.initialize();  // 250 meters, true = using meter units
  //bmp.init(high_above_sea);  // 250 meters, true = using meter units
  //bmp.init(MODE_ULTRA_HIGHRES, high_above_sea, true);  // 250 meters, true = using meter units
                  // this initialization is useful if current altitude is known,
                  // pressure will be calculated based on TruePressure and known altitude.
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

#ifdef RainSensdef
void counterISRRain() { 
  pulseCountRain=true;
}
#endif