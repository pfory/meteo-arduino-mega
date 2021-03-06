/* ==============================
 * This code, which assumes you're using the official Arduino Ethernet shield,
 * enables you to control your Arduino outputs from a web form 
 * (see http://community.pachube.com/ for more info)
 * 
 * Tested with Arduino 14
 *
 * Pachube is www.pachube.com - connect, tag and share real time sensor data
 * code by usman (www.haque.co.uk), may 2009
 * copy, distribute, change, whatever, as you like.
 *
 * v1.1 - added User-Agent & fixed HTTP parser for new Pachube headers
 * and check millis() for when it wraps around
 *
 * =============================== */

#define VERBOSE

#include <Ethernet.h>
#include <string.h>
#include <limits.h>
#undef int() // needed by arduino 0011 to allow use of stdio
#include <stdio.h> // for function sprintf

byte mac[] = { 
  0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x91 }; // make sure this is unique on your network
//byte ip[] = { 
//  192, 168, 1, 5   };                  // no DHCP so we set our own IP address
char remoteServer[] = "datel.asp2.cz";
EthernetClient localClient;

bool isDownloaded = false;
char buff[64];
int pointer = 0;
boolean found_status_200 = false;
boolean found_data_id = false;
char *found;
//String param="STATIONNAME";
//String param="CHECK";
String data;
unsigned int stampOld=0;
unsigned int stamp=INT_MAX;

int high_above_sea; //in cm
unsigned int savingInterval; //in sec
unsigned int postingInterval; //in sec

String nextAction="";

void setup()
{
  Serial.begin(115200); 
  
  pinMode(53, OUTPUT);
  digitalWrite(53,HIGH);

  Serial.println("Ethernet begin....");
  Ethernet.begin(mac);
  Serial.print("\nIP:");
  Serial.println(Ethernet.localIP());
  Serial.print("Mask:");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway:");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS:");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println();
  nextAction = "stamp";
}

void loop()
{
  checkConfig();
}

void checkConfig() {
  if (nextAction!="") {
    if (isDownloaded) {
      while (localClient.available()) {
        char c = localClient.read();
        //Serial.print(c);
        buff[pointer] = c;
        if (pointer < 64) pointer++;
        if (c == '\n') {
          found = strstr(buff, "200 OK");
          if (found != 0){
            found_status_200 = true; 
            #ifdef VERBOSE
            Serial.println("Status 200 found");
            #endif
          }
          buff[pointer]=0;
          //found_content = true;
          clean_buffer();    
        }
        
        if (found_status_200) {
          found = strstr(buff, "id=\"Data\">");
          if (found != 0) {
            #ifdef VERBOSE
            Serial.println("Data was found.");
            #endif
            clean_buffer();
            found_data_id = true; 
          }
        }
        
        if (found_data_id) {
          found = strstr(buff, "</span>");
          if (found != 0) {
            found_data_id = false;
            buff[strlen(buff)-7]='\0';
            data=buff;
            #ifdef VERBOSE
            Serial.print("Data:");
            Serial.println(data);
            Serial.print("Next action:");
            Serial.println(nextAction);
            #endif
            clean_buffer();
            localClient.stop();
            isDownloaded=false;
            
            if (nextAction=="stamp") {
              stamp = string2Int(data);
              #ifdef VERBOSE
              Serial.println("STAMP action");
              Serial.print("Stamp=");
              Serial.println(stamp);
              #endif
              
              if (stamp<INT_MAX && stamp!=0 && stamp!=stampOld) {
                stampOld=stamp;
                Serial.print("Stamp set/change STAMP=");
                Serial.println(data);
                nextAction = "high_above_sea";
              }
            }
            else if (nextAction=="high_above_sea") {
              high_above_sea = string2Int(data);
              nextAction = "savingInterval";
            }
            else if (nextAction=="savingInterval") {
              savingInterval = string2Int(data);
              nextAction = "postingInterval";
            }
            else if (nextAction=="postingInterval") {
              postingInterval = string2Int(data);
              nextAction = "";
            }
          }
        }
      }
    }
    else {
      #ifdef VERBOSE
      Serial.println("Connecting...");
      #endif
      if (localClient.connect(remoteServer, 80)) {
        #ifdef VERBOSE
        Serial.println("GET request to retrieve");
        #endif
        localClient.print("GET /getconfigdata.aspx?param=");
        localClient.print(nextAction);
        localClient.println(" HTTP/1.1");
        localClient.print("Host: ");
        localClient.println(remoteServer);
        localClient.println("User-Agent: Arduino (Solar)");
        localClient.println("Connection: close");
        localClient.println();
        found_status_200 = false;
        found_data_id = false;
        
        isDownloaded=true;


      }
      else
      {
        Serial.println("Connecting error.");
      }
    }
  }
}


void clean_buffer() {
  pointer = 0;
  memset(buff,0,sizeof(buff)); 
}


int string2Int(String d) {
  Serial.println("fce string2Int");
  Serial.println(d);
  char this_char[d.length() + 1];
  d.toCharArray(this_char, sizeof(this_char));
  Serial.println(this_char);
  return atoi(this_char);
 }
  
