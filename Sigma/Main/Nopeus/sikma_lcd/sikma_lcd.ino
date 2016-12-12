/*
*		SIKMA
*
*The circuit
*	Button	
*		BUTTON = pin 5, pin 3
*
*	Hall Effect Switch ICs C1024 
*		Schmitt trigger and open-collector output. 
*		Sensor Vout pull-up resistor to Vcc.
*		Vout 	= pin 2
*
*	Arduino Tft Screen
*		Screen GND pin switch IRF640N MOSFET.
*		SCK 	= pin 13
*		MOSI 	= pin 11
*		CS 		= pin 10
*		DC 		= pin 9
*		RST 	= pin 8
*
*	Adafruit DS1307 RTC
*		SDA 	= pin A4
*		SCL		= pin A5
*/
#include <TFT.h>  //Arduino LCD library
#include <SPI.h>
#include <EEPROM.h>
#include <MsTimer2.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <Wire.h>
#include "RTClib.h"

#define cs   10
#define dc   9
#define rst  8

RTC_DS1307 rtc;

TFT TFTscreen = TFT(cs, dc, rst);
unsigned long t,s,m,h,seconds,secondsoff,oldseconds,ekierrokset;
int sensorPin = 5,sleepFlag, Case=1,del=0,mph,delayflag,cas,tuumakoko,oldtuumakoko,
buttonflag,kierrokset = 0,Casesetup=1,screenFlag=0,exitflag=0,skip=0,oldS=0;
float start, matka = 0, revs, elapsed,ematka, time,matkat,matkar,matkaoff,matkaold,kmh,huippu=0,matkav,vert1,vert2,temp;
char oldsensor[6], Matka[7], sensorPrintout[6], secc[4], mnc[4], hrc[4],MatkaT[7],Huippu[6]={0},VAL[7],
sotk[5],stk[5],minuutit[10], sekunnit[10], tunnit[10], H[4], M[4], S[4], tunniT[10], minuutiT[10], sekunniT[10],RTC[10];
String oldVal,sensorVal,matkaVal,sec,minn,hou,matkaT,hUippu,Sotk,Stk,VAK;

void setup() {
	Serial.begin(9600);
	//Setup pins.
	pinMode(7,OUTPUT);
	digitalWrite(7,HIGH);
	
	//Screen setup (spi.begin).
	TFTscreen.begin();									
	tftSetup();
	
	//Set Size for variables.
	ekierrokset = sizeof(unsigned long);							
	mph = sizeof(int);
	tuumakoko = sizeof(int);	
	
	//Get saved data from eeprom.
	EEPROM.get(120, mph);											
	EEPROM.get(140, tuumakoko);
	EEPROM.get(160, ekierrokset);
	ematka = ekierrokset * (tuumakoko*2.54*3.1459)/100000;
	matkat=ematka;
	
	//Setup timer for interrupt.
	MsTimer2::set(4000, nollaus); 
	MsTimer2::start();
	screenFlag=1;
	start=millis();
	
	//Set interrupts.
	attachInterrupt(digitalPinToInterrupt(2), trig, FALLING);		
	attachInterrupt(digitalPinToInterrupt(3), wakeUp, RISING);		
	
 if (! rtc.begin()) {
    while (1);
  }

  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 			//uncomment to set time to rtc.
  }
}

//Interrupt from hall-sensor where distance and speed is calculated.
void trig() {														
	noInterrupts();
	//Set a flag and disable interrupts to avoid multiple interrupts.
	delayflag=1;
	sleepFlag=0;
	elapsed=millis()-start;
	start=millis();
	float revs = 60000/elapsed;
	kmh = ((tuumakoko*2.54*3.1459)*revs*60/100000);
	kierrokset += 1;
	ekierrokset += 1;
	matka = kierrokset * (tuumakoko*2.54*3.1459)/100000;
	ematka = ekierrokset * (tuumakoko*2.54*3.1459)/100000;
	
	//If mph selected convert to kmh to mph.
	if(mph==1)										
	{
		kmh = kmh * 0.621371;
		matka = matka * 0.621371;
	}
	sensorVal = String(kmh);
}

//Button press interrupt resets sleep counter.
void wakeUp()				
{
	sleepFlag=0;
}

//Screen setup function draws first case on screen.
void tftSetup()			
	{
	Case=1;
	TFTscreen.background(0,0,0);
	TFTscreen.stroke(1000, 200, 200);
	TFTscreen.setTextSize(2);
	TFTscreen.text("SIKMA ", 0, 0);
	TFTscreen.line(0, 55, 160, 55);   
	matkaVal = String (matkar);
	matkaVal.toCharArray(Matka, 7);
	TFTscreen.stroke(255, 255, 255);
	TFTscreen.text(Matka, 0, 60);
	matkaold=matkar;
	TFTscreen.setTextSize(1);
	huippu = kmh;
	hUippu = String (huippu);
	hUippu.toCharArray(Huippu, 6);
	TFTscreen.stroke(1000, 1000, 1000);
	TFTscreen.text(Huippu, 130, 0);
	TFTscreen.text("Huippu", 85, 0);
	TFTscreen.setTextSize(2);
	TFTscreen.text(Matka, 0, 60);
	TFTscreen.setTextSize(4);
	TFTscreen.stroke(255, 255, 255);
	TFTscreen.text(sensorPrintout, 0, 20);
	oldVal = sensorVal;					
	oldVal.toCharArray(oldsensor, 6);
	if(Case==1){
	if(mph == 1)	
	{
		TFTscreen.setTextSize(1);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text("km TRIP", 60, 65);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text("miles TRIP", 60, 65);
	}
	else{
		TFTscreen.setTextSize(1);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text("miles TRIP", 60, 65);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text("km TRIP", 60, 65);
	}
	}
	//delay(200);
}

//Sleep mode if now input in 35s.
void sleepNow(void)											
{
	set_sleep_mode(SLEEP_MODE_EXT_STANDBY);		//set sleepmode.
	digitalWrite(7,LOW);						//Turn screen off.
	power_timer2_disable();						//Timer2 shutdown.
	sleep_enable();
	sleep_mode();								//Goes to sleep here.
	sleep_disable();							//Returns from sleep.
	delay(50);
	noInterrupts();
	pinMode(7,OUTPUT);
	digitalWrite(7,HIGH);						//Screen on.
	TFTscreen.begin();							//Screen setup.
	tftSetup();									//Draws first case.
	interrupts();
	delay(200);
	power_timer2_enable();						//timer2 enable.
}

//Reset function for trip, time and highspeed.
void reset()									
{
	secondsoff = millis() / 1000;
	matkaoff = matka;
	huippu=0;
	TFTscreen.setTextSize(1);
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.text(Huippu, 130, 0); 
	huippu = 0;
	hUippu = String (huippu);
	hUippu.toCharArray(Huippu, 6);
	TFTscreen.stroke(1000, 1000, 1000);
	TFTscreen.text(Huippu, 130, 0);
}
//Resets speed if not moving.
void nollaus()											
{
	vert1=kmh;
	if(vert1==vert2){
		kmh=0;	
		sensorVal = String(kmh);
		sleepFlag+=1;
	}
	vert2=kmh;
} 
//Screen setup for settings menu.
void tftsetuptup(){
	TFTscreen.background(0,0,0);
	if(mph==1){
		TFTscreen.setTextSize(2);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text("KM/h", 10, 10);	
		TFTscreen.stroke(1000, 1000, 1000);
		TFTscreen.text("MP/h", 10, 10);	
	}		
	if(mph==0){
		TFTscreen.setTextSize(2);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text("MP/h", 10, 10);	
		TFTscreen.stroke(1000, 1000, 1000);
		TFTscreen.text("KM/h", 10, 10);	
	}	
	Stk = String (tuumakoko);
	Stk.toCharArray(stk, 5);
	TFTscreen.stroke(255, 255, 255);
	TFTscreen.text(stk, 110, 10);				
	TFTscreen.text("Exit", 10, 105);
	TFTscreen.text("Reset", 10, 55);
	TFTscreen.text("Set", 100, 55);	
}
//function for setting custom total distance.
//Raises char arrays values one at a time.
void plus(int a){
	int h=1;
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.text(VAL, 10, 40);
	VAL[a]+=1;
	if(VAL[a]<45||VAL[a]>57&&h==1){
		VAL[a]=46;
		h=0;
	}	
	if(VAL[a]==47){
		VAL[a]=48;
	}
	TFTscreen.stroke(255, 255, 255);
	TFTscreen.text(VAL, 10, 40);
}
void loop() 
{
	//If sensor interrupt detected then delayflag is set and waits here 80ms to avoid multiple interrupts.
	if(delayflag==1){							
		delay(80);
		EIMSK |= (0 << INT0);  //resets interrupt flag.
		interrupts();
		delayflag=0;
	}
	//Keeps trip distance updated.
	matkar=matka-matkaoff;
	
	//Resets variables for button press.
	del=0;
	buttonflag=0;
	exitflag=0;
	
	//Sets wheel size on scale if off scale.
	if(tuumakoko<20||tuumakoko>30){
		tuumakoko=20;
	}
	
	//Starts timing when button pressed.
	while(digitalRead(5) == HIGH)					
	{
		del=del+1;
		delay(1);
		buttonflag=1;
	}
	
	//Short press moves to next case(screen).
	if(del<500 && buttonflag==1){				
		Case+=1;
		screenFlag=1;
		if(Case>4){
			Case=1;
		}
	}
	//Medium long press reset function.
	if(del>500 && buttonflag==1 && del<5000){	
		reset();
	}
	
	//Long press goes to setup.
	if(del>3500 && buttonflag==1){				
		del=0;
		buttonflag=0;
		tftsetuptup();
		
		//Starts loop where setup runs
		for(;exitflag!=1;){
			del=0;
			buttonflag=0;
		
			//Timer for button press.
			while(digitalRead(5) == HIGH){
				del+=1;
				buttonflag=1;
				delay(1);
			}
				//Short press moves to next spot.
				if(del<500 && buttonflag==1){				
					Casesetup+=1;
					if(Casesetup==6){
						Casesetup=1;
					}
				}
				
				//Long press changes variable or exits setup.
				if(del>500 && buttonflag==1){				
					switch(Casesetup){
						
						//case 1: Select units.
						case 1:										
							if(mph==0){
								mph=1;	
							}
							else{
								mph=0;
							}
							if(mph==1){
								TFTscreen.setTextSize(2);
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.text("KM/h", 10, 10);	
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.text("MP/h", 10, 10);	
							}		
							if(mph==0){
								TFTscreen.setTextSize(2);
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.text("MP/h", 10, 10);	
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.text("KM/h", 10, 10);	
							}
							break;
							
						//case 2: Select whell size.
						case 2:									
						oldtuumakoko=tuumakoko;
						tuumakoko+=2;
						if(tuumakoko>30){
							tuumakoko=20;
						}
						Sotk = String (oldtuumakoko);
						Sotk.toCharArray(sotk, 5);
						TFTscreen.setTextSize(2);
						TFTscreen.stroke(0, 0, 0);
						TFTscreen.text(sotk, 110, 10);	
						Stk = String (tuumakoko);
						Stk.toCharArray(stk, 5);
						TFTscreen.stroke(255, 255, 255);
						TFTscreen.text(stk, 110, 10);
						break;
						
						//case 3: Reset total distance.
						case 3:									
						for (int i=100 ;i<120;){
							EEPROM.write(i, 0);
							i++;
						}
						ekierrokset=0;
						ematka = ekierrokset*(tuumakoko*2.54*3.1459)/100000;
						break;
						
						//case 4: Set total distance.
						case 4:												
						VAK = String (ematka);			
						VAK.toCharArray(VAL, 7);
						TFTscreen.background(0,0,0);
						TFTscreen.stroke(255, 255, 255);
						TFTscreen.text(VAL, 10, 40);
						TFTscreen.text("Exit", 102, 40);
						cas=1;
						for(int x=0;x!=1;){
							del=0;
							buttonflag=0;
							while(digitalRead(5) == HIGH)	
							{
								del+=1;
								buttonflag=1;
								delay(1);
							}
								if(del<500 && buttonflag==1){	
									cas+=1;	
									if(cas==8){
										cas=1;
									}
								}
								if(del>500 && buttonflag==1){
									
								//nubmer selected by case is raised by 1.
								switch(cas){
									
								case 1:
								plus(0);
								break;
								
								case 2:
								plus(1);
								break;
								
								case 3:
								plus(2);
								break;
								
								case 4:
								plus(3);
								break;
								
								case 5:
								plus(4);
								break;
								
								case 6:
								plus(5);
								break;
								
								case 7:
								x=1;
								break;
								}
								}
								
								//Move pointer according to case.
								if(cas==1){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(102, 55, 152, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(10, 55, 20, 55);	
								}
								if(cas==2){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(10, 55, 20, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(22, 55, 32, 55);	
								}
								if(cas==3){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(22, 55, 32, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(34, 55, 44, 55);	
								}
								if(cas==4){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(34, 55, 44, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(46, 55, 56, 55);	
								}
								if(cas==5){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(46, 55, 56, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(58, 55, 68, 55);	
								}
								if(cas==6){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(58, 55, 68, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(70, 55, 80, 55);	
								}
								if(cas==7){
									TFTscreen.stroke(0, 0, 0);
									TFTscreen.line(70, 55, 80, 55);	
									TFTscreen.stroke(1000, 1000, 1000);
									TFTscreen.line(102, 55, 152, 55);	
								}
						}
						temp = atof(VAL);
						ekierrokset=temp*100000/(tuumakoko*2.54*3.1459);
						ematka = ekierrokset*(tuumakoko*2.54*3.1459)/100000;
						EEPROM.put(160, ekierrokset);
						matkat=ematka;
						tftsetuptup();
						break;
						
						//case 5: Exits and saves variables to eeprom.
						case 5:									
						EEPROM.put(120, mph);
						EEPROM.put(140, tuumakoko);
						TFTscreen.setTextSize(2);
						TFTscreen.stroke(0, 0, 0);
						TFTscreen.text(stk, 110, 10);
						TFTscreen.text("MP/h", 10, 10);
						TFTscreen.text("KM/h", 10, 10);
						TFTscreen.text("Exit", 10, 105);
						TFTscreen.text("Reset", 10, 55);
						TFTscreen.text("Set", 100, 55);
						TFTscreen.line(10, 120, 58, 120);	
						TFTscreen.stroke(1000, 1000, 1000);
						TFTscreen.text("saved", 40, 50);
						delay(1500);
						TFTscreen.stroke(0, 0, 0);
						TFTscreen.text("saved", 40, 50);
						Casesetup=1;				
						exitflag=1;
						skip=1;
						screenFlag=1;
						TFTscreen.text("exit", 50, 105);
						tftSetup();
						sleepFlag=0;
						break;
						}
						}
						
						//Is run if not exiting setup. Moves pointer.
						if(skip==0){
							if(Casesetup==1){
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.line(10, 120, 58, 120);
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.line(10, 25, 58, 25);
							}
							if(Casesetup==2){
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.line(10, 25, 58, 25);
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.line(110, 25, 134, 25);
							}
							if(Casesetup==3){
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.line(110, 25, 134, 25);
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.line(10, 70, 70, 70);	
							}							
							if(Casesetup==4){
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.line(10, 70, 70, 70);
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.line(100, 70, 136, 70);	
							}							
							if(Casesetup==5){
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.line(100, 70, 136, 70);
								TFTscreen.stroke(1000, 1000, 1000);
								TFTscreen.line(10, 120, 58, 120);							
							}
			
					}
					
					//Is run when exiting setup. Clears screen.
					if(skip==1){								
						if(mph==1){
							TFTscreen.setTextSize(1);
							TFTscreen.stroke(0, 0, 0);
							TFTscreen.text("KM/h", 130, 41);
							TFTscreen.stroke(1000, 1000, 1000);
							TFTscreen.text("MP/h", 130, 41);
						}
						if(mph==0){	
							TFTscreen.setTextSize(1);
							TFTscreen.stroke(0, 0, 0);
							TFTscreen.text("MP/h", 130, 41);
							TFTscreen.stroke(1000, 1000, 1000);
							TFTscreen.text("KM/h", 130, 41);
						}
						if(Case==3){
							if(mph==1){
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.text("Km TOTAL", 75, 65);
								TFTscreen.stroke(255, 255, 255);
								TFTscreen.text("Miles TOTAL", 75, 65);
							}
							else{
								TFTscreen.stroke(0, 0, 0);
								TFTscreen.text("Miles TOTAL", 75, 65);	
								TFTscreen.stroke(255, 255, 255);
								TFTscreen.text("Km TOTAL", 75, 65);
							}
						}
					}
				skip=0;	
			}	
	}
	
	//Counts training length in seconds.
	seconds = (millis() / 1000)-secondsoff;  	
	
	//Gets time realtime from RTC.
	DateTime now = rtc.now();							

	//If speed Changed update speed.
	if(sensorVal != oldVal){							
		noInterrupts();
		sensorVal.toCharArray(sensorPrintout, 6);
		oldVal = sensorVal;
		interrupts();
		TFTscreen.setTextSize(4);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text(oldsensor, 0, 20);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text(sensorPrintout, 0, 20);
		oldVal.toCharArray(oldsensor, 6);
		sleepFlag=0;
		
			//Draws selected unit.
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
	
	//Update topspeed if higher than previous topspeed.
	if(huippu < kmh && kmh<120)							
	{
		TFTscreen.setTextSize(1);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text(Huippu, 130, 0); 
		huippu = kmh;
		hUippu = String (huippu);
		hUippu.toCharArray(Huippu, 6);
		TFTscreen.stroke(1000, 1000, 1000);
		TFTscreen.text(Huippu, 130, 0);
		TFTscreen.text("Huippu", 85, 0);
	} 
	
	//Save total distance to eeprom every 200 meters.
	if(roundf((matkat+0.2) * 100) < roundf(ematka * 100)){ 
		EEPROM.put(160, ekierrokset);
		matkat=ematka;
	}
	
	//Switch case to select shown screen.
	switch (Case){ 		
	
	//case 1: trip distance
	case 1:													
	
	//Screenflag sets if moving between screens and sets screen for next screen.
	if(screenFlag == 1){
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.setTextSize(1);
		TFTscreen.text("Km TOTAL", 75, 65);
		TFTscreen.text("Miles TOTAL", 75, 65);
		TFTscreen.setTextSize(2);
		TFTscreen.text(MatkaT, 0, 60);
		TFTscreen.text(RTC, 0, 60);
		TFTscreen.stroke(255, 255, 255);
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
	
	//Update distance if risen.
	if(roundf(matkar * 100) / 1 != roundf(matkaold * 100) / 1)	
	{
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.setTextSize(2);
		TFTscreen.text(Matka, 0, 60);
		matkaVal = String (matkar);
		matkaVal.toCharArray(Matka, 7);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text(Matka, 0, 60);
		matkaold=matkar;
	}
	break;
	
	//case 2: Training timer.
	case 2:						
	
	//Screenflag sets if moving between screens and sets screen for next screen.
	if(screenFlag == 1)											
	{		
		if(mph == 1)	
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
		TFTscreen.text(tunnit, 0, 60);
		screenFlag = 0;
	}
	
	//Update time if changed.
	if(seconds!=oldseconds){					
		oldseconds=seconds;
		t = seconds;				
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
		
		//unsigned long = string
		ultoa(s,secc,10);							
		ultoa(m,mnc,10);
		ultoa(h,hrc,10);
		
		//Transform time to one string.
		strcpy(tunnit, hrc);						
		strcat(tunnit, ":");
		strcpy(minuutit, mnc);
		strcat(minuutit, ":");
		strcat(tunnit, minuutit);
		strcpy(sekunnit, secc);
		strcat(tunnit, sekunnit);

		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text(tunnit, 0, 60);
	}
	break;
	
	//case 3: Total distancce since reset or device setup.
	case 3:			
	
	//Screenflag sets if moving between screens and sets screen for next screen.
	if(screenFlag == 1){	
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.setTextSize(2);
		TFTscreen.text(tunnit, 0, 60);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text(MatkaT, 0, 60);
		TFTscreen.setTextSize(1);
	if(mph==1){
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text("Km TOTAL", 75, 65);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text("Miles TOTAL", 75, 65);
	}
	else{
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text("Miles TOTAL", 75, 65);	
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text("Km TOTAL", 75, 65);
	}
	screenFlag = 0;
	}
	
	if(roundf(matkav * 100) != roundf(ematka * 100)){
		TFTscreen.setTextSize(2);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text(Matka, 0, 60);
		TFTscreen.text(tunnit, 0, 60);
		TFTscreen.text(MatkaT, 0, 60);
		matkav = ematka;
		matkaT = String (ematka);
		matkaT.toCharArray(MatkaT, 7);
		TFTscreen.stroke(255, 255, 255);
		TFTscreen.text(MatkaT, 0, 60);
		}
	if(mph==1){
		TFTscreen.setTextSize(1);
		TFTscreen.text("Miles TOTAL", 75, 65);
	}
	
	else{
		TFTscreen.setTextSize(1);
		TFTscreen.text("Km TOTAL", 75, 65);
	}
	break;
	
	//case 4: Realtime clock.
	case 4:								
	if(now.second() != oldS){
		oldS = now.second();
		if(mph==1){
			TFTscreen.setTextSize(1);
			TFTscreen.stroke(0, 0, 0);
			TFTscreen.text("Miles TOTAL", 75, 65);
		}
		
		else{
			TFTscreen.setTextSize(1);
			TFTscreen.stroke(0, 0, 0);
			TFTscreen.text("Km TOTAL", 75, 65);
		}
		TFTscreen.setTextSize(2);
		TFTscreen.stroke(0, 0, 0);
		TFTscreen.text(Matka, 0, 60);
		TFTscreen.text(tunnit, 0, 60);
		TFTscreen.text(MatkaT, 0, 60);
		TFTscreen.text(tunniT, 0, 60);

		itoa(now.hour(), H, 10);
		itoa(now.minute(), M, 10);
		itoa(now.second(), S, 10);
		
		strcpy(tunniT, H);
		strcat(tunniT, ":");
		strcpy(minuutiT, M);
		strcat(minuutiT, ":");
		strcat(tunniT, minuutiT);
		strcpy(sekunniT, S);
		strcat(tunniT, sekunniT);
		strcpy(RTC, tunniT);
		TFTscreen.setTextSize(2);
		TFTscreen.stroke(1000, 1000, 1000);
		TFTscreen.text(RTC, 0, 60);
	}
	break;
	}
	
	//Checks sleep flag.
	if(sleepFlag>=2)							
	{
		sleepFlag=0;
		sleepNow();
	}
	}
