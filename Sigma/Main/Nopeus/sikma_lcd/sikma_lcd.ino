#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);
unsigned long t,s,m,h,seconds,secondsoff,oldseconds;
int sensorPin = 5, v=1,del=0,mph = 1;
float start, tk=22, kierrokset = 0, matka = 0, revs, elapsed, time,matkat,matkar,matkaoff,matkaold;
char oldsensor[6], Matka[6], sensorPrintout[6], secc[4], mnc[4], hrc[4],MatkaT[6];
String oldVal,sensorVal,matkaVal,sec,minn,hou,matkaT;
char minuutit[10], sekunnit[10], tunnit[10];
char screenClear[10];

void setup() {

  TFTscreen.begin();
  tftSetup();
  Serial.begin(9600);
  attachInterrupt(0, RPM, RISING);
 // attachInterrupt(digitalPinToInterrupt(3), lisa, RISING);

  start=millis();
}

void RPM()
{
  
  elapsed=millis()-start;
  start=millis();
    float revs = 60000/elapsed;
    float kmh = ((tk*2.54*3.1459)*revs*60/100000);
    
  kierrokset += 1;
  matka = kierrokset * (tk*2.54*3.1459)/100000;
  if(mph==1)
  {
    kmh = kmh * 0.621371;
    matka = matka * 0.621371;
  }
  sensorVal = String(kmh);
}
/*
void lisa()
{	
}
*/

void reset()
{
	secondsoff = millis() / 1000;
	matkaoff = matka;
}
 
void loop() {
	
	
  matkar=matka-matkaoff;
  del=0;
 int f = 0;
  while(digitalRead(5) == HIGH)
  {
    del=del+1;
	delay(1);
 f=1;
  }
  if(del<500 && f==1){
	  v+=1;
	if(v>4){
	v=1;
  }
  }
  if(del>500 && f==1){
	secondsoff = millis() / 1000;
	matkaoff = matka;
  }
 
 seconds = (millis() / 1000)-secondsoff;  	//secondsoff = offset resetistä.

  if(sensorVal != oldVal)
  {
  sensorVal.toCharArray(sensorPrintout, 6);
  
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(oldsensor, 0, 20);

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
 if(mph == 1)
 {
	 TFTscreen.setTextSize(1);
    TFTscreen.stroke(1000, 1000, 1000);
  TFTscreen.text("MP/h", 130, 41);
 }
 else
 {
  TFTscreen.setTextSize(1);
    TFTscreen.stroke(1000, 1000, 1000);
  TFTscreen.text("KM/h", 130, 41);
 }
  }

	switch (v){
  
  
	case 1:
 if(roundf(matkar * 100) / 1 != roundf(matkaold * 100) / 1)
 {
  Serial.print(matkar);
  Serial.print("\t");
  Serial.print(matkaold);
  Serial.print("\t");
	
	TFTscreen.stroke(0, 0, 0);
  TFTscreen.setTextSize(1);
  TFTscreen.text("TOTAL", 60, 65);

  TFTscreen.setTextSize(2);
	TFTscreen.text(Matka, 0, 60);
  TFTscreen.text(tunnit, 0, 60);
  TFTscreen.text(MatkaT, 0, 60);
	//matkar=matka-matkaoff;
	
 // Serial.print(matkaold);
	matkaVal = String (matkar);
    matkaVal.toCharArray(Matka, 6);
    TFTscreen.stroke(255, 255, 255);

    TFTscreen.setTextSize(2);
    TFTscreen.text(Matka, 0, 60);
    matkaold=matkar;

    if(mph == 1)
    {
      TFTscreen.setTextSize(1);
      TFTscreen.text("miles TRIP", 60, 65);
    }
    else
    {
      TFTscreen.setTextSize(1);
      TFTscreen.text("km TRIP", 60, 65);
    }
	/*
	Serial.print(Matka[0]);
	Serial.print(Matka[1]);
	Serial.print(Matka[2]);
	Serial.print(Matka[3]);
	Serial.println(Matka[4]);*/
 Serial.println("CASE1");
 }
	break;
 
	case 2:
	
	if(seconds!=oldseconds){
 Serial.println("CASE2");
 TFTscreen.setTextSize(2);
 Serial.print(secondsoff);
 
	
	oldseconds=seconds;
  	t = seconds;					// h= hours m=minutes s=seconds 
    s = t % 60;
    t = (t - s)/60;
    m = t % 60;
    t = (t - m)/60;
    h = t;
TFTscreen.setTextSize(1);
  TFTscreen.stroke(0, 0, 0);
 TFTscreen.text("miles TRIP", 60, 65);
 TFTscreen.text("km TRIP", 60, 65);
 TFTscreen.setTextSize(2);
 TFTscreen.text(Matka, 0, 60);
 TFTscreen.text(tunnit, 0, 60);
 TFTscreen.text(MatkaT, 0, 60);
  
	ultoa(s,secc,10);						//unsigned long = string
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
  
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(tunnit, 0, 60);
	}
	break;
	
	case 3:
  TFTscreen.setTextSize(1);
  TFTscreen.text("TOTAL", 60, 65);
	TFTscreen.setTextSize(2);
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.text(Matka, 0, 60);
 TFTscreen.text(tunnit, 0, 60);
 TFTscreen.text(MatkaT, 0, 60);
 
	matkat=matka;
	matkaT = String (matkat);
    matkaT.toCharArray(MatkaT, 6);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text(MatkaT, 0, 60);
   
	
	
	Serial.print(MatkaT[0]);
	Serial.print(MatkaT[1]);
	Serial.print(MatkaT[2]);
	Serial.print(MatkaT[3]);
	Serial.println(MatkaT[4]);
	Serial.println("CASE3");
 
	delay(1000);
	break;
}
  }
  void tftSetup()
  {
    // clear the screen with a black background
  TFTscreen.background(0,0,0);
  TFTscreen.stroke(1000, 200, 200);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("SIKMA ", 0, 0);
  TFTscreen.line(0, 55, 160, 55);    
  }



