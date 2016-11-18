#include <avr/interrupt.h>
int a,f;
void setup() {
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
 
 ADCSRB = 0;
 ACSR =
   (0 << ACD) |
   (0 << ACBG) |   
   (0 << ACO) |    
   (1 << ACI) |    
   (1 << ACIE) |   
   (0 << ACIC) |   
   (1 << ACIS1) | 
   (1 << ACIS0);   
}
void loop() {
  if(f==1){
  delay(25);
  f=0;
  a+=1;
  
  digitalWrite(5,HIGH);
  delay(25);
  ACSR = (1 << ACI);
  ACSR = (1 << ACIE);
  }      
}
ISR(ANALOG_COMP_vect) {
  digitalWrite(5,LOW);
  Serial.println("triggered"); 
  Serial.println(a); 
  f=1;
  ACSR=(0 << ACIE);
}

