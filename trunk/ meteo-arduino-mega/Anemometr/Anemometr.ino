/*PINOUT

D0 Rx
D1 Tx
D2 free
D3 Wind speed
D4 free
D5-D7 free
D8 free
D9 free
D10 Ethernet shield
D11 free
D12
D13
A0 DALLAS temperature
A1 DHT sensor
A2 Wind direction
A3 free
A4 SDA for BMP
A5 SCL for BMP


*/

#include <FreqPeriodCounter.h>
 
const byte counterPin = 3; 
const byte counterInterrupt = 1; // = pin 3
FreqPeriodCounter counter(counterPin, micros, 0);

//-------------------------------------------------------------------------SETUP------------------------------------------------------------------------------
void setup() {
  // start serial port:
  Serial.begin(115200);
  Serial.println("SW inicialization");

  attachInterrupt(counterInterrupt, counterISR, CHANGE);
}

//-------------------------------------------------------------------------LOOP------------------------------------------------------------------------------

void loop() {
  int val=analogRead(3);
  Serial.print(val);
  Serial.print(" - ");
  Serial.print(calculateWindDirection(val));
  if(counter.ready()) {
    Serial.print("; ");
    Serial.println(counter.hertz());
  }
  else
  {
    Serial.println();
  }
  delay(500);
}

//calculate wind direction
String calculateWindDirection(int adcValue)
{
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

void counterISR()
{ counter.poll();
}
