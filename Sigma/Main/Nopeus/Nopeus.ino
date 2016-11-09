
int sensorPin = 2;
float start;
float elapsed, time;
float revs;
float revolution;

void setup() 
{
  Serial.begin(9600);
  attachInterrupt(0, RPM, RISING);
  start=millis();
}

void RPM()
{
  int sensorValue = digitalRead(sensorPin);
  float revs = 60000/elapsed;
  float revolution = elapsed/1000;
  float kmh = ((3.1459*55.88)*revs*60/100000);
  elapsed=millis()-start;
  start=millis();

  Serial.print(elapsed);
  Serial.print(" mS ");
  Serial.print(revolution);
  Serial.print("SEC ");
  Serial.print(revs);
  Serial.print("RPM ");
  Serial.print(kmh);
  Serial.println("KMH ");
}

void loop() 
{
  
}
