#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);

// variables for the position of the ball and paddle
char nopeus;
int kello = 0;
unsigned long time;
const int buttonPin = 12;
int buttonState = 0;

int sensorPin = 2;
float start, tk=22;
float elapsed;
float revs;
float revolution;
char sensorPrintout[6];
void setup() {

  pinMode(buttonPin, INPUT);
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0,0,0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(400, 200, 1200);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("Nopeus:\n ", 0, 0);
  // ste the font size very large for the loop
 // TFTscreen.setTextSize(4);
  Serial.begin(9600);
  attachInterrupt(0, RPM, RISING);
  start=millis();
}

void RPM()
{
  int sensorValue = digitalRead(sensorPin);
  float revs = 60000/elapsed;
  float revolution = elapsed/1000;
  float kmh = ((tk*2.54*3.1459)*revs*60/100000);
  elapsed=millis()-start;
  start=millis();

/*  Serial.print(elapsed);
  Serial.print(" mS ");
  Serial.print(revolution);
  Serial.print("SEC ");
  Serial.print(revs);
  Serial.print("RPM ");*/  
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(sensorPrintout, 0, 20);
   String sensorVal = String(kmh);
   sensorVal.toCharArray(sensorPrintout, 6);
   TFTscreen.stroke(255, 255, 255);
   TFTscreen.text(sensorPrintout, 0, 20);
 Serial.print(sensorPrintout[0]);
 Serial.print(sensorPrintout[1]);
 Serial.print(sensorPrintout[2]);
 Serial.print(sensorPrintout[3]);
 Serial.print(sensorPrintout[4]);
 Serial.println(sensorPrintout[5]);
 Serial.println(kmh);
   delay(250);

}

void loop() {
  
  buttonState = digitalRead(buttonPin);

  TFTscreen.setTextSize(4);
  TFTscreen.stroke(255, 255, 255);

  TFTscreen.text(nopeus, 0, 20);
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(1024, 1000, 1024);
  TFTscreen.text(" KM/H\n", 30, 50);

  delay(250);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(kello, 60, 100);
}

