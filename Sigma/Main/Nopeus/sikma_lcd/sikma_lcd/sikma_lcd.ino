#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);
unsigned long t,s,m,h,seconds,secondsoff;
const int buttonPin = 12, inputPin=3;
int buttonState = 0, sensorPin = 2, v=1, button_delay=0,button=3,status_button=0;
float start, tk=22, kierrokset = 0, matka = 0, revs, elapsed, time,matkat;
char oldsensor[6], Matka[6], sensorPrintout[6], secc[4], mnc[4], hrc[4], minuutit[6], sekunnit[6], tunnit[6];;
String oldVal,sensorVal,matkaVal,sec,minn,hou;



void setup() {

  pinMode(sensorPin, INPUT);
  pinMode(5, INPUT);
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
  TFTscreen.line(0, 55, 160, 55);

  Serial.begin(9600);
  attachInterrupt(0, RPM, RISING);
  attachInterrupt(digitalPinToInterrupt(3), lisa, RISING);
  attachInterrupt(digitalPinToInterrupt(5), Reset, RISING);
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
}

void lisa()
{	
  v+=1;
  if(v>4){
  v=1;
  
}
}
/*void Reset()
{
secondsoff+=seconds;
matkat+=matka;
}
void loop() {


*/
  if(sensorVal != oldVal)
  {
  sensorVal.toCharArray(sensorPrintout, 6);
  
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(oldsensor, 0, 20);
	//TFTscreen.stroke(0, 0, 0);
	//TFTscreen.text(Matka, 0, 60);
//String sensorVal = String(kmh);
//sensorVal.toCharArray(sensorPrintout, 6);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(sensorPrintout, 0, 20);
   
//ottaa vanhan arvon talteen näytön tyhjennystä varten
   oldVal = String (sensorPrintout);
   oldVal.toCharArray(oldsensor, 6);


 
 Serial.print(sensorPrintout[0]);
 Serial.print(sensorPrintout[1]);
 Serial.print(sensorPrintout[2]);
 Serial.print(sensorPrintout[3]);
 Serial.print(sensorPrintout[4]);
 Serial.println(sensorPrintout[5]);
 
	
  }

	switch (v){
  
	case 1:
 TFTscreen.setTextSize(2);
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.text(Matka, 0, 60);
 TFTscreen.text(tunnit, 0, 60);
	matkat=matka-matkat;
	matkaVal = String (matkat);
    matkaVal.toCharArray(Matka, 6);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(Matka, 0, 60);

	
	
	Serial.print(Matka[0]);
	Serial.print(Matka[1]);
	Serial.print(Matka[2]);
	Serial.print(Matka[3]);
	Serial.println(Matka[4]);
 Serial.println("CASE1");
 delay(1000);
	break;
	
	case 2:
 Serial.println("CASE2");
 TFTscreen.setTextSize(2);
	seconds = (millis() / 1000)-secondsoff; 	//secondsoff = offset resetistä.
  	t = seconds;					// h= hours m=minutes s=seconds 
    s = t % 60;
    t = (t - s)/60;
    m = t % 60;
    t = (t - m)/60;
    h = t;
	
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(tunnit, 0, 60);
  TFTscreen.text(Matka, 0, 60);
  
	ultoa(s,secc,10);						//long = string
	ultoa(m,mnc,10);
  ultoa(h,hrc,10);
	Serial.println(secc);
  Serial.println(mnc);

  strcpy(tunnit, hrc);
  strcat(tunnit, ":");
  strcpy(minuutit, mnc);
  strcat(minuutit, ":");
  strcat(tunnit, minuutit);
  strcpy(sekunnit, secc);
  strcat(tunnit, sekunnit);
  Serial.println(tunnit);
	//sec.toCharArray(secc,6);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(tunnit, 0, 60);
    //TFTscreen.text(secc, 20, 60);
    TFTscreen.line(0, 55, 160, 55);
    

	delay(1000);
}
}
	
  /*
  buttonState = digitalRead(buttonPin);

  TFTscreen.setTextSize(4);
  TFTscreen.stroke(255, 255, 255);

  TFTscreen.text(nopeus, 0, 20);
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(1024, 1000, 1024);
  TFTscreen.text(" KM/H\n", 30, 50); */




