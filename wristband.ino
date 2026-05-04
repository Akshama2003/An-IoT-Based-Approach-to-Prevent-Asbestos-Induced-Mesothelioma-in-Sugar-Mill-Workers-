#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  Serial.println("System Starting...");

  if (!bmp.begin()) {
    Serial.println("BMP180 not found!");
    while (1);
  }

  Serial.println("BMP180 Connected!");
}

void loop() {
  // ====== Real BMP180 Reading ======
  float pressure = bmp.readPressure(); // in Pascals

  // ====== Simulated Human Body Values ======
  float bodyTemp = 36.5 + random(-5, 5) * 0.1;   // 36.0 - 37.0 °C
  int systolic = 110 + random(-10, 10);          // ~100–120 mmHg
  int diastolic = 75 + random(-5, 5);            // ~70–80 mmHg

  // ====== Output ======
  Serial.println("------ HUMAN HEALTH (SIMULATED) ------");

  Serial.print("Body Temperature: ");
  Serial.print(bodyTemp);
  Serial.println(" °C");

  Serial.print("Blood Pressure: ");
  Serial.print(systolic);
  Serial.print("/");
  Serial.print(diastolic);
  Serial.println(" mmHg");

  Serial.println("------------------------------------");

  // Optional: also show real pressure
  Serial.print("Atmospheric Pressure (BMP180): ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.println();

  delay(2000);
}