/*
 Outputs power through sensor to adafruit display
 */

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

#define BLUE 0x4
#define OFF 0x0

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

unsigned long duration = 0;
float avgAmps = 0;
float maxAmps = 0;
float sensorZeroValue = 0;
unsigned long starttime; 

// the setup routine runs once when you press reset:
void setup() {        
  lcd.begin(16, 2);
  lcd.print("Open circuit to");
  lcd.setCursor(0,1);
  lcd.print("sensor. Push sel");
  Serial.begin(9600);
  Serial.print("waiting for user to zero sensor");
  
// loop until button select is pressed
  waitForButton(BUTTON_SELECT);  
  lcd.clear();
  lcd.print("Zeroing Sensor..");
  zero();    
  // turn off LED
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  lcd.clear();
  lcd.print("zero:");
  lcd.print(sensorZeroValue);
  lcd.setCursor(9,0);
  lcd.setCursor(0,1);  
  lcd.print("push > to start");
  waitForButton(BUTTON_RIGHT);
  lcd.clear();
  lcd.setBacklight(OFF);
  lcd.print("Running...");

  starttime = millis();
}

void waitForButton(uint8_t button) {
  for (uint8_t buttons = lcd.readButtons();!(buttons & button);buttons = lcd.readButtons()) {}
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
  lcd.setCursor(0,0);
  lcd.print(amps, 3);
  lcd.setCursor(5,0);
  lcd.print("A");
  lcd.print(" T:");
  long seconds = (millis() - starttime)/1000;
  // less than 2 minutes
  if (seconds < 120) {
    lcd.print(seconds);
    lcd.print("s");
  // less than 2 hours
  } else if (seconds < 7200) {
    lcd.print((float)seconds/60);
    lcd.print("m");
  // less than 3 days
  } else if (seconds < 259200) {
    lcd.print((float)seconds/3600);
    lcd.print("h");
  } else {
    lcd.print((float)seconds/86400);
    lcd.print("d");
  }
  
  float ampHours = avgAmps * seconds / 3600;
  
  lcd.setCursor(0,1);
  lcd.print(ampHours,4);
  lcd.setCursor(6,1);
  lcd.print("AH");

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
