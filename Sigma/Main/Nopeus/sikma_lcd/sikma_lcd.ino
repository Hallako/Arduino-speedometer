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
	//pinnien alustus.
	pinMode(7,OUTPUT);
	digitalWrite(7,HIGH);
	
	//Alustetaan näyttö.
	TFTscreen.begin();									
	tftSetup();
	
	//asetetaan muuttujille koot.
	ekierrokset = sizeof(unsigned long);							
	mph = sizeof(int);
	tuumakoko = sizeof(int);	
	
	//Haetaan eepromista arvot matkalle ja muuttujille.
	EEPROM.get(120, mph);											
	EEPROM.get(140, tuumakoko);
	EEPROM.get(160, ekierrokset);
	ematka = ekierrokset * (tuumakoko*2.54*3.1459)/100000;
	matkat=ematka;
	
	//Alustetaan timer nollaukselle.
	MsTimer2::set(4000, nollaus); 
	MsTimer2::start();
	screenFlag=1;
	start=millis();
	
	//Asetetaan keskeytyksen.
	attachInterrupt(digitalPinToInterrupt(2), trig, FALLING);		
	attachInterrupt(digitalPinToInterrupt(3), wakeUp, RISING);		
	
 if (! rtc.begin()) {
    while (1);
  }

  if (! rtc.isrunning()) {
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 			//uncomment to set time to rtc.
  }
}

//keskeytysfunktio joka laskee matkan ja nopeuden.
void trig() {														
	noInterrupts();
	//asetetaan delayflag joka odottaa loopissa 80ms ja sallii keskeytykset uudelleen.
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
	
	//muunnos maileiksi mikäli valittuna.
	if(mph==1)										
	{
		kmh = kmh * 0.621371;
		matka = matka * 0.621371;
	}
	sensorVal = String(kmh);
}

//painikkeen painallus nollaa uniajastimen
void wakeUp()				
{
	sleepFlag=0;
}

//Näytön alustus funktio jossa näytölle tulee normaali tila.
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
}

//Nukkumis funktio mikäli ei havaittu syöttöä 40 sek.
void sleepNow(void)											
{
	set_sleep_mode(SLEEP_MODE_EXT_STANDBY);		//asetetaan sleepmode 
	digitalWrite(7,LOW);						//näyttö kiinni
	power_timer2_disable();						//timer2 sammutus
	sleep_enable();
	sleep_mode();								//menee uneen.
	sleep_disable();							//poistuu unesta.
	digitalWrite(7,HIGH);						//näyttö päälle
	TFTscreen.begin();							//tftn alustus
	delay(100);
	tftSetup();									//piirretään alku näyttö
	power_timer2_enable();						//timer2 päälle
}

//reset funktio tripille ja ajastimelle.
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
//Nollaa nopeuden kun ei liikuta.
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
//Näytön alustus funktio jossa näytölle tulee asetustila.
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
	//Mikäli keskeytys suoritettu niin delayn jälkeen nollataan keskeytykset ja sallittaan uudelleen.
	if(delayflag==1){							
		delay(80);
		EIMSK |= (0 << INT0);
		interrupts();
		delayflag=0;
	}
	//päivittää trippi matkaa.
	matkar=matka-matkaoff;
	
	//nollataan muuttujat painalluksille.
	del=0;
	buttonflag=0;
	exitflag=0;
	
	//asettaa tuumakoon vakioon mikäli laiton.
	if(tuumakoko<20||tuumakoko>30){
		tuumakoko=20;
	}
	
	//Aloittaa ajastuksen painalluksen pituudelle.
	while(digitalRead(5) == HIGH)					
	{
		del=del+1;
		delay(1);
		buttonflag=1;
	}
	
	//lyhyt painallus = seuraava näyttö.
	if(del<500 && buttonflag==1){				
		Case+=1;
		screenFlag=1;
		if(Case>4){
			Case=1;
		}
	}
	//keskipitkä painallus = nollaus.
	if(del>500 && buttonflag==1 && del<5000){	
		reset();
	}
	
	//pitkä painallus = setup.
	if(del>3500 && buttonflag==1){				
		del=0;
		buttonflag=0;
		tftsetuptup();
		
		//aloitetaan loop jossa setup pyörii
		for(;exitflag!=1;){
			del=0;
			buttonflag=0;
		
			//Aloitetaan ajastus
			while(digitalRead(5) == HIGH){
				del+=1;
				buttonflag=1;
				delay(1);
			}
				//lyhyt painallus seuraava kohta.
				if(del<500 && buttonflag==1){				
					Casesetup+=1;
					if(Casesetup==6){
						Casesetup=1;
					}
				}
				
				//pitkä painallus muuttaa arvoa tai poistuu.
				if(del>500 && buttonflag==1){				
					switch(Casesetup){
						
						//case 1: mph tai kmh valinta.
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
							
						//case 2:valitaan tuumakoko.
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
						
						//case 3: nollataan matka.
						case 3:									
						for (int i=100 ;i<120;){
							EEPROM.write(i, 0);
							i++;
						}
						ekierrokset=0;
						ematka = ekierrokset*(tuumakoko*2.54*3.1459)/100000;
						break;
						
						//case 4: asetetaan alku arvo
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
									
								//casen mukaan valittua numeroa kasvatetaan.
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
								
								//siirretään osoitinta eteenpäin casen mukaan.
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
						
						//case 5: poistutaan ja tallennetaan muuttujat eepromiin.
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
						break;
						}
						}
						
						//suoritetaan mikäli ei poistuta setupista
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
					
					//suoritetaan kun poistutaan funktiosta
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
	
	//lasketaan trippi matka 
	seconds = (millis() / 1000)-secondsoff;  	
	
	//RTC alustus
	DateTime now = rtc.now();							

	//päivitetään nopeus mikäli muuttunut.
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
		
			//piirtää mailit mikäli valittu muuten kmh
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
	
	//huippu arvon päivitys
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
	
	//tallennetaan arvo eepromiin 200m välein
	if(roundf((matkat+0.2) * 100) < roundf(ematka * 100)){ 
		EEPROM.put(160, ekierrokset);
		matkat=ematka;
	}
	
	//näyttö moodin valinta
	switch (Case){ 		
	
	//case 1: trippi mittari	
	case 1:													
	
	//screenflag asettuu mikäli näyttö tilaa muutetaan ja se alustaa näytön moodille mihin siirrytään
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
	
	//matkan päivitys mikäli muuttunut.
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
	
	//case 2: harjoituksen kesto ajastin
	case 2:						
	
	//screenflag asettuu mikäli näyttö tilaa muutetaan ja se alustaa näytön moodille
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
	
	//ajan päivitys kun se muuttuu.
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
		
		//aika muunnetaan yhdeksi stringiksi
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
	
	//case 3: kokonais matka mitä ajettu alustuksesta lähtien
	case 3:			
	
	//screenflag asettuu mikäli näyttö tilaa muutetaan ja se alustaa näytön moodille mihin siirrytään
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
	
	//case 4: näyttää reali aikaisen kellon
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
	
	//nukkumaan meno muuttujan tarkistus
	if(sleepFlag==10)							
	{
		sleepFlag=0;
		sleepNow();
	}
	}
