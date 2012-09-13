/*EEPROM
address   0      vyska MSB
          1      vyska LSB
          2-11   stationName
          12-61  apiKey
          62-67  feedId
          68-117 userAgent
        

*/
#define STATIONNAME   2
#define APIKEY        12
#define FEEDID        62
#define USERAGENT     68


//#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress ip(192,168,1, 60);

char buffer[100];
byte poziceBuffer=0;

unsigned int vyska=0;
char stationName[10+1];
char apiKey[50+1];
char feedId[6+1];
char userAgent[50+1];

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  readFromEEPROM(STATIONNAME, stationName, sizeof(stationName));
  Serial.print("Nazev stanice: ");
  Serial.println(stationName);

  vyska = EEPROM.read(0) * 256 + EEPROM.read(1);
  Serial.print("Vyska stanice v m.n.m.: ");
  Serial.println(vyska);

  readFromEEPROM(APIKEY, apiKey, sizeof(apiKey));
  Serial.print("Cosm API key: ");
  Serial.println(apiKey);

  readFromEEPROM(FEEDID, feedId, sizeof(feedId));
  Serial.print("Cosm feed ID: ");
  Serial.println(feedId);
  
  readFromEEPROM(USERAGENT, userAgent, sizeof(userAgent));
  Serial.print("Cosm user agent: ");
  Serial.println(userAgent);

  eepromDump();
  
  //Serial.println(sizeof(stationName)); 11
  //Serial.println(sizeof(vyska)); 2

}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    //Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    poziceBuffer=0;
    byte start=0;    
    char b[50];
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        
        //parsing and saving 2 EEPROM

        if (poziceBuffer<100) {
          buffer[poziceBuffer++]=c;
          if(poziceBuffer==5 && start==0) {
            if (buffer[0]=='G' && buffer[1]=='E' && buffer[2]=='T' && buffer[3]==' ' && buffer[4]=='/') { //start GET block
              poziceBuffer=0;
              start=1;
            }
          }
      
          if (start==1 && c=='&') { //end var
            memset(b, '\0', sizeof(b)-1);
            Serial.println("SAVE TO EEPROM");
            getParameter(b);
            if (strstr(buffer, "jmeno")>0) {
              Serial.print("JMENO=");
              memset(stationName, '\0', sizeof(stationName)-1);
              strncpy(stationName,b,sizeof(stationName)-1);
              save2EEPROM(stationName, STATIONNAME);
            }
            else if (strstr(buffer, "vyska")>0) {
              Serial.print("VYSKA=");
              vyska = strtol(b,NULL,10);
              EEPROM.write(0, vyska>>8);
              EEPROM.write(1, vyska&0xff);
            }
            else if (strstr(buffer, "apikey")>0) {
              Serial.print("API KEY=");
              memset(apiKey, '\0', sizeof(apiKey)-1);
              strncpy(apiKey,b,sizeof(apiKey)-1);
              save2EEPROM(apiKey, APIKEY);
            }
            else if (strstr(buffer, "feedid")>0) {
              Serial.print("FEED ID=");
              memset(feedId, '\0', sizeof(feedId)-1);
              strncpy(feedId,b,sizeof(feedId)-1);
              save2EEPROM(feedId, FEEDID);
            }
            else if (strstr(buffer, "useragent")>0) {
              Serial.print("USER AGENT=");
              memset(userAgent, '\0', sizeof(userAgent)-1);
              strncpy(userAgent,b,sizeof(userAgent)-1);
              save2EEPROM(userAgent, USERAGENT);
            }
            else {
              Serial.print("\nUNKNOWN PARAMETER!!!=");
            }
            
            //Serial.println(b);
            
            memset(buffer, '\0', 100);
            poziceBuffer=0;
          }
          //GET /?jmeno=datel&vyska=350&apikey=123&feedid=1234&useragent=456 HTTP/1.1

        }
        
        
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Meteo</title>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
//          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          client.println("<meta http-equiv=\"content-type\" content=\"text/html;charset=utf8\">");
          client.println("</head>");
          client.println("<body style=\"background-color: Wheat;)\">");
          client.println("<h1>METEO Setup</h1>");
          client.println("<form action=\"\" method=\"get\">");
          
          //read from EEPROM
          client.println("<label for=\"jmeno\">Zadej jmeno stanice:</label>");
          client.println("<input type=\"text\" name=\"jmeno\" size=\"10\" maxlength=\"10\" value=\"");
          client.print(stationName);
          client.print("\" >");
          client.print("<br/>");
          
          client.println("<label for=\"vyska\">Zadej nadmořskou výšku stanice:</label>");
          client.print("<input type=\"text\" name=\"vyska\" size=\"4\" maxlength=\"4\" value=\"");
          client.print(vyska);
          client.print("\" >");
          client.print("<br/>");
          
          //setup cosm client
          client.println("<label for=\"apikey\">API key:</label>");
          client.println("<input type=\"text\" name=\"apikey\" size=\"50\" maxlength=\"50\" value=\"");
          client.print(apiKey);
          client.print("\" >");
          client.print("<br/>");
          
          client.println("<label for=\"feedid\">Feed ID:</label>");
          client.println("<input type=\"text\" name=\"feedid\" size=\"6\" maxlength=\"6\" value=\"");
          client.print(feedId);
          client.print("\" >");
          client.print("<br/>");
          
          client.println("<label for=\"useragent\">User agent:</label>");
          client.println("<input type=\"text\" name=\"useragent\" size=\"50\" maxlength=\"50\" value=\"");
          client.print(userAgent);
          client.print("\" >");
          client.print("<br/>");

          client.println("<input type=\"hidden\" name=\"dummy\" >");
          
          client.println("<input type=\"submit\" value=\"Odeslat\">");
          client.println("</form>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //  Serial.println("client disonnected");
    eepromDump();

  }
}


void getParameter(char * b) {
  char * posStart=0;
  char * posEnd=0;
  posStart=strstr(buffer, "=");
  posEnd=strstr(buffer, "&");
  strncpy(b, posStart+1, posEnd-posStart-1);
  b[posEnd-posStart]='\0';
  Serial.print("getParameter:");
  Serial.println(b);
}

void readFromEEPROM(unsigned int aStart, char * var, unsigned int delka) {
  for (unsigned int i=0; i<delka-1; i++) {
    var[i] = EEPROM.read(i+aStart);
  }
  var[delka-1]='\0';
}

void save2EEPROM(char * varName, unsigned int aStart) {
  byte i=0;

  //Serial.println("save2EEPROM");
  Serial.print(varName);
  Serial.print("-");
  Serial.println(aStart);
  
  while (varName[i++]!='\0') {
    Serial.print(varName[i-1]);
    EEPROM.write(aStart++, varName[i-1]);
  }
  EEPROM.write(aStart, '\0');
//  Serial.println();
}

void eepromDump() {
  Serial.println("EEPROM DUMP");
  for (unsigned int i=0; i<150; i++) {
    Serial.print(i);
    Serial.print("\t");
    char asciiChar = static_cast<char>(EEPROM.read(i));
    Serial.print(asciiChar);
    Serial.println();
  }
}