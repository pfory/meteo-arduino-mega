/*
--------------------------------------------------------------------------------------------------------------------------

               METEO - meteostation

Petr Fory pfory@seznam.cz
SVN  - https://meteo-arduino-mega.googlecode.com/svn/trunk/

Version history:
1.01 - 19.10.2014 change humidity sensor DHT11 -> DHT22 (29034 FLASH, 1742 RAM)
1.00 - 



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
A1 DHT22 humidity
A2 free
A3 Anemometer - Wind direction
A4 SDA for Pressure BMP085
A5 SCL fpr Pressure BMP085
*/

#define watchdog //enable this only on board with UNO bootloader
#ifdef watchdog
#include <avr/wdt.h>
#endif

#include <limits.h>

//#define debug
#define Ethernetdef
#define DALLASdef 
#define Anemodef
#define BMP085def
#define RainSensdef
//#define SWI2C  // - proverit zda funguje, kdyz ano tak pouzit protoze je uspornejsi
#define DHTdef //1022

#ifndef dummy //this section prevent from error while program is compiling without Ethernetdef
char a[0]; //do not delete this dummy variable
#endif

#ifdef Ethernetdef
#include <Ethernet.h>
byte mac[] = { 0x00, 0xE0, 0x07D, 0xCE, 0xC6, 0x6F };
EthernetClient client;
char server[] = "api.cosm.com";   // name address for cosm API
bool checkConfigFlag = false;
IPAddress ip(192,168,1,102);

//XIVELY
#include <Xively.h>
#include <HttpClient.h>


char xivelyKey[] 			= "q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g";

#define xivelyFeed 				63310

char VersionID[]	 	    = "V";
char StatusID[]	 	      = "H";
char TempID[]	 	        = "T2899BDCF02000076";
char HumidityID[]	 	    = "Humidity";
char PressID[]	 	      = "Press";
char RainID[]	 	        = "Rain";
char Temp085ID[]	 	    = "Temp085";
char TempDHTID[]	 	    = "TempDHT";
char WindSpeedID[]	    = "WindS";
char WindSpeedMaxID[]	  = "WindSM";
char WindDirectionID[]	= "WindDir";
char PulseLengthID[]	  = "Pulse";


XivelyDatastream datastreams[] = {
	XivelyDatastream(VersionID, 		    strlen(VersionID), 	      DATASTREAM_FLOAT),
	XivelyDatastream(StatusID, 		      strlen(StatusID), 		    DATASTREAM_INT),
	XivelyDatastream(TempID,  		      strlen(TempID), 		      DATASTREAM_FLOAT),
	XivelyDatastream(HumidityID, 		    strlen(HumidityID), 		  DATASTREAM_INT),
	XivelyDatastream(PressID, 		      strlen(PressID), 		      DATASTREAM_FLOAT),
	XivelyDatastream(RainID, 		        strlen(RainID), 		      DATASTREAM_INT),
	XivelyDatastream(Temp085ID, 		    strlen(Temp085ID), 		    DATASTREAM_FLOAT),
	XivelyDatastream(TempDHTID, 		    strlen(TempDHTID), 		    DATASTREAM_INT),
	XivelyDatastream(WindSpeedID, 		  strlen(StatusID), 		    DATASTREAM_INT),
	XivelyDatastream(WindSpeedMaxID, 		strlen(WindSpeedMaxID),   DATASTREAM_INT),
	XivelyDatastream(WindDirectionID, 	strlen(WindDirectionID), 	DATASTREAM_INT),
	XivelyDatastream(PulseLengthID, 	  strlen(PulseLengthID), 	  DATASTREAM_INT)
};

XivelyFeed feed(xivelyFeed, 			datastreams, 			12);
XivelyClient xivelyclient(client);

unsigned long lastSendTime;
int ethOK=false;
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
#define NUMBER_OF_DEVICES 1
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
//unsigned int sample=0;
unsigned int const dsPrintTimeDelay=4000; //interval to show results
unsigned int const sendTimeDelay=30000; //to send to cosm.com

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
#define DHTTYPE DHT22   // DHT 11 
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
//1 klapnutí 5,235ml = 0.2mm/m2
//překlopení trvá 45 a 77ms
//plocha 2,925dm2
const byte counterPinRain = 2; 
const byte counterInterruptRain = 0; // = pin D2
volatile byte pulseCountRainAll=0;
unsigned long startRain=0;
unsigned int pulseLength=0;
#endif

byte counter=0;
byte status=0;

float versionSW=1.1;
char versionSWString[] = "METEO Simple v"; //SW name & version

//byte ledPin=9;
unsigned int const SERIAL_SPEED=9600;
////#define verbose

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(SERIAL_SPEED);
//  Serial.println();
  Serial.println(versionSW);
 
#ifdef verbose
	//delay(5000);
  Serial.println("waiting for net...");
#endif
	//lcd.setCursor(0,0);
  //lcd.print("waiting for net");
	//Ethernet.begin(mac, ip, dnServer, gateway, subnet);
 
  Ethernet.begin(mac, ip);
  ethOK = true;

/*  while (ethOK==false && cyklus++<10) {
    if (Ethernet.begin(mac) == 0) {
      Serial.println("EthOK");
      ethOK = true;
    } else {
#ifdef verbose
    Serial.println("Error getting IP address via DHCP, trying again...");
#endif
    }
    delay(2000);
  }
*/
//#ifdef verbose
  if (ethOK) {
    Serial.println("EthOK");
    Serial.print("\nIP:");
    Serial.println(Ethernet.localIP());
    /*Serial.print("Mask:");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Gateway:");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS:");
    Serial.println(Ethernet.dnsServerIP());
    Serial.println();*/
  } else {
    Serial.println("No internet!");
  }
//#endif

  lastSendTime = dsLastPrintTime = lastMeasTime = millis();

  
#ifdef watchdog
	wdt_enable(WDTO_8S);
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
  attachInterrupt(counterInterruptRain, counterISRRain, CHANGE);
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

   //pinMode(ledPin, OUTPUT);
  //Serial.println("End of SW initialization phase, I am starting measuring.");

}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {
#ifdef watchdog
	wdt_reset();
#endif

  //start sampling
  if ((millis()) - lastMeasTime >= dsMeassureInterval) {
    lastMeasTime = millis();
    lastDsMeasStartTime=lastMeasTime;
    //sample++;
    //startTimer();
#ifdef DALLASdef
    dsSensors.requestTemperatures(); 
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
    Temperature = bmp.readTemperature();
#else
    bmp.getPressure(&Pressure);
    bmp.getTemperature(&Temperature); 
#endif
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
        Serial.println(sensor[i]);
      } 
      //obcas se vyskytne chyba a vsechna cidla prestanou merit
      //zkusim restartovat sbernici
      bool reset=true;
      for (byte i=0; i<numberOfDevices; i++) {
        if (sensor[i]!=0.0) {
          reset=false;
        }
      }
      if (reset) {
        status=2;
        dsInit();
      }
    }
  }
#endif
  
  
#ifdef Ethernetdef
  /*if (sample==2) {
    client.stop();
  }

  if (sample==5 && checkConfigFlag == false) {
    //checkConfig();
  }

  if (sample==8) {
    checkConfigFlag = false;
  }
  */

  if (millis() - lastSendTime >= sendTimeDelay) {
    //digitalWrite(ledPin, HIGH);
    sendData();
    //digitalWrite(ledPin, LOW);
    //sample=0;
  }
#endif
}

//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
#ifdef Ethernetdef
void sendData() {
  datastreams[0].setFloat(versionSW);  
  datastreams[1].setInt(status);  
  datastreams[2].setFloat((int)(sensor[0]*10.f)/10.f);  
  datastreams[3].setInt(humidity);
  datastreams[4].setFloat((float)Pressure);  
  datastreams[5].setInt(pulseCountRainAll);
  datastreams[6].setFloat((float)Temperature/10.0);  
  datastreams[7].setInt(tempDHT);  
  datastreams[8].setInt(pulseCountAll/numberOfWindSamples);  
  datastreams[9].setInt(pulseCountMax);  
  datastreams[10].setInt(windDirectionAll/numberOfWindSamples);  
  datastreams[11].setInt(pulseLength);  

//#ifdef verbose
  Serial.println("Uploading data to Xively");
//#endif
#ifdef watchdog
	wdt_disable();
#endif

  int ret = xivelyclient.put(feed, xivelyKey);
  
  if (ret==200) {
    if (status==0) status=1; else status=0;
    pulseCountRainAll=0;  
    pulseLength = 0;
    pulseCountAll=0;
    pulseCountMax=0;
    windDirectionAll=0;
    numberOfWindSamples=0;
    Serial.print("Xively OK:");
	} else {
  //#ifdef verbose
    Serial.print("Xively err: ");
  //#endif
  }
  Serial.println(ret);
  
#ifdef watchdog
	wdt_enable(WDTO_8S);
#endif
  lastSendTime = millis();
}
#endif


#ifdef DALLASdef
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

#ifdef RainSensdef
void counterISRRain() { 
  if (digitalRead(counterPinRain)==LOW) {
    startRain=millis();
  } else {
    pulseLength = millis()-startRain;
    if ((pulseLength)>35 && (pulseLength)<80) {
      pulseCountRainAll++;
    }
  }
}
#endif