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



#include <limits.h>
#include "Arduino.h"

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

unsigned long start, stop;
unsigned int sample=0;

unsigned long lastMeasTime;
unsigned long dsLastPrintTime;
String versionSW("METEO Simple v0.2"); //SW name & version


//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(115200);
  Serial.println(versionSW);


  Serial.println("SW inicialization");

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

  dsInit();
  lastDisplayTempTime = millis();
  dsSensors.setResolution(12);
  dsSensors.setWaitForConversion(false);
  lastSendTime = millis();
  
  dsLastPrintTime = millis();
  lastMeasTime = millis();
  dsSensors.requestTemperatures(); 
  
  Serial.println("End of SW initialization phase, I am starting measuring.");

}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {

  //start sampling
  if (millis() - lastMeasTime > 4000) {
    sample++;
    lastMeasTime = millis();
    //startTimer();
    dsSensors.requestTemperatures(); 
    lastDsMeasStartTime=millis();
    dsMeasStarted=true;
  }
  
  if (dsMeasStarted) {
    if (millis() - lastDsMeasStartTime>750) {
      dsMeasStarted=false;
      //saving temperatures into variables
      for (byte i=0;i<numberOfDevices; i++) {
        float tempTemp=-126;
        for (byte j=0;j<10;j++) { //try to read temperature ten times
          tempTemp = dsSensors.getTempCByIndex(i);
          if (tempTemp>=-55) {
            break;
          }
        }
        sensor[i]=tempTemp;
      } 
    }
  }
  
    
  if (millis() - dsLastPrintTime > 1000) {

    Serial.println();
    printTemperatureAll();
  
  
    Serial.println("");
    dsLastPrintTime = millis(); 
  }
  
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
}

//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
void sendData() {

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
      for (byte j=0; j<8; j++) {
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

  Serial.print("DALLAS on pin D");
  Serial.print(ONE_WIRE_BUS);
  Serial.println(" OK");
}

