#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <EEPROM.h>
#include <MsTimer2.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define cs   10
#define dc   9
#define rst  8

TFT TFTscreen = TFT(cs, dc, rst);
unsigned long t,s,m,h,seconds,secondsoff,oldseconds;
int sensorPin = 5,sleepFlag, v=1,del=0,mph,fi,tk,otk,f,kierrokset = 0,ekierrokset,ca=1,screenFlag=0,ex=0,skip=0,sleepflag;
float start, matka = 0, revs, elapsed,ematka, time,matkat,matkar,matkaoff,matkaold,kmh,huippu=0,matkav,vert1,vert2;
char oldsensor[6], Matka[6], sensorPrintout[6], secc[4], mnc[4], hrc[4],MatkaT[6],Huippu[6]={0},
	sotk[5],stk[5],minuutit[10], sekunnit[10], tunnit[10];
String oldVal,sensorVal,matkaVal,sec,minn,hou,matkaT,hUippu,Sotk,Stk;
char ;
int muuttuja;

void setup() {
	TFTscreen.begin();										//Alustetaan näyttö.
	tftSetup();
	Serial.begin(9600);
	start=millis();
	ekierrokset = sizeof(int);								//asetetaan muuttujille koot.
	mph = sizeof(int);
	tk = sizeof(int);	
	EEPROM.get(120, mph);									//Haetaan eepromista arvot matkalle ja muuttujille.
	EEPROM.get(140, tk);
	EEPROM.get(100, ekierrokset);
	ematka = ekierrokset * (tk*2.54*3.1459)/100000;
	matkat=ematka;
	MsTimer2::set(4000, nollaus); 							//Alustetaan timer nollaukselle.
	MsTimer2::start();
	screenFlag=1;
	attachInterrupt(digitalPinToInterrupt(2), trig, FALLING);		//Alustetaan keskeytykset.
	attachInterrupt(digitalPinToInterrupt(3), wakeUp, RISING);
	pinMode(2,INPUT);												//alustetaan pinnit.
	pinMode(3,INPUT);
}

void trig() {											//keskeytysfunktio joka laskee matkan ja nopeuden.
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
	if(mph==1)												//Muunnos maileiksi mikäli valittuna.
	{
    kmh = kmh * 0.621371;
    matka = matka * 0.621371;
	}
	sensorVal = String(kmh);
}
void wakeUp()
{
	sleepflag=0;
}
void sleepNow(void)										//Nukkumis funktio mikäli ei havaittu syöttöä 40 sek.
{
	sleepFlag=0;
	delay(100);
    set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
	power_timer2_disable();
    sleep_enable();
    sleep_mode();					//menee uneen.
	sleep_disable();				//poistuu unesta.
	power_timer2_enable();
}
void reset()						//reset funktio tripille ja ajastimelle.
{
	secondsoff = millis() / 1000;
	matkaoff = matka;
	huippu=0;
}

void nollaus()						//Nolla nopeuden kun ei liikuta.
{
	vert1=kmh;
	if(vert1==vert2){
	kmh=0;	
	sensorVal = String(kmh);
	sleepFlag+=1;
	}
	vert2=kmh;
} 
void loop() 
{
	if(fi==1){							//Mikäli keskeytys suoritettu niin delayn jälkeen nollataan keskeytykset ja sallittaan uudelleen.
	delay(80);
	EIMSK |= (0 << INT0);
	interrupts();
	fi=0;
	}
	matkar=matka-matkaoff;
	del=0;
	f = 0;
  
  while(digitalRead(5) == HIGH)			//Mikäli painike painettu niin aloitetaan laskemaan napautuksen pituus.
	{
    del=del+1;
	delay(1);
    f=1;
	ex=0;
		if(del>3600){						//timeout.
		break;
		}
	}
	if(del<500 && f==1){				//lyhyt painallus = seuraava näyttö.
	v+=1;
	screenFlag=1;
		if(v>4){
		v=1;
		}
	}
	
	if(del>500 && f==1 && del<5000){	//keskipitkä painallus = nollaus.
	reset;
	}
	
	if(del>3500 && f==1){				//pitkä painallus = setup.
	delay(1000);
	del=0;
	f=0;
	
		for(;ex!=1;){
		del=0;
		f=0;
			while(digitalRead(5) == HIGH)		//Aloitetaan ajastus
			{
			del+=1;
			f=1;
			delay(1);
			}
				if(del<500 && f==1){				//lyhyt painallus seuraava kohta.
				ca+=1;
					if(ca==4){
					ca=1;
					}
				}
				if(del>500 && f==1){				//pitkä painallus muuttaa arvoa tai poistuu.
					switch(ca){
						case 1:									//case 1: mph tai kmh valinta.
							if(mph==0){
							mph=1;	
							}
							else{
							mph=0;
							}
							break;
						case 2:									//case 2:valitaan tuumakoko.
							otk=tk;
							tk+=2;
							if(tk>30){
							tk=20;
							}
							break;
						case 3:									//case 3: poistutaan ja tallennetaan eepromiin.
						EEPROM.put(120, mph);
						EEPROM.put(140, tk);
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
						
						if(skip==0){									//suoritetaan mikäli ei poistuta funktiosta
							if(mph==1){
							TFTscreen.setTextSize(2);
							TFTscreen.stroke(0, 0, 0);
							TFTscreen.text("KM/h", 0, 100);	
							TFTscreen.setTextSize(2);
							TFTscreen.stroke(1000, 1000, 1000);
							TFTscreen.text("MP/h", 0, 100);			
							}		
							if(mph==0){
							TFTscreen.setTextSize(2);
							TFTscreen.stroke(0, 0, 0);
							TFTscreen.text("MP/h", 0, 100);	
							TFTscreen.setTextSize(2);
							TFTscreen.stroke(1000, 1000, 1000);
							TFTscreen.text("KM/h", 0, 100);				
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
					
					if(skip==1){									//suoritetaan kun poistutaan funktiosta
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
	seconds = (millis() / 1000)-secondsoff;  			//secondsoff = offset resetistä.
  
	if(sensorVal != oldVal)							//päivitetään nopeus mikäli muuttunut.
	{							
	noInterrupts();
	sensorVal.toCharArray(sensorPrintout, 6);
	TFTscreen.setTextSize(4);
	TFTscreen.stroke(0, 0, 0);
	TFTscreen.text(oldsensor, 0, 20);
	TFTscreen.stroke(255, 255, 255);
	TFTscreen.text(sensorPrintout, 0, 20);
	oldVal = sensorVal;								//ottaa vanhan arvon talteen näytön tyhjennystä varten
	oldVal.toCharArray(oldsensor, 6);
	
	if(mph == 1)										//piirtää mailit mikäli valittu muuten kmh
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
  
	if(huippu < kmh)									//huippu arvon päivitys
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
	matkat=ematka;
	}
	switch (v){ 										//näyttö moodin valinta
	case 1:												//case 1: trippi mittari
	
	if(screenFlag == 1)									//screenflag asettuu mikäli näyttö tilaa muutetaan ja se alustaa näytön moodille
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
	
	if(roundf(matkar * 100) / 1 != roundf(matkaold * 100) / 1)	//matkan päivitys mikäli muuttunut.
	{
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
	break;
	
	case 2:														//case 2: harjoituksen kesto ajastin
	
	if(screenFlag == 1)											//screenflag asettuu mikäli näyttö tilaa muutetaan ja se alustaa näytön moodille
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
		TFTscreen.setTextSize(2);
		TFTscreen.text(tunnit, 0, 60);
		screenFlag = 0;
	}
	
	if(seconds!=oldseconds){				//ajan päivitys kun se muuttuu.
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
  
	ultoa(s,secc,10);						//unsigned long = string
	ultoa(m,mnc,10);
	ultoa(h,hrc,10);
	
	strcpy(tunnit, hrc);					//aika muunnetaan yhdeksi stringiksi
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
	case 3:									//case 3: kokonais matka mitä ajettu alustuksesta lähtien
	
    if(screenFlag == 1)						//screenflag asettuu mikäli näyttö tilaa muutetaan ja se alustaa näytön moodille
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
	break;
	
	case 4:									//case 4: näyttää reali aikaisen kellon
	Serial.println("CASE4");
	delay(1000);
	break;
	}
	
	if(sleepFlag==10)							//nukkumaan meno muuttujan tarkistus
	{
	sleepflag=0;
	sleepNow();
	}
	}
	
	void tftSetup()							//näytön alustus funktio
	{
	TFTscreen.background(0,0,0);
	TFTscreen.stroke(1000, 200, 200);
	TFTscreen.setTextSize(2);
	TFTscreen.text("SIKMA ", 0, 0);
	TFTscreen.line(0, 55, 160, 55);    
	}