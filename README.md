# Arduino Uno Sikma bike speedometer

We built bike speedometer as school project in group of two.

Device has four screens which are trip distance, training length, total distance and realtime clock.

Theres also settings menu where units and wheel size can be set and total distance reset or set.

Youtube showcase videos:

https://www.youtube.com/watch?v=SHsmYizxoB8

https://www.youtube.com/watch?v=4nt99oDK5W8

### Button
*Connecting pull-down resistor arduino pin to GND, one button output connects to both pins*

|   	| Arduino Pin 	|
| ---  	| ---  	|
| `BUTTON`| 5, 3 	|


### Hall Effect Switch ICs C1024 

*Schmitt trigger and open-collector output.*

*Sensor Vout pull-up resistor to Vcc.*

|   	| Arduino Pin 	|
| ---  	| ---   |
| `Vout` 	|  2 	|



### Arduino [Tft Screen](https://www.arduino.cc/en/Guide/TFT)

 *Screen GND pin switch IRF640N MOSFET.*

|   	| Arduino Pin 	|
| ---  	| ---       |
| `SCK` 		| 13   	|
| `MOSI`  	| 11    |
| `CS` 		  | 10    |
| `DC`    	| 9    	|
| `RST` 		| 8    	|

### RTC module [Adafruit DS1307](https://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/arduino-library)

|   	| Arduino Pin 	|
| ---  	| ---   |
| `SDA` 	| A4   	|
| `SCL`  	| A5    |

