# Meteo station on arduino #


## Project goals ##
Air Pressure
Humidity
Temperatures
Wind direction and speed
Voltage
Current

### Hardware ###
#### Arduino Mega 2560 ####
#### Display ####
http://www.dfrobot.com/wiki/index.php?title=Arduino_LCD_KeyPad_Shield_%28SKU:_DFR0009%29<br>
Schema - <a href='http://www.dfrobot.com/image/data/DFR0009/LCDKeypad%20Shield%20V1.0%20SCH.pdf'>http://www.dfrobot.com/image/data/DFR0009/LCDKeypad%20Shield%20V1.0%20SCH.pdf</a>

use pin - A0 (for keys, not used)<br>
D4 - this pin is switched to pin 11 on screw board<br>
D5-D9,D10 (for backlight is not used yet). Backlight is driven by potentiometer. Pin 10 is not connected to Arduino board.<br>
<br>
<h4>Ethernet shield</h4>
<a href='http://arduino.cc/en/Main/ArduinoEthernetShield'>http://arduino.cc/en/Main/ArduinoEthernetShield</a><br>

use pin - 10 SS for Ethernet card<br>
D4 - SS for SD card<br>
ISP (MISO, MOSI, SCK)<br>
<br>
<h4>Screw shield</h4>
<a href='http://www.robotshop.com/dfrobot-arduino-compatible-screw-shield-4.html'>http://www.robotshop.com/dfrobot-arduino-compatible-screw-shield-4.html</a>

<h4>Sensors</h4>
BMP085 - air pressure, air temperature<br>
DHT11 - humidity and temperature<br>
DS18B20 - temperature<br.<br>
<br>
<h4>Pinout</h4>

D0 Rx<br>
D1 Tx<br>
D2 DHT sensor <br>
D3 DALLAS I2C<br>
D4 SS for SD card<br>
D5-D9 display<br>
D10 Ethernet shield<br>
D11 display<br>
D12<br>
D13<br>
A0 free reserved for rain sensor<br>
A1 free reserved for anemometer (current sensor Holcik)<br>
A2 free reserved for anemometer (voltage sensor Holcik)<br>
A3 free reserved for light sensor<br>
A4-SDA for BMP085 ATMEGA328<br>
A5-SCL for BMP085 ATMEGA328<br>

<pre><code>digital<br>
  13 12 11 10  9  8  7  6  5  4  3  2  1  0 <br>
|-------------------------------------------|------------|<br>
|                                           |            |<br>
|                                           |            |<br>
|           Arduino MEGA 2560               |            |<br>
|                                           |            |<br>
|                                           |            |<br>
|                                           |            |<br>
|                                           |            |<br>
|-------------------------------------------|------------|<br>
                           analog          <br>
                           0  1  2  3  4  5<br>
</code></pre>