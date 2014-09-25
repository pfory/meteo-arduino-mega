/*PINOUT

D0 Rx from CentralUnit
D1 Tx to CentralUnit
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

#define watchdog //enable this only on board with UNO bootloader
#ifdef watchdog
#include <avr/wdt.h>
#endif

#include <limits.h>

#include <avr/pgmspace.h>
unsigned long crc;
static PROGMEM prog_uint32_t crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

#define START_BLOCK 			'#'
#define DELIMITER 				';'
#define END_BLOCK 				'$'
#define END_TRANSMITION 	'*'

#define LEDPIN 13

//#define debug
#define DALLASdef 
#define Anemodef
#define BMP085def
#define RainSensdef
//#define SWI2C  // - proverit zda funguje, kdyz ano tak pouzit protoze je uspornejsi
#define DHTdef //1022

#ifndef dummy //this section prevent from error while program is compiling without Ethernetdef
char a[0]; //do not delete this dummy variable
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
unsigned long lastPressureTime=0;
#define PRESSNOCHANGE 0
#define PRESSUP       1
#define PRESSDOWN     2
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

#include <SoftwareSerial.h>
#define RX 10
#define TX 11
SoftwareSerial mySerial(RX, TX);

const unsigned int serialTimeout=2000;

byte counter=0;
byte status=0;

bool firstMeasComplete                    = false;

float versionSW=1.0;
char versionSWString[] = "METEO Simple v"; //SW name & version

//byte ledPin=9;
unsigned int const SERIAL_SPEED=9600;
//#define verbose

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(SERIAL_SPEED);
	Serial.print(versionSWString);
  Serial.println(versionSW);
  
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

}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {
#ifdef watchdog
	wdt_reset();
#endif

  //start sampling
  if ((millis()) - lastMeasTime >= dsMeassureInterval) {
    firstMeasComplete=true;
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
    tempDHT = dht.readTemperature();
#endif

    
#ifdef BMP085def
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

  communication();
}


//-------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------
char dataRequested() {
	char incomingByte=0;
	if (mySerial.available() > 0) {
    incomingByte = (char)mySerial.read();
#ifdef serial
		Serial.print("Data req-");
		Serial.println(incomingByte);
#endif
  }
	return incomingByte;
}

void communication() {
  char req=dataRequested();
	if (req=='R') { //if data were requested from central unit then send data
		sendDataSerial();
  }
}



//send data to CentralUnit via RS232
void sendDataSerial() {
  if (firstMeasComplete==false) return;

#ifdef serial
  Serial.print("DATA:");
#endif
	//#0;25.31#H;25#P;101416#U;15#R;0#P;0#T;0.00#T24.4;#D;25#W;30#S;4;#M;13;V#0.78$3600177622*
	digitalWrite(LEDPIN,HIGH);
	crc = ~0L;
  for (byte i=0;i<numberOfDevices; i++) {
		send(START_BLOCK);
		send(i);
		send(DELIMITER);
		send(sensor[i]);
	}
	send(START_BLOCK);
	send('H'); //Humidity
	send(DELIMITER);
	send((char)humidity);

	send(START_BLOCK);
	send('P'); //Press
	send(DELIMITER);
	send((float)Pressure);

	send(START_BLOCK);
	send('U'); //Pulse length ms
	send(DELIMITER);
  send(pulseLength);

	send(START_BLOCK);
	send('R'); //Rain pulse
	send(DELIMITER);
	send(pulseCountRainAll);
  pulseCountRainAll=0;  

	send(START_BLOCK);
	send('T'); //temperature 085
	send(DELIMITER);
	send((float)((float)Temperature/10.0));
	
	send(START_BLOCK);
	send('D'); //temp DHT
	send(DELIMITER);
	send((char)tempDHT);
	
	send(START_BLOCK);
	send('W'); //Wind direction
	send(DELIMITER);
	send(windDirectionAll/numberOfWindSamples);
	
	send(START_BLOCK);
	send('S'); //Wind speed
	send(DELIMITER);
	send(pulseCountAll/numberOfWindSamples);
	
	send(START_BLOCK);
	send('M'); //Wind speed maximal
	send(DELIMITER);
	send(pulseCountMax);
	
	send(START_BLOCK);
	send('V');
	send(DELIMITER);
	send(versionSW);

	send(END_BLOCK);
  pulseLength = 0;
  pulseCountAll=0;
  pulseCountMax=0;
  windDirectionAll=0;
  numberOfWindSamples=0;
  
#ifdef serial
	Serial.print(crc);
	Serial.println(END_TRANSMITION);
#endif	
	mySerial.print(crc);
	mySerial.print(END_TRANSMITION);
	mySerial.flush();
	digitalWrite(LEDPIN,LOW);
}

void send() {}

void send(char s) {
	send(s, ' ');
}


void send(char s, char type) {
	if (type=='X') {
#ifdef serial
		Serial.print(s, HEX);
#endif
		mySerial.print(s, HEX);
	}
	else {
#ifdef serial
		Serial.print(s);
#endif
		mySerial.print(s);
	}
	crc_string(byte(s));
}

void send(byte s) {
	send(s, ' ');
}

void send(byte s, char type) {
	if (type=='X') {
#ifdef serial
		Serial.print(s, HEX);
#endif
		mySerial.print(s, HEX);
	}
	else {
#ifdef serial
		Serial.print(s);
#endif
		mySerial.print(s);
	}
	crc_string(s);
}

void send(unsigned long s) {
#ifdef serial
  Serial.print(s);
#endif
	mySerial.print(s);
}

void send(unsigned int s) {
#ifdef serial
  Serial.print(s);
#endif
  mySerial.print(s);
}

void send(float s) {
	char tBuffer[8];
	dtostrf(s,0,2,tBuffer);
	for (byte i=0; i<8; i++) {
		if (tBuffer[i]==0) break;
		send(tBuffer[i]);
	}
}

unsigned long crc_update(unsigned long crc, byte data)
{
    byte tbl_idx;
    tbl_idx = crc ^ (data >> (0 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    tbl_idx = crc ^ (data >> (1 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    return crc;
}

void crc_string(byte s)
{
  crc = crc_update(crc, s);
  crc = ~crc;
}


#ifdef DALLASdef
void dsInit(void) {
  dsSensors.begin();
  numberOfDevices = dsSensors.getDeviceCount();

  // Loop through each device, print out address
  for (byte i=0;i<numberOfDevices; i++) {
      // Search the wire for address
    if (dsSensors.getAddress(tempDeviceAddress, i)) {
      memcpy(tempDeviceAddresses[i],tempDeviceAddress,8);
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
  dht.begin();
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