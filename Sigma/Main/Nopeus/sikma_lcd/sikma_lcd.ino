#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);

// variables for the position of the ball and paddle



const int buttonPin = 12;
int buttonState = 0;

int sensorPin = 2;
float start, tk=22;
float elapsed, time;
float revs;
float revolution;
char sensorPrintout[6];
char oldsensor[6];
String oldVal,sensorVal;
float kierrokset = 0;
float matka = 0;
String matkaVal;
char Matka[6];

void setup() {

  pinMode(buttonPin, INPUT);
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0,0,0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(1000, 950, 950);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("SIKMA ", 0, 0);
  // ste the font size very large for the loop
 // TFTscreen.setTextSize(4);
  Serial.begin(9600);
  attachInterrupt(0, RPM, RISING);
  start=millis();
}

void RPM()
{
  
  elapsed=millis()-start;
  start=millis();
    float revs = 60000/elapsed;
    float kmh = ((tk*2.54*3.1459)*revs*60/100000);
    sensorVal = String(kmh);
  kierrokset += 1;
  matka = kierrokset * (tk*2.54*3.1459)/100000;
/*  Serial.print(elapsed);
  Serial.print(" mS ");
  Serial.print(revolution);
  Serial.print("SEC ");
  Serial.print(revs);
  Serial.print("RPM ");*/  


}

void loop() {
  if(sensorVal != oldVal)
  {

    sensorVal.toCharArray(sensorPrintout, 6);
  
  TFTscreen.setTextSize(4);
 TFTscreen.stroke(0, 0, 0);
 TFTscreen.text(oldsensor, 0, 20);
  TFTscreen.stroke(0, 0, 0);
 TFTscreen.text(Matka, 0, 60);
 //  String sensorVal = String(kmh);
   //sensorVal.toCharArray(sensorPrintout, 6);
   TFTscreen.stroke(255, 255, 255);
   TFTscreen.text(sensorPrintout, 0, 20);
   
  //ottaa vanhan arvon talteen näytön tyhjennystä varten
   oldVal = String (sensorPrintout);
   oldVal.toCharArray(oldsensor, 6);

   matkaVal = String (matka);
   matkaVal.toCharArray(Matka, 6);
   TFTscreen.stroke(255, 255, 255);
   TFTscreen.text(Matka, 0, 60);
 
 Serial.print(sensorPrintout[0]);
 Serial.print(sensorPrintout[1]);
 Serial.print(sensorPrintout[2]);
 Serial.print(sensorPrintout[3]);
 Serial.print(sensorPrintout[4]);
 Serial.println(sensorPrintout[5]);
 Serial.print(Matka[0]);
 Serial.print(Matka[1]);
 Serial.print(Matka[2]);
 Serial.print(Matka[3]);
 Serial.println(Matka[4]);
 

  }


  /*
  buttonState = digitalRead(buttonPin);

  TFTscreen.setTextSize(4);
  TFTscreen.stroke(255, 255, 255);

  TFTscreen.text(nopeus, 0, 20);
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(1024, 1000, 1024);
  TFTscreen.text(" KM/H\n", 30, 50);*/


}

