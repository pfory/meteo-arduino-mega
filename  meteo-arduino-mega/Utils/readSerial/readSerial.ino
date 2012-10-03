#include <EEPROM.h>

int incomingByte = 0;   // for incoming serial data
char buffer[30];
byte poziceBuffer=0;

unsigned int offsetIPAddress=0;
byte lengthIPAddress=15;
byte poziceOffset=0;

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
}

void loop() {

        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();

                // say what you got:
//                Serial.print(poziceBuffer);
//                Serial.print("I received: ");
//                Serial.println(incomingByte, DEC);
                buffer[poziceBuffer++]=incomingByte;
                if (incomingByte==10) {
                  buffer[poziceBuffer-1]='\0';
                  Serial.println(buffer);
                  if (buffer[0]=='A' && buffer[1]=='T') {
                    if (buffer[2]=='I' && buffer[3]=='P') {
                      if (buffer[4]=='=') {
                        //set IP address
                        poziceBuffer=5;
                        Serial.print("IP:");
                        for (poziceOffset=0; poziceOffset<lengthIPAddress; poziceOffset++) {
                          EEPROM.write(poziceOffset, buffer[poziceBuffer]);
                          Serial.print(buffer[poziceBuffer++]);
                        }
                        Serial.println("<OK>");
                      }
                      if (buffer[4]=='?') {
                        Serial.print("IP:");
                        for (poziceOffset=0; poziceOffset<lengthIPAddress; poziceOffset++) {
                          Serial.print(char(EEPROM.read(offsetIPAddress+poziceOffset)));
                        }
                        Serial.println("<OK>");
                      }
                    } else {
                      Serial.println("Not recocnized");
                    }
                  } else {
                    Serial.println("Bad command");
                  }
                  memset(buffer,'\0',30);
                  poziceBuffer=0;
                }
        }
}
