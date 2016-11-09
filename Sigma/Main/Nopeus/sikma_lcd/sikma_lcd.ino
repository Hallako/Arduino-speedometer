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
//  TFTscreen.setTextSize(4);
}

void loop() {
  
  buttonState = digitalRead(buttonPin);


  // set the font color
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(255, 255, 255);

  TFTscreen.text(nopeus, 0, 20);
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(1024, 1000, 1024);
  TFTscreen.text(" KM/H\n", 30, 20);


  
  delay(250);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(kello, 60, 100);
}

