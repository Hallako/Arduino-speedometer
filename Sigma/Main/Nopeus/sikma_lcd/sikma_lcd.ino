#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <EEPROM.h>
#include <MsTimer2.h>

#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);
unsigned long t,s,m,h,seconds,secondsoff,oldseconds;
int sensorPin = 5, v=1,del=0,mph,fi,tk,otk,f,kierrokset = 0,ekierrokset,ca=1,screenFlag=0,ex=0,skip=0;
float start, matka = 0, revs, elapsed,ematka, time,matkat,matkar,matkaoff,matkaold,kmh,huippu=0,matkav,vert1,vert2;
char oldsensor[6], Matka[6], sensorPrintout[6], secc[4], mnc[4], hrc[4],MatkaT[6],Huippu[6]={0},sotk[5],stk[5];
String oldVal,sensorVal,matkaVal,sec,minn,hou,matkaT,hUippu,Sotk,Stk;
char minuutit[10], sekunnit[10], tunnit[10];
int muuttuja;


void setup() {
  TFTscreen.begin();
  tftSetup();
  Serial.begin(9600);
 /* 
 ADCSRB &= ~(1 << ACME);
  ACSR = B00011011;
	ACSR |= (1 << ACIS1) ; 
	ACSR |=(1 << ACIS0); 		//Analog comparaattorin alustus
   ACSR |=(0 << ACD)   ;				
   ACSR |=(0 << ACBG)  ;      
   ACSR |= (0 << ACI)   ;    
  ACSR |= (1 << ACIE)  ;   
  ACSR |= (0 << ACIC)  ; 


//bitSet(0x30, 0);
//bitSet(0x30, 1);

bitSet(0x30, 1);
bitSet(0x30, 2);
bitSet(0x30, 3);
bitSet(0x30, 4);
bitSet(0x30, 5);
bitSet(0x30, 6);
bitSet(0x30, 7);
  
   ACSR &= ~_BV(ACD);
   ACSR &= ~_BV(ACBG);
   ACSR &= ~_BV(ACO);
   ACSR |= _BV(ACI);
   ACSR |= _BV(ACIE);
   ACSR &= ~_BV(ACIC);
   ACSR &= ~_BV(ACIS1);
   ACSR &= ~_BV(ACIS0);
   */ 
  start=millis();
  ekierrokset = sizeof(int);
  mph = sizeof(int);
  tk = sizeof(int);
  EEPROM.get(120, mph);
  EEPROM.get(140, tk);
  EEPROM.get(100, ekierrokset);
  ematka = ekierrokset * (tk*2.54*3.1459)/100000;
  matkat=ematka;
  MsTimer2::set(3000, nollaus); 
  MsTimer2::start();
  screenFlag=1;
  attachInterrupt(digitalPinToInterrupt(2), trig, FALLING);
  pinMode(2,INPUT);
  pinMode(5,INPUT);
}

void trig() {
	//ACSR|=(0 << ACIE);
	noInterrupts();
  fi=1;
  elapsed=millis()-start;
  start=millis();
    float revs = 60000/elapsed;
    kmh = ((tk*2.54*3.1459)*revs*60/100000);
  kierrokset += 1;
  ekierrokset += 1;
  matka = kierrokset * (tk*2.54*3.1459)/100000;
  ematka = ekierrokset * (tk*2.54*3.1459)/100000;
 if(mph==1)
  {
    kmh = kmh * 0.621371;
    matka = matka * 0.621371;
  }
  sensorVal = String(kmh);
Serial.println("trig");
 // ACSR |= (1 << ACI);
 // ACSR |= (1 << ACIE);
}

void sleepNow()
{
	attachInterrupt(0, pinInterrupt, LOW);
	delay(100);
    set_sleep_mode(SLEEP_PWR_DOWN);
 
    // Set sleep enable (SE) bit:
    sleep_enable();
 
    // Put the device to sleep:
	digitalWrite(5,LOW);
    sleep_mode();
 
    // Upon waking up, sketch continues from this point.
    sleep_disable();
	digitalWrite(5,HIGH);
}
void reset()
{
	secondsoff = millis() / 1000;
	matkaoff = matka;
}

void nollaus()
{
	vert1=kmh;
	if(vert1==vert2){
	kmh=0;	
	sensorVal = String(kmh);
	}
	vert2=kmh;
} 
void loop() 
{
  if(fi==1){
	  
  delay(80);
  EIMSK |= (0 << INT0);
  interrupts();
  fi=0;
  }
  
  
  matkar=matka-matkaoff;
  del=0;
  f = 0;
  while(digitalRead(5) == HIGH)
  {
    del=del+1;
	delay(1);
    f=1;
	ex=0;
  }
  if(del<500 && f==1){
	  v+=1;
	  screenFlag=1;
	if(v>4){
	v=1;
	
  }
  }
  if(del>500 && f==1 && del<5000){
	secondsoff = millis() / 1000;
	matkaoff = matka;
	huippu=0;
  }
  if(del>5000 && f==1){
	for(;ex!=1;){
	del=0;
	f=0;
		while(digitalRead(5) == HIGH)
		{
		del+=1;
		f=1;
		delay(1);
		}
		if(del<500 && f==1){
		ca+=1;
		if(ca==4){
		ca=1;
		}
		}
		if(del>500 && f==1){
			switch(ca){
				case 1:
					if(mph==0){
					mph=1;	
					}
					else{
					mph=0;
					}
					Serial.println("c1");
					break;
				case 2:
					otk=tk;
					tk+=2;
					if(tk>30){
					tk=20;
					}
					Serial.println("c2");
					break;
				case 3:
				EEPROM.put(120, mph);
				EEPROM.put(140, tk);
				Serial.println("c3");
				TFTscreen.setTextSize(2);
				TFTscreen.stroke(0, 0, 0);
				TFTscreen.line(110, 120, 160, 120);
				TFTscreen.line(65, 120, 89, 120);
				TFTscreen.line(0, 120, 48, 120);
				TFTscreen.text("KM/h", 130, 41);
				TFTscreen.text(stk, 65, 100);
				TFTscreen.text("MP/h", 0, 100);
				TFTscreen.text("KM/h", 0, 100);
				TFTscreen.text("exit", 110, 100);
				TFTscreen.stroke(1000, 1000, 1000);
				TFTscreen.text("saved", 30, 100);
				delay(1500);
				TFTscreen.stroke(0, 0, 0);
				TFTscreen.text("saved", 30, 100);
				
				ca=1;				
				ex=1;
				skip=1;
				screenFlag=1;
				break;
				}
				}
				if(skip==0){
					if(mph==1){
					TFTscreen.setTextSize(2);
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.text("KM/h", 0, 100);	
					TFTscreen.setTextSize(2);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.text("MP/h", 0, 100);
					Serial.println("ifmph");				
					}		
					
					if(mph==0){
					TFTscreen.setTextSize(2);
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.text("MP/h", 0, 100);	
					TFTscreen.setTextSize(2);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.text("KM/h", 0, 100);
					Serial.println("ifkmh");					
					}
					Sotk = String (otk);
					Sotk.toCharArray(sotk, 5);
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.text(sotk, 65, 100);
					
					Stk = String (tk);
					Stk.toCharArray(stk, 5);
					TFTscreen.stroke(255, 255, 255);
					TFTscreen.text(stk, 65, 100);
					Serial.println("loop");
					Serial.println(mph);
					
					TFTscreen.setTextSize(2);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.text("exit", 110, 100);
					
					if(ca==1){
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.line(110, 120, 160, 120);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.line(0, 120, 48, 120);
					}
					if(ca==2){
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.line(0, 120, 48, 120);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.line(65, 120, 89, 120);
					}
					if(ca==3){
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.line(65, 120, 89, 120);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.line(110, 120, 160, 120);
					}
					delay(100);
				}
				
				if(skip==1){
				if(mph==1){
					TFTscreen.setTextSize(1);
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.text("KM/h", 130, 41);
					TFTscreen.setTextSize(1);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.text("MP/h", 130, 41);
				}
				if(mph==0){	
					TFTscreen.setTextSize(1);
					TFTscreen.stroke(0, 0, 0);
					TFTscreen.text("MP/h", 130, 41);
					TFTscreen.setTextSize(1);
					TFTscreen.stroke(1000, 1000, 1000);
					TFTscreen.text("KM/h", 130, 41);
					
				}
				
				}
				skip=0;
		}	
  }
  
  seconds = (millis() / 1000)-secondsoff;  	//secondsoff = offset resetistä.
 
  if(sensorVal != oldVal)
  {
	  noInterrupts();
  sensorVal.toCharArray(sensorPrintout, 6);
  TFTscreen.setTextSize(4);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(oldsensor, 0, 20);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(sensorPrintout, 0, 20);
	
//ottaa vanhan arvon talteen näytön tyhjennystä varten
   oldVal = sensorVal;
   oldVal.toCharArray(oldsensor, 6);
	

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
 interrupts();
  }
  
  if(huippu < kmh && huippu <1000)
  {
    TFTscreen.setTextSize(1);
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.text(Huippu, 130, 0); 
    huippu = kmh;
    hUippu = String (huippu);
    hUippu.toCharArray(Huippu, 6);
    TFTscreen.stroke(1000, 1000, 1000);
    TFTscreen.text(Huippu, 130, 0);
  } 
  if(roundf((matkat+0.2) * 100) < roundf(ematka * 100)){ //tallennetaan arvo eepromiin 200m välein
	EEPROM.put(100, ekierrokset);
	Serial.print("eeprom.write");
	matkat=ematka;
  }
	switch (v){
	case 1:
	if(screenFlag == 1)
	{
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.setTextSize(1);
		TFTscreen.text("TOTAL", 70, 65);
		TFTscreen.setTextSize(2);
		TFTscreen.text(MatkaT, 0, 60);
		
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.setTextSize(2);
		TFTscreen.text(Matka, 0, 60);
		screenFlag = 0;
		
	if(mph == 1)	
    {
	  TFTscreen.setTextSize(1);
	  TFTscreen.stroke(0, 0, 0);
      TFTscreen.text("km TRIP", 60, 65);
      TFTscreen.stroke(255, 255, 255);
      TFTscreen.text("miles TRIP", 60, 65);
    }
    if(mph == 0)
    { 
      TFTscreen.setTextSize(1);
	  TFTscreen.stroke(0, 0, 0);
	  TFTscreen.text("miles TRIP", 60, 65);
      TFTscreen.stroke(255, 255, 255);
      TFTscreen.text("km TRIP", 60, 65);
    }
	}
	
	
  if(roundf(matkar * 100) / 1 != roundf(matkaold * 100) / 1)
  {
	Serial.print(matkar);
	Serial.print("\t");
	Serial.print(matkaold);
	Serial.print("\t");
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.setTextSize(2);
	TFTscreen.text(Matka, 0, 60);

	matkaVal = String (matkar);
    matkaVal.toCharArray(Matka, 6);
    TFTscreen.stroke(255, 255, 255);

    TFTscreen.setTextSize(2);
    TFTscreen.text(Matka, 0, 60);
    matkaold=matkar;
	}
 Serial.println("CASE1");
 
	break;
 
	case 2:
	if(screenFlag == 1)
	{		if(mph == 1)
			{	
				TFTscreen.stroke(0, 0, 0);
				TFTscreen.setTextSize(1);
				TFTscreen.text("miles TRIP", 60, 65);
			}
			else
			{
				TFTscreen.stroke(0, 0, 0);
				TFTscreen.setTextSize(1);
				TFTscreen.text("km TRIP", 60, 65);
			}
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.setTextSize(2);
		TFTscreen.text(Matka, 0, 60);
		
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.setTextSize(2);
		TFTscreen.text(tunnit, 0, 60);
		screenFlag = 0;
	}
	if(seconds!=oldseconds){
 Serial.println("CASE2");
 TFTscreen.setTextSize(2);
 
	
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
	Serial.println(bitRead(0x30, 0));
Serial.println(bitRead(0x30, 1));
Serial.println(bitRead(0x30, 2));
Serial.println(bitRead(0x30, 3));
Serial.println(bitRead(0x30, 4));
Serial.println(bitRead(0x30, 5));
Serial.println(bitRead(0x30, 6));
Serial.println(bitRead(0x30, 7));
	}
	break;
	
	case 3:
    if(screenFlag == 1)
	{	
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.setTextSize(2);
		TFTscreen.text(tunnit, 0, 60);
		
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.setTextSize(2);
		TFTscreen.text(MatkaT, 0, 60);
		TFTscreen.setTextSize(1);
		TFTscreen.text("TOTAL", 70, 65);
		screenFlag = 0;
	}
  if(roundf(matkav * 100) != roundf(ematka * 100))
  {
  TFTscreen.setTextSize(2);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(Matka, 0, 60);
  TFTscreen.text(tunnit, 0, 60);
  TFTscreen.text(MatkaT, 0, 60);
 
  matkav = ematka;
  matkaT = String (ematka);
  matkaT.toCharArray(MatkaT, 6);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(MatkaT, 0, 60);
  }
  TFTscreen.setTextSize(1);
  TFTscreen.text("TOTAL", 70, 65);
  Serial.println("CASE3");
  break;
  
  case 4:
  
  
  Serial.println("CASE4");
  delay(100);
  break;
}
}

  void tftSetup()
  {
  TFTscreen.background(0,0,0);
  TFTscreen.stroke(1000, 200, 200);
  TFTscreen.setTextSize(2);
  TFTscreen.text("SIKMA ", 0, 0);
  TFTscreen.line(0, 55, 160, 55);    
  }



