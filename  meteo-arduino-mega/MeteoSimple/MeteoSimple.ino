/*PINOUT

D0 Rx
D1 Tx
D2 free
D3 free
D4 free
D5-D9 free
D10 Ethernet shield
D11 free
D12
D13
A0 DALLAS temperature
A1 free
A2 free
A3 free
A4 free
A5 free


*/

#define Ethernetdef
#define DALLASdef 
//#define Anemodef
//#define BMP085def


#ifdef Anemodef
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >
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
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A0
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

int sensorReading = INT_MIN;

unsigned int sample=0;

unsigned long lastMeasTime;
unsigned long dsLastPrintTime;
#endif

#ifdef BMP085def
#include <Wire.h>
//#include <BMP085.h> //558 bytes +
#include <swI2C_BMP085.h>
#include <I2cMaster.h>
//BMP085 dps = BMP085();      // Digital Pressure Sensor 
swI2C_BMP085 bmp;
unsigned long lastDisplayBMPTime;
unsigned long avgPressure=0;
unsigned long lastAvgPressure=0;
unsigned int numberOfSamples=0;
unsigned long lastPressureTime=0;
#define PRESSNOCHANGE 0
#define PRESSUP       1
#define PRESSDOWN     2
byte pressureChange=PRESSNOCHANGE;
signed long         high_above_sea=34700;
signed long Temperature = 0;
unsigned long Pressure = 0;//, Altitude = 0;

#endif


#ifdef Anemodef 
ISR(TIMER1_COMPA_vect)
{
  Serial.println("COMPA");
  counterOverflow++;
  if (counterOverflow>1) 
  {
    Serial.println("bezvetri");
  }
}

ISR(TIMER1_CAPT_vect)
{
   Serial.println("CAPT");
	// uint16_t value, result;
	// value = ICR1L;
	// value += (ICR1H<<8);

  // if (counterOverflow==1)
    // result=(0xFFFF-old_value)+value;
  // else if (counterOverflow==0)
   	// result=value-old_value;
  // else
    // result=0;
	
	// if (result>65500)
	  // result = 0;

 	// old_value=value;

  // Serial.print(216000/result);
  // Serial.println("/min");


  // counterOverflow=0;
}
#endif 

byte counterOverflow=0;
unsigned int old_value=0;
byte counter=0;


String versionSW("METEO Simple v0.4"); //SW name & version

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(115200);
  Serial.println(versionSW);


  Serial.println("SW inicialization");

  #ifdef Ethernetdef
  Serial.print("waiting for net connection...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed using DHCP");
    // DHCP failed, so use a fixed IP address:
  }

  Serial.println("Ethernet OK");
  
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
  
  #ifdef DALLASdef
  dsInit();
  lastDisplayTempTime = millis();
  dsSensors.setResolution(12);
  dsSensors.setWaitForConversion(false);
  lastSendTime = millis();
  
  dsLastPrintTime = millis();
  lastMeasTime = millis();
  dsSensors.requestTemperatures(); 
  #endif
  
  #ifdef Anemodef 
  OCR1A=0xE100;     //57600   
  TCCR1B |= 1<<WGM12;
  TIMSK1|=1<<ICIE1; //input capture interrupt enable
	TCCR1B|=1<<ICES1; //input capture edge select
	TIMSK1|=1<<OCIE1A;	//output compareA match interrupt enable

	TCCR1B|=1<<CS12;  //prescaler clk/256

  pinMode(8, INPUT);
  #endif

  #ifdef BMP085def
  bmp085Init();
  lastDisplayBMPTime = millis();
  #endif

  
  Serial.println("End of SW initialization phase, I am starting measuring.");

}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {

  //start sampling
  if (millis() - lastMeasTime > 4000) {
    sample++;
    lastMeasTime = millis();
    //startTimer();
    #ifdef DALLASdef
    dsSensors.requestTemperatures(); 
    lastDsMeasStartTime=millis();
    dsMeasStarted=true;
    #endif
    
    #ifdef BMP085def
    unsigned long oldPress=Pressure;
    Pressure = bmp.readPressure();
    Pressure = getRealPressure(Pressure, high_above_sea);
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
  
    
  if (millis() - dsLastPrintTime > 1000) {

    Serial.println();
    printTemperatureAll();

    #ifdef BMP085def
    Serial.print("Press(Pa):");
    Serial.print(Pressure);
    #endif

    Serial.println("");
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

  if(!client.connected() && (millis() - lastSendTime > 20000)) {
    lastSendTime = millis();
    sendData();
    sample=0;
  }
  #endif
}

#ifdef Ethernetdef
//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
void sendData() {

  Serial.println("sending data");

  //prepare data to send
  String dataString = "";
  char buffer[3];
  //temperature from DALLAS
  //00 01 02 03 04 05 06 07
  //-----------------------
  //28 C9 B8 41 04 00 00 97
  
  for(byte i=0;i<numberOfDevices; i++) {
    dataString += "T";

    for (byte j=0; j<8; j++) {
      sprintf (buffer, "%X", tempDeviceAddresses[i][j]);
      if (tempDeviceAddresses[i][j]<16) {
        dataString += "0";
        dataString += buffer[0];
      }
      else {
        dataString += buffer[0];
        dataString += buffer[1];
      }
    }

    dataString += ",";
    int t = (int)(sensor[i]*10);
    
    if (t<0&&t>-10) {
      dataString += "-";
    }
    dataString += t/10;
    dataString += ".";
    dataString += abs(t%10);
    dataString += "\n";
  }

  #ifdef BMP085def
  //Pressure
  dataString += "Press,";
  dataString += Pressure;
  #endif
  //dataString += "\n";
  
   

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println();
    Serial.print("connecting to COSM [FEEDID=");
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
    client.println(dataString.length());

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
  
 
  Serial.println("\nDATA:");
  Serial.println(dataString);
  Serial.println();
  
  // note the time that the connection was made or attempted:
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
      for (byte j=0; j<8;j++) {
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
  numberOfDevices = dsSensors.getDeviceCount();

  // Loop through each device, print out address
  for(byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if(dsSensors.getAddress(tempDeviceAddress, i)) {
      /*for (byte j=0; j<8; j++) {
        if (tempDeviceAddress[j] < 16) Serial.print("0");
      }
      */
      memcpy(tempDeviceAddresses[i],tempDeviceAddress,8);
      
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      dsSensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    }
  }
}
#endif

#ifdef BMP085def
void bmp085Init() {
  bmp.begin();
}

long getRealPressure(long TruePressure, long _param_centimeters) {
  return TruePressure / pow((1 - (float)_param_centimeters / 4433000), 5.255); // + 101325;
}

#endif
