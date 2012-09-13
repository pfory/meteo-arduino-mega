/*
  SD card basic file example
 
 This example shows how to create and destroy an SD card file 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 */
#include <SD.h>

File myFile;
int incomingByte = 0;   // for incoming serial data
char buffer[300];
byte poziceBuffer=0;

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(10, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void loop()
{

  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    /*Serial.print(poziceBuffer);
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);*/
    buffer[poziceBuffer++]=incomingByte;
    if (incomingByte==10) {
      buffer[poziceBuffer-1]='\0';
      Serial.println(buffer);
      if (buffer[0]=='A' && buffer[1]=='T') {
        if (buffer[2]=='R') {
          myFile = SD.open("20120820.txt", FILE_READ);
          // read from the file until there's nothing else in it:
          while (myFile.available()) {
    	    Serial.write(myFile.read());
          }
          myFile.close();
          
          Serial.println("<OK>");
        }
        if (buffer[2]=='W') {
          myFile = SD.open("20120820.txt", FILE_WRITE);
          poziceBuffer=3;
          while (buffer[poziceBuffer++]!='\0') {
            myFile.write(buffer[poziceBuffer-1]);
          }
          myFile.write("\n");
          
          myFile.close();
          
          Serial.println("<OK>");
        }

      }
      memset(buffer,'\0',30);
      poziceBuffer=0;
    }
  }


  // Check to see if the file exists: 
  if (!SD.exists("20120820.txt")) {
    myFile = SD.open("20120820.txt", FILE_WRITE);
  }
  myFile.close();
}


