#include <limits.h>

#include <Ethernet.h>
#include <SPI.h>
// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,1,1);

// initialize the library instance:
EthernetClient client;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
char server[] = "api.cosm.com";   // name address for cosm API
//IPAddress server(216,52,233,121);      // numeric IP for api.cosm.com

EthernetUDP Udp;
#include <UdpBytewise.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008 

unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServer(192, 43, 244, 18); // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
const unsigned long UTCcorrection = 0;    // keep UTC 
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
// A UDP instance to let us send and receive packets over UDP
byte SNTP_server_IP[]    = { 192, 43, 244, 18}; // time.nist.gov
//byte SNTP_server_IP[] = { 130,149,17,21};    // ntps1-0.cs.tu-berlin.de
//byte SNTP_server_IP[] = { 192,53,103,108};   // ptbtime1.ptb.de
//#include <Time.h>
typedef unsigned long time_t;

//#include <SD.h>
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
//Sd2Card card;
//SdVolume volume;
//SdFile root;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
//const int chipSelect = 4;  

void write2File(String data);

//COSM
#define APIKEY         "q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g" // your cosm api key
#define FEEDID         63310 // your feed ID
#define USERAGENT      "Cosm Arduino Example (63310)" // user agent is the project name

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5*1000; //delay between updates to Cosm.com
long dsLastMeasTime = 0;
byte sample=0;
boolean dsStart = false;


void setup() {
  // start serial port:
  Serial.begin(9600);
  Serial.println("Ethernet and SD card Test");
  // give the ethernet module time to boot up:
  delay(1000);

  //must be before ethernet init
//  Serial.print("SD card init...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  //pinMode(10, OUTPUT);
  //digitalWrite(10, HIGH);
 
    // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  //if (!card.init(SPI_HALF_SPEED, chipSelect)) {
  //if (!SD.begin(chipSelect)) {  
  //  Serial.println("initialization failed.");
  //} 
  //else
  //{
  // Serial.println("Wiring is correct and a card is present."); 
  //}

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  Serial.print("Ethernet OK - ");
  Serial.println(Ethernet.localIP());
  
  Serial.println("waiting for sync");
//  setSyncProvider(getNtpTime);
//  while(timeStatus()== timeNotSet)   
//     ; // wait until the time is set by the sync provider

  
  Udp.begin(localPort);
//  readTime();

 
}

void loop() {
  if (!dsStart) {
    dsLastMeasTime = millis();
    dsStart = true;
  }
  
  if (millis() - dsLastMeasTime > 1000) {

    Serial.print(++sample);
    Serial.print(". ");
    Serial.print(" T(C):");
    Serial.print("20.0");
    Serial.print("  Alt(cm):0");
    Serial.print(" P(Pa):101325");
    Serial.println("");
    dsStart = false;
  }
  
  if (client.connected() && sample>=3) {
    Serial.println("disconnected...");
    client.stop();
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {

    String dataString = "TempOut,20.0";
    dataString += "\nPress,101325";
    sendData(dataString);
    //write2File(dataString);
    
    lastConnectionTime = millis();
    sample=0;
  }
}


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

/*void write2File(String data) {
  File myFile;
  
  if (!SD.exists("data.txt")) {
    Serial.println("New file data.txt was created.");
    myFile = SD.open("data.txt", FILE_WRITE);
    myFile.close();
  }
  
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (myFile) {
    myFile.println(data);
    myFile.close();
    Serial.println("Data was written to data.txt");
    // print to the serial port too:
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening data.txt");
  } 
}
*/
/*
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
    
    time_t t = epoch + UTCcorrection;  
    //setTime(t);    // sets arduino internal clock  
  }
  else
    Serial.println("UDP parse error.");
}

*/
/*-------- NTP code ----------*/
/*
unsigned long getNtpTime()
{
  sendNTPpacket(SNTP_server_IP);
  delay(1000);
  if ( UdpBytewise.available() ) {
    for(int i=0; i < 40; i++)
       UdpBytewise.read(); // ignore every field except the time
    const unsigned long seventy_years = 2208988800UL + timeZoneOffset;        
    return getUlong() -  seventy_years;      
  }
  return 0; // return 0 if unable to get the time
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
*/
