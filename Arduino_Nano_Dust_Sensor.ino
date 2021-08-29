#include <GP2Y1010sensor.h>
#include <Wire.h>

// Arduino pin numbers.
const int sharpLEDPin = 10;   // Arduino digital pin D5 connect to sensor LED.
const int sharpVoPin = A0;   // Arduino analog pin 0 connect to sensor Vo.
// Set the typical output voltage in Volts when there is zero dust. 
static float Voc = 0.6;
// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.5;
// Number of last N raw voltage readings
#define N 200
#define i2c_ADDR 8

union u_tag {
   float density; 
   byte density_byte[4];
} u;

GP2Y1010sensor GP2;

void printFValue(String text, float value, String units, bool isLast = false) {
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  Serial.print(units);
  if (!isLast) {
    Serial.print(", ");
  }
}

void setup() {
  Wire.begin(i2c_ADDR);
  // REQUIRED as ESP32 is not 5V tolerant, this will turn off pullup of SDA and SCL and we will 
  // rely on ESP32's pullup instead
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
  
  Wire.onRequest(requestEvent); // register event
  
  Serial.begin(9600);
  GP2.init(sharpLEDPin, sharpVoPin, K, N);
  Serial.println("Initialized");
}

void loop() {
  float mvolt;
  
  GP2.read(&u.density, &mvolt);
  printFValue("Vo", mvolt * 1000, "mV");
  printFValue("Dust Density", u.density, "ug/m3", true);
  Serial.println("");
}
void requestEvent() {
  Wire.write(u.density_byte, 4);
}
