/*

 Udp NTP Client
 
 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol
 
 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe
 
 This code is in the public domain.

 */

#include <SPI.h>        
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Time.h>  

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

unsigned int localPort = 8888;      // local port to listen for UDP packets

IPAddress timeServer(192, 43, 244, 18); // time.nist.gov NTP server

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

const unsigned long seventyYears = 2208988800UL;       
unsigned long epoch = seventyYears; 
const unsigned long UTCcorrection = 0;    // keep UTC  

void setup()
{
 Serial.begin(9600);
  Ethernet.begin(mac);  
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  while(timeStatus()== timeNotSet)  
     ; // wait until the time is set by the sync provider
}

void loop()
{
 
}

void ntpTimeSync() {  
  sendNTPpacket(timeServer); // send an NTP packet to a time server & wait if a reply is available  
  delay(1000);    
  if ( Udp.available() ) {    
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer  
    // the timestamp starts at byte 40 of the received packet and is four bytes,  
    // or two words, long. First, esxtract the two words:  
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);  
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);    
    // combine the four bytes (two words) into a longint: NTP time (seconds since Jan 1 1900)  
    unsigned long secsSince1900 = highWord << 16 | lowWord;    
    // NTP gives secs from 1 1 1900. Unix time starts on Jan 1 1970. In seconds, that's 2208988800:  
    // subtract seventy years:  
    epoch = secsSince1900 - seventyYears;    
    Serial.print("T=");  
  } else {  
    Serial.print("Te!");   
  }  
  Serial.println(epoch);      
  time_t t = epoch + UTCcorrection;  
  setTime(t);    // sets arduino internal clock  
}  

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
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
