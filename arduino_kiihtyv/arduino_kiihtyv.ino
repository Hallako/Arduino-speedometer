const int xpin = A1;                
const int ypin = A2;                  
const int zpin = A3;                 
void setup()
{
 Serial.begin(9600);
 pinMode(xpin, INPUT);
 pinMode(ypin, INPUT);
 pinMode(zpin, INPUT);
}
void loop()
{
 float x = analogRead(xpin);
   delay(1);
 float y = analogRead(ypin);
   delay(1);
 float z = analogRead(zpin);
 x=0.088*x-44.34;
 y=0.0876*y-44.671;
 z=0.0971*z-49.27;

 
 Serial.print((float)x);
 Serial.print("\t");
 
 Serial.print((float)y);
 Serial.print("\t");
 
 Serial.print((float)z);
 Serial.print("\t");

 Serial.print(atan2(x,z)*57.2958);
 Serial.print("\t");

 Serial.print(atan2(y,z)*57.2958);
 Serial.print("\t");


 Serial.print((asin(x/9.81))*180/3.14159);
 Serial.print("\t");
 
 Serial.print((asin(y/9.81))*180/3.14159);
 Serial.print("\n");




 delay(50);
}
