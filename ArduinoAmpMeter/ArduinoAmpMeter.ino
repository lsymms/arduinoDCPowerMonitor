/*
 Outputs power through sensor to console
 */
 
unsigned long duration = 0;
float avgAmps = 0;
float maxAmps = 0;
float sensorZeroValue = 0;
unsigned long starttime; 

// the setup routine runs once when you press reset:
void setup() {        
  // turn off LED
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);  
  zero();    
  starttime = millis();
}

// the loop routine runs over and over again forever:
void loop() {
  int sensorValue = 0;
  float amps = 0;
  for (int i=0; i<10; i++) {
    sensorValue += analogRead(A0);
  }
  sensorValue = sensorValue / 10;
  float valueFromZero = sensorValue - sensorZeroValue;
  amps = getAmps(valueFromZero);    
   
  if (amps > maxAmps) {
    maxAmps = amps;
  }
  
  calcAvg(amps,1,avgAmps,duration);
  Serial.print(valueFromZero);
  Serial.print(" :: ");
  Serial.print(amps, 3);
  Serial.print(" :: ");
  Serial.print(avgAmps, 3);
  Serial.print(" :: ");
  Serial.print(maxAmps, 3);
  Serial.print(" :: ");
  Serial.println(duration);
}

float getVolts(int sensorValue) {
  // 5V SensorRange / 1023 sensor values
  float volts = sensorValue * 0.00489;
  return volts;
}

float getAmps(int sensorValue) {
  // 5V SensorRange * 1000mV/V / 66mV/A / 1023 sensor values
  float amps = sensorValue * .0741;
  return amps;
}

void zero() {
  unsigned long count = 0;
  float avg = 0;
  unsigned long start = millis();
  Serial.println("Caluculating Zero Value...");
  while (count < 100) {
    int sensorValue = analogRead(A0);
    calcAvg(sensorValue,1,avg,count);
    delay(14);
  }
  sensorZeroValue = avg;
  Serial.print("Zero Value: ");
  Serial.println(sensorZeroValue);
  Serial.print(millis() - start);
  Serial.print("ms elapsed");
}

float calcAvg(float valueToAvg, unsigned long sampleSize, float &avg, unsigned long &total) {
  float deviation = valueToAvg - avg;
  total += sampleSize;
  double weightedDev = deviation * sampleSize / total;
  avg = avg + weightedDev;
}
