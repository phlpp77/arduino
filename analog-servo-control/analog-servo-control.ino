#include <Servo.h> // includes the servo library

#define sensorPin A0
#define ledPin 11
int sensorValue = 0;

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(sensorPin);
  analogWrite(ledPin, sensorValue - 800);
  Serial.print("Sensor value: ");
  Serial.println(sensorValue);
  // put your main code here, to run repeatedly:
  delay(2);

}
