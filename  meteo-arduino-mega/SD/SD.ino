#include <limits.h>

#include <SPI.h>
#include <SD.h>
const int chipSelect = 4;

File myFile;

void setup() {
  // start serial port:
  Serial.begin(9600);
  Serial.println("SD Test");
  
/*  if (Ethernet.begin(mac) == 0) {
	Serial.println("Failed using DHCP");
	// DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip);
  }
*/  
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);
 
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  }
  else {
    Serial.println("example.txt doesn't exist.");
    // open a new file and immediately close it:
    Serial.println("Creating example.txt...");
    myFile = SD.open("example.txt", FILE_WRITE);
    myFile.close();
  }


  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("Write to example.txt.");
    myFile = SD.open("example.txt", FILE_WRITE);
    if (myFile) {
      myFile.write("Sample data");
      myFile.flush();
      myFile.close();
      Serial.println("Data were written.");
    }
    else
      Serial.println("Open failed.");
  }
  else {
    Serial.println("example.txt doesn't exist.");  
  }
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("example.txt");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening example.txt");
  } 

//  // delete the file:
//  Serial.println("Removing example.txt...");
//  SD.remove("example.txt");
//
//  if (SD.exists("example.txt")){
//    Serial.println("example.txt exists.");
//  }
//  else {
//    Serial.println("example.txt doesn't exist.");  
//  }
  
}

void loop() {
  
}


