#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);
<<<<<<< HEAD
unsigned int s,m,h,t,seconds,secondsoff;
const int buttonPin = 12;
int buttonState = 0, sensorPin = 2, v=1;
float start, tk=22, kierrokset = 0, matka = 0, revs, elapsed, time, kmh;
char oldsensor[6], Matka[6], sensorPrintout[6], secc[6], Matkakok[6];
String oldVal,sensorVal,matkaVal,sec,minu,hou,matkak;
=======
unsigned long t,s,m,h,seconds,secondsoff;
const int buttonPin = 12;
int buttonState = 0, sensorPin = 2, v=1;
float start, tk=22, kierrokset = 0, matka = 0, revs, elapsed, time;
char oldsensor[6], Matka[6], sensorPrintout[6], secc[6], matkakok[6];
String oldVal,sensorVal,matkaVal,sec,min,hou,matkak;
>>>>>>> Lauri

void setup() {

  pinMode(buttonPin, INPUT);
  TFTscreen.begin();
  TFTscreen.background(0,0,0);
  TFTscreen.stroke(1000, 950, 950);

  TFTscreen.setTextSize(2);
  TFTscreen.text("SIKMA ", 0, 0);
<<<<<<< HEAD

=======
  // ste the font size very large for the loop
  // TFTscreen.setTextSize(4);
>>>>>>> Lauri
  Serial.begin(9600);
  attachInterrupt(0, RPM, RISING);
  attachInterrupt(digitalPinToInterrupt(3), lisa, RISING);
  start=millis();
}

void RPM()
{
  
  elapsed=millis()-start;
  start=millis();
  
  revs = 60000/elapsed;
  kmh = ((tk*2.54*3.1459)*revs*60/100000);
  sensorVal = String(kmh);
  
  kierrokset += 1;
  matka = kierrokset * (tk*2.54*3.1459)/100000;
}
<<<<<<< HEAD

void lisa()
{	v+=1;
	if(v==4){
	v=1;
}
}

void reset()
{
	seconds=secondsoff;
	
=======

void lisa()
{	v+=1;
	if(v>4){
	v=1;
}
}
void reset()
{
	seconds=secondsoff;
	matkaoff = matkaVal;
>>>>>>> Lauri
	
}


void loop() {

<<<<<<< HEAD
=======


>>>>>>> Lauri
  if(sensorVal != oldVal)
  {
  sensorVal.toCharArray(sensorPrintout, 6);
  
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(oldsensor, 0, 20);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(sensorPrintout, 0, 20);
   
<<<<<<< HEAD
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
=======
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
  
>>>>>>> Lauri
	case 1:
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.text(Matka, 0, 60);
	
	matkaVal = String (matka);
<<<<<<< HEAD
=======
	matkaVal = MatkaVal-matkaoff
>>>>>>> Lauri
    matkaVal.toCharArray(Matka, 6);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(Matka, 0, 60);
 
	Serial.print(Matka[0]);
	Serial.print(Matka[1]);
	Serial.print(Matka[2]);
	Serial.print(Matka[3]);
	Serial.println(Matka[4]);
<<<<<<< HEAD
	break;
	
	case 2:
	seconds = (millis()/1000)-secondsoff; 	//secondsoff = offset resetistä.
	t = seconds;							// h= hours m=minutes s=seconds 
=======
	Serial.println("CASE1");
	delay(1000);
	break;
	
	case 2:
	Serial.println("CASE2");
	seconds = (millis()-secondsoff) / 1000; 	//secondsoff = offset resetistä.
  	t = seconds;								// h= hours m=minutes s=seconds 
>>>>>>> Lauri
    s = t % 60;
    t = (t - s)/60;
    m = t % 60;
    t = (t - m)/60;
    h = t;
	
	TFTscreen.stroke(0, 0, 0);
<<<<<<< HEAD
    TFTscreen.text(secc, 0, 60);
	ultoa(s,secc,10);						//long = chart
	
	sec.toCharArray(secc,6);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(secc, 0, 60);
	delay(1001);
	
=======
	TFTscreen.text(secc, 0, 60);
  
	ultoa(s,secc,10);						//long = string
	
	Serial.println(secc);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(secc, 0, 60);
	delay(1000);
>>>>>>> Lauri
	break;
	
	case 3:
	
	TFTscreen.stroke(0, 0, 0);
<<<<<<< HEAD
	TFTscreen.text(Matka, 0, 60);
=======
	TFTscreen.text(Matkakok, 0, 60);
>>>>>>> Lauri
	
	matkak = String (matka);
    matkak.toCharArray(Matkakok, 6);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(Matkakok, 0, 60);
<<<<<<< HEAD
 
	
	
	
	
	
	
	
	
	
}
}
=======
	delay(500);
	break;
}
}



>>>>>>> Lauri

