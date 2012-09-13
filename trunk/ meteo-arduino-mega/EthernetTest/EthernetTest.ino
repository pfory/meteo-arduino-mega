#define OneWiredef //1702 bytes
#define Wiredef //3656 bytes
#define DHTdef //1052 bytes
#define UDPdef //672 bytes
#define Ethernetdef //10614 bytes

#include <limits.h>
#include <MemoryFree.h>

#ifdef Ethernetdef
#include <Ethernet.h>
#include <SPI.h>
// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
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
#endif
#endif

#ifdef Ethernetdef
//COSM
#define APIKEY         "q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g" // your cosm api key
#define FEEDID         63310 // your feed ID
#define USERAGENT      "Cosm Arduino Example (63310)" // user agent is the project name
#endif

#ifdef OneWiredef
#include <OneWire.h>
OneWire ds(9);  // on pin 9
byte addr[8];
void dsInit();

#endif
int sensorReading = INT_MIN;

#ifdef Wiredef
#include <Wire.h>
//#include <BMP085.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 dps;
//BMP085 dps = BMP085();      // Digital Pressure Sensor 
#endif
#define HIGH_ABOVE_SEA 34700 //in cm
long Temperature = 0, Pressure = 0, Altitude = 0;

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
#endif
float humidity = 0;
float tempDHT = 0;


unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 20*1000; //delay between updates to Cosm.com
long dsLastMeasTime = 0;
byte sample=0;
boolean dsStart = false;


void setup() {
  // start serial port:
  Serial.begin(9600);
  Serial.println("Ethernet Test");
  
  Serial.print("Free memory : ");
  Serial.print(freeMemory());
  Serial.println(" bytes");

  #ifdef Ethernetdef
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed using DHCP");
    // DHCP failed, so use a fixed IP address:
    //Ethernet.begin(mac, ip);
  }
  Serial.println("Ethernet OK");
  #endif
  
  #ifdef UDPdef
  Udp.begin(localPort);
  readTime();
  #endif
  
  #ifdef OneWiredef    
  dsInit();
  Serial.println("DALLAS OK");
  #else
  Serial.println("DALLAS N/A");
  #endif
  
  #ifdef Wiredef
  //Wire.begin();
  //delay(1000);
  //dps.init(MODE_ULTRA_HIGHRES, HIGH_ABOVE_SEA, true);  // 250 meters, true = using meter units
                  // this initialization is useful if current altitude is known,
                  // pressure will be calculated based on TruePressure and known altitude.
  dps.begin();
  Serial.println("BMP085 OK");
  #else
  Serial.println("BMP085 N/A");
  #endif

  #ifdef DHTdef
  dht.begin();
  Serial.println("DHT OK");
  #else
  Serial.println("DHT N/A");
  #endif

}

void loop() {

  if (!dsStart) {
    #ifdef OneWiredef    
    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end
    #endif
    dsLastMeasTime = millis();
    dsStart = true;
  }

  if (millis() - dsLastMeasTime > 1000) {
    byte data[12];
    int Whole, Fract, SignBit;

    Serial.print(++sample);
    Serial.print(". ");

    #ifdef OneWiredef    
    int HighByte, LowByte, TReading,  Tc_100;
    ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
  
    for (byte i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
    }
    
    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;
    SignBit = TReading & 0x8000;  // test most sig bit
    if (SignBit) // negative
    {
      TReading = (TReading ^ 0xffff) + 1; // 2's comp
    }
    Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25
  
    Whole = Tc_100 / 100;  // separate off the whole and fractional portions
    Fract = Tc_100 % 100;
    #else
      Whole=20;
      Fract=0;
    #endif

    Serial.print(" T(C):");
  
    if (SignBit) // If its negative
    {
       Serial.print("-");
    }
    Serial.print(Whole);
    Serial.print(".");
    if (Fract < 10)
    {
       Serial.print("0");
    }
    Serial.print(Fract);
  
    sensorReading = (Whole * 10) + (Fract / 10);

    #ifdef Wiredef
    Pressure = bmp.readPressure();
    Altitude = bmp.reafAltitude();
    //dps.getPressure(&Pressure);
    //dps.getAltitude(&Altitude);
    #else
    Altitude=HIGH_ABOVE_SEA;
    Pressure=101325;
    #endif
    
    Serial.print(" Alt(cm):");
    Serial.print(Altitude);
    Serial.print(" Press(Pa):");
    Serial.print(Pressure);

    #ifdef DHTdef
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    tempDHT = dht.readTemperature();
    #endif

    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(tempDHT) || isnan(humidity)) {
      Serial.println("Failed to read from DHT");
    } else {
      Serial.print(" Humidity(%): "); 
      Serial.print(humidity);
      Serial.print(" Temperature(C): "); 
      Serial.print(tempDHT);
    }

    Serial.println("");

    dsStart = false;
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

    //temperature from DALLAS
    String dataString = "TempOut,";
    dataString += sensorReading / 10;
    dataString += ".";
    dataString += sensorReading % 10;

    //Pressure
    dataString += "\nPress,";
    dataString += Pressure;

    //Humidity
    dataString += "\nHumidity,";
    dataString += (int)humidity;

    //temperature from DHT11
    dataString += "\nTempIn,";
    dataString += (int)tempDHT;
  
    #ifdef Ethernetdef
    sendData(dataString);
    #endif

    lastConnectionTime = millis();
    sample=0;
  }
}

#ifdef Ethernetdef
void sendData(String thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
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
void readTime(void) {
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
    Serial.print("UTC=");       // UTC is the time at Greenwich Meridian (GMT)
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
  }
  else
    Serial.println("UDP parse error.");
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
#endif

#ifdef OneWiredef    
void dsInit() {
    if ( !ds.search(addr)) {
      Serial.print("No more addresses.\n");
      ds.reset_search();
      return;
    }
 
//   Serial.print("R=");
//   for(byte i = 0; i < 8; i++) {
//     Serial.print(addr[i], HEX);
//     Serial.print(" ");
//   }
 
   if ( OneWire::crc8( addr, 7) != addr[7]) {
       Serial.print("CRC !valid!\n");
       return;
   }
 
//   if ( addr[0] == 0x10) {
//       Serial.print("DS18S20 family device.\n");
//   }
//   else if ( addr[0] == 0x28) {
//       Serial.print("DS18B20 family device.\n");
//   }
//   else {
//       Serial.print("Not recognized: 0x");
//       Serial.println(addr[0],HEX);
//       return;
//   }
}
#endif

